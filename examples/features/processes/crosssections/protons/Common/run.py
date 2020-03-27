import pybdsim
import os as _os
import subprocess as _subprocess
import ROOT as _ROOT
import IPython as _IPython
import numpy as _np
import matplotlib.pyplot as _plt

from itertools import izip_longest

def materialDictForRun(dictIn):
    dictOut =  {"modelString":   dictIn.get("modelString"),
                "collength":     dictIn.get("thickness"),
                "target":        dictIn.get("material"),
                "ngenerate":     dictIn.get("nTotPart")
    }

    return dictOut

def materialDictForPlot(dictIn):
    dictOut =  {"nTotPart":    dictIn.get("nTotPart"),
                "thickness":     dictIn.get("thickness"),
                "densitygcm3":   dictIn.get("densitygcm3"),
                "atomicweight":  dictIn.get("atomicweight"),
                "lc":            dictIn.get("lc", "b"),
                "leg":           dictIn.get("leg", "xSection"),
                "pdg_tot_file":  dictIn.get("pdg_tot_file", None),
                "pdg_el_file":  dictIn.get("pdg_el_file", None),
    }

    return dictOut

def main(energies, kwargs, dir_path=_os.getcwd()+"/", common_path=_os.getcwd()+"/", debug=0, outfilename="output.dat", differentialXSec=False) :

    f = open(outfilename,"w")

    for energy in energies :
        print "Running",energy

        # make run files
        makeRunFiles(energy=energy, dir_path = dir_path, common_path = common_path, **kwargs)

        """
        #Some other common options
        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"solidnitrogen",100000)
        makeRunFiles("ftfp_bert_hp hadronic_elastic",energy,0.001,"solidoxygen",100000)
        """

        # execute bdsim
        bdsimProc = _subprocess.Popen("bdsim --file=proton.gmad --batch",
                                      stdout=_subprocess.PIPE,shell=True);
        #                                      stdout=_subprocess.PIPE,stderr=_subprocess.STDOUT,shell=True);

        bdsimOutput = bdsimProc.stdout.read()

        if debug:
            print "-------------------------"
            print bdsimOutput
            print "-------------------------"


        #Extract interation counts
        nEvent, nElastic, nInelastic = countHadronicInteractions(dir_path+"output.root")

        f.write(str(energy)+" "+str(nElastic)+" "+str(nInelastic)+"\n");
        f.flush()

        if differentialXSec:
            histos = differentialCountHistograms(dir_path+"output.root")
            #The lenght of the bin edges array is always 1 longer than the length of the histogram
            #Use izip_longest and add a dummy None element the the histogram for easy writing to file
            #When loading the file be careful of the dummy value

            _np.savetxt('energy_hist.dat', list(izip_longest(*histos[0], fillvalue=0)))
            _np.savetxt('angle_hist.dat', list(izip_longest(*histos[1], fillvalue=0)))
            _np.savetxt('pseudorapidity_hist.dat', list(izip_longest(*histos[2], fillvalue=0)))

        if len(energies)<100: #Dummy condition to always evaluate to true for testing purposes, set right later
            #_os.system("rm -rf output.root") #Delete the output root file if more than 1 energy value is used
            _os.system("mv output.root output_energy"+str(energy)+".root")

    f.close();

def makeRunFiles(modelString, energy, collength, target, ngenerate, dir_path, common_path) :
    optDict = {"__PHYSICSLISTS__":" "+modelString+" "}
    substituteTokens(common_path+"protonOptions.tpl", optDict, dir_path)

    runDict = {"__ENERGY__":str(energy),
               "__COLLENGTH__":str(collength),
               "__MATERIAL__":target,
               "__NGENERATE__":str(ngenerate)};
    substituteTokens(common_path+"proton.tpl", runDict, dir_path)

def substituteTokens(fileName, subDict, dir_path) :
    inFile   = open(fileName)
    basename = _os.path.basename(fileName)

    outFileName = dir_path+basename[0:basename.rfind(".")]+".gmad"
    outFile     = open(outFileName,"w")
    for inLine in inFile :
        line = inLine
        for key in subDict.keys() :
            line = line.replace(key,subDict[key])
        outFile.write(line)

    inFile.close()
    outFile.close()

def countHadronicInteractions(rooteve_file):
    
    root_data = pybdsim.Data.Load(rooteve_file)
    evt   = root_data.GetEventTree()
    nEvents  = int(evt.GetEntries())
    
    elasticWeights = []
    inelasticWeights = []

    for i in range(nEvents):
        dt = pybdsim.Data.TrajectoryData(root_data, i)
        
        postpt = dt.trajectories[0]["postPT"] # Chose the first point as it is the primary
        postpst = dt.trajectories[0]["postPST"]
       
        for i, pt in enumerate(postpt):
            if pt == 4:
                if postpst[i] == 111:
                    elasticWeights.append(1) # Should really be the weight
                elif postpst[i] == 121:
                    inelasticWeights.append(1)
                break # Only count the first interaction
            
    """
    # This is the old code that worked in 2017
    interactions = []
    for i in range(nEvents):
        evt.GetEntry(i)

        tracks = ev.trajectory.trackInteractions(1) #Ignore transport processes
        if tracks.empty():
            continue
        else:
            interactions.append(tracks)
            #_IPython.embed()

    #elasticInteractions   = [tr.at(0) for tr in interactions if tr.at(0).processType == 4 and tr.at(0).processSubType == 111]
    elasticWeights        = [tr.at(0).weight for tr in interactions if tr.at(0).processType != 1]
    #inelasticInteractions = [tr.at(0) for tr in interactions if tr.at(0).processType == 4 and tr.at(0).processSubType == 121]
    inelasticWeights      = [tr.at(0).weight for tr in interactions if tr.at(0).processType == 4 and tr.at(0).processSubType == 0] #DUMMY to preserve data structure
    """

    nElastic   = sum(elasticWeights)
    nInelastic = sum(inelasticWeights)

    return (nEvents ,nElastic, nInelastic)
