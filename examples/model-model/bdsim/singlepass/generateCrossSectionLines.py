import pyg4ometry

import pyg4ometry.config as _config
_config.setGlobalMeshSliceAndStack(56)

def GenerateCrossSectionLines(filename='bmm-sp-geometry.gdml'):

    r = pyg4ometry.gdml.Reader(filename)
    reg = r.getRegistry()
    wlv = reg.getWorldVolume()

    v = pyg4ometry.visualisation.VtkViewer()
    v.setCutterOrigin('z', [0,0,500000])
    v.addLogicalVolume(wlv)
    mmToM = 1e-3
    v.exportCutterSection('bmm-sp-xy-z500m-plane-outline.dat', 'z', mmToM)

    v.exportCutterSection('bmm-sp-xz-plane-outline.dat', 'y', mmToM)
    v.view()
