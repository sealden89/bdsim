import numpy as np
import ROOT
import root_numpy


def AverageEventTime(fileName):
    """
    Returns average and standard devation of File>Event>Info.duration in
    BDSIM ROOT (event format) output.
    """
    f = ROOT.TFile(fileName)
    eventTree = f.Get("Event")
    duration  = root_numpy.tree2array(eventTree, branches="Info.duration")
    return np.mean(duration), np.std(duration)
