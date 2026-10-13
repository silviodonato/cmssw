#!/usr/bin/env python3
"""Create a self-contained 3D WebGL viewer of HGCal digi charge, one panel per endcap.

Reads the JSON written by exportHGCalDigiGeometry_cfg.py (HGCalDigiGeometryExport) and embeds
the cells as gzip-compressed column arrays in hgcalDigi3DViewer.html.
"""
import argparse
import base64
import gzip
import json
import sys
from array import array
from pathlib import Path

MISSING = 2  # mode value for a sample the digi does not have


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--geometry-json', default='hgcal-d128-cells.json', help='Export from exportHGCalDigiGeometry_cfg.py')
    p.add_argument('--product', default='hltHgcalDigisDecompressed:EE:ROUNDTRIP', help='Label shown in the viewer')
    p.add_argument('--source', default='hltHgcalDigisRecHitRoundTripZSTD3.root', help='Label shown in the viewer')
    p.add_argument('--output', default='hgcal-d128-3d.html')
    a = p.parse_args()
    if sys.byteorder != 'little':
        p.error('The viewer reads little-endian arrays')
    data = json.loads(Path(a.geometry_json).read_text())
    cells = data['cells']
    if not cells:
        p.error('No cells in ' + a.geometry_json)
    n = len(cells)
    S = max(len(r[11]) for r in cells)
    has_toa = all(len(s) > 2 for r in cells for s in r[11])
    ids, xs, ys, zs = array('I'), array('f'), array('f'), array('f')
    values, toas, layers, modes = array('H'), array('H'), array('B'), array('B')
    for r in cells:
        ids.append(r[0]); layers.append(r[1]); xs.append(r[7]); ys.append(r[8]); zs.append(r[9])
        for i in range(S):
            if i < len(r[11]):
                s = r[11][i]
                values.append(s[0]); modes.append(s[1]); toas.append(s[2] if has_toa else 0)
            else:
                values.append(0); modes.append(MISSING); toas.append(0)
    # Column order keeps every typed array aligned (see the offsets in hgcalDigi3DViewer.html).
    blob = b''.join(c.tobytes() for c in (ids, xs, ys, zs, values, toas, layers, modes))
    meta = {'n': n, 'samples': S, 'hasToa': has_toa, 'event': data.get('event'), 'geometry': data.get('geometry'),
            'product': a.product, 'source': a.source}
    payload = base64.b64encode(gzip.compress(blob, 9)).decode()
    template = Path(__file__).with_name('hgcalDigi3DViewer.html').read_text()
    html = template.replace('/*META*/', json.dumps(meta)).replace('/*DATA_GZIP*/', payload)
    Path(a.output).write_text(html)
    print(f'Created {a.output}: {n:,} digis, {S} samples, TOA {"included" if has_toa else "missing (old export)"}, '
          f'{len(html) / 1e6:.1f} MB')


if __name__ == '__main__':
    main()
