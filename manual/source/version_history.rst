V1.4 - 2019 / 05 / 20
=====================

Expected Changes To Results
---------------------------

* Any wirescanner elements should be updated to use :code:`wireAngle` instead of :code:`angle` for
  their rotation angle. Not doing this will result in different angles and therefore results.

New Features
------------

* Improved event level verbosity.
* All verbosity options now documented, including corresponding executable options.
* BDSIM will now exit if invalid ranges and bins are specified for the single 3D
  energy deposition ('scoring') histogram that can be specified via options.
* New verbose event stepping options. See :ref:`bdsim-options-verbosity` for more details.
* New beam loss monitors (BLMs) with :code:`blm` command (See ref:`detectors-blms`).
* New executable option :code:`--distrFileNLinesSkip` for the number of lines to skip into
  a distribution file.
* Support for partially stripped ions in output samplers.

* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|
  
+-----------------------------------+--------------------------------------------------------------------+
| **Option**                        | **Description**                                                    |
+===================================+====================================================================+
| storeApertureImpacts              | Create an optional branch called "ApertureImpacts" in the Event    |
|                                   | tree in the output that contains coordinates of where the primary  |
|                                   | particle exists the beam pipe. Note this could be multiple times.  |
+-----------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsIons          | If `storeApertureImpacts` is on, the information will be generated |
|                                   | for all secondary ions as well as the primay. No information will  |
|                                   | be generated for other particles.                                  |
+-----------------------------------+--------------------------------------------------------------------+
| storeApertureImpactsAll           | If `storeApertureImpacts` is on, the information will be generated |
|                                   | for all particles leaving the beam pipe when this option is turned |
|                                   | on.                                                                |
+-----------------------------------+--------------------------------------------------------------------+


General
-------

* Executable verbosity options, now accepted in input gmad.
* Valid default ranges for general single 3D energy deposition 'scoring' histogram
  available through options. Now 1m in x,y,z with 1 bin.
* wirescanner element now uses :code:`wireAngle` for the rotation angle and not :code:`angle`.
* wirescanner element now requires a material to be specified as this makes a large difference
  to the expected result. This should be specified.
* Sampler hits now store rigidity, mass and charge as these are only correct from the G4DynamicParticle
  and cannot be reliably or easily back-calcualted afterwards based on the particle definition (PDG ID)
  for partially stripped ions. This storage marginally increasese the memory usage per sampler hit, so
  a small increase in memory (RAM) usage may be observed for very large numbers of sampler hits.
  
Bug Fixes
---------

* Fixed warnings about exiting when Geant4 geometry in closed state in the event
  of a warning being produced and BDSIM exiting. Now correctly intercept and re-throw
  the exception.
* Fix a bug where setting a rotation angle for a wire scanner would result in energy deposition
  S coordinates all being -1. This was because the :code:`angle` parameter is assumed to only
  ever be for bends and BDSIM reduces the sampler and curvilinear world (used for coordinate
  transforms) diameter given the maximum bending angle of bends in the whole lattice. This is
  required to avoid overlaps before construction. The new parameter :code:`wireAngle` is used
  instead.
* Partial fix for aggressive looping particle killing in Geant4.10.5. For electrons and positrons,
  and the beam particle, the looping threshold has be lowered to 1 keV. Ongoing investigation.
* The rigidity was correcte for partially stripped ions in the sampler output.
* The initial kinetic energy of partially stripped ions was slightly inflated due to subtracting
  the nuclear mass not including the mass of the electrons. The magnetic fields were however
  calculated correctly and this resulted in incorrect behaviour. This has been since fixed.
* Fix a bug where if a userfile with different particle types was used and `-\\-generatePrimariesOnly`
  was used the phase space coordinates would be correct but the mass, charge, rigidity would be
  written wrongly to the output. The particle definition is now updated correctly in the special
  case of generating primaries only where the Geant4 kernel isn't used.

Output Changes
--------------

* Samplers now have a new variable called `nElectrons` that is the number of electrons on a
  partially stripped ion (if it is one) passing through the sampler. This is filled alongside
  the other ion information.
* `isIon`, `ionA` and `ionZ` are now non-zero when a Hydrogen ion with one or two electrons
  passes through a sampler.
* All extra coordinates are now recorded in the Primary sampler structure no matter if these
  are turned on or not for the samplers.

Utilities
---------

* pybdsim v2.1.0
* pymadx v1.7.1
* pymad8 v1.5.0
* pytransport v1.3.0


V1.3.3 - 2019 / 05 / 21
=======================

Bug Fixes
---------

* Hot fix for fields not attached to thin elements such as dipole fringes or thin multipoles. This bug
  crept in through a modification to avoid Geant4 getting stuck with strong fields in very narrow gaps
  between layers of geometry in beam pipes, resulting in subsequent bad tracking due to the bad state of
  Geant4 navigators internally. Regression testing has subsequently been introduced to protect against
  this kind of bugging going unnoticed in future.

V1.3.2 - 2019 / 04 / 20
=======================

New Features
------------

* Can now use any particle available through the physics list for a beam particle.
* Generic cross-section biasing can now be used for any particle, including ions.

General
-------

* Tested with Geant4.10.5.p01
* Geometry navigators are reset at the beginning of a run now in a similar way to the start of
  an event to ensure independence between runs - future proofing.
* For Geant4.10.5, we now use the 'low' looping particle thresholds for tracking.
* The 'vacuum' field is now not applied to the container volume of a beam pipe. However, it is
  still applied to the vacuum and beam pipe volumes. This makes the tracking more robust against
  stuck particles in the extremely small gap between volumes.
* The yoke magnetic field now uses a wrapped G4ClassicalRK4 integrator. This wrapper acts as
  a drift for short (< 1um) steps. This makes tracking more robust for secondaries in the yoke.
* Improve testing for user bunch distribution for robustness.
* Increase transverse length safety margin between beam pipes and magnet volumes for safety.
* Translate bunch coordinates in global coordinates backwards by 1x length safety to avoid
  starting on a volume boundary at the start of the event. This is 1nm so will not affect
  tracking results. The local coordinates in the output are identically the same.

Bug Fixes
---------

* Fix strong recreation when using user file supplied bunch distribution. The file was
  always read from the beginning in the past. Now the correct coordinates will be
  read and the event is correctly reproduced.
* Fix userinterface example given changes to sensitive detector manager - simple edit.
* Fix calculated phase offset for rfcavity in the beam line. This was peak at the
  end of the element rather at the middle.
* Fix possible segfault if event aborted due to extra collimator hit information.
* Fix user file beam loader for anomalous particle coordinates generated once at the end
  of a file if the end of the file was a blank line. It would result in all zero coordinates
  and beam energy x1000 for the first particle generated after the end of the file was reached.
* Fix abort of event if unknown particle ID specified in user file beam loader.
* Fix user file distribution file loading for comment lines, incomplete lines and empty
  (white space) lines.
* Fix phase offset calculation for rf cavities with respect to nominal value. Phase would have
  been smaller than intended. It was scaled to :math:`1/2\pi` instead of :math:`2\pi`.
* Fix ambiguity in manual for rf cavities. Time is generally in seconds in BDSIM, however the
  rf cavity took nanoseconds. A time offset of `1*ns` in the input gmad would result in double
  units.
* Fix warning when loading an output file with data loader class when the file was created
  without storing primary coordinates. The warning was related to the PrimaryGlobal branch.
* Fix warnings and artificial killing of particles by high looping particle thresholds for
  Geant4.10.5, which are default. Use the 'low' looping thresholds by default. Issue #268.
* Fix stuck particles by attaching the vacuum field in a beam pipe to every volume in the
  beam pipe apart from the container volume to avoid navigation problems in very thin gaps. Issue #268.
* Remove half-implemented integrator types in internal dictionaries.
* Fixed model-model example conversion Python scripts as these were specific to the developer's computer.
* Fix coil end-piece placement with respect to main magnet body - now includes required length safety
  gap to avoid possible navigation issues with large sized models.
* Fix for exotic particle beams. Can now use any particle available in the physics list.
  Particle definitions now constructed earlier than in the regular physics list call.
* Fix bad tracking in undulators caused by nan values in field caused by querying the field
  at arbitrarily large positions. Limited the range of validity of the field to the beam pipe. The
  field is now no longer attached to the magnets outside the beam pipe.
* Biasing was not attached to components that were found to be unique in construction - i.e. an
  rbend back-to-back with another rbend will not have fringe fields in the middle at the join, so
  is considered a unique construction. This would result in these not having biasing attached.

Utilities
---------

* pybdsim v2.1.0
* pymadx v1.7.1
* pymad8 v1.5.0
* pytransport v1.3.0


V1.3.1 - 2019 / 03 / 05
=======================

Bug Fixes
---------

* Fix transform3d element where offsets were not working.


V1.3 - 2019 / 02 / 27
=====================

Expected Changes To Results
---------------------------

* The density of the surrounding air has changed very slightly to that of the standard
  G4_AIR one.
* Energy deposition in vacuum is now separated into a separate branch and is not mixed
  with general Eloss. Therefore, less energy deposition will be seen in the Eloss branch.
* The minimum kinetic energy option will now be respected when using a Geant4 reference
  physics list, whereas it wasn't before.
* The range cuts can now be applied when using a Geant4 reference physics list if the
  option :code:`g4PhysicsUseBDSIMRangeCuts` is turned on (=1). Previously, these had no
  effect with a Geant4 reference physics list.

New Features
------------

* Support for Geant4.10.5.
* New environment script in :code:`<bdsim-install-dir>/bin/bdsim.sh` to make running BDSIM easier.
* All Geant4 reference physics lists are now available.
* New beam pipe aperture for the CLIC post collision line.
* New jaw collimator element "jcol" with two blocks in the horizontal plane.
* New wire scanner element "wirescanner" with cylindrical wire in a beam pipe.
* Completed CMake to allow user applications based on BDSIM to easily link against it.
* New :code:`dump` beam line element that is an infinite absorber. This prevents simulations
  running for a long time when particles may hit the air at the end of the beam line.
* BDSIM as a class for interfacing. Ability to add custom beam line components.
  See :ref:`interfacing-section`.
* New samplerplacement object that defines an arbitrarily placed sampler in the world that
  may overlap with anything (see :ref:`user-sampler-placement`).
* New importance sampling implementation when using a user-supplied world geometry. (see
  :ref:`physics-bias-importance-sampling`.

* New options:

.. tabularcolumns:: |p{0.30\textwidth}|p{0.70\textwidth}|
  
+----------------------------------+------------------------------------------------------------------+
| **Option**                       | **Description**                                                  |
+==================================+==================================================================+
| collimatorsAreInfiniteAbosrbers  | When turned on, all particles that enter the material of a       |
|                                  | collimator (`rcol`, `ecol` and `jcol`) are killed and the energy |
|                                  | recorded as deposited there.                                     |
+----------------------------------+------------------------------------------------------------------+
| geant4Macro                      | Fun an optional macro in the visualiser once it's started.       |
+----------------------------------+------------------------------------------------------------------+
| g4PhysicsUseBDSIMCutsAndLimits   | If on, the maximum step length will be limited to 110% of the    |
|                                  | component length - this makes the tracking more robust and is    |
|                                  | the default with a regular BDSIM physics list. The minimum       |
|                                  | kinetic option is also obeyed. Default off.                      |
+----------------------------------+------------------------------------------------------------------+
| g4PhysicsUseBDSIMRangeCuts       | If on, this will apply the BDSIM range cut lengths to the Geant4 |
|                                  | physics list used. This is off by default.                       |
+----------------------------------+------------------------------------------------------------------+
| ignoreLocalMagnetGeometry        | If turned on, this option means that only the magnet geometry    |
|                                  | from options will be used. Similar to `ignoreLocalAperture`.     |
+----------------------------------+------------------------------------------------------------------+
| importanceVolumeMap              | File path for text file that maps importance values to volumes.  |
+----------------------------------+------------------------------------------------------------------+
| importanceWorldGeometryFile      | File path for the externally provided geometry that will be used |
|                                  | as the parallel world for the importance sampling.               |
+----------------------------------+------------------------------------------------------------------+
| physicsEnergyLimitLow            | Control minimum energy for all physics models. (advanced)        |
+----------------------------------+------------------------------------------------------------------+
| physicsEnergyLimitHigh           | Control maximum energy for all physics models. (advanced)        |
+----------------------------------+------------------------------------------------------------------+
| minimumKineticEnergyTunnel       | Any particles below this energy (in GeV by default) will be      |
|                                  | artificially killed in all BDSIM-generated tunnel segments.      |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorInfo              | Store collimator structure with primary hits per collimator.     |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorHitsAll           | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, hits will be generated for all particles interacting  |
|                                  | with the collimators whether primary or secondary and whether    |
|                                  | ion or not.                                                      |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorHitsIons          | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, `isIon`, `ionA` and `ionZ` variables are filled.      |
|                                  | Collimator hits will now also be generated for all ions.         |
+----------------------------------+------------------------------------------------------------------+
| storeCollimatorLinks             | If `storeCollimatorInfo` is on and collimator hits are           |
|                                  | generated, extra information is stored for each collimator hit.  |
+----------------------------------+------------------------------------------------------------------+
| storeEloss                       | Ability to completely turn off generation of energy deposition   |
|                                  | hits to save memory usage and output file size. Default on.      |
+----------------------------------+------------------------------------------------------------------+
| storeElossModelID                | Control whether the beam line index is stored in the energy      |
|                                  | loss output. More granular than :code:`storeElossLinks`.         |
+----------------------------------+------------------------------------------------------------------+
| storeElossTurn                   | Control whether energy deposition turn number is saved.          |
+----------------------------------+------------------------------------------------------------------+
| storeElossVacuum                 | Control whether energy deposition in the residual gas in the     |
|                                  | beam pipe 'vacuum' is recorded.                                  |
+----------------------------------+------------------------------------------------------------------+
| storeElossWorld                  | Turn on generation of energy deposition in the world volume      |
|                                  | (i.e. the air) as well as record energy leaving the simulation.  |
|                                  | Default off.                                                     |
+----------------------------------+------------------------------------------------------------------+
| storeElossWorldContents          | Turn on generation and storage of energy deposition in any       |
|                                  | included with the externally provided world geometry. Off by     |
|                                  | default but turned on automatically when using importance        |
|                                  | sampling. Allows the user to distinguish energy deposition in    |
|                                  | the air as stored in ElossWorld from the contents of the world.  |
+----------------------------------+------------------------------------------------------------------+
| storeGeant4Data                  | Control whether the basic particle data is stored in the output  |
|                                  | for all particles used or not.                                   |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerAll                  | Conveniently store all optional sampler data with one option.    |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerKineticEnergy        | Store kinetic energy in the sampler output.                      |
+----------------------------------+------------------------------------------------------------------+
| storeSamplerPolarCoords          | Store the polar coordinates (r, phi and rp, phip) in the         |
|                                  | sampler output.                                                  |
+----------------------------------+------------------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | When turned on, any BDSIM-generated tunnel segments will absorb  |
|                                  | and kill any particle of any energy. Used to speed up the        |
|                                  | simulation. Default off.                                         |
+----------------------------------+------------------------------------------------------------------+
| worldGeometryFile                | External geometry file for world geometry.                       |
+----------------------------------+------------------------------------------------------------------+

* Access to data version in DataLoader in analysis.
* External geometry can be supplied as the world volume with the option
  :code:`worldGeometryFile`.
* New complete physics list for crystal channelling to achieve the correct result.
* New ability to specify a different beam particle that is different from the design
  particle used for magnetic field strength calculations (:code:`beamParticleName`).
* Specify the particle assumed for the user file distribution that can be different from
  the design particle.
* New option to use a one turn map generated from MAD-X PTC to correct
  multi-turn tracking for circular machines.
* New option :code:`geant4Macro` and executable option :code:`--geant4Macro` to run an optional
  macro in the visualiser once it's started.
* A warning will print if a user-defined material is more dense than 100g/cm3 as this is much higher
  than any naturally occurring material (on Earth). The simulation will still proceed.
* New optional collimator output structure in event made per collimator with prefix
  "COLL\_". Controlled by new option :code:`collimatorInfo`.
* New mini-summary of collimators in Model tree when :code:`collimatorInfo` option is used.
* New units "PeV", "J", and variations thereof for energy.
* New parameter for collimator elements :code:`minimumKineticEnergy` that allows the user to kill
  particles below a certain kinetic energy in a collimator.

General
-------

* All collimators now require a material to be specified and the default copper has
  been removed. This is because it strongly affects the results obtained and defaults
  should not be relied upon for this.
* The turn number for energy deposition hits is now automatically stored if
  a circular model is used.
* The `sensitiveBeamlineComponents` option has now been renamed to `sensitiveOuter`
  to better reflect its functionality. The old option is still accepted.
* The `tunnelSensitive` option has now been renamed to `storeElossTunnel` to be
  more consistent with the other sensitivity options. The old option is still
  accepted.
* The generic beam line element `element` now supports angle and the beam line
  will be curved by this amount.
* The world volume is now sensitive and can record energy deposition. Geant4.10.3 upwards
  is required to record both this information and the energy leaving the world
  as this requires G4MultiSensitiveDetector.
* New tests for testing backwards compatibility of analysis tool with previous data version.
* "Model Preparation" is now "Model Conversion" in the manual to be clearer.
* Visualisation now uses macro search path to look for visualisation macro in the installation
  directory then the build directory of BDSIM.
* In recreate mode, there is explicit print out about when the seed is set and if if was successfully
  loaded from the output file.
* The Cherenkov example has now been updated to show 3 materials (air, water, YAG).
* Fixes from static code analysis for virtual functions called in constructors of factories,
  shadow member variables and initialisation of crystal variables in parser.
* Significant reduction in use of the singleton pattern for beam pipe, magnet yoke,
  tunnel and geometry factories.
* Reduced memory usage for energy deposition hits by removing unused numbers stored each time.
* Reduced memory usage for energy deposition hits when not using extra variables such as the 'links'.


Materials
---------

* The materials construction in src/BDSMaterials.cc was checked through thoroughly.
* "air" is now G4_AIR instead of custom BDSIM air (similar composition). The old air is now "airbdsim".
* The refractive index data for optical and cherenkov physics has been added on top of G4_AIR
  as well as "airbdsim".
* "airbdsim" now has a density of 1.225mg/cm3.
* "bp_carbonmonoxide" material now has correct pressure (previously near infinite).
* Fixed double density for the following materials. They would have been extremely dense.
  
   - "berylliumcopper"
   - "stainless_steel_304L"
   - "stainless_steel_304L_87K"
   - "stainless_steel_316LN"
   - "stainless_steel_316LN_87K"
   - "tungsten_heavy_alloy"
   - "fusedsilica"
   - "n-bk7"
   - "yag"
   - "pet"
   - "lhc_rock"

* "niobium" is now "niobium_2k" to better reflect the unusual temperature.
* "nbti" is now "nbti_4k" to better reflect the unusual temperature.
* "waterCkov" has been removed. "water" or "G4_WATER" (the same) should be used. The refractive
  index data has been added to G4_WATER material.

Developer Changes
-----------------

* The BDSGeometryComponent base class now has the ability to specify which
  sensitive detector should be attached in a map using the BDSSDType enum. There is no default
  sensitive detector (previously general energy deposition) as the developer must be explicit
  about what sensitivity they want so nothing unexpected can happen.
* BDSBeamline can now return indices of beam line elements of a certain type.
* All sensitive detector classes have been renamed as have the accessor functions in BDSSDManager.
  This is to make the naming more consistent.
  
Bug Fixes
---------

* Fixed reloading user file when reading more particles than defined in the file.
* Fixed "pt" column in user file for reading particle PDG IDs. The first particle would be read
  correctly and all subsequent particles would revert to the beam definition.
* Fixed infinite tracking from nans return from field map when BDSIM format
  field map file was lacking lower and upper limits.
* Fixed incorrect writing of optional sampler information.
* The `sensitiveBeamPipe` option now works and controls whether the beam pipe produces
  energy loss or not. This does not affect the physics, merely whether output
  information is generated or not.
* The `sensitiveOuter` (formerly `sensitiveBeamlineComponents`) option has
  been fixed and now controls whether the parts outside the beam pipe in an
  element record energy loss or not.
* Degrader and undulator did not record energy deposition.
* Energy deposition is now correctly recorded when tracks are artificially killed.
* Fix crystal channelling with cylindrical and torus shaped crystals. The crystal implementation
  only works along the local X direction of any solid. Fixed by using a G4DisplacedSolid to
  allow use of more advanced geometries than a box.
* Fix channelling physics for standard EM and hadronic processes as this requires process biasing.
* Fix A and Z being the wrong way around for ions in samplers.
* Charge now correctly recorded in primaries and in samplers for partially stripped ions.
* Solenoid tracking fixed. Fringes are constructed as appropriate according to integrator set.
* Fix possible nan values given to Geant4 tracking with miscalculated auto-scaling value for
  field maps.
* Fix setting default seed state for random number generator if using recreate mode
  and progressing beyond an event stored in the file.
* Fix setting the energy level of an ion - wasn't set from input.
* SQL geometry factory didn't clean up after repeated use. This geometry isn't
  generally supported.
* Fixed a bug where very weak actions on particles in tracking would not be taken due to
  too stringent tests of finite numbers. This would result in particles with small offsets
  in magnets or particles with high momentum that would see only very small deviations being
  tracked as if it were a drift.
* Fixed segfault crash from ROOT with rebdsim when there were more dimensions in the variables
  than the declared number of dimensions. For example, "y:x" for Histogram1D.
* Fixed rare bug where segfault would occur in trying to account for energy deposition of
  artificially killed particles.
* Fix memory leak of sampler structures (relatively small).
* Fixed parsing of + or - symbols with ion definition. Now supports H- ion.
* Fixed very slow memory leak associated with the primary trajectory. only visible for very
  large numbers of events.
* Fixed dipole tracking for off-charge ions - reverts to backup integrator.
* Fixed Pythonic range iteration of Event tree when trying to look at Info branch. Conflicted with
  Info method of TObject. Now renamed to Summary.
* Fixed catching the construction of dipoles with too large an angle. Limit rbends and unsplit
  sbends to a maximum angle of pi/2, limit the maximum angle of all other dipoles to 2 pi.
  
Output Changes
--------------

* "Info" branch of the Event and Run trees are now "Summary". This is to avoid conflict with
  ROOT TObject::Info() that could result in broken analysis or range iteration. The DataLoader
  class in analysis (used by pybdsim.Data.Load) is backwards compatible. In the case of loading
  older data with updated software, there will still be a member called Info that the data will
  be loaded into. Python range iteration cannot be used in this case.
* "TunnelHit" is now "EnergyLossTunnel" to be consistent. `rebdsim` and the analysis DataLoader
  class (both Python and ROOT) are backwards compatible and both TunnelHit and ElossTunnel are
  available. Only the correct one is filled with loaded data during analysis.
* Much more granular control of what is stored in the output. See new options in 'new' section
  above.
* Vacuum energy deposition separated from general energy deposition and now in its own branch.
* Memory usage (for Mac & Linux) added at the end of each event in event info. This
  is the memory usage of the whole program at that point including event independent
  quantities such as the model.
* Boolean flag store in even info as to whether the primary was absorbed in a collimator or not.
* New options to control level of output as described in table in new features..
* Tunnel energy deposition hits now respond to the :code:`storeElossXXXX` options to control the
  level of detail with extra variables of their output.
* New class BDSOutputROOTEventLossWorld for a record of coordinates when a particle leaves a volume,
  use currently for exiting the world.
* New structures ("branches") in the `Event` tree called :code:`ElossWorld` and
  :code:`ElossWorldExit` for energy deposition in the world material and energy leaving
  the world (and therefore the simulation) respectively.
* New members in :code:`Event.Info` that are the integrated energy deposited in various parts
  for that event. These are for convenience and are the integrals of the various Eloss parts.

Output Class Versions
---------------------

* Data Version 4.

+-----------------------------------+-------------+-----------------+-----------------+
| **Class**                         | **Changed** | **Old Version** | **New Version** |
+===================================+=============+=================+=================+
| BDSOutputROOTEventBeam            | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCoords          | N           | 1               | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimator      | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventCollimatorInfo  | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLossWorld       | Y           | NA              | 1               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHeader          | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventHistograms      | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventInfo            | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventLoss            | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventModel           | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventOptions         | Y           | 3               | 4               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventRunInfo         | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventSampler         | Y           | 2               | 3               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectory      | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTEventTrajectoryPoint | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+
| BDSOutputROOTGeant4Data           | N           | 2               | 2               |
+-----------------------------------+-------------+-----------------+-----------------+


Utilities
---------

* pybdsim v2.0.0
* pymadx v1.7.0
* pymad8 v1.5.0
* pytransport v1.3.0


V1.2 - 2018 / 08 / 26
=====================

Highlights
----------

* :code:`outerDiameter` is now :code:`horizontalWidth` to better describe its function (backwards-compatible).
* Fixed dipole scaling with (the default) bdsimmatrix integrator set.
* Solenoid tracking fixed.

New Features
------------

* New options to activate extra models in em_extra physics list.
* New :code:`crystalcol` element for channelling crystals.
* New :code:`crystal` definition in parser.
* New "channelling" physics list for Geant4 crystal channelling physics process.
* Field maps need not be in `x`, `y`, `z`, `t` order and lower dimension fields (i.e. 1D or 2D) can
  be made for any dimension, i.e. it is now possible to specify a 1D field along the `z` direction.
* Rebdsim can now analyse a select range of events specified by "EventStart" and "EventEnd" options.
  Issue #240.
* Placements can now be made with respect to S,x, and y in the main beam line, with respect to a beam line
  element and lastly in global Cartesian coordinates.
* Samplers will no longer be automatically attached (with :code:`sample, all;`) to dipoles with finite
  pole face rotations, as this may lead to tracking issues in Geant4. A developer option can force
  this on, although this is not recommended. Issue #241.
* `hkicker` and `vkicker` strength can be specified via the magnetic field :code:`B` instead of
  `hkick` or `vkick`.
* Support for dipole poleface curvature in tracking.
* Pole face rotations and fringe fields are now available for hkickers and vkickers, both thick and thin.
* New ability to specify the colour of any magnet and most elements through custom colour definition.
* Geant4's DNA physics lists have been added.
* Solenoid fringe fields have been implemented and are on by default. They are controlled with
  the `includeFringeFields` option.
  
General
-------

* :code:`outerDiameter` is now :code:`horizontalWidth` to better describe its function
  (backwards-compatible). This naming was from a time when BDSIM could only create cylindrical
  magnets and beam pipes. Given it can now create more complicated geometry, this name is not
  a good choice and so has been renamed. BDSIM still supports the old syntax.
* :code:`vhratio` is now consistent with vkickers and refers to the vertical and horizontal ratio in
  the lab frame.
* The horizontal width of kickers is now taken from :code:`outerDiameter`. Previously, :code:`outerDiameter`
  corresponded to the height and :code:`vhratio` was really the horizontal-to-vertical ratio in
  the lab frame.
* Synchrotron radiation is now disabled with em_extra physics list (use dedicated
  synchrad physics list). Avoids the double registration of the physics process.
* New CMake variable ROOTSYS to allow easy specification of a specific ROOT installation.
* Visualisation of trajectories significantly faster (~10x) due to different strategy with Geant4
  visualisation system.
* "ang" unit is added to the parser for Angstroms.
* BDSIM will now exit if there is no space to make the necessary circular management objects
  (teleporter and terminator).
* long int used explicitly instead of int for event indices in analysis.
* Reimplemented primary first hit and last hit. Last hit is now the end point of the
  primary trajectory. No more linear memory usage with tracking time.
* Beam pipe extent calculation re-implemented and much less simplistic - used
  to check whether a pipe will fit inside a magnet.
* Mini-contents for syntax section of manual, as it's grown to a large size.
* New rmatrix element (experimental).
* EM Dissociation is now applicable up to 100 TeV.
* Significantly improved aperture shape checking for whether beam pipe will fit inside a magnet.
* BDSIM now recognises all elements by chemical abbreviation. These are found in the Geant4 NIST
  database by automatically prefixing the name with "G4\_". Issue #236.
* `circle` distribution z and t distributions are now reversed to be consistent with all other generators.
  These were T0 - dt and are now T0 + dt. Distribution will be different for the same seed as compared
  to a previous version of BDSIM.
* `square` distribution now calls random number generator for each coordinate every time for
  consistency. Distribution will be different for the same seed as compared
  to a previous version of BDSIM.
* Memory usage for sampler hits has been significantly reduced with no affect to the output
  information stored.
* The "water" material in BDSIM is now the NIST G4_WATER material and no longer the one
  that was defined by BDSIM.
* New options for physics processes in em_extra.

Output Changes
--------------

* Data v3 incremented from v2.
* Options class (GMAD::optionsBase) number is incremented in output.
* New optional stepLength variable in Eloss part of Event Tree with option
  :code:`storeElossStepLength` to use this data.
* New optional preStepKineticEnergy in Eloss part of Event Tree with option
  :code:`storeElossPreStepKineticEnergy` to use this data.
* Energy Loss class (BDSOutputROOTEventLoss) number is increment in output.
* Tilt, offsetX, offsetY and material are added to the Model Tree output.
* Model class (BDSOutputROOTEventModel) number is incremented in output.
* Model information extended to provide everything necessary to make machine diagrams.
* New option :code:`storeModel` to turn off model storage in the output.
* Even Info class (BDSOutputROOTEventInfo) number is incremented in output.
* Event.Info now has a Boolean of whether the primary particle hit something or not.
* Samplers are **no longer** placed next to elements with angled faces when using the :code:`bdsimtwo`,
  :code:`geant4`, or :code:`geant4dp` integrator sets.
* Units are now written to the ASCII survey output for each column.
* New output class :code:`BDSOutputROOTEventCoords` to store coordinates for primary global coordinates.
* New branch called "PrimaryGlobal" in Event tree that stores the coordinates used with Geant4 in
  the global Cartesian frame.
* Sampler name now stored in Orbit output from rebdsimOrbit to make sampler matching possible.

Bug Fixes
---------

* Fixed solenoid tracking. The anti-spiralling code in the dipole integrator that is designed
  to stop infinite spiralling of low energy particles in strong fields was causing incorrect
  tracking in solenoids. This has been fixed with the re-implementation of the solenoid matrix
  and now includes the fringe effects. Issue #255.
* Fixed tracking bug where particle in very niche coordinates may reflect from a sampler
  at the end of a dipole with a very strongly angled pole face. #Issue 241.
* Fixed automatic tunnel building algorithm, which accumulated wrong variables, leading to
  problems when thin elements such as fringe fields or thin multipoles were included.
* Further improvements made to tunnel building algorithm for magnets with tilt. Issue #243.
* Fixed length check for very short elements. Small drifts would cause a crash from
  Geant4 due to invalid parameters - occurred as length check was wrong.
* Fixed non-positive definite warnings for no energy spread and time spread when using
  a Gaussian beam in a composite beam definition.
* Fixed Gauss beams used in composite distribution.
* Fixed the problem where no particles were being tracked when using a userfile bunch distribution with only one column.
* Fixed bug where last particle was missed from user bunch distribution file.
* Fixed corrupted example files for userfile bunch distribution. Issue #244.
* Fixed cutting planes in G4CutTubs constructor for tunnel in Geant up to Geant4.10.2.p02
  from too short tunnel section.
* Reimplemented the method of finding primary first and last hit so BDSIM doesn't need to
  save the whole trajectory for the primary. This fixes the behaviour of linearly growing
  unbounded memory usage when tracking for a long time in a ring. Issue #246, #242.
* Optical calculation now works for sub-relativistic positrons.
* ATF2 MAD-X output was not included in worked example as advertised - now included.
* Fixed scaling variable used when scaling a field map to a decapole magnet strength.
* Survey units for magnetic fields are now fixed from kT to T.
* Fixed issue where C-shaped vkickers and hkickers would ignore :code:`yokeOnInside`. Issue #251.
* Fixed possible overlap in vkicker, hkicker, and h-style dipole geometry with highly asymmetric
  beam pipes.
* Fixed incorrect report that beam pipe wouldn't fit in magnet for various aperture shapes. Issue #253.
* Fixed issue where the option :code:`writePrimaries = 0` would result in the hits for the first sampler
  being written to the primary sampler structure. Issue #245.
* Fixed lack of interaction with vacuum when processes biased - due to a specific Geant4 version.
  Issue #220.
* Fixed incorrect dipole scaling. Issue #239.
* Fixed E0 spread in `ring` beam distribution, which was finite in contrast to the description
  that it is always the central value.
* Fixed reproducibility for the `ring` distribution that didn't use the same random number generator
  as every other distribution. Coordinates will be different for this distribution for the same seed now.
* Fixed inconsistency of `t` and `z` coordinate in `square` beam distribution.
* `square` beam distribution now varies with :code:`envelopeT`.
* Fixed S coordinate in output. Issues #247 and #248.
* Fixed the setting of the sampler diameter where the user specifies a smaller one than that calcualted
  from the minimum bending radius.

Utilities
---------

* pybdsim v1.9.0
* pymadx v1.5.0
* pymad8 v1.4.1
* pytransport v1.2.1
  

V1.1 - 2018 / 05 / 23
=====================

New Features
------------

* New visualiser command "/bds/beamline/goto name" to move visualiser to view a particular element.
* Check explicitly on incompatible physics lists that may cause a crash when used together.

General
-------

* Global coordinates are now always stored for primary first hit.
  
Bug Fixes
---------

* Fixed wrong transforms for finite `S0` in composite beam distribution.
* Fixed crash when finite `S0` was used with `-\\-generatePrimariesOnly` executable option.
* Fixed units from mm to m for PrimaryFirstHit and PrimaryLastHit for
  `x`, `y`, `z`, `X`, `Y`, `Z` positions.
* Fixed segfault for double deletion when 'qgsp_bic' and 'qgsp_bert' were attempted to be used together.

Utilities
---------

* pybdsim v1.6
* pymadx v1.2
* pymad8 v1.1
* pytransport v1.0


V1.0 - 2018 / 04 / 10
=====================

Highlights
----------

* Full support for dipole fringe fields and pole faces in tracking.
* Full low energy (sub-relativistic) tracking.
* Validation against PTC for sub-relativistic to high energy.

New Features
------------

* Support for Geant4.10.4 - however, this version is unusable as G4ExtrudedSolid is
  broken and used in BDSIM. We recommend Geant4.10.4.p01.
* H-style dipoles controllable by default or per element with `hStyle` option.
* Control over dipole proportions with global and per element options `vhRatio`,
  `coilWidthFraction` and `coilHeightFraction`.
* Support for extra Geant4 physics lists: `G4ChargeExchangePhysics`, `G4HadronDElasticPhysics`,
  `G4HadronElasticPhysicsHP`, `G4HadronElasticPhysicsLEND`, `G4HadronElasticPhysicsXS`,
  `G4HadronHElasticPhysics`, `G4HadronPhysicsShielding`, `G4HadronPhysicsShieldingLEND`,
  `G4IonElasticPhysics`, `G4IonQMDPhysics`, `G4RadioactiveDecayPhysics`, `G4StoppingPhysics`,
  `G4HadronElasticPhysicsPHP`, `G4MuonicAtomDecayPhysics`, `G4NeutronTrackingCut`.
* New options `neutronTrackingTime`, `neutronKineticEnerygLimit` and `useLENDGammaNuclear`
  (Geant4.10.4 onward) physics options.
* Support for new numerical integrator tracking algorithms in Geant 4.10.3 and 4.10.4.
* New integrator set "geant4dp" for Dormand Prince integrators (Geant 4.10.4 or higher required).
* Significantly improved analysis documentation.
* New component: 'gap'. Creates a space in the beam line with no geometry.
* Ability to specify the world volume material with the `worldMaterial` option.
* Introduced `minimumRange` and `minimumKineticEnergy` user limits as provided by G4UserLimits.
* Ability to limit step size in user-specified fields.
* Ability to control turn number print out with `printFractionTurns`.
* Magnet yokes now have a general multipolar field.
* Sampler diameter is automatically reduced when high angle bends are used to prevent overlaps.
* New CMake option to disable event display for installation with ROOT EVE problems.
* Ability to combine rebdsim output files with correct statistical analysis, allowing high throughput
  analysis with scaling. New tool `rebdsimCombine` for this purpose.
* Parameter tests for all bunch distributions - BDSIM will exit with message if parameters are invalid.
* `scaling` parameter for each element allowing simple scaling of strengths.
* New program to convert PTC output in a TFS file to BDSIM output format (`ptc2bdsim`).

Output Changes
--------------

* The ROOT class definition has been incremented to "2" from "1".
* Output files now have header structure with software versions.
* Output files now have 'geant4 data' that includes particle masses used in the simulation.
* "t" is now "T" in samplers and trajectory output to be consistent with naming convention
  of global and local coordinates.
* Samplers now have optional charge, mass, rigidity and ion A and Z
  (see :ref:`bdsim-options-output` for details).
* Classes in library can calculate on-the-fly for user analysis.
* Trajectory momentum is now in GeV.

Analysis Changes
----------------

* "**librebdsimlib**" has been changed to "**librebdsim**" to be more consistent for output loading.
* Support for logarithmic binning of histograms in rebdsim.
* "HistogramND" in rebdsim now creates **per-entry** histograms on the tree. This introduces the
  ability to create per-event histograms in analysis that were not previously possible. Older
  style histograms that are a sum across all events are now made with "SimpleHistogramND".
* New option in rebdsim to turn off histogram merging (for speed).
* Analysis classes have member names changed to match those in the output files, i.e. "eloss" is
  now "Eloss" in `bdsim/analysis/Event.hh`.
* Rebdsim is now tolerant of a missing '.' on the end of the Tree name (a ROOT subtlety).
* 'orbit' and 'optics' are now 'Orbit' and 'Optics' in the output.
* New executable option for `rebdsimOptics` :code:`--emittanceOnFly` to calculate emittance
  at each sampler.

General
-------

* Physics list names are now consistently named with '_' between words. Old list
  names are still supported.
* `hadronic` and `hadronic_hp` physics lists have switched from `qgsp_bert` and
  `qgsp_bert_hp` to `ftfp_bert` and `ftfp_bert_hp` respectively, as these are
  recommended by Geant4 for high energy hadronic interactions.
* "bdsim" integrator set now maps to "bdsimtwo" integrator set.
* All objects in the parser can now be extended later rather than just elements.
* Tuned colours of hkicker and vkicker.
* Relative file paths are no longer padded excessively with slashes when translated
  to absolute paths.
* More efficient file IO in rebdsim improves analysis speed.
* The world material is now air instead of G4_Galactic.
* `printModuloFraction` is now `printFractionEvents`.
* `includeIronMagFields` option is now `yokeFields`.
* Vacuum volumes now visible in `-\\-vis_debug` mode.
* Only forward moving particles are now considered for the optical function calculation.
* Updated examples for ATF2, LHC. New Diamond machine example.

Bug Fixes
---------

* Fixed magnetic field strength for AWAKE dipole using pure dipole field.
* User limits are now applied to external geometry.
* Fixed bug where some visualisation settings wouldn't be applied to all logical
  volumes in external geometry.
* Fixed bug where some file paths may not be translated to absolute paths correctly.
* Fixed a bug where recreate mode would fail with the new Beam structure in the output.
* Prevent segfault when analysing wrong type of file with rebdsim (use 'backwardsCompatible'
  option to analyse old files without the new header structure).
* The `stopTracks` option has been removed as it did not function as intended. Use `stopSecondaries`.
* `thresholdCutCharged` and `thresholdCutPhotons` have been removed as they did not function as intended.
* Fixed bug where the world size would be too small with placements of large externally provided
  geometry with strong rotations.
* When no energy was specified in the `userfile` bunch distribution, the central beam energy was
  1000x too low - fixed - missing factory of GeV.
* Number of turns simulated is now number desired and not number+1.
* Limits to control maximum step length and tracking time were not attached to magnet yoke geometry.
* Fixed crash when using octagonal or racetrack apertures in a magnet with pole geometry.
* Fixed issue where ~10% of particles may appear offset in samplers in large ring models.
* Fixed some very small overlaps not reported by Geant4 in magnet yoke geometry.
* Fixed issue where drift in magnet was always 1pm shorter than expected.
* Fringe fields are no longer built in between two dipoles against each other.
* Fixed Gauss Twiss bunch generator for finite dispersion in `x`, `xp`, `y`, `yp`.
* Fixed bug where overlapping dipole end pieces would be produced.
* Fixed GDML preprocessing for parameterised variables.
* Tracking limits are now attached to magnet yokes.
* Fixed central value of `T0` not being set for `circle`, `gauss`,
  `gausstwiss`, `gaussmatrix`, `halo` and `square`
  distributions.

Utilities
---------

* pybdsim v1.4
* pymadx v1.1
* pymad8 v1.0
* pytransport v1.0

V0.993 - 2017 / 12 / 11
=======================

New Features
------------

* Executable option `-\\-writeseedstate` is now `-\\-writeSeedState`.
* Executable option `-\\-exportgeometryto` is now `-\\-exportGeometryTo`.
* Executable option `-\\-distrfile` is now `-\\-distrFile`.
* Redefined simpler syntax for halo distribution. Please see manual for new parameters.
* Support for all EM physics lists included with Geant4.10.3.p03.
* Support for an ion as the primary beam particle.
* Support for ion physics lists.
* Ability to load two GDML files with degenerate object names correctly compensating for
  deficiency in Geant4 GDML parser.
* Local copy of GDML schema included. No longer require internet access to load GDML files. Custom
  local schema still supported.
* Support for Geant4.10.4 - however, we don't recommend using this until p01 is used, as there
  is a fault with G4ExtrudedSolid that is used for all poled magnet geometry.

Bug Fixes
---------

* Compilation fixes for compilers with XCode 9.
* Fixed possible compiler-dependent tracking bug where particle would get stuck in dipole.
* Cherenkov radiation in water fixed with specification of optical refractive index.
* Fixed ATF2 example input syntax and updated model.
* Removed temporary files created in current working directory.

Output Changes
--------------

* Options split into options and beam - beam contains all information related to beam definition.
* Associated output classes and analysis classes added for Beam in output.
* Removed older output format.

General
-------

* Updated automatic Geant4 from AFS to version 10.3.p01 (latest available).
* Updated automatic ROOT from AFS to version 6.06.08.
* Remove support for ROOT v5 and require v6 onward.

Utilities
---------

* Python utilities now use a setup.py compatible with PIP.
* pybdsim v1.3
* pymadx v1.0
* pymad8 v0.8
* pytransport v0.1 (new)


V0.992 - 2017 / 07 / 26
=======================

New Features
------------

* Preparsing for GDML for pygeometry generated geometry - overcomes Geant4's GDML parser
  deficiency of only allowing one GDML file to be loaded in the whole program.
* Visualisation of multiple beam lines.
* Option to use first sampler emittance as assumed emittance throughout lattice in optical
  function calculation.
* Additional materials for LHC tunnel and geometry.

Bug Fixes
---------

* Fixed uncaught exception in analysis DataLoader class construction depending on optional arguments.
* BDSIM installation no longer dependent on build files - ROOT analysis dictionaries would use
  build headers. Issues #197 and #199.
* Fixed magnitude of B-field in rfcavity that resulted in extraordinarily strong B-fields.
* Fixed rf E- and pill-box fields ignoring phase and being a factor of 2:math:`\pi` too low in frequency.
* Fixed for crash when particle was at exactly 0 in a quadrupole.
* Fixed compiler warnings for Clang 8.1.
* Fixed all variable shadowing throughout codebase and reintroduced compiler warning if present.
* Fixed field transform for tilted and offset magnets. This fixes incorrect tracking for tilted
  magnets when using the "bdsimtwo" integrator set for dipoles.

General
-------

* Separated field and geometry in rf cavity element.
* Revised implementation of rf cavity construction for greater flexibility.
* RF cavity phase now automatically calculated based on location in lattice.
* Removal of old ROOT analysis scripts for very old data format.
* Revised construction to allow construction of multiple beam lines dynamically.

Utilities
---------

* pymadx v0.9
* pybdsim v1.2
* pymad8 v0.7


V0.991 - 2017 / 07 / 04
=======================

Bug Fixes
---------

* Fixed simple histogram weighting with Boolean expressions in rebdsim.
* Fixed comparator always failing.
* Fixed loading default event information in analysis.
* Fixed executing BDSIM from outside the directory containing the main gmad file.
* Fixed charge dependence of thin multipole and dipole fringe integrators.
* Scaled dipole fringe and thin multipole by momentum of particle.
* Fixed for loading geometry files prefixed with superfluous './'.
* Fixed for duplicate run beam on icon in visualiser for Geant4.10.3 onward.


General
-------

* Separated Doxygen cmake into own macro.

Utilities
---------

* pymadx v0.9
* pybdsim v1.1
* pymad8 v0.7


V0.99 - 2017 / 06 / 09
======================

New Features
------------

 * Parser will expand values from structures when printing. Issue #167.
 * Optical physics example and test. Issue #156.
 * Improved parser error messages. Issue #170.
 * Support for compressed input coordinate files for beam distributions using tar and gz.
 * Switch entirely to CMake labels instead of naming convention for tests.
 * AWAKE experiment code refactored into module.
 * New *shield* element that is a drift surrounded by rectangle of material.
 * New *placement* element that allows an object with geometry to be placed independent of the beam line.
 * maximumTrackLength option to limit any track in the geometry.
 * Ability to offset beam line w.r.t. world coordinates at start.
 * Check for required Geant4 environment variables.
 * Thin horizontal and vertical (and combined) kickers.
 * Thin multipoles.
 * Compatibility with Geant4.10.3.

Fields & Integrators
^^^^^^^^^^^^^^^^^^^^
 * Complete refactorisation of field classes and construction.
 * Centralised construction of fields.
 * 1-4D BDSIM format field map loading.
 * 2D Poisson SuperFish SF7 format field map loading.
 * 1-4D nearest neighbour, linear and cubic interpolators for field maps.
 * Support for compressed field maps using tar and gz.
 * Ability to choose integrator sets for all elements via parser.
 * Removal of all individual magnet classes - centralised construction in BDSMagnet.
 * New executable - "bdsinterpolator" - allows loaded and interpolated field to be queried and written out.
 * Rewritten dipole integrator using Geant4's helical stepper.
 * All integrators tested for low energy spiralling particles.
 * Introduction of visualisation commands.

Geometry
^^^^^^^^

 * Rewritten external geometry loading.
 * Ability to overlay externally provided geometry on magnets (except sbend).
 * Automatically generated tight-fitting containers for externally loaded GDML geometry.
 * *circularvacuum* beam pipe geometry that allows no geometry for the beam pipe; only vacuum.
 * Magnet colours tweaked slightly - pybdsim now matches BDSIM colour-wise.
 * Additional curvilinear bridge world to ensure continuous curvilinear coordinates.


Output & Analysis
^^^^^^^^^^^^^^^^^

 * Protection against invalid sampler names that would cause ROOT branching errors.
 * 1x 3D histogram in default output that can be placed along the beam line.
 * Support for 3D histograms in rebdsim.
 * All magnet strength components written out to survey.
 * Change of syntax in rebdsim analysis file to specify dimensions of histogram.
 * Stricter parsing of analysisConfig.txt for syntax checking.
 * New executable rebdsimOrbit to extract single orbit from sampler data.


Bug Fixes
---------

 * ASCII seed state can be loaded properly. Issue #163.
 * rfcavity can be created without a cavitymodel instance. Issue #165.
 * Memory leak in comparator event tree comparison fixed. Issue #169.
 * Zero angle bend with finite field can be created. Issue #176.
 * Samplers are compared properly in comparator. Issue #177.
 * Sampler names in Model tree now match those exactly in the Event tree.
 * Missing virtual keyword from destructors fixed through to fix leaks at the end of the program.
 * GFlash parameterisation is only loaded if specified in physics list.
 * Fixed geometry construction errors that may occur due to dynamic tolerances for physically large models.
 * Fixed infinite loop events if the primary vertex starts outside the world volume.
 * Regions and biases set correctly to components in BDSLine class.
 * Circle distribution did not have central value offsets.
 * Fix double registration of pion decay as well as some others for muons when using muon physics list.
 * Particles from physics list are now constructed correctly allowing more
   particles to be used in the beam definition.
 * Removal of Cherenkov radiation from muon physics significantly reducing simulation time.
 * Fix double registration of pion decay with muon physics list.
 * Issue #134 - samplers cause tracking warning.
 * Long running events due to spiralling particles. Issues #178, #132, #187.

General
-------

 * ``Sampler_`` prefix was removed from all samplers in rootevent output.
 * Sampler thickness reduced from 40 nm to 10 pm.
 * Removal of unnecessary step length limit to half the length of an element.
 * Revised region construction allowing arbitrary number to be constructed.
 * Revised bend construction with reduced volume count in some cases.

Utilities
---------

* pymadx v0.8
* pybdsim v1.0
* pymad8 v0.7

V0.95 - 2016 / 11 / 07
======================

New Features
------------

* Comparator program introduced for statistical comparison against reference results.
* rebdsim analysis examples and tests added.
* ROOT examples and tests added for analysis code usage in ROOT.
* Discrete optics only program (rebdsimOptics) added.
* Update CLHEP in AFS build to 2.3.1.0 for apple and 2.3.3.0 for RHL6.
* Reduced compilation time.

Bug Fixes
---------

* Fixed geometry tolerance issue that would cause Geant4 run-time errors for
  regular geometry for some particularly large spatial size models.
* Fixed for linker error with ZLIB and gzstream. Issues #9, #155, #158.
* Fixed NaN errors in certain circumstances when calculating optical functions.
* Fixed shadowing compilation warnings.
* Fixed geometry overlaps in rf cavity geometry. Issue #136.
* Coverity fixes for uninitialised variables / small memory leaks. Issues #152, #156.
* Fixed potential magnet geometry errors when creating very thin components.
* Fixed negative interaction warnings due to biasing. Issue #141.

General
-------

* Deprecate BDSIM Plank scattering, laserwire calorimeter and Bremsstrahlung
  lead particle biasing.

Utilities
---------
* pymadx v0.7
* pybdsim v0.9
* pymad8 v0.6
* robdsim v0.7

V0.94 - 2016 / 09 / 13
======================

New Features
------------

Analysis
^^^^^^^^

* Analysis class has been refactored to allow analysis on any tree in the BDSIM
  ROOT event output format using rebdsim.

Geometry
^^^^^^^^

* Tilted dipoles are now supported.
* The Read-Out geometry has been moved to the Geant4 parallel world scheme.
* The parallel sensitive geometry for tunnel hits has been deprecated and the
  functionality now provided by the new parallel read-out geometry.
* The read-out geometry construction has moved out of BDSAcceleratorComponent
  to its own geometry factory.
* Beam pipes are now constructed with arbitrary 3-vector surface normals, rather
  than angled faces described by an angle only in the x-z plane.
* The side of the yoke of a dipole with poled geometry can now be controlled with
  the :code:`yokeOnLeft` option.
* New interfaces to the auxiliary navigator have been written that use the mid-point
  of a step to much more robustly locate the required volume in the parallel
  curvilinear geometry.
* Overlap checking between adjacent dipoles with pole face rotations (and even tilts)
  prevents overlaps in geometry.

Output
^^^^^^

* Tunnel hits are now of the same type as general energy loss hits.
* The track ID can now be optionally written out to energy deposition hits.

Parser
^^^^^^

* The option :code:`modularPhysicsListsOn` has been deprecated.
* New per-element parameter :code:`yokeOnLeft`.

Physics
^^^^^^^

* The modular physics lists are now compulsory and the old physics construction has
  been deprecated.
* The existing BDSIM laserwire Compton scattering process construction was moved to
  the modular physics list scheme.


Bug Fixes
---------

Geometry
^^^^^^^^

* Extent inheritance with BDSGeoemetryComponent was fixed resolving rare overlaps.
* Poled geometry variable clean up fixed to ensure components from factories aren't
  related to each other - could cause rare crash on exit.
* Fixed extents and possible overlaps in tunnel 'rectaboveground' geometry.

Output
^^^^^^

* Energy deposition coordinates are randomly chosen along the step of the deposition. The
  x,y,z coordinates now match the random point, whereas they were the post step point previously.

Physics
^^^^^^^

* Fixed an issue that would cause infinite loops with strong process biasing.

General
^^^^^^^

* Initialisation of variables fixed throughout.
* Significantly improved compilation speed.
* Removed executable permission on all source files.

Utilities
---------
* pymadx v0.6
* pybdsim v0.8
* pymad8 v0.5
* robdsim v0.7


V0.93 - 2016 / 08 / 24
======================

New Features
------------

Analysis
^^^^^^^^

* New analysis tool 'rebdsim' replaces robdsim.
* Analysis directory with event, model, event info and options analysis.
* Histogram merging with correct statistical uncertainties.
* Deprecated root utilities to :code:`analysis/old/`.
* Rewritten optical function calculation with validated calculation.

Build
^^^^^

* Require CMake 2.8.12 or higher.
* Require Geant4 compiled with external CLHEP - ensures strong reproducibility.
* Start of bootstrapping scripts in :code:`depend/`.
* Factorisation of BDSIM's cmake package finding into :code:`cmake/`.


Geometry
^^^^^^^^

* Coil geometry introduced to generic library magnets.
* Overlap checking between magnets with pole face rotations.
* Collimator colour can now be controlled.
* End pieces for coils also introduced
* Default poled dipole geometry is now a C-shaped magnet with yoke on inside of bend.

Output
^^^^^^

* Switched to rootevent as default and **recommended** format.
* Include full set of options used in simulation in output.
* Include software version in output.
* Store seed state per event.
* Store histograms per event.
* Run and event durations stored in output.
* Output written in event of a crash.
* Refactor of trajectory information.
* Write out primary trajectory points.

Parser
^^^^^^

* Factorised options into optionsBase that is simple structure for easy saving.

Physics
^^^^^^^

* "Modular physics" list is now the default.
* Use geant4 helper class for physics lists construction to ensure correct order.
* Ability to provide a default bias to all types of volumes (vacuum, accelerator, all).
* Attribute energy deposition (uniformly) randomly along the step where it occurred for more accurate energy deposition - currently only s, not x,y,z - they represent before, after.
* 'solid' air materials for cross-section validation.
* Seed states are saved and restored in the primary generator action rather than event action.

Tracking
^^^^^^^^

* Geant4 Runge-Kutta stepper for quadrupole and sextupole for increased robustness.


General
^^^^^^^

* Strong recreation for an event by setting seed state issue (#118, #139).
* A BDSAcceleratorComponent can own an associated end piece(s) (before and after).
* A BDSAcceleratorComponent can have a input and output angled face.
* Halo bunch distribution developed significantly.
* Revised executable options for recreation / using a seed state.
* Signal handling improved.
* Templated user bunch file - can now use gzip compressed files.
* Improved default options for more realistic geometry.


Bug Fixes
---------

Geometry
^^^^^^^^

* Fixed loading for multiple GDML files having conflicting 'world' volumes.
* Reimplementation of pole geometry fixes gaps in poles (issue #110).

Parser
^^^^^^

* Fixed issue of parser python interface (issue #133).

Physics
^^^^^^^

* Modular physics lists are truly modular - fixes segfaults (issue #130).

Tracking
^^^^^^^^

* Fixes for cavity field values (issue #124).
* Fixed field value transform in sextupoles and above for global / local coordinates.
* Auxiliary navigator used more routinely for transforms with optional caching.

General
^^^^^^^

* Issues #115, #127, #129, #131


Utilities
---------
* pymadx v0.6
* pybdsim v0.7
* pymad8 v0.41
* robdsim v0.7


V0.92 - 2016 / 03 / 29
======================

New Features
------------

* Samplers are attached at the *exit* instead of the *entrance* of an element.
* Poleface rotations for bends are implemented (issue #100).
* Geant4 9.6.x versions support has been dropped (issue #111).
* DUMP element removed (issue #116).

Geometry
^^^^^^^^

* Samplers are no longer placed in the physical world but in a parallel sampler world.
* Above ground 'tunnel' geometry implemented.
* Introduced new RF cavity geometry and fields (still in development).

Output
^^^^^^

* ROOT version 6 support (issue #114).
* Option to fill ROOT with double or float precision.

Parser
^^^^^^

* Support for string variable (issue #126).

Physics
^^^^^^^

* Modular physics lists are default (issue #121).
* Use Geant4 provided synchrotron radiation instead of BDSIM one.

Bug fixes
---------

Geometry
^^^^^^^^

* Tunnel geometry fixes (issues #88 and #89).

Parser
^^^^^^

* Fixed fast list insertion (issue #113).
* Support for tildes in path names (issue #119).

Physics
^^^^^^^

* Old physics code cleanup (issue #123).
* Physics biasing properly initialised (issue #84).

General
^^^^^^^

* PDF Manual builds on Ubuntu (issue #85).
* 1D Histogram class significantly faster for uneven bin width histograms.

Utilities
---------
* pymadx v0.4
* pybdsim v0.5
* pymad8 v0.3
* robdsim v0.5

V0.91 - 2015 / 12 / 17
======================

New Features
------------

* New tests for file IO, coordinate transforms, aperture models, extra optical lattice patterns and general ring examples.

Geometry
^^^^^^^^

* Race track and octagonal aperture models introduced.
* New wedged energy degrader component introduced.

Output
^^^^^^

* Optional reduced number of variables in ROOT output - formats now "root" and "rootdetailed" (issue #107)
* Forced dependency on ROOT

Parser
^^^^^^

* Parser warns for redefined variable, and exits for usage of undeclared variables (issue #98)
* Parser reorganised to C++ class structure (issue #77)
* Command line options more flexible (issue #105)
* Ability to define Geant4 regions as objects in parser.
* Can attach samplers to all elements of one type (i.e. collimators).

Physics
^^^^^^^

* Can attach biasing to any part of any element from parser.

Tracking
^^^^^^^^

* Ability to start bunch from any S-position along accelerator, rather than just at beginning.

Bug fixes
---------

Geometry
^^^^^^^^

* Fixed for LHC detailed geometry when beam shield is rotated.
* Consolidation and improvement of aperture parameter validity testing.
* Fixed for femtometre occasional overlaps in magnet outer geometry.
* Fixed placement overlaps in rbend.
* Fixed segfault with RfCavity at end of run.
* Fixed crashes with zero angle sector bends.

Parser
^^^^^^

* Multiple command line arguments without space will now be recognised and highlighted.

Physics
^^^^^^^

* Made required version of Geant4 consistent across biasing code.

Tracking
^^^^^^^^

* Fields only constructed if non-zero strength used - avoids tracking errors for zero strength components.
* Fixed several issues with vertical and horizontal kicker construction and tracking.
* Broken external magnet fields disabled by default.
* Circular turn counting bugs fixed.
* Particles no longer killed with circular flag on if starting slightly behind starting midpoint.
* Particles no longer stepped by teleporter at beginning of 1st turn if starting behind starting midpoint.
* Fixed teleporter tracking for backwards travelling particles that would get stuck in a loop.

General
^^^^^^^

* Add CMake protection against Geant4 built with multithreading on (issue #103).

Utilities
---------
* pymadx v0.3
* pybdsim v0.4
* pymad8 v0.2
* robdsim v0.4

V0.9 - 2015 / 11 / 10
=====================

New Features
------------

* Physics biasing with ability to change cross-section for individual particles
  and processes, as well as attach to a variety of objects
* Decapole magnet
* Robdsim analysis package as separate executable for testing
* Tracking tester
* Improved C++11 use and iterator implementation across containers
* Can fill histogram with energy hit over a range covering several bins
* Introduced a separate auxiliary G4Navigator to avoid accidentally moving
  the particle during tracking when querying global-to-local transforms.
* Transform for curvilinear coordinates to global coordinates
  so primaries in those coordinates can be injected from anywhere (issue #63)
* Parser put in GMAD namespace
* New executable options for writing out geometry coordinates as built by BDSIM
* Magnets now have tightly fitting invisible container volumes, as opposed to
  large boxes before.
* Changed return type of magnet outer geometry factories to new BDSMagnetOuter
  class. This is because the container construction is now delegated to the
  magnet outer factory for tight-fitting container volumes.
* Extended examples and tests
* Move entirely to Geant4 visualisation manager supporting all available visualisers
  available with the local Geant4 installation

Bug fixes
---------

Geometry
^^^^^^^^

* Fixed bug where the read-out coordinates would also be offset by the offset
  of the element.
* Fixed overlaps in read out geometry.
* Reduced duplication in magnet outer factories.
* Fixed overlaps in rbend geometry (issue #64).
* Increased tolerance for sector bends (issue #73).
* Protected against zero angle sector bends (issue #74).
* Fixed overlaps in GDML geometry (issue #81).
* Geometry fixes (issues #76, 94, 95)

Physics
^^^^^^^

Parser
^^^^^^

* Occasional material parser segfault fixed (issue #25)
* Improved syntax checking and to not ignore unknown keywords (issue #71)
* Element extension fixed (issue #87)

Tracking
^^^^^^^^

* Dipole uses local coordinates and can bend in any direction (issue #78)

General
^^^^^^^

* Samplers can be attached to occurrence of a duplicated element (issue #47).
* Output survey updated and fixed (issue #60)
* Check for Geant4 environment variables (issue #62)
* Consistent policy for overwriting output files (issue #65)
* Improved memory and CPU for output writing (issue #86)

Utilities
---------
* pymadx v0.2
* pybdsim v0.3
* pymad8 v0.2
* robdsim v0.3

V0.8 - 2015 / 08 / 10
=====================

New Features
------------

* Tunnel geometry and flexible tunnel factories for different styles
* Tunnel read out geometry introduced for coordinates along tunnel axis
* C++11 adopted (required)
* `stopSecondaries` option
* Removed dependency on boost (issue #57)
* Restructured examples directory - top level contains only full machines
  and sub-directories contain features
* Example documentation in manual and in place beside each example with
  example screenshots
* Updated python utilities *pybdsim v0.1*, *pymadx v0.1*, *pymad8 v0.1* and *robdsim v0.2*
* Repeated components are not duplicated in memory - previously, they would
  be repeatedly constructed. Reduced memory footprint.
* Component information comes from Physical Volumes instead of Logical Volumes
* Improved manual documentation
* Improved Doxygen documentation
* Rubbish collection for all objects, rather than relying on only one run and
  Geant4 (partial) rubbish collection.
* String representation of enum types leading to more readable output
* Introduced ability to switch to new modular physics lists with flexible
  construction and addition of physics lists without hard-coded names for each
  combination - the user must turn this on explicitly

Bug fixes
---------

Geometry
^^^^^^^^
* Geometry overlaps (issues #55 and #58)
* Transform3d fix (issue #54)
* Fixed placement of objects outside x,z global plane - rotation bug, similarly
  for read-out geometry placement.
* Fixed broken circular control - bug was introduced in v0.7 - (issue #51).
* Strict checking of read out geometry construction to avoid invalid solids that
  would cause Geant4 to exit and BDSIM to crash.
* Strict checking on teleporter volume construction for circular machines that
  would cause Geant4 to exit and BDSIM to crash.
* Fixed calculation of length of sector bend magnet that would cause it to be
  slightly short - introduced in v0.7.
* Removed stored axes of rotation due to better implementation in BDSBeamline,
  avoiding duplication of information.
* Fixed issue of zero angle rbends causing a crash (issue #44).
* Event number print-out is now dynamic and based on the number of events to be
  generated. It is also controllable with the `printModuloFraction` option.
* Protected against bad user-specified values of `lengthSafety` to avoid
  geometry overlaps.
* Improved parser speed.

Physics
^^^^^^^
* SR radiation fixed in dipole (issue #53).
* Removed continuous synchrotron radiation, as it traps particles in low step-size
  infinite loop.
* Removal of poorly set deltaIntersection, chordStepMinimum and lengthSafety
  variables from examples - these should be left unset unless the user knows
  their purpose.

Output
^^^^^^
* Changed all transverse output units to **metres** - manual updated accordingly.
* Changed `z` in ASCII output to **global Z** instead of local z.
* Recorded energy in output is now unweighted, but energy recorded in convenient
  energy loss histogram is. Could have lead to double weighting previously.
* Fixed global coordinates being written out as local coordinates in ROOT
  output.
* Random number generator seed state not written out when no output is specified.

Parser
^^^^^^
* Return error if superfluous arguments are present (issue #56).
* Make parser more robust against duplicate element names (issue #43).
* Fixed warnings about compiling c as c++ being deprecated behaviour.

General
^^^^^^^
* Fixed wrong print out warning due to logic error (issue #51).
* Fixed boundary effects of energy deposition (issue #52).
* Fixed large memory leak for events with large number of particles - was due to
  accumulation of BDSTrajectory objects.


V0.702 2015 / 07 / 28 - Hotfix
==============================

* Fixes for physics production range cuts were not obeyed in simulation.

V0.701 2015 / 07 / 02 - Hotfix
==============================

* Fix for global X-coordinate not written to output for energy deposition.

V0.7 - 2015 / 06 / 30
=====================

New Features
------------

* Ability to write no output
* New magnet geometry factories introduced with seven possible magnet types.
* Introduction of --vis_debug flag to see container volumes without debug build.
* Revised magnet colours (same base colour, just prettier variant)
* New manual using sphinx documentation system
* Default visualiser provided - no requirement for a vis.mac by the user
* Nicer visualisation GUI by default
* Improved visualisation for GDML geometry
* Support for all Geant4 visualisers introduced (issue #11).

Bug fixes
---------

* Fixed overlapping volumes and tracking errors in beam pipes.
* Fixes for wrong transverse coordinates for geometry other than cylindrical magnets (issue #30).
* Histograms are now written to disk in case of crash or kill signal (issue #38).
* Fix for uncontrolled memory consumption for synchrotron radiation (issue #36).
* Fixed syntax error in parser on windows end of line character (issue #40).
* Follow user paths properly (issue #24).
* Parser can end on commented line (issue #41).
* Introduction of more flexible and weighted halo bunch distribution.
* Significant tidy of BDSAcceleratorComponent base class and derived classes.
* Fix LHC magnet geometry overlaps and improve efficiency as well as more flexible with different beam pipes.
* New BDSBeamline class used for component placement consistently in code.

V0.65 - 2015 / 04 / 10
======================

* New base class for any geometrical object BDSGeometryComponent
* New interchangeable beam pipes with six possible beam pipe shapes
* New sensitive detector manager to hold single instance of sd classes
* Introduction of G4Galactic material for 'empty' volumes rather than beam pipe vacuum
* Possibility to write to multiple output formats at once
* Extensive removal of unnecessary headers throughout
* Updated python utilities
* Fix for muon spoiler magnetic field (thanks to B. Pilicer)
* Fix for invisible cylinder of iron surrounding drifts previously


V0.64 - 2015 / 02 / 16
======================

* New histogram manager and factorisation of histograms from outputs
* Extra per-element histograms
* Basic implementation of valid solenoid

V0.63 - 2015 / 02 / 06
======================

* Large angle sbends split into multiple sbends, based on aperture error tolerance - currently 1mm.
* New geometry construction and placement for sbends and rbends - no overlapping volumes and
  simpler / increased performance
* Proper building under c++11 if available
* Introduction of composite bunch distribution
* Drop support for Geant4 versions 9.5 and older

V0.62 - 2014 / 08 / 07
======================

V0.61 - 2014 / 08 / 05
======================

* Geant4 version 10 support

v0.6 - 2013 / 12 / 02
=====================


v0.5 - 2008 / 11 / 08
=====================


v0.4 - 2008 / 02 / 26
=====================


v0.3 - 2007 / 01 / 26
=====================


v0.2 - 2006 / 05 / 18
=====================


v0.1 - 2006 / 02 / 22
=====================


beta - 2005 / 05 / 01
=====================
