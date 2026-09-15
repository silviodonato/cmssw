#!/usr/bin/env python3
"""Dump per-event sizes for split HGC compressed rec-hit members.

The input file must have been written with the rec-hit product split
into ROOT branches.  ROOT compresses baskets, not individual events, so the
per-event compressed value reported here is an estimate.  The exact measured
compressed size of each member branch is included in every row as branch_zip_bytes.
"""

import argparse
import csv
import fnmatch
import sys

import ROOT


PRODUCT_PREFIXES = {
    "compressed": (
        "HGCUncalibratedRecHitCompressedsSorted_"
        "hltHGCalUncalibRecHitCompressed_"
    ),
    "decompressed": (
        "HGCUncalibratedRecHitsSorted_"
        "hltHGCalUncalibRecHitDecompressed_"
    ),
}
# The legacy edm::SortedCollection uses .obj.obj.; the delta-ID collection
# persists its hits in hits_.
MEMBER_MARKERS = (".obj.obj.", ".hits_.")


def leaf_branches(branch):
    """Yield terminal branches below branch, including branch itself if terminal."""
    children = branch.GetListOfBranches()
    if children and children.GetEntriesFast():
        for index in range(children.GetEntriesFast()):
            yield from leaf_branches(children.At(index))
        return

    yield branch


def matches_collection(product, collection, pattern):
    """Match a short collection name or a full compressed/decompressed branch name."""
    full_branch = PRODUCT_PREFIXES[product] + collection
    return any(
        fnmatch.fnmatchcase(candidate, pattern)
        for candidate in (collection, full_branch, full_branch + ".")
    )


def find_member_branches(tree, collection_pattern="*"):
    """Return (product, collection, member, branch, leaf) tuples."""
    result = []
    top_branches = tree.GetListOfBranches()

    for index in range(top_branches.GetEntriesFast()):
        top = top_branches.At(index)
        for branch in leaf_branches(top):
            name = branch.GetName()
            marker = next((candidate for candidate in MEMBER_MARKERS if candidate in name), None)
            if marker is None:
                continue

            for product, prefix in PRODUCT_PREFIXES.items():
                if not name.startswith(prefix):
                    continue

                collection, member = name.split(marker, 1)
                collection = collection[len(prefix) :]
                if not matches_collection(product, collection, collection_pattern):
                    continue
                leaves = branch.GetListOfLeaves()
                if not leaves or leaves.GetEntriesFast() != 1:
                    continue

                result.append((product, collection, member, branch, leaves.At(0)))
                break

    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="ROOT file containing the Events tree")
    parser.add_argument(
        "-o", "--output", default="hgcal_rec_hit_member_sizes.csv", help="CSV output file"
    )
    parser.add_argument("--first-event", type=int, default=0)
    parser.add_argument("--last-event", type=int, default=None, help="Exclusive event index")
    parser.add_argument(
        "--collection",
        "--branch",
        dest="collection",
        default="*",
        help=(
            "Short collection name, full compressed/decompressed EDM branch name, "
            "or shell-style wildcard pattern (default: all products/collections)"
        ),
    )
    parser.add_argument(
        "--per-event",
        action="store_true",
        help="Write one row per event instead of summing over the selected events",
    )
    args = parser.parse_args()

    root_file = ROOT.TFile.Open(args.input)
    if not root_file or root_file.IsZombie():
        raise RuntimeError("Could not open {}".format(args.input))

    tree = root_file.Get("Events")
    if not tree:
        raise RuntimeError("Could not find Events tree in {}".format(args.input))

    members = find_member_branches(tree, args.collection)
    if not members:
        raise RuntimeError(
            "No split compressed/decompressed rec-hit member branches found for pattern "
            "'{}'. The file may contain an unsplit .obj branch, or the pattern may not match."
            .format(args.collection)
        )

    first = max(0, args.first_event)
    last = tree.GetEntries() if args.last_event is None else min(args.last_event, tree.GetEntries())
    if first >= last:
        raise ValueError("Empty event range [{}, {})".format(first, last))

    fieldnames = [
        "event",
        "product",
        "collection",
        "member",
        "type",
        "elements",
        "uncompressed_bytes",
        "compressed_bytes_estimate",
        "branch_tot_bytes",
        "branch_zip_bytes",
        "branch_compression",
    ]

    totals = {
        (product, collection, member): {
            "elements": 0,
            "uncompressed_bytes": 0,
            "compressed_bytes_estimate": 0.0,
            "branch_tot_bytes": int(branch.GetTotBytes()),
            "branch_zip_bytes": int(branch.GetZipBytes()),
            "branch_compression": (
                float(branch.GetTotBytes()) / branch.GetZipBytes() if branch.GetZipBytes() else 0.0
            ),
            "type": leaf.GetTypeName(),
        }
        for product, collection, member, branch, leaf in members
    }

    with open(args.output, "w", newline="") as output_file:
        writer = csv.DictWriter(output_file, fieldnames=fieldnames)
        writer.writeheader()

        for event in range(first, last):
            tree.GetEntry(event)
            for product, collection, member, branch, leaf in sorted(members, key=lambda item: item[:3]):
                elements = int(leaf.GetLen())
                bytes_per_element = int(leaf.GetLenType())
                uncompressed = elements * bytes_per_element
                total = totals[(product, collection, member)]
                compressed_estimate = uncompressed / total["branch_compression"] if total["branch_compression"] else 0.0

                total["elements"] += elements
                total["uncompressed_bytes"] += uncompressed
                total["compressed_bytes_estimate"] += compressed_estimate

                if args.per_event:
                    writer.writerow(
                        {
                            "event": event,
                            "product": product,
                            "collection": collection,
                            "member": member,
                            "type": total["type"],
                            "elements": elements,
                            "uncompressed_bytes": uncompressed,
                            "compressed_bytes_estimate": "{:.6f}".format(compressed_estimate),
                            "branch_tot_bytes": total["branch_tot_bytes"],
                            "branch_zip_bytes": total["branch_zip_bytes"],
                            "branch_compression": "{:.6f}".format(total["branch_compression"]),
                        }
                    )

        if not args.per_event:
            for product, collection, member in sorted(totals):
                total = totals[(product, collection, member)]
                writer.writerow(
                    {
                        "event": "ALL",
                        "product": product,
                        "collection": collection,
                        "member": member,
                        "type": total["type"],
                        "elements": total["elements"],
                        "uncompressed_bytes": total["uncompressed_bytes"],
                        "compressed_bytes_estimate": "{:.6f}".format(
                            total["compressed_bytes_estimate"]
                        ),
                        "branch_tot_bytes": total["branch_tot_bytes"],
                        "branch_zip_bytes": total["branch_zip_bytes"],
                        "branch_compression": "{:.6f}".format(total["branch_compression"]),
                    }
                )

    root_file.Close()
    print("Wrote {}".format(args.output))
    print("Members: {}".format(", ".join(
        "{}.{}.{}".format(product, collection, member)
        for product, collection, member, _, _ in members
    )))


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, ValueError) as error:
        print("error: {}".format(error), file=sys.stderr)
        sys.exit(1)
