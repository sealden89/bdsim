import numpy as _np
import multiprocessing as _mp
import threading as _threading
from subprocess import Popen as _Popen
from subprocess import call as _call
import matplotlib.pyplot as _plt
import datetime as _datetime
import os as _os
import os.path as _path
import shutil as _shutil
import glob as _glob

import pybdsim as _pybdsim
import pymadx as _pymadx


# Basis for optics comparisons script:
#
# 1) Run BDSIM for each model in separate threads. Run subsequent rebdsimOptics analysis in same thread.
#
# 2) Load and extract appropriate data for either BDSIM reference or TFS comparison.
#
# 3) Calculate value of some metric(s) for comparing optics.
#     If machine passes, delete output data.
#     If fails, store data in a new directory
#
# 4) Print comparison summary to terminal and log file.
#
# 5) Optionally produce comparison plots for each model.
#
# Notes:  BDSIM is run with Popen rather than using the pybdsim.Run classes. This is purely because the BDSIM
#           terminal output is written to log file for debugging. Otherwise, the only other options are A) devnull
#           where debugging becomes impossible, B) print terminal output which for asynchronous processes will not
#           be printed in order and debugging will be very difficult.
#         Comparison will be to gold standard reference data generated with BDSIM. Generating a gold standard
#            that agrees with Madx data is decided at group discretion.
#         With MAD comparisons, PSIG2 should be to PTC instead of TFS. The madx comparison has always been off so
#           most metrics will always be wrong.
#         Direct within error comparison is proving to not be a good metric. The results are still
#           returned but should be used cautiously.
#         Pool methods (apply_async etc) cannot plot graphs as the multiprocessing library cannot
#           stably interact with the CoreFoundation libraries. It's a known bug that's been fixed for python3,
#           but for now run the plotting comparison in separate method post data generation.

#### pre-defined optical function dictsfor plotting

_BETA =    {"bdsimdata"  : ("Beta_x", "Beta_y"),
            "bdsimerror" : ("Sigma_Beta_x","Sigma_Beta_y"),
            "mad"        : ("BETX", "BETY"),
            "title"      : "Beta"
            }

_ALPHA =   {"bdsimdata"  : ("Alpha_x", "Alpha_y"),
            "bdsimerror" : ("Sigma_Alpha_x","Sigma_Alpha_y"),
            "mad"        : ("ALFX", "ALFY"),
            "title"      : "Alpha"
           }

_DISP  =   {"bdsimdata"  : ("Disp_x", "Disp_y"),
            "bdsimerror" : ("Sigma_Disp_x","Sigma_Disp_y"),
            "mad"        : ("DX", "DY"),
            "title"      : "Dispersion"
            }

_SIGMA =   {"bdsimdata"  : ("Sigma_x", "Sigma_y"),
            "bdsimerror" : ("Sigma_Sigma_x","Sigma_Sigma_y"),
            "mad"        : ("SIGMAX", "SIGMAY"),
            "title"      : "Sigma"
            }

_SIGMA_P = {"bdsimdata"  : ("Sigma_xp", "Sigma_yp"),
            "bdsimerror" : ("Sigma_Sigma_xp","Sigma_Sigma_yp"),
            "mad"        : ("SIGMAXP", "SIGMAYP"),
            "title"      : "SigmaP"
            }

_MEAN    = {"bdsimdata"  : ("Mean_x", "Mean_y"),
            "bdsimerror" : ("Sigma_Mean_x","Sigma_Mean_y"),
            "mad"        : ("X", "Y"),
            "title"      : "Mean"
            }


#### pre-defined machine dicts. Add dict to list below for machine to be compared.

_diamond = {"name"      : "diamond",
            "gmadfile"  : "diamond/bdsim/diamond.gmad",
            "reference" : "diamond/diamond_optics_standard.root",
            "tfs"       : "diamond/madx/twiss_madx.tfs.gz",
            "ngenerate" : 1000,
            "circular"  : True,
            "madisptc"  : False,
            "timeout"   : 500
            }

_lhc     = {"name"      : "lhc",
            "gmadfile"  : "lhc/bdsim/lhc.gmad",
            "reference" : "lhc/lhc_optics_standard.root",
            "tfs"       : "lhc/twiss_lhcb1.tfs.gz",
            "ngenerate" : 500,
            "circular"  : True,
            "madisptc"  : False,
            "timeout"   : 1500
            }

_atf2    = {"name"      : "atf2",
            "gmadfile"  : "atf2/bdsim/atf2.gmad",
            "reference" : "atf2/atf2_optics_standard.root",
            "tfs"       : "atf2/twiss_v5.2.tfs.gz",
            "ngenerate" : 1000,
            "circular"  : False,
            "madisptc"  : False,
            "timeout"   : 500
            }

_psig2   = {"name"      : "psig2",
            "gmadfile"  : "psig2/bdsim/psig2.gmad",
            "reference" : "psig2/psig2_optics_standard.root",
            "tfs"       : "psig2/ptc/ptc_optics.root",
            "ngenerate" : 1000,
            "circular"  : False,
            "madisptc"  : True,
            "timeout"   : 500
            }

# list of predefined machine dicts that will be compared.
_machineDicts = [_psig2, _diamond, _lhc, _atf2]


class _Machine(dict):
    """ Data container class for machine information. Copies data from supplied machine dict
        and create all appropriate filenames/paths entries."""
    def __init__(self, machine, isMadComparison=False, datename=""):
        dict.__init__(self)
        self["isMadComparison"] = isMadComparison

        for key in machine.keys():
            self[key] = machine[key]
        self["fullFilename"] = _path.basename(machine["gmadfile"])
        filename = _path.splitext(self["fullFilename"])[0]

        self["filename"]           = filename
        self["rebdsimOpticsFile"]  = filename + "_optics.root"
        self["bdsimOutfile"]       = filename
        self["outfileFullName"]    = filename + ".root"
        self["bdsimLogFile"]       = filename + "_bdsim.log"
        self["rebdsimLogFile"]     = filename + "_rebdsim.log"
        self["opticsFileName"]     = filename + "_" + datename + "_optics.pdf"

    def __repr__(self):
        s = "OpticsTesting._Machine instance."
        s += "Data container for " + self["name"]
        return s

    def AddResults(self, results):
        self["resultsData"] = results[0]
        self["matches"] = results[1]


class Compare:
    def __init__(self, isMadComparison=False, plotOptics=False, loadExisting=False, silent=False):
        """
        A class for optics regression testing. Upon instantiation it will automatically generate a new bdsim
        and rebdsim optics output file for each machine in the predefined list. These will then be compared
        to a reference data set.

        Results are automatically generated and printed. For any machine that fails the comparison, the data
        will be copied into a timestamped directory. For machines that pass, the data is deleted. All terminal
        output is recorded in a log file.

        Example:

        >>> a = Compare()

        Optional:
        isMadComparison: Compare the new data set to supplied MADX output files instead of reference data set.
                         Default = False.
        plotOptics: Plot the new optics against the reference data set. Will plot against supplied MADX files
                    if isMadComparison is True. Default = False.
        loadExisting: Load existing data set in current directory. Should only be used for debuggin in event
                      of python exception during analysis. Default = False.
        silent: Do not print analysis results to terminal (still written to log file). Default = False.

                plotOptics      : Plots optics comparison using pybdsim.Compare classes. Default = False.
        isMadComparison : Compare BDSIM data set to TFS data for each machine. Default = False.
        loadExisting    : Load existing data set. Loads recently generated set in case of analysis failure.
                          Requires machine_optics.root files to be present in working directory. Default = False.
        silent          : No printout to terminal. Log files still written.
        """

        # timestamped filename for optics plots.
        self._opticsLogDate = _datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

        # timestamped filename for comparison log.
        self._compLogDate = _datetime.datetime.now().strftime("%Y%m%d_%H%M%S") + ".log"

        # timestamped directory name for failed comparisons.
        self._failDir = _datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

        # create list of _Machine instances which contain all necessary filename data.
        self.machines = [_Machine(machine, isMadComparison, datename=self._opticsLogDate) for machine in _machineDicts]

        # run the comparison and get the results
        self.results = _Compare(self.machines, loadExisting)

        # add results to machine
        for index, result in enumerate(self.results):
            self.machines[index].AddResults(result)

        # check all machine rebdsim output files exist
        _CheckAllRebdsimFilesExists(self.machines)

        # optics plotting comparison and print analysis results
        for machine in self.machines:
            _PrintComparisonResults(machine, silent, logFile=self._compLogDate)
            _PlotOpticsComparisons(machine, plotOptics)

        _Clean(self.machines, self._failDir)

    def __repr__(self):
        s = "OpticsTesting.Compare instance\n"
        s += "Optics being tested for the following machines: \n"
        s += " "
        for Machine in self.machines:
            s += Machine["name"] + " "
        s += "\n"
        return s

    def PlotOpticsComparisons(self, machine="", isMadComparison=False):
        """ Plot optics comparisons for all machines.
            BDSIM data must exist in main directory. Else, either regenerate data or copied from failed dir to main.
            Optional:
            machine: will plot comparison for only that machine. Must match one of the machines ["name"]. Default = "".
            isMadComparison: plot BDSIM optics against supplied MADX optics. Default = False.
            """

        if machine == "":  # plot for all machines
            # check all machine rebdsim output files exist
            _CheckAllRebdsimFilesExists(self.machines)
            for Machine in self.machines:
                if isMadComparison:
                    Machine["isMadComparison"] = True
                _PlotOpticsComparisons(Machine, plotOptics=True)
        else:
            for Machine in self.machines:
                if Machine["name"] == machine:
                    if _CheckRebdsimFileExists(Machine):
                        if isMadComparison:
                            Machine["isMadComparison"] = True
                        _PlotOpticsComparisons(Machine, plotOptics=True)
                    else:
                        raise IOError("Rebdsim file for " + Machine["name"] + "not found.")

    def PlotMadOpticsComparisons(self, machine=""):
        """ Plot madx optics comparisons for all machines.
            BDSIM data must exist in main directory. Else, either regenerate data or copied from failed dir to main.
            Optional:
            machine: will plot comparison for only that machine. Must match one of the machines ["name"]. Default = "".
            """
        self.PlotOpticsComparisons(machine, isMadComparison=True)

    def PrintComparisonResults(self, silent=False):
        """ Reprint the comparison results in a formatted table. """
        for machine in self.machines:
            _PrintComparisonResults(machine, silent, logFile=self._compLogDate)

    def RegenerateData(self):
        """ Regenerate the BDSIM output. """
        for machine in self.machines:
            _Generate(machine)

    def ReanalyseData(self):
        """ Reanalyse data. Will overwrite existing results."""
        _CheckAllRebdsimFilesExists(self.machines)
        for index, machine in enumerate(self.machines):
            self.machines[index].AddResults(_Analyse(machine))

    def Clean(self):
        """ Remove all passed data and associated files from main directory if regenerated."""
        _Clean(self.machines, self._failDir)


def _Compare(machines, loadExisting=False):
    """ Runs machine optics comparisons in parallel threads.

        loadExisting    : Load existing data set. Loads recently generated set in case of analysis failure.
                          Requires machine optics root files to be present in working directory. Default = False.
        """
    # new pool limited by number of cpu threads available
    pool = _mp.Pool(_mp.cpu_count())

    # run machines asyncronously. No need to runs machines in order and no communication between threads needed.
    # dump results to dummy callback method as alternative way to store data. Can be removed if unneeded.
    # res = [pool.apply_async(_GenerateAndAnalyse, args=(machine,loadExisting,), callback=_CollectResult) for machine in machines]
    res = [pool.apply_async(_GenerateAndAnalyse, args=(machine, loadExisting,)) for machine in machines]

    # Get results locally. Each entry in results is of the type (data,boolean) which are the analysis data
    # and a boolean of if the new data matches the reference data respectively.
    results = [p.get() for p in res]

    pool.close()
    pool.join()  # postpones the execution of next line of code until all processes in the queue are done.

    return results

def _CollectResult(result):
    pass

def _Clean(machines, failDir=None):
    """ Delete generated files if the data matches. Move into a new timestamped directory if it doesn't match."""

    # change failed directory name if it already exists
    if failDir is None:
        failDir = _datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

    # all current files
    dirFiles = _glob.glob('*')
    filesMoved = False

    if failDir not in dirFiles:
        _os.mkdir(failDir)

    for index, machine in enumerate(machines):
        matches = machine["matches"]

        files = [machine["rebdsimOpticsFile"],
                 machine["outfileFullName"],
                 machine["bdsimLogFile"],
                 machine["rebdsimLogFile"],
                 machine["opticsFileName"]]

        # check if file exists, delete if passed, move if failed.
        for file in files:
            if file in dirFiles:
                if matches:
                    _os.remove(file)
                else:
                    _shutil.move(file, failDir)
                    filesMoved = True

    # delete failed directory if nothing was moved into it.
    if not filesMoved:
        _shutil.rmtree(failDir)

def _CheckRebdsimFileExists(machine):
    """ Check if the rebdsim output exists in the current directory for a given machine."""
    files = _glob.glob('*')
    if not machine["rebdsimOpticsFile"] in files:
        return False
    return True

def _CheckAllRebdsimFilesExists(machines):
    """ Check if the rebdsim output exists in the current directory for all machines."""
    # list of machine names whose rebdsim files don't exist in main dir
    missing = [machine["name"] for machine in machines if not _CheckRebdsimFileExists(machine)]

    if len(missing) > 0:
        s = "No Rebdsim file found for "
        for machine in missing:
            s += machine + " "
        s += "\n"
        s += "Please regenerate data or check failed directory in case of comparison failure."
        raise IOError(s)
    else:
        pass

def _PlotOpticsComparisons(machine, plotOptics=False):
    """ Plot optics comparison of results and reference optics using pybdsim/pymadx."""
    if not machine["matches"]:
        plotOptics = True

    if plotOptics:
        opticsFileName = machine["opticsFileName"]

        bdsdata = _pybdsim.Data.Load(machine["rebdsimOpticsFile"]).optics

        if machine["isMadComparison"]:
            tfs = machine["tfs"]
            if not machine["madisptc"]:
                tfsdata = _pymadx.Data.Tfs(tfs)
                _pybdsim.Compare.MadxVsBDSIM(tfsdata, bdsdata, outputFileName=opticsFileName)
            else:
                tfsdata = _pybdsim.Data.Load(tfs).optics
                _pybdsim.Compare.PTCVsBDSIM(tfsdata, bdsdata, outputFileName=opticsFileName)
        else:
            referenceData = _pybdsim.Data.Load(machine["reference"]).optics
            _pybdsim.Compare.BDSIMVsBDSIM(referenceData, bdsdata, outputFileName=opticsFileName)
        _plt.close('all')
        print("")  # blank line between machines.

def _PrintComparisonResults(machine, silent=False, logFile=""):
    """ Print the comparison results in a formatted table. """
    # set log file name
    if logFile == "":
        logFile = _datetime.datetime.now().strftime("%Y%m%d_%H%M") + ".log"

    def _Write(s, silent=False):
        """ writes string to terminal and log file."""
        if not silent:
            print(s)
        f = open(logFile, 'a')
        f.write(s + "\n")
        f.close()

    _Write("Summary for " + machine["name"] + ":\n", silent)

    key = "  Key: PSM = Percentage of samplers matching within error\n" + \
          "       ARR = Average relative residuals (BDSIM - REFERENCE)\n" + \
          "       LSM = Last sampler matches\n"
    _Write(key, silent)

    # unpack results
    data = machine["resultsData"]
    machineMatches = machine["matches"]

    # keep record of last sampler mismatches
    lastSamplerMatches = []

    # loop over all optical functions
    for opticalFunction in data:
        opf             = opticalFunction["opticalfunction"]
        percentMatching = '%.2f' %opticalFunction["percentMatchingSamplers"]
        averageRelResid = '%.3e' %opticalFunction["relativeresidualMean"]
        stdevRelResid   = '%.3e' %opticalFunction["relativeresidualSTD"]
        samplerMatches  = _np.str(opticalFunction["finalSamplerMatches"])
        lastSamplerMatches.append(opticalFunction["finalSamplerMatches"])

        # note sci notation above but right aligned N-char long format string below (10 long as string to allow
        # possible minus sign). Combining the two formats below doesn't work for unknown reasons.
        strformat = '{:12}: PSM: {:<7} | ARR: {:>10} +/- {:>10} | LSM: {:<5} '
        _Write(strformat.format(opf, percentMatching, averageRelResid, stdevRelResid, samplerMatches), silent)
    _Write("\n", silent)

    if False in lastSamplerMatches:
        _Write("Final Sampler Mismatch Data: ")

    # only print out the last sampler data if there is a mismatch
    for opticalFunction in data:
        opf = opticalFunction["opticalfunction"]
        samplerMatches = _np.str(opticalFunction["finalSamplerMatches"])
        if samplerMatches == "False":
            samplerData  = opticalFunction["finalSamplerData"]
            samplerMean  = "%.3e" % samplerData[0]
            samplerError = "%.3e" % samplerData[1]
            referenceMean = "%.3e" % samplerData[2]

            if not machine["isMadComparison"]:
                finalReferenceError = "%.3e" % samplerData[3]  # unpack here as entry is None for TFS data
                strformat = '{:12}: {:>10} +/- {:>10} (Reference Data: {:>10} +/- {:>10})'
                _Write(strformat.format(opf, referenceMean, finalReferenceError, samplerMean, samplerError), silent)
            else:
                strformat = '{:12}: {:>10} +/- {:>10} (Reference Data: {:>10})'
                _Write(strformat.format(opf, referenceMean, samplerMean, samplerError), silent)

    if machineMatches:
        s = "Pass"
    else:
        s = "Fail"
    _Write("")
    _Write(machine["name"] + " optics match: " + s + "\n")
    _Write("\n", silent)

def _GenerateAndAnalyse(machine, loadExisting=False):
    if not loadExisting:
        _Generate(machine)
    return _Analyse(machine)

def _Generate(machine):
    """ Generate BDSIM output, generate rebdsimOptics output, and run comparison."""

    # bdsim command to be run
    bdsimCommand =  ["bdsim",
                     "--file=" + machine["gmadfile"],
                     "--outfile="+machine["bdsimOutfile"],
                     "--ngenerate="+_np.str(machine["ngenerate"]),
                     "--batch",
                     "--seed=2019"]

    # add circular flag optionally
    if machine["circular"]:
        bdsimCommand.append("--circular")

    # Start the BDSIM process
    process = _Popen(bdsimCommand,
                     stdout=open(machine["bdsimLogFile"], 'a'),
                     stderr=open(machine["bdsimLogFile"], 'a'))

    # Method of communicating with BDSIM process. Start and apply the timeout via joining
    processThread = _threading.Thread(target=process.communicate)
    processThread.start()
    processThread.join(machine["timeout"])

    # After the timeout, if the process is still alive, kill it.
    if processThread.is_alive():
        process.kill()

    # run rebdsimOptics and write output to log file.
    f = open(machine["rebdsimLogFile"], 'a')
    rebdsim = _call(args=["rebdsimOptics", machine["outfileFullName"], machine["rebdsimOpticsFile"]], stdout=f)
    f.close()

def _Analyse(machine):
    # get new bdsim optics data
    bdsdata = _pybdsim.Data.Load(machine["rebdsimOpticsFile"]).optics

    # compare to appropriate data set.
    if machine["isMadComparison"]:
        if not machine["madisptc"]:
            tfsdata = _pymadx.Data.Tfs(machine["tfs"])
        else:
            tfsdata = _pybdsim.Data.Load(machine["tfs"]).optics

        betax, betay = _CompareMadParameter(_BETA,  machine, tfsdata, bdsdata)
        alfax, alfay = _CompareMadParameter(_ALPHA, machine, tfsdata, bdsdata)
        dispx, dispy = _CompareMadParameter(_DISP,  machine, tfsdata, bdsdata)
        sigx, sigy   = _CompareMadParameter(_SIGMA, machine, tfsdata, bdsdata)
        sigxp, sigyp = _CompareMadParameter(_SIGMA_P, machine, tfsdata, bdsdata)
        meanx, meany = _CompareMadParameter(_MEAN,  machine, tfsdata, bdsdata)
    else:
        referencedata = _pybdsim.Data.Load(machine["reference"]).optics
        betax, betay = _CompareBdsimParameter(_BETA,  referencedata, bdsdata)
        alfax, alfay = _CompareBdsimParameter(_ALPHA, referencedata, bdsdata)
        dispx, dispy = _CompareBdsimParameter(_DISP,  referencedata, bdsdata)
        sigx,  sigy  = _CompareBdsimParameter(_SIGMA, referencedata, bdsdata)
        sigxp, sigyp = _CompareBdsimParameter(_SIGMA_P, referencedata, bdsdata)
        meanx, meany = _CompareBdsimParameter(_MEAN,  referencedata, bdsdata)
    data = (betax, betay, alfax, alfay, dispx, dispy, sigx, sigy, sigxp, sigyp, meanx, meany)

    # calculate percentage of all matching sampler across all optical functions
    total = 0
    numLastMatching = 0
    for opt in data:
        total += opt["percentMatchingSamplers"]
        if opt["finalSamplerMatches"]:
            numLastMatching += 1
    pcTotal = total / (len(data) * 100)

    matches = False
    if numLastMatching == len(data):
        matches = True
    elif pcTotal > 95.0:
        matches = True
    return (data, matches)

def _CompareBdsimParameter(optFuncDict, referenceData, bdsimData):
    """ Compare BDSIM data to reference data. Separate comparison for both axes.
        Returns (xdata, ydata). """
    bdsimXData     = _GetBdsimLikeData(optFuncDict, bdsimData, axis='x')
    referenceXData = _GetBdsimLikeData(optFuncDict, referenceData, axis='x')
    bdsimXData.extend(referenceXData)

    bdsimYData     = _GetBdsimLikeData(optFuncDict, bdsimData, axis='y')
    referenceYData = _GetBdsimLikeData(optFuncDict, referenceData, axis='y')
    bdsimYData.extend(referenceYData)

    # supplied data should be a len-4 list of (bdsim data, bdsim error, reference data, reference error)
    xresults = _CompareData(optFuncDict, bdsimXData, 'x')
    yresults = _CompareData(optFuncDict, bdsimYData, 'y')
    return xresults, yresults

def _CompareMadParameter(optFuncDict, machine, madData, bdsimData):
    """ Compare BDSIM data to TFS/PTC data. Separate comparison for both axes.
        Returns (xdata, ydata). """
    isPtcComparison = False
    if machine["madisptc"]:
        isPtcComparison = True

    axesResults = []
    for axis in ('x', 'y'):
        bdsimdata = _GetBdsimLikeData(optFuncDict, bdsimData, axis=axis)
        if isPtcComparison:
            # ptc data was created using ptc2bdsim and therefore is like a bdsim output file.
            data = _GetBdsimLikeData(optFuncDict, madData, axis=axis, referenceData=bdsimData)
        else:
            axisNum = 0  # assume x
            if axis == 'y':
                axisNum = 1

            tfsname = optFuncDict['mad'][axisNum]

            # machines will most unlikely be the same length as madx model will contain a start & end marker,
            # plus markers & some other thin elements are omitted during conversion to gmad, in which case
            # extract appropriate indices from TFS to ensure data is same length
            tfsindices = [madData.IndexFromNearestS(i) for i in bdsimData.S()]
            tfsparamdata = _np.array([madData[tfsindices[index]][tfsname] for index in range(len(bdsimData))])

            # append none as 2nd entry should be mad error which doesn't exist for tfs.
            data = [tfsparamdata, None]
        bdsimdata.extend(data)

        axesResults.append(_CompareData(optFuncDict, bdsimdata, axis))
    return axesResults

def _GetBdsimLikeData(optFuncDict, bdsdata, axis='x', referenceData=None):
    """ Extract appropriate data from BDSIM-like data set. """
    axisNum = 0  # assume x
    if axis == 'y':
        axisNum = 1

    # data to be looped over. This is included as PTC files are also bdsim-like however the
    # looping should be over BDSIM samplers for matching as PTC may contain bonus markers.
    if referenceData is None:
        referenceData = bdsdata

    bdsname = optFuncDict['bdsimdata'][axisNum]
    bdserrorname = optFuncDict['bdsimerror'][axisNum]

    bdsparamdata = []
    bdserrordata = []
    for index in range(len(referenceData)):
        bdselementdata = bdsdata[index]
        bdsparamdata.append(bdselementdata[bdsname])
        bdserrordata.append(bdselementdata[bdserrorname])

    bdsparamdata = _np.array(bdsparamdata)
    bdserrordata = _np.array(bdserrordata)
    return [bdsparamdata, bdserrordata]

def _CompareData(optFuncDict, data, axis='x'):
    """ Compare BDSIM optical function data to reference data. Supplied data must be a len-4 list
        in the order [bdsim data, bdsim error, reference data, reference error/None]."""

    # run analyses on data
    nTrue, nFalse, pcInError = _WithinError(data)
    meanrel, stdres          = _RelativeResiduals(data[0], data[2])
    finalSamplerMatches      = _FinalSamplerMatches(data)
    finalSamplerData         = _GetFinalSamplerData(data)

    # data container for all analysis results.
    results = {'opticalfunction'          : optFuncDict["title"]+axis,
               'numMatchingSamplersTrue'  : nTrue,
               'numMatchingSamplersFalse' : nFalse,
               'percentMatchingSamplers'  : pcInError,
               'relativeresidualMean'     : meanrel,
               'relativeresidualSTD'      : stdres,
               'finalSamplerMatches'      : finalSamplerMatches,
               'finalSamplerData'         : finalSamplerData
               }
    return results

def _GetFinalSamplerData(data):
    # extract data
    bdsimData  = data[0]
    bdsimError = data[1]
    referenceData  = data[2]
    referenceError = data[3]

    # get all final sampler values
    referenceErr = None
    if referenceError is not None:
        referenceErr = referenceError[-1]
    finalSampler = (bdsimData[-1], bdsimError[-1], referenceData[-1], referenceErr)
    return finalSampler

def _FinalSamplerMatches(data):
    """ Calculate if final sampler data in the new BDSIM output matches the reference data.
        Data must be supplied as a len-4 list: [bdsim data, bdsim error, reference data, reference error/None].
        The reference errors may be None as this may be for a TFS comparison where there is no error.
        Returns matches boolean. """

    bdsimData  = data[0]
    bdsimError = data[1]
    referenceData  = data[2]
    referenceError = data[3]

    # extract final sample data (not reference error, may be None and so can't be indexed).
    observed = bdsimData[-1]
    obserror = bdsimError[-1]
    expected = referenceData[-1]

    if referenceError is None:
        matches = _Inbounds(expected, observed + obserror, observed - obserror)
    else:
        experror = referenceError[-1]
        if observed > expected:
            matches = _Inbounds(expected + experror, observed + obserror, observed - obserror)
        elif observed < expected:
            matches = _Inbounds(expected - experror, observed + obserror, observed - obserror)
        else:  # identical
            matches = True

    return matches

def _Inbounds(expected, observedUpper, observedLower):
    """ Check if expected value falls in error bounds of observed value (BDSIM data)."""
    if (expected  < observedUpper) and (expected > observedLower):
        return True
    else:
        return False

def _MatchesAtEnd(data, errors):
    """ Check if the start and end values of a machine match. May be useful for circular machines
        but use cautiously. """
    start      = data[0]
    startError = errors[0]
    end        = data[-1]
    endError   = errors[-1]

    if start > end:
        return _Inbounds(end + endError, start + startError, start - startError)
    elif start < end:
        return _Inbounds(end - endError, start + startError, start - startError)
    else:  # identical
        return True

def _RelativeResiduals(bdsimData ,referenceData):
    """ Calculate relative residuals."""

    residuals = bdsimData - referenceData
    relResid  = residuals / referenceData
    meanrel   = _np.average(relResid)
    stdres    = _np.std(relResid)
    return meanrel, stdres

def _WithinError(data):
    """ Compares set of new data and checks if the points matches the mad data within error.
        Data must be supplied as a len-4 list: [bdsim data, bdsim error, reference data, reference error/None].
        Mad value can be from PTC which will have an associated error so include in matching. """

    # extract data
    bdsimData = data[0]
    bdsimError = data[1]
    referenceData = data[2]
    referenceError = data[3]

    # get true/false for each sampler.
    inError = []
    for index, observed in enumerate(bdsimData):
        expected = referenceData[index]
        obserror = bdsimError[index]
        if referenceError is None:
            inError.append(_Inbounds(expected, observed+obserror, observed-obserror))
        else:
            experror = referenceError[index]
            if observed > expected:
                inError.append(_Inbounds(expected+experror, observed+obserror, observed-obserror))
            elif observed < expected:
                inError.append(_Inbounds(expected-experror, observed+obserror, observed-obserror))
            else:  # identical
                inError.append(True)

    # get number of true/false matching samplers and percentage matching.
    uniquea, countsa = _np.unique(_np.array(inError), return_counts=True)

    if len(uniquea) == 1:  # either all true or false
        if not uniquea[0]:
            numFalse = countsa[0]
            numTrue = 0
        else:
            numTrue = countsa[0]
            numFalse = 0
    else:
        if not uniquea[0]:  # if first entry is false
            numFalse = countsa[0]
            numTrue = countsa[1]
        else:
            numFalse = countsa[1]
            numTrue = countsa[0]
    percentInError = _np.float(numTrue)/(_np.float(numTrue)+_np.float(numFalse)) * 100

    return numTrue, numFalse, percentInError
