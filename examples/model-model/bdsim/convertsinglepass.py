import pybdsim
import pymadx


# generate collimator settings
import makecollimatorgaps

# aperture information
ap = pymadx.Data.Aperture('../madx/ring_aperture.tfs.gz')
ap = ap.RemoveBelowValue(0.005)

# collimator settings
# uses pybdsim.Data.BDSAsciiData which behaves like dictionaries of dictionaries
cols = pybdsim.Data.Load('collimatorSettings.dat')

a,b = pybdsim.Convert.MadxTfs2Gmad("../madx/ring_from_collimation.tfs.gz",
                                   "singlepass/bmm-sp",
                                   linear=True,
                                   aperturedict=ap,
                                   collimatordict=cols,
                                   stopname="QUAD.F.H_17",
                                   samplers=None)

