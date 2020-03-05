import purefieldplotter as _pfp
import pybdsim as _pybdsim
import numpy as _np

def Convert(fileIn, fileOut):
    """
    Convert the output of BDSFieldTester to a BDSIM format
    field map. Works on 2D data with the cartesian suffix.

    The data format from BDSFieldTester was a temporary one
    in the development of the field map features of BDSIM.

    Example:
    converttobdsim.Convert('quadrupole_carteasian.dat', 'quad-bdsim2d.dat')
    """
    d = _pfp.Load(fileIn)
    nx = int(d['header']['nX'])
    ny = int(d['header']['nY'])
    p = d['position'].reshape([nx,ny,3])[:,:,:2]
    f = d['field'].reshape([nx,ny,3])
    r = _np.append(p, f, axis=2)
    rf = _pybdsim.Field.Field2D(r)
    rf.Write(fileOut)
