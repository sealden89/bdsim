import pyg4ometry as pg

def MakeBoxOfWater(vis=True):
    # registry holds all the bits of a model and definitions
    reg = pg.geant4.Registry()

    air   = pg.geant4.MaterialPredefined("G4_AIR", reg)
    steel = pg.geant4.MaterialPredefined("G4_STAINLESS-STEEL", reg)
    water = pg.geant4.MaterialPredefined("G4_WATER", reg)

    containerSolid = pg.geant4.solid.Box("container_solid", 202, 202, 202, reg)
    containerLV = pg.geant4.LogicalVolume(containerSolid, air, "container_lv", reg)
    
    boxOuter = pg.geant4.solid.Box("box_outer_solid", 200, 200, 200, reg)
    boxInner = pg.geant4.solid.Box("box_inner_solid", 180, 180, 180, reg)
    boxSolid = pg.geant4.solid.Subtraction("box_solid", boxOuter, boxInner, [[0,0,0],[0,0,0]], reg)
    boxLV = pg.geant4.LogicalVolume(boxSolid, steel, "box_lv", reg)

    waterSolid = pg.geant4.solid.Box("water_solid", 179.8, 179.8, 179.8, reg)
    waterLV = pg.geant4.LogicalVolume(waterSolid, water, "water_lv", reg)

    pg.geant4.PhysicalVolume([0,0,0],[0,0,0],boxLV,"box_pv",containerLV,reg)
    pg.geant4.PhysicalVolume([0,0,0],[0,0,0],waterLV,"water_pv",containerLV,reg)

    # check overlaps and recurse down into the geometry all the way to check all overlaps
    containerLV.checkOverlaps(recursive=True)
    
    reg.setWorld(containerLV)

    w = pg.gdml.Writer()
    w.addDetector(reg)
    w.write("boxofwater.gdml")

    if vis:
        v = pg.visualisation.VtkViewerColouredMaterial()
        v.addLogicalVolume(containerLV)
        v.addAxes(length=50)
        v.view()
    
if __name__ == "__main__":
    MakeBoxOfWater(vis=False)
