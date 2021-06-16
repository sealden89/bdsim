import matplotlib.pyplot as plt
import matplotlib.patches as patches
import gzip
import json
import pybdsim
import pymadx
import numpy as np

#plt.rcParams.update({"text.usetex": True,
#                     "font.size"  : 14})

plt.rcParams.update({"font.family": "serif"})


def PlotAll(filename='ipac-run123.root', outputname=None):
    filename= pybdsim.Data.Load(filename)
    
    PlotXYPlanes(filename, outputname, "run123_xy_free__r")
    vmin = [1e-9,1e-9,1e-10,1e-10,1e-8,1e-8]
    vmax = [1e-6,1e-6,1e-6,1e-6,1e-5,1e-5]
    PlotXYPlanes(filename, outputname, "run123_xy_fixed_r", vmin, vmax)
    
    PlotXZPlanes(filename, outputname, "origin_xz_free__r")

    PlotOriginInZ(filename, outputname, "origin_z")
    PlotOriginInZ(filename, outputname, "origin_z_rebin5", vmin=1e-7, vmax=3e-5, rebin=5)

def LoadCrossSectionLines(xy = "bmm-sp-xy-z500m-plane-outline.dat.gz", xz = "bmm-sp-xz-plane-outline.dat.gz"):
    with gzip.open(xy,'r') as f:
        xy = json.load(f)
    with gzip.open(xz,'r') as f:
        xz = json.load(f)
    return xy,xz

def ParticlesAndNames():
    particles = ['+12','-12','+13','-13','+14','-14']
    names     = [r'$\nu_e$',     r'$\overline{\nu}_e$',
                 r'$\mu^-$',     r'$\mu^+$',
                 r'$\nu_{\mu}$', r'$\overline{\nu}_{\mu}$']
    return particles,names

def ParticleFileNames(particles):
    return [p.rjust(3,'_')+"_" for p in particles] # file names

def PlotXYPlanes(filename, outputname=None, suffix="", vmin=None, vmax=None):
    d = pybdsim.Data.Load(filename) if type(filename) is str else filename
    #xy,xz = LoadCrossSectionLines()
    particles,names = ParticlesAndNames()
    pfnames = ParticleFileNames(particles)

    vmins = [vmin]*len(particles) if type(vmin) is not list else vmin
    vmaxs = [vmax]*len(particles) if type(vmax) is not list else vmax
    for particle,name,vmi,vma,pfn in zip(particles,names,vmins,vmaxs,pfnames):
        hname = 'Event/PerEntryHistograms/DetectorXY_' + particle
        pybdsim.Plot.Histogram2D(d.histograms2dpy[hname],
                                 logNorm=True,
                                 title=name,
                                 aspect='equal',
                                 xlabel='X (m)',
                                 ylabel='Y (m)',
                                 vmin=vmi,
                                 vmax=vma)
        #for i in range(len(xy[0])):
        #    plt.plot(xy[0][i],xy[1][i],c='k',lw=0.5)
        plt.xlim(-2,2)
        plt.ylim(-2,2)
        plt.tight_layout()
        if outputname is not None:
            plt.savefig(outputname+pfn+suffix+".png", dpi=500)

def PlotXZPlanes(filename, outputname=None, suffix="", vmin=None, vmax=None):
    d = pybdsim.Data.Load(filename) if type(filename) is str else filename
    xy,xz = LoadCrossSectionLines()
    particles,names = ParticlesAndNames()
    particles = particles[2:4]
    names     = names[2:4]
    pfnames = ParticleFileNames(particles)

    vmi = vmin if vmin is not None else 1e-9
    vma = vmax if vmax is not None else 1e-6
    vmins = [vmin]*len(particles) if type(vmin) is not list else vmin  # TBC
    vmaxs = [vmax]*len(particles) if type(vmax) is not list else vmax
    for particle,name,pfn in zip(particles,names,pfnames):
        hname = 'Event/PerEntryHistograms/OriginZX_' + particle
        h = d.histograms2dpy[hname]
        pixelArea = h.xwidths[0] * h.ywidths[0]
        zlabel = name + ' Flux / {} m$^2$ / 450 GeV proton'.format(np.round(pixelArea,3))
        pybdsim.Plot.Histogram2D(h,
                                 logNorm=True,
                                 title='Origin of '+name,
                                 xlabel='Z (m)',
                                 ylabel='X (m)',
                                 zlabel=zlabel,
                                 figsize=(12,5),
                                 vmin=vmi,
                                 vmax=vma)
        for i in range(len(xz[0])):
            plt.plot(xz[1][i],xz[0][i],c='k',lw=0.5)
        rect = patches.Rectangle((498,-2), 1.5, 4, color='r')
        ax = plt.gca()
        ax.add_patch(rect)
        plt.xlim(0,500)
        plt.ylim(-5,5)
        plt.tight_layout()
        plt.subplots_adjust(right=1.04)
        if outputname is not None:
            plt.savefig(outputname+pfn+suffix+".png", dpi=500)


def PlotOriginInZ(filename, outputname=None, suffix="", vmin=None, vmax=None, rebin=None):
    d = pybdsim.Data.Load(filename) if type(filename) is str else filename

    particles,names = ParticlesAndNames()

    base = 'Event/PerEntryHistograms/OriginZ_'
    hists = [d.histograms1dpy[base+k] for k in particles[2:4]]
    if rebin != None:
        hists = [pybdsim.Data.TH1(d.histograms1d[base+k].Rebin(rebin)) for k in particles[2:4]]
    
    pybdsim.Plot.Histogram1DMultiple(hists,
                                     names[2:4],
                                     log=True,
                                     xlabel='Z (m)',
                                     ylabel='Number / p-p collision @ 6.5 TeV / m')

    avmin = vmin if vmin is not None else 1e-8
    avmax = vmax if vmax is not None else 1e-5
    plt.ylim(avmin, avmax)
    a = pymadx.Data.Tfs("../../madx/ring_from_collimation.tfs.gz")
    a = a[:a.IndexFromNearestS(500)]
    pymadx.Plot.AddMachineLatticeToFigure(plt.gcf(), a)
    if outputname is not None:
        plt.savefig(outputname+"_origin_z_"+suffix+".pdf")
