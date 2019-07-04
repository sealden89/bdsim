import numpy as _np

import string as _string
from matplotlib import colors as _color
from matplotlib import ticker as _tick
import matplotlib.pyplot as _plt
import matplotlib.patches as _patches
import matplotlib.backends.backend_pdf as _bkpdf

import Globals

GlobalData = Globals.Globals()


def PlotResults(allResults, componentType=''):
    """ Function for plotting the testing results for a specific component.
        """

    GlobalData._CheckComponent(componentType)

    # get all results of this component type.
    res = allResults[componentType]

    # split results into proton and electron
    electronResults = res.GetResultsByParticle('e-')
    protonResults = res.GetResultsByParticle('proton')

    # use appropriate function depending on the number of tests per particle type
    if electronResults._numEntries == 0:
        _plotSingleParticle(protonResults)
    elif protonResults._numEntries == 0:
        _plotSingleParticle(electronResults)
    elif (protonResults._numEntries > 0) and (electronResults._numEntries > 0):
        if (protonResults._numEntries <= GlobalData.testsPerAxes) and (electronResults._numEntries <= GlobalData.testsPerAxes):
            fileName = 'Results/' + componentType + '.png'
            _singleFigureDoubleData(electronResults, protonResults, fileName)
        else:
            _plotSingleParticle(electronResults)
            _plotSingleParticle(protonResults)


def PlotTimingData(timingData, component):
    """ Plot the timing data as histograms."""
    f = _plt.figure()
    ax = f.add_subplot(121)
    ax2 = f.add_subplot(122)

    # max values to define histogram range
    bdsimMax = _np.max(timingData.bdsimTimes[component])
    compMax = _np.max(timingData.comparatorTimes[component])

    y, x, _ = ax.hist(timingData.bdsimTimes[component], bins=30, log=True, range=(0, _np.ceil(bdsimMax)))
    y2, x2, _ = ax2.hist(timingData.comparatorTimes[component], bins=30, log=True, range=(0, _np.ceil(compMax)))

    if (_np.max(y) < 100) and (_np.max(y2) < 100):

        ax.set_yscale("linear", nonposx='clip')
        ax2.set_yscale("linear", nonposx='clip')

        maxtimes = [_np.max(y), _np.max(y2)]

        ax.set_ylim(ymin=0, ymax=2.0*_np.max(maxtimes))
        ax2.set_ylim(ymin=0, ymax=2.0*_np.max(maxtimes))
    else:
        # calculate largest number of entries to get y-axis scale.
        maxtimes = [_np.max(y), _np.max(y2)]
        orderOfMag = _np.int(_np.log10(_np.max(maxtimes)))

        # plot on log scale, set min to 0.9 to show single entry bins.
        ax.set_ylim(ymin=0.9, ymax=2 * 10 ** orderOfMag)
        ax2.set_ylim(ymin=0.9, ymax=2 * 10 ** orderOfMag)

    ax.set_xlim(xmin=0)
    ax2.set_xlim(xmin=0)

    ax.set_xlabel('Time (s)')
    ax.set_ylabel('Number of Tests')
    ax.set_title('BDSIM Run Time')

    ax2.set_xlabel('Time (s)')
    ax2.set_title('Comparator Run Time')
    ax2.yaxis.set_visible(False)

    filename = 'Results/' + component + '_timingData'

    pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
    for i in _plt.get_fignums():
        pdf.savefig(i)
    pdf.close()


def _addColorBar(f, ax):
    """ Add a colorbar to the results plot."""
    # colorbar colors and values independant of data, can be set according to either subplot.
    numReturns = len(GlobalData.ReturnsAndErrors['name'])
    cbar = f.colorbar(ax)
    cbarTicks = _np.linspace(0.5, numReturns - 0.5, numReturns)
    cbar.set_ticks(cbarTicks)
    cbar.set_ticklabels(GlobalData.ReturnsAndErrors['name'])
    cbar.ax.tick_params(labelsize=8)
    f.tight_layout()


def _getNumFigures(num):
    """ Function to calculate the number of figures required based on the number of axes supplied."""
    remainder = _np.mod(num, 2)
    if remainder == 0:
        numFigures = num / 2.0
    else:
        numFigures = ((num - remainder) / 2.0) + 1
    return _np.int(numFigures)


def _getFigSize(res1, res2=None):
    """ Function to return the figure size based on the number of results that will
        be displayed. """
    # No need to have complicated algorithm, just return discrete sizes.
    if res2 is not None:
        if (res1._numEntries < 25) or (res2._numEntries < 25):
            figHeight = 6
        elif (res1._numEntries < 50) or (res2._numEntries < 50):
            figHeight = 9
        else:
            figHeight = 12
        if (res1._numEntries == 0) or (res2._numEntries == 0):
            figWidth = 8
        else:
            figWidth = 12
    else:
        if res1._numEntries < 25:
            figHeight = 6
        elif res1._numEntries < 50:
            figHeight = 9
        else:
            figHeight = 12
        if res1._numEntries == 0:
            figWidth = 8
        else:
            figWidth = 12

    return figWidth, figHeight


def _plotSingleParticle(results):
    """ Function for plotting the results for a single particle type."""

    # if number of tests is too high, split tests by particle energy, and call appropriate function.
    if results._numEntries > GlobalData.testsPerAxes:
        numEnergies = len(results.uniqueValues['energy'])
        if numEnergies == 1:
            _singleDataAxesByEnergy(results)
        else:
            numFigures = _getNumFigures(numEnergies)
            if (_np.mod(numFigures, 2) == 0) or (numFigures == 1):
                _doubleDataAxesByEnergy(numFigures, results)
            else:
                _doubleDataAxesByEnergy(numFigures - 1, results)
                _singleDataAxesByEnergy(results)
    else:
        fileName = 'Results/' + results._component + '.png'
        _singleFigureSingleData(results, fileName)


def _singleFigureDoubleData(results1, results2, filename):
    """ Function for plotting two sets of results on a single figure."""
    figsize = _getFigSize(results2, results1)
    f = _plt.figure(figsize=figsize)
    # 4 axes, two for the data, two for the label boxing structure.
    ax1 = f.add_subplot(141)
    ax2 = f.add_subplot(142)
    ax3 = f.add_subplot(143)
    ax4 = f.add_subplot(144)

    # calculate the offset for the parameter name that will be added to the label boxing structure.
    # Normalised so that the offsets should appear at the same vertical position on both axes.
    res1Size = _np.float(figsize[1] - 1) / len(results1)
    res2Size = _np.float(figsize[1] - 1) / len(results2)
    res1Offset = 1.0
    res2Offset = 1.0
    if res1Size > res2Size:
        res1Offset = res2Size / res1Size
    elif res1Size < res2Size:
        res2Offset = res1Size / res2Size

    # plot the data / label boxing.
    dataAx1 = _updateAxes(ax2, ax1, results1, res1Offset)
    dataAx2 = _updateAxes(ax4, ax3, results2, res2Offset)

    _addColorBar(f, dataAx1)

    pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
    for i in _plt.get_fignums():
        pdf.savefig(i)
    pdf.close()


def _doubleDataAxesByEnergy(numFigures, results):
    """ Function for plotting data sets that are split by energy on multiple figures."""
    # loop over number of figures
    for i in range(numFigures):
        # get results by energy
        energy1 = results.uniqueValues['energy'][2 * i]
        energy2 = results.uniqueValues['energy'][2 * i + 1]
        res1 = results.GetResultsByEnergy(energy1)
        res2 = results.GetResultsByEnergy(energy2)

        particle = results[0]['particle']
        energyString = '_' + particle + '_energies__' + energy1 + '_' + energy2
        fileName = 'Results/' + results._component + energyString + '.png'

        _singleFigureDoubleData(res1, res2, fileName)


def _singleDataAxesByEnergy(results):
    """ Function for plotting a data set with a single energy on a single figure."""

    # get results by energy. Use last energy in unique values (though there should only be one value.
    res1 = results.GetResultsByEnergy(results.uniqueValues['energy'][-1])
    energy1 = results.uniqueValues['energy'][-1]
    particle = results[0]['particle']
    energyString = '_' + particle + '_energies__' + energy1
    fileName = 'Results/' + results._component + energyString + '.png'
    _singleFigureSingleData(res1, fileName)


def _singleFigureSingleData(results, filename):
    """ Function to plot a single data set on a single figure."""
    figsize = _getFigSize(results)
    f = _plt.figure(figsize=figsize)
    if len(results) == 1:
        ax1 = None
        ax2 = f.add_subplot(111)
    else:
        ax1 = f.add_subplot(121)
        ax2 = f.add_subplot(122)

    dataAxes = _updateAxes(ax2, ax1, results, 0.1)
    _addColorBar(f, dataAxes)

    pdf = _bkpdf.PdfPages(filename+"_plots.pdf")
    for i in _plt.get_fignums():
        pdf.savefig(i)
    pdf.close()


def _updateAxes(dataAxis, diagramAxis, dataSetresults, labOffset):
    """ Function to update the axes of a figure."""

    # get uniques values of this data set.
    uniqueValues = dataSetresults.uniqueValues

    def updateDataAxis(ax, results):
        # get all necessary data.
        data = results.GetResults()
        generalStatus = results.GetGeneralStatus()
        particle = results[0]['particle']
        numTests = len(results)

        cmap = _color.ListedColormap(GlobalData.ReturnsAndErrors['colours'])

        # for a given test and phase space axis, the results may contain multiple values
        # e.g [passed, failed, no_data]. Here, we plot using the first of the multiple values
        # and add the remaining values on as boxes later.
        zeroData = []
        for test in data:
            dataRes = []
            for val in test:
                if type(val) in GlobalData.multiEntryTypes:
                    dataRes.append(val[0])
                else:
                    dataRes.append(val)
            zeroData.append(dataRes)

        # set normalised colormap.
        numReturns = len(GlobalData.ReturnsAndErrors['name'])
        bounds = _np.linspace(0, numReturns, numReturns + 1)
        norm = _color.BoundaryNorm(bounds, cmap.N)

        extent = [0, 7, 0, results._numEntries]

        commonValues = results.commonValues
        subplotTitle = particle

        # subplot title based on common parameter values.
        if commonValues is not None:
            subplotTitle += ", "
            for key, value in commonValues.iteritems():
                if key != 'energy':
                    subplotTitle += _string.capitalize(key) + " = " + value
                    index = commonValues.keys().index(key)
                    if index != (len(commonValues.keys()) - 1):
                        subplotTitle += ", "
                    # multiple lines if title is longer than 22 chars
                    if len(subplotTitle.split('\n')[-1]) > 22:
                        subplotTitle += '\n'

        # plot the data
        cax = ax.imshow(zeroData, interpolation='none', origin='lower', cmap=cmap, norm=norm,
                        extent=extent, aspect='auto')

        ax.set_xlim(0, 8)

        # overlay the data with boxes for the results values that were not plotted before.
        for index, status in enumerate(data):
            for coord, vals in enumerate(status):
                # only plot rectangles if there's more than one coord status value, imshow will
                # have already plotted that box correctly.
                if type(vals) in GlobalData.multiEntryTypes:
                    numStatus = len(vals)
                    yIndex = index
                    for statIndex, stat in enumerate(vals):
                        boxColor = cmap.colors[_np.int(stat)]
                        boxWidth = 1.0 / numStatus
                        ax.add_patch(_patches.Rectangle((coord + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

        # plot the general status for each test using boxes.
        for index, status in enumerate(generalStatus):
            # only plot rectangles if there's more than one general status value, imshow will
            # have already plotted that box correctly.
            if type(status) in GlobalData.multiEntryTypes:
                numStatus = len(status)
                yIndex = index
                for statIndex, stat in enumerate(status):
                    boxColor = cmap.colors[stat]
                    boxWidth = 1.0 / numStatus
                    ax.add_patch(_patches.Rectangle((7 + statIndex*boxWidth, yIndex), boxWidth, 1, color=boxColor))

        if subplotTitle != '':
            ax.set_title(subplotTitle)

        xtickMajors = _np.linspace(1, 8, 8)
        xtickCentre = xtickMajors - 0.5

        ax.set_xticks(xtickCentre)
        ax.set_xticklabels(['x', 'xp', 'y', 'yp', 't', 'zp', 'n', 'Gen'])

        if numTests > 1:
            ytickMajors = _np.linspace(numTests / (numTests - 1), numTests, numTests)
            ytickCentre = ytickMajors - 0.5

            ax.set_yticks(ytickCentre)
            empty_string_labels = [''] * numTests
            ax.set_yticklabels(empty_string_labels)

            ytickMinors = _np.linspace(0, len(data), len(data) + 1)
            minorYTicks = _tick.FixedLocator(ytickMinors)
            ax.yaxis.set_minor_locator(minorYTicks)
            ax.tick_params(axis='y', which='both', length=0, labelsize=9)
            ax.grid(which='minor', axis='y', linestyle='--')

        minorXTicks = _tick.FixedLocator(xtickMajors)
        ax.xaxis.set_minor_locator(minorXTicks)
        ax.tick_params(axis='x', which='both', length=0)
        ax.grid(which='minor', axis='x', linestyle='-')
        return cax

    def updateDiagramAxis(ax, results, labelOffset):
        # get all necessary data.
        params = results.GetParams()
        # dict for param value labels
        boxText = {}

        # calculate number of boxes per level
        numBoxesPerLevel = []
        numLevel = 1
        for i in range(len(uniqueValues.keys())):
            boxText[_np.str(i)] = []
            numValues = len(uniqueValues[uniqueValues.keys()[i]])
            if i == 0:  # alway include energy (1st key in uniqueValues)
                numLevel *= numValues
                numBoxesPerLevel.append(numLevel)
            elif numValues > 0:
                numLevel *= numValues
                numBoxesPerLevel.append(numLevel)

        # update dict of param values per box
        def updateBoxTextList(depth):
            for value in uniqueValues[uniqueValues.keys()[depth]]:
                boxText[_np.str(depth)].append(value)
                if depth < (len(uniqueValues.keys())-1):
                    updateBoxTextList(depth+1)

        # get text for each box
        updateBoxTextList(0)
        numLevels = len(numBoxesPerLevel)

        fontSizes = ['medium', 'medium', 'small', 'xx-small']

        # plot the lines representing the boxes.
        for level, numBoxes in enumerate(numBoxesPerLevel):
            ax.plot([level-numLevels, level-numLevels], [0, len(params)], linewidth=1, color='k')  # vertical bar
            for boxNum in range(numBoxes):
                boxHeight = len(params) / numBoxesPerLevel[level]
                textRot = 'horizontal'
                txtHorPos = level + 0.5
                txtVerPos = (boxNum * boxHeight) + 0.5 * boxHeight

                ax.plot([level - numLevels, 0], [boxNum*boxHeight, boxNum*boxHeight],
                        linewidth=1, color='k')  # bottom horizontal bar
                ax.plot([level - numLevels, 0], [(boxNum+1)*boxHeight, (boxNum+1)*boxHeight],
                        linewidth=1, color='k')  # top hor. bar
                ax.text(txtHorPos - numLevels, txtVerPos, boxText[_np.str(level)][boxNum],
                        rotation=textRot, va='center', ha='center', fontsize=fontSizes[level])  # param value
            dataValue = _string.capitalize(uniqueValues.keys()[level])
            ax.text(level+0.5 - numLevels, len(params) + labelOffset, dataValue,
                    fontsize=12, ha='center')  # parameter title above the boxes

        ax.set_xlim(-numLevels, 0)
        ax.set_ylim(0, results._numEntries)

        ax.axis('off')
        ax.xaxis.set_visible(False)
        ax.yaxis.set_visible(False)

        return ax

    # update axes
    if diagramAxis is None:
        dataAx1 = updateDataAxis(dataAxis, dataSetresults)
    else:
        dataAx1 = updateDataAxis(dataAxis, dataSetresults)
        labAx1 = updateDiagramAxis(diagramAxis, dataSetresults, labOffset)

    return dataAx1

