import pymadx

a = pymadx.Data.Aperture('ring_aperture.tfs.tar.gz')
b = a.RemoveBelowValue(0.005)
b.PlotN1(machine='ring.tfs.tar.gz', outputfilename="ring_n1.pdf")


o = pymadx.Data.Tfs("ring.tfs.tar.gz")
o.Plot(dispersion=True, outputfilename="ring_beta")
o.PlotSigma(outputfilename="ring_sigma.pdf")
