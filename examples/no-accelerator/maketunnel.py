import numpy as _np
import pyg4ometry as pg

# make the visualisation of cylinders a little smoother as we only have one here
pg.config.SolidDefaults.Tubs.nslice = 32

def MakeTunnel(vis=True):
    # registry holds all the bits of a model and definitions
    reg = pg.geant4.Registry()

    # make a container solid for the geometry - units are mm by default
    # 50x50x50m
    containerSolid = pg.geant4.solid.Box("container_solid",
                                         50e3, 50e3, 50e3, reg)
    air = pg.geant4.MaterialPredefined("G4_AIR")
    containerLV = pg.geant4.LogicalVolume(containerSolid, air, "container_lv", reg)

    # create some tunnel geometry
    # we create a hollow concrete tube with a bit of a floor
    # so we use a hollow cylinder (Tubs) combined (unioned) with a box
    # note lengths in pyg4ometry are the same as GDML which are full length
    tunnelLength = 20e3
    tunnelInnerRadius = 2000 # mm
    tunnelThickness   = 500  # mm
    tunnelFloorFromCentre = 1000 #mm
    
    twopi = pg.gdml.Constant("twopi", "2.0*pi", reg)
    safety = pg.gdml.Constant("safety", 1e-3, reg)
    
    tunnelPartA = pg.geant4.solid.Tubs("tunnel_part_a_solid",
                                       tunnelInnerRadius,
                                       tunnelInnerRadius + tunnelThickness,
                                       tunnelLength,
                                       0, twopi,
                                       reg)

    # calculate how wide the floor needs to be - x1.1 for some overlap
    partBHalfWidthX = _np.sqrt(tunnelInnerRadius**2 - tunnelFloorFromCentre**2) * 1.1
    partBHeightY = tunnelInnerRadius - tunnelFloorFromCentre
    # note it's important to make this bit not the exact same length in z
    # as partA because they'll be unioned (joined) together and we should avoid
    # coplanar faces - ie the end faces would be exactly the same and this can cause
    # tracking problems - make them ever so slightly different in length
    tunnelPartB = pg.geant4.solid.Box("tunnel_part_b_solid",
                                      2*partBHalfWidthX,
                                      partBHeightY,
                                      tunnelLength-2*safety,
                                      reg)

    partBDY = tunnelFloorFromCentre + 0.5*partBHeightY
    tunnelSolid = pg.geant4.solid.Union("tunnel_solid",
                                        tunnelPartA,
                                        tunnelPartB,
                                        [[0,0,0],[0,-partBDY,0]],
                                        reg)
    concrete = pg.geant4.MaterialPredefined("G4_CONCRETE")
    tunnelLV = pg.geant4.LogicalVolume(tunnelSolid, concrete, "tunnel_lv", reg)
    tunnelPV = pg.geant4.PhysicalVolume([0,0,0],
                                        [0,0,0.5*tunnelLength],
                                        tunnelLV,
                                        "tunnel_pv",
                                        containerLV,
                                        reg)

    # check overlaps and recurse down into the geometry all the way to check all overlaps
    containerLV.checkOverlaps(recursive=True)
    
    reg.setWorld(containerLV)

    w = pg.gdml.Writer()
    w.addDetector(reg)
    w.write("tunnel.gdml")

    if vis:
        v = pg.visualisation.VtkViewerColouredMaterial()
        v.addLogicalVolume(containerLV)
        v.addAxes(length=2000)
        v.view()
    
if __name__ == "__main__":
    MakeTunnel(vis=False)
