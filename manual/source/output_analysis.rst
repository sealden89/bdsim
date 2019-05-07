.. _output-analysis-section:

===============
Output Analysis
===============

This section describes how to load and view data from the recommended output **rootevent**
format.

See :ref:`basic-data-inspection` for how to view the data and make the most basic
on-the-fly histograms.

.. _output-analysis-setup:

Setup
=====

1) BDSIM must be installed after compilation for the analysis tools to function properly.
2) Environmental variables should be set by sourcing :code:`<bdsim-install-dir>/bin/bdsim.sh`.
3) A ROOT logon macro may optionally be written for convenience in loading libraries.

If the setup is correct, you should be able to execute 'rebdsim' in the terminal. See
:ref:`installation-building` and :ref:`installation-environmental-variables` for more
details.

.. figure:: figures/rebdsim_execution.png
	    :width: 100%
	    :align: center

If the analysis will be regularly used interactively, it is worth automating the library
loading in root by finding and editing the :code:`rootlogon.C` in your
:code:`<root-install-dir>/macros/` directory.  Example text would be::

  cout << "Loading rebdsim libraries" << endl;
  gSystem->Load("librebdsim");
  gSystem->Load("libbdsimRootEvent");

.. note:: The file extension is omitted on purpose.

The absolute path is not necessary, as the above environmental variables are used by ROOT
to find the library.

Quick Recipes
=============

Inspect Histograms
------------------

* Run rebdsimHistoMerge on BDSIM output file (quick).
* Browse output of rebdsimHistoMerge in TBrowser in ROOT.
* See :ref:`rebdsim-histo-merge` for details.

::

   rebdsimHistoMerge output.root results.root

Plot Energy Deposition \& Losses
--------------------------------

* Run rebdsimHistoMerge on BDSIM output file (quick).
* Plot in Python using `pybdsim` using dedicated plotting function.

::
   
   rebdsimHistoMerge output.root results.root
   ipython
   >>> import pybdsim
   >>> pybdsim.Plot.LossAndEnergyDeposition("results.root")


rebdsim - General Analysis Tool
===============================

BDSIM is accompanied by an analysis tool called `rebdsim` ("root event BDSIM")
that provides the ability to use simple text input files to specify histograms and process data.
It also provides the ability to calculate optical functions from the sampler data.

`rebdsim` is based on a set of analysis classes that are compiled into a library. These
may be used through `rebdsim`, but also through the ROOT interpreter and in a user's
ROOT macro or compiled code. They may also be used through Python if the user has
ROOT available through Python.

`rebdsim` is executed with one argument which is the path to an analysis configuration text
file. This is a simple text file that describes which histograms to make from the data.
Optionally, a second argument of a data file to operate on will override the one specified
in the analysis configuration file. This allows the same analysis configuration to be used
to analyse many different data files. A third optional argument (must have second argument
specified) is the output file name that the resultant analysis will be written to.

Examples::

  rebdsim analysisConfig.txt
  rebdsim analysisConfig.txt output.root
  rebdsim analysisConfig.txt output.root results.root

.. _analysis-preparing-analysis-config:

Preparing an Analysis Configuration File
----------------------------------------

The analysis configuration file is a simple text file. This can be prepared by copying
and editing an example. The text file acts as a thin interface to an analysis in ROOT
that would commonly use the :code:`TTree->Draw()` method.

We strongly recommend browsing the data in a TBrowser beforehand and double-clicking
the variables. This gives you an idea of the range of the data. See :ref:`basic-data-inspection`
for more details.

There are three types of histograms that `rebdsim` can produce:

1. "Simple" histograms - these are sum over all entries in that tree.
2. "Per-Entry" histograms - here an individual histogram is made for each entry in the
   tree and these are averaged across all entries. In the case of the Event tree, each
   entry is a single event. A per-entry histogram is therefore a per-event histogram.
3. "Merged" histograms - these are the mean taken across all entries of a histogram
   already in the output file. For example, there is an energy deposition histogram
   stored with each event. This would be merged into a per-event average.

Per-Entry and Simple Histograms
-------------------------------

For the energy deposition histogram for example, the energy deposition hits are binned
as a function of the curvilinear `S` position along the accelerator. In fact, the `S` position
is binned with the weight of the energy. In each event, a single primary particle can lead
to the creation of thousands of secondaries that can each create many energy deposition hits.
In the case of a simple histogram, all energy deposition hits across all events are binned.
This gives us a total for the simulation performed and the bin error (uncertainty associated
with a given histogram bin) is proportional to :math:`1/sqrt(N)`, where :math:`N` is the
number of entries in that bin. This, however, doesn't correctly represent the variation seen
from event to event. Using the per-event histograms, a single simple 1D histogram of energy
deposition is created and these are averaged. The resultant histogram has the mean per-event
(note the normalisation here versus the simple histograms) and the error on the bin is the
standard error on the beam, i.e.

.. math::
  \mathrm{bin~error} = \frac{\sigma}{\sqrt{n_{events}}}

where :math:`\sigma` is the standard deviation of the values in that bin for all events.

.. note:: Per-entry histograms will only be calculated where there exists two or more entries
	  in the tree. In the case of the Event tree, this corresponds to more than two events.

.. _output-analysis-configuration-file:
	  
Analysis Configuration File
---------------------------

The input text file has roughly two sections: options and histogram definitions.

Examples can be found in:

* `<bdsim>/examples/features/io/1_rootevent/analysisConfig.txt`
* `<bdsim>/examples/features/analysis/simpleHistograms/analysisConfig.txt`
* `<bdsim>/examples/features/analysis/perEntryHistograms/analysisConfig.txt`

::

  Debug                                   True
  InputFilePath                           ./output.root
  OutputFileName                          ./ana_1.root
  CalculateOpticalFunctions               True
  CalculateOpticalFunctionsFileName       ./ana_1.dat
  # Object  Tree Name Histogram Name  # of Bins  Binning             Variable            Selection
  Histogram1D  Event.    Primaryx        {100}      {-0.1:0.1}          Primary.x           1
  Histogram1D  Event.    Primaryy        {100}      {-0.1:0.1}          Primary.y           1
  Histogram1D  Options.  seedState       {200}      {0:200}             Options.GMAD::OptionsBase.seed 1
  Histogram1D  Model.    componentLength {100}      {0.0:100}           Model.length        1
  Histogram1D  Run.      runDuration     {1000}     {0:1000}            Summary.duration    1
  Histogram2D  Event.    XvsY            {100,100}  {-0.1:0.1,-0.1:0.1} Primary.x:Primary.y 1
  Histogram3D  Event.    PhaseSpace3D    {50,50,50} {-5e-6:5e-6,-5e-6:5e-6,-5e-6:5e-6} Primary.x:Primary.y:Primary.z 1
  Histogram1DLog Event.  PrimaryXAbs     {20}       {-9:-3}      abs(Primary.x)                 1
  Histogram2DLinLog Event. PhaseSpaceAbs {20,20}    {-1e-6:1e-5,-9:-3} Primary.x:abs(Primary.y) 1
  Histogram2DLog    Event. PhaseSpaceAbs2 {20,20}   {-9:-3,-1e-6:1e-5} abs(Primary.x):Primary.y 1


* :code:`HistogramND` defines an N-dimension per-entry histogram where `N` is 1,2 or 3.
* :code:`SimpleHistogramND` defines an N-dimension simple histogram where `N` is 1,2 or 3.
* Arguments in the histogram rows must not contain any white space!
* Columns in the histogram rows must be separated by any amount of white space (at least one space).
* A line beginning with :code:`#` is ignored as a comment line.
* Empty lines are also ignored.
* For bins and binning, the dimensions are separated by :code:`,`.
* For bins and binning, the range from low to high is specified by :code:`low:high`.
* For a 2D or 3D histogram, x vs. y variables are specified by :code:`samplername.y:samplername.x`.
  See warning below for order of variables.
* Variables must contain the full 'address' of a variable inside a Tree.
* Variables can also contain a value manipulation, e.g. :code:`1000*(Primary.energy-0.938)` (to get
  the kinetic energy of proton primaries in MeV).
* The selection is a weight. In the case of the Boolean expression, it is a weight of 1 or 0.
* Selection can be a Boolean operation (e.g. :code:`Primary.x>0`) or simply :code:`1` for all events.
* Multiple Boolean operations can be used e.g. :code:`Primary.x>0&&samplername.ParentID!=0`.
* If a Boolean and a weight is desired, multiply both with the Boolean in brackets, e.g.
  :code:`Eloss.energy*(Eloss.S>145.3)`.
* True or False, as well as 1 or 0, may be used for Boolean options at the top.
* ROOT special variables can be used as well, such as :code:`Entry$` amd :code:`Entries$`. See
  the documentation link immediately below.

.. note:: Per-entry histograms will only be calculated where there exists two or more entries
	  in the tree. In the case of the Event tree, this corresponds to more than two events.
	  Whilst the per-entry histograms will work for any tree in the output, they are primarily
	  useful for per-event analysis on the Event tree.

A full explanation on the combination of selection parameters is given in the ROOT TTree class:
`<https://root.cern.ch/doc/master/classTTree.html>`_.  See the "Draw" method and "selection".

Logarithmic Binning
-------------------

Logarithmic binning may be used by specifying 'Log' after 'HistogramND' for each dimension.
The dimensions specified in order are `x`, `y`, `z`. If a linearly spaced dimension is
required, the user should write 'Lin'. If nothing is specified it is assumed to be linear.

Examples::

  Histogram1D       // linearly spaced
  Histogram1DLog    // logarithmically spaced
  Histogram2D       // X and Y are linearly spaced
  Histogram2DLog    // X is logarithmically spaced and Y linearly
  Histgoram2DLinLog // X is linearly spaced and Y logarithmically

The bin's lower edges and upper edges should be an exponent of 10. For example, to generate
a 1D histogram with thirty logarithmically spaced bins from 1e-3 to 1e3, the following syntax
would be used::

  Histogram1DLog Event. EnergySpectrum {30} {-3:3} Eloss.energy 1

.. warning:: The variable for plotting is really a simple interface to CERN ROOT's TTree Draw
	     method.  This has some inconsistency.  If 1D, there is just `x`.  If 2D, it's
	     `y` : `x`. If 3D, it's `x` : `y` : `z`.  This *only* applies to the variable and
	     not to the bin specification.


Analysis Configuration Options
------------------------------

The following (case-insensitive) options may be specified in the top part.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------+------------------------------------------------------+
| **Option**                 | **Description**                                      |
+============================+======================================================+
| BackwardsCompatible        | ROOT event output files from BDSIM prior to v0.994   |
|                            | do not have the header structure that is used to     |
|                            | ensure the files are the right format and prevent    |
|                            | a segfault from ROOT. If this option is true, the    |
|                            | header will not be checked, allowing old files to be |
|                            | analysed.                                            |
+----------------------------+------------------------------------------------------+
| CalculateOptics            | Whether to calculate optical functions or not        |
+----------------------------+------------------------------------------------------+
| Debug                      | Whether to print out debug information               |
+----------------------------+------------------------------------------------------+
| EmittanceOnTheFly          | Whether to calculate the emittance freshly at each   |
|                            | sampler or simply use the emittance calculated from  |
|                            | the first sampler (i.e. the primaries). The default  |
|                            | is false and therefore calculates the emittance at   |
|                            | each sampler.                                        |
+----------------------------+------------------------------------------------------+
| EventStart                 | Event index to start from - zero counting. Default   |
|                            | is 0.                                                |
+----------------------------+------------------------------------------------------+
| EventEnd                   | Event index to finish analysis at - zero counting.   |
|                            | Default is -1 that represents how ever many events   |
|                            | there are in the file (or files if multiple are      |
|                            | being analysed at once).                             |
+----------------------------+------------------------------------------------------+
| InputFilePath              | The root event file to analyse (or regex for         |
|                            | multiple).                                           |
+----------------------------+------------------------------------------------------+
| MergeHistograms            | Whether to merge the event level default histograms  |
|                            | provided by BDSIM. Turning this off will             |
|                            | significantly improve the speed of analysis if only  |
|                            | separate user-defined histograms are desired.        |
+----------------------------+------------------------------------------------------+
| OutputFileName             | The name of the result file  written to              |
+----------------------------+------------------------------------------------------+
| OpticsFileName             | The name of a separate text file copy of the         |
|                            | optical functions output                             |
+----------------------------+------------------------------------------------------+
| PrintModuloFraction        | The fraction of events to print out (default 0.01).  |
|                            | If you require print out for every event, set this   |
|                            | to 0.                                                |
+----------------------------+------------------------------------------------------+
| ProcessSamplers            | Whether to load the sampler data or not              |
+----------------------------+------------------------------------------------------+



Variables In Data
-----------------

See :ref:`basic-data-inspection` for how to view the data and make the most basic
on-the-fly histograms.

.. _rebdsim-combine:

rebdsimCombine - Output Combination
===================================

`rebdsimCombine` is a tool that can combine `rebdsim` output files correctly
(i.e. the mean of the mean histograms) to provide the overall mean and error on
the mean, as if all events had been analysed in one execution of `rebdsim`.

The combination of the histograms from the `rebdsim` output files is very quick
in comparison to the analysis. `rebdsimCombine` is used as follows: ::

  rebdsimCombine <result.root> <file1.root> <file2.root> ....

where `<result.root>` is the desired name of the merge output file and `<file.root>` etc.
are input files to be merged. This workflow is shown schematically in the figure below.

.. _rebdsim-histo-merge:

rebdsimHistoMerge - Simple Histogram Merging
============================================

BDSIM, by default, records a few histograms per event that typically include the primary
particle impact and loss location as well as the energy deposition. The histograms are
stored in vectors inside the Event tree of the output. These cannot be viewed directly
in the ROOT TBrowser as they are in a vector. Even then, each histogram is for one event
only. To view the average of all the histograms, a dedicated tool is provided that provides
a subset of the `rebdsim` functionality. `rebdsim` would automatically combine these
histograms while performing analysis.

A tool `rebdsimHistoMerge` is provided to take the average of only the already existing
histograms without the need to prepare an analysis configuration file. It is run as
follows::

  rebdsimHistoMerge output.root results.root

This creates a ROOT file called "results.root" that contains the average histograms
across all events.  This can only operate on BDSIM output files, not `rebdsim`
output files.

rebdsimOptics - Optical Functions
=================================

`rebdsimOptics` is a tool to load sampler data from a BDSIM output file and calculate
optical functions as well as beam sizes. It is run as follows::

  rebdsimOptics output.root optics.root

This creates a ROOT file called "optics.root" that contains the optical functions
of the sampler data.

See :ref:`optical-validation` for more details.

.. _output-analysis-efficiency:

Speed & Efficiency
==================

Whilst the ROOT file IO is very efficient, the sheer volume of data to process can
easily result in slow running analysis. To combat this, only the minimal variables
should be loaded that need to be. `rebdsim` automatically activates only the 'ROOT
branches' it needs for the analysis. A few possible ways to improve performance are:

* Reduce number of 2D or 3D histograms if possible. Analysis is linear in time with number
  of bins.
* Remove unnecessary histograms from your analysis configuration file.
* Avoid unnecessary filters in the selection.
* Turn off optical function calculations if they're not needed or don't make sense, i.e.
  if you're analysing the spray from a collimator in a sampler, it makes no sense to
  calculate the optical functions of that distribution.
* Turn off the MergeHistograms option. If you're only making your own histograms, this should
  considerably speed up the analysis for a large number of events.

Simple histograms to not require loading each entry in the tree and an analysis with
only simple histograms will be quicker. Per-entry histograms of course, require loading
each entry.

`rebdsim` 'turns off' the loading of all data and only loads what is necessary for the
given analysis.

.. _output-analysis-scaling-up:

Scaling Up - Parallelising Analysis
-----------------------------------

For high-statistics studies, it's common to run multiple instances of BDSIM with different
seeds (different seeds ensures different results) on a high throughout the computer cluster.
There are two possible strategies to efficiently scale the statistics and analysis; both
produce numerically identical output but make different use of computing resources. The
more data stored per event in the output files, the longer it takes to load it from disk and
the longer the analysis. Similarly, the more events simulated, the longer the analysis will
take. Of course either strategy can be used.

Low-Data Volume
---------------

If the overall output data volume is relatively low, we recommend analysing all of the
output files at once with `rebdsim`. In the `Analysis Configuration File`_ file,
the `InputFilePath` should be specified as `"*.root"` to match all the root files
in the current directory.

.. note:: For `"*.root"` all files should be from the same simulation and only BDSIM
	  output files (i.e. not `rebdsim` output files).

`rebdsim` will 'chain' the files together to behave as one big file with all of the events.
This is shown schematically in the figure below.

.. figure:: figures/multiple_outputs_rebdsim.pdf
	    :width: 100%
	    :align: center

	    Schematic of strategy for a low volume of data produced from a computationally
	    intense simulation. Multiple instances of BDSIM are executed, each producing their
	    own output file. These are analysed all at once with `rebdsim`.

This strategy works best for a relatively low number of events and data volume (example
numbers might be < 10000 events and < 10 GB of data).

High-Data Volume
----------------

In this case, it is better to analyse each output file with `rebdsim` separately and then
combine the results. In the case of per-event histograms, `rebdsim` provides the mean
per event, along with the error on the mean for the bin error. A separate tool,
`rebdsimCombine`, can be used to combine these `rebdsim` output files into one single
file. This is numerically equivalent to analysing all the data in one execution of
`rebdsim` but significantly faster. See :ref:`rebdsim-combine` for more details.

.. figure:: figures/multiple_analyses.pdf
	    :width: 100%
	    :align: center

	    Schematic of strategy for a high-data volume analysis. Multiple instances of
	    BDSIM are executed in a script that then executes `rebdsim` with a suitable
	    analysis configuration. Only the output files from `rebdsim` are then combined
	    into a final output identical to what would have been produced from analysing
	    all data at once, but in vastly reduced time.


User Analysis
=============

Whilst `rebdsim` will cover the majority of analyses, the user may desire a more
detailed or customised analysis. Methods to accomplish this are detailed here for
interactive or compiled C++ with ROOT, or through Python.

The classes used to store and load data in BDSIM are packaged into a library. This
library can be used interactively in Python and ROOT to load the data manually.

Analysis in Python
------------------

This is the preferred method. Analysis in Python can be done using ROOT in Python
directly or through our library `pybdsim` (see :ref:`python-utilities`).

.. note:: ROOT must have been installed or compiled with Python support.

You can test whether ROOT works with your Python installation by starting Python and
trying to import ROOT - there should be no errors.

   >>> import ROOT

The library containing the analysis classes may be then loaded:

   >>> import ROOT
   >>> ROOT.gSystem.Load("librebdsim")
   >>> ROOT.gSystem.Load("libbdsimRootEvent")

The classes in :code:`bdsim/analysis` will now be available inside ROOT in Python.

This can also be conveniently achieved with pybdsim: ::

  >>> import pybdsim
  >>> pybdsim.Data.LoadROOTLibraries()

This raises a Python exception if the libraries aren't found correctly. This is done
automatically when any BDSIM output file is loaded using the ROOT libraries.

IPython
*******

We recommend using IPython instead of pure Python to allow interactive exploration
of the tools. After typing at the IPython prompt for example :code:`pybdsim.`, press
the tab key and all of the available functions and objects inside `pybdsim` (in this
case) will be shown.

For any object, function or class, type a question mark after it to see the docstring
associated with it. ::
  
  >>> import pybdsim
  >>> d = pybdsim.Data.Load("combined-ana.root")
  >>> d.
  d.ConvertToPybdsimHistograms d.histograms2d                
  d.filename                   d.histograms2dpy              
  d.histograms                 d.histograms3d               >
  d.histograms1d               d.histograms3dpy              
  d.histograms1dpy             d.histogramspy 

General Data Loading
********************

Any output file from the BDSIM set of tools can be loaded with: ::

  >>> import pybdsim
  >>> d = pybdsim.Data.Load("myoutputfile.root")

This will work for files from BDSIM, `rebdsim`, `rebdsimCombine`, `rebdsimHistoMerge`
and `rebdsimOptics`. This function may return a different type of object depending
on the file that was loaded. The two types are `DataLoader`, which is the same as
the `rebdsim` C++ class but in Python, and `RebdsimFile` (defined in
:code:`pybdsim/pybdsim/Data.py`), which is a Python class
to hold the output from a `rebdsim` output file and conveniently convert ROOT histograms
to numpy arrays. The type can easily be inspected: ::

  >>> type(d)
  pybdsim.Data.RebdsimFile
 

Looping Over Events
*******************

The following is an example of how to loop over events in a BDSIM output file using
pybdsim. ::

  >>> import pybdsim
  >>> import numpy
  >>> d = pybdsim.Data.Load("myoutputfile.root")
  >>> eventTree = d.GetEventTree()
  >>> for event in eventTree:
  ...     print numpy.array(event.Primary.x)

In this example, the variable :code:`event` will have the same structure as the
Event tree in the BDSIM output. See :ref:`basic-data-inspection` for more details
on how to browse the data.

.. note:: The branch "Summary" in the Event and Run trees used to be called "Info"
	  in BDSIM < V1.3. This conflicted with TOjbect::Info() so this looping in
	  Python would work for any data in this branch, hence the change.

Sampler Data
************

The following shows the convenience methods to access sampler data from a BDSIM
output file using pybdsim: ::

  >>> import pybdsim
  >>> import numpy
  >>> d = pybdsim.Data.Load("myoutputfile.root")
  >>> primaries = pybdsim.Data.SamplerData(d)
  >>> primaries.data.keys()
  ['weight',
  'trackID',
  'energy',
  'turnNumber',
  'parentID',
  'xp',
  'zp',
  'rigidity',
  'ionZ',
  'charge',
  'ionA',
  'modelID',
  'S',
  'T',
  'yp',
  'partID',
  'n',
  'mass',
  'y',
  'x',
  'z',
  'isIon']
  >>> primaries.data['x']
  array([0.001, 0.001, 0.001, ..., 0.001, 0.001, 0.001])

The :code:`SamplerData` function has an optional second argument that takes the
index (zero counting) of the sampler or the name as it appears in the file. This
includes the primaries ("Primary").

.. note:: This loads all data into memory at once and is generally not as efficient
	  as looping over event by event. This is provided for convenience, but may
	  not scale well to very large data sets.

.. warning:: This concatenates all events into one array, so the event by event
	     nature of the data is lost. This may be acceptable in some cases, but
	     it is worth considering making a 2D histogram directly using `rebdsim`
	     rather than say loading the sampler data here and making a 2D plot.
	     Certainly, if the statistical uncertainties are to be calculated, this
	     is a far preferable route.

REBDSIM Histograms
******************

Output from `rebdsim` can be loaded using pybdsim. The histograms made by `rebdsim`
are loaded as the ROOT objects they are, but are also converted to numpy arrays
using classes provided by pybdsim for convenience. The Python converted ones are
held in dictionaries suffixed with 'py'. The histograms are loaded into dictionaries
where the key is a string with the full path and name of the histogram in the `rebdsim`
output file. The value is the histogram from the file. ::

  >>> import pybdsim
  >>> d = pybdsim.Data.Load("rebdsimoutputfile.root")
  >>> d.histograms
  {'Event/MergedHistograms/ElossHisto': <ROOT.TH1D object ("ElossHisto") at 0x7fbe365e9520>,
  'Event/MergedHistograms/ElossPEHisto': <ROOT.TH1D object ("ElossPEHisto") at 0x7fbe365ea750>,
  'Event/MergedHistograms/ElossTunnelHisto': <ROOT.TH1D object ("ElossTunnelHisto") at 0x7fbe365eab40>,
  'Event/MergedHistograms/ElossTunnelPEHisto': <ROOT.TH1D object ("ElossTunnelPEHisto") at 0x7fbe365eaf30>,
  'Event/MergedHistograms/PhitsHisto': <ROOT.TH1D object ("PhitsHisto") at 0x7fbe365e8bd0>,
  'Event/MergedHistograms/PhitsPEHisto': <ROOT.TH1D object ("PhitsPEHisto") at 0x7fbe365e9cb0>,
  'Event/MergedHistograms/PlossHisto': <ROOT.TH1D object ("PlossHisto") at 0x7fbe365e8fc0>,
  'Event/MergedHistograms/PlossPEHisto': <ROOT.TH1D object ("PlossPEHisto") at 0x7fbe365ea0a0>,
  'Event/PerEntryHistograms/EnergyLossManual': <ROOT.TH1D object ("EnergyLossManual") at 0x7fbe365a3a50>,
  'Event/PerEntryHistograms/EnergySpectrum': <ROOT.TH1D object ("EnergySpectrum") at 0x7fbe365a2e20>,
  'Event/PerEntryHistograms/EventDuration': <ROOT.TH1D object ("EventDuration") at 0x7fbe325907b0>,
  'Event/PerEntryHistograms/TunnelDeposition': <ROOT.TH3D object ("TunnelDeposition") at 0x7fbe35e2c800>,
  'Event/PerEntryHistograms/TunnelLossManual': <ROOT.TH1D object ("TunnelLossManual") at 0x7fbe365a40b0>,
  'Event/SimpleHistograms/Primaryx': <ROOT.TH1D object ("Primaryx") at 0x7fbe325cf9d0>,
  'Event/SimpleHistograms/Primaryy': <ROOT.TH1D object ("Primaryy") at 0x7fbe325d0230>,
  'Event/SimpleHistograms/TunnelHitsTransverse': <ROOT.TH2D object ("TunnelHitsTransverse") at 0x7fbe30a7fe00>}
  >>> d.histogramspy
  {'Event/MergedHistograms/ElossHisto': <pybdsim.Data.TH1 at 0x12682fa10>,
  'Event/MergedHistograms/ElossPEHisto': <pybdsim.Data.TH1 at 0x12682f850>,
  'Event/MergedHistograms/ElossTunnelHisto': <pybdsim.Data.TH1 at 0x12682f690>,
  'Event/MergedHistograms/ElossTunnelPEHisto': <pybdsim.Data.TH1 at 0x12682f990>,
  'Event/MergedHistograms/PhitsHisto': <pybdsim.Data.TH1 at 0x12682f890>,
  'Event/MergedHistograms/PhitsPEHisto': <pybdsim.Data.TH1 at 0x12682f950>,
  'Event/MergedHistograms/PlossHisto': <pybdsim.Data.TH1 at 0x12682f7d0>,
  'Event/MergedHistograms/PlossPEHisto': <pybdsim.Data.TH1 at 0x12682f5d0>,
  'Event/PerEntryHistograms/EnergyLossManual': <pybdsim.Data.TH1 at 0x12682f810>,
  'Event/PerEntryHistograms/EnergySpectrum': <pybdsim.Data.TH1 at 0x122d577d0>,
  'Event/PerEntryHistograms/EventDuration': <pybdsim.Data.TH1 at 0x12682f910>,
  'Event/PerEntryHistograms/TunnelDeposition': <pybdsim.Data.TH3 at 0x116abe090>,
  'Event/PerEntryHistograms/TunnelLossManual': <pybdsim.Data.TH1 at 0x122d67190>,
  'Event/SimpleHistograms/Primaryx': <pybdsim.Data.TH1 at 0x12682f710>,
  'Event/SimpleHistograms/Primaryy': <pybdsim.Data.TH1 at 0x12682f790>,
  'Event/SimpleHistograms/TunnelHitsTransverse': <pybdsim.Data.TH2 at 0x12682fa50>}
  

  

Analysis in C++ or ROOT
-----------------------

The following commands can be used as either compiled C++ or as interactive C++ using
ROOT. Here, we show their usage using ROOT interactively.

When using ROOT's interpreter, you can use the functionality of the BDSIM classes
dynamically. First, you must load the shared library (if not done so in your ROOT logon
macro) to provide the classes::

  root> gSystem->Load("librebdsim");
  root> gSystem->Load("libbdsimRootEvent");

Loading this library exposes all classes that are found in :code:`<bdsim>/analysis`. If you
are familiar with ROOT, you may use the ROOT file as you would any other given the
classes provided by the library::

  root> TFile* f = new TFile("output.root", "READ");
  root> TTree* eventTree = (TTree*)f->Get("Event");
  root> BDSOutputROOTEventLoss* elosslocal = new BDSOutputROOTEventLoss();
  root> eventTree->SetBranchAddress("Eloss.", &elosslocal);
  root> eventTree->GetEntry(0);
  root> cout << elosslocal->n << endl;
        345
  root>

The header (".hh") files in :code:`<bdsim>/analysis` provide the contents and abilities
of each class.

General Data Loading
********************

This would of course be fairly tedious to load all the structures in the output. Therefore,
a data loader class is provided that constructs local instances of all the objects and
sets the branch address on them (links them to the open file). For example::

  root> gSystem->Load("librebdsim");
  root> gSystem->Load("libbdsimRootEvent");
  root> DataLoader* dl = new DataLoader("output.root");
  root> Event* evt = dl->GetEvent();
  root> TTree* evtTree = dl->GetEventTree();

Here, a file is loaded and by default all data is loaded in the file.

Looping Over Events
*******************

We get access to event by event information through a local event object and the linked
event tree (here, a chain of all files) provided by the DataLoader class. We can then load
a particular entry in the tree, which for the Event tree is an individual event::

  root> evtTree->GetEntry(10);

The event object now contains the data loaded from the file. ::

  root> evt->Eloss.n
  (int_t) 430

For our example, the file has 430 entries of energy loss for event \#10. The analysis loading
classes are designed to have the same structure as the output file. Look at
`bdsim/analysis/Event.hh` to see what objects the class has.

One may manually loop over the events in a macro::

  void DoLoop()
  {
    gSystem->Load("librebdsim");
    DataLoader* dl = new DataLoader("output.root");
    Event* evt = dl->GetEvent();
    TTree* evtTree = dl->GetEventTree();
    int nentries = (int)evtTree->GetEntries();
    for (int i = 0; i < nentries; ++i)
      {
        evtTree->GetEntry(i);
        std::cout << evt->Eloss.n << std::endl;
      }
  }

  root> .L myMacro.C
  root> DoLoop()


This would loop over all entries and print the number of energy deposition hits per
event.

Sampler Data
************

Samplers are dynamically added to the output based on the names the user decides in
their input accelerator model. The names of the samplers can be accessed from the
DataLoader class::

  std::vector<std::string> samplerNames = dl->GetSamplerNames();


REBDSIM Histograms
******************

To load histograms, the user should open the ROOT file and access the histograms directly.::

  root> TFile* f = new TFile("output.root");
  root> TH1D* eloss = (TH1D*)f->Get("Event/MergedHistograms/ElossHisto");

It is recommended to use a TBrowser to get the exact names of objects in the file.

  
Output Classes
**************

The following classes are used for data loading and can be found in `bdsim/analysis`:

* DataLoader.hh
* Beam.hh
* Event.hh
* Header.hh
* Model.hh
* Options.hh
* Run.hh

Numerical Methods
=================

Algorithms used to accurately calculate quantities are described here. These are
documented explicitly as a simple implementation of the mathematical formulae
would result in an inaccurate answer in some cases.

Numerically Stable Calculation of Mean \& Variance
--------------------------------------------------

To calculate the mean in the per-entry histograms as well as the associated error
(the standard error on the mean), the following formulae are used:

.. math::

   \bar{x} &= \sum_{i = 0}^{n} x_{i}\\
   \sigma_{\bar{x}} &= \frac{1}{\sqrt{n}}\sigma = \frac{1}{\sqrt{n}}\sqrt{\frac{1}{n}\sum_{i = 0}^{n}(x_{i} - \bar{x})^2 }

These equations are however problematic to implement computationally. The formula above
for the variance requires two passes through the data to first calculate the mean,
then the variance using that mean. The above equation can be rearranged to provide the same
calculation with a single pass through the data, however, such algorithms are typically
numerically unstable, i.e. they rely on a small difference between two very large numbers.
With the finite precision of a number represented in a C++ double type (~15 significant
digits), the instability may lead to un-physical results (negative variances) and generally
incorrect results.

The algorithm used in `rebdsim` to calculate the means and variances is an online, single-pass
numerically stable one. This means that the variance is calculated as each data point
is accumulated, it requires only one pass of the data, and does not suffer numerical instability.
To calculate the mean, the following recurrence relation is used:

.. math::

   \bar{x}_{i = 0} &= 0\\
   \bar{x}_{i+1} &= \bar{x}_{i} + \frac{(x - \bar{x}_{i})}{i}\\

   \mathrm{for}~ i~ [1\, ...\, n_{event}]


The variance is calculated with the following recurrence relation that requires the above
online mean calculation:

.. math::

   Var\,(x)_{i = 0} &= 0 \\
   Var\,(x)_{i+1} &= Var\,(x)_{i} + (x - \bar{x}_{i})\,(x - \bar{x}_{i+1})\\

   \mathrm{for}~ i~ [1\, ... \,n_{event}]

After processing all entries, the variance is used to calculate the standard error on the mean
with:

.. math::

   \sigma_{\bar{x}} = \frac{1}{\sqrt{n}}\sqrt{\frac{1}{\sqrt{n-1}} Var\,(x)}


Merging Histograms
------------------

`rebdsimCombine` merges histograms that already have the mean and the error on the
mean in each bin. These are combined with a separate algorithm that is also numerically
stable.

The mean is calculated as:

.. math::

   \bar{x}_{i = 0} &= 0\\
   \delta &= x_{i+1} - \bar{x}_{i}\\
   \bar{x}_{i+1} &= \bar{x}_{i} + n_{i+1}\frac{\delta}{n_{i} + n_{i+1}}


.. math::

   Var\,(x)_{i = 0} &= 0 \\
   Var\,(x)_{i+1} &= Var\,(x)_{i} + Var\,(x)_{i+1} + (n_{i}\,n_{i+1} \frac{\delta^{2}}{n_{i} + n_{i+1}})


.. math::

   \mathrm{for}~ i~ [1\, ... \,n_{rebdsim\, files}]
