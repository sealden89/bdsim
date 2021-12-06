import matplotlib.pyplot as _plt
import pybdsim

def PlotSpectra(filename, outputfilename='spectra', xlog=False, ylog=True):
    d = pybdsim.Data.Load(filename)

    keyLabels = [("All",              "All"),
                 ("ProtonsPrimary",   "p (primary)"),
                 ("ProtonsSecondary", "p (secondary)"),
                 ("Neutrons",         "n"),
                 ("PiPlusMinus",      "$\pi^{\pm}$"),
                 ("PiZero",           "$\pi^{0}$"),
                 ("Electrons",        "e$^{-}$"),
                 ("Positrons",        "e$^{+}$"),
                 ("Gammas",           "$\gamma$"),
                 ("Muons",            "$\mu^{\pm}$")]

    logString = "Log" if xlog else ""
    histograms = [ d.histogramspy["Event/PerEntryHistograms/Q2"+logString+keyLabel[0] ] for keyLabel in keyLabels ]
    labels     = [kl[1] for kl in keyLabels]
    xlabel     = "Total Particle Energy (GeV)"
    ylabel     = "Number / Proton / "
    binWidth   = d.histogramspy["Event/PerEntryHistograms/Q2"+logString+"All"].xwidths[0]
    ylabel     += "dlog(E) GeV" if xlog else str(round(binWidth,0)) + " GeV"

    pybdsim.Plot.Histogram1DMultiple(histograms,
                                     labels,
                                     log=ylog,
                                     xlabel=xlabel,
                                     ylabel=ylabel,
                                     legendKwargs={'fontsize':'small'})
    
    if xlog:
        _plt.xlim(1e1,1e4)
        _plt.xscale('log')
    else:
        _plt.xlim(-10,6600)
        
    if not outputfilename.endswith(".pdf"):
        outputfilename += ".pdf"
    _plt.savefig(outputfilename)
