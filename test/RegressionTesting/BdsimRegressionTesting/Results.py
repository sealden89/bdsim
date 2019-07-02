import numpy as _np
import time as _time
import collections
import pickle
import string as _string

import _General
import Globals
import _Plot

GlobalData = Globals.Globals()


class Results(list):
    """ A class containing all the data from the test results.

        Parameters:

        componentType :  string
            The component type that this class holds the data of.
        """
    def __init__(self, componentType=''):
        list.__init__(self)
        GlobalData._CheckComponent(componentType)
        self._component = componentType
        self._numEntries = 0

    def GetResultsByParticle(self, particle=''):
        """ A function to return a Results instance containing the subset of
            results which have the specified particle type.
            """
        if particle not in GlobalData.particles:
            raise ValueError("Unknown particle type.")
        particleResults = Results(self._component)
        for testNum, testResult in enumerate(self):
            if testResult['particle'] == particle:
                particleResults.append(testResult)
        particleResults._numEntries = len(particleResults)
        setattr(particleResults, 'uniqueValues', particleResults._getUniqueValues())
        setattr(particleResults, 'commonValues', particleResults._getCommonValues())
        return particleResults

    def GetResultsByEnergy(self, energy=''):
        """ A function to return a Results instance containing the subset of
            results which have the specified energy. Energy should be a string.
            """
        energyResults = Results(self._component)
        for testNum, testResult in enumerate(self):
            if testResult['testParams']['energy'] == energy:
                energyResults.append(testResult)
        energyResults._numEntries = len(energyResults)
        setattr(energyResults, 'uniqueValues', energyResults._getUniqueValues())
        setattr(energyResults, 'commonValues', energyResults._getCommonValues())
        return energyResults

    def _getCommonValues(self):
        """ A function get the common parameter values for all entries
            in this class. Returns an ordered dictionary with the parameters
            and common values.
            """
        uniqueValues = self._getUniqueValues()
        if uniqueValues is None:
            return None

        commonValues = collections.OrderedDict()
        for key, value in uniqueValues.iteritems():
            if len(value) == 1:
                commonValues[key] = value[0]

        for values in commonValues.values():
            if len(values) > 0:
                return commonValues
        return None

    def _getUniqueValues(self):
        """ A function get the unique parameter values for all entries
            in this class. Returns an ordered dictionary with the parameters
            and unique values.
            """
        uniqueValues = collections.OrderedDict()
        for test in self:
            for key, value in test['testParams'].iteritems():
                if key not in uniqueValues.keys():
                    uniqueValues[key] = []
                if value not in uniqueValues[key]:
                    uniqueValues[key].append(value)
        return uniqueValues

    def GetResults(self):
        """ A function get the results for all entries in this class.
            Returns a list of the results.
            """
        testResults = [test['comparatorResults'] for test in self]
        return testResults

    def GetGeneralStatus(self):
        """ A function get the general status for all entries in this class.
            Returns a list of the results.
            """
        statResults = [test['generalStatus'] for test in self]
        return statResults

    def GetParams(self):
        """ A function get the component parameter values for all
            entries in this class. Returns a list of the results.
            """
        paramResults = [test['testParams'] for test in self]
        return paramResults


class Timing:
    """ A class containing the running time data for BDSIM and the
        comparator for each test.
        """
    def __init__(self):
        self.componentTimes = {}
        self.bdsimTimes = {}
        self.comparatorTimes = {}
        self.totalTime = 0

    def SetTotalTime(self, totalTime):
        """ Set the total time for all tests for all components.
            """
        try:
            totalTime = _np.float(totalTime)
            self.totalTime = totalTime
        except ValueError:
            pass

    def AddComponentTotalTime(self, component, componentTime):
        """ Set total time for all tests for a given components.
            """
        try:
            cTime = _np.float(componentTime)
            self.componentTimes[component] = cTime
        except ValueError:
            print("Cannot convert componentTime to a numerical value.")

    def AddComponentTestTime(self, component, test):
        if component not in self.bdsimTimes.keys():
            self.bdsimTimes[component] = []

        if component not in self.comparatorTimes.keys():
            self.comparatorTimes[component] = []
        self.bdsimTimes[component].append(test['bdsimTime'])
        self.comparatorTimes[component].append(test['compTime'])

    def __repr__(self):
        totalBDSIMTime = []
        totalCompTime = []
        for component, times in self.bdsimTimes.iteritems():
            totalBDSIMTime.extend(times)
        for component, times in self.comparatorTimes.iteritems():
            totalCompTime.extend(times)

        s = 'Total Testing time  = ' + _np.str(self.totalTime) + '\r\n'
        s += 'Average BDSIM time = ' + _np.str(_np.average(totalBDSIMTime)) + " +/- "
        s += _np.str(_np.std(totalBDSIMTime)) + '.\r\n'
        s += 'Average Comparator time = ' + _np.str(_np.average(totalCompTime)) + " +/- "
        s += _np.str(_np.std(totalCompTime)) + '.\r\n'
        return s


class ResultsUtilities:
    """ A class containing utility functions and data containers for the Analysis class.
        """
    def __init__(self):
        self.Results = {}
        self.DipoleResults = {}  # seperate results dict for dipoles.
        self.TimingData = Timing()  # timing data.

    def _processTimingData(self, component):
        bdsimMean = _np.mean(self.TimingData.bdsimTimes[component])
        compMean = _np.mean(self.TimingData.comparatorTimes[component])
        bdsimStd = _np.std(self.TimingData.bdsimTimes[component])
        compMean = _np.std(self.TimingData.comparatorTimes[component])

        bdsimLimit = bdsimMean + 3 * bdsimStd
        longTests = Results(component)
        longTests.extend([test for test in self.Results[component] if test['bdsimTime'] > bdsimLimit])

        commonFactors = _General.GetCommonFactors(longTests)

        if len(commonFactors.keys()) > 0:
            s = "There were " + _np.str(len(longTests)) + " tests that took longer than " \
                + _np.str(_np.round(bdsimLimit, 6)) + " s,\r\n"
            s += "these tests had the following common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _pickleResults(self):
        """ Function to pickle the Results instances and save to disk.
            """
        with open('results.pickle', 'wb') as handle:
            pickle.dump(self.Results, handle, protocol=pickle.HIGHEST_PROTOCOL)
        with open('timing.pickle', 'wb') as handle:
            pickle.dump(self.TimingData, handle, protocol=pickle.HIGHEST_PROTOCOL)
        with open('dipoleResults.pickle', 'wb') as handle:
            pickle.dump(self.DipoleResults, handle, protocol=pickle.HIGHEST_PROTOCOL)


class Analysis(ResultsUtilities):
    """ A class for processing the test results."""
    def __init__(self):
        ResultsUtilities.__init__(self)

    def AddResults(self, results):
        """ Add results to the class's data containers.
            """
        componentType = results['componentType']
        if componentType not in self.Results.keys():
            self.Results[componentType] = Results(componentType)
        if isinstance(results, dict):
            self.Results[componentType].append(results)
        elif type(results) in GlobalData.multiEntryTypes:
            for res in results:
                self.Results[componentType].append(res)
        self.Results[componentType]._numEntries = len(self.Results[componentType])

    def AddTimingData(self, componentType, timingData):
        """ Add timing data to the class's data containers.
            """
        if not isinstance(timingData, Timing):
            raise TypeError("Timing data muse be a TestResults.Timing instance.")
        else:
            self.Results[componentType].timingData = timingData

    def ProcessOriginals(self):
        """ Process the output when generating an original data set.
            """
        numTests = 0
        numFailed = 0
        failedTests = []
        softFails = {}

        for component, compResults in self.Results.iteritems():
            for testdict in compResults:
                numTests += 1

                generalStatus = testdict['generalStatus']
                # if failed
                if (generalStatus is not None) and (0 not in generalStatus):
                    numFailed += 1
                    failedFile = testdict['testFile'].split('/')[-1]
                    failedTests.append(failedFile)
                # check for soft failures
                for failure in GlobalData.ReturnsAndErrors.GetSoftCodes():
                    if GlobalData.ReturnsAndErrors.GetCode(failure) in generalStatus:
                        if failure not in softFails.keys():
                            softFails[failure] = []
                        failedFile = testdict['testFile'].split('/')[-1]
                        softFails[failure].append(failedFile)

        s = _np.str(numTests - numFailed) + "/" + _np.str(numTests) + " ROOT files were successfully generated.\r\n"
        print(s)

        f = open('DataSetGeneration.log', 'a')
        timestring = '! ' + _time.strftime("%a, %d %b %Y %H:%M:%S +0000", _time.gmtime()) + '\n'
        f.write(timestring)
        f.write('\r\n')
        f.write(s)
        f.write('\r\n')

        if numFailed > 0:
            f.write("The following tests failed to generate ROOT output:\r\n")
            for test in failedTests:
                f.write(test + "\r\n")
            f.write("\r\n")

        for failure in GlobalData.ReturnsAndErrors.GetSoftCodes():
            failures = []
            if failure in softFails.keys():
                failures = softFails[failure]

            if len(failures) > 0:
                failString = failure.replace("_", " ")
                failString = _string.lower(failString)
                f.write("The following " + _np.str(len(failures)) + " generated files contain " + failString + ":\r\n")
                for test in failures:
                    f.write(test + "\r\n")
                f.write("\r\n")

        f.write(_General.GetGitCommit())
        f.close()

    def ProcessResults(self, componentType=''):
        """ Process the results data held in the results class.
            """
        if componentType != '':
            GlobalData._CheckComponent(componentType)

            testResults = self.Results[componentType]
            # set comparator results of all failed tests
            for index, result in enumerate(testResults):
                coords = _General.GetPhaseSpaceComparatorData(result, 'FailedTests/' + result['compLogFile'])
                self.Results[componentType][index]['comparatorResults'] = coords
        else:
            GlobalData._CheckComponent(componentType)  # raises value error

        # reverse list of results. This is purely for plotting purposes,
        # we want the data to be displayed descending rather than ascending.
        self.Results[componentType].reverse()
        setattr(self.Results[componentType], 'uniqueValues', self.Results[componentType]._getUniqueValues())
        setattr(self.Results[componentType], 'commonValues', self.Results[componentType]._getCommonValues())

        if (componentType == 'rbend') or (componentType == 'sbend'):
            self._groupDipoleResults(componentType)
        self._processTimingData(componentType)

    def _groupDipoleResults(self, componentType=''):
        """ Process the dipole results data held in the results class.
            Dipole data is grouped according to energy, length, and angle
            or field. The results are then representative of all
            poleface parameter combinations.
            """
        # get dictionary of poleface parameter values
        def _dictPolefaceCompVals(testparams):
            paramSet = collections.OrderedDict()
            paramSet['e1'] = testparams['e1']
            paramSet['e2'] = testparams['e2']
            paramSet['fint'] = testparams['fint']
            paramSet['fintx'] = testparams['fintx']
            paramSet['hgap'] = testparams['hgap']
            return paramSet

        # update general status column
        def _updateGeneralStatus(_genStatus):
            genStat = []
            for codes in _genStatus:
                if (len(codes) == 1) and (not codes[0] in genStat):
                    genStat.append(codes[0])
                elif len(codes) > 1:
                    for code in codes:
                        if code not in genStat:
                            genStat.append(code)
            return genStat

        # update phasespace coords
        def _updateCoords(_comparatorResults):
            resList = [[], [], [], [], [], [], []]
            coords = _np.array(_comparatorResults)
            for index in range(coords.shape[1]):
                values = coords[:, index]
                templist = []
                for i in values:
                    if i not in templist:
                        templist.append(i)
                resList[index] = templist
            return resList

        # get dictionary of non-poleface parameters
        def _getResDict(energyVal, lengthVal, angleVal=0, fieldVal=0):
            dipResults = {'testParams': collections.OrderedDict(),
                          'polefaceParams': []}
            dipResults['testParams']['energy'] = energyVal
            dipResults['testParams']['length'] = lengthVal
            if angleVal != 0:
                dipResults['testParams']['angle'] = angleVal
            elif fieldVal != 0:
                dipResults['testParams']['field'] = fieldVal
            return dipResults

        # get the results data
        if (componentType != 'rbend') and (componentType != 'sbend'):
            raise ValueError("Component must be an rbend or sbend.")
        params = self.Results[componentType].GetParams()
        uniqueValues = self.Results[componentType].uniqueValues
        generalStatus = self.Results[componentType].GetGeneralStatus()
        comparatorResults = self.Results[componentType].GetResults()
        dipoleResults = Results(componentType)

        if len(self.Results[componentType]) == 1:
            self.DipoleResults[componentType] = self.Results[componentType]
        else:
            # loop over all non-poleface parameters
            for energy in uniqueValues['energy']:
                for length in uniqueValues['length']:
                    if 'angle' in uniqueValues.keys():
                        for angle in uniqueValues['angle']:
                            # containers for dipole results
                            dipRes = _getResDict(energy, length, angleVal=angle)
                            _genStat = []
                            _resList = []

                            # create list of tests that have the correct energy, length, and angle.
                            # Also update poleface parameter values of appropriate tests.
                            for testNum in range(self.Results[componentType]._numEntries):
                                testParams = params[testNum]
                                if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                        and (testParams['angle'] == angle):
                                    dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                    dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                    _genStat.append(generalStatus[testNum])
                                    _resList.append(comparatorResults[testNum])

                            # update the poleface parameter values, general status, and coords results
                            dipRes['generalStatus'] = _updateGeneralStatus(_genStat)
                            dipRes['comparatorResults'] = _updateCoords(_resList)

                            dipoleResults.append(dipRes)
                    elif 'field' in uniqueValues.keys():
                        for field in uniqueValues['field']:
                            # containers for dipole results
                            dipRes = _getResDict(energy, length, fieldVal=field)
                            _genStat = []
                            _resList = []

                            # create list of tests that have the correct energy, length, and field.
                            # Also update poleface parameter values of appropriate tests.
                            for testNum in range(self.Results[componentType]._numEntries):
                                testParams = params[testNum]
                                if (testParams['energy'] == energy) and (testParams['length'] == length) \
                                        and (testParams['field'] == field):
                                    dipRes['particle'] = self.Results[componentType][testNum]['particle']
                                    dipRes['polefaceParams'].append(_dictPolefaceCompVals(testParams))
                                    _genStat.append(generalStatus[testNum])
                                    _resList.append(comparatorResults[testNum])

                            # update the poleface parameter values, general status, and coords results
                            dipRes['generalStatus'] = _updateGeneralStatus(_genStat)
                            dipRes['comparatorResults'] = _updateCoords(_resList)

                            dipoleResults.append(dipRes)
            self.DipoleResults[componentType] = dipoleResults

    def PlotResults(self, componentType=''):
        """ Plot the results for the specified component type.
            """
        if componentType in self.Results.keys():
            self._processTimingData(componentType)
            # if the component is a dipole, use the dipole data
            if (componentType == 'rbend') or (componentType == 'sbend'):
                _Plot.PlotResults(self.DipoleResults, componentType)
                _Plot.PlotTimingData(self.TimingData, componentType)
            # otherwise use the normal data
            else:
                _Plot.PlotResults(self.Results, componentType)
                _Plot.PlotTimingData(self.TimingData, componentType)

    def ProduceReport(self):
        """ Produce a written report for all component types.
            The report will contain an overall results summary, a breakdown
            of the hard and soft failures, a breakdown of the comparator failures,
            and display the component parameter values coomon to those failures.
            """
        report = _Report(self.Results, self.DipoleResults)
        report.ProduceReport()


class _Report:
    """ A class for producing a report of the testing results. Class should be hidden,
        it is instantiated in the Analysis class and used there."""
    def __init__(self, results, dipoleResults):
        self.Results = results
        self.DipoleResults = dipoleResults
        self.groupedResults = {}
        self._numResults = {}
        self._fileName = "Results/TestResults_" + _time.strftime("%d%m%Y_%H%M%S", _time.gmtime()) + ".txt"

    def _getResults(self, component):
        """ A function to get the test results for the specified
            component type.
            """
        compResults = None
        if ((component == 'rbend') or (component == 'sbend')) and (component in self.DipoleResults.keys()):
            compResults = self.DipoleResults[component]
        elif component in self.Results.keys():
            compResults = self.Results[component]
        return compResults

    def _groupResults(self, comparatorResults, component):
        """ A function to group the results according to their general status,
            i.e, the would find all tests that have tracking warnings, all tests
            that have stuck particles ... etc.
            """
        # seperate dicts for results and numbers. Keys for both will be
        # Globals.Globals.returnCodes values.
        groupedResults = {}
        numResults = {}
        for codeIndex, error in enumerate(GlobalData.ReturnsAndErrors['name']):
            groupedResults[error] = Results(component)
            numResults[error] = 0
            for resultNum, result in enumerate(comparatorResults):
                if (codeIndex in result['generalStatus']) or (_np.str(codeIndex) in result['generalStatus']):
                    groupedResults[error].append(result)
            for resultNum, result in enumerate(self.Results[component]):
                if (codeIndex in result['generalStatus']) or (_np.str(codeIndex) in result['generalStatus']):
                    numResults[error] += 1

        self.groupedResults[component] = groupedResults
        self._numResults[component] = numResults

    def _processFatals(self, component):
        """ A function that gets all results that contain fatal exceptions.
            Returns a string with parameter values common to the results that were found.
            """
        results = self.groupedResults[component]['FATAL_EXCEPTION']
        commonFactors = _General.GetCommonFactors(results)

        if len(commonFactors.keys()) > 0:
            s = "\tTests where a fatal exception was called had the common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _processSoftFail(self, component, failure):
        """ A function that gets all results that contain the soft failures.
            Returns a string with parameter values common to the results that were found.
            """
        results = self.groupedResults[component][failure]
        commonFactors = _General.GetCommonFactors(results)

        if commonFactors.keys().__len__() > 0:
            strName = failure.replace("_", " ")
            strName = _string.lower(strName)
            s = "\tTests where a " + strName + "was called had the common parameters:\r\n"
            for param, value in commonFactors.iteritems():
                s += "\t" + param + " : " + _np.str(value) + ".\r\n"
            return s
        else:
            return None

    def _processComparatorFailure(self, component):
        """ A function that gets all results for the phasespace coordinates.
            Returns a string with the number of failures per dimension, and
            parameter values common to the results that were found.
            """
        results = self.Results[component]
        coordLabels = ['x', 'xp', 'y', 'yp', 't', 'zp', 'n']
        numFailures = [0, 0, 0, 0, 0, 0, 0]
        # get number of failures.
        for res in results:
            compResults = res['comparatorResults']
            for index, coord in enumerate(compResults):
                if coord == 1:
                    numFailures[index] += 1

        s = ''
        # only print comparator phase space failures if there are any
        compPasses = True
        for i in numFailures:
            if i > 0:
                compPasses = False
                break

        if not compPasses:
            s = "Number of comparator failures: \r\n"
            s += "  x:  " + _np.str(numFailures[0]) + ",\r\n"
            s += "  xp: " + _np.str(numFailures[1]) + ",\r\n"
            s += "  y:  " + _np.str(numFailures[2]) + ",\r\n"
            s += "  yp: " + _np.str(numFailures[3]) + ",\r\n"
            s += "  t:  " + _np.str(numFailures[4]) + ",\r\n"
            s += "  zp: " + _np.str(numFailures[5]) + ",\r\n"
            s += "  n:  " + _np.str(numFailures[6]) + ".\r\n"
            s += "\r\n"
            s += "Comparator failures had the following common parameters:\r\n"

            # get list of all failures per dimension and the parameter values common
            # to those failed tests.
            for i in range(7):
                s += "  " + coordLabels[i] + ":\r\n"
                failedTests = Results(component)
                for res in results:
                    compResults = res['comparatorResults']
                    if compResults[i] == 1:
                        failedTests.append(res)
                commonFactors = _General.GetCommonFactors(failedTests)

                if commonFactors.keys().__len__() > 0:
                    for param, value in commonFactors.iteritems():
                        s += "    " + param + " : " + _np.str(value) + ".\r\n"
        return s

    def _componentSectionTitle(self, component):
        """ A function to return the component section title
            in the report.
            """
        s1 = ''
        for i in range(80):
            s1 += "-"
        s1 += "\r\n"

        # component type in the centre
        numSpaces = 80 - len(component)
        if _np.mod(numSpaces, 2) == 0:
            spBefore = numSpaces / 2.0
        else:
            spBefore = (numSpaces - 1) / 2.0
        s2 = ''
        for i in range(_np.int(spBefore)):
            s2 += ' '
        s2 += component + "\r\n"
        title = s1 + s2 + s1
        return title

    def _overallResults(self, component):
        """ A function to collate the results for a specifed component and
            return a string containing the overall results, breakdown of hard
            failures, and breakdown of soft failures.
            """
        # get data
        overallRes = self._numResults[component]
        totalTests = self.Results[component].__len__()

        # start string with overall results
        s = "Overall Results: \r\n"
        s += "  Passed: " + _np.str(overallRes['SUCCESS']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Soft Failure: " + _np.str(overallRes['FAILED']) + "/" + _np.str(totalTests) + ",\r\n"
        s += "  Hard Failure: " + _np.str(overallRes['FILE_NOT_FOUND']) + "/" + _np.str(totalTests) + ".\r\n"
        s += "\r\n"

        numHardFailures = overallRes['FILE_NOT_FOUND']

        if numHardFailures > 0:
            # breakdown of hard failures.
            s += "Breakdown of Hard Failures: \r\n"

            # update string with number of fatal exceptions
            if overallRes['FATAL_EXCEPTION'] > 0:
                s += "  Fatal Exceptions: " + _np.str(overallRes['FATAL_EXCEPTION']) + "/" + \
                     _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
                numHardFailures -= overallRes['FATAL_EXCEPTION']
                fatalString = self._processFatals(component)
                s += fatalString

            # update string with number of timeouts
            if overallRes['TIMEOUT'] > 0:
                s += "  Timeouts: " + _np.str(overallRes['TIMEOUT']) + "/" + \
                     _np.str(overallRes['FILE_NOT_FOUND']) + ",\r\n"
                numHardFailures -= overallRes['TIMEOUT']


            s += "  Unknown Reason: " + _np.str(numHardFailures) + "/" + _np.str(overallRes['FILE_NOT_FOUND']) + "\r\n"
            s += "    (likely to be BDSIM self exit).\r\n"
            s += "\r\n"

        # breakdown soft failures
        softStr = "Breakdown of Soft Failures: \r\n"
        softFailures = False

        # loop over all soft failures and update string with numbers per failure.
        for index, failure in enumerate(GlobalData.ReturnsAndErrors.GetSoftCodes()):
            if overallRes[failure] > 0:
                softFailures = True
                strName = failure.replace("_", " ")
                strName = _string.capitalize(strName)
                softStr += "  " + strName + ": " + _np.str(overallRes[failure])
                if index == len(GlobalData.ReturnsAndErrors.GetSoftCodes())-1:
                    softStr += ".\r\n"
                else:
                    softStr += ",\r\n"
                failString = self._processSoftFail(component, failure)
                if failString is not None:
                    softStr += failString
        if softFailures:
            s += softStr
        return s

    def _componentSection(self, component):
        """ A function to write the section report for the specified component.
            """
        section = self._componentSectionTitle(component)
        section += "\r\n"
        section += self._overallResults(component)
        section += "\r\n"
        section += self._processComparatorFailure(component)
        section += "\r\n"
        f = open(self._fileName, 'a')
        f.write(section)
        f.close()

    def ProduceReport(self):
        """ Produce a report of the results.
            """
        for componentType in GlobalData.components:
            results = self._getResults(componentType)

            # if the results for that element exist
            if results is not None:
                self._groupResults(results, componentType)
                self._componentSection(componentType)
            else:
                pass
