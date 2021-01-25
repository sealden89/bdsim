import pyg4ometry 

def gdmlDisplayPyg4ometry(fileName) :
    r = pyg4ometry.gdml.Reader(fileName)
    l = r.getRegistry().getWorldVolume()
    v = pyg4ometry.visualisation.VtkViewer()
    v.addLogicalVolume(l)
    v.view()
