.. _dev-analysisoutput:

Sensitivity, Output & Analysis
******************************

Information From Geant4
=======================

Geant4 allows the developer to gather information at every stage of the simulation, but is
by default a *quiet* simulation with no output generated.

BDSIM collects information broadly in two ways. Firstly, through sensitive detector ("SD")
classes that are attached to individual volumes and make a 'hit', which is a snapshot of
some desired information.  Secondly, through trajectory information. Trajectories are created
for the passage of a single particle throughout the whole geometry, whereas SDs only generate
information for particles taking steps in the volumes they are attached to.

Sensitive Detectors
-------------------

The role of a sensitive detector is to generate output information from steps taken in
a volume. It is registered to a 'logical volume' and generates hits when
particles pass through only the logical volume(s) it's attached to.

These 'hits' are then stored in a 'collection' (i.e. vector) that can be processed
at the end of an event.

BDSIM provides several SD classes to prepare different types of information. The
two main sensitive detector classes in BDSIM are `BDSEnergyCounterSD` and
`BDSSamplerSD`. These are responsible for generating energy deposition hits in
accelerator components and for recording information in sampler volumes respectively.
The hits are translated and put into output structures in the end of event action
in BDSEventAction.

There is usually only one instance of the sensitive detector class even though it
is attached to many logical volumes. These are held in the singleton class
BDSSDManager.

Attachment of Sensitive Detectors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All beam line objects in BDSIM generally attach the sensitive detector they want
at construction time. This is done through the BDSGeometryComponent base class
that keeps a map of G4LogicalVolume* to BDSSDType, a class enum(eration) for which
sensitive detector class to use.

During the placement of the beam lines in BDSDetectorConstruction, the SD classes
are retrieved from BDSSDManager and attached to the G4LogicalVolumes. At this stage
the user options are applied and if the hits are not required because some output
options are turned off, the SDs will not be attached saving CPU time and memory
during the simulation.

* Only SDs that are requried are attached based on the options.


Trajectory Creation \& Storage
------------------------------

Trajectory objects are created for each new track in the Geant4 user tracking action
hook (in `BDSTrackingAction`). If turned on for all particles, this would record
every piece of available information for the full simulation - an unmanageable amount
of data in large studies. Therefore, this is by default only turned on for primary
particles. The trajectory information is used by the Geant4 visualisers to view the
tracks, so it must be turned on in this case too.

If the user turns on trajectory storage, trajectories are created for every particle
in the event and then filtered to store only the ones of interest in the output
in the end of event action in `BDSEventAction`.

Output
======

BDSIM writes currently to one format which is ROOT (we call it "root event" format).
The code is designed so that other formats can be added in future without affecting
the current structure of the program or output formats.

Generally, we keep all the information we might want to store in the output in a
base class called BDSOutputStructures. This contains only the basic information
that will be stored. BDSOutput inherits this class and is an abstract interface for
any specific output format. BDSOutput helps fill the objects in BDSOutputStructures
as well as define the interface for all output.

Any output format is represented by a class that inherits BDSOutput, such as
BDSOutputROOT for example. We use ROOT objects and classes in BDSIM as they are
convenient for preparing the output information from a particle physics study.
BDSOutputROOT is relatively simple as it is relatively simple to store ROOT objects
in a ROOT file. Any other output format that is written would have access to the
information in the base class BDSOutputStructures and can translate and write that
as required. We also define ROOT dictionaries and create shared libraries with all
the classes used in the output so that they can be easily loaded again.

* Generally, we use SI units + GeV and ns in the output.

How A ROOT File Works
---------------------

The ROOT file system works by have an instance of whatever you want to store that
exists in the current memory. The file is laid out pointing to these local objects
(roughly SetBranchAddress('name in output', &localObject) ). The developer then
sets the values of the local object, i.e. to the data they want to store. The
Fill function (on a ROOT 'Tree') is then called which copies the data to the file.

Loading the file is done in reverse. First a local empty object is created, the file
is attached to it and GetEntry() is called which loads one entry from the output
into the local object, which can then be read by the user just as if they'd created
it then and there.

A ROOT file can store data as one of objects in the file (such as a histogram), but
the most common usage is with a 'Tree' (TTree class), that is really equivalent to
a vector. Whatever structure the tree has is duplicated for each 'entry'. In a tree,
there can be single objects or 'branches' with 'leaves' (so a maximum number of dimensions
of 2). These objects may be basic C++ types or ROOT objects, or classes defined by
the user. The ROOT file secretly stores a template of all classes stored in it, so
even if a user class is used to write a file and later on, the software is lost, the
data can still be read.

In BDSIM, the main output tree is called "Event" and each entry in that tree represents
one event in the simulation that starts with one primary particle fired into the accelerator.
Everything you see in the Event tree is replicated for each event but with different data
of course.

