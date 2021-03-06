import pyg4ometry.gdml as _gd
import pyg4ometry.geant4 as _g4
import pyg4ometry.visualisation as _vi

def GenerateMuonWall(vis = False):
    reg = _g4.Registry()

    steel = _g4.MaterialPredefined("G4_STAINLESS-STEEL")
    
    wallBlock = _g4.solid.Box("wall_block",2500, 2000, 500, reg)
    wallCut   = _g4.solid.Box("wall_cut",   200,  200, 600, reg)
    wall      = _g4.solid.Subtraction("wall",
                                      wallBlock,
                                      wallCut,
                                      [[0,0,0],[0,0,0]],
                                      reg)
    wallLV = _g4.LogicalVolume(wall, steel, "wall_lv", reg)

    # place another volume inside with some finite rotation to check
    # field transforms and coordinate lookups

    core = _g4.solid.Tubs("core",
                          0,
                          200,
                          400,
                          0, "2*pi",
                          reg)
    coreLV = _g4.LogicalVolume(core, steel, "core_lv", reg)

    corePV = _g4.PhysicalVolume([0,"-pi/2.123",0],
                                [300,400,0],
                                coreLV,
                                "core_pv",
                                wallLV,
                                reg)
    
    # set world volume
    reg.setWorld(wallLV.name)
    
    # gdml output 
    w = _gd.Writer()
    w.addDetector(reg)
    w.write("muon-wall2.gdml")
    
    if vis: 
        v = _vi.PubViewer()
        v.addLogicalVolume(reg.getWorldVolume())
        v.addAxes(_vi.axesFromExtents(extentBB)[0])
        v.view(interactive=interactive)

if __name__ == "__main__":
    GenerateMuonWall()
