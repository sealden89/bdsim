import pybdsim
import ROOT

def Analyse(filename):
    # load data and access the event tree
    d = pybdsim.Data.Load(filename)
    eventTree = d.GetEventTree()
    
    combined     = set()
    creator      = set()

    # loop over events
    for i,event in enumerate(eventTree):
        # loop over trajectories
        for j in range(event.Trajectory.n):
            if abs(event.Trajectory.partID[j]) == 13:
                #event.Trajectory.printTrajectoryInfo(i)
                parentTrackIndex    = event.Trajectory.parentIndex[j]
                #event.Trajectory.printTrajectoryInfo(parentTrackIndex)
                parentTrajStepIndex = event.Trajectory.parentStepIndex[j]
                creatorPostProcessID    = event.Trajectory.postProcessTypes[parentTrackIndex][parentTrajStepIndex]
                creatorPostProcessSubID = event.Trajectory.postProcessSubTypes[parentTrackIndex][parentTrajStepIndex]
                combined.add((creatorPostProcessID,creatorPostProcessSubID))
                creator.add((event.Trajectory.preProcessTypes[j][0],event.Trajectory.preProcessSubTypes[j][0]))
                
    print(combined)
    print(creator)
            
