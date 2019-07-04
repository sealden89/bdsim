import numpy as _np
import os as _os
import glob as _glob
import subprocess as _sub
import collections
import socket as _soc
import time as _time
from scipy import constants as _con
import Globals
from pybdsim import Writer as _pybdsimWriter
from pybdsim import Options as _options
from pybdsim import Builder as _Builder
from pybdsim import Beam as _Beam


GlobalData = Globals.Globals()


def CheckForOriginal(dataSetDirectory, testname, componentType):
    """ Check for the existence of the directory containing the original data set
        that these tests will be compared to."""
    if dataSetDirectory != '':
        dataDir = dataSetDirectory
    else:
        dataDir = 'OriginalDataSet'
    testname = testname.replace('Tests', dataDir)
    testname = testname.replace('.gmad', '_event.root')
    files = _glob.glob('../' + dataDir + '/' + componentType + "/*.root")
    if testname in files:
        return testname
    else:
        return ''


def CompareOutput(originalFile='', newFile='', isSelfComparison=False):
    """ Compare the output file against another file. This function uses BDSIM's comparator.
        The test gmad file name is needed for updating the appropriate log files.
        If the comparison is successful:
            The generated root file is deleted.
            The BDSIM pass log file is updated.
        If the comparison is not successful:
            The generated file is moved to the FailedTests directory.
            The Comparator log is updated.

        This function can only be used for single thread testing.
        """

    outputLog = open("tempComp.log", 'w')  # temp log file for the comparator output.
    TestResult = _sub.call(args=[GlobalData._comparatorExecutable, originalFile, newFile], stdout=outputLog)
    outputLog.close()

    # immediate pass/fail bool for decision on keeping root output
    hasPassed = True
    if TestResult != 0:
        hasPassed = False

    if hasPassed:
        # _os.system("rm "+newFile)
        if isSelfComparison:
            _os.system("rm " + originalFile)
        _os.system("rm tempComp.log")
        f = open('bdsimOutputPassed.log', 'a')  # Update the pass log
        f.write("File " + newFile + " has passed.\r\n")
        f.close()
    else:
        if isSelfComparison:
            _os.system("rm " + originalFile)
        _os.system("mv " + newFile + " FailedTests/" + newFile)  # move the failed file
        UpdateComparatorLog(newFile)
        _os.system("rm tempComp.log")


def GenerateRootFile(inputfile):
    """ Generate the rootevent output for a given gmad file.
        This function can only be used for single thread testing.
        """
    # strip extension to leave test name, will be used as output name
    if inputfile[-5:] != '.gmad':
        outputfile = inputfile[:-5]
    else:
        outputfile = inputfile
    outputfile = outputfile.split('/')[-1]

    # run bdsim and dump output to temporary log file.
    # os.system used as subprocess.call has difficulty with the arguments for some reason.
    bdsimCommand = GlobalData._bdsimExecutable + " --file=" + inputfile + " --output=rootevent --outfile="
    bdsimCommand += outputfile + " --batch"
    _os.system(bdsimCommand + ' > temp.log')

    # quick check for output file. If it doesn't exist, update the main failure log and return None.
    # If it does exist, delete the log and return the filename
    files = _glob.glob('*.root')
    outputevent = outputfile + '_event.root'
    if outputevent not in files:
        return None
    else:
        _os.system("rm temp.log")
        return outputevent


def GetHostName():
    hostName = _soc.gethostname()
    return hostName


def GetOrderedTests(testParamValues, testlist, componentType):
    """ Function to order the tests according to their parameter values."""
    OrderedTests = []
    particles = []
    compKwargs = collections.OrderedDict()  # keep order of kwarg in file name

    for test in testlist:
        fnameStartIndex = test.rfind('/')
        filename = test[fnameStartIndex + 1:]
        path = test[:fnameStartIndex+1]
        if filename[-5:] == '.gmad':
            filename = filename[:-5]  # remove .gmad extension
        splitFilename = filename.split('__')  # split into param_value parts
        particle = splitFilename[1]
        if particle not in particles:
            particles.append(particle)

        # dict of compiled lists of different kwarg values.
        for kwarg in splitFilename[2:]:
            param = kwarg.split('_')[0]
            value = kwarg.split('_')[1]
            if param not in compKwargs.keys():
                compKwargs[param] = []
            if value not in compKwargs[param]:
                compKwargs[param].append(value)
        # sort the kwarg values
        for key in compKwargs.keys():
            compKwargs[key].sort()

    if componentType not in testParamValues.keys():
        testParamValues[componentType] = []

    # sort energy when energy is a float, not string.
    for param in compKwargs.keys():
        if len(compKwargs[param]) > 1:
            energy = [_np.float(x) for x in compKwargs[param]]
            energy.sort()
            compKwargs[param] = [_np.str(x) for x in energy]
    testParamValues[componentType].append(compKwargs)

    kwargKeys = compKwargs.keys()

    # recursively create filenames from all kwarg value permutations.
    # if filename matches one in supplied test list, add to ordered list.
    # please do not change this, it took ages to get it working correctly.
    def sublevel(depth, nameIn):
        for kwargValue in compKwargs[kwargKeys[depth]]:
            name = nameIn + '__' + kwargKeys[depth] + "_" + kwargValue
            fullname = path + name + '.gmad'
            if depth < (len(kwargKeys) - 1):
                sublevel(depth + 1, name)
            elif fullname in testlist:
                OrderedTests.append(path + name + '.gmad')

    for particle in particles:
        fname = componentType + "__" + particle
        sublevel(0, fname)
    return OrderedTests


def UpdateBDSIMFailLog(testFileName):
    """ Update the test failure log.
        This function can only be used for single thread testing.
        """
    f = open('temp.log', 'r')
    g = open('bdsimOutputFailures.log', 'a')
    g.write('\r\n')
    g.write('FAILED TEST FILE: ' + testFileName)
    g.write('\r\n')
    for line in f:
        g.write(line)
    g.close()
    f.close()


def UpdateComparatorLog(testFileName):
    """ Update the test pass log.
        This function can only be used for single thread testing.
        """
    f = open('tempComp.log', 'r')
    g = open('comparatorOutput.log', 'a')
    g.write('\r\n')
    g.write('FAILED TEST FILE: ' + testFileName)
    g.write('\r\n')
    for line in f:
        g.write(line)
    g.close()
    f.close()


def WriteGlobalOptions(robust=False):
    """ Write the options file that will be used by all test files.
        """
    options = _options.Options()
    options.SetSamplerDiameter(3)
    options.SetWritePrimaries(False)
    if not robust:
        options.SetStopSecondaries(True)
    options.SetPhysicsList(physicslist="em hadronic")
    options.SetBeamPipeRadius(beampiperadius=20)
    writer = _pybdsimWriter.Writer()
    writer.WriteOptions(options, 'Tests/trackingTestOptions.gmad')


def GetBDSIMLogData(result):
    """ A function to recognise errors and warnings in the printout when
        running BDSIM .

        result :  an entry from the Results object which respresents a single test.
        """
    generalStatus = []

    # append comparator check
    if result['code'] == 0:
        generalStatus.append(0)
    elif result['code'] == 1:
        generalStatus.append(1)
    elif result['code'] == 2:
        generalStatus.append(2)
    elif result['code'] == 3:
        generalStatus.append(3)
    elif result['code'] == 6:
        generalStatus.append(6)

    # get data in logfile.
    f = open(result['bdsimLogFile'])
    lines = f.read()
    f.close()

    splitLines = lines.split('\n')

    # start and endlines. There are two copies of the G4Exception warning/error, however one
    # has a space at the end.
    startLines = ['-------- WWWW ------- G4Exception-START -------- WWWW -------',
                  '-------- WWWW ------- G4Exception-START -------- WWWW ------- ',
                  '-------- EEEE ------- G4Exception-START -------- EEEE -------',
                  '-------- EEEE ------- G4Exception-START -------- EEEE ------- ']
    endLines = ['-------- WWWW -------- G4Exception-END --------- WWWW -------',
                '-------- WWWW -------- G4Exception-END --------- WWWW ------- ',
                '-------- EEEE -------- G4Exception-END --------- EEEE -------',
                '-------- EEEE -------- G4Exception-END --------- EEEE ------- ']

    # indices of start and end of geant4 exceptions and warnings.
    startLineIndices = [i for i, x in enumerate(splitLines) if x in startLines]
    endLineIndices = [i for i, x in enumerate(splitLines) if x in endLines]
    endLineIndices.sort()  # sort just in case

    if len(startLineIndices) != len(endLineIndices):
        # something went wrong
        return generalStatus

    # loop over all soft codes and update general status with error code depending
    # on which Geant4 class(es) the error was issued by. There are two seperate checks,
    # one for the whole error line in the file, the other for the Geant4 class that issued
    # the error in the G4Exception section in the file. The whole error line must be checked
    # as some Geant4 errors are written out with cerr rather than G4Exception class.
    softCodes = GlobalData.ReturnsAndErrors.GetSoftCodes()
    for code in softCodes:
        issuedBy = GlobalData.ReturnsAndErrors.GetIssuedBy(code)

        if type(issuedBy) in GlobalData.multiEntryTypes:
            for issue in issuedBy:
                issueCR = issue + "\r\n"
                if issueCR in splitLines:
                    generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
        else:
            issueCR = issuedBy + "\r\n"
            if issueCR in splitLines:
                generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))

        if len(startLineIndices) > 0:
            for index, startLine in enumerate(startLineIndices):
                exceptions = splitLines[startLine:endLineIndices[index] + 1]
                issuedLine = exceptions[2]

                if type(issuedBy) in GlobalData.multiEntryTypes:
                    for issue in issuedBy:
                        if issue in issuedLine:
                            generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
                else:
                    if issuedBy in issuedLine:
                        generalStatus.append(GlobalData.ReturnsAndErrors.GetCode(code))
    return generalStatus


def GetGitCommit():
    """ Function to get the information about which commit BDSIM was built using.
        """
    pwd = _os.getcwd()  # keep copy of testing cwd
    logfile = pwd + '/gitCommit.log'
    branchfile = pwd + '/gitBranch.log'

    # cd to git repo, output info to log file in testing dir, and cd back
    _os.chdir(GlobalData._bdsimSource)
    _os.system("git log -1 > " + logfile)
    _os.system("git branch | grep  \* > " + branchfile)
    fetchTime = _time.strftime('%Y-%m-%d %H:%M:%S', _time.localtime(_os.stat('.git/FETCH_HEAD').st_mtime))
    _os.chdir(pwd)

    # branch info should only be one line of text
    f = open('gitBranch.log', 'r')
    branchLine = f.next()
    f.close()
    # get branch name
    branchLine = branchLine.strip('\r')
    branchLine = branchLine.strip('\n')
    branch = branchLine.split(' ')[1]

    gitLines = "BDSIM was built from the git repository using the branch " + branch + ".\r\n"
    gitLines += "Last fetch from remote was at " + fetchTime + ".\r\n"
    gitLines += "Local repository is at the following commit: \r\n"
    gitLines += "\r\n"

    # append lines from commit log
    f = open('gitCommit.log')
    for line in f:
        gitLines += (line + '\r\n')
    _os.remove('gitCommit.log')
    _os.remove('gitBranch.log')
    return gitLines


def GetCommonFactors(results):
    """ Function to get the common parameter values of a data set.
        """
    # Note, this is equivalent to the _getCommonValues function in the Results
    # class, however, here the results are supplied as a argument which doesn't
    # have to be a Results instance.

    commonFactors = {}
    globalParams = {}
    for res in results:
        for param, value in res['testParams'].iteritems():
            if not globalParams.keys().__contains__(param):
                globalParams[param] = []
            if not globalParams[param].__contains__(value):
                globalParams[param].append(value)
    for param, value in globalParams.iteritems():
        if len(value) == 1:
            commonFactors[param] = value[0]
        elif len(value) == 2:
            commonFactors[param] = value
    return commonFactors


def GetPhaseSpaceComparatorData(result, logFile=''):
    """ A function to get the comparator results for all 6 dimensions.

        result :  an entry from the Results object which respresents a single test.

        logfile :  string
            The filename and path of the comparator log file for the result.
        """
    # phasespace coords initialised as passed.
    coords = _np.zeros(7)

    # return all true if comparator returned passed
    code = result['code']
    if code == 0:
        return coords
    elif code == 2:  # incorrect args
        coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
        return coords
    elif code == 3:  # file not found
        coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
        return coords
    elif code == 6:  # Timeout
        coords[:] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
        return coords

    # get data in logfile.
    f = open(logFile)
    lines = f.read()
    f.close()

    # get data on number particles comparison. If the numbers are not equal,
    # set the phase space coords to no data, and number of particle compared to failed.
    numParticlesIndex = lines.find('Event Tree (1/2) entries')
    if numParticlesIndex != -1:
        offendingSamplerBranchesLine = lines[numParticlesIndex:].split('\n')[0]
        branches = offendingSamplerBranchesLine.replace('Event Tree (1/2) entries ', '')
        branches = branches.replace('(', '')
        branches = branches.replace(')', '')
        numParticles = branches.split('/')
        if numParticles[0] != numParticles[1]:
            coords[6] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            coords[0:6] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
            # if num particles don't match, there'll be no phase space comparison
            return coords

    # get data on phase space branches that failed. Log file only contains failed branches.
    phasespaceIndex = lines.find('type Sampler')
    if phasespaceIndex != -1:
        offendingSamplerBranchesLine = lines[phasespaceIndex:].split('\n')[1]
        branches = offendingSamplerBranchesLine.split(' ')
        branches.remove('Offending')
        branches.remove('branches:')
        branches.remove('')
        if branches.__contains__('x'):
            coords[0] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('xp'):
            coords[1] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('y'):
            coords[2] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('yp'):
            coords[3] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('t'):
            coords[4] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('zp'):
            coords[5] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
        if branches.__contains__('n'):
            coords[6] = GlobalData.ReturnsAndErrors.GetCode('FAILED')
            coords[0:6] = GlobalData.ReturnsAndErrors.GetCode('NO_DATA')
    return coords


def CalculateEnergy(total_energy, particle='e-'):
    # Calculate the energy & momentum of a proton and electron at a given total energy.
    eMass = _con.electron_mass * _con.c**2 / _con.e / 1e9
    pMass = _con.proton_mass * _con.c**2 / _con.e / 1e9

    electronKinetic = 0
    electronEnergy = eMass
    electronMomentum = 0
    protonKinetic = 0
    protonEnergy = pMass
    protonMomentum = 0
    brho = 0

    if particle == 'proton' or particle == 'p':
        protonEnergy = total_energy
        protonKinetic = total_energy - pMass
        protonMomentum = _np.sqrt(total_energy**2 - pMass**2)
        brho = 3.335640951981521 * protonMomentum
        electronMomentum = protonMomentum
        electronEnergy = _np.sqrt(electronMomentum**2 + eMass**2)
        electronKinetic = electronEnergy - eMass
    elif particle == 'e' or particle == 'e-':
        electronEnergy = total_energy
        electronKinetic = total_energy - eMass
        electronMomentum = _np.sqrt(total_energy**2 - eMass**2)
        brho = 3.335640951981521 * electronMomentum
        protonMomentum = electronMomentum
        protonEnergy = _np.sqrt(protonMomentum**2 + pMass**2)
        protonKinetic = protonEnergy - pMass

    res={'e-'     : {'KE': electronKinetic,
                     'TE': electronEnergy,
                     'P' : electronMomentum},
         'proton' : {'KE': protonKinetic,
                     'TE': protonEnergy,
                     'P' : protonMomentum},
         'brho'   : brho}
    return res


def CalcBField(length, angle, energy, particle):
    # Calculate the magnetic field for a dipole
    if angle == 0:
        return 0
    else:
        energies = CalculateEnergy(energy, particle)
        rho = length / angle
        b = energies['brho'] / rho
        return b


def Machine(particle, robust=False):
    machine = _Builder.Machine()
    if robust:
        colLength = 1.0  # default
        if particle == 'e-':
            colLength = 0.01
        elif particle == 'proton':
            colLength = 0.5
        machine.AddDrift(name='precr1', length=0.1)
        machine.AddRCol(name='FeCol', length=colLength, xsize=0, ysize=0)
        machine.AddDrift(name='precr2', length=0.1)
    return machine


def debugFunc(debug):
    # dynamically create function to print debug output. Saves accessing bool at runtime to
    # check if debug is true every time the function is called
    if debug:
        def function_template(output):
            print output

        return function_template
    else:
        def function_template(output):
            pass

        return function_template


def CheckDirExistsElseMake(dir):
    if not _os.path.exists(dir):
        _os.mkdir(dir)


def GetBeam(test):
    """ Function to create and return a pybdsim.Beam.Beam instance.
        The beam is set to PTC."""
    beam = _Beam.Beam()
    beam.SetParticleType(particletype=test.Particle)
    beam.SetEnergy(test.Energy)
    beam.SetDistributionType(distrtype='ptc')
    beam._SetSigmaE(sigmae=0)
    return beam


def MakeTestDirs(test):
    """ Function to make the directories that the tests will be written in.
        If the number of tests for the component is > 1000, the files will
        be split into multiple subdirectories. """
    component = test.Component  # component type from test object
    if not _os.path.exists(component):
        _os.system("mkdir -p " + component)
    if test._numFiles > 1000:
        _os.chdir(component)
        numdirs = test._numFiles - _np.mod(test._numFiles, 1000)
        dirs = _np.linspace(0, numdirs, (numdirs / 1000) + 1)
        for direc in dirs:
            _os.system("mkdir -p " + _np.str(_np.int(direc)))
        _os.chdir('../')