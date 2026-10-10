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
    if len(matches) not in (1, 2, 3, 8):
        parser.error(f"Expected one coded stream, or two, three, or eight SoA and sidecar branches for {instance}, found {len(matches)}")
    return add_sizes(*(branch_sizes(branch) for branch in matches)), len(matches)


def print_row(product, sizes, reduction=""):
    uncompressed, compressed = sizes
    divisor = 1000 * event_count
    print(
        f"| {product} | {uncompressed / divisor:,.2f} | "
        f"{compressed / divisor:,.2f} | {reduction} |"
    )


rec_hit_instances = {
    "EE": "HGCEEUncalibRecHits",
    "HEfront": "HGCHEFUncalibRecHits",
    "HEback": "HGCHEBUncalibRecHits",
    "HFNose": "HGCHFNoseUncalibRecHits",
}
sizes_by_instance = {}
for instance, rec_hit_instance in rec_hit_instances.items():
    products = {
        "recHits": size_for(
            f"_hltHGCalUncalibRecHit_{rec_hit_instance}_ROUNDTRIP."
        ),
        "recHitsDecompressed": size_for(
            f"_hltHGCalUncalibRecHitDecompressed_{rec_hit_instance}_ROUNDTRIP."
        ),
    }
    if instance != "HFNose":
        products["Original"] = size_for(f"_hltHgcalDigis_{instance}_ROUNDTRIP.")
        products["SoA + sidecars"], product_count = soa_size_for(instance)
        standard = [
            branch for branch in branches if f"_hltHgcalDigisStandardSoA{instance}_" in branch.GetName()
        ]
        if standard:
            products["Standard SoA + detIds"] = add_sizes(*(branch_sizes(branch) for branch in standard))
        products["Restored"] = size_for(
            f"_hltHgcalDigisDecompressed_{instance}_ROUNDTRIP."
        )
        products["sidecar_count"] = product_count - 1
    sizes_by_instance[instance] = products

total_products = {}
for products in sizes_by_instance.values():
    for product in (
        "Original",
        "Standard SoA + detIds",
        "SoA + sidecars",
        "Restored",
        "recHits",
        "recHitsDecompressed",
    ):
        if product in products:
            total_products[product] = add_sizes(
                total_products.get(product, (0, 0)), products[product]
            )

print(f"Events: {event_count}  ")
print("1 kB = 1000 bytes\n")
for section in ("Total", "EE", "HEfront", "HEback", "HFNose"):
    products = total_products if section == "Total" else sizes_by_instance[section]
    print(f"### {section}")
    print(
        "| Product | Uncompressed (kB/event) | Compressed (kB/event) | "
        "Compressed reduction vs original |"
    )
    print("|---|---:|---:|---:|")
    original = products.get("Original")
    for product in (
        "Original",
        "Standard SoA + detIds",
        "SoA + sidecars",
        "Restored",
        "recHits",
        "recHitsDecompressed",
    ):
        if product not in products:
            continue
        reduction = ""
        label = product
        if product == "SoA + sidecars":
            sidecar_count = products.get("sidecar_count")
            if section == "Total":
                sidecar_count = sum(
                    sizes_by_instance[instance]["sidecar_count"]
                    for instance in ("EE", "HEfront", "HEback")
                )
                label = f"SoAs + {sidecar_count} sidecar{'s' if sidecar_count != 1 else ''}"
            else:
                label = f"SoA + {sidecar_count} sidecar{'s' if sidecar_count != 1 else ''}"
            if sidecar_count == 0:
                label = "Coded streams" if section == "Total" else "Coded stream"
            reduction = f"{100 * (1 - products[product][1] / original[1]):.1f}%"
            if "Standard SoA + detIds" in products:
                standard = products["Standard SoA + detIds"][1]
                reduction += f" ({100 * (1 - products[product][1] / standard):.1f}% vs standard SoA)"
        if product == "Standard SoA + detIds":
            reduction = f"{100 * (1 - products[product][1] / original[1]):.1f}%"
        print_row(label, products[product], reduction)
    print()
