import pybdsim
import pymadx

import halosigma as _h

o = pymadx.Data.Tfs("../madx/ring.tfs.gz")
rcols = o.GetElementsOfType('RCOLLIMATOR')
names = rcols.GetColumn('NAME')

primSig = _h.primSig
secoSig = _h.secoSig
openSig = _h.openSig
smear   = _h.smear

for col in rcols:
    name = col['NAME']
    index = o.IndexFromName(name)
    data  = o[index-1] # element before to get the optics at the start of the collimator

    beam = pybdsim.Convert.MadxTfs2GmadBeam(o, data['NAME'])

    # customise the beam
    horizontal = 'X' in name
    hx = 'halo' if horizontal else 'gausstwiss'
    hy = 'gausstwiss' if horizontal else 'halo'
    beam.SetDistributionType('composite')
    beam.SetXDistrType(hx)
    beam.SetYDistrType(hy)
    beam.SetZDistrType('gausstwiss')
    beam.SetSigmaE(0)
    beam.SetS0(data['S']-5e-6) # put 5um in front of collimator

    primary = 'prim' in name.lower()
    sig = primSig if primary else secoSig
    beam.SetHaloNSigmaXInner(sig-smear)
    beam.SetHaloNSigmaXOuter(sig+3*smear)
    beam.SetHaloNSigmaYInner(sig-smear)
    beam.SetHaloNSigmaYOuter(sig+3*smear)
    beam.SetHaloXCutInner(sig-smear)

    # write to a file
    beam.WriteToFile('beam_'+name+'.gmad')
