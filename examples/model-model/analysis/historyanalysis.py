import numpy as np
import pickle
import matplotlib.pyplot as plt

def Main(filename):
    hraw,hsorted,reduced = HistoryAnalysis(filename)
    Print2(reduced)

names = { 12   : r'$\nu_e$',
          -12  : r'$\overline{\nu}_e$',
          13   : r'$\mu^-$',
          -13  : r'$\mu^+$',
          14   : r'$\nu_{\mu}$',
          -14  : r'$\overline{\nu}_{\mu}$',
          22   : r'$\gamma$',
          111  : r'$\pi^0$',
          211  : r'$\pi^+$',
          -211 : r'$\pi^-$',
          130  : r'$K^0_L$',
          310  : r'$K^0_S$',
          311  : r'$K^0$',
          321  : r'$K^+$',
          -321 : r'$K^-$',
          2112 : r'$n$',
          2212 : r'$p$',
          3122 : r'$\Lambda$',
          3112 : r'$Sigma^-',
          3222 : r'$\Sigma^+$'}


def Name(pdg):
    """Default to just a string of the number if we don't find it."""
    return names.get(pdg,str(pdg))

def HistoryAnalysis(filename):
    """
    Returns raw histories, 
    """
    f = open(filename, "rb")
    histories = pickle.load(f)
    f.close()

    # histories is { tuple(pdg,pdg,pdg..) : [number, uncertainty] } 

    # sum up total number
    total = np.sum([v[0] for k,v in histories.items()])

    # python3 dictionary preserves order
    # sort by 'number' ie occurence of each history
    hs = sorted(histories.items(), key=lambda item: item[1][0], reverse=True)

    # categorise but final particle we see
    # create dict of { final pdg : { tuple(:final) : [number, uncertainty] } }
    reduced = {}
    for ht,rate in histories.items():
        pdgID = ht[0]
        if pdgID not in reduced:
            reduced[pdgID] = {}
        newhistory = ht[1:] if len(ht) > 1 else "primary"
        reduced[pdgID][newhistory] = rate

    # work out fractions for each final particle
    for pdg,hists in reduced.items():
        t = 0.0
        for h,rateError in hists.items():
            t += rateError[0]

        for h,rateError in hists.items():
            hists[h] = tuple(v/t for v in rateError)
        # sort the history for this particular final particle
        reduced[pdg] = sorted(hists.items(), key=lambda item: item[1][0], reverse=True)
    
    return histories,hs,reduced


def Print(history):

    for pdg,h in history.items():
        print("History of "+Name(pdg))
        for hist,rateError in history[pdg][:10]:
            re = [v*100.0 for v in rateError]
            v = "{:.2f} +- {:.2f} %".format(*re)
            hr = reversed(hist)
            n = " -> ".join(map(Name,hr)) if type(hist) is tuple else hist
            if n.count("primary") > 0:
                print(n," : ",Name(pdg)," ",v)
            else:
                print(n," -> ",Name(pdg)," ",v)

        print("\n")


def Print2(history):
    plt.figure(figsize=(4.3,7.3))
    yPerLine = 12
    yTotal = -20  # offset
    xl = 0
    indent = 5
    xr = 45
    fs = 'x-small'
    for pdg,h in history.items():
        plt.text(xl, yTotal, r"History of "+Name(pdg), fontsize=fs)
        yTotal -= 1.3*yPerLine
        for hist,rateError in history[pdg][:10]:
            re = [v*100.0 for v in rateError]
            v = "{:.2f} +- {:.2f} %".format(*re)
            hr = reversed(hist)
            n = " -> ".join(map(Name,hr)) if type(hist) is tuple else hist
            histstr = ""
            if n.count("primary") > 0:
                histstr = n + " : " + Name(pdg)
            else:
                histstr = n + " -> " + Name(pdg)
            #histstr = histstr.rjust(50)
            plt.text(xl+indent, yTotal, histstr, fontsize=fs)
            plt.text(xr, yTotal, str(v), fontsize=fs)
            yTotal -= yPerLine

    plt.xlim(-5,xr+20)
    plt.ylim(yTotal*1.1,0)

    frame1 = plt.gca()
    frame1.axes.get_xaxis().set_ticks([])
    frame1.axes.get_yaxis().set_ticks([])
    plt.tight_layout()
