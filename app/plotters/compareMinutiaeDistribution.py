#!/usr/bin/env python

import sys
import matplotlib.pyplot as plt

if __name__=="__main__":
    if len(sys.argv) != 3:
        print("Error, must be 2 arguments.")
        print("app/plotMinutiaeDistribution.py path/to/txt/minutiae/file/jfjx path/to/txt/minutiae/file/Casia")
        exit()

    path1 = sys.argv[1]
    path2 = sys.argv[2]
    x1,y1 = [],[]
    with open(path1) as f:
        n = int(f.readline())  # number of minutiae points
        for _ in range(n):
            x,y,_,_,_ = f.readline().split(' ')
            x1.append(int(x))
            y1.append(int(y))

    x2,y2 = [],[]
    with open(path2) as f:
        for line in f.readlines():
            x,y,_ = line.strip().replace("[","").replace("]","").replace(" ","").split(",")
            x2.append(int(float(x)))
            y2.append(int(float(y)))


    fig, ax = plt.subplots()
    ax.scatter(x1, y1, c='r')
    ax.scatter(x2, y2, c='b')
    plt.show()
