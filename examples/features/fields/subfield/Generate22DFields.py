#!/usr/bin/env python2.7

from math import sqrt
import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim

def Field(fx,fy,fz,xValue,yValue):
    data = []
    # loop over and build up 3d lists of lists of lists
    for xi in [-xValue,xValue]:
        v = []
        for yi in [-yValue,yValue]:
            v.append([xi,yi,fx,fy,fz])
        data.append(v)

    # convert to numpy array
    data = _np.array(data)
    return data

def main():
    outer = Field(sqrt(2),sqrt(2),0,30,30)
    inner = Field(-sqrt(2),sqrt(2),0,20,20)
    
    # construct a BDSIM format field object and write it out
    fo = pybdsim.Field.Field2D(outer)
    fo.Write('outer.dat')

    fi = pybdsim.Field.Field2D(inner)
    fi.Write('inner.dat')
    
if __name__ == "__main__":
    main()
