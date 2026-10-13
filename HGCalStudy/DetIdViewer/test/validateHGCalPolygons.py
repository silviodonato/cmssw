"""Check convex cell polygons for genuine interior overlap, ignoring coordinate rounding."""
import json
import math
import sys
from collections import Counter, defaultdict
rows=json.load(open(sys.argv[1] if len(sys.argv)>1 else 'hgcal-d128-cells.json'))['cells']
grid=defaultdict(list)
overlap=[]
for r in rows:
    poly=r[10]
    cx=sum(p[0] for p in poly)/len(poly)
    cy=sum(p[1] for p in poly)/len(poly)
    ix,iy=math.floor(cx/2),math.floor(cy/2)
    for dx in (-1,0,1):
        for dy in (-1,0,1):
            for q in grid[r[1],r[2],ix+dx,iy+dy]:
                a,b=poly,q[10]
                if max(p[0] for p in a)<=min(p[0] for p in b) or max(p[0] for p in b)<=min(p[0] for p in a):continue
                if max(p[1] for p in a)<=min(p[1] for p in b) or max(p[1] for p in b)<=min(p[1] for p in a):continue
                separated=False
                for shape in (a,b):
                    for i in range(len(shape)):
                        x=shape[(i+1)%len(shape)][1]-shape[i][1];y=shape[i][0]-shape[(i+1)%len(shape)][0]
                        pa=[p[0]*x+p[1]*y for p in a];pb=[p[0]*x+p[1]*y for p in b]
                        if min(max(pa),max(pb))-max(min(pa),min(pb))<=2e-5*math.hypot(x,y):separated=True;break
                    if separated:break
                if not separated:overlap.append((r[0],q[0],r[3:5]==q[3:5]))
    grid[r[1],r[2],ix,iy].append(r)
def area(p):return abs(sum(p[i][0]*p[i-1][1]-p[i-1][0]*p[i][1] for i in range(len(p))))/2
def aspect(p):return (max(q[1] for q in p)-min(q[1] for q in p))/(max(q[0] for q in p)-min(q[0] for q in p))
# A regular hexagon with a vertex on the x axis has height/width = sqrt(3)/2.
misshapen=[r[0] for r in rows if abs(aspect(r[10])-math.sqrt(3)/2)>1e-3]
print('Misshapen (non-regular) polygons by endcap:',Counter(r[2] for r in rows if abs(aspect(r[10])-math.sqrt(3)/2)>1e-3),misshapen[:5])
assert not misshapen
print('Cells:',len(rows),'overlapping pairs:',len(overlap),'same/different wafer:',Counter(p[2] for p in overlap))
print('Examples:',overlap[:10])
print('Sample counts:',Counter(len(r[11]) for r in rows))
print('Sample 2 modes:',Counter(r[11][2][1] for r in rows if len(r[11])>2))
print('Sample 2 ADC min/max:',min(r[11][2][0] for r in rows if not r[11][2][1]),max(r[11][2][0] for r in rows if not r[11][2][1]))
assert not overlap
