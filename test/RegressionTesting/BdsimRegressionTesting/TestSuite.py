import numpy as _np
import os as _os
import glob as _glob
import subprocess as _sub
import multiprocessing
import time
from subprocess import Popen
import threading
import pickle

import _General
import Globals
import PhaseSpace
import Results
import Test
import Writer


GlobalData = Globals.Globals()

# result utility functions for checking output log files.
# Needed by Run function, so instantiate here.
ResultUtils = Results.ResultsUtilities()


def _runBDSIM(inputDict, timeout):

    # Start the BDSIM process
    process = Popen([GlobalData._bdsimExecutable,
                    "--file=" + inputDict['testFile'],
                    "--output=rootevent",
                    "--outfile="+inputDict['outputFile'],
                    "--batch",
                    "--seed="+_np.str(GlobalData.seed)],
                    stdout=open(inputDict['bdsimLogFile'], 'a'),
                    stderr=open(inputDict['bdsimLogFile'], 'a'))

    # Method of communicating with BDSIM process. Start and apply the timeout via joining
    processThread = threading.Thread(target=process.communicate)
    processThread.start()
    processThread.join(timeout)

    # After the timeout, if the process is still alive, kill it.
    if processThread.is_alive():
        process.kill()

    # get process return code. If negative, the process was killed. If positive, it was successful.
    # If None, it means the process is still 'ongoing'.
    returnCode = process.returncode

    # return negative if returnCode is None, its likely that there's a small time delay between
    # the kill signal and the program ending.
    if returnCode is None:
        return -1
    else:
        return returnCode


def Run(inputDict):
    """ Generate the rootevent output for a given gmad file.
        """
    selfCompare = inputDict['selfCompare']
    generateOriginals = inputDict['generateOriginals']

    t = time.time()

    # run bdsim and get return code
    processReturnCode = _runBDSIM(inputDict, timeout=GlobalData.timeout)

    files = _glob.glob('*.root')

    # set code for timeout. Delete root file if it exists, is likely to be zombie.
    if processReturnCode < 0:
        inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('TIMEOUT')
        inputDict['generalStatus'] = [GlobalData.ReturnsAndErrors.GetCode('TIMEOUT')]
        if inputDict['ROOTFile'] in files:
            _os.remove(inputDict['ROOTFile'])

    bdsimTime = _np.float(time.time() - t)
    inputDict['bdsimTime'] = bdsimTime
    originalFile = inputDict['originalFile']

    # check if the file exists - immediately determines if there was a generation error
    if not inputDict['ROOTFile'] in files:
        if inputDict['code'] != GlobalData.ReturnsAndErrors.GetCode('TIMEOUT'):
            inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('FILE_NOT_FOUND')  # False
    # if comparing
    elif not generateOriginals:
        with open(inputDict['compLogFile'], 'w') as outputLog:  # temp log file for the comparator output.

            # Only compare if the output was generated.
            if selfCompare:
                originalFile = inputDict['ROOTFile'].split('.root')[0] + '2.root'
                copyString = 'cp ' + inputDict['ROOTFile'] + ' ' + originalFile
                _os.system(copyString)
                if inputDict['originalFile'] == '':
                    inputDict["originalFile"] = originalFile
            else:
                originalFile = inputDict['originalFile']

            # run comparator and record comparator time.
            t = time.time()
            outputLog.write('\r\n')
            TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile, inputDict['ROOTFile']], stdout=outputLog)
            outputLog.close()
            ctime = time.time() - t
            inputDict['compTime'] = ctime

            if TestResult == 0:
                inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('SUCCESS')  # True
            elif TestResult != 0:
                inputDict['code'] = TestResult  # not passed comparator return code
    elif generateOriginals:
        # else root output was generated otherwise it would've been caught before, therefore success
        inputDict['code'] = GlobalData.ReturnsAndErrors.GetCode('SUCCESS')

    generalStatus = _General.GetBDSIMLogData(inputDict)

    inputDict['generalStatus'] = generalStatus

    # list of soft failure code in the general status.
    hasSofts = [code for code in GlobalData.ReturnsAndErrors.GetSoftCodeNumbers() if code in generalStatus]

    # if the comparator passed
    if inputDict['code'] == 0:
        # if only one general status entry then it must be 0 (passed), in which case,
        # delete all log and root files
        if len(generalStatus) == 1:
            if selfCompare:
                _os.remove(inputDict['originalFile'])
            if not generateOriginals:
                _os.remove(inputDict['compLogFile'])
                _os.remove(inputDict['ROOTFile'])
                _os.remove(inputDict['bdsimLogFile'])

        # else if the general status contains one of the 'soft' failures, move the bdsim log
        # and root output into the failed dir, and delete the passed comparator log.
        elif len(hasSofts) > 0:
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
            _os.system("mv " + inputDict['ROOTFile'] + " FailedTests/" + inputDict['ROOTFile'])
            if selfCompare:
                _os.remove(inputDict['originalFile'])
            if not generateOriginals:
                _os.remove(inputDict['compLogFile'])

    # elif the comparator failed
    elif inputDict['code'] == 1:
        # move the comparator log and failed root file
        _os.system("mv " + inputDict['ROOTFile'] + " FailedTests/" + inputDict['ROOTFile'])
        if selfCompare:
            _os.remove(inputDict['originalFile'])
        if not generateOriginals:
            _os.system("mv " + inputDict['compLogFile'] + " FailedTests/" + inputDict['compLogFile'])

        # if the general status contains one of the 'soft' failures, move the bdsim log, otherwise delete.
        if len(hasSofts) > 0:
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
        elif len(generalStatus) == 1:
            _os.remove(inputDict['bdsimLogFile'])

    # elif the comparator couldn't find one of the files
    elif inputDict['code'] == 2:
        # if the original file that is being compared to doesn't exist, delete ROOT and
        # BDSIM log file, and move the comparator file.
        if (originalFile == '') and (inputDict['ROOTFile'] in files):
            _os.remove(inputDict['bdsimLogFile'])
            _os.remove(inputDict['ROOTFile'])
            _os.system("mv " + inputDict['compLogFile'] + " FailedTests/" + inputDict['compLogFile'])
        else:
            # move bdsim log file to fail dir
            _os.system("mv " + inputDict['bdsimLogFile'] + " FailedTests/" + inputDict['bdsimLogFile'])
        if (originalFile != '') and selfCompare:
            _os.remove(inputDict['originalFile'])

    return inputDict


class TestSuite(object):
    """ A class that is the test suite. Tests are added to this class,
        after which the BDSIM output will be generated, the comparator run,
        and result/plots generated.

        SuiteType : integer
            0 : Generate original data set a

        Parameters:

        testingDirectory :  string
            The name of the directory that the test files, output and results will
            be written in.
        dataSetDirectory : string, optional
            The filepath to the directory which contains the data to which the generated
            ROOT file will be compared. The filepath must be relative to the testingDirectory.
        usePickledData :  bool, optional
            Load the results data from pickled files. If True, the data will be loaded in the
            TestResults.Analysis class. Default = False.
        fullTestSuite :  bool, optional
            Automatically run the full test suite, i.e generate Test objects for every
            particle (& energy), and for every component using the default component
            parameters. Default = False.
        """
    def __init__(self, testingDirectory,
                 generateOriginalSet        = False,
                 generateDataSet            = True,
                 originalDataSetDirectory   = '',
                 usePickledData             = False,
                 fullTestSuite              = False,
                 plotResults                = True,
                 analyse                    = True,
                 generateOnly               = True,
                 numThreads                 = -1,
                 debug                      = False):
        self._tests = []      # list of test objects
        self._testNames = {}  # dict of test file names (a list of names per component)
        self._testParamValues = {}
        self._debugOutput = _General.debugFunc(debug)

        selfCompare = False
        if generateOnly:
            selfCompare = True

        numSysCores = multiprocessing.cpu_count()

        if numThreads == -1:
            numCores = numSysCores

            # reduce number of cores by a third on linappserv in case it is run by accident
            if _General.GetHostName() in GlobalData.limitedServers:
                numCores = _np.floor(numCores - (numCores / 3.0))
            self._debugOutput("Using " + _np.str(numCores) + " threads.")
        elif numThreads == 0:
            numCores = 1
            self._debugOutput("Number of threads must be finite. Setting number of threads to 1.")
        else:
            numCores = numThreads
            if numCores > numSysCores:
                numCoreStr = _np.str(numCores)
                numSysStr = _np.str(numSysCores)
                threadError = "Number of threads specified (" + numCoreStr + ") is greater than the number \n"
                threadError += "available (" + numSysStr + "), setting number of threads to " + numSysStr + "."
                self._debugOutput(threadError)
                numCores = numSysCores
            else:
                self._debugOutput("Using " + _np.str(numCores) + " threads.")

        self._testEnv = {"pickledData"      : usePickledData,
                         "plotResults"      : plotResults,
                         "debug"            : debug,
                         "analyse"          : analyse,
                         "generateOriginals": generateOriginalSet,
                         "generateData"     : generateDataSet,
                         "numThreads"       : numCores,
                         "selfCompare"      : selfCompare,
                         "generateOnly"     : generateOnly
                         }

        # set directory of existing data set that the tests will be compared to
        dataSetDirectory = 'OriginalDataSet'
        if isinstance(originalDataSetDirectory, _np.str):
            dataSetDirectory = dataSetDirectory

        self._directories = {
                            "tests"      : 'Tests',
                            "results"    : 'Results',
                            "dataSet"     : 'GeneratedDataSet',
                            "origSet"    : dataSetDirectory,
                            "failed"     : 'FailedTests'
                            }

        # create testing directory
        if not isinstance(testingDirectory, _np.str):
            raise TypeError("Testing directory is not a string")
        else:
            if testingDirectory == '':
                raise ValueError("Testing directory string cannot be empty")
            # already in directory
            elif _os.getcwd().split('/')[-1] == testingDirectory:
                pass
            else:
                _General.CheckDirExistsElseMake(testingDirectory)

        # cd into main testing directory
        _os.chdir(testingDirectory)

        # make dirs for gmad files, results, bdsim output, and failed outputs
        _General.CheckDirExistsElseMake(self._directories["tests"])
        _General.CheckDirExistsElseMake(self._directories["results"])
        _General.CheckDirExistsElseMake(self._directories["dataSet"])
        _General.CheckDirExistsElseMake(self._directories["origSet"])
        _os.chdir(self._directories["dataSet"])
        _General.CheckDirExistsElseMake(self._directories["failed"])
        _os.chdir("../")

        self.Analysis = Results.Analysis()  # results instance

        if fullTestSuite:
            self._FullTestSuite()

    def __repr__(self):
        s = 'BdsimRegressionTesting.TestSuite instance.\r\n'
        s += 'This suite contains ' + _np.str(len(self._tests)) + '.\r\n'
        return s

    def WriteGmadFiles(self):
        """ Write the gmad files for all tests in the Tests directory.
            """
        _os.chdir('Tests')
        for test in self._tests:
            writer = Writer.Writer()
            writer.WriteTests(test)
            if test.Component not in self._testNames:
                self._testNames[test.Component] = []
            self._testNames[test.Component].extend(writer._fileNamesWritten[test.Component])
        _os.chdir('../')

    def _Run(self, testlist, componentType):
        """ Function that determines the numbers of cores,
            creates the pool, and calls the function for running BDSIM, comparator etc.
            """

        pool = multiprocessing.Pool(self._testEnv["numThreads"])

        # apply results asynchronously
        results = [pool.apply_async(Run, args=(i,)) for i in testlist]
        output = [p.get() for p in results]

        for testRes in output:
            self.Analysis.AddResults(testRes)
            self.Analysis.TimingData.AddComponentTestTime(componentType, testRes)
        pool.close()

    def _loadPickledData(self):
        """ Function to get the pickled data files.
            """
        self._debugOutput("Using pickled data.")
        _os.chdir('BDSIMOutput')
        with open('results.pickle', 'rb') as handle:
            self.Analysis.Results = pickle.load(handle)
        with open('dipoleResults.pickle', 'rb') as handle:
            self.Analysis.DipoleResults = pickle.load(handle)
        with open('timing.pickle', 'rb') as handle:
            self.Analysis.TimingData = pickle.load(handle)
        for comp in GlobalData.components:
            self.Analysis.PlotResults(comp)
        self.Analysis.ProduceReport()

    def AddTest(self, test):
        """ Add a Test instance to the test suite.
            """
        if not isinstance(test, Test.Test):
            raise TypeError("Test is not a Test instance")
        else:
            self._tests.append(test)

    def GenerateOriginalDataSet(self):
        """ Function to generate an original data set from the supplied tests.
            """
        # set _generateOriginals here, generate data, and change back.
        # It's safer to not pass the bool in as an arg, it could be accidentally
        # passed and potentially overwrite an existing data set.
        self._testEnv['generateOriginals'] = True
        if not _os.path.exists('OriginalDataSet'):
            _os.system("mkdir OriginalDataSet")
        self.RunTestSuite()
        self._testEnv['generateOriginals'] = False
        _os.chdir('OriginalDataSet')
        self.Analysis.ProcessOriginals()
        _os.chdir('../')

    def _GenerateDataSet(self, generateOriginals=False, selfCompare=False):
        for componentType in self._testNames.keys():
            self._debugOutput("  Component: "+componentType)

            _General.CheckDirExistsElseMake(componentType)
            _os.chdir(componentType)
            _General.CheckDirExistsElseMake("FailedTests")
            testfilesDir = '../../Tests/'

            t = time.time()  # initial time

            # list of file paths for every test of this component type
            tests = [(testfilesDir + testFile) for testFile in self._testNames[componentType]]

            # order tests
            if (len(tests) > 1) and (componentType != 'multipole') and (componentType != 'thinmultipole'):
                tests = _General.GetOrderedTests(self._testParamValues, tests, componentType)

            # compile iterable list of dicts for multithreading function.
            testlist = []
            for test in tests:
                origname = _General.CheckForOriginal(self._directories["dataSet"], test, componentType)

                # pass data in a dict. Easier to pass single expandable variable.
                testDict = Test.TestData(testFile=test,
                                         componentType=componentType,
                                         originalFile=origname,
                                         generateOriginals=generateOriginals,
                                         selfCompare=selfCompare)

                testlist.append(testDict)
            self._debugOutput("... Done.")

            # Run BDSIM for these tests.
            self._Run(testlist, componentType)

            componentTime = time.time() - t  # final time
            self.Analysis.TimingData.AddComponentTotalTime(componentType, componentTime)

            self._debugOutput("\tElement finished.")
            # component finished, onto next component type
            _os.chdir('../')

    def RunTestSuite(self):
        """ Run all tests in the test suite. This will generate the tests rootevent
            output, compares to an original file, and processes the comparison results.

            Breakdown:
            1. Write All Tests
            2. Generate Original Data Set or point to Pre-Existing Data Set
            3. Generate New Data Set
            4. Compare Data Sets
            5. Analysis and Generate Plots/Results.

            """
        # if using pickled data, just produce the plots and report.
        if self._testEnv['pickledData']:
            self._loadPickledData()
            return None

        _General.WriteGlobalOptions()  # Write the global options file.
        self.WriteGmadFiles()  # Write all gmad files for all test objects.

        initialTime = time.time()

        if self._testEnv["generateOriginals"]:
            # generate original data set
            _os.chdir(self._directories["origSet"])
            self._GenerateDataSet(selfCompare=self._testEnv["selfCompare"])
            _os.chdir('../')

        if self._testEnv["generateData"]:
            # generate new data set
            _os.chdir(self._directories["dataSet"])
            self._GenerateDataSet(selfCompare=self._testEnv["selfCompare"])
            _os.chdir('../')

        # analysis
        if not self._testEnv["generateOriginals"]:
            self._debugOutput("Running analysis...")
            for componentType in self._testNames.keys():
                self.Analysis.AddTimingData(componentType, self.Analysis.TimingData)

                self._debugOutput("\tGenerating " + componentType + " output results...")
                self.Analysis.ProcessResults(componentType=componentType)

                if self._testEnv["plotResults"]:
                    self.Analysis.PlotResults(componentType=componentType)
                    self._debugOutput("\tGenerating " + componentType + " plot(s)...")
        else:
            self.Analysis.ProcessOriginals()

        self._debugOutput("Analysis and output complete.")

        if not self._testEnv['generateOnly']:
            finalTime = time.time() - initialTime
            self.Analysis.TimingData.SetTotalTime(finalTime)
            self.Analysis._pickleResults()
            self.Analysis.ProduceReport()
        _os.chdir('../')

        self._debugOutput("Testing complete.")

    def _FullTestSuite(self):
        # data containers for tests and number of files
        numFiles = {}
        for component in GlobalData.components:
            numFiles[component] = 0

        # set phase space
        TestPS = GlobalData.BeamPhaseSpace
        BeamPhaseSpace = PhaseSpace.PhaseSpace(TestPS['X'], TestPS['PX'], TestPS['Y'], TestPS['PY'], TestPS['T'], TestPS['PT'])
        BeamPhaseSpace._WriteToInrays('Tests/trackingTestBeam.madx')

        # loop over all machines which have an energy and particle
        for machineInfo in GlobalData.accelerators.values():
            energy = machineInfo['energy']
            particle = machineInfo['particle']

            #bannedComponents = ["rbend", "sbend", "muspoiler"]
            bannedComponents = ["muspoiler"]
            # loop over all components
            for component in GlobalData.components:
                if component not in bannedComponents:
                    componentTest = Test.Test(component, energy, particle, BeamPhaseSpace, useDefaults=True)
                    self.AddTest(componentTest)
                    numFiles[component] += componentTest._numFiles

        self.totalFiles = 0
        self.numFiles = numFiles
        for component in numFiles:
            self.totalFiles += numFiles[component]

        # run the test suite now that all the tests have been added.
        self.RunTestSuite()
