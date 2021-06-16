import math
import pickle
import pybdsim
import ROOT

def AnalyseFile(filename, outfilename):
    d = pybdsim.Data.Load(filename)
    eventTree = d.GetEventTree()

    headerTree = d.GetHeaderTree()
    header     = d.GetHeader()
    headerTree.GetEntry(0)
    nOriginalEvents = header.header.nOriginalEvents

    particles    = [12,-12,13,-13,14,-14]
    particlesStr = [str(p) for p in particles]
    names        = [r'$\nu_e$',     r'$\overline{\nu}_e$',
                    r'$\mu^-$',     r'$\mu^+$',
                    r'$\nu_{\mu}$', r'$\overline{\nu}_{\mu}$']
    
    # setup histograms
    oZXCentral = {}
    oZXSelect = {}
    oZX = {}
    spectraCentral = {}
    spectraSelect = {}
    spectra = {}
    for p,ps in zip(particles,particlesStr):
        h = ROOT.TH2D("Origin_ZX_Select_"+ps, "Origin of "+ps,
                      500, 0, 500,
                      160, -20, 20)
        oZXSelect[p] = h
        h2 = ROOT.TH2D("Origin_ZX_"+ps, "Origin of "+ps,
                       500, 0, 500,
                       160, -20, 20)
        oZX[p] = h2
        hs = ROOT.TH1D("Spectra_Select_"+ps, "Kinetic Energy of "+ps,
                       650, 0, 6500)
        spectraSelect[p] = hs
        hs2 = ROOT.TH1D("Spectra_"+ps, "Kinetic Energy of "+ps,
                        650, 0, 6500)
        spectra[p] = hs2

    # dictionaries for histories tuple(pdg sequence) : [sum(w), sum(w^2)]
    # use tuple as key, so unique histories can be added up
    histories = {}
    historiesSelect = {}

    nEvents = 0
    for event in eventTree:
        nEvents +=1

        # for each hit in the "ti18ramp" sampler
        for i in range(event.ti18ramp.n):
            tanTX = event.ti18ramp.xp[i] / event.ti18ramp.zp[i] # tan(theta_x)
            
            partID = event.ti18ramp.partID[i]
            weight  = event.ti18ramp.weight[i] # weight when it hits the sampler
            trackID = event.ti18ramp.trackID[i]
            trajectoryStorageIndex = event.Trajectory.trackID_trackIndex[trackID]
            originXYZ = event.Trajectory.XYZ[trajectoryStorageIndex][0]

            hist = GetHistoryTuple(event, trackID)

            # add to general histories
            if hist not in histories:
                histories[hist] = [0.0,0.0]
            histories[hist][0] += weight
            histories[hist][1] += weight*weight

            interesting = 0.005 < tanTX < 0.01

            if partID in particles:
                x,z = originXYZ[0], originXYZ[2]
                if interesting:
                    # where does that bump come from?
                    oZXSelect[partID].Fill(z, x, weight)
                    spectraSelect[partID].Fill(event.ti18ramp.kineticEnergy[i], weight)

                    # add to specific history
                    if hist not in historiesSelect:
                        historiesSelect[hist] = [0.0,0.0]
                    historiesSelect[hist][0] += weight
                    historiesSelect[hist][1] += weight*weight
                else:
                    oZX[partID].Fill(z, x, weight)
                    spectra[partID].Fill(event.ti18ramp.kineticEnergy[i], weight)



    historiesFileName = outfilename[:-5]
    TerminateHistoryAndWrite(histories,       historiesFileName+".dat")
    TerminateHistoryAndWrite(historiesSelect, historiesFileName+"_select.dat")
            
    f = ROOT.TFile(outfilename, "RECREATE")
    for d in [oZXSelect, oZX, spectraSelect, spectra]:
        for pid,h in d.items():
            h.AddDirectory(True) # bdsim / rebdsim classes turn off AddDirectory by default
            h.Write()
    f.Close()


def GetHistoryTuple(currentEvent, trackID):
    ce = currentEvent # shortcut
    
    storageIndex = ce.Trajectory.trackID_trackIndex[trackID]
    parentID = 1 # just not 0
    history = []
    
    while parentID !=0:
        pdgID = ce.Trajectory.partID[storageIndex]
        history.append(pdgID)
        parentID = ce.Trajectory.parentID[storageIndex]
        storageIndex = ce.Trajectory.parentIndex[storageIndex]
    
    return tuple(history)

def TerminateHistoryAndWrite(histories, outfilename):
    # error is sqrt(sum of weights^2)
    for k,v in histories.items():
        histories[k] = (v[0],math.sqrt(v[1]))
        
    f = open(outfilename, "wb")
    pickle.dump(histories, f)
    f.close()
    
