#!/usr/bin/env python

import os as _os
import sys as _sys

#Need this to extendend the python path in the script.
#This keeps common files separate without using system paths

common_path = _os.path.abspath('../../Common')
_sys.path.append(common_path)

import run
import plot
import matplotlib.pyplot as _plt
import os as _os

def RunAnalysis():
    physics_lists = "ftfp_bert hadronic_elastic"

    materialDict = {"material":     "solidhydrogen",
                    "densitygcm3":   8.96,
                    "atomicweight":  1,
                    "nTotPart":      2,
                    "thickness":     0.001,
                    "modelString":   physics_lists,
                    "lc":            "r",
                    "leg":           r" + ".join([pp.replace(r"_", r"\_") for pp in physics_lists.split()]),
                    "pdg_tot_file":  "../../Common/rpp2014-pp_total.dat",
                    "pdg_el_file":   "../../Common/rpp2014-pp_elastic.dat",
    }

    energies = [0.94,0.95,0.96,0.97,0.98,0.99,
                1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,
                2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7,2.8,2.9,
                3.0,3.5,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,9.0,10.0, 20, 30, 40, 50, 60, 70, 80, 90,
                100, 125, 150,200,225,250,300,350,375,425,450,500,1000,2000,3000,4000,5000,
                6000,6500,7000,7500,8000, 8500,9000,9500,
                10000,12500,25000,37500,50000,75000,100000]

    a = run.main(energies, run.materialDictForRun(materialDict), dir_path = _os.getcwd()+"/", common_path = common_path+"/")
    p = plot.plot(filename=_os.getcwd()+"/output.dat", **run.materialDictForPlot(materialDict))
    _plt.savefig(materialDict["material"]+"_xSec.pdf")

if __name__ == "__main__":
    RunAnalysis()









