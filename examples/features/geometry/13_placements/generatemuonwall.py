import pyg4ometry.gdml as _gd
import pyg4ometry.geant4 as _g4
import pyg4ometry.visualisation as _vi

def GenerateMuonWall(vis = False):
    reg = _g4.Registry()

    steel = _g4.MaterialPredefined("G4_STAINLESS-STEEL")
    
    wall   = _g4.solid.Box("muon_wall",2500,2000,500, reg)
    wallLV = _g4.LogicalVolume(wall, steel, "wall_lv", reg)
    
    # set world volume
    reg.setWorld(wallLV.name)
    
    # gdml output 
    w = _gd.Writer()
    w.addDetector(reg)
    w.write("muon-wall.gdml")
    
    if vis : 
        v = _vi.PubViewer()
        v.addLogicalVolume(reg.getWorldVolume())
        v.addAxes(_vi.axesFromExtents(extentBB)[0])
        v.view(interactive=interactive)

if __name__ == "__main__":
    GenerateMuonWall()
