import matplotlib.pyplot as _plt
import pybdsim
from OrderedDict import OrderedDict

def Spectra(filename, outputfilename='spectra', log=False):
    d = pybdsim.Data.Load(filename)

    keys = OrderedDict([("All",               "All"),
                        ("ProtonsPrimary",    "p (primary)"),
                        ("ProtonsSecondary",  "p (secondary)"),
                        ("Neutrons",          "n"),
                        ("PiPlusMinus",       "$\pi^{\pm}$"),
                        ("PiZero",            "$\pi^{0}$"),
                        ("Electrons",         "e$^{-}$"),
                        ("Positrons",         "e$^{+}$"),
                        ("Gammas",            "$\gamma$"),
                        ("Muons",             "$\mu^{\pm}$")])
    
    _plt.figure()
    extra = "Log" if log else ""
    for k,name in keys.iteritems():
        ho  = d.histograms1dpy["Event/PerEntryHistograms/Q2"+extra+k]
        h   = pybdsim.Data.PadHistogram1D(ho)
        _plt.errorbar(h.xcentres, h.contents, yerr=h.errors, drawstyle="steps-mid", label=name)

    binWidth = d.histogramspy["Event/PerEntryHistograms/Q2"+extra+"All"].xwidths[0]
        
    if log:
        _plt.xscale("log")
        _plt.ylabel("Number / Proton / d\,log(E) GeV")
        _plt.xlim(9,6700)
        _plt.ylim(1e-3,4)
    else:
        _plt.ylabel("Number / Proton / " + str(round(binWidth,0)) + " GeV")
        _plt.xlim(-50,6600)
        _plt.ylim(1e-3,1e3)

    _plt.xlabel('Total Particle Energy (GeV)')
    _plt.yscale('log', nonposy='clip')
    _plt.legend(fontsize="small")
    _plt.tight_layout()

    if not outputfilename.endswith(".pdf"):
        outputfilename += ".pdf"
    _plt.savefig(outputfilename)
