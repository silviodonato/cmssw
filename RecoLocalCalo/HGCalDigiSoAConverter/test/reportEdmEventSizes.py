#!/usr/bin/env python3
"""Summarize edmEventSize -v branches by module label in kB/event."""

import argparse
import re
import subprocess
import sys


LABELS = (
    "hltHgcalDigis",
    "hltHgcalDigisDecompressed",
    "hltHgcalDigisSoA",
)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "file", nargs="?", default="hltHgcalDigisRoundTripLZMA4.root"
    )
    args = parser.parse_args()

    try:
        result = subprocess.run(
            ["edmEventSize", "-v", args.file],
            check=True,
            capture_output=True,
            text=True,
        )
    except FileNotFoundError:
        parser.error("edmEventSize was not found; run cmsenv first")
    except subprocess.CalledProcessError as error:
        parser.error(error.stderr.strip() or error.stdout.strip())

    totals = {label: [0.0, 0.0, 0] for label in LABELS}
    events = None
    for line in result.stdout.splitlines():
        event_match = re.search(r"\bEvents\s+(\d+)\b", line)
        if event_match:
            events = int(event_match.group(1))
            continue

        fields = line.split()
        if len(fields) != 3 or not fields[0].endswith("."):
            continue
        try:
            uncompressed, compressed = map(float, fields[1:])
            _, label, _, _ = fields[0].rstrip(".").rsplit("_", 3)
        except ValueError:
            continue
        if label in totals:
            totals[label][0] += uncompressed
            totals[label][1] += compressed
            totals[label][2] += 1

    if events is None or events == 0:
        parser.error("edmEventSize reported no events")
    missing = [label for label, (_, _, count) in totals.items() if count == 0]
    if missing:
        parser.error("no branches found for: " + ", ".join(missing))

    print(f"Events: {events}  ")
    print("Units: kB/event (1 kB = 1000 bytes)\n")
    print("| Module label | Branches | Uncompressed (kB/event) | Compressed (kB/event) |")
    print("|---|---:|---:|---:|")
    for label, (uncompressed, compressed, count) in totals.items():
        print(f"| `{label}` | {count} | {uncompressed / 1000:.2f} | {compressed / 1000:.2f} |")


if __name__ == "__main__":
    main()
