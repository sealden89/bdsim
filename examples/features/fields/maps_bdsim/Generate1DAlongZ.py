#!/usr/bin/env python2.7

import numpy as _np

def main():
    amplitude = 0.5
    length    = 0.6 #m
    period    = 0.1 #m
    ncells    = length / period
    zStep     = length / 100.
    z  = _np.arange(0, length+1e-9, zStep)
    by = amplitude * _np.sin(2*_np.pi * z / period)
    
    zlen = len(z)
    zmin = z.min()*100
    zmax = z.max()*100
    
    # make new file name 
    outputFileName = "1dexample-along-z.dat"

    # open file 
    f = open(outputFileName,"w")

    # write header 
    f.write("zmin> " + str(zmin) + '\n')
    f.write("nz>   " + str(zlen) + '\n')
    f.write("zmax> " + str(zmax) + '\n')
    f.write("! Z Fx Fy Fz\n")
    
    # write field
    for i in range(0,len(z),1) : 
        f.write("%.9f %.9f %.9f %.9f\n" % (z[i]*100, 0, by[i], 0))
    f.close()
    return z,by

if __name__ == "__main__":
    main()
