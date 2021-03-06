#!/usr/bin/env python2.7

import matplotlib.pyplot as _plt
import numpy as _np
import pybdsim
from subprocess import check_call as _check_call



def MuonField(x,y,z,B):
    # no variation in z

    field = {0 : [0,B,0],
             1 : [-B,0,0],
             2 : [0,-B,0],
             3 : [B,0,0]}

    if x == 0 and y == 0:
        return [0,0,0]
    else:
        # arctan2 is like atan but with right for each quadrant
        # arctan2 goes from -pi to pi -> shift by pi so always positive
        # and add pi/4 to go for diamond shape we want
        theta = _np.arctan2(y,x) + _np.pi + _np.pi/4.0
        quadrant = int(_np.floor(theta / (_np.pi/2.0) ))
        quadrant %= 4 # modulo it with 4 so we wrap around quadrants
        #print(x,y,theta,quadrant)
        return field[quadrant]
    

def main():
    # in a quick literature search, I found 16kG as an expected figure
    # for magnetised iron (maybe with some windings). This is around 1.6T
    # so, saturated iron. A little more modest value chosen.
    B = 0.8 # T
    
    # generate x and y points along their own axes
    x = _np.linspace(-250, 250, 10)
    y = _np.linspace(-200, 200, 10)
    
    data = []
    # loop over and build up 3d lists of lists of lists
    for xi in x:
        v = []
        for yi in y:
            f = MuonField(xi,yi,0,B)
            v.append([xi,yi,f[0],f[1],f[2]])
        data.append(v)

    # convert to numpy array
    data = _np.array(data)
    
    # construct a BDSIM format field object and write it out
    f = pybdsim.Field.Field2D(data)
    f.Write('muon-field.dat')

    # compress the result
    _check_call(['gzip', "muon-field.dat"])

    Plot(data)
    #return data

def Plot(array):

    a = array #shortcut
    mag = _np.sqrt(a[:,:,2]**2 + a[:,:,3]**2)

    _plt.figure()
    _plt.quiver(a[:,:,0], a[:,:,1], a[:,:,2], a[:,:,3], mag, cmap=_plt.cm.magma, pivot='mid', scale=None)
    _plt.xlabel('X (cm)')
    _plt.ylabel('Y (cm)')
    _plt.title('2D Muon Wall Field')
    _plt.colorbar(label='Tesla')
    _plt.tight_layout()
    _plt.axes().set_aspect('equal', 'datalim')
    _plt.savefig('muon-field.pdf',dpi=400)
    
if __name__ == "__main__":
    main()
