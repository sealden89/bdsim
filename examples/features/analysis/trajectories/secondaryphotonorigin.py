import matplotlib.pyplot as plt
import pickle
import pybdsim
import ROOT

def Analyse(filename):
    """
    Plot 2D histogram of Z,X global position where secondary photons were produced.
    Uses a per event average histogram.
    """
    # load data and access the event tree
    d = pybdsim.Data.Load(filename)
    eventTree = d.GetEventTree()

    # construct a single histogram we're going to fill and use as a base
    baseHistogram = ROOT.TH2D("BasePhotonOrigin",  # object name
                              "Photon Origin",     # title
                              20, 0, 5,            # nbinsx, xlow, xhigh
                              10, -0.5, 0.5)       # nbinsy, ylow, yhigh

    # use bdsim histogram accumulator class that's been imported
    # by pybdsim and now exists in the ROOT namespace
    accumulator = ROOT.HistogramAccumulator(baseHistogram,
                                            2,               # n dimensions
                                            "PhotonOrigin",  # result object name
                                            "Photon Origin") # result histogram title

    # loop over events
    for event in eventTree:
        baseHistogram.Reset() # empty our 1 histogram
        # loop over trajectories
        for i in range(event.Trajectory.n):
            # do analysis here
            # if gamma and secondary
            if event.Trajectory.partID[i] == 22 and event.Trajectory.parentID > 0:
                # bin into the histogram
                baseHistogram.Fill(event.Trajectory.trajectories[i][0].z(),
                                   event.Trajectory.trajectories[i][0].y())
        # at the end of the analysis on this event, we accumulate the histogram
        accumulator.Accumulate(baseHistogram)

    # at the end of all events, we calculate the final mean per event
    result = accumulator.Terminate()

    # plot using pybdsim
    resultpy = pybdsim.Data.TH2(result)
    pybdsim.Plot.Histogram2D(resultpy, logNorm=True)
    plt.savefig('photonzy-distribution-log.pdf')
    pybdsim.Plot.Histogram2D(resultpy, logNorm=False)
    plt.savefig('photonzy-distribution.pdf')

    # save to a root file for later use
    # absolutely unintuitive how to write a basic object with root
    f = ROOT.TFile("resulthistogram.root", "RECREATE")
    # by default we bdsim classes turn off the default behaviour
    # for any new histogram to be added to any existing open file
    # because this is just crazy - we turn it back on otherwise
    # it won't write to file
    result.AddDirectory(True)
    result.Write()
    f.Close()

    # save with python
    f = open("resulthistogram.dat", "wb")
    pickle.dump(resultpy, f)
    f.close()

    return result


            
