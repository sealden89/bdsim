import numpy as np
import pybdsim

basename = "d2"

dRaw = pybdsim.Data.Load(basename+".root")

energydep = []
for Event in dRaw.GetEventTree():
    energydep.append(Event.Summary.energyDeposited)

energydep = np.array(energydep)

em = np.mean(energydep)
er = np.std(energydep)

dRaw.GetEventTree().GetEntry(0)
primEK = np.array(dRaw.GetEvent().Primary.kineticEnergy, dtype=float)[0]

print "Mean energy deposited:    ",em,"+-",er," GeV"
print "Primary kinetic energy:   ",primEK," GeV"
print "Fraction energy absorbed: ",(em/primEK)*100, " %"

dHist = pybdsim.Data.Load(basename+"-histos.root")
hscorer = dHist.histogramspy['Event/MergedHistograms/meshForAll-dosesc']
dose = hscorer.contents[0,0,0]

density = 19.3 #g/cm3 for tungsten (approximate)
volume  = 100*100*100 #cm*cm*cm
mass    = (density * volume) / 1000.0 # in kg
print "Density: ",density," g/cm^3"
print "Volume:  ",volume," cm^3"
print "Mass:    ",mass," kg"

energyFromDose = (dose * mass)/ 1.6021773e-10 # GeV to J

print "Dose:             ",dose," Gray"
print "Energy from dose: ",energyFromDose," GeV"

print "ratio (dose energy / energy deposition integral): ",energyFromDose/em
