.. macro for non breaking white space usefulf or units:
.. |nbsp| unicode:: 0xA0
   :trim:

.. _model-control:

=============
Model Control
=============

* :ref:`sampler-output`

  - :ref:`sampler-syntax`
  - :ref:`sampler-dimensions`
  - :ref:`sampler-visualisation`
  - :ref:`user-sampler-placement`
    
* :ref:`physics-processes`

  - :ref:`physics-modular-physics-lists`
  - :ref:`physics-geant4-lists`
  - :ref:`physics-complete-lists`
    
* :ref:`bdsim-options`
* :ref:`beam-parameters`
* :ref:`controlling-simulation-speed`
* More details about :ref:`bend-tracking-behaviour`


.. _sampler-output:

Output at a Plane - Samplers
----------------------------

BDSIM provides a `sampler` as a means to observe the particle distribution at a
point in the lattice. A sampler is 'attached' to an already defined element
and records all the particles passing through a plane at the **exit** face of
that element.

A sampler will record any particles passing through that plane in any direction.
It is defined in reality by a box 5 x 5 m that is 1 nm thick. The user
may consider it an infinitely thin plane.

.. _sampler-syntax:

Attaching a Sampler to a Beamline Element
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

They are defined using the following syntax::

  sample, range=<element_name>;

where `element_name` is the name of the element you wish to sample. Depending on the
output format chosen, the element name may be recorded in the output ('rootevent' output only).

.. note:: Samplers **can only** be defined **after** the main sequence has been defined
	  using the `use` command (see :ref:`the-use-command`). Failure to do
	  so will result in an error and BDSIM will exit.

.. note:: Samplers record **all** particles impinging on them (i.e. both forwards and
	  backwards). Even secondary particles that may originate from further along the
	  lattice are recorded. They have no material so they do not absorb or affect particles, only
	  witness them.

To place a sampler before an item, attach it to the previous item. If however,
you wish to record the coordinates with another name rather than the name of the
element before, you can define a marker; place it in the sequence; and then define
a sampler that uses that marker::

  d1: drift, l=2.4*m;
  d2: drift, l=1*m;
  interestingplane: marker;
  l1: line=(d1,d1,interestingplane,d2,d1);
  use,period=l1;

  sample, range = interestingplane;

When an element is defined multiple times in the line (such as "d1" in the above example),
samplers will be attached to all instances. If you wish to sample only one specific
instance, the following syntax can be used::

  sample, range=<element_name>[index];

To attach samplers after all elements: ::

  sample, all;

And to attach samplers after all elements of a specific type::

  sample, <type>;

e.g. ::

  sample, quadrupole;

.. note:: If a sampler is placed at the very beginning of the lattice, it may appear
	  that only approximately half of the primary particles seem to pass through it. This
	  is the correct behaviour, as unlike an optics program such as MAD-X, the sampler
	  represents a thin plane in 3D space in BDSIM. If the beam distribution has some
	  finite extent in *z* or *t*, particles may start beyond this first sampler and
	  never pass through it.

.. warning:: The record of the primary particle coordinates in the output ("Primary") may
	     resemble a sampler but it is just a record of the initial coordinates. It is
	     not a sampler and cannot record other secondary particles.

.. _sampler-dimensions:
	  
Sampler Dimensions
^^^^^^^^^^^^^^^^^^

The sampler is represented by a box solid that is 1 nm thick along z and 5m wide
transversely in x and y. If a smaller or larger capture area for the samplers is required,
the option *samplerDiameter* may be specified in the input gmad. ::

  option, samplerDiameter=3*m;

This affects all samplers.

.. note:: For a very low energy lattice with large angle bends, the default samplerDiameter
	  may cause geometrical overlap warnings from Geant4. This situation is difficult to
	  avoid automatically, but easy to remedy by setting the samplerDiameter to a lower
	  value. We recommend reducing :code:`samplerDiameter` for low energy or strongly
	  curving accelerators.

.. _sampler-visualisation:
	  
Sampler Visualisation
^^^^^^^^^^^^^^^^^^^^^

The samplers are normally invisible and are built in a parallel world geometry in Geant4. To
visualise them, the following command should be used in the visualiser::

  /vis/drawVolume worlds

The samplers will appear in semi-transparent green, as well as the curvilinear geometry used
for coordinate transforms (cylinders).

.. _user-sampler-placement:

Output at an Arbitrary Plane - User Placed Sampler
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The user may place a sampler anywhere in the model with any orientation. This is called a
`samplerplacement`. The sampler may have either a circular or rectangular (including
square) shape and be placed with any orientation. A `samplerplacement` will record all
particles travelling in any direction through it. A branch in the Event output will be
create with the name of the `samplerplacement`. The user may define an arbitrary number of
`samplerplacement`s.  A `samplerplacement` is defined with the following syntax::

  s1: samplerplacement, referenceElement="d1",
                        referenceElementNumber=1,
			x=20*cm, y=-1*cm, z=30*cm,
			axisAngle=1, axisY=1, angle=pi/4,
			aper1=10*cm;

This defines a circular (by default) sampler with radius 10 cm positioned with respect to
the 2nd instance of the d1 element (zero counting) in the main beam line with a rotation
about the unit Y axis of :math:`\pi / 4`.

Placement
*********

A `samplerplacement` may be placed in 3 ways.

1) In global Cartesian coordinates.
2) In curvilinear coordinates.
3) In curvilinear coordinates with respect to a beam line element by name.

The strategy is automatically determined based on the parameters set. The full list of
parameters is described below, but the required ones for each scenario are described in
:ref:`placements`.

.. warning:: This sampler can nominally overlap with any geometry. However, the user
	     should **avoid** co-planar overlaps with other geometry. e.g. do not place
	     one just at the end of an element or perfectly aligned with the face of an
	     object. This will cause bad tracking and overlaps. This is a limitation of
	     Geant4. The user placed samplers are slightly thicker than normal ones
	     to help avoid this problem.

Shape
*****

The sampler will be 1 nm thick in reality but may be treated by the user an
infinitely thin plane. It is composed of vacuum and should not interfere with the ongoing
physics of the simulation. The user may select the shape of the sampler from either
circular or rectangular (including square). The parameter :code:`apertureType` should
be specified as either :code:`"circular"` or :code:`"rectangular"`. The aperture parameters
typically used in BDSIM should also be used - these are :code:`aper1` and :code:`aper2`.
The meaning of these parameters is described in :ref:`aperture-parameters`.

Parameters
**********
			
The `samplerplacement` object accepts a number of parameters similar to other parts of BDSIM.
These include a subset of the aperture parameters (see :ref:`aperture-parameters`) and the position
information from the `placements`. The full list of accepted parameters is given below.

+-------------------------+--------------------------------------------------------------------+
| **Parameter**           |  **Description**                                                   |
+-------------------------+--------------------------------------------------------------------+
| x                       | Offset in global x                                                 |
+-------------------------+--------------------------------------------------------------------+
| y                       | Offset in global y                                                 |
+-------------------------+--------------------------------------------------------------------+
| z                       | Offset in global z                                                 |
+-------------------------+--------------------------------------------------------------------+
| s                       | Curvilinear s coordinate (global | local depending on parameters)  |
+-------------------------+--------------------------------------------------------------------+
| phi                     | Euler angle phi for rotation                                       |
+-------------------------+--------------------------------------------------------------------+
| theta                   | Euler angle theta for rotation                                     |
+-------------------------+--------------------------------------------------------------------+
| psi                     | Euler angle psi for rotation                                       |
+-------------------------+--------------------------------------------------------------------+
| axisX                   | Axis angle rotation x-component of unit vector                     |
+-------------------------+--------------------------------------------------------------------+
| axisY                   | Axis angle rotation y-component of unit vector                     |
+-------------------------+--------------------------------------------------------------------+
| axisZ                   | Axis angle rotation z-component of unit vector                     |
+-------------------------+--------------------------------------------------------------------+
| angle                   | Axis angle, angle to rotate about unit vector                      |
+-------------------------+--------------------------------------------------------------------+
| axisAngle               | Boolean whether to use axis angle rotation scheme (default false)  |
+-------------------------+--------------------------------------------------------------------+
| sensitive               | Whether the geometry records energy deposition (default true)      |
+-------------------------+--------------------------------------------------------------------+
| referenceElement        | Name of element to place geometry with respect to (string)         |
+-------------------------+--------------------------------------------------------------------+
| referenceElementNumber  | Occurence of `referenceElement` to place with respect to if it     |
|                         | is used more than once in the sequence. Zero counting.             |
+-------------------------+--------------------------------------------------------------------+
| apertureType            | The shape of the sampler desired as described using the aperture   |
|                         | syntax of BDSIM. Currently, only `circular` and `rectangular` are  |
|                         | supported.                                                         |
+-------------------------+--------------------------------------------------------------------+
| shape                   | An intuitive alias to `apertureType`.                              |
+-------------------------+--------------------------------------------------------------------+
| aper1                   | Aperture parameter #1.                                             |
+-------------------------+--------------------------------------------------------------------+
| aper2                   | Aperture parameter #2.                                             |
+-------------------------+--------------------------------------------------------------------+
| aper3                   | Aperture parameter #3.                                             |
+-------------------------+--------------------------------------------------------------------+
| aper4                   | Aperture parameter #4.                                             |
+-------------------------+--------------------------------------------------------------------+

Examples
********

The following are examples of `samplerplacement`::

   s1: samplerplacement, referenceElement="d1",
                         referenceElementNumber=1,
			 x=20*cm, y=-1*cm, z=30*cm,
    			 axisAngle=1, axisY=1, angle=pi/4,
			 aper1=10*cm;


This places a circular sampler called "s1" with respect to the 2nd instance of the beam line
element "d1". The x,y,z are offsets from the centre of this element along the direction of
travel of the beam. The sampler is rotated about the unit Y axis (again with respect to the
centre of the beam line element rotation) by an angle of :math:`\pi / 4`. The sampler will
be circular (by default) with a radius of 10 cm. ::

   s2: samplerplacement, x=0.2*m, y=-1*cm, z=30.123*m,
    			 axisAngle=1, axisY=1, angle=-pi/6,
			 aper1=10*cm, aper2=5*cm, shape="rectangular";


This will place a sampler called "s2" in global Cartesian coordinates approximately 30 m
forward from the centre of model. The placement rotation is done in the global coordinate
system. The sampler shape is rectangular and is 20 cm wide and 10 cm tall.

User Sampler Visualisation
**************************

Samplers are by default invisible. To visualise the samplerplacement, all samplers should be
visualised as described in :ref:`sampler-visualisation`. The scene tree can then be explored
in the visualiser to hide other hidden volumes (such as the 'curvilinear' coordinate transform
worlds) and other samplers. It is recommended to tick and un-tick the desired element to see
it appear and disappear repeatedly.



.. _physics-processes:

Physics Processes
-----------------

BDSIM can exploit all the physics processes that come with Geant4. It is advantageous to
define **only** the processes required so that the simulation covers the desired outcome
want but is also efficient. Geant4 says, "There is no one model that covers all physics
at all energy ranges."

By default, only tracking in magnetic fields is provided (e.g. **no** physics) and other
processes must be specified to be used.

Rather than specify each individual particle physics process on a per-particle basis,
a series of "physics lists" are provided that are a predetermined set of physics processes
suitable for a certain application. BDSIM follows the Geant4 ethos in this regard and the
majority of those in BDSIM are simple shortcuts to the Geant4 ones.

There are 3 ways to specify physics lists in BDSIM:

1) BDSIM's modular physics lists as described in :ref:`physics-modular-physics-lists`: ::

     option, physicsList = "em qgsp_bert";
     
These are modular and can be added independently. BDSIM provides a 'physics list' for
a few discrete processes that aren't covered inside Geant4 reference physics lists such as
crystal channelling and cherenkov radiation. It is possible to create a physics list similar
to a Geant4 reference physics list using BDSIM's modular approach as internally Geant4 does
the same thing.

2) Geant4's reference physics lists as described in :ref:`physics-geant4-lists`: ::

     option, physicsList = "g4FTFTP_BERT";

These are more complete "reference physics lists" that use several modular physics lists from Geant4
like BDSIM but in a predefined way that Geant4 quote for references results. These have rather confusingly
similar names. :code:`ftfp_bert` causes BDSIM to use :code:`G4HadronPhysicsFTFP_BERT` whereas
:code:`g4FTFP_BERT` uses :code:`FTFP_BERT` in Geant4. We refer the pattern 1) as 'modular physics lists'
and pattern 2) as Geant4 reference physics lists.

3) A *complete* physics list. This is a custom solution for a particular application that is
   hard coded in BDSIM. These all start with 'complete'. See :ref:`physics-complete-lists`. ::

     option, physicsList = "completechannelling";


For general high energy hadron physics we recommend::

  option, physicsList = "em ftfp_bert decay muon hadronic_elastic em_extra"


Some physics lists are only available in later versions of Geant4. These are filtered at compile
time for BDSIM and it will not recognise a physics list that requires a later version of Geant4
than BDSIM was compiled with respect to.

A summary of the available physics lists in BDSIM is provided below (others can be easily added
by contacting the developers - see :ref:`feature-request`).

See the Geant4 documentation for a more complete explanation of the physics lists.

* `Physics List Guide <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/PhysicsListGuide/html/physicslistguide.html>`_
* `User Case Guide <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/PhysicsListGuide/html/reference_PL/index.html>`_

.. _physics-modular-physics-lists:
  
Modular Physics Lists
^^^^^^^^^^^^^^^^^^^^^

A modular phyiscs list can be made by specifying several physics lists separated by spaces. These
are independent.

* The strings for the modular physics list are case-insensitive.

Examples: ::

  option, physicsList="em ftfp_bert";

  option, physicsList="em_low decay ion hadron_elastic qgsp_bert em_extra;
  

.. warning:: Not all physics lists can be used with all other physics lists. BDSIM will print
	     a warning and exit if this is the case. Generally, lists suffixed with "hp" should
	     not be used along with the unsuffixed ones (e.g. "qgsp_bert" and "qgsp_bert_hp" should
	     not be used together). Similarly, the standard electromagnetic variants should not
	     be used with the regular "em".

.. tabularcolumns:: |p{5cm}|p{10cm}|

+------------------------------+------------------------------------------------------------------------+
| **String to use**            | **Description**                                                        |
+==============================+========================================================================+
|                              | Transportation of primary particles only - no scattering in material   |
+------------------------------+------------------------------------------------------------------------+
| all_particles                | All particles definitions are constructed but no physics processes are |
|                              | created and attached to them. Useful for exotic beams. Note by default |
|                              | we only construct the necessary particles. It is more efficient to     |
|                              | keep the particle set to the minimum. This uses G4LeptonConstructor,   |
|                              | G4ShortLivedConstructor, G4MesonConstructor, G4BaryonConstructor and   |
|                              | G4IonConstructor.                                                      |
+------------------------------+------------------------------------------------------------------------+
| charge_exchange              | `G4ChargeExchangePhysics`                                              |
+------------------------------+------------------------------------------------------------------------+
| channelling                  | This constructs the `G4Channelling` and attaches it to all charged     |
|                              | particles. Note this physics process will only work in crystals. This  |
|                              | alone will not give an accurate representation of the distribution     |
|                              | after a crystal as EM physics is required. Multiple scattering should  |
|                              | not be used in combination with this however to achieve the correct    |
|                              | results.                                                               |
+------------------------------+------------------------------------------------------------------------+
| cherenkov                    | Provides Cherenkov radiation for all charged particles. Issued by the  |
|                              | BDSIM physics builder `BDSPhysicsCherenkov` that provides the process  |
|                              | `G4CherenkovProcess`.                                                  |
+------------------------------+------------------------------------------------------------------------+
| decay                        | Provides radioactive decay processes using `G4DecayPhysics`. Crucial   |
|                              | for pion decay for example.                                            |
+------------------------------+------------------------------------------------------------------------+
| decay_radioactive            | Radioactive decay of long-lived nuclei. Uses                           |
|                              | `G4RadioactiveDecayPhysics`.                                           |
+------------------------------+------------------------------------------------------------------------+
| decay_muonic_atom            | `G4MuonicAtomDecayPhysics`. Available from Geant4.10.3 onwards.        |
+------------------------------+------------------------------------------------------------------------+
| decay_spin                   | Decay physics, but with spin correctly implemented. Note: only the     |
|                              | Geant4 tracking integrators track spin correctly. Uses                 |
|                              | `G4SpinDecayPhysics`. Available from Geant4.10.2.p01 onwards.          |
+------------------------------+------------------------------------------------------------------------+
| dna                          | G4EmDNAPhysics list.  Only applies to G4_WATER material.               |
+------------------------------+------------------------------------------------------------------------+
| dna_1                        | Variant 1 of G4EmDNAPhysics list. Uses G4EmDNAPhysics_option1.         |
+------------------------------+------------------------------------------------------------------------+
| dna_X                        | Variant X of G4EmDNAPhysics list, where X is one of 1,2,3,4,5,6,7.     |
+------------------------------+------------------------------------------------------------------------+
| em                           | Transportation of primary particles, ionisation, Bremsstrahlung,       |
|                              | Cherenkov, multiple scattering. Uses `G4EmStandardPhysics`.            |
+------------------------------+------------------------------------------------------------------------+
| em_extra                     | This provides extra electromagnetic models, including muon-nuclear     |
|                              | processes and the Bertini electro-nuclear model. Provided by           |
|                              | `G4EmPhysicsExtra`. Responds to the  option `useLENDGammaNuclear` that |
|                              | requires the `G4LENDDATA` environmental variable to be set for the     |
|                              | optional LEND data set (see ** below). Additional options described    |
|                              | below also allow different parts of this model to be turned on or off. |
+------------------------------+------------------------------------------------------------------------+
| em_gs                        | `G4EmStandardPhysicsGS`. Available from Geant4.10.2 onwards.           |
+------------------------------+------------------------------------------------------------------------+
| em_livermore                 | `G4EmLivermorePhysics`                                                 |
+------------------------------+------------------------------------------------------------------------+
| em_livermore_polarised       | `G4EmLivermorePolarizedPhysics`                                        |
+------------------------------+------------------------------------------------------------------------+
| em_low_ep                    | `G4EmLowEPPhysics`                                                     |
+------------------------------+------------------------------------------------------------------------+
| em_penelope                  | The same as `em`, but using low-energy electromagnetic models. Uses    |
|                              | `G4EmPenelopePhysics`                                                  |
+------------------------------+------------------------------------------------------------------------+
| em_ss                        | `G4EmStandardPhysicsSS`                                                |
+------------------------------+------------------------------------------------------------------------+
| em_wvi                       | `G4EmStandardPhysicsWVI`                                               |
+------------------------------+------------------------------------------------------------------------+
| em_1                         | `G4EmStandardPhysics_option1`                                          |
+------------------------------+------------------------------------------------------------------------+
| em_2                         | `G4EmStandardPhysics_option2`                                          |
+------------------------------+------------------------------------------------------------------------+
| em_3                         | `G4EmStandardPhysics_option3`                                          |
+------------------------------+------------------------------------------------------------------------+
| em_4                         | `G4EmStandardPhysics_option4`                                          |
+------------------------------+------------------------------------------------------------------------+
| ftfp_bert                    | Fritiof Precompound Model with Bertini Cascade Model. The FTF model    |
|                              | is based on the FRITIOF description of string excitation and           |
|                              | fragmentation. This is provided by `G4HadronPhysicsFTFP_BERT`. All     |
|                              | FTF physics lists require `G4HadronElasticPhysics` to work correctly.  |
+------------------------------+------------------------------------------------------------------------+
| ftfp_bert_hp                 | Similar to `FTFP_BERT`, but with the high precision neutron package.   |
|                              | This is provided by `G4HadronPhysicsFTFP_BERT_HP`.                     |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic             | Elastic hadronic processes. This is provided by                        |
|                              | `G4HadronElasticPhysics.`                                              |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic_d           | `G4HadronDElasticPhysics`                                              |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic_h           | `G4HadronHElasticPhysics`                                              |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic_hp          | `G4HadronElasticPhysicsHP`                                             |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic_lend (`**`) | `G4HadronElasticPhysicsLEND`                                           |
+------------------------------+------------------------------------------------------------------------+
| hadronic_elastic_xs          | `G4HadronElasticPhysicsXS`                                             |
+------------------------------+------------------------------------------------------------------------+
| ion                          | `G4IonPhysics`                                                         |
+------------------------------+------------------------------------------------------------------------+
| ion_binary (`*`)             | `G4IonBinaryCascadePhysics`                                            |
+------------------------------+------------------------------------------------------------------------+
| ion_elastic                  | `G4IonElasticPhysics`                                                  |
+------------------------------+------------------------------------------------------------------------+
| ion_elastic_qmd              | `G4IonQMDPhysics`                                                      |
+------------------------------+------------------------------------------------------------------------+
| ion_em_dissocation           | Electromagnetic dissociation for ions. Uses `G4EMDissociation`. May    |
|                              | produce warnings. Experimental.                                        |
+------------------------------+------------------------------------------------------------------------+
| ion_inclxx (`*`)             | `G4IonINCLXXPhysics`                                                   |
+------------------------------+------------------------------------------------------------------------+
| ion_php (`*`)                | `G4IonPhysicsPHP`. Available from Geant4.10.3 onwards.                 |
+------------------------------+------------------------------------------------------------------------+
| lw                           | Laserwire photon producing process as if the laserwire had scattered   |
|                              | photons from the beam. Not actively developed, but will register       |
|                              | process.                                                               |
+------------------------------+------------------------------------------------------------------------+
| muon                         | Provides muon production and scattering processes. Gamma to muons,     |
|                              | annihilation to muon pair, 'ee' to hadrons, pion decay to muons,       |
|                              | multiple scattering for muons, muon Bremsstrahlung, pair production    |
|                              | and Cherenkov light are all provided. Given by BDSIM physics           |
|                              | builder (a la Geant4) `BDSPhysicsMuon`.                                |
+------------------------------+------------------------------------------------------------------------+
| neutron_tracking_cut         | `G4NeutronTrackingCut` allows neutrons to be killed via their tracking |
|                              | time (i.e. time of flight) and minimum kinetic energy. These options   |
|                              | are set via the option command, `neutronTimeLimit` (s) and             |
|                              | `neutronKineticEnergyLimit` (GeV).                                     |
+------------------------------+------------------------------------------------------------------------+
| optical                      | Optical physics processes including absorption, Rayleigh scattering,   |
|                              | Mie scattering, optical boundary processes, scintillation and          |
|                              | Cherenkov. This uses `G4OpticalPhysics` class.                         |
+------------------------------+------------------------------------------------------------------------+
| qgsp_bert                    | Quark-Gluon String Precompound Model with Bertini Cascade model.       |
|                              | This is based on the `G4HadronPhysicsQGSP_BERT` class and includes     |
|                              | hadronic elastic and inelastic processes. Suitable for high energy     |
|                              | (>10 GeV).                                                             |
+------------------------------+------------------------------------------------------------------------+
| qgsp_bert_hp                 | Similar to `QGSP_BERT`, but with the addition of data-driven high      |
|                              | precision neutron models to transport neutrons below 20 MeV down to    |
|                              | thermal energies. This is provided by `G4HadronPhysicsQGSP_BERT_HP`.   |
+------------------------------+------------------------------------------------------------------------+
| qgsp_bic                     | Like `QGSP`, but using Geant4 Binary cascade for primary protons and   |
|                              | neutrons with energies below ~10GeV, thus replacing the use of the LEP |
|                              | model for protons and neutrons. In comparison to the LEP model, Binary |
|                              | cascade better describes production of secondary particles produced    |
|                              | from interactions of protons and neutrons with nuclei. This is         |
|                              | provided by `G4HadronPhysicsQGSP_BIC`.                                 |
+------------------------------+------------------------------------------------------------------------+
| qgsp_bic_hp                  | Similar to `QGSP_BIC`, but with the high precision neutron package.    |
|                              | This is provided by `G4HadronPhysicsQGSP_BIC_HP`.                      |
+------------------------------+------------------------------------------------------------------------+
| shielding                    | `G4HadronPhysicsShielding`. Inelastic hadron physics suitable for      |
|                              | shielding applications.                                                |
+------------------------------+------------------------------------------------------------------------+
| shielding_lend  (`**`)       | `G4HadronPhysicsShieldingLEND`. Similar to `shielding`, but requires   |
|                              | LEND data set for low-energy neutrons. Available from Geant4.10.4      |
|                              | onwards.                                                               |
+------------------------------+------------------------------------------------------------------------+
| stopping                     | `G4StoppingPhysics`. Hadronic physics for stopping particles.          |
+------------------------------+------------------------------------------------------------------------+
| synch_rad                    | Provides synchrotron radiation for all charged particles. Provided by  |
|                              | BDSIM physics builder `BDSPhysicsSynchRad` that provides the process   |
|                              | `G4SynchrotronRadiation`.                                              |
+------------------------------+------------------------------------------------------------------------+

The following are also accepted as aliases to current physics lists. These are typically previously
used names.

.. tabularcolumns:: |p{5cm}|p{5cm}|

+---------------------------+--------------------------+
| **Physics List**          | **Alias To**             |
+===========================+==========================+
| cerenkov                  | cherenkov                |
+---------------------------+--------------------------+
| em_low                    | em_penelope              |
+---------------------------+--------------------------+
| hadronic                  | ftfp_bert                |
+---------------------------+--------------------------+
| hadronic_hp               | ftfp_bert_hp             |
+---------------------------+--------------------------+
| ionbinary                 | ion_binary               |
+---------------------------+--------------------------+
| ioninclxx                 | ion_inclxx               |
+---------------------------+--------------------------+
| ionphp                    | ion_php                  |
+---------------------------+--------------------------+
| spindecay                 | decay_spin               |
+---------------------------+--------------------------+
| synchrad                  | synch_rad                |
+---------------------------+--------------------------+


.. warning:: (*) These physics lists require the optional high-precision data from Geant4. The user should
	     download this data from the Geant4 website and install it (for example: extract to
	     <install-dir>/share/Geant4-10.3.3/data/ beside the other data) and export the environmental
	     variable `G4PARTICLEHPDATA` to point to this directory.

.. warning:: (**) These physics lists require the optional LEND data set that can be downloaded
	     from the Geant4 website. It should be extracted and the environmental variable
	     `G4LENDDATA` set to the directory containing it.


em_extra Physics Notes
^^^^^^^^^^^^^^^^^^^^^^

The em_extra model is an interface to `G4EmExtraPhysics` that collects a variety of extra electromagnetic models
together. Not all of these are activated by default. BDSIM provides options to turn these components on and off.
See :ref:`physics-process-options` for more details on the specific options.

+------------------------+-----------------------------+--------------+
| **Option**             | **Minimum Geant4 Version**  | **Default**  |
+========================+=============================+==============+
| useLENDGammaNuclear    | 10.4                        | Off          |
+------------------------+-----------------------------+--------------+
| useElectroNuclear      | 10.4                        | On           |
+------------------------+-----------------------------+--------------+
| useMuonNuclear         | 10.2                        | On           |
+------------------------+-----------------------------+--------------+
| useGammaToMuMu         | 10.3                        | Off          |
+------------------------+-----------------------------+--------------+
| usePositronToMuMu      | 10.3                        | Off          |
+------------------------+-----------------------------+--------------+
| usePositronToHadrons   | 10.3                        | Off          |
+------------------------+-----------------------------+--------------+

Example::

  option, physicsList="em em_extra",
          useMuonNuclear=1,
          useGammaToMuMu=1;

The options will always be accepted by BDSIM if an earlier version of Geant4 is used than supported, however,
these will have no effect.

`G4EmExtraPhysics` provides a simple interface to increase the cross-section of some processes. This interface
is not used in BDSIM, as it does not propagate the associated weights correctly. Biasing should be done through
the generic biasing interface with the name of the process (described in the following section), as this will
propagate the weights correctly.

.. _physics-geant4-lists:

Geant4 Reference Physics Lists
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

BDSIM allows use of the Geant4 reference physics lists directly and more details can be found in the Geant4
documentation:

* `Physics List Guide`_
* `User Case Guide <http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/PhysicsListGuide/html/reference_PL/index.html>`_

Notes:

* Only one Geant4 reference physics list can be used and it cannot be used in combination
  with any modular physics list.
* The range cuts specified with BDSIM options apply by default and the option
  :code:`g4PhysicsUseBDSIMRangeCuts` should be set to 0 ('off') to avoid this
  if required. The defaults are 1 mm, the same as Geant4.
* If the option :code:`minimumKineticEnergy` is set to a value greater than 0 (the default), a
  physics process will be attached to the Geant4 reference physics list to enforce this cut. This
  must be 0 and :code:`g4PhysicsUseBDSIMCutsAndLimits` option off to **not** use the physics
  process to enforce cuts and limits and therefore achieve the exact reference physics list. This
  is the default option.

.. warning:: Turning off all limits may result in tracking warnings. The events should still proceed
	     as normal, but Geant4 by default requests step lengths of 10 km or more, which often
	     break the validity of the accelerator tracking routines. This is unavoidable, hence
	     why we use the limits by default. BDSIM, by default applies step length limits of 110%
	     the length of each volume.
  
The following reference physics lists are included as of Geant4.10.4.p02. These **must** be
prefix with "g4" to work in BDSIM.

* FTFP_BERT
* FTFP_BERT_TRV
* FTFP_BERT_ATL
* FTFP_BERT_HP
* FTFQGSP_BERT
* FTFP_INCLXX
* FTFP_INCLXX_HP
* FTF_BIC
* LBE
* QBBC
* QGSP_BERT
* QGSP_BERT_HP
* QGSP_BIC
* QGSP_BIC_HP
* QGSP_BIC_AllHP
* QGSP_FTFP_BERT
* QGSP_INCLXX
* QGSP_INCLXX_HP
* QGS_BIC
* Shielding
* ShieldingLEND
* ShieldingM
* NuBeam

The **optional** following suffixes may be added to specify the electromagnetic physics variant:

* _EMV
* _EMX
* _EMY
* _EMZ
* _LIV
* _PEN
* __GS
* __LE
* _WVI
* __SS

Examples: ::

  option, physicsList="g4QBBC";

  option, physicsList="g4QBBC_EMV";

  option, physicsList="g4FTFP_BERT_PEN",
          g4PhysicsUSeBDSIMCutsAndLimits=0;

This last example turns off the minimum kinetic energy and also the maximum step length
limit which is by default 110% the length of the element. If bad tracking behaviour
is experienced (stuck particles etc.) this should be considered. ::
	  
  option, physicsList="g4FTFP_BERT";

This following example will enforce a minimum kinetic energy but also limit the maximum step length
(consequently) to 110% the length of the component and provide more robust tracking. ::

  option, physicsList="g4FTFP_BERT",
          minimumKineticEnergy=20*GeV;

.. note:: "g4" is not case sensitive but the remainder of the string is. The remainder is passed
	  to the Geant4 physics list that constructs the appropriate physics list and this is
	  case sensitive.

.. _physics-complete-lists:
   
Complete Physics Lists
^^^^^^^^^^^^^^^^^^^^^^

These are complete physics lists provided for specialist applications. Currently, only one is provided
for crystal channelling physics. These all begin with "complete".

These cannot be used in combination with any other physics processes.

+---------------------------+---------------------------------------------------------------------------+
| **Physics List**          | **Description**                                                           |
+===========================+===========================================================================+
| completechannelling       | Modified em option 4 plus channelling as per the Geant4 example           |
|                           | for crystal channelling. The exact same physics as used in their example. |
+---------------------------+---------------------------------------------------------------------------+

.. note:: The range cuts specified with BDSIM options to not apply and cannot be used with a 'complete'
	  physics list.

.. _bdsim-options:

Options
-------

Various simulation details can be controlled through the `option` command. Options are defined
using the following syntax::

  option, <option_name>=<value>;

Values accepted can be a number (integer, floating point or scientific notation), a string
with the value enclosed in "double inverted commas", or a Boolean. For Boolean options (described
as on or off, or true or false) a number 1 or 0 is used.

Multiple options can be defined at once using the following syntax::

  option, <option1> = <value>,
          <option2> = <value>;

.. note:: No options are required to be specified to run a BDSIM model.  Defaults will be used in
	  all cases.  However, we do recommend you select an appropriate physics list and beam pipe
	  radius, as these will have a large impact on the outcome of the simulation.

Below is a full list of all options in BDSIM. Please also see :ref:`executable-options` for options
that are used on the command line when executing BDSIM. The executable options override whatever
options are specified in the input gmad files.

Common options are duplicated below for convenience as these are the most useful ones. All options
are described in the following sub-sections:

* `General Run Options`_
* `Geometry Options`_
* `Tracking Options`_
* :ref:`physics-process-options`
* `Visualisation`_
* `Output Options`_
* :ref:`one-turn-map`
* :ref:`bdsim-options-verbosity`
* `Offset for Main Beam Line`_
* `Scoring Map`_
* `Developer Options`_

.. _options-common:

Common Options
^^^^^^^^^^^^^^

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| beampipeRadius                   | Default beam pipe inner radius [m]                    |
+----------------------------------+-------------------------------------------------------+
| beampipeThickness                | Default beam pipe thickness [m]                       |
+----------------------------------+-------------------------------------------------------+
| beampipeMaterial                 | Default beam pipe material                            |
+----------------------------------+-------------------------------------------------------+
| elossHistoBinWidth               | The width of the default energy deposition and        |
|                                  | particle loss histogram bins made as BDSIM runs [m]   |
|                                  | Default 1.0 m                                         |
+----------------------------------+-------------------------------------------------------+
| eventNumberOffset                | Event that the recreation should start from           |
+----------------------------------+-------------------------------------------------------+
| hStyle                           | Whether default dipole style is H-style vs. C-style   |
|                                  | (default false)                                       |
+----------------------------------+-------------------------------------------------------+
| ngenerate                        | Number of primary particles to simulate               |
+----------------------------------+-------------------------------------------------------+
| nturns                           | The number of revolutions that the particles are      |
|                                  | allowed to complete in a circular accelerator.        |
|                                  | Requires --circular executable option to work.        |
+----------------------------------+-------------------------------------------------------+
| horizontalWidth                  | Default accelerator component full width [m] Default  |
|                                  | is 0.5 m.                                             |
+----------------------------------+-------------------------------------------------------+
| physicsList                      | The physics list to use                               |
+----------------------------------+-------------------------------------------------------+
| printFractionEvents              | How often to print out the event number as a fraction |
|                                  | of the total number of events to simulation (default  |
|                                  | is 0.1 (i.e. 10%).  Varies from 0 to 1. -1 for all.   |
+----------------------------------+-------------------------------------------------------+
| printFractionTurns               | How often to print out the turn number as a fraction  |
|                                  | of the total number of turns to simulation (default   |
|                                  | is 0.2 (i.e. 20%).  Varies from 0 to 1. -1 for all.   |
|                                  | Will only print out in an event that also prints out. |
+----------------------------------+-------------------------------------------------------+
| printPhysicsProcesses            | (Boolean) Print out every particle registered         |
|                                  | according to the physics list and for each particle,  |
|                                  | print out the name of every physics process           |
|                                  | registered to it. Done at the start of a run. Run 1   |
|                                  | particle for minimal job to see this output.          |
+----------------------------------+-------------------------------------------------------+
| prodCutPhotons                   | Standard overall production cuts for photons          |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutElectrons                 | Standard overall production cuts for electrons        |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutPositrons                 | Standard overall production cuts for positrons        |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutProtons                   | Standard overall production cuts for protons          |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| removeTemporaryFiles             | Whether to delete temporary files (typically gdml)    |
|                                  | when BDSIM exits. Default true.                       |
+----------------------------------+-------------------------------------------------------+
| seed                             | The integer seed value for the random number          |
|                                  | generator                                             |
+----------------------------------+-------------------------------------------------------+
| stopSecondaries                  | Whether to stop secondaries or not (default = false)  |
+----------------------------------+-------------------------------------------------------+
| worldMaterial                    | The default material surrounding the model. This is   |
|                                  | by default air.                                       |
+----------------------------------+-------------------------------------------------------+

.. _options-general-run:

General Run Options
^^^^^^^^^^^^^^^^^^^

For a description of recreating events, see :ref:`running-recreation`.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| ngenerate                        | Number of primary particles to simulate               |
+----------------------------------+-------------------------------------------------------+
| nturns                           | The number of revolutions particles are allowed to    |
|                                  | complete in a circular accelerator - requires         |
|                                  | --circular executable option to work.                 |
+----------------------------------+-------------------------------------------------------+
| printFractionEvents              | How often to print out the event number as a fraction |
|                                  | of the total number of events to simulation (default  |
|                                  | is 0.1 i.e. 10%.  Varies from 0 to 1. -1 for all.     |
+----------------------------------+-------------------------------------------------------+
| printFractionTurns               | How often to print out the turn number as a fraction  |
|                                  | of the total number of turns to simulation (default   |
|                                  | is 0.2 i.e. 20%.  Varies from 0 to 1. -1 for all.     |
|                                  | Will only print out in an event that also prints out. |
+----------------------------------+-------------------------------------------------------+
| recreate                         | Whether to use recreation mode or not (default 0). If |
|                                  | used as an executable option, this should be a string |
|                                  | with a path to the :code:`recreateFileName`.          |
+----------------------------------+-------------------------------------------------------+
| recreateFileName                 | Path to BDSIM output file to use for recreation.      |
+----------------------------------+-------------------------------------------------------+
| removeTemporaryFiles             | Whether to delete temporary files (typically gdml)    |
|                                  | when BDSIM exits. Default true.                       |
+----------------------------------+-------------------------------------------------------+
| seed                             | The integer seed value for the random number          |
|                                  | generator                                             |
+----------------------------------+-------------------------------------------------------+
| startFromEvent                   | Number of event to start from when recreating. 0      |
|                                  | counting.                                             |
+----------------------------------+-------------------------------------------------------+
| writeSeedState                   | Writes the seed state of the last event start in      |
|                                  | ASCII                                                 |
+----------------------------------+-------------------------------------------------------+

.. _options-geometry:

Geometry Options
^^^^^^^^^^^^^^^^

These affect the construction of the 3D model in BDSIM. Tunnel parameters are also
described in :ref:`tunnel-geometry`.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| aper1                            | Default aper1 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper2                            | Default aper2 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper3                            | Default aper3 parameter                               |
+----------------------------------+-------------------------------------------------------+
| aper4                            | Default aper4 parameter                               |
+----------------------------------+-------------------------------------------------------+
| beampipeRadius                   | Default beam pipe inner radius - alias for aper1 [m]  |
+----------------------------------+-------------------------------------------------------+
| beampipeThickness                | Default beam pipe thickness [m]                       |
+----------------------------------+-------------------------------------------------------+
| beampipeMaterial                 | Default beam pipe material                            |
+----------------------------------+-------------------------------------------------------+
| buildTunnel                      | Whether to build a tunnel (default = 0)               |
+----------------------------------+-------------------------------------------------------+
| buildTunnelStraight              | Whether to build a tunnel, ignoring the beamline and  |
|                                  | just in a straight line (default = 0).                |
+----------------------------------+-------------------------------------------------------+
| buildTunnelFloor                 | Whether to add a floor to the tunnel                  |
+----------------------------------+-------------------------------------------------------+
| checkOverlaps                    | Whether to run Geant4's geometry overlap checker      |
|                                  | during geometry construction (slower)                 |
+----------------------------------+-------------------------------------------------------+
| coilWidthFraction                | 0.05 - 0.98 - fraction of available horizontal space  |
|                                  | between pole and yoke that coil will occupy           |
+----------------------------------+-------------------------------------------------------+
| coilHeightFraction               | 0.05 - 0.98 - fraction of available vertical space    |
|                                  | between pole tip and yoke that coil will occupy       |
+----------------------------------+-------------------------------------------------------+
| dontSplitSBends                  | If true, do not split sbends into multiple segments   |
|                                  | (default = false).                                    |
+----------------------------------+-------------------------------------------------------+
| emptyMaterial                    | This is 'vacuum' material where no material is        |
|                                  | required. Note: this is not 'vacuum' in the beam pipe,|
|                                  | but the lowest density material (G4_GALACTIC) Geant4  |
|                                  | can muster, as all materials must have a finite       |
|                                  | density. This is used for the gap between             |
|                                  | tight-fitting container volumes and objects.          |
+----------------------------------+-------------------------------------------------------+
| horizontalWidth                  | The default full width of a magnet                    |
+----------------------------------+-------------------------------------------------------+
| hStyle                           | Whether default dipole style is H-style vs. C-style   |
|                                  | (default false)                                       |
+----------------------------------+-------------------------------------------------------+
| ignoreLocalAperture              | If this is true (1), any per-element aperture         |
|                                  | definitions will be ignored and the ones specified    |
|                                  | in Options will be used.                              |
+----------------------------------+-------------------------------------------------------+
| ignoreLocalMagnetGeometry        | If this is true (1), any per-element magnet geometry  |
|                                  | definitions will be ignored and the ones specified    |
|                                  | in Options will be used.                              |
+----------------------------------+-------------------------------------------------------+
| includeIronMagFields             | Whether to include magnetic fields in the magnet      |
|                                  | poles                                                 |
+----------------------------------+-------------------------------------------------------+
| magnetGeometryType               | The default magnet geometry style to use              |
+----------------------------------+-------------------------------------------------------+
| outerMaterial                    | The default material to use for the yoke of magnet    |
|                                  | geometry                                              |
+----------------------------------+-------------------------------------------------------+
| preprocessGDML                   | Whether to prepend the element name at the front of   |
|                                  | every tag in a temporary copy of the GDML file.       |
|                                  | loaded. This is to compensate for the Geant4 GDML     |
|                                  | loader that cannot load multiple files correctly. On  |
|                                  | by default.                                           |
+----------------------------------+-------------------------------------------------------+
| preprocessGDMLSchema             | Whether to preprocess a copy of the GDML file where   |
|                                  | the URL of the GDML schema is changed to a local copy |
|                                  | provided in BDSIM so geometry can be loaded without   |
|                                  | internet access. On by default.                       |
+----------------------------------+-------------------------------------------------------+
| removeTemporaryFiles             | Whether to delete temporary files (typically gdml)    |
|                                  | when BDSIM exits. Default true.                       |
+----------------------------------+-------------------------------------------------------+
| samplerDiameter                  | Diameter of samplers (default 5 m) [m]. This is also  |
|                                  | the diameter of the curvilinear world volumes used in |
|                                  | curvilinear transforms. In the case of lower energy   |
|                                  | machines with strong bending angles (10s of degrees), |
|                                  | this should be reduced to prevent overlaps between    |
|                                  | curvilinear volumes along the beam line.              |
+----------------------------------+-------------------------------------------------------+
| sensitiveBeamPipe                | Whether the beam pipe records energy loss. This       |
|                                  | includes cavities.                                    |
+----------------------------------+-------------------------------------------------------+
| sensitiveOuter                   | Whether the outer part of each component (other than  |
|                                  | the beam pipe records energy loss                     |
+----------------------------------+-------------------------------------------------------+
| sensitiveVacuum                  | Whether energy deposition in the residual vacuum gas  |
|                                  | is recorded.                                          |
+----------------------------------+-------------------------------------------------------+
| soilMaterial                     | Material for soil outside tunnel wall                 |
+----------------------------------+-------------------------------------------------------+
| thinElementLength                | The length of all thinmultipoles and dipole           |
|                                  | fringefields in a lattice (default 1e-6) [m]          |
+----------------------------------+-------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | Whether all particles entering the tunnel material    |
|                                  | should be killed or not (default = false)             |
+----------------------------------+-------------------------------------------------------+
| tunnelType                       | Which style of tunnel to use - one of:                |
|                                  | `circular`, `elliptical`, `square`, `rectangular`,    |
|                                  | `ilc`, or `rectaboveground`.                          |
+----------------------------------+-------------------------------------------------------+
| tunnelAper1                      | Tunnel aperture parameter #1 - typically              |
|                                  | horizontal [m]                                        |
+----------------------------------+-------------------------------------------------------+
| tunnelAper2                      | Tunnel aperture parameter #2 - typically              |
|                                  | vertical [m]                                          |
+----------------------------------+-------------------------------------------------------+
| tunnelFloorOffset                | The offset of the tunnel floor from the centre of the |
|                                  | tunnel (**not** the beam line)                        |
+----------------------------------+-------------------------------------------------------+
| tunnelMaterial                   | Material for tunnel wall                              |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetX                    | Horizontal offset of the tunnel with respect to the   |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetY                    | Vertical offset of the tunnel with respect to the     |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelSoilThickness              | Soil thickness outside tunnel wall [m]                |
+----------------------------------+-------------------------------------------------------+
| tunnelThickness                  | Thickness of tunnel wall [m]                          |
+----------------------------------+-------------------------------------------------------+
| vacuumMaterial                   | The material to use for the beam pipe vacuum          |
|                                  | (default = "Vacuum")                                  |
+----------------------------------+-------------------------------------------------------+
| vacuumPressure                   | The pressure of the vacuum gas [bar]                  |
+----------------------------------+-------------------------------------------------------+
| vhRatio                          | Default vertical to horizontal ratio for dipoles      |
+----------------------------------+-------------------------------------------------------+
| worldVolumeMargin                | The margin added in all directions to the world       |
|                                  | volume [m]. Default 5m, minimum 2m.                   |
+----------------------------------+-------------------------------------------------------+
| worldMaterial                    | The default material surrounding the model. This is   |
|                                  | by default air.                                       |
+----------------------------------+-------------------------------------------------------+
| worldGeometryFile                | The filename of the world geometry file. See          |
|                                  | :ref:`external-world-geometry` for more details.      |
|                                  | Default = "".                                         |
+----------------------------------+-------------------------------------------------------+
| yokeFields                       | Whether to include a general multipolar field for     |
|                                  | the yoke of each magnet (using a fourth order         |
|                                  | Runge-Kutta integrator). Default true.                |
+----------------------------------+-------------------------------------------------------+

.. _options-tracking:

Tracking Options
^^^^^^^^^^^^^^^^

These control over the tracking routines used, as well as roughly the speed of the simulation
with various options.

Tracking integrator sets are described in detail in :ref:`integrator-sets` and
:ref:`integrator_algorithms_section`.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| collimatorsAreInfiniteAbosrbers  | When turned on, all particles that enter the material |
|                                  | of a collimator (`rcol`, `ecol` and `jcol`) are       |
|                                  | killed and the energy recorded as deposited there.    |
+----------------------------------+-------------------------------------------------------+
| includeFringeFields              | Places thin fringefield elements on the end of bending|
|                                  | magnets with finite poleface angles, and solenoids.   |
|                                  | The length of the total element is conserved.         |
|                                  | (default = true).                                     |
+----------------------------------+-------------------------------------------------------+
| includeFringeFieldsCavities      | Include thin fringe fields for RF cavities only.      |
|                                  | Cavity fringes are not affected by the                |
|                                  | includeFringeFields option,                           |
|                                  | includeFringeFieldsCavities must be explicitly turned |
|                                  | off if no fringes are to be built at all in the model.|
|                                  | (default = true).                                     |
+----------------------------------+-------------------------------------------------------+
| integratorSet                    | Set of tracking routines to use ("bdsimmatrix",       |
|                                  | "bdsimtwo", "bdsimmatrixfringescaling", "geant4", or  |
|                                  | "geant4dp")                                           |
+----------------------------------+-------------------------------------------------------+
| killNeutrinos                    | Whether to always stop tracking neutrinos for         |
|                                  | increased efficiency (default = true)                 |
+----------------------------------+-------------------------------------------------------+
| maximumStepLength                | Maximum step length [m] (default = 20 m)              |
+----------------------------------+-------------------------------------------------------+
| maximumTrackingTime              | The maximum time of flight allowed for any particle   |
|                                  | before it is killed [s]                               |
+----------------------------------+-------------------------------------------------------+
| maximumTrackLength               | The maximum length in metres of any track passing     |
|                                  | through any geometry in the model (not including the  |
|                                  | world volume)                                         |
+----------------------------------+-------------------------------------------------------+
| minimumKineticEnergy             | A particle below this energy will be killed and the   |
|                                  | energy deposition recorded at that location [GeV]     |
+----------------------------------+-------------------------------------------------------+
| minimumKineticEnergyTunnel       | A particle below this energy in any BDSIM-generated   |
|                                  | tunnel sections will be killed and the energy         |
|                                  | deposition recorded at that location [GeV]            |
+----------------------------------+-------------------------------------------------------+
| minimumRadiusOfCurvature         | Minimum tolerable radius of curvature of a particle,  |
|                                  | below which, the energy will be decreased by 2% on    |
|                                  | each use of the integrators to prevent infinite       |
|                                  | loops - should be just greater than width of beam     |
|                                  | pipe [m].                                             |
+----------------------------------+-------------------------------------------------------+
| minimumRange                     | A particle that would not travel this range           |
|                                  | (a distance) in the current material will be cut [m]  |
+----------------------------------+-------------------------------------------------------+
| ptcOneTurnMapFileName            | File name for a one turn map prepared in PTC that is  |
|                                  | used in the teleporter to improve the accuracy of     |
|                                  | circular tracking. See :ref:`one-turn-map`.           |
+----------------------------------+-------------------------------------------------------+
| stopSecondaries                  | Whether to stop secondaries or not (default = false)  |
+----------------------------------+-------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | Whether all particles entering the tunnel material    |
|                                  | should be killed or not (default = false)             |
+----------------------------------+-------------------------------------------------------+

.. _physics-process-options:

Physics Processes
^^^^^^^^^^^^^^^^^

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| defaultBiasVacuum                | Name of bias object to be attached to vacuum volumes  |
|                                  | by default                                            |
+----------------------------------+-------------------------------------------------------+
| defaultBiasMaterial              | Name of bias object to be attached to general         |
|                                  | material of components outside the vacuum by default  |
+----------------------------------+-------------------------------------------------------+
| defaultRangeCut                  | The default predicted range at which a particle is    |
|                                  | cut. Overwrites other production cuts unless these    |
|                                  | are explicitly set (default 1e-3) [m].                |
+----------------------------------+-------------------------------------------------------+
| g4PhysicsUseBDSIMCutsAndLimits   | If on, the maximum step length will be limited to     |
|                                  | 110% of the component length - this makes the         |
|                                  | tracking more robust and is the default with a        |
|                                  | regular BDSIM physics list. The minimum kinetic       |
|                                  | option is also obeyed. Default off.                   |
+----------------------------------+-------------------------------------------------------+
| g4PhysicsUseBDSIMRangeCuts       | If on, this will apply the BDSIM range cut lengths    |
|                                  | to the Geant4 physics list used. This is off by       |
|                                  | default.                                              |
+----------------------------------+-------------------------------------------------------+
| minimumKineticEnergy             | A particle below this energy will be killed and the   |
|                                  | energy deposition recorded at that location [GeV]     |
+----------------------------------+-------------------------------------------------------+
| minimumKineticEnergyTunnel       | A particle below this energy in any BDSIM-generated   |
|                                  | tunnel sections will be killed and the energy         |
|                                  | deposition recorded at that location [GeV]            |
+----------------------------------+-------------------------------------------------------+
| minimumRange                     | A particle that would not travel this range           |
|                                  | (a distance) in the current material will be cut [m]  |
+----------------------------------+-------------------------------------------------------+
| neutronTimeLimit                 | Maximum allowed tracking time for a neutron when      |
|                                  | using the `neutron_tracking_cut` physics list [s]     |
+----------------------------------+-------------------------------------------------------+
| neutronKineticEnergyLimit        | Minimum allowed energy for neutrons when using the    |
|                                  | `neutron_tracking_cut` physics list [GeV]             |
+----------------------------------+-------------------------------------------------------+
| physicsEnergyLimitLow            | Optional lower energy level for all physics models.   |
|                                  | This is usually 990 eV by default in Geant4. The user |
|                                  | may change this if required. Warning, this must       |
|                                  | be used only if the user understands how this will    |
|                                  | affect the running of Geant4. [GeV]                   |
+----------------------------------+-------------------------------------------------------+
| physicsEnergyLimitHigh           | Optional upper energy level for all physics models.   |
|                                  | This is usually 100 TeV by default in Geant4. The     |
|                                  | user may change this if required. Warning, this must  |
|                                  | be used only if the user understands how this will    |
|                                  | affect the running of Geant4. [GeV]                   |
+----------------------------------+-------------------------------------------------------+
| physicsList                      | Which physics lists to use - default tracking only    |
+----------------------------------+-------------------------------------------------------+
| physicsVerbose                   | Prints out all processes linked to primary particle   |
|                                  | and all physics processes registered in general       |
+----------------------------------+-------------------------------------------------------+
| prodCutPhotons                   | Standard overall production cuts for photons          |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutElectrons                 | Standard overall production cuts for electrons        |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutPositrons                 | Standard overall production cuts for positrons        |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| prodCutProtons                   | Standard overall production cuts for protons          |
|                                  | (default 1e-3) [m]                                    |
+----------------------------------+-------------------------------------------------------+
| stopSecondaries                  | Whether to stop secondaries or not (default = false)  |
+----------------------------------+-------------------------------------------------------+
| synchRadOn                       | Whether to use synchrotron radiation processes        |
+----------------------------------+-------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | Whether all particles entering the tunnel material    |
|                                  | should be killed or not (default = false)             |
+----------------------------------+-------------------------------------------------------+
| turnOnCerenkov                   | Whether to produce Cherenkov radiation                |
+----------------------------------+-------------------------------------------------------+
| useElectroNuclear                | Uses electro-nuclear processes when `em_extra` physics|
|                                  | list is used. Default On. Requires Geant4.10.4 or     |
|                                  | greater.                                              |
+----------------------------------+-------------------------------------------------------+
| useGammaToMuMu                   | Uses gamma to muon pair production process when using |
|                                  | `em_extra` physics list is used. Default Off.         |
|                                  | Requires Geant4.10.3 onwards.                         |
+----------------------------------+-------------------------------------------------------+
| useLENDGammaNuclear              | Uses the low-energy neutron data set, as provided by  |
|                                  | the environmental variable `G4LENDDATA` when using    |
|                                  | the `em_extra` physics list. Boolean. Available in    |
|                                  | Geant4.10.4 onwards.                                  |
+----------------------------------+-------------------------------------------------------+
| useMuonNuclear                   | Uses muon-nuclear interaction processes when using    |
|                                  | `em_extra` phyiscs list. Default On. Requires         |
|                                  | Geant4.10.2 onwards.                                  |
+----------------------------------+-------------------------------------------------------+
| usePositronToMuMu                | Uses muon pair production from positron annihilation  |
|                                  | when using `em_extra` physics list. Default Off.      |
|                                  | Requires Geant4.10.3 onwards.                         |
+----------------------------------+-------------------------------------------------------+
| usePositronToHadrons             | Uses hadron production from positron-electron         |
|                                  | annihilation process when using `em_extra` physics    |
|                                  | list. Default Off.  Requires Geant4.10.3 onwards.     |
+----------------------------------+-------------------------------------------------------+


Visualisation
^^^^^^^^^^^^^

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| nSegmentsPerCircle               | the number of facets per 2 :math:`\pi` in the         |
|                                  | visualiser. Note, this does not affect the accuracy   |
|                                  | of the geometry - only the visualisation (default =   |
|                                  | 50).                                                  |
+----------------------------------+-------------------------------------------------------+

.. _bdsim-options-output:

Output Options
^^^^^^^^^^^^^^

The particle physics simulation in BDSIM can produce an impressive quantity of output
information. The data describing a full record of every particle and their interaction
would prove too difficult to manage or analyse sensibly. BDSIM records the most useful
information, but provides options to record less or even more data. This is controlled
with the following options.

.. note:: These options may increase the output file size by a large amount. Use only the
	  ones you need.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+------------------------------------+--------------------------------------------------------------------+
| **Option**                         | **Function**                                                       |
+====================================+====================================================================+
| elossHistoBinWidth                 | The width of the histogram bins [m]                                |
+------------------------------------+--------------------------------------------------------------------+
| nperfile                           | Number of events to record per output file                         |
+------------------------------------+--------------------------------------------------------------------+
| sensitiveOuter                     | Whether the outer part of each component (other than the beam      |
|                                    | pipe) records energy loss. `storeELoss` is required to be on for   |
|                                    | this to work. The user may turn off energy loss from the           |
|                                    | beam pipe and retain losses from the magnet outer in combination   |
|                                    | with the next option `sensitiveBeamPipe`. Both are stored together |
|                                    | in `Eloss` branch of the Event Tree in the output. Default on.     |
+------------------------------------+--------------------------------------------------------------------+
| sensitiveBeamPipe                  | Whether the beam pipe records energy loss. This includes cavities. |
|                                    | This can be used in combination with the above option              |
|                                    | `sensitiveOuter`, to control which energy loss is recorded.        |
|                                    | Energy loss from this option is recorded in the `Eloss` branch     |
|                                    | of the Event Tree in the output. Default on.                       |
+------------------------------------+--------------------------------------------------------------------+
| sensitiveVacuum                    | Whether energy deposition in the residual vacuum gas is recorded.  |
|                                    | Energy loss from this option is recorded in the `Eloss` branch     |
|                                    | of the Event Tree in the output. Default on.                       |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpacts               | Create an optional branch called "ApertureImpacts" in the Event    |
|                                    | tree in the output that contains coordinates of where the primary  |
|                                    | particle exists the beam pipe. Note this could be multiple times.  |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsIons           | If `storeApertureImpacts` is on, the information will be generated |
|                                    | for all secondary ions as well as the primay. No information will  |
|                                    | be generated for other particles.                                  |
+------------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsAll            | If `storeApertureImpacts` is on, the information will be generated |
|                                    | for all particles leaving the beam pipe when this option is turned |
|                                    | on.                                                                |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHits                | Store hits in per-collimator structures with hits for only primary |
|                                    | particles. With only `storeCollimatorInfo` on, only the            |
|                                    | `primaryInteracted` and `primaryStopped` Booleans are stored.      |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHitsIons            | If `storeCollimatorInfo` is on and collimator hits are generated,  |
|                                    | `isIon`, `ionA` and `ionZ` variables are filled. Collimator hits   |
|                                    | will now also be generated for all ions whether primary or         |
|                                    | secondary. Default off.                                            |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHitsAll             | If `storeCollimatorInfo` is on and collimator hits are generated,  |
|                                    | hits will be generated for all particles interacting with the      |
|                                    | collimators whether primary or secondary and whether ion or not.   |
|                                    | Default off.                                                       |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorHitsLinks           | If `storeCollimatorHits` is on and collimator hits are generated,  |
|                                    | `charge`, `mass`, `rigidity` and `kineticEnergy` variables are     |
|                                    | also stored for each collimator hit.                               |
+------------------------------------+--------------------------------------------------------------------+
| storeCollimatorInfo                | With this option on, summary information in the Model Tree about   |
|                                    | only collimators is filled. Collimator structures are created in   |
|                                    | the Event Tree of the output for each collimator and prefixed with |
|                                    | "COLL\_" and contain hits from (only) primary particles.           |
|                                    | Collimator summary histograms are also created and stored. Default |
|                                    | off.                                                               |
+------------------------------------+--------------------------------------------------------------------+
| storeEloss                         | Whether to store the energy deposition hits. Default on. By        |
|                                    | turning off, `sensitiveBeamPipe`, `sensitiveOuter` and             |
|                                    | `sensitiveVacuum` have no effect. Saves run time memory and output |
|                                    | file size. See next option `storeEloss` for combination.           |
+------------------------------------+--------------------------------------------------------------------+
| storeElossHistograms               | Whether to store energy deposition histograms `Eloss` and          |
|                                    | `ElossPE`. This will automatically be on if `storeEloss` is on.    |
|                                    | With `storeEloss` off, this option can be turned on to retain the  |
|                                    | energy deposition histograms. If both this and `storeEloss` are    |
|                                    | off, no energy deposition hits will be generated saving memory.    |
+------------------------------------+--------------------------------------------------------------------+
| storeElossVacuum                   | Whether to store energy deposition from the vacuum volumes as hits |
|                                    | in the `ElossVacuum` branch and the corresponding summary          |
|                                    | histograms. Default off.                                           |
+------------------------------------+--------------------------------------------------------------------+
| storeElossVacuumHistograms         | Whether to generate summary histograms of energy deposition in the |
|                                    | vacuum volumes. If `storeElossVacuum` is on, this will be on. The  |
|                                    | user may turn off `storeElossVacuum` but turn this on to store     |
|                                    | the energy deposition histograms.                                  |
+------------------------------------+--------------------------------------------------------------------+
| storeElossTunnel                   | Whether to store energy deposition hits from the tunnel geometry   |
|                                    | in the `ElossTunnel` branch of the Event Tree. Default off.        |
+------------------------------------+--------------------------------------------------------------------+
| storeElossTunnelHistograms         | Whether to generate summary histograms of energy deposition in the |
|                                    | tunnel volumes. If `storeElossTunnel` is on, this will be on. The  |
|                                    | user may turn off `storeElossTunnel` but turn this on to store     |
|                                    | the energy deposition histograms.                                  |
+------------------------------------+--------------------------------------------------------------------+
| storeElossWorld                    | Whether to record energy deposition in the world volume and, in    |
|                                    | the case of using Geant4.10.3 or newer, the energy leaving the     |
|                                    | world volume as well. Default off.                                 |
+------------------------------------+--------------------------------------------------------------------+
| storeElossWorldContents            | Whether to record energy deposition in the daughter volumes within |
|                                    | the world volume when supplied as external world geometry.         |
|                                    | Default off.                                                       |
+------------------------------------+--------------------------------------------------------------------+
| storeElossGlobal                   | Global coordinates will be stored for each energy deposition hit   |
|                                    | and for each trajectory point. Default off.                        |
+------------------------------------+--------------------------------------------------------------------+
| storeElossLinks                    | For each energy deposition hit, the particle ID, track ID, parent  |
|                                    | ID and beam line index will be stored - this is intended to help   |
|                                    | 'link' the energy deposition back to other information. Default    |
|                                    | off.                                                               |
+------------------------------------+--------------------------------------------------------------------+
| storeElossLocal                    | Local coordinates will be stored for each energy deposition hit    |
|                                    | and for each trajectory point. Default off.                        |
+------------------------------------+--------------------------------------------------------------------+
| storeElossModelID                  | Store the beam line index of the object the energy deposition hit  |
|                                    | was in. If `storeElossLinks` is on, this will be on irrespective   |
|                                    | of this option.                                                    |
+------------------------------------+--------------------------------------------------------------------+
| storeElossTime                     | The time since the start of the event will be stored for each point|
|                                    | of energy deposition and trajectory. Default off.                  |
+------------------------------------+--------------------------------------------------------------------+
| storeElossTurn                     | Store the turn number of each energy deposition hit. Default off,  |
|                                    | but automatically on when using a circular machine with the        |
|                                    | (also executable) option :code:`circular`.                         |
+------------------------------------+--------------------------------------------------------------------+
| storeElossStepLength               | Stores the step length for each energy deposition hit or not.      |
|                                    | Default off.                                                       |
+------------------------------------+--------------------------------------------------------------------+
| storeElossPreStepKineticEnergy     | Stores the kinetic energy of the particle causing energy deposition|
|                                    | as taken from the beginning of the step before it made it. Default |
|                                    | off.                                                               |
+------------------------------------+--------------------------------------------------------------------+
| storeGeant4Data                    | Whether to store basic particle information for all particles used |
|                                    | in the simulation under Geant4Data in the output. This can be      |
|                                    | relatively large when ions are used as there are many thousands    |
|                                    | of ion definitions. Default on.                                    |
+------------------------------------+--------------------------------------------------------------------+
| storeModel                         | Whether to store the model information in the output. Default on.  |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerAll                    | Convenience option to turn on all optional sampler output.         |
|                                    | Equivalent to turning on `storeSamplerCharge`,                     |
|                                    | `storeSamplerKineticEnergy`, `storeSamplerMass`,                   |
|                                    | `storeSamplerRigidity`, `storeSamplerIon`. Overrides these         |
|                                    | options even if they are explicitly set to off (0).                |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerCharge                 | Stores corresponding charge of particle for every entry in sampler |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerKineticEnergy          | Stores corresponding kinetic energy of particle for every entry in |
|                                    | sampler.                                                           |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerMass                   | Stores corresponding mass (in GeV) of particle for every entry in  |
|                                    | the sampler.                                                       |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerPolarCoords            | Calculate and store the polar coordinates (r, phi) and (rp, phip)  |
|                                    | for the sampler data.                                              |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerRigidity               | Stores the rigidity (in Tm) of particle for every entry in sampler |
+------------------------------------+--------------------------------------------------------------------+
| storeSamplerIon                    | Stores A, Z and Boolean whether the entry is an ion or not as well |
|                                    | as the `nElectrons` variable for possible number of electrons.     |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectory                    | Whether to store trajectories. If turned on, all trajectories are  |
|                                    | stored. This must be turned on to store any trajectories at all.   |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectories                  | An alias to `storeTrajectory`                                      |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryLocal               | For the trajectories that are stored (according to the filters),   |
|                                    | store `xyz` and `pxpypz` local coordinate variables.               |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryLinks               | For the trajectories that are stored (according to the filters),   |
|                                    | store `charge`, `kineticEnergy`, `turnsTaken`, `mass` and          |
|                                    | `rigidity` variables for each step.                                |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryIons                | For the trajectories that are stored (according to the filters),   |
|                                    | store `isIon`, `ionA`, `ionZ` and `nElectrons` variables.          |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryDepth               | The depth of the particle tree to store the trajectories to  0 is  |
|                                    | the primary, 1 is the first generation of secondaries, etc.        |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryELossSRange         | Ranges in curvilinear S coordinate that if a particular track      |
|                                    | causes energy deposition in this range, its trajectory will be     |
|                                    | stored. The value should be a string inside which are pairs of     |
|                                    | numbers separated by a colon and ranges separated by whitespace    |
|                                    | such as "0.3:1.23 45.6:47.6".                                      |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryParticle            | The Geant4 name of particle(s) to only store trajectories for.     |
|                                    | This is case sensitive. Multiple particle names can be used with   |
|                                    | a space between them. e.g. "proton pi-".                           |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryParticleID          | The PDG ID of the particle(s) to only store trajectories for.      |
|                                    | Multiple particle IDs can be supplied with a space between them.   |
|                                    | e.g. "11 12 22 13".                                                |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryEnergyThreshold     | The threshold energy for storing trajectories. Trajectories for    |
|                                    | any particles with energy less than this amount (in GeV) will not  |
|                                    | be stored.                                                         |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectorySamplerID           | If a trajectory reaches the name of these samplers, store that     |
|                                    | trajectory. This value supplied should be a whitespace separated   |
|                                    | string such as "cd1 qf32x".                                        |
+------------------------------------+--------------------------------------------------------------------+
| storeTrajectoryTransportationSteps | On by default. If true, include steps in the trajectories that     |
|                                    | are created by transportation only. When a particle crosses a      |
|                                    | boundary, this forces a step in geant4 and therefore another       |
|                                    | trajectory point. Legacy option is :code:`trajNoTransportation`    |
|                                    | that is opposite to this option.                                   |
+------------------------------------+--------------------------------------------------------------------+
| trajConnect                        | Stores all the trajectories that connect a trajectory to be        |
|                                    | stored all the way to the primary particle. For example, if the    |
|                                    | filters from other trajectory options are to store only muons      |
|                                    | with an energy greater than 10 GeV, the few trajectories stored    |
|                                    | would appear unrelated. This option forces the storage of only     |
|                                    | the trajectories of any particles (irrespective of filters) that   |
|                                    | lead to the muon in question.                                      |
+------------------------------------+--------------------------------------------------------------------+
| trajCutGTZ                         | Only stores trajectories whose *global* z-coordinate is greater    |
|                                    | than this value in metres [m].                                     |
+------------------------------------+--------------------------------------------------------------------+
| trajCutLTR                         | Only stores trajectories whose *global* radius is from the start   |
|                                    | position (sqrt(x^2, y^2)).                                         |
+------------------------------------+--------------------------------------------------------------------+

.. _bdsim-options-verbosity:

Print Out Options
^^^^^^^^^^^^^^^^^

The following options control the level of print out both from BDSIM and from Geant4. Note, excessive
amounts of output will cause a simulation to run slowly and should only be used for understanding a
particular physics outcome if really desired or not understood. It is recommended to print out as little
as possible and then work 'up' to more print out as required.

BDSIM generally prints out the most minimal information for its purpose. The physics tables printed out can be
lengthy, but are an important set of information for a given simulation.

Some of the following options are available through executable options (with different names). See
:ref:`executable-options` for more details.

Recommendations:

* `-\\-verboseSteppingLevel=2` to see one line per entry / exit of a volume to see where a particle is going.
* "Tracking" refers to a particle track which is essentially one particle being put through the simulation.
* Stepping is the incremental step of each particle trajectory through the simulation.
* Event is the minimal unit of simulation - usually in BDSIM this is the propagation of 1 primary particle.
* Run is a group of events where the physics and geometry remained the same.

The options listed below are list roughly in terms of the simulation hiearchy.

+----------------------------------+----------+-------------------------------------------------------------------+
| **Option**                       | **Type** | **Description**                                                   |
+==================================+==========+===================================================================+
| verbose                          | Boolean  | Whether general verbosity is on - some extra print out.           |
|                                  |          | This highlights general construction steps of the                 |
|                                  |          | geometry; print out any field definitions defined in the          |
|                                  |          | parser; a summary of all modular physics lists activated          |
|                                  |          | or not.                                                           |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseRunLevel                  | integer  | (0-5) level of Geant4 run level print out. The same as            |
|                                  |          | `-\\-verboseRun=X` executable option.                             |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseEventBDSIM                | Boolean  | Extra print out identifying the start and end of event            |
|                                  |          | action as well as the allocator pool sizes. Print out             |
|                                  |          | the size of each hits collection if it exists at all. The         |
|                                  |          | same as `-\\-verboseEventBDSIM` executable option.                |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseEventStart                | integer  | Event index to start print out according to                       |
|                                  |          | `verboseEventBDSIM`. Zero counting.                               |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseEventContinueFor          | integer  | Number of events to continue print out event information          |
|                                  |          | according to `verboseEventBDSIM`. -1 means all subsequent         |
|                                  |          | events.                                                           |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseEventLevel                | integer  | (0-5) level of Geant4 event level print out for all               |
|                                  |          | events.                                                           |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingBDSIM             | Boolean  | Extra print out for all steps of all particles from BDSIM         |
|                                  |          | for events in the range according to `verboseSteppingEventStart`  |
|                                  |          | and `verboseSteppingEventContinueFor`. Default is all events.     |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingLevel             | integer  | (0-5) level of Geant4 print out per step of each particle. This   |
|                                  |          | done according to the range of `verboseSteppingEventStart, and    |
|                                  |          | `verboseSteppingEventContinueFor`. Default is all events and all  |
|                                  |          | particles.                                                        |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingEventStart        | integer  | Event offset (zero counting) to start stepping print out          |
|                                  |          | according to `verboseSteppingLevel`.                              |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingEventContinueFor  | integer  | Number of events to continue print out stepping information for   |
|                                  |          | according to `verboseSteppingLevel`.                              |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingPrimaryOnly       | Boolean  | If true, only print out stepping information for the primary.     |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseImportanceSampling        | integer  | (0-5) level of importance sampling related print out.             |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseStep                      | Boolean  | Whether to use the verbose stepping action for every              |
|                                  |          | step. Note, this is a lot of output.                              |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseSteppingLevel             | integer  | (0-5) level of Geant4 stepping level print out. The same          |
|                                  |          | as `-\\-verbose_G4stepping=X` executable option.                  |
+----------------------------------+----------+-------------------------------------------------------------------+
| verboseTrackingLevel             | integer  | (0-5) level of Geant4 tracking level print out. The same          |
|                                  |          | as `-\\-verbose_G4tracking=X` executable option.                  |
+----------------------------------+----------+-------------------------------------------------------------------+

Examples: ::

  option, verboseEventStart=3,
          verboseEventLevel=2;

This will print out verbose stepping information for the primary particle (default is only the primary)
for the 4th event onwwards (3 in 0 counting) with a verbose stepping level of 2 showing individual volumes. This
example is in :code:`bdsim/examples/features/options/verboseEvent-primaries.gmad`. This will print out for
every event after this.  Another example is: ::

  option, verboseSteppingEventStart=3,
          verboseSteppingLevel=2,
	  verboseSteppingEventContinueFor=1,
	  verboseSteppingPrimaryOnly=0;

This will print out verbose stepping information for all particles starting from the 4th event for 1 event.

::

   bdsim --file=sm.gmad --batch --ngenerate=10 --verboseSteppingLevel=2 --verboseSteppingEventStart=3 \\
         --verboseSteppingEventContinueFor=1 --verboseSteppingPrimaryOnly

This will print out the volume name for each step of the primary particle (only) for event #3 (the 4th event).
	 
.. _beamline-offset:

Offset for Main Beam Line
^^^^^^^^^^^^^^^^^^^^^^^^^

The following options may be used to offset the main beam line with respect to the world
volume, which is the outermost coordinate system.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------+--------------------------------------------------------------------+
| **Option**           | **Funciton**                                                       |
+======================+====================================================================+
| beamlineX            | Offset in x                                                        |
+----------------------+--------------------------------------------------------------------+
| beamlineY            | Offset in y                                                        |
+----------------------+--------------------------------------------------------------------+
| beamlineZ            | Offset in z                                                        |
+----------------------+--------------------------------------------------------------------+
| beamlinePhi          | Euler angle phi for rotation                                       |
+----------------------+--------------------------------------------------------------------+
| beamlineTheta        | Euler angle theta for rotation                                     |
+----------------------+--------------------------------------------------------------------+
| beamlinePsi          | Euler angle psi for rotation                                       |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisX        | Axis angle rotation x-component of unit vector                     |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisY        | Axis angle rotation y-component of unit vector                     |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisZ        | Axis angle rotation z-component of unit vector                     |
+----------------------+--------------------------------------------------------------------+
| beamlineAngle        | Axis angle to rotate about unit vector.                            |
+----------------------+--------------------------------------------------------------------+
| beamlineAxisAngle    | Boolean whether to use axis angle rotation scheme (default false)  |
+----------------------+--------------------------------------------------------------------+
| beamlineS            | S offset of start of beamline (default 0)                          |
+----------------------+--------------------------------------------------------------------+

Two styles of rotation can be used: either a set of three Euler angles, or the axis angle
rotation scheme where a **unit** vector is provided in :math:`x,y,z` and an angle to
rotate about it. These variables are used to construct a :code:`G4RotationMatrix`
directly, which is also the same as a :code:`CLHEP::HepRotation`.

.. Note:: Geant4 uses a right-handed coordinate system and :math:`m` and :math:`rad` are
	  the default units for offsets and angles in BDSIM.

Example::

  option, beamlineX = 3*m,
          beamlineY = 20*cm,
	  beamlineZ = -30*m,
	  beamlineAxisAngle = 1,
	  beamlineAxisY = 1,
	  beamlineAngle = 0.2;

This offsets the beam line by (3,0.2,-30) m and rotates about the unit vector (0,1,0) (i.e. in the
horizontal plane - x,z) by 0.2 rad.

.. _scoring-map-description:

Scoring Map
^^^^^^^^^^^

BDSIM provides the capability to create one 3D histogram of energy deposition hits irrespective
of the geometry. The hits are only created where the geometry exists and are sensitive.
The histogram is independent of the geometry.

* The user should ideally set all parameters to specify the desire ranges, otherwise be
  aware of the default values.
* BDSIM will exit with a warning if zero range is found in any dimension as this means
  nothing will be histogrammed and there is no point in continuing.

An example can be found in :code:`bdsim/examples/features/io/1_rootevent/sc_scoringmap.gmad`.

.. note:: This is called a scoring map for historical reasons but it does not limit the step
	  length in the way a typical Geant4 scoring map would. This only histograms energy
	  deposition data.


+----------------------------------+-------------------------------------------------------+
| **Option**     | **Default**     | **Function**                                          |
+==================================+=======================================================+
| useScoringMap  | 0               | Whether to create a scoring map                       |
+----------------+-----------------+-------------------------------------------------------+
| nbinsx         | 1               | Number of bins in global X                            |
+----------------+-----------------+-------------------------------------------------------+
| nbinsy         | 1               | Number of bins in global Y                            |
+----------------+-----------------+-------------------------------------------------------+
| nbinsz         | 1               | Number of bins in global Z                            |
+----------------+-----------------+-------------------------------------------------------+
| xmin           | -0.5            | Lower global X limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+
| xmax           | 0.5             | Upper global X limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+
| ymin           | -0.5            | Lower global Y limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+
| ymax           | 0.5             | Upper global Y limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+
| zmin           | 0               | Lower global Z limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+
| zmax           | 1               | Upper global Z limit (m)                              |
+----------------+-----------------+-------------------------------------------------------+


.. _developer-options:

Developer Options
^^^^^^^^^^^^^^^^^

These are documented here, but use with caution, as they lead to undesirable behaviour and
should only be used with understanding.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+-----------------------------------+--------------------------------------------------------------------+
| **Option**                        | **Description**                                                    |
+===================================+====================================================================+
| chordStepMinimum                  | Minimum step size                                                  |
+-----------------------------------+--------------------------------------------------------------------+
| deltaChord                        | Chord finder precision                                             |
+-----------------------------------+--------------------------------------------------------------------+
| deltaIntersection                 | Boundary intersection precision                                    |
+-----------------------------------+--------------------------------------------------------------------+
| deltaOneStep                      | Set position error acceptable in an integration step               |
+-----------------------------------+--------------------------------------------------------------------+
| lengthSafety                      | Element overlap safety (caution!)                                  |
+-----------------------------------+--------------------------------------------------------------------+
| maximumEpsilonStep                | Maximum relative error acceptable in stepping                      |
+-----------------------------------+--------------------------------------------------------------------+
| minimumEpsilonStep                | Minimum relative error acceptable in stepping                      |
+-----------------------------------+--------------------------------------------------------------------+
| sampleElementsWithPoleface        | Default false. Samplers are not to be attached to elements with    |
|                                   | poleface rotations, as the sampler will overlap with the mass world|
|                                   | geometry, resulting in incorrect tracking. This only occurs in     |
|                                   | integrator sets which construct the poleface geometry, namely      |
|                                   | :code:`bdsimtwo`, :code:`geant4`, and :code:`geant4dp`. This option|
|                                   | overides this, allowing samplers to be attached. This option will  |
|                                   | not affect the default integrator set, :code:`bdsimmatrix`.        |
+-----------------------------------+--------------------------------------------------------------------+
| teleporterFullTransform           | Default true. Whether to use the newer teleporter offset method    |
|                                   | that uses a G4Transform3D to apply both an offset and a rotation.  |
|                                   | The newer method works in any 3D orientation whereas the old one   |
|                                   | only works with the beam line starting along unit Z (i.e. no       |
|                                   | beam line offset or rotation.                                      |
+-----------------------------------+--------------------------------------------------------------------+
| beam, offsetSampleMean=1          | Default false. If true, this will remove the sample mean from the  |
|                                   | bunch distribution to match the central values. This is useful for |
|                                   | optical function calculation. BDSIM is not currently able to       |
|                                   | reproduce results when this option is used and coordinates will    |
|                                   | be different for each run, or even when using -\\-recreate.  Only  |
|                                   | suitable for large (>100) numbers of particles. Note: this isn't   |
|                                   | an option, but part of the beam command. This cannot be used with  |
|                                   | the visualiser.                                                    |
+-----------------------------------+--------------------------------------------------------------------+


.. _beam-parameters:

Beam Parameters
---------------

BDSIM starts each event in one of two ways.

1) Particles coordinates for one particle
   are generated from a chosen beam distribution, which is specified in the input GMAD file.
   In most cases, the particle coordinates are randomly generated according
   to the distribution.

2) A primary vertex is loaded from an event genertor file. This currently requires linking to
   HepMC3 to load such files. In this case, each event may start with 1 or more particles. (see
   `eventgeneratorfile`_).

To specify the input particle distribution, the :code:`beam` command is
used. This also specifies the particle species and **reference total energy**, which is the
design total energy of the machine. This is used along with the particle species to calculate
the momentum of the reference particle and therefore the magnetic rigidity that normalised magnetic
field strengths are calculated with respect to. For example, the field of dipole magnets
is calculated using this if only the `angle` parameter has been specified.

Apart from the design particle and energy, a beam of particles of a different species and total
energy may be specified. By default, if only one particle is specified this is assumed to be
both the design particle and the particle used for the beam distribution.

.. note:: The design energy is required to be specified, but the central energy, of
	  a bunch, for example with a Gaussian distribution, can be specified with `E0`.

.. note:: `energy` here is the **total energy** of the particle. This must be greater than
	  the rest mass of the particle.

The user **must** specify at least `energy` and the `particle` type. In this case the
`reference`_ distribution will be used as well as default parameters. The minimum
beam definitions are::

  beam, particle="proton",
        energy=34.2*GeV;


Other parameters, such
as the beam distribution type, `distrType`, are optional and can be specified as described
in the following sections. The beam is defined using the following syntax::

  beam, particle="proton",
        energy=4.0*TeV,
	distrType="reference";

Energy is the total energy in `GeV`. The beam particle may be specified by name as it is
in Geant4 (exactly) or by it's PDG ID. The follow are available by default:

* `e-` or `e+`
* `proton` or `antiproton`
* `gamma`
* `neutron`
* `mu-` or `mu+`
* `pi-` or `pi+`
* `photon` or `gamma`
* `kaon-`, `kaon+` or `kaon0L`

In fact, the user may specify any particle that is available through the physics lists
used. If given by name, the particle must be given by the Geant4 name exactly. The ones
above are always defined and so can always safely be used irrespective of the physics
lists used. If the particle definition is not found, BDSIM will print a warning and exit.

If more exotic particles are desired but no corresponding physics processes are desired, then
the special physics list "all_particles" can be used to only load the particle definitions.

The PDG IDs can be found at the PDG website; reviews and tables; Monte Carlo Numbering Scheme.

* `<http://pdg.lbl.gov/2019/reviews/rpp2018-rev-monte-carlo-numbering.pdf>`_

Ion Beams
^^^^^^^^^

The user may also specify any ion with the following syntax::

  beam, particle="ion A Z";

or::

  beam, particle="ion A Z Q";

where `A`, `Z` and `Q` should be replaced by the atomic mass number (an integer),
the number of protons in the nucleus, and the charge respectively. The charge is
optional and by default is Z (i.e. a fully ionised ion).  For example: ::

  beam, particle="ion 12 6",
        energy = 52 * GeV;

* The user should take care to use a physics list that includes ion physics processes.

Available input distributions and their associated parameters are described in the following
section.

Different Beam and Design Particles
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The model may use one particle for design and one for the beam distribution. The "design" particle
is used to calculate the rigidity that is used along with normalised field strengths (such as
:code:`k1` for quadrupoles) to calculate an absolute field or field gradient. However, it is
often useful to simulate a beam of other particles. To specify a different central energy, the
parameter :code:`E0` should be used. If a different particle is required the parameter
:code:`beamParticleName` should be used.

Examples: ::

   beam, particle="e-",
         energy=100*GeV,
	 beamParticleName="e+";

This specifies that the magnet field strengths are calculated with respect to a 100 GeV electron
and the beam tracked is a 100 GeV positron beam (along with any other relevant distribution
parameters). ::

   beam, particle="e-",
         energy=100*GeV,
	 beamParticleName="e+",
	 E0=20*GeV;

This specified that the magnet field strengths are calculated with respect to a 100 GeV electron
and the beam tracked is a 20 GeV positron beam.

* If no :code:`beamParticleName` variable is specified, it's assumed to be the same as :code:`particle`.
* If no :code:`E0` variable is specified, it's assumed to be the same as :code:`energy`.
	

Generate Only the Input Distribution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

BDSIM can generate only the input distribution and store it to file without creating a model or
running any physics simulation. This is very fast and can be used to verify the input distribution
with a large number of particles (for example, 10k to 100k in under one minute).

BDSIM should be executed with the option :code:`--generatePrimariesOnly` as described in
:ref:`executable-options`.

* The exact coordinates generated will not be the same as those generated in a run, even
  with the same seed. This is because the physics models will also advanced the random
  number generator, where as with :code:`--generatePrimariesOnly`, only the bunch distribution
  generator will. For a large number of primaries (at least 100), the option
  :code:`offsetSampleMean` can be used with Gaussian distributions to pre-generate the coordinates
  before the run. In this case, they would be consistent.
* This will not work when using an event generator file. Using an event generator
  file requires the particle table in Geant4 be loaded and this can only be done
  in a full run where we construct the model. By default, the generate primaries
  only option only generates coordinates and does not build a Geant4 model.

.. warning:: In a conventional run of BDSIM, after a set of coordinates are generated, a check
	     is made to ensure the total energy chosen is greater than the rest mass of the
	     particle. This check is **not** done in the case of :code:`--generatePrimariesOnly`.
	     Therefore, it's possible to generate values of total energy below the rest mass of
	     the beam particle.


Beam in Output
^^^^^^^^^^^^^^

All of the beam parameters are stored in the output, as described in :ref:`output-beam-tree`. The
particle coordinates used in the simulation are stored directly in the Primary branch of the
Event Tree, as described in :ref:`output-event-tree`.

.. note:: These are the exact coordinates supplied to Geant4 at the beginning of the event.
	  Conceptually, these are 'local' coordinates with respect to the start of the beam
	  line. However, if a finite `S0` is specified, the bunch distribution is transformed
	  to that location in the World, therefore the coordinates are the **global** ones used.

.. warning:: For large `S0` in a large model, the particles may be displaced by a large
	     distance as compared to the size of the beam, e.g. 1km offset for 1um beam.
	     In this case, the limited precision of the `float` used to store the coordinates
	     in the output may not show the beam distribution as expected. Internally, double
	     precision numbers are used so that the beam distribution is accurate. A float typically
	     has seven significant figures and a double 15.

Beam Tilt
^^^^^^^^^

The possibility exists to rotate the beam after the local curvilinear coordinates are calculated
from one of the following bunch distributions. This is an angle about the local unit Z axis, i.e.
the direction of the beam by default. This is applied **after** the local coordinates are generated
by the bunch distribution and rotates, the x,y and xp,yp coordinates by an angle in radians. The
rotation is in a right-handed coordinate system.

Looking along the direction of the beam, a particle at positive X0 and zero Y0 with a tilt of
positive pi/2 will become zero X0 and finite Y0. Looking along the beam direction, the rotation
is clockwise. This is irrespective of particle charge.

The parameter that controls this is `tilt` in the beam command and is in radians. For example: ::

  beam, particle="e-",
        energy=10*GeV,
	distrType="gauss",
	sigmaX=100*um,
	sigmaY=1*um,
	sigmaXp=1e-8,
	sigmaYp=1e-10,
	tilt=0.01;

Here a beam 100 x 1 um is generated as a Gaussian and then rotated by 0.01 radians.

.. _beam-distributions:

Beam Distributions
^^^^^^^^^^^^^^^^^^
The following beam distributions are available in BDSIM

- `reference`_
- `gaussmatrix`_
- `gauss`_
- `gausstwiss`_
- `circle`_
- `square`_
- `ring`_
- `eshell`_
- `halo`_
- `composite`_
- `userfile`_
- `ptc`_
- `eventgeneratorfile`_
- `sphere`_

.. note:: For `gauss`_, `gaussmatrix`_ and `gausstwiss`_, the beam option `beam, offsetSampleMean=1`
	  documented in :ref:`developer-options` can be used to pre-generate all particle coordinates and
	  subtract the sample mean from these, effectively removing any small systematic offset in
	  the bunch at the beginning of the line. This is used only for optical comparisons currently.


reference
^^^^^^^^^
This is a single particle with the same position and angle defined by the following parameters. The
coordinates are the same for every particle fired using the reference distribution. It is therefore
not likely to be useful to generate a large number of repeated events with this distribution unless
the user wishes to explore the different outcome from the physics processes, which will be different
each time should the particle interact. This distribution may be referred to as a 'pencil' distribution
by other codes.

These parameters also act as **central** parameters for all other distributions. For example, a Gaussian
distribution may be defined with the `gauss`_ parameters, but with `X0` set to offset the centroid of the
Gaussian with respect to the reference trajectory. Note: **energy** is **total energy** of the
particle - including the rest mass.

  .. tabularcolumns:: |p{5cm}|p{6cm}|p{2cm}|

+----------------------------------+-------------------------------------------------------+----------+
| Option                           | Description                                           | Default  |
+==================================+=======================================================+==========+
| `X0`                             | Horizontal position [m]                               | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Y0`                             | Vertical position [m]                                 | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Z0`                             | Longitudinal position [m]                             | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `S0`                             | Curvilinear S offset [m]                              | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `T0`                             | Longitudinal position [s]                             | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Xp0`                            | Horizontal canonical momentum                         | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Yp0`                            | Vertical canonical momentum                           | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `E0`                             | Central total energy of bunch distribution (GeV)      | 'energy' |
+----------------------------------+-------------------------------------------------------+----------+

* `S0` allows the beam to be translated to a certain point in the beam line, where the beam
  coordinates are with respect to the curvilinear frame at that point in the beam line.
* `S0` and `Z0` cannot both be set - BDSIM will exit with a warning if this conflicting input is given.
* If `S0` is used, the local coordinates are generated and then transformed to that point in the beam line.
  Each set of coordinates will be stored in the output under `Primary` (local) and `PrimaryGlobal` (global).

Examples: ::

  beam, particle = "e-",
        energy = 10*GeV,
	distrType = "reference";

Generates a beam with all coordinates=0 at the nominal energy. ::

  beam, particle = "e-",
        energy = 10*GeV,
	distrType = "reference",
	X0 = 100*um,
	Y0 = 3.5*um;

Generates a particle with an offset of 100 :math:`\mu\mathrm{m}` horizontally and 3.5
:math:`\mu\mathrm{m}` vertically.

gaussmatrix
^^^^^^^^^^^

Uses the :math:`N` dimensional Gaussian generator from `CLHEP`, `CLHEP::RandMultiGauss`. The generator
is initialised by a :math:`6\times1` means vector and :math:`6\times 6` sigma matrix.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+------------------+-----------------------------------+
| Option           | Description                       |
+==================+===================================+
| `sigmaNM`        | Sigma matrix element (N,M)        |
+------------------+-----------------------------------+

* Only the upper-right half of the matrix and diagonal should be populated, as the
  elements are symmetric across the diagonal.
* The coordinates are in order 1:`x` (m), 2:`xp`, 3:`y` (m), 4:`yp`, 5:`t` (s), 6:`E` (GeV).

The user should take care to ensure they specify a positive definite matrix. BDSIM will
emit an error and stop running if this is not the case.

Examples: ::

   beam, particle = "e-",
         energy = 10*GeV,
	 distrType = "gaussmatrix",
	 sigma11 = 100*um,
	 sigma22 = 3*um,
	 sigma33 = 50*um,
	 sigma44 = 1.4*um,
	 sigma55 = 1e-12
	 sigma66 = 1e-4,
	 sigma12 = 1e-2,
	 sigma34 = 1.4e-3;

.. note:: One should take care in defining, say, sigma16, as this is the covariance of the `x` position
	  and energy. However, this may be proportional to momentum and not total energy. Note, for such
	  a *correlation* between `x` and `E`, other off-diagonal terms in the covariance matrix should
	  be finite also.

gauss
^^^^^

Uses the `gaussmatrix`_ beam generator but with simplified input parameters, as opposed to a complete
beam sigma matrix. This beam distribution has a diagonal :math:`\sigma`-matrix and does not allow for
correlations between phase space coordinates, so

.. math::
   \sigma_{11} & =  \sigma_x^2   \\
   \sigma_{22} & =  \sigma_x^{\prime 2}  \\
   \sigma_{33} & =  \sigma_y^2   \\
   \sigma_{44} & =  \sigma_y^{\prime 2}  \\
   \sigma_{55} & =  \sigma_{T}^2 \\
   \sigma_{66} & =  \sigma_{E}^2.

* The coordinates are in order 1:`x` (m), 2:`xp`, 3:`y` (m), 4:`yp`, 5:`t` (s), 6:`E` (GeV).
* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+------------------+----------------------------------------------------+
| Option           | Description                                        |
+==================+====================================================+
| `sigmaX`         | Horizontal Gaussian sigma [m]                      |
+------------------+----------------------------------------------------+
| `sigmaY`         | Vertical Gaussian sigma [m]                        |
+------------------+----------------------------------------------------+
| `sigmaXp`        | Sigma of the horizontal canonical momentum         |
+------------------+----------------------------------------------------+
| `sigmaYp`        | Sigma of the vertical canonical momentum           |
+------------------+----------------------------------------------------+
| `sigmaE`         | Relative energy spread :math:`\sigma_{E}/E`        |
+------------------+----------------------------------------------------+
| `sigmaT`         | Sigma of the temporal distribution [s]             |
+------------------+----------------------------------------------------+


gausstwiss
^^^^^^^^^^

The beam parameters are defined by the usual Twiss parameters :math:`\alpha`, :math:`\beta` and
:math:`\gamma`, plus dispersion :math:`\eta`, from which the beam :math:`\sigma` -matrix
is calculated, using the following equations:

.. math::
   \sigma_{11} & =  \epsilon_x \beta_x + \eta_{x}^{2}\sigma_{E}^{2} \\
   \sigma_{12} & = -\epsilon_x \alpha_x + \eta_{x}\eta_{xp}\sigma_{E}^{2}\\
   \sigma_{21} & = -\epsilon_x \alpha_x + \eta_{x}\eta_{xp}\sigma_{E}^{2}\\
   \sigma_{22} & =  \epsilon_x \gamma_x + \eta_{xp}^{2}\sigma_{E}^{2}\\
   \sigma_{33} & =  \epsilon_y \beta_y + \eta_{y}^{2}\sigma_{E}^{2}\\
   \sigma_{34} & = -\epsilon_y \alpha_y + \eta_{y}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{43} & = -\epsilon_y \alpha_y + \eta_{y}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{44} & =  \epsilon_y \gamma_y + \eta_{yp}^{2}\sigma_{E}^{2}\\
   \sigma_{13} & = \eta_{x}\eta_{y}\sigma_{E}^{2}\\
   \sigma_{31} & = \eta_{x}\eta_{y}\sigma_{E}^{2}\\
   \sigma_{23} & = \eta_{xp}\eta_{y}\sigma_{E}^{2}\\
   \sigma_{32} & = \eta_{xp}\eta_{y}\sigma_{E}^{2}\\
   \sigma_{14} & = \eta_{x}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{41} & = \eta_{x}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{24} & = \eta_{xp}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{42} & = \eta_{xp}\eta_{yp}\sigma_{E}^{2}\\
   \sigma_{16} & = \eta_{x}\sigma_{E}^{2}\\
   \sigma_{61} & = \eta_{x}\sigma_{E}^{2}\\
   \sigma_{26} & = \eta_{xp}\sigma_{E}^{2}\\
   \sigma_{62} & = \eta_{xp}\sigma_{E}^{2}\\
   \sigma_{36} & = \eta_{y}\sigma_{E}^{2}\\
   \sigma_{63} & = \eta_{y}\sigma_{E}^{2}\\
   \sigma_{46} & = \eta_{yp}\sigma_{E}^{2}\\
   \sigma_{64} & = \eta_{x}\sigma_{E}^{2}\\
   \sigma_{55} & =  \sigma_{T}^2 \\
   \sigma_{66} & =  \sigma_{E}^2

* All parameters from `reference`_ distribution are used as centroids.
* Longitudinal parameters :math:`\sigma_{E}` and :math:`\sigma_{T}` used as defined in `gauss`_ .


.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `emitx`                          | Horizontal beam core emittance [m]                    |
+----------------------------------+-------------------------------------------------------+
| `emity`                          | Vertical beam core emittance [m]                      |
+----------------------------------+-------------------------------------------------------+
| `betx`                           | Horizontal beta function [m]                          |
+----------------------------------+-------------------------------------------------------+
| `bety`                           | Vertical beta function [m]                            |
+----------------------------------+-------------------------------------------------------+
| `alfx`                           | Horizontal alpha function                             |
+----------------------------------+-------------------------------------------------------+
| `alfy`                           | Vertical alpha function                               |
+----------------------------------+-------------------------------------------------------+
| `dispx`                          | Horizontal dispersion function [m]                    |
+----------------------------------+-------------------------------------------------------+
| `dispy`                          | Vertical dispersion function [m]                      |
+----------------------------------+-------------------------------------------------------+
| `dispxp`                         | Horizontal angular dispersion function                |
+----------------------------------+-------------------------------------------------------+
| `dispyp`                         | Vertical angular dispersion function                  |
+----------------------------------+-------------------------------------------------------+


circle
^^^^^^

Beam of randomly distributed particles with a uniform distribution within a circle in each
dimension of phase space - `x` & `xp`; `y` & `yp`, `T` & `E` with each uncorrelated.
Each parameter defines the maximum absolute extent in that dimension, i.e. the possible values
range from `-envelopeX` to `envelopeX` for example. Total
energy is also uniformly distributed between $\pm$ `envelopeE`.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `envelopeR`                      | Maximum position                                      |
+----------------------------------+-------------------------------------------------------+
| `envelopeRp`                     | Maximum canonical momentum                            |
+----------------------------------+-------------------------------------------------------+
| `envelopeT`                      | Maximum time offset [s]                               |
+----------------------------------+-------------------------------------------------------+
| `envelopeE`                      | Maximum energy offset [GeV]                           |
+----------------------------------+-------------------------------------------------------+


square
^^^^^^

This distribution has similar properties to the `circle`_ distribution, with the
exception that the particles are randomly uniformly distributed within a square. Total
energy is also uniformly distributed between $\pm$ `envelopeE`.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `envelopeX`                      | Maximum position in X [m]                             |
+----------------------------------+-------------------------------------------------------+
| `envelopeXp`                     | Maximum canonical momentum in X                       |
+----------------------------------+-------------------------------------------------------+
| `envelopeY`                      | Maximum position in Y [m]                             |
+----------------------------------+-------------------------------------------------------+
| `envelopeYp`                     | Maximum canonical momentum in Y                       |
+----------------------------------+-------------------------------------------------------+
| `envelopeT`                      | Maximum time offset [s]                               |
+----------------------------------+-------------------------------------------------------+
| `envelopeE`                      | Maximum energy offset [GeV]                           |
+----------------------------------+-------------------------------------------------------+


ring
^^^^

The ring distribution randomly and uniformly fills a ring in `x` and `y` between two radii. For
all other parameters, the `reference`_ coordinates are used, i.e. `xp`, `yp` etc.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `Rmin`                           | Minimum radius in `x` and `y` [m]                     |
+----------------------------------+-------------------------------------------------------+
| `Rmax`                           | Maximum radius in `x` and `y` [m]                     |
+----------------------------------+-------------------------------------------------------+

* No variation in `z`, `xp`, `yp`, `t`, `s` and total energy. Only central values.


eshell
^^^^^^

Defines an elliptical annulus in phase space in each dimension that's uncorrelated.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+--------------------------------------------------------------------+
| Option                           | Description                                                        |
+==================================+====================================================================+
| `shellX`                         | Ellipse semi-axis in phase space in horizontal position [m]        |
+----------------------------------+--------------------------------------------------------------------+
| `shellXp`                        | Ellipse semi-axis in phase space in horizontal canonical momentum  |
+----------------------------------+--------------------------------------------------------------------+
| `shellY`                         | Ellipse semi-axis in phase space in vertical position [m]          |
+----------------------------------+--------------------------------------------------------------------+
| `shellYp`                        | Ellipse semi-axis in phase space in vertical momentum              |
+----------------------------------+--------------------------------------------------------------------+
| `shellXWidth`                    | Spread of ellipse in phase space in horizontal position [m]        |
+----------------------------------+--------------------------------------------------------------------+
| `shellXpWidth`                   | Spread of ellipse in phase space in horizontal canonical momentum  |
+----------------------------------+--------------------------------------------------------------------+
| `shellYWidth`                    | Spread of ellipse in phase space in vertical position [m]          |
+----------------------------------+--------------------------------------------------------------------+
| `shellYpWidth`                   | Spread of ellipse in phase space in vertical momentum              |
+----------------------------------+--------------------------------------------------------------------+
| `sigmaE`                         | Extent of energy spread in fractional total energy. Uniformly      |
|                                  | distributed between $\pm$ `sigmaE`.                                |
+----------------------------------+--------------------------------------------------------------------+

* No variation in `t`, `z`, `s`. Only central values.

.. _beam-halo-distribution:

halo
^^^^
The halo distribution is effectively a flat phase space with the central beam core removed at
:math:`\epsilon_{\rm core}`. The beam core is defined using the standard Twiss parameters described
previously. The implicit general form of a rotated ellipse is

.. math::

   \gamma x^2 + 2\alpha\;x\;x^{\prime} + \beta x^{\prime 2} = \epsilon

where the parameters have their usual meanings. A phase space point can be rejected or weighted
depending on the single particle emittance, which is calculated as

.. math::
   \epsilon_{\rm SP} = \gamma x^2 + 2\alpha\;x\;x^{\prime} + \beta x^{\prime 2}

if the single particle emittance is less than beam emittance, such that :math:`\epsilon_{\rm SP} < \epsilon_{\rm core}`
the particle is rejected. `haloPSWeightFunction` is a string that selects the function
:math:`f_{\rm haloWeight}(\epsilon_{\rm SP})` which is 1 at the ellipse defined by :math:`\epsilon_{\rm core}`. The
weighting functions are either `flat`, one over emittance `oneoverr` or exponential `exp`.

.. math::
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = 1 \\
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = \left(\frac{\epsilon_{\rm core}}{\epsilon_{\rm SP}}\right)^p \\
   f_{\rm haloWeight}(\epsilon_{\rm SP}) & = \exp\left(-\frac{\epsilon_{SP}-\epsilon_{\rm core}}{p \epsilon_{\rm core}}\right)

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-----------------------------------------------------------------------------+
| Option                           | Description                                                                 |
+==================================+=============================================================================+
| `emitx`                          | Horizontal beam core emittance [m] :math:`\epsilon_{{\rm core},x}`          |
+----------------------------------+-----------------------------------------------------------------------------+
| `emity`                          | Vertical beam core emittance [m] :math:`\epsilon_{{\rm core},y}`            |
+----------------------------------+-----------------------------------------------------------------------------+
| `betx`                           | Horizontal beta function [m]                                                |
+----------------------------------+-----------------------------------------------------------------------------+
| `bety`                           | Vertical beta function [m]                                                  |
+----------------------------------+-----------------------------------------------------------------------------+
| `alfx`                           | Horizontal alpha function                                                   |
+----------------------------------+-----------------------------------------------------------------------------+
| `alfy`                           | Vertical alpha function                                                     |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaXInner`               | Inner radius of halo in x (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaXOuter`               | Outer radius of halo in x (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaYInner`               | Inner radius of halo in y (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloNSigmaYOuter`               | Outer radius of halo in y (multiples of sigma)                              |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloPSWeightFunction`           | Phase space weight function [string]                                        |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloPSWeightParameter`          | Phase space weight function parameters []                                   |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloXCutInner`                  | X position cut in halo (multiples of sigma)                                 |
+----------------------------------+-----------------------------------------------------------------------------+
| `haloYCutInner`                  | Y position cut in halo (multiples of sigma)                                 |
+----------------------------------+-----------------------------------------------------------------------------+

* No variation in `t`, total energy, `z` and `s`. Only central values.

Example::

  beam, particle              = "e-",
        energy                = 1.0*GeV,
        distrType             = "halo",
        betx                  = 0.6,
        bety                  = 1.2,
        alfx                  = -0.023,
        alfy                  = 1.3054,
        emitx                 = 5e-9,
        emity                 = 4e-9,
        haloNSigmaXInner      = 0.1,
        haloNSigmaXOuter      = 2,
        haloNSigmaYInner      = 0.1,
        haloNSigmaYOuter      = 2,
        haloPSWeightParameter = 1,
        haloPSWeightFunction  = "oneoverr";

.. _beam-composite:

composite
^^^^^^^^^

The horizontal, vertical and longitudinal phase spaces can be defined independently. The `xDistrType`,
`yDistrType` and `zDistrType` can be selected from all the other beam distribution types. All of the
appropriate parameters need to be defined for each individual distribution.

* All parameters from `reference`_ distribution are used as centroids.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `xDistrType`                     | Horizontal distribution type                          |
+----------------------------------+-------------------------------------------------------+
| `yDistrType`                     | Vertical distribution type                            |
+----------------------------------+-------------------------------------------------------+
| `zDistrType`                     | Longitudinal distribution type                        |
+----------------------------------+-------------------------------------------------------+

.. note:: It is currently not possible to use two differently specified versions of the same
	  distribution within the composite distribution, i.e. gaussTwiss (parameter set 1) for x
	  and gaussTwiss (parameter set 2) for y. They will have the same settings.

Examples: ::

  beam, particle="proton",
        energy=3500*GeV,
        distrType="composite",
        xDistrType="eshell",
        yDistrType="gausstwiss",
        zDistrType="gausstwiss",
        betx = 0.5*m,
        bety = 0.5*m,
        alfx = 0.00001234,
        alfy = -0.0005425,
        emitx = 1e-9,
        emity = 1e-9,
        sigmaE = 0.00008836,
        sigmaT = 0.00000000001,
        shellX  = 150*um,
        shellY  = 103*um,
        shellXp = 1.456e-6,
        shellYp = 2.4e-5,
        shellXWidth = 10*um,
        shellYWidth = 15*um,
        shellXpWidth = 1e-9,
        shellYpWidth = 1d-9;


userFile
^^^^^^^^

The `userFile` distribution allows the user to supply an ASCII text file with particle
coordinates that are tab-delimited. The column names and the units are specified in an
input string.

The file may also be compressed using gzip. Any file with the extension `.gz`
will be automatically decompressed during the run without any temporary files. This is
recommended, as compressed ASCII is significantly smaller in size.

If the number of particles to be generated with ngenerate is greater than the number of
particles defined in the file, the bunch generation will reload the file and read the
particle coordinates from the beginning. A warning will be printed out in this case.

This distribution reads lines at the start of each event to be memory efficient. However,
this prevents reading a whole file by the number of lines in the file unlike the :code:`ptc`
distribution that loads all lines and can use the beam option :code:`matchDistrFileLength`.

.. note:: For gzip support, BDSIM must be compiled with GZIP. This is normally sourced
	  from Geant4 and is on by default.

* tar + gz will not work. The file must be a single file compressed through gzip only.
* Lines starting with `#` will be ignored.
* Empty lines will also be ignored.
* A warning will be printed if the line is shorter than the number of variables specified
  in `distrFileFormat` and the event aborted - the simulation safely proceeds to the next event.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `distrFile`                      | File path to ASCII data file                          |
+----------------------------------+-------------------------------------------------------+
| `distrFileFormat`                | A string that details the column names and units      |
+----------------------------------+-------------------------------------------------------+
| `nlinesIgnore`                   | Number of lines to ignore when reading user bunch     |
|                                  | input files                                           |
+----------------------------------+-------------------------------------------------------+
| `nlinesSkip`                     | Number of lines to skip into the file. This is for    |
|                                  | number of coordinate lines to skip. This also counts  |
|                                  | comment lines.                                        |
+----------------------------------+-------------------------------------------------------+
| `matchDistrFileLength`           | Option for certain distributions to simulate the same |
|                                  | number of events as are in the file. Currently only   |
|                                  | for the `ptc` distribution.                           |
+----------------------------------+-------------------------------------------------------+

Skipping and Ignoring Lines:

* `nlinesIgnore` is intended for header lines to ignore at the start of the file.
* `nlinesSkip` is intended for the number of particle coordinate lines to skip after `nlinesIgnore`.
* `nlinesSkip` is available as the executable option :code:`--distrFileNLinesSkip`.
* The number of lines skipped from a file is `nlinesIgnore` + `nlinesSkip`. The user could use
  only one of these, but only `nlinesSkip` is available through the executable option described above.
* If more events are generated than are lines in the file, the file is read again including the skipped
  lines.

Examples:

1) `nlinesIgnore=1` and `nlinesSkip=3`. The first four lines are ignored always in the file.
2) `nlinesIgnore=1` in the input gmad and `--distrFileNLinesSkip=3` is used as an executable option.
   The first four lines are skipped. The user has the option of controlling the 3 though - perhaps
   for another instance of BDSIM on a compure farm.

Acceptable tokens for the columns are:

+------------+----------------------------------------+
| **Token**  |  **Description**                       |
+============+========================================+
| "E"        | Total energy                           |
+------------+----------------------------------------+
| "Ek"       | Kinetic energy                         |
+------------+----------------------------------------+
| "P"        | Momentum                               |
+------------+----------------------------------------+
| "t"        | Time                                   |
+------------+----------------------------------------+
| "x"        | Horizontal position                    |
+------------+----------------------------------------+
| "y"        | Vertical position                      |
+------------+----------------------------------------+
| "z"        | Longitudinal position                  |
+------------+----------------------------------------+
| "xp"       | Horizontal angle                       |
+------------+----------------------------------------+
| "yp"       | Vertical angle                         |
+------------+----------------------------------------+
| "zp"       | Longitudinal angle                     |
+------------+----------------------------------------+
| "S"        | Global path length displacement,       |
|            | not to be used in conjunction with "z".|
+------------+----------------------------------------+
| "pt"       | PDG particle ID                        |
+------------+----------------------------------------+
| "w"        | Weight                                 |
+------------+----------------------------------------+
| "-"        | Skip this column                       |
+------------+----------------------------------------+

**Energy Units**
"eV", "KeV", "MeV", "GeV", "TeV"

**Length Units**
"m, "cm", "mm", "mum", "um", "nm"

**Angle Units**
"rad", "mrad", "murad", "urad"

**Time Units**
"s", "ms", "mus", "us", "ns", "mm/c", "nm/c"

Examples: ::

  beam, particle = "e-",
        energy = 1*GeV,
        distrType  = "userfile",
        distrFile  = "Userbeamdata.dat",
        distrFileFormat = "x[mum]:xp[mrad]:y[mum]:yp[mrad]:z[cm]:E[MeV]";


The corresponding `userbeamdata.dat` file looks like::

  0 1 2 1 0 1000
  0 1 0 1 0 1002
  0 1 0 0 0 1003
  0 0 2 0 0 1010
  0 0 0 2 0 1100
  0 0 0 4 0 1010
  0 0 0 3 0 1010
  0 0 0 4 0 1020
  0 0 0 2 0 1000


ptc
^^^

Output from MAD-X PTC used as input for BDSIM.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| Option                           | Description                                           |
+==================================+=======================================================+
| `distrFile`                      | PTC output file                                       |
+----------------------------------+-------------------------------------------------------+

* Reference offsets specified in the gmad file such as `X0` are added to each coordinate.

eventgeneratorfile
^^^^^^^^^^^^^^^^^^

To use a file from an event generator, the HepMC3 library must be used and BDSIM must be
compiled with respect to it.  See :ref:`installation-bdsim-config-options` for more details.

When using an event generator file, the **design** particle and total energy must still be
specified. These are used to calculate the magnetic field strengths.

The following parameters are used to control the use of an event generator file.

.. tabularcolumns:: |p{3cm}|p{14cm}|

+----------------+---------------------------------------------------------------------+
| Option         | Description                                                         |
+================+=====================================================================+
| distrType      | This should be "eventgeneratorfile:format" where format is one of   |
|                | the acceptable formats listed below.                                |
+----------------+---------------------------------------------------------------------+
| distrFile      | The path to the input file desired.                                 |
+----------------+---------------------------------------------------------------------+

.. warning:: Only particles available through the chosen physics list can be used otherwise they will
	     not have the correct properties and will **not be** added to the primary vertex and are
	     simply skipped. The number (if any) that are skipped will be printed out for every event.
	     We recommend using the physics list :code:`option, physicsList="all_particles";` to
	     define all particles without any relevant physics list. This can be used in combination
	     with other physics lists safely.

.. warning:: If the executable option `-\\-generatePrimariesOnly` is used, the coordinates will
	     not reflect the loaded event and will only be the reference coordinates. This is
	     because when this option is used, no Geant4 model is built. The event generator
	     file loader is significantly different from the other distributions and effectively
	     replaces the primary generator action. In this case, a small model of only a
	     drift with `option, worldMaterial="vacuum";` is the quickest way to achieve the
	     same thing.

* Compressed ASCII files (such as gzipped) cannot be used as HepMC3 does not support this.

The following formats are available:

* `hepmc2` - HepMC2 data format
* `hepmc3` - HepMC3 data format
* `hpe` - HEP EVT format (fortran format)
* `root` - HepMC ROOT format (not BDSIM's)
* `treeroot` - HepMC ROOT tree format (not BDSIM's)
* `lhef` - LHEF format files

These are put together with "eventgeneratorfile" for the `distrType` parameter. e.g.
:code:`distrType="eventgeneratorfile:hepmc2";`.

Examples can be found in `bdsim/examples/features/beam/eventgeneratorfile`. Below are some
examples: ::

  option, physicsList="g4FTFP_BERT";
  beam, particle = "proton",
        energy = 6.5*TeV,
	distrType = "eventgeneratorfile:hepmc3",
	distrFile = "/Users/nevay/physics/lhcip1/sample1.dat";

sphere
^^^^^^

The `sphere` distribution generates a distribution with a uniform random direction at one location.
Points are randomly and uniformly generated on a sphere that are used in a unit vector for the
momentum direction. This is implemented using `G4RandomDirection`, which in turn uses the
Marsaglia (1972) method.

* `Xp0`, `Yp0`, `Zp0` are ignored.
* `X0`, `Y0`, `Z0`, `S0`, `T0` can be used for the position of the source.
* No energy spread.

If an energy spread is desired, please use a :ref:`beam-composite` distribution.

An example can be found in `bdsim/examples/features/beam/sphere.gmad`. Below is an example: ::

  beam, particle = "proton",
        energy = 1.2*GeV,
	distrType = "sphere",
	X0 = 9*cm,
	Z0 = 0.5*m;


.. _controlling-simulation-speed:

Controlling Simulation Speed
----------------------------

The particle showers created in high energy particle interactions with matter can lead to a
very large number of particles being produced in an event. These in turn each take time to
track through the model and the computational time per event increases. When simulating a
very high-energy scale, the user may not be interested in very low-energy particles, however
these may dominate the simulation time.

To improve efficiency, there are several options the user can adjust. These however may reduce
the accuracy of the results obtained and must be used cautiously and only where required.

Range Cuts
^^^^^^^^^^

The production range cuts are the recommended method from Geant4, who strongly advocate
these over energy-based tracking cuts. These produce the most accurate results while
reducing simulation time. Approximately, these are the length a secondary must travel
before interacting. If the secondary would not travel further than this (depending on
the secondary species, physics lists, material and energy), the secondary will not
be produced. These can be set globally or for a *region* (see :ref:`regions`) that is attached
to individual volumes through the "region" parameter for that accelerator element. In
fact, a range cut always exists in Geant4 (to prevent infrared divergence) and is by
default 0.7 mm.
::

   rangecut=3*cm;
   option, prodCutPhotons   = rangecut,
           prodCutElectrons = rangecut,
           prodCutPositrons = rangecut,
           defaultRangeCut  = rangecut;

.. warning:: The range cut should **not** be longer than the typical dimension of the objects
	     (i.e. a range cut of 1 km is likely to produce very rough energy deposition
	     around boundaries).

Minimum Kinetic Energy
^^^^^^^^^^^^^^^^^^^^^^

The user may specify a minimum kinetic energy, below which any particle will be killed.
This may break conservation of energy if used aggressively. The default is 0 eV, as all
particles are tracked to zero energy (allowing for the above range cuts). ::

   option, minimumKineticEnergy=10*MeV;

.. warning:: This will affect the location of energy deposition - i.e. the curve of
	     energy deposition of a particle showering in a material will be different.

Minimum Range
^^^^^^^^^^^^^

The user may specify a minimum range for a particle to travel. Any particles with step
sizes proposed below this will be killed. Again, this can break energy conservation
if used aggressively. ::

  option, minimumRange=2*cm;

.. warning:: This will affect the location of energy deposition - i.e. the curve of
	     energy deposition of a particle showering in a material will be different.


.. _physics-biasing:

Physics Biasing
---------------

BDSIM currently provides two ways to artificially interfere with the physics processes
to make the desired outcome happen more often. In both cases, the goal is to simulate
the correct physical outcome, but more efficiently in the parameters of interest,
i.e. variance reduction.

The two cases are :ref:`physics-bias-cross-section-biasing` and
:ref:`physics-bias-importance-sampling`, each described below.

.. _physics-bias-cross-section-biasing:

Cross-Section Biasing
^^^^^^^^^^^^^^^^^^^^^

The cross-section for a physics process for a specific particle can be artificially altered
by a numerical scaling factor using cross-section biasing (up or down scaling it). This is
done on a per-particle and per-physics-process basis.  The biasing is defined with the
keyword **xsecbias**, to define a bias 'object'. This can then be attached to various bits
of the geometry or all of it. This is provided with the Geant4 generic biasing feature.

Geant4 automatically includes the reciprocal of the factor as a weighting, which is
recorded in the BDSIM output as "weight" in each relevant piece of data. Any data
used should be multiplied by the weight to achieve the correct physical result.

Generally, one should understand that Geant4 has particle definitions and physics processes
are attached to these. e.g. "protonElastic" is a physics process that's attached to the
(unique) definition of a proton. There can be many individual proton tracks, but there is
only one proton definition.

.. note:: This only works with Geant4 version 10.1 or higher. It does not work Geant4.10.3.X series.

+------------------+------------------------------------------------------+
| **Parameter**    | **Description**                                      |
+==================+======================================================+
| name             | Biasing process name                                 |
+------------------+------------------------------------------------------+
| particle         | Particle that will be biased                         |
+------------------+------------------------------------------------------+
| proc             | Process(es) to be biased                             |
+------------------+------------------------------------------------------+
| xsecfact         | Biasing factor(s) for the process(es)                |
+------------------+------------------------------------------------------+
| flag             | Flag which particles are biased for the process(es)  |
|                  | (1=all, 2=primaries, 3=secondaries)                  |
+------------------+------------------------------------------------------+

* Particle names should be exactly as they are in Geant4 (case-sensitive). The
  best way to find these out is to the run a single event with the desired physics
  list and the executable option `--printPhysicsProcesses`. Also the input option
  `option, physicsVerbose=1;` will show the primary particle and all physics processes
  registered to it by name.
* The process name should be exactly as they are in Geant4 (case-sensitive). Similarly,
  the best way to find these names is to run a single event with the desired physics
  list using the input option `option, physicsVerbose=1;` to see all the names of the
  physics processes.
* A special particle name "all" will bias all defined particles. (case-sensitive).
* In the case of an **ion** beam, the particle name should be "GenericIon". The
  biasing will apply to all ions, so the flag should be used to select primary
  or secondary or all particles. This is because Geant4 uses the concept of a
  generic ion as there are so many possible ions.
* Examples can be found in :code:`bdsim/examples/features/processes/5_biasing`.
* The option :code:`option, printPhysicsProcesses=1;` or executable option
  :code:`--printPhysicsProcesses` will print out all particle names and all
  the physics processes registered for each particle. This is useful to get
  the exact particle names and process names. We recommend running one event
  with the desired physics list, or a complete Geant4 one such as
  :code:`option, physicsList="g4FTFP_BERT";` to see all particles and processes.

Example::

  biasDef1: xsecBias, particle="e-", proc="all", xsecfact=10, flag=3;
  biasDef2: xsecBias, particle="e+", proc="eBrem eIoni msc", xsecfact={10,1,5}, flag={1,1,2};

The process can also be attached to a specific element using the keywords `biasVacuum` or
`biasMaterial` for the biasing to be attached the vacuum volume or everything outside the
vacuum respectively::

  q1: quadrupole, l=1*m, material="Iron", biasVacuum="biasDef1 biasDef2"; ! uses the process biasDef1 and biasDef2
  q2: quadrupole, l=0.5*m, biasMaterial="biasDef2";

.. _physics-bias-importance-sampling:
  
Geometric Importance Sampling
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To enable importance sampling, the user must provide both a mass world and a separate importance
sampling world as external geometry files. The mass world file should contain the appropriate
volumes as if you were conducting a standard simulation without importance sampling. The
importance world file should contain the volumes that will be the importance cells only. A
third text file must also be provided which contains a map of the physical volumes that form
the importance cells and their corresponding importance volumes.

+------------------------------+-------------------------------------------------------------+
| **Parameter**                | **Description**                                             |
+==============================+=============================================================+
| worldGeometryFile            | Geometry file containing the mass world                     |
+------------------------------+-------------------------------------------------------------+
| importanceWorldGeometryFile  | Geometry file containing the importance sampling world      |
+------------------------------+-------------------------------------------------------------+
| importanceVolumeMap          | ASCII file containing a map of the importance world         |
|                              | physical volumes and their corresponding importance values  |
+------------------------------+-------------------------------------------------------------+

Example: ::

  option, worldGeometryFile="gdml:shielding-world.gdml",
          importanceWorldGeometryFile="gdml:importance-cell-world.gdml",
          importanceVolumeMap="importanceValues.dat";

An example of the world volume geometry (top), the importance sampling world geometry (middle), and
an importance volume map (bottom) are shown below with an example beamline.

In the output a new branch in the event tree calls "ElossWorldContents" is automatically added
when using importance sampling. This is the global energy deposition hits from any volumes
that were in the externally supplied world - such as shielding blocks. This distinguishes
the energy deposition in the world volume itself (i.e. the air).

.. figure:: figures/importanceSampling_massWorld.png
	    :width: 90%
	    :align: center

.. figure:: figures/importanceSampling_importanceWorld.png
	    :width: 90%
	    :align: center

.. figure:: figures/importanceSampling_VolumeMap.png
	    :width: 90%
	    :align: center

		    
* Both the mass world and importance sampling world must be the same size.
* Both the mass world and importance sampling world must be large enough to encompass the machine
  beamline. If not, BDSIM will exit.
* It is down to the user to ensure the importance cells are correctly positioned.
* If a importance cell volume exists in the importance world geometry and is not listed
  in the ASCII map file with a importance value, BDSIM will exit.
* The importance sampling world volume has an importance value of 1.

	     
.. _bend-tracking-behaviour:
	    
Bends
-----

Fringe Field Integral Behaviour
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Fringe fields can be specified for dipole magnets through the parameters `hgap`, `fint` and `fintx`.
`fint` is the fringe field integral, as described in :ref:`dipole-fringe-integrator` for the entrance
face of the dipole. `fintx` is for the same, but for the exit face. Even when there is no pole face
rotation, there is still a small fringe field effect.

If `fint` is specified but `fintx` is not, `fintx` will default to the same value as `fint`. If,
however, `fintx` is set to 0, it will in fact be 0 and will not take the value of `fint`. This is
the same default behaviour as MAD-X. MAD-X will write out a value of `fintx` as -1 in this case in
any output. BDSIM will write out the value used, even if it's equal to 0.

Pole Face Rotations
^^^^^^^^^^^^^^^^^^^

The `bdsimtwo` integrator set (see :ref:`integrator-sets`) provides tracking through a uniform
magnetic field in a dipole. The field exists wherever the magnet exists; in the case of pole
face rotations on the end of a dipole, the magnet is constructed with the appropriate angled face.
The field therefore also has a hard edge with exactly no field immediately outside the magnet volume.

The tracking routine for dipoles in the `bdsimtwo` integrator set (see :ref:`bdsim-dipole-rodrigues2`)
tracks the particle using the analytical helical solution in a pure magnetic field in Cartesian
coordinates. This however does not agree with the tracking provided by MAD-X. We therefore provide
an equivalent to MAD-X in `bdsimmatrix` integrator set (the default). The vertical focussing provided
by the fringe field is the same in both cases.

The difference between the two is negligible for small pole face angles - for example, the LHC lattice
shows no difference between the integrator sets (~14mrad bending angle and very low pole face angles).
However, with higher angle bends and stronger pole face angles (maximum is up to 45 degrees), the
difference is non-negligible.

The integrator for dipoles in `bdsimtwo` is computationally faster and should be used for lattices
like the LHC, where speed matters and the pole faces are not a strong feature.

.. note:: To provide equivalent tracking to MAD-X with the `bdsimmatrix` integrator set, the
	  magnet geometry is constructed with flat ends (i.e. always an sbend). Rbends are constructed
	  as sbends with additional poleface rotation angles equal to half the bend angle. Instead of
	  constructing the poleface geometry, the effect of a poleface rotation is applied in a thin
	  fringefield magnet (1 micron thick by default) at the beginning (for non-zero e1) or at the
	  end (for non-zero e2) of the dipole. In future, this will be decoupled to allow both the
	  physical angled faces in the model as well as accurate tracking, using the MAD-X style matrix
	  integrators.

Large Angle Bends
^^^^^^^^^^^^^^^^^
For a model that includes large angle bends (for example > 0.1rad), the user should consider reducing
the sampler diameter (see :ref:`sampler-dimensions` and :ref:`options-geometry`). This is because
the default 5m width of a sampler may cause overlaps between samplers, or each sampler may record
particles from multiple positions in the beam line.

One other point is that the parallel geometry used for curvilinear transforms (the "curvilinear world")
may overlap with other curvilinear elements earlier in the beam line. The size of the curvilinear
world `cylinders` is based on the samplerDiameter and reducing the samplerDiameter will reduce its size.
There is some automatic provision for this in BDSIM where the sampler diameter is automatically reduced
when large angle bends are present in the lattice, but this is based on a heuristic, rather than direct
overlap checks.

In short, we recommend running with :code:`option, checkOverlaps=1;` once to verify there are no
problems for a machine with large angle bends. If there are any overlaps, reduce the sampler diameter
to the typical full width of a magnet.
