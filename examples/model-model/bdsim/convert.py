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

a,b,c = pybdsim.Convert.MadxTfs2Gmad('../madx/ring.tfs.gz', 'linear/bmm-linear', linear=True, aperturedict=ap, collimatordict=cols)

a,b,c = pybdsim.Convert.MadxTfs2Gmad('../madx/ring.tfs.gz', 'nl/bmm-nl', aperturedict=ap, collimatordict=cols)

a,b,c = pybdsim.Convert.MadxTfs2Gmad('../madx/ring.tfs.gz', 'halo/bmm-nl-halo', aperturedict=ap, collimatordict=cols)

# make collimator beams in gmad
import makecollimatorbeams

