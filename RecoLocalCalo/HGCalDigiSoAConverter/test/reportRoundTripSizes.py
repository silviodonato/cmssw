"""Print round-trip branch sizes in kB/event for a ROOT file."""

import argparse

import ROOT


def add_sizes(*sizes):
    return tuple(sum(size[index] for size in sizes) for index in (0, 1))


def branch_sizes(branch):
    return add_sizes(
        (branch.GetTotBytes(), branch.GetZipBytes()),
        *(branch_sizes(child) for child in branch.GetListOfBranches()),
    )


parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("file", help="round-trip ROOT file to inspect")
args = parser.parse_args()

root_file = ROOT.TFile.Open(args.file)
if not root_file or root_file.IsZombie():
    parser.error(f"Cannot open {args.file}")
events = root_file.Get("Events")
if not events:
    parser.error(f"No Events tree in {args.file}")

branches = list(events.GetListOfBranches())
event_count = events.GetEntries()
if event_count == 0:
    parser.error(f"No events in {args.file}")


def size_for(token):
    matches = [branch for branch in branches if token in branch.GetName()]
    if len(matches) != 1:
        parser.error(f"Expected one branch containing {token}, found {len(matches)}")
    return branch_sizes(matches[0])


def soa_size_for(instance):
    token = f"_hltHgcalDigisSoA_{instance}"
    matches = [branch for branch in branches if token in branch.GetName()]
    if len(matches) not in (3, 8):
        parser.error(f"Expected three or eight SoA and sidecar branches for {instance}, found {len(matches)}")
    return add_sizes(*(branch_sizes(branch) for branch in matches)), len(matches)


def print_row(instance, product, sizes, reduction=""):
    uncompressed, compressed = sizes
    divisor = 1000 * event_count
    print(
        f"| {instance} | {product} | {uncompressed / divisor:,.2f} | "
        f"{compressed / divisor:,.2f} | {reduction} |"
    )


totals = {"original": (0, 0), "packed": (0, 0), "decompressed": (0, 0)}
print(f"Events: {event_count}  ")
print("1 kB = 1000 bytes\n")
print("| Instance | Product | Uncompressed (kB/event) | Compressed (kB/event) | Compressed reduction vs original |")
print("|---|---|---:|---:|---:|")
for instance in ("EE", "HEfront", "HEback"):
    original = size_for(f"_hltHgcalDigis_{instance}_ROUNDTRIP.")
    packed, product_count = soa_size_for(instance)
    restored = size_for(f"_hltHgcalDigisDecompressed_{instance}_ROUNDTRIP.")
    totals["original"] = add_sizes(totals["original"], original)
    totals["packed"] = add_sizes(totals["packed"], packed)
    totals["decompressed"] = add_sizes(totals["decompressed"], restored)
    print_row(instance, "Original", original)
    print_row(instance, f"SoA + {product_count - 1} sidecars", packed, f"{100 * (1 - packed[1] / original[1]):.1f}%")
    print_row(instance, "Restored", restored)

print_row("Total", "Original", totals["original"])
print_row(
    "Total",
    "SoA + sidecars",
    totals["packed"],
    f"{100 * (1 - totals['packed'][1] / totals['original'][1]):.1f}%",
)
print_row("Total", "Restored", totals["decompressed"])
