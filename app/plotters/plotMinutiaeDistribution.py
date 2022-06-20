#!/usr/bin/env python
"""
Plot the distribution of minutiae points with a given dataset.

The format of the dataset must be the txt files generated by the main cpp app.
```txt
<number of minutiae points>
<x> <y> <theta> <type> <quality>
<x> <y> <theta> <type> <quality>
<x> <y> <theta> <type> <quality>
...
```
To plot the map, the script needs also the image size.
Consider using the `file` command for that.
"""

import os
import sys
import matplotlib.pyplot as plt

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Error, must be 3 arguments.")
        print("app/plotMinutiaeDistribution.py path/to/txt/minutiae/files img_sizex img_sizey")
        exit()
    assert len(sys.argv) == 4
    path = sys.argv[1]
    img_sizex = int(sys.argv[2])
    img_sizey = int(sys.argv[3])
    filenames = os.listdir(path)
    minutiae = [[0]*img_sizey for _ in range(img_sizex)]
    for filename in filenames:
        with open(path+"/"+filename) as f:
            n = int(f.readline())  # number of minutiae points
            for _ in range(n):
                x, y, _, _, _ = f.readline().split(' ')
                try:
                    minutiae[int(x)][int(y)] += 1
                except IndexError:
                    print("Error, the specified sizex or sizey must be wrong")
                    exit()

    xs = []
    ys = []
    zs = []
    for i in range(img_sizex):
        for j in range(img_sizey):
            z = minutiae[i][j]
            if z != 0:
                xs.append(i)
                ys.append(j)
                zs.append(z)

    m = max(zs)
    zs = [z/m for z in zs]

    fig, ax = plt.subplots()
    ax.scatter(xs, ys, c=zs, s=5)
    plt.show()
