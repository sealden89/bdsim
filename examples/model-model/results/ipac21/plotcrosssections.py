import gzip
import json
import matplotlib.pyplot as plt

def LoadCrossSectionLines():
    xy = "bmm-sp-xy-z500m-plane-outline.dat.gz"
    xz = "bmm-sp-xz-plane-outline.dat.gz"
    with gzip.open(xy,'r') as f:
        xy = json.load(f)
    with gzip.open(xz,'r') as f:
        xz = json.load(f)
    return xy,xz

def PlotXZ():
    xy,xz = LoadCrossSectionLines()

    plt.figure()
    # it's made of thousands of single lines
    for i in range(len(xz[0])):
        plt.plot(xz[1][i],xz[0][i],c='k',lw=0.5)
    plt.xlabel('Z (m)')
    plt.ylabel('X (m)')
        
def PlotXY():
    xy,xz = LoadCrossSectionLines()

    plt.figure()
    # it's made of thousands of single lines
    for i in range(len(xy[0])):
        plt.plot(xy[0][i],xy[1][i],c='k',lw=0.5)
    plt.xlabel('X (m)')
    plt.ylabel('Y (m)')

def PlotFile(filename):
    f = open(filename, 'r')
    d = json.load(f)
    f.close()

    for i in range(len(d[0])):
        plt.plot(d[0][i],d[1][i],c='k',lw=0.5)
