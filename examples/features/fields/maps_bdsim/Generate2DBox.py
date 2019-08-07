#!/usr/bin/env python2.7

from math import sqrt
import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim

def main():
    fx,fy,fz = sqrt(0.5),sqrt(0.5),0
    data = []
    # loop over and build up 3d lists of lists of lists
    for xi in [-10,10]:
        v = []
        for yi in [-20,20]:
            v.append([xi,yi,fx,fy,fz])
        data.append(v)

    # convert to numpy array
    data = _np.array(data)
    
    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field2D(data)
    f.Write('2dboxexample.dat')

    fd = pybdsim.Field.Field2D(data,doublePrecision=True)
    fd.Write('2dboxexample_dp.dat')

    # compress the result
    from subprocess import check_call
    check_call(['gzip', '2dboxexample.dat'])

    #Plot(data)
    return data

def Plot(array):

    a = array #shortcut
    mag = _np.sqrt(a[:,:,2]**2 + a[:,:,3]**2)

    _plt.figure()
    _plt.quiver(a[:,:,0], a[:,:,1], a[:,:,2], a[:,:,3], mag, cmap=_plt.cm.magma, pivot='mid', scale=None)
    _plt.xlabel('X (cm)')
    _plt.ylabel('Y (cm)')
    _plt.title('2D Example Data')
    _plt.colorbar(label='Tesla')
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')
    _plt.savefig('2dexample.png',dpi=400)
    
if __name__ == "__main__":
    main()
