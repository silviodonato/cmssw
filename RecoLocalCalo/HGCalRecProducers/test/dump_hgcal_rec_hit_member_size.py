#!/usr/bin/env python3
"""Dump per-event sizes for split HGC compressed rec-hit members.

The input file must have been written with the compressed rec-hit product split
into ROOT branches.  ROOT compresses baskets, not individual events, so the
per-event compressed value reported here is an estimate.  The exact measured
compressed size of each member branch is included in every row as branch_zip_bytes.
"""

import argparse
import csv
import sys

import ROOT


PRODUCT_PREFIX = (
    "HGCUncalibratedRecHitCompressedsSorted_"
    "hltHGCalUncalibRecHitCompressed_"
)
MEMBER_MARKER = ".obj.obj."


def leaf_branches(branch):
    """Yield terminal branches below branch, including branch itself if terminal."""
    children = branch.GetListOfBranches()
    if children and children.GetEntriesFast():
        for index in range(children.GetEntriesFast()):
            yield from leaf_branches(children.At(index))
        return

    yield branch


def find_member_branches(tree):
    """Return (collection, member, branch, leaf) tuples."""
    result = []
    top_branches = tree.GetListOfBranches()

    for index in range(top_branches.GetEntriesFast()):
        top = top_branches.At(index)
        for branch in leaf_branches(top):
            name = branch.GetName()
            if not name.startswith(PRODUCT_PREFIX) or MEMBER_MARKER not in name:
                continue

            collection, member = name.split(MEMBER_MARKER, 1)
            collection = collection[len(PRODUCT_PREFIX) :]
            leaves = branch.GetListOfLeaves()
            if not leaves or leaves.GetEntriesFast() != 1:
                continue

            result.append((collection, member, branch, leaves.At(0)))

    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("input", help="ROOT file containing the Events tree")
    parser.add_argument(
        "-o", "--output", default="hgcal_rec_hit_member_sizes.csv", help="CSV output file"
    )
    parser.add_argument("--first-event", type=int, default=0)
    parser.add_argument("--last-event", type=int, default=None, help="Exclusive event index")
    args = parser.parse_args()

    root_file = ROOT.TFile.Open(args.input)
    if not root_file or root_file.IsZombie():
        raise RuntimeError("Could not open {}".format(args.input))

    tree = root_file.Get("Events")
    if not tree:
        raise RuntimeError("Could not find Events tree in {}".format(args.input))

    members = find_member_branches(tree)
    if not members:
        raise RuntimeError(
            "No split compressed rec-hit member branches found. "
            "The file contains an unsplit .obj branch, or the branch was not selected."
        )

    first = max(0, args.first_event)
    last = tree.GetEntries() if args.last_event is None else min(args.last_event, tree.GetEntries())
    if first >= last:
        raise ValueError("Empty event range [{}, {})".format(first, last))

    fieldnames = [
        "event",
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

    with open(args.output, "w", newline="") as output_file:
        writer = csv.DictWriter(output_file, fieldnames=fieldnames)
        writer.writeheader()

        for event in range(first, last):
            tree.GetEntry(event)
            for collection, member, branch, leaf in sorted(members, key=lambda item: item[:2]):
                elements = int(leaf.GetLen())
                bytes_per_element = int(leaf.GetLenType())
                uncompressed = elements * bytes_per_element
                tot_bytes = int(branch.GetTotBytes())
                zip_bytes = int(branch.GetZipBytes())
                compression = float(tot_bytes) / zip_bytes if zip_bytes else 0.0
                compressed_estimate = uncompressed / compression if compression else 0.0

                writer.writerow(
                    {
                        "event": event,
                        "collection": collection,
                        "member": member,
                        "type": leaf.GetTypeName(),
                        "elements": elements,
                        "uncompressed_bytes": uncompressed,
                        "compressed_bytes_estimate": "{:.6f}".format(compressed_estimate),
                        "branch_tot_bytes": tot_bytes,
                        "branch_zip_bytes": zip_bytes,
                        "branch_compression": "{:.6f}".format(compression),
                    }
                )

    root_file.Close()
    print("Wrote {}".format(args.output))
    print("Members: {}".format(", ".join("{}.{}".format(collection, member)
                                        for collection, member, _, _ in members)))


if __name__ == "__main__":
    try:
        main()
    except (OSError, RuntimeError, ValueError) as error:
        print("error: {}".format(error), file=sys.stderr)
        sys.exit(1)
