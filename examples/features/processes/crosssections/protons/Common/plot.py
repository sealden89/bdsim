import pylab as _pl
import numpy as _np


def compareHadronic(dir_path=".", pdg_tot_file = None, pdg_el_file = None, subtitle="") :
    _pl.clf()
    plot(dir_path+"/"+"01_hydrogen.dat", 100000,0.001, 8.96,  1.00,"r","H", pdg_tot_file, pdg_el_file);
    plot(dir_path+"/"+"02_carbon.dat",   100000,0.01 , 1.88, 12.00,"b","C");
    plot(dir_path+"/"+"03_aluminium.dat",100000,0.01 , 2.70, 26.98,"g","Al");
    plot(dir_path+"/"+"04_copper.dat",   100000,0.001, 8.96, 63.55,"c","Cu");
    plot(dir_path+"/"+"05_tungsten.dat", 100000,0.001,19.30,183.84,"m","W");
    plot(dir_path+"/"+"06_lead.dat",     100000,0.001,11.35,207.20,"y","Pb");

    _pl.suptitle(subtitle)
    _pl.xlim(0.1,1e7)
    _pl.legend(loc=0,prop={'size':14})

def compareModel() :
    #_pl.figure(1)
    _pl.clf()
    plot("01_qgsp_bert/qgsp_xsections/01_hydrogen.dat"   , 100000,0.001, 8.96,  1.00,"b","H QGSP\_BERT + ION");
    #plot("02_qgsp_bert_hp/01_hydrogen.dat", 100000,0.001, 8.96,  1.00,"g","H QGSP\_BERT\_HP");
    plot("03_ftfp_bert/ftfp_xsections/01_hydrogen.dat"   , 100000,0.001, 8.96,  1.00,"r","H FTFP\_BERT + ION");
    #plot("04_ftfp_bert_hp/01_hydrogen.dat", 100000,0.001, 8.96,  1.00,"b","H FTFP\_BERT\_HP");

    _pl.ylim(1,1e4)
    _pl.legend(loc=1,prop={'size':14})


def plot(filename = "hydrogen.dat", nTotPart = 100000,thickness = 0.001, densitygcm3 = 1.0, atomicweight = 1, lc = "b", leg = "Target", pdg_tot_file = None, pdg_el_file = None) :
    barn        = 1E-28
    mbarn       = barn*1E-3
    d           = _np.loadtxt(filename)
    eTot        = d[:,0]
    eBeam       = _np.sqrt(d[:,0]**2-0.938270**2)
    nElastic    = d[:,1]
    nInelastic  = d[:,2]
    nTotal      = nElastic+nInelastic

    ##########DEBUG###########
    print "Ebeam: "+str(eBeam)
    print "N elastic: "+str(nElastic)
    print "N total: "+str(nTotal)
    ##########################\
    amu             = 1.660539040E-27
    densitykgm3     = densitygcm3*1e-3/(0.01)**3
    numberdensitym3 = densitykgm3/(atomicweight*amu)
    numberdensitym2 = thickness*numberdensitym3
    eleXSec         = nElastic/nTotPart/numberdensitym2/mbarn
    totXSec         = nTotal/nTotPart/numberdensitym2/mbarn

    err_eleXSec     = _np.sqrt(nElastic)/nTotPart/numberdensitym2/mbarn
    err_totXSec     = _np.sqrt(nTotal)/nTotPart/numberdensitym2/mbarn

    print densitygcm3, densitykgm3, numberdensitym3, numberdensitym2


    #    _pl.clf()
    _pl.figure(1, figsize=(13, 8), dpi=80, facecolor='w', edgecolor='k')
    #_pl.errorbar(eBeam, totXSec, yerr=err_eleXSec, linestyle="-", color=lc, label=r"$\mathrm{"+leg+" \;\; tot}$")
    #_pl.errorbar(eBeam,eleXSec, yerr=err_totXSec, linestyle="--", color=lc,label=r"$\mathrm{"+leg +" \;\; el}$")

    _pl.plot(eBeam, totXSec, linestyle="-", color=lc, label=r"$\mathrm{"+leg+" \;\; tot}$")
    _pl.plot(eBeam, eleXSec, linestyle="--", color=lc,label=r"$\mathrm{"+leg +" \;\; el}$")

    if pdg_el_file :
        # elastic xsec
        pdgEle= loadPdgCrossSection(pdg_el_file)
        xerrs  = (pdgEle['PLAB_MIN'].astype(_np.float) - pdgEle['PLAB_MAX'].astype(_np.float))
        yerrs  = (pdgEle['STA_ERR-'].astype(_np.float),  pdgEle['STA_ERR+'].astype(_np.float))
        _pl.errorbar(pdgEle['PLAB(GEV/C)'].astype(_np.float),pdgEle['SIG(MB)'].astype(_np.float), xerr=xerrs,yerr=yerrs, fmt=".", color="b", label=r"$\mathrm{PDG \;\; el}$")

    if pdg_tot_file :
        # total xsec
        pdgTot = loadPdgCrossSection(pdg_tot_file)
        xerrs  = (pdgTot['PLAB_MIN'].astype(_np.float) - pdgTot['PLAB_MAX'].astype(_np.float))
        yerrs  = (pdgTot['STA_ERR-'].astype(_np.float),  pdgTot['STA_ERR+'].astype(_np.float))
        _pl.errorbar(pdgTot['PLAB(GEV/C)'].astype(_np.float),pdgTot['SIG(MB)'].astype(_np.float), xerr=xerrs,  yerr=yerrs, fmt=".",color="g", label=r"$\mathrm{PDG \;\; tot}$")

    _pl.gca().set_xscale('log')
    _pl.gca().set_yscale('log')
    _pl.grid(True,which="both");
    _pl.xlabel(r"$P_\mathrm{lab} \;\; [\mathrm{GeV}/{c}]$")
    _pl.ylabel(r"$\sigma \;\; [\mathrm{mb}]$")
    _pl.legend(loc=0)


def plotDifferential(plottype="energy", filename = "energy_hist.dat", nTotPart = 100000,thickness = 0.001, densitygcm3 = 1.0, atomicweight = 1, lc = "b", leg = "Energy", pdg_tot_file = None, pdg_el_file = None) :
    barn        = 1E-28
    mbarn       = barn*1E-3
    d           = _np.loadtxt(filename, unpack=True)
    hist        = d[0]
    bin_edges   = d[1]

    amu             = 1.660539040E-27
    densitykgm3     = densitygcm3*1e-3/(0.01)**3
    numberdensitym3 = densitykgm3/(atomicweight*amu)
    numberdensitym2 = thickness*numberdensitym3

    totCount     = 0
    bin_centres  = []
    diffXSec     = []
    err_diffXSec = []

    for i in range(len(bin_edges)-1):
        bin_l     = bin_edges[i]
        bin_h     = bin_edges[i+1]
        bin_w     = bin_h - bin_l
        bin_c     = bin_l + bin_w/2
        count     = hist[i]
        totCount += count 

        bin_centres.append(bin_c)
        dsigma     = count/nTotPart/numberdensitym2/mbarn/bin_w
        err_dsigma = _np.sqrt(count)/nTotPart/numberdensitym2/mbarn
        diffXSec.append(dsigma)
        err_diffXSec.append(err_dsigma)

    totXSec     = totCount/nTotPart/numberdensitym2/mbarn
    err_totXSec = _np.sqrt(totCount)/nTotPart/numberdensitym2/mbarn

    print densitygcm3, densitykgm3, numberdensitym3, numberdensitym2

    _pl.figure(len(plottype), figsize=(13, 8), dpi=80, facecolor='w', edgecolor='k') #make sure its new figure every time
    _pl.plot(bin_centres, diffXSec, linestyle="-", color=lc)#, label=r"$\mathrm{"+leg+"}$")
    #_pl.bar(bin_edges[:-1], diffXSec, 0.85*(bin_edges[1]-bin_edges[0]), color='b', ec='b',alpha=0.5)

    _configurePlot(plottype, totXSec)



def _configurePlot(plottype, totxsec):
    ax = _pl.gca()

    all_plottypes=["energy","angle","pseudorapidity"]
    if plottype=="energy":
        _pl.xlabel(r"$E \;\; [\mathrm{GeV}/{c}]$")
        _pl.ylabel(r"$\mathrm{d}\sigma/\mathrm{d}E \;\; [\mathrm{mb}/\mathrm{GeV}]$")
        ax.set_yscale("log")

    elif plottype=="angle":
        _pl.xlabel(r"$\Theta \;\; [\mathrm{r}]$")
        _pl.ylabel(r"$\mathrm{d}\sigma/\mathrm{d}\Theta \;\; [\mathrm{mb}/\mathrm{sr}]$")

    elif plottype=="pseudorapidity":
        _pl.xlabel(r"$\eta$")
        _pl.ylabel(r"$\mathrm{d}\sigma/\mathrm{d}\eta \;\; [\mathrm{mb}]$")

    else:
        print "No such plot type:", plottype
        for pt in all_plottypes: print pt,
        print

    _pl.plot(_np.NaN, _np.NaN, color='none', label = r"$\sigma_{tot} \;=\; $"+"{0:.2f}".format(totxsec)+r"$ \;mb$")
    _pl.grid(True,which="both")
    _pl.legend(loc=0)
    return

def loadPdgCrossSection(file) :
    import fortranformat as _ff

    f = open(file,'r')

    # load header text
    header = True
    headerText = [] 
    while(header) :
        l = f.readline()
        if l.find("---") != -1:
            header = False
        else :
            headerText.append(l.strip('\n\r '))

    # decode header
    nheader = len(headerText) 
    keys    = headerText[4].split()
    ffmt    = headerText[5][headerText[5].find('('):headerText[5].find(')')+1]
#    print nheader
#    print headerText
#    print keys
#    print ffmt

    # read preable
    preamble = {}
    for i in range(0,nheader-2,1) :
        l = f.readline().strip('\r\n ')
        preamble[headerText[i]] = l
#        print headerText[i],l
#    print preamble

    # read data table
    fr = _ff.FortranRecordReader(ffmt)
    data = []
    for i in range(0,int(preamble['NUMBER_OF_DATA_POINTS']),1) :
        data.append(fr.read(f.readline()))

    data = _np.array(data)
    dataDict = {}

    # format into dict
    for k in keys[0:-1] :     # FLAG not present
        idx = keys.index(k)
        dataDict[k] = data[:,idx]

    # close file
    f.close()

    return dataDict
