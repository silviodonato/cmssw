#!/usr/bin/env python3
"""Export D128 silicon digi geometry and create a self-contained hexagonal-cell viewer."""
import argparse
import base64
import gzip
import json
from pathlib import Path
import subprocess
from dumpHGCalDigiDetIds import layer_argument


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('file', help='Input EDM ROOT file (first event)')
    p.add_argument('--layer', type=layer_argument, required=True, help='1–31 or all')
    p.add_argument('--zside', choices=('-1', '1', 'all'), default='1')
    p.add_argument('--module', default='hltHgcalDigisDecompressed')
    p.add_argument('--instance', choices=('EE', 'HEfront'), default='EE')
    p.add_argument('--process', default='ROUNDTRIP')
    p.add_argument('--output', default='hgcal-detids', help='Output prefix (one HTML plus PNGs)')
    p.add_argument('--geometry-json', help='Reuse an export from this file/product made by exportHGCalDigiGeometry_cfg.py')
    p.add_argument('--no-png', action='store_true', help='Only generate the single HTML viewer')
    a = p.parse_args()
    prefix = Path(a.output)
    prefix.parent.mkdir(parents=True, exist_ok=True)
    geometry_json = Path(a.geometry_json) if a.geometry_json else Path(str(prefix) + '-geometry.json')
    if not a.geometry_json:
        source = a.file if ':' in a.file else 'file:' + str(Path(a.file).resolve())
        subprocess.run(['cmsRun', str(Path(__file__).with_name('exportHGCalDigiGeometry_cfg.py')),
                        'inputFiles=' + source, 'module=' + a.module, 'instance=' + a.instance,
                        'processName=' + a.process, 'jsonOutput=' + str(geometry_json)], check=True)
    data = json.loads(geometry_json.read_text())
    if data.get('geometry') != 'D128' or data.get('schema') != 2:
        p.error('Expected a fresh schema-2 D128 export with corrected lattice polygons and ADC samples')
    data['cells'] = [r for r in data['cells'] if (a.layer == 'all' or r[1] == a.layer)
                     and (a.zside == 'all' or r[2] == int(a.zside))]
    if not data['cells']:
        p.error('No digis matched this layer/endcap in the first event')
    data['product'] = ':'.join((a.module, a.instance, a.process))
    data['source'] = a.file
    template = Path(__file__).with_name('hgcalDetIdViewer.html').read_text()
    payload = base64.b64encode(gzip.compress(json.dumps(data, separators=(',', ':')).encode())).decode()
    Path(str(prefix) + '.html').write_text(template.replace('/*DATA_GZIP*/', payload))
    layers = sorted({r[1] for r in data['cells']})
    print(f'Created {prefix}.html: {len(data["cells"]):,} digis, {len(layers)} layers')
    if not a.no_png:
        for side in sorted({r[2] for r in data['cells']}):
            for layer in layers:
                rows = [r for r in data['cells'] if r[1] == layer and r[2] == side]
                if rows:
                    png = (str(prefix) + f'-layer{layer:02d}-z{side:+d}.png'
                           if a.layer == 'all' or a.zside == 'all' else str(prefix) + '.png')
                    render_png(rows, data, png)


def render_png(rows, data, output):
    from collections import Counter
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt
    from matplotlib.collections import PolyCollection
    from matplotlib.colors import Normalize
    counts = Counter((r[3], r[4]) for r in rows)
    wafer = max(counts, key=counts.get)
    cells = [r for r in rows if (r[3], r[4]) == wafer]
    fig, axes = plt.subplots(1, 2, figsize=(13, 6), layout='constrained')
    vmax = max((r[11][2][0] for r in rows if len(r[11]) > 2 and r[11][2][1] == 0), default=1)
    norm = Normalize(0, max(1, vmax))
    cmap = plt.get_cmap('viridis')
    for ax, selected in zip(axes, (rows, cells)):
        ax.add_collection(PolyCollection([r[10] for r in selected], facecolors=[cmap(norm(r[11][2][0])) if len(r[11]) > 2 and r[11][2][1] == 0 else '#d64ac5' for r in selected],
                                        edgecolors='#144460', linewidths=.2))
        ax.autoscale_view()
        ax.set(aspect='equal', xlabel='Global x [cm]', ylabel='Global y [cm]')
    fig.colorbar(plt.cm.ScalarMappable(norm=norm, cmap=cmap), ax=list(axes), label='ADC count, sample 2 (magenta: TOT/missing)')
    axes[0].set_title(f'{len(rows):,} digi cells')
    axes[1].set_title(f'Wafer {wafer}: {len(cells)} digi cells')
    fig.suptitle(f'D128 | Layer {rows[0][1]}, z side {rows[0][2]:+d} | Event {data["event"]}\n'
                 'D128 nominal cell centres and lattice hexagons')
    fig.savefig(output, dpi=160)
    plt.close(fig)
    print('Created ' + output)


if __name__ == '__main__':
    main()
