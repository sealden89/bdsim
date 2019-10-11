.. macro for non breaking white space usefulf or units:
.. |nbsp| unicode:: 0xA0
   :trim:

.. _model-construction:

==================
Model Construction
==================

The following sections describe the basics of how to prepare a BDSIM model.

* :ref:`lattice-description`
* :ref:`circular-machines`
* :ref:`lattice-elements`
* :ref:`offsets-and-tilts`
* :ref:`lattice-sequence`
* :ref:`multiple-beamlines`
* :ref:`detectors`


.. _lattice-description:

Lattice Description
-------------------

A model of the accelerator is given to BDSIM via input text files in the :ref:`gmad-syntax`.
The overall program structure should follow:

1) Component definition (see :ref:`lattice-elements`)
2) Sequence definition using defined components (see :ref:`lattice-sequence`)
3) Which sequence to use (see :ref:`the-use-command`)
4) Where to record output (see :ref:`sampler-output`)
5) Options, including which physics lists, number to simulate etc. (see :ref:`bdsim-options`)
6) A beam definition (see :ref:`beam-parameters`)

* Specifing and option or definition again, will overwrite the previous value.
* The only **order specific** part is the **use** command (see :ref:`the-use-command`) as this
  copied whatever sequence is *used* at that point, so any further updates to the component
  definitions will not be observed.
* Apart from this, all other parts can be defined or redefined in any order in the input.
   
These are described in the following sections. Aside from these standard parameters, more
detail may be added to the model through customisation - see :ref:`model-customisation`.
  
.. _circular-machines:

Circular Machines
-----------------

To simulate circular machines, BDSIM should be executed with the :code:`--circular` executable option
(see :ref:`executable-options`), or with the :code:`option, circular=1;` in the input GMAD file. This
installs special beam line elements called the `teleporter` and `terminator` at the end of the lattice
that are described below.

.. note:: There must be a minimum :math:`0.2 \mu m` gap between the last element and the beginning
	  of the sequence to accommodate these elements. This has a minimal impact on tracking.

Both the terminator and teleporter are invisible and very thin elements that are not normally
shown in the visualiser. These can be visualised by executing BDSIM with the :code:`--vis_debug`
executable option.

The turn number is automatically stored in the energy loss output in the data when the
circular option is used.


Terminator
^^^^^^^^^^

In a Geant4 / BDSIM model, all particles are tracked down to zero energy or until they leave the world
volume. In the case of a circular accelerator, the particles may circulate indefinitely as they lose
no energy traversing the magnetic fields. To control this behaviour and limit the number of turns
taken in the circular machine, the terminator is inserted. This is a very thin disk that has
dynamic limits attached to it. It is normally transparent to all particles and composed of vacuum.
After the desired number of turns of the primary particle has elapsed, it switches to being
an infinite absorber. It achieves this by setting limits (G4UserLimits) with a maximum allowed energy
of 0eV.

The user should set the option `nturns` (default 1) (see :ref:`options-common`). ::

  option, nturns=56;


Teleporter
^^^^^^^^^^

Not all optical models close perfectly in Cartesian coordinates, i.e. the ends don't perfectly
align. Some small offsets may be tolerable, as most tracking codes use curvilinear coordinates.
To account for this, the teleporter is a small disk volume inserted to make up the space
and shift particles transversely as if the ends matched up perfectly. This is automatically
calculated and constructed when using the :code:`--circular` executable option.

Although the teleporter may not be required in a well-formed model that closes, the minimum
gap of :math:`0.2 \mu m` is required for the terminator.

.. _lattice-elements:

Beamline Elements
-----------------

BDSIM provides a variety of different elements each with their own function, geometry and
potential fields. Any element in BDSIM is described with the following pattern ::

  name: type, parameter=value, parameter="string";

.. note:: Note the colon ':' between name and type. The double (not single) inverted commas
	  for a string parameter and that each functional line must end with a semi-colon.
	  Spaces will be ignored.

The following elements may be defined

* `drift`_
* `rbend`_
* `sbend`_
* `quadrupole`_
* `sextupole`_
* `octupole`_
* `decapole`_
* `multipole`_
* `thinmultipole`_
* `vkicker`_
* `hkicker`_
* `kicker`_
* `tkicker`_
* `rf`_
* `rcol`_
* `jcol`_
* `ecol`_
* `degrader`_
* `muspoiler`_
* `shield`_
* `dump`_
* `solenoid`_
* `laser`_
* `gap`_
* `crystalcol`_
* `undulator`_
* `transform3d`_
* `element`_
* `marker`_
* `wirescanner`_

.. TODO add screen, awakescreen

These are detailed in the following sections.

Simple example, extend and copy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Example: ::

  d1: drift, l=5*m;

This defines a drift element with name `d1` and a length of 5 metres. The definition can later be changed or extended with ::

  d1: l=3*m, aper=0.1*m;

Note the omission of the type `drift`. This will change the length of `d1` to 3 metres and set the aperture size to 10 centimetres.

.. Warning:: This only works for beam line elements and not other objects in GMAD syntax (such as a placement).

An element can also be defined by copying an existing element ::

  d2: d1, l=2*m;

Element `d2` is a drift with the properties of `d1` and a length of 2 metres. Note that if `d1` is changed again, `d2` will **not** change.

Magnet Strength Polarity
^^^^^^^^^^^^^^^^^^^^^^^^

.. note:: BDSIM strictly follows the MAD-X definition of magnet strength parameter
	  `k` - a **positive** `k` corresponds to **horizontal focussing** for a
	  **positively** charged particle. This therefore indicates a positive `k`
	  corresponds to horizontal defocussing for a negatively charged particle.
	  However, MAD-X treats all particles as positively charged for tracking purposes.

.. warning:: BDSIM currently treats k absolutely, so to convert a MAD-X lattice for
	     negatively particles, the MAD-X k values must be multiplied by -1. The
	     pybdsim converter provides an option called `flipmagnets` for this
	     purpose. This may be revised in future releases depending on changes
	     to MAD-X.


Component Strength Scaling
^^^^^^^^^^^^^^^^^^^^^^^^^^

In the case of acceleration or energy degradation, the central energy of the beam may
change. However, BDSIM constructs all fields with respect to the rigidity calculated
from the particle species and the `energy` parameter in the beam definition (not `E0`,
but `energy`). To easily scale the strengths, every beam line element has the parameter
`scaling` that enables its strength to be directly scaled.

In the case of a dipole, this scales the field but not the angle (the field may be calculated
from the angle if none is specified). For example ::

  beam, particle="e-",
        energy=10*GeV;

  sb1: sbend, l=2.5*m, angle=0.1;
  d1: drift, l=1*m;
  cav1: rf, l=1*m, gradient=50, frequency=0;
  sb2: sbend, l=2.5*m, angle=0.1, scaling=1.005;

  l1: line=(sb1,d1,cav1,d1,sb2,d1);

In this example an rf cavity is used to accelerate the beam by 50 MeV (50 MeV / m for 1 m).
The particle passes through one bend, the cavity and then another. As the second bend is
scaled (by a factor of (10 GeV + 50 MeV) / 10 GeV) = 1.005) a particle starting at (0,0) with
perfect energy will appear at (0,0) after this lattice.

In the case of a quadrupole or other magnet, the scaling is internally applied to the `k1`
or appropriate parameter that is used along with the design rigidity to calculate a field
gradient.

An example is included in `examples/features/components/scaling.gmad`.

.. note:: The user should take care to use this linear scaling parameter wisely- particularly
	  in sub-relativistic regimes. The fields should typically be scaled with momentum and
	  not total energy of the particle.


drift
^^^^^

.. figure:: figures/drift.png
	    :width: 50%
	    :align: center

`drift` defines a straight beam pipe with no field.

================  ===================  ==========  =========
Parameter         Description          Default     Required
`l`               Length [m]           0           Yes
`vacuumMaterial`  The vacuum material  vacuum      No
                  to use, can be user
		  defined
================  ===================  ==========  =========

Notes:

* The :ref:`aperture-parameters` may also be specified.

Examples: ::

   l203b: drift, l=1*m;
   l204c: drift, l=3*cm, beampipeRadius=10*cm;

rbend
^^^^^

.. figure:: figures/rbend.png
	    :width: 40%
	    :align: center

.. |angleFieldComment| replace:: Either the total bending angle, `angle`, or the magnetic field, `B`, (in Tesla)
				 for the nominal beam energy can be specified.
				 If both are defined the magnet is under or over-powered.

`rbend` defines a rectangular bend magnet. |angleFieldComment|
The faces of the magnet are normal to the chord of the
input and output points. Can be specified using:

1) `angle` only - `B` calculated from the angle and the beam design rigidity.
2) `B` only - the angle is calculated from the beam design rigidity.
3) `angle` & `B`  - physically constructed using the angle, and field strength as `B`.

Pole face rotations can be applied to both the input and output faces of the
magnet, based upon the reference system shown in the figure below. A pure dipole
field is provided in the beam pipe and a more general dipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke. A
quadrupolar component can be specified using the `k1` parameter that is given by:

.. math::

   k_{1} = \frac{1}{B \rho}\,\frac{dB_{y}}{dx}\,[m^{-2}]

If `k1` is specified, the integrator from the `bdsimmatrix` integrator set is used. This
results in no physical pole face angle being constructed for tracking purposes. The
tracking still includes the pole face effects.

.. note:: See :ref:`bend-tracking-behaviour` for important notes about dipole tracking.

+-----------------+-----------------------------------+-----------+-----------------+
| Parameter       | Description                       | Default   | Required        |
+=================+===================================+===========+=================+
| `l`             | Length [m]                        | 0         | Yes             |
+-----------------+-----------------------------------+-----------+-----------------+
| `angle`         | Angle [rad]                       | 0         | Yes, and or `B` |
+-----------------+-----------------------------------+-----------+-----------------+
| `B`             | Magnetic field [T]                | 0         | Yes             |
+-----------------+-----------------------------------+-----------+-----------------+
| `e1`            | Input pole face angle [rad]       | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `e2`            | Output pole face angle [rad]      | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `material`      | Magnet outer material             | Iron      | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `yokeOnInside`  | Yoke on inside of bend            | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `hStyle`        | H style poled geometry            | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `k1`            | Quadrupole coefficient for        | 0         | No              |
|                 | function magnet                   |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fint`          | Fringe field integral for the     | 0         | No              |
|                 | entrance face of the rbend        |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintx`         | Fringe field integral for the     | -1        | No              |
|                 | exit face of the rbend. -1 means  |           |                 |
|                 | default to the same as fint. 0    |           |                 |
|                 | there will be no effect.          |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintK2`        | Second fringe field integral for  | 0         | No              |
|                 | the entrance face of the rbend    |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintxK2`       | Second fringe field integral for  | 0         | No              |
|                 | the exit face of the rbend        |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `hgap`          | The half gap of the poles for     | 0         | No              |
|                 | **fringe field purposes only**    |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h1`            | Input poleface curvature          | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h2`            | Output poleface curvature         | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `tilt`          | Tilt of magnet [rad]              | 0         | no              |
+-----------------+-----------------------------------+-----------+-----------------+

Notes:

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.

.. figure:: figures/poleface_notation_rbend.pdf
	    :width: 75%
	    :align: center

	    Pole face notation for an rbend.

A few points about rbends:

1) For large angles (> 100 mrad), particles may hit the aperture, as the beam pipe is
   represented by a straight (chord) section and even nominal energy particles
   may hit the aperture, depending on the degree of tracking accuracy specified. In this
   case, consider splitting the `rbend` into multiple ones.
2) The pole face rotation angle is limited to :math:`\pm \pi /4` radians.
3) If a non-zero pole face rotation angle is specified, the element preceding / succeeding
   the rotated magnet face must either be a drift or an rbend with opposite rotation (e.g. an rbend with
   :math:`e2 = 0.1` can be followed by an rbend with :math:`e1 = -0.1`). The preceding / succeeding
   element must be longer than the projected length from the rotation, given by
   :math:`2 \tan(\mathrm{eX})`.
4) Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the beginning
   or at the end of the rbend. The length of the fringe field element can be
   set by the option `thinElementLength` (see :ref:`bdsim-options`) but is an advanced option.
5) In the case of finite `fint` or `fintx` and `hgap`, a fringe field is used even
   if `e1` and `e2` have no angle.
6) The `fintK2` and `fintxK2` parameters are for a second fringe field correction term that are included to
   enable optics comparisons with TRANSPORT. Whilst this term is not available in MAD-X, the default values
   of 0 mean this second fringe field correction will not be applied unless `fintK2` or `fintxK2` are
   explicitly specified as non-zero.
7) The effect of pole face rotations and fringe field kicks can be turned off for all dipoles by setting
   the option `includeFringeFields=0` (see :ref:`bdsim-options`).
8) The poleface curvature does not construct the curved geometry. The effect is instead applied in the thin
   fringefield magnet.
9) rbends are limited in angle to less than :math:`\pi/2`.
10) A positive `tilt` angle corresponds to a clockwise rotation when looking along the beam direction as
    we use a right-handed coordinate system. A positive tilt angle of :math:`\pi/2` for an rbend with a
    positive bending angle will produce a vertical bend where the beam is bent downwards.

Examples: ::

   MRB20: rbend, l=3*m, angle=0.003;
   r1: rbend, l=5.43m, beampipeRadius=10*cm, B=2*Tesla;
   RB04: rbend, l=1.8*m, angle=0.05, e1=0.1, e2=-0.1

sbend
^^^^^

.. figure:: figures/sbend.png
	    :width: 40%
	    :align: center


`sbend` defines a sector bend magnet. |angleFieldComment|
The faces of the magnet are normal to the curvilinear coordinate
system. `sbend` magnets are made of a series of straight segments. Can be specified using:

1) `angle` only - `B` calculated from the angle and the beam design rigidity.
2) `B` only - the angle is calculated from the beam design rigidity.
3) `angle` & `B`  - physically constructed using the angle, and field strength as `B`.

Pole face rotations can be applied to both the input and output faces of the magnet,
based upon the reference system shown in the figure below. A pure dipole field is
provided in the beam pipe and a more general dipole (as described by
:ref:`yoke-multipole-field`) is provided for the yoke. A quadrupolar component can
be specified using the `k1` parameter that is given by:

.. math::

   k_{1} = \frac{1}{B \rho}\,\frac{dB_{y}}{dx}\,[m^{-2}]

If `k1` is specified, the integrator from `bdsimmatrix` integrator set is used. This
results in no physical pole face angle being constructed for tracking purposes. The
tracking still includes the pole face effects.

The `sbend` geometry is constructed as many small straight sections with angled faces. This
makes no effect on tracking, but allows a much higher variety of apertures and magnet
geometry to be used given the Geant4 geometry. The number of segments is computed such
that the maximum tangential error in the aperture is 1 mm.

.. note:: See :ref:`bend-tracking-behaviour` for important notes about dipole tracking.

+-----------------+-----------------------------------+-----------+-----------------+
| Parameter       | Description                       | Default   | Required        |
+=================+===================================+===========+=================+
| `l`             | Length [m]                        | 0         | Yes             |
+-----------------+-----------------------------------+-----------+-----------------+
| `angle`         | Angle [rad]                       | 0         | Yes, and or `B` |
+-----------------+-----------------------------------+-----------+-----------------+
| `B`             | Magnetic field [T]                | 0         | Yes             |
+-----------------+-----------------------------------+-----------+-----------------+
| `e1`            | Input poleface angle [rad]        | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `e2`            | Output poleface angle [rad]       | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `material`      | Magnet outer material             | Iron      | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `yokeOnInside`  | Yoke on inside of bend or not     | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `hStyle`        | H style poled geometry            | 0         | No              |
+-----------------+-----------------------------------+-----------+-----------------+
| `k1`            | Quadrupole coefficient for        | 0         | No              |
|                 | function magnet                   |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fint`          | Fringe field integral for the     | 0         | No              |
|                 | entrance face of the sbend        |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintx`         | Fringe field integral for the     | -1        | No              |
|                 | exit face of the sbend. -1 means  |           |                 |
|                 | default to the same as fint. 0    |           |                 |
|                 | there will be no effect.          |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintK2`        | Second fringe field integral for  | 0         | No              |
|                 | the entrance face of the sbend    |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `fintxK2`       | Second fringe field integral for  | 0         | No              |
|                 | the exit face of the sbend        |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `hgap`          | The half gap of the poles for     | 0         | No              |
|                 | **fringe field purposes only**    |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h1`            | Input poleface curvature          | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h2`            | Output poleface curvature         | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `tilt`          | Tilt of magnet [rad]              | 0         | no              |
+-----------------+-----------------------------------+-----------+-----------------+

Notes:

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.

.. figure:: figures/poleface_notation_sbend.pdf
	    :width: 75%
	    :align: center

	    Pole face notation for an sbend.

A few points about sbends:

1) The pole face rotation angle is limited to :math:`\pm \pi /4` radians.
2) If a non-zero pole face rotation angle is specified, the element preceding / succeeding
   the rotated magnet face must either be a drift or an sbend with the opposite rotation
   (e.g. an sbend with :math:`e2 = 0.1` can be followed by an sbend with
   :math:`e1 = -0.1`). The preceding / succeeding element must be longer than
   the projected length from the rotation, given by :math:`2 \tan(\mathrm{eX})`.
3) Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the beginning
   or at the end of the sbend. The length of the fringe field magnet can be
   set by the option `thinElementLength` (see :ref:`bdsim-options`).
4) In the case of finite `fint` or `fintx` and `hgap` a fringe field is used even
   if `e1` and `e2` have no angle.
5) The `fintK2` and `fintxK2` parameters are for a second fringe field correction term that are included to
   enable optics comparisons with TRANSPORT. Whilst this term is not available in MAD-X, the default values
   of 0 mean this second fringe field correction will not be applied unless `fintK2` or `fintxK2` are
   explicitly specified as non-zero.
6) The effect of pole face rotations and fringe field kicks can be turned off for all dipoles by setting
   the option `includeFringeFields=0` (see :ref:`bdsim-options`).
7) The poleface curvature does not construct the curved geometry. The effect is instead applied in the thin
   fringefield magnet.
8) Sbends are limited in angle to less than :math:`2\pi`. If the sbends are not split with the option dontSplitSBends,
   an sbend will be limited in angle to a maximum of :math:`\pi/2`.
9) A positive `tilt` angle corresponds to a clockwise rotation when looking along the beam direction as
   we use a right-handed coordinate system. A positive tilt angle of :math:`\pi/2` for an sbend with a
   positive bending angle will produce a vertical bend where the beam is bent downwards.

Examples: ::

   s1: sbend, l=14.5*m, angle=0.005, magnetGeometryType="lhcright";
   mb201x: sbend, l=304.2*cm, b=1.5*Tesla;
   SB17A: sbend, l=0.61*m, angle=0.016, e1=-0.05, e2=0.09

quadrupole
^^^^^^^^^^

.. figure:: figures/quadrupole.png
	    :width: 30%
	    :align: center

`quadrupole` defines a quadrupole magnet. The strength parameter :math:`k_1` is defined as

.. math::

   k_{1} = \frac{1}{B \rho}\,\frac{dB_{y}}{dx}\,[m^{-2}]

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`k1`              Quadrupole coefficient       0           Yes
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

Notes:

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

A pure quadrupolar field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

Examples: ::

   q1: quadrupole, l=0.3*m, k1=45.23;
   qm15ff: quadrupole, l=20*cm, k1=95.2;

sextupole
^^^^^^^^^

.. figure:: figures/sextupole.png
	    :width: 30%
	    :align: center

`sextupole` defines a sextupole magnet. The strength parameter :math:`k_2` is defined as

.. math::

   k_{2} = \frac{1}{B \rho}\,\frac{dB^{2}_{y}}{dx^{2}}\,[m^{-3}]

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`k2`              Sextupole coefficient        0           Yes
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

Notes:

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

A pure sextupolar field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

Examples: ::

   sx1: sextupole, l=0.5*m, k2=4.678;
   sx2: sextupole, l=20*cm, k2=45.32, magnetGeometry="normalconducting";

octupole
^^^^^^^^

.. figure:: figures/octupole.png
	    :width: 30%
	    :align: center

`octupole` defines an octupole magnet. The strength parameter :math:`k_3` is defined as

.. math::

   k_{3} = \frac{1}{B \rho}\,\frac{dB^{3}_{y}}{dx^{3}}\,[m^{-4}]

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`k3`              Octupole coefficient         0           Yes
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

Notes:

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

A pure octupolar field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

Examples: ::

   oct4b: octupole, l=0.3*m, k3=32.9;

decapole
^^^^^^^^

.. figure:: figures/decapole.png
	    :width: 30%
	    :align: center

`decapole` defines a decapole magnet. The strength parameter :math:`k_4` is defined as

.. math::

   k_{2} = \frac{1}{B \rho}\,\frac{dB^{4}_{y}}{dx^{4}}\,[m^{-5}]

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`k4`              Decapole coefficient         0           Yes
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

A pure decapolar field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples: ::

   MXDEC3: decapole, l=0.3*m, k4=32.9;

multipole
^^^^^^^^^

`multipole` defines a general multipole magnet. The strength parameter
:math:`knl` is a list defined as

.. math::

   knl[n] = \frac{1}{B \rho} \frac{dB^{n}_{y}}{dx^{n}}\,[m^{-(n+1)}]

starting with the **quadrupole** component. The skew strength parameter :math:`ksl`
is a list representing the skew coefficients.

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`knl`             List of normal coefficients  0           No
`ksl`             List of skew coefficients    0           No
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

Notes:

* The values for `knl` and `ksl` are 1-counting, i.e. the first number is the order 1 component,
  which is the quadrupole coefficient.
* The :ref:`aperture-parameters` may also be specified.
* The :ref:`magnet-geometry-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.
* No yoke field is provided.

Examples: ::

   OCTUPOLE1 : multipole, l=0.5*m , knl={ 0,0,1 } , ksl={ 0,0,0 };
   QUADRUPOLE1: multipole, l=20*cm, knl={2.3};
   

thinmultipole
^^^^^^^^^^^^^

`thinmultipole` is the same as multipole, but is set to have a default length of 1 micron.
For thin multipoles, the length parameter is not required. The element will appear as a thin length of drift
tube. A thin multipole can be placed next to a bending magnet with finite pole face rotation angles.

* `knl` and `ksl` are the same as the thick multiple documented above.  See `multipole`_.

Examples: ::

   THINOCTUPOLE1 : thinmultipole , knl={ 0,0,1 } , ksl={ 0,0,0 };

.. note:: The length of the thin multipole can be changed by setting `thinElementLength` (see :ref:`bdsim-options`).

vkicker
^^^^^^^

`vkicker` can either be a thin or thick vertical dipole magnet. If specified
with a finite length :code:`l`, it will be constructed as a thick dipole. However, if no length (or
a length of exactly 0 is specified), a thin kicker will be built. In practice, the thin version is
constructed as a 1um slice with only the aperture geometry and no surrounding geometry and is not
visible with the default visualisation settings.

The strength is specified by the parameter :code:`vkick`, which is the fractional momentum kick
in the vertical direction. A positive value corresponds to an increase in :math:`p_y`. In the
case of the thin kicker the position is not affected, whereas with the thick kicker, the position
will change.

The strength may also be specified by the magnetic field :code:`B`. A positive field value corresponds
to an increase in :math:`p_y` for a positively charged particle.

.. warning:: :code:`vkick` will supercede the strength even if :code:`B` is specified. Therefore, the
	     user should specify only :code:`vkick` or :code:`B`.

In the case of a thick kicker, the resulting bending angle is calculated using:

.. math::

   \theta = \sin^{-1}(\,p_x)

The dipole field strength is then calculated with respect to the chord length:

.. math::

   \mathbf{B} = B\rho\, \frac{\theta}{\mathrm{chord\,length}}


* The :ref:`aperture-parameters` may also be specified.
* For a vkicker with a finite length, the :ref:`magnet-geometry-parameters` may also be specified.

.. note:: Pole face rotations and fringe fields can be applied to vkickers by supplying the same
      parameters that would be applied to an `rbend`_ or `sbend`_ . If the vkicker is zero length,
      the B field value must be supplied in order to calculate the bending radius which required
      to apply the effects correctly.

* Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the
  beginning or at the end of the vkicker. The length of the fringe field element can be set by the
  option `thinElementLength` (see :ref:`bdsim-options`).
* For zero length vkickers, the pole face and fringe field kicks are applied in the same thin element
  as the vkick.
* In the case of finite fint or fintx and hgap, a fringe field is used even if e1 and e2 have no angle.
* The effect of pole face rotations and fringe field kicks can be turned off for all magnets by setting
  the option `includeFringeFields=0` (see :ref:`bdsim-options`).
* No pole face geometry is constructed.


A pure dipole field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

Examples: ::

   KX15v: vkicker, vkick=1.3e-5;
   KX17v: vkicker, vkick=-2.4e-2, l=0.5*m;
   KX18v: vkicker, B=0.04*T;


hkicker
^^^^^^^

`hkicker` can either be a thin horizontal kicker or a thick horizontal dipole magnet. If
specified with a finite length :code:`l`, it will be constructed as a dipole. However, if no length (or
a length of exactly 0) is specified, a thin kicker will be built. This is typically a 1um slice
with only the shape of the aperture and no surrounding geometry. It is also typically not
visible with the default visualisation settings.

The strength is specified by the parameter :code:`hkick`, which is the fractional momentum kick
in the vertical direction. A positive value corresponds to an increase in :math:`p_x`. In the
case of the thin kicker the position is not affected, whereas with the thick kicker, the position
will change.

The strength may also be specified by the magnetic field :code:`B`. A positive field value corresponds
to an decrease in :math:`p_x` (note right-handed coordinate frame) for a positively charged particle.

.. warning:: :code:`hkick` will supercede the strength even if :code:`B` is specified. Therefore, the
	     user should specify only :code:`hkick` or :code:`B`.

.. note:: A positive value of `hkick` causes an increase in horizontal momentum, so the particle
	  will bend to the left looking along the beam line, i.e. in positive `x`. This is
	  the opposite of a bend where a positive *angle* causes a deflection in negative
	  `x`.

* The :ref:`aperture-parameters` may also be specified.
* For a hkicker with a finite length, the :ref:`magnet-geometry-parameters` may also be specified.

.. note:: Pole face rotations and fringe fields can be applied to hkickers by supplying the same
      parameters that would be applied to an `rbend`_ or `sbend`_ . If the hkicker is zero length,
      the B field value must be supplied in order to calculate the bending radius which required
      to apply the effects correctly.

* Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the
  beginning or at the end of the hkicker. The length of the fringe field element can be set by the
  option `thinElementLength` (see :ref:`bdsim-options`).
* For zero length hkickers, the pole face and fringe field kicks are applied in the same thin element
  as the hkick.
* In the case of finite fint or fintx and hgap, a fringe field is used even if e1 and e2 have no angle.
* The effect of pole face rotations and fringe field kicks can be turned off for all magnets by setting
  the option `includeFringeFields=0` (see :ref:`bdsim-options`).
* No pole face geometry is constructed.

A pure dipole field is provided in the beam pipe and a more general multipole (as
described by :ref:`yoke-multipole-field`) is provided for the yoke.

Examples: ::

   KX17h: hkicker, hkick=0.01;
   KX19h: hkicker, hkick=-1.3e-5, l=0.2*m;
   KX21h: hkicker, B=0.03*T;


kicker
^^^^^^

`kicker` defines a combined horizontal and vertical kicker.  Either both or one of the
parameters `hkick` and `vkick` may be specified. Like the `hkicker` and `vkicker`, this
may also be thin or thick. In the case of the thick kicker, the field is the linear
sum of two independently calculated fields.

.. note:: Pole face rotation and fringe fields kicks are unavailable for plain kickers

Example::

  kick1: kicker, l=0.45*m, hkick=1.23e-4, vkick=0.3e-4;


tkicker
^^^^^^^

BDSIM, like MAD-X, provides a `tkicker` element. This is an alias in BDSIM for a `kicker`_,
however MAD-X differentiates the two on the basis of fitting parameters. BDSIM does
not make this distinction. See `kicker`_ for more details.

In the case of a `tkicker`, the field :code:`B` cannot be used and only `hkick` and `vkick`
can be used.

.. note:: Pole face rotation and fringe fields kicks are unavailable for tkickers

rf
^^^^

.. figure:: figures/rfcavity.png
	    :width: 50%
	    :align: center

`rf` or `rfcavity` defines an RF cavity with a time varying electric or electromagnetic field.
There are several geometry and field options as well as ways to specify the strength.
The default field is a uniform (in space) electric-only field that is time varying
according to a cosine (see :ref:`field-sinusoid-efield`).  Optionally, the electromagnetic
field for a pill-box cavity may be used (see :ref:`field-pill-box`). The `G4ClassicalRK4`
numerical integrator is used to calculate the motion of particles in both cases. Fringes for
the edge effects are provided by default and are controllable with the option `includeFringeFieldsCavities`.


+----------------+-------------------------------+--------------+---------------------+
| **Parameter**  | **Description**               | **Default**  | **Required**        |
+================+===============================+==============+=====================+
| `l`            | Length [m]                    | 0            | Yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `E`            | Electric field strength       | 0            | Yes (or `gradient`) |
+----------------+-------------------------------+--------------+---------------------+
| `gradient`     | Field gradient [MV/m]         | 0            | Yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `frequency`    | Frequency of oscillation (Hz) | 0            | Yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `phase`        | Phase offset (rad)            | 0            | No                  |
+----------------+-------------------------------+--------------+---------------------+
| `tOffset`      | Offset in time (s)            | 0            | No                  |
+----------------+-------------------------------+--------------+---------------------+
| `material`     | Outer material                | ""           | Yes                 |
+----------------+-------------------------------+--------------+---------------------+
| `cavityModel`  | Name of cavity model object   | ""           | No                  |
+----------------+-------------------------------+--------------+---------------------+

.. note:: The design energy of the machine is not affected, so the strength and fields
	  of components after an RF cavity in a lattice are calculated with respect to
	  the design energy, the particle and therefore, design rigidity. The user should
	  scale the strength values appropriately if they wish to match the increased
	  energy of the particle.

.. warning:: The elliptical cavity geometry may not render or appear in the Geant4
	     QT visualiser.  The geometry exists and is valid, but this is due to
	     deficiencies of the Geant4 visualisation system. The geometry exists
	     and is fully functional.

* The field is such that a positive E-field results in acceleration of the primary particle.
* The phase is calculated automatically such that zero phase results in the peak E-field at
  the centre of the component for its position in the lattice.
* Either `tOffset` or `phase` may be used to specify the phase of the oscillator.
* The material must be specified in the `rf` gmad element or in the attached cavity model
  by name. The cavity model will override the element material.
* The entrance / exit cavity fringes are not constructed if the previous / next element
  is also an rf cavity.
* The cavity fringe element is by default the same radius as the beam pipe radius. If a cavity
  model is supplied, the cavity fringes are built with the same radius as the model iris radius.

If `tOffset` is specified, a phase offset is calculated from this time for the **speed
of light in a vacuum**. Otherwise, the curvilinear S-coordinate of the centre of the rf
element is used to find the phase offset.

.. note:: As the phase offset is calculated from the speed of light in a vacuum, this is
	  only correct for already relativistic beams. Development is underway to improve
	  this calculation for sub-relativistic beams.

If `phase` is specified, this is added to the calculated phase offset from either the lattice
position or `tOffset`.

In the case where `frequency` is not set, the phase offset is ignored and only the `phase` is
used. See the developer documentation :ref:`field-sinusoid-efield` for a description of the field.

Simple examples: ::

   rf1: rf, l=10*cm, E=10*MV, frequency=90*MHz, phase=0.02;
   rf2: rf, l=10*cm, gradient=14*MV / m, frequency=450*MHz;
   rf3: rf, l=10*cm, E=10*MV, frequency=90*MHz, tOffset=3.2*ns;

Rather than just a simple E-field, an electromagnetic field that is the solution to
a cylindrical pill-box cavity may be used. A cavity object (described in more detail
below) is used to specify the field type. All other cavity parameters may be safely ignored
and only the field type will be used. The field is described in :ref:`field-pill-box`.

Pill-box field example::

  rffield: field, type="rfcavity";
  rf4: rf, l=10*cm, E=2*kV, frequency=1.2*GHz, fieldVacuum="rffield";

Elliptical SRF cavity geometry is also provided and may be specified by use of another
'cavity' object in the parser. This cavity object can then be attached to an `rf`
object by name. Details can be found in :ref:`cavity-geometry-parameters`.


rcol
^^^^

.. figure:: figures/rcol.png
	    :width: 40%
	    :align: center

An `rcol` defines a rectangular collimator. The aperture is rectangular and the external
volume is square.

+--------------------+------------------------------+--------------+---------------+
| **Parameter**      | **Description**              | **Default**  | **Required**  |
+====================+==============================+==============+===============+
| `l`                | Length [m]                   | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `xsize`            | Horizontal half aperture [m] | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `ysize`            | Half height of jaws [m]      | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `material`         | Outer material               | None         | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `horizontalWidth`  | Outer full width [m]         | 0.5 m        | No            |
+--------------------+------------------------------+--------------+---------------+
| `xsizeLeft`        | Left jaw aperture [m]        | 0            | No            |
+--------------------+------------------------------+--------------+---------------+
| `xsizeRight`       | Right jaw aperture [m]       | 0            | No            |
+--------------------+------------------------------+--------------+---------------+

Notes: 

* `horizontalWidth` should be big enough to encompass the xsize and ysize.
* If no `xsize` or `ysize` are provided, they are assumed to be 0 and a solid block is made.
* The parameter `minimumKineticEnergy` (GeV by default) may be specified to artificially kill
  particles below this kinetic energy in the collimator. This is useful to match other simulations
  where collimators can be assumed to be infinite absorbers. If this behaviour is required, the
  user should specify an energy greater than the total beam energy.
* The collimator can be tapered by specifying an exit aperture size with `xsizeOut` and
  `ysizeOut`, with the `xsize` and `ysize` parameters defining the entrance aperture.
* All collimators can be made infinite absorbers with the general option
  :code:`collimatorsAreInfiniteAbsorbers` (see :ref:`options-tracking`).


Examples: ::

   ! Standard
   TCP15: rcol, l=1.22*m, material="graphite", xsize=104*um, ysize=5*cm;

   ! Tapered
   TCP16: rcol, l=1.22*m, material="graphite", xsize=104*um, ysize=5*cm, xsizeOut=208*um, ysizeOut=10*cm;
   ! with kinetic energy limit
   TCP6CD: rcol, l=0.6*m, material="C", xsize=200*um, ysize=5*cm, minimumKineticEnergy=10*MeV;
   

ecol
^^^^

.. figure:: figures/ecol.png
	    :width: 40%
	    :align: center

`ecol` defines an elliptical collimator. This is exactly the same as `rcol` except that
the aperture is elliptical and the `xsize` and `ysize` define the horizontal and vertical
half-axes respectively.

* A circular aperture collimator can be achieved by setting `xsize` and `ysize` to the
  same value.
* When tapered, the ratio between the horizontal and vertical half-axes of the entrance
  aperture must be the same ratio for the exit aperture.
* All the same conditions for `rcol` apply for `ecol`.

jcol
^^^^

.. figure:: figures/jcol.png
	    :width: 40%
	    :align: center


`jcol` defines a jaw collimator with two square blocks on either side in the horizontal plane.
If a vertical `jcol` is required, the `tilt` parameter should be used to rotate it by :math:`\pi/2`.
The horizontal position of each jaw can be set separately with the `xsizeLeft` and `xsizeRight`
apertures which are the distances from the centre of element to the left and right jaws respectively.


+--------------------+------------------------------+--------------+---------------+
| **Parameter**      | **Description**              | **Default**  | **Required**  |
+====================+==============================+==============+===============+
| `l`                | Length [m]                   | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `xsize`            | Horizontal half aperture [m] | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `ysize`            | Half height of jaws [m]      | 0            | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `material`         | Outer material               | None         | Yes           |
+--------------------+------------------------------+--------------+---------------+
| `xsizeLeft`        | Left jaw aperture [m]        | 0            | No            |
+--------------------+------------------------------+--------------+---------------+
| `xsizeRight`       | Right jaw aperture [m]       | 0            | No            |
+--------------------+------------------------------+--------------+---------------+
| `horizontalWidth`  | Outer full width [m]         | 0.5 m        | No            |
+--------------------+------------------------------+--------------+---------------+


Notes: 

* The `horizontalWidth` must be greater than 2x `xsize`.
* To prevent the jaws overlapping with one another, a jaw cannot be constructed that crosses the
  X axis of the element (i.e supplying a negative `xsizeLeft` or `xsizeRight` will not work). Should
  you require this, please offset the element using the element parameters `offsetX` and `offsetY` instead.
* To construct a collimator jaws with one jaw closed (i.e. an offset of 0), the horizontal half aperture
  must be set to 0, with the other jaws half aperture set as appropriate.
* If `xsize`, `xsizeLeft` and `xsizeRight` are not specified, the collimator will be constructed
  as a box with no aperture.
* Specifying a jaw aperture which is larger than half the `horizontalWidth` value will result in
  that jaw not being constructed. If both jaw apertures are greater than half the `horizontalWidth`,
  no jaws will be built and BDSIM will exit.
* The parameter `minimumKineticEnergy` (GeV by default) may be specified to artificially kill
  particles below this kinetic energy in the collimator. This is useful to match other simulations
  where collimators can be assumed to be infinite absorbers. If this behaviour is required, the
  user should specify an energy greater than the total beam energy.
* All collimators can be made infinite absorbers with the general option
  :code:`collimatorsAreInfiniteAbsorbers` (see :ref:`options-tracking`).


Examples: ::

   ! Standard
   TCP15: jcol, l=1.22*m, material="graphite", xsize=0.1*cm, ysize=5*cm;

   ! with kinetic energy limit
   TCP6CD: rcol, l=0.6*m, material="C", xsize=200*um, ysize=5*cm, minimumKineticEnergy=10*MeV;


degrader
^^^^^^^^

.. figure:: figures/degrader.png
	    :width: 60%
	    :align: center


`degrader` defines interleaved pyramidal pieces of material. Depending on the physics list
used, this is capable of reducing the beam energy. This happens only through interaction
and the use of a physics list. Note, the default physics list in BDSIM is no physics and
only magnetic tracking, in which case this component will have no effect.

.. tabularcolumns:: |p{4cm}|p{4cm}|p{2cm}|p{2cm}|

===================    =======================================  ==========  ===========
Parameter              Description                              Default     Required
`l`                    Length [m]                               0           Yes
`numberWedges`         Number of degrader wedges                1           Yes
`wedgeLength`          Degrader wedge length [m]                0           Yes
`degraderHeight`       Degrader height [m]                      0           Yes
`materialThickness`    Amount of material seen by the beam [m]  0           Yes/No*
`degraderOffset`       Horizontal offset of both wedge sets     0           Yes/No*
`material`             Degrader material                        Carbon      Yes
`horizontalWidth`      Outer full width [m]                     global      No
===================    =======================================  ==========  ===========

.. note:: Either `materialThickness` or `degraderOffset` can be specified to adjust the horizontal
	  lateral wedge position, and consequently the total material thickness the beam can propagate
	  through. If both are specified, `degraderOffset` will be ignored. When numberWedges is specified
	  to be n, the degrader will consist of n-1 `full` wedges and two `half` wedges. When viewed from
	  above, a `full` wedge appears as an isosceles triangle, and a `half` wedge appears as a
	  right-angled triangle.


Examples: ::

    DEG1: degrader, l=0.25*m, material="carbon", numberWedges=5, wedgeLength=100*mm, degraderHeight=100*mm, materialThickness=200*mm;
    DEG2: degrader, l=0.25*m, material="carbon", numberWedges=5, wedgeLength=100*mm, degraderHeight=100*mm, degraderOffset=50*mm;


muspoiler
^^^^^^^^^

.. figure:: figures/muspoiler.png
	    :width: 40%
	    :align: center

`muspoiler` defines a muon spoiler, which is a rotationally magnetised iron cylinder with
a beam pipe in the middle. There is no magnetic field in the beam pipe.

=================  ============================  ==========  ===========
Parameter          Description                   Default     Required
`l`                Length [m]                    0           Yes
`B`                Magnetic field [T]            0           Yes
`material`         Outer material                Iron        No
`horizontalWidth`  Outer full width [m]          global      No
=================  ============================  ==========  ===========

shield
^^^^^^

.. figure:: figures/shield.png
	    :width: 40%
	    :align: center

`shield` defines a square block of material with a square aperture. The user may choose
the outer width and inner horizontal and vertical apertures of the block. A beam pipe
is also placed inside the aperture.  If the beam pipe dimensions (including thickness)
are greater than the aperture, the beam pipe will not be created.

=================  ==================================  ==========  ===========
Parameter          Description                         Default     Required
`l`                Length [m]                          0           Yes
`material`         Outer material                      Iron        No
`horizontalWidth`  Outer full width [m]                global      No
`xsize`            Horizontal inner half aperture [m]  0           Yes
`ysize`            Vertical inner half aperture [m]    0           No
=================  ==================================  ==========  ===========

Notes:

* The :ref:`aperture-parameters` may also be specified.

dump
^^^^

.. figure:: figures/dump.png
	    :width: 40%
	    :align: center

`dump` defines a square or circular block of material that is an infinite absorber. All
particles impacting the dump will be absorbed irrespective of the particle and physics
processes.

This is intended as a useful way to absorb a beam with no computational time. Normally,
the world volume is filled with air. If the beam reaches the end of the beam line it will
hit the air and likely create a shower of particles that will take some time to simulate.
In this case, when this isn't required, it is recommended to use a dump object to absorb
the beam.

=================  ==================================  ===========  ===========
Parameter          Description                         Default      Required
`l`                Length [m]                          0            Yes
`horizontalWidth`  Outer full width [m]                global       No
`apertureType`     Which shape                         rectangular  No
=================  ==================================  ===========  ===========

How this works: the material of the dump is actually vacuum, but G4UserLimits are used to
kill particles. This requires the cuts and limits physics process that is included automatically.
In the case of using a Geant4 reference physics list (see :ref:`physics-geant4-lists`), the
necessary process is added automatically to enforce this.

The dump may accept `apertureType` with the value of either `circular` or `rectangular` for
the shape of the dump. By default it is rectangular.

Examples: ::

  d1: dump, l=0.2*m, horizontalWidth=20*cm;
  d2: dump, l=0.4*m, horizontalWidth=30*cm, apertureType="circular";
  d3: dump, l=0.3*m, horizontalWidth=40*cm, apertureType="rectangular";


solenoid
^^^^^^^^

.. figure:: figures/solenoid.png
	    :width: 40%
	    :align: center

`solenoid` defines a solenoid magnet. This utilises a thick lens transfer map with a
hard edge field profile. Fringes for the edge effects are provided by default and
are controllable with the option `includeFringeFields`.

=================  ============================  ==========  ===========
Parameter          Description                   Default     Required
`l`                Length [m]                    0           Yes
`ks`               Solenoid strength             0           No
`B`                Magnetic field                0           No
`material`         Outer material                Iron        No
`horizontalWidth`  Outer full width [m]          global      No
=================  ============================  ==========  ===========

* A positive field corresponds to a field in along the direction of positive S.
* The entrance / exit solenoid fringes are not constructed if the previous / next element is also a solenoid.
* See `Magnet Strength Polarity`_ for polarity notes.
* No yoke field is provided.

Examples: ::

   atlassol: solenoid, l=20*m, ks=0.004;

wirescanner
^^^^^^^^^^^

.. figure:: figures/wirescanner.png
            :width: 40%
	    :align: center


`wirescanner` defines a cylindrical object within a beam pipe to represent a wire
scanner typically use in an accelerator.

=====================  ===============================================  ==========  ==========
parameter              description                                      default     required
`l`                    length of drift section around wire              0           yes
`wireDiameter`         diameter of wire [m]                             0           yes
`wireLength`           length of wirescanner [m]                        0           yes
`material`             material of wire                                 none        yes
`wireAngle`            angle of the wire w.r.t. vertical                0           no
`wireOffsetX`          x offset of the wire from the center [m]         0           no
`wireOffsetY`          y offset of the wire from the center [m]         0           no
`wireOffsetZ`          z offset of the wire from the center [m]         0           no
=====================  ===============================================  ==========  ==========

Notes:

* The angle is the rotation from vertical in the clock-wise direction looping in the
  positive S direction (the usually direction of the beam).

.. warning:: After BDSIM V1.3.2 :code:`wireAngle` is used for the angle instead of
	     :code:`angle` as :code:`angle` is used specifically for angles of bends
	     and this could result in the curvilinear world being made very small.

The offsets are with respect to the centre of the beam pipe section the wire is placed inside.
This should therefore be less than half the element length `l`. The usual beam pipe parameters
can be specified and apply the to the beam pipe. For example, `material` is used for the beam
pipe material whereas `wireMaterial` is used for the material of the wire.

The user should take care to define a wire long enough to intercept the beam but be sufficiently
short to fit inside the beam pipe given the offsets in x, y and z. Checks are made on the end
points of the wire.

Examples: ::

    ws45Deg: wirescanner, l=4*cm, wireDiameter=0.1*mm, wireLength=5*cm,
                          wireOffsetX=1*cm, angle=pi/4, wireMaterial="C",
			  aper1=5*cm;

    wsVertical: wirescanner, l=4*cm, wireDiameter=0.1*mm, wireLength=5*cm,
                             wireOffsetX=1*cm, wireOffsetZ=1.6*cm, wireMaterial="C";


laser
^^^^^

`laser` defines a drift section with a laser beam inside. The laser acts as a static target
of photons.

================  =================================================  ==========  ===========
Parameter         Description                                        Default     Required
`l`               Length of drift section [m]                        0           Yes
`x`, `y`, `z`     Components of laser direction vector (normalised)  (1,0,0)     yes
`waveLength`      Laser wavelength [m]                               532*nm      Yes
================  =================================================  ==========  ===========

Examples: ::

   laserwire: laser, l=1*um, x=1, y=0, z=0, wavelength=532*nm;


gap
^^^

`gap` defines a gap where no physical geometry is placed. It will be a region of the world,
composed of the same material as the world volume.

.. tabularcolumns:: |p{4cm}|p{4cm}|p{2cm}|p{2cm}|

===================    =======================================  ==========  ===========
Parameter              Description                              Default     Required
`l`                    Length [m]                               0           Yes
`angle`                Angle [rad]                              0           No
===================    =======================================  ==========  ===========

Examples: ::

    GAP1: gap, l=0.25*m, angle=0.01*rad;

.. _element-crystal-col:
    
crystalcol
^^^^^^^^^^

.. figure:: figures/crystalcol.png
	    :width: 30%
	    :align: center


`crystalcol` defines a crystal collimator that uses crystals to channel particles. It is
composed of a beam pipe with one or two crystals inside it. The crystals can be the same
(but placed at different angles) or different. The crystals are placed +- `xsize` away
from the centre.

The crystal is defined in a separate object in the parser and referred to by the name of
that object. At least one of `crystalBoth`, `crystalLeft` and `crystalRight` must be
specified.

.. warning:: This requires the user to use the "completechannelling" or "channelling" physics list
	     for channelling to take place.

==========================  ======================================================  ===========  =========
Parameter                   Description                                             Default      Required
`l`                         Length [m]                                              0            Yes
`xsize`                     Half-gap distance of each crystal from centre [m]       0            Yes
`material`                  Material                                                ""           Yes
`crystalBoth`               Name of crystal object for both crystals                ""           No
`crystalLeft`               Name of crystal object for right crystal                ""           No
`crystalRight`              Name of crystal object for left crystal                 ""           No
`crystalAngleYAxisLeft`     Rotation angle of left crystal [rad]                    0            No
`crystalAngleYAxisRight`    Rotation angle of right crystal [rad]                   0            No
==========================  ======================================================  ===========  =========

Notes:

* Crystal channelling potential files are required for this - see :ref:`crystals` for more details.
* If only `crystalLeft` or `crystalRight` is specified, only one crystal will be placed.
* If both `crystalLeft` and `crystalRight` are specified, both will be constructed uniquely and placed.
* If `crystalBoth` is specified, `crystalLeft` and `crystalRight` will be ignored and the `crystalBoth`
  definition will be used for both crystals. The angles will be unique.

.. note:: Crystal channelling is only available in Geant4.10.4 onwards. If BDSIM is compiled with a Geant4
	  version below this, the geometry will be constructed correctly but the channelling physics process
	  will not be used and the crystal will not channel particles.

* See :ref:`crystals` for the definition of a crystal object.

Examples: ::

  lovelycrystal: crystal, material = "G4_Si",
	       		data = "data/Si220pl",
			shape = "box",
			lengthY = 5*cm,
			lengthX = 0.5*mm,
			lengthZ = 4*mm,
			sizeA = 5.43*ang,
			sizeB = 5.43*ang,
			sizeC = 5.43*ang,
			alpha = 1,
			beta  = 1,
			gamma = 1,
			spaceGroup = 227,
			bendingAngleYAxis = 0.1*rad,
			bendingAngleZAxis = 0;

   col1 : crystalcol, l=6*mm, apertureType="rectangular", aper1=0.25*cm, aper2=5*cm,
                      crystalBoth="lovelycrystal", crystalAngleYAxisLeft=-0.1*rad,
		      crystalAngleYAxisRight=-0.1*rad, xsize=2*mm;


More examples can be found in :code:`bdsim/examples/components` and are described in :ref:`crystal-examples`.

undulator
^^^^^^^^^

.. figure:: figures/undulator.png
	    :width: 60%
	    :align: center

`undulator` defines an undulator magnet which has a sinusoidally varying field along the element with
field components:

.. math::

   B_{x} ~ &= ~ 0 \\
   B_{y} ~ &= ~ B \cdot \cos\big(z \frac{2\pi}{\lambda}\big) \cosh\big(y \frac{2\pi}{\lambda}\big)\\
   B_{z} ~ &= ~ -B \cdot \sin\big(z \frac{2\pi}{\lambda}\big) \sinh\big(y \frac{2\pi}{\lambda}\big)

where :math:`\lambda` is the undulator period.

=======================  =============================  ==========  ===========
Parameter                Description                    Default     Required
`l`                      Length [m]                     0           Yes
`B`                      Magnetic field [T]             0           Yes
`undulatorPeriod`        Undulator magnetic period [m]  1           Yes
`undulatorGap`           Undulator gap [m]              0           No
`undulatorMagnetHeight`  Undulator magnet height [m]    0           No
`material`               Magnet outer material          Iron        No
=======================  =============================  ==========  ===========

Notes:

* The undulator period must be an integer factor of the undulator length. If not, BDSIM will exit.
* The undulator gap is the total distance between the upper and lower sets of magnets. If not supplied,
  it is set to twice the beam pipe diameter.
* The undulator magnet height is the vertical height of the sets of magnets. If not supplied, it is set
  to the 0.5*horizontalWidth - undulator gap.
* The :ref:`aperture-parameters` may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.
* To generate radiation from particles propagating through the undulator field, synchrotron radiation
  physics must be included in the model's physicsList. See :ref:`physics-processes` for further details.

Examples: ::

 u1: undulator, l=2.0*m, B=0.1*T, undulatorPeriod=0.2*m;
 u2: undulator, l=3.2*m, B=0.02*T, undulatorPeriod=0.16*m, undulatorGap=15*cm, undulatorMagnetHeight=10*cm;


transform3d
^^^^^^^^^^^

`transform3d` defines an arbitrary three-dimensional transformation of the curvilinear coordinate
system at that point in the beam line sequence.  This is often used to rotate components by a large
angle.


================  ============================  ==========  ===========
Parameter         Description                   Default     Required
`x`               x offset                      0           No
`y`               y offset                      0           No
`z`               z offset                      0           No
`phi`             phi Euler angle               0           No
`theta`           theta Euler angle             0           No
`psi`             psi Euler angle               0           No
================  ============================  ==========  ===========

.. note:: this permanently changes the coordinate frame, so care must be taken to undo any rotation
	  if intended for only one component.

Examples: ::

   rcolrot: transform3d, psi=pi/2;

.. _element:

element
^^^^^^^

`element` defines an arbitrary element that's defined by externally provided geometry. It includes
the possibility of overlaying a field as well. Several geometry formats are supported. The user
must supply the length (accurately) as well as a diameter, such that the geometry will be
contained in a box that has horizontal and vertical sizes of diameter.

The geometry is simply placed in the beam line. There is no placement offset other than the
offset and tilt of the element in the beam line. Therefore, the user must prepare geometry
with the placement as required.

An alternative strategy is to use the `gap`_ beam line element
and make a placement at the appropriate point in global coordinates.

+-------------------+----------------------------------+--------------+---------------+
| **Parameter**     | **Description**                  | **Default**  | **Required**  |
+===================+==================================+==============+===============+
| `geometryFile`    | Filename of geometry             | NA           | Yes           |
+-------------------+----------------------------------+--------------+---------------+
| `l`               | Length. Arc length in case of a  | NA           | Yes           |
|                   | finite angle.                    |              |               |
+-------------------+----------------------------------+--------------+---------------+
| `horizontalWidth` | Diameter of component [m]        | NA           | Yes           |
+-------------------+----------------------------------+--------------+---------------+
| `fieldAll`        | Name of field object to use      | NA           | No            |
+-------------------+----------------------------------+--------------+---------------+
| `angle`           | Angle the component bends the    | 0            | No            |
|                   | beam line.                       |              |               |
+-------------------+----------------------------------+--------------+---------------+
| `tilt`            | Tilt of the whole component.     | 0            | No            |
+-------------------+----------------------------------+--------------+---------------+

* `geometryFile` should be of the format `format:filename`, where `format` is the geometry
  format being used (`gdml` | `gmad` | `mokka`) and filename is the path to the geometry
  file. See :ref:`externally-provided-geometry` for more details.
* `fieldAll` should refer to the name of a field object the user has defined in the input
  gmad file. The syntax for this is described in :ref:`field-maps`.
* The field map will also be tilted with the component if it is tilted.

.. note:: The length must be larger than the geometry so that it is contained within it and
	  no overlapping geometry will be produced. However, care must be taken, as the length
	  will be the length of the component inserted in the beamline.  If this is much larger
	  than the size required for the geometry, the beam may be mismatched into the rest of
	  the accelerator. A common practice is to add a picometre to the length of the geometry.

Simple example::

  detector: element, geometryFile="gdml:atlasreduced.gdml", horizontalWidth=10*m,l=44*m;

Example with field: ::

  somefield: field, type="ebmap2d",
		    eScaling = 3.1e3,
		    bScaling = 0.5,
		    integrator = "g4classicalrk4",
		    magneticFile = "poisson2d:/Path/To/File.TXT",
		    magneticInterpolator = "nearest2D",
		    electricFile = "poisson2d:/Another/File.TX",
		    electricInterpolator = "linear2D";

   detec: element, geometryFile="mokka:qq.sql", fieldAll="somefield", l=5*m, horizontalWidth=0.76*m;

.. note:: For GDML geometry, we preprocess the input file prepending all names with the name
	  of the element. This is to compensate for the fact that the Geant4 GDML loader does
	  not handle unique file names. However, in the case of very large files with  many
	  vertices, the preprocessing can dominate. In this case, the option `preprocessGDML`
	  should be turned off. The loading will only work with one file in this case.


marker
^^^^^^
`marker` defines a point in the lattice. This element has no physical length and is only
used as a reference. For example, a `sampler` (see :ref:`sampler-output`)
is used to record particle passage at the
front of a component, but how would you record particles exiting a particular component?
The intended method is to use a `marker` and place it in the sequence after that element,
then attach a sampler to the marker.

Examples: ::

   m1: marker;

.. _offsets-and-tilts:

Offsets & Tilts - Component Misalignment
----------------------------------------

To simulate a real accelerator it may be necessary to introduce measured placement offsets or misalignments
and rotations. Every component can be displaced transversely and rotated along the axis of the beam propagation.

.. note:: Components that have a finite angle (rbend and sbend) will only respond to tilt and not vertical or
	  horizontal offsets. This is because these would change the length of the bend about its central axis.
	  This is not currently handled but may be implemented in future releases.

.. note:: A tilt on a component with a finite angle causes the axis the angle is induced in (typically the y-
	  axis) to be rotated without rotating the reference frame of the beam, i.e. a dipole with a :math:`\pi/2`
	  tilt will become a vertical bend without flipping x and y in the sampler or subsequent components. This
	  matches the behaviour of MAD8 and MAD-X.

.. note:: A right-handed coordinate system is used and the beamline is built along the `z` direction.

.. note:: Tilts and offsets are independent of one another, that is to
          say, a tilt of :math:`\pi/2` combined with a non-zero
          offsetX will not result in a vertical displacement.

The misalignments can be controlled through the following parameters.

+--------------+------------------------------------------------------------------------------------+
| Parameter    | Default value                                                                      |
+==============+====================================================================================+
| `offsetX`    | Horizontal displacement of the component [m]                                       |
+--------------+------------------------------------------------------------------------------------+
| `offsetY`    | Vertical displacement of the component [m]                                         |
+--------------+------------------------------------------------------------------------------------+
| `tilt`       | Clockwise rotation of the component, facing in the direction of the beamline       |
|              | `z` [rad]. In the case of an rbend or sbend, this rotates the axis about which the |
|	       | beam bends.                                                                        |
+--------------+------------------------------------------------------------------------------------+

Examples: ::

  d1: drift, l=1*m, offsetX=1*cm;
  d2: drift, l=0.5*m, offsetY = 0.3*cm, tilt=0.003;


.. _lattice-sequence:

Lattice Sequence
----------------

Once all the necessary components have been defined, they must be placed in a sequence to make
a lattice. Elements can be repeated. A sequence of elements is defined by
a `line`_. Lines of lines can be made to describe the accelerator sequence programmatically, i.e.
::

   d1: drift, l=3*m;
   q1: quadrupole, l=0.1*m, k1=0.684;
   q2: quadrupole, l=0.1*m, k1=-0.684;
   fodo: line = (q1,d1,q2,d1);
   transportline: line(fodo, fodo, fodo, fodo);


Line
^^^^

`line` defines a sequence of elements. ::

  name: line=(element1, element2, element3, ... );

Here, `element` can be any element or line. Lines can also be reversed using ::

  line_name : line=-(line_2)

or within another line by::

  line=(line_1,-line_2)

Reversing a line also reverses all nested lines within.

.. _the-use-command:

use - Defining which Line to Use
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Once all elements and at least one `line` is defined, the main sequence of the
beam line can be defined. This must be defined using the following syntax::

  use, period=<line_name>

Examples: ::

   d1: drift, l=3.2*m;
   q1: quadrupole, l=20*cm, k1=4.5;
   q2: quadrupole, l=20*cm, k1=-4.5;
   fodo: line=(d1,q1,d1,q2,d1);
   use, period=fodo;

The beam line is placed in the world volume (the outermost coordinate system) starting
at position (0,0,0) with direction (0,0,1) - i.e. pointing in positive `z`. The user
may specify an initial offset and rotation for the beam line with respect to the world
volume using the options described in :ref:`beamline-offset`.

Multiple beam lines may also be visualised - but only visualised (not suitable for
simulations currently).  Details are provided in :ref:`multiple-beamlines`.

.. _multiple-beamlines:

Multiple Beam Lines
-------------------

BDSIM has the ability to use multiple beam lines.  This feature is still in development and
is currently only for visualisation purposes. Secondary beam lines are placed either with
respect to the world coordinate system or with respect to a particular element in the main
beam line. A few caveats:

* Only for visualisation purposes
* Beam lines cannot be placed with respect to an element in another secondary beam line.
* Secondary beam lines are not suitable for tracking.
* Secondary beam lines are not sensitive to energy deposition, nor do they produce output.
* The user is entirely responsible for overlapping geometry. The visualiser will render
  the geometry, but of course it will not be suitable for simulations, as overlaps lead
  to volume navigation problems and incorrect tracking.

The user may use any sequence defined in the parser before the `use` command. The secondary
beam line is produced by declaring a placement. The placement definition (see
:ref:`placements`) is augmented with the following parameters:

.. tabularcolumns:: |p{5cm}|p{10cm}|

+------------------------+---------------------------------------------------------------+
| **Parameter**          |  **Description**                                              |
+------------------------+---------------------------------------------------------------+
| sequence               | Name of the sequence (with `line`) to use for the secondary   |
|                        | beam line                                                     |
+------------------------+---------------------------------------------------------------+
| referemeceElement      | The element in the sequence with respect to which the beam    |
|                        | line will be placed                                           |
+------------------------+---------------------------------------------------------------+
| referenceElementNumber | The *i* th instance of the element in the sequence (zero      |
|                        | counting) ( i.e. 2 -> the 3rd instance of `referenceElement`  |
|                        | in the `sequence`).                                           |
+------------------------+---------------------------------------------------------------+

Examples
^^^^^^^^

This example is shown in bdsim/examples/features/geometry/10_multiple_beamlines.  It defines
a simple beam line and two other sequences that are placed alongside it. Further explanation
is given below the example.

::

   d1: drift, l=1*m;
   d2: drift, l=3*m;
   d3: drift, l=5*m;
   sb1: sbend, l=1*m, angle=0.5;
   sb2: sbend, l=1*m, angle=-0.5;
   q1: quadrupole, l=0.2*m, k1=4.166666;
   q2: quadrupole, l=0.2*m, k1=-4.166666;

   fodo: line=(d1,q1,d1,q2);
   mainLine: line=(d2,sb1,d2,sb2,d2,fodo,fodo);

   auxLine1: line=(d3,sb1,d1,sb2,d1,fodo,d1);
   auxLine2: line=(d1,sb1,d1,sb2,d1,fodo,d1);
   auxLine3: line=(fodo);

   use, mainLine;

   beam, particle="e-",
         energy=3*GeV;

   auxLine1Place: placement, sequence = "auxLine1",
                             referenceElement = "d2" ,
			     referenceElementNumber = 2,
			     x = -5*cm,
			     z = -1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = -0.2;

   auxLine2Place: placement, sequence = "auxLine2",
                             referenceElement = "d2",
			     referenceElementNumber = 2,
			     x = -10*cm,
			     z = -1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = -0.5;

   auxLine3Place: placement, sequence = "auxLine3",
                             x = 1*m,
			     axisAngle = 1,
			     axisY = 1,
			     angle = 0.2;

Firstly, a series of simple elements are defined (drifts, quadrupoles and bends). A simple
sequence called `fodo` is defined and also the main beam line, called `mainLine`. After this,
extra sequences are defined that we will use for secondary beam lines.  The `use` command
selects which beam line the simulation will be based on. ::

  use, mainLine;

After this, the beam is defined (required for any simulation for rigidity calculations),
then the placement of secondary beam lines.

The first placement `auxLine1Place` is a placement that will place the sequence named
`auxLine1` with respect to the third instance of the element `d2` in the primary sequence
(`mainLine`). ::

  auxLine1Place: placement, sequence="auxLine1",
                            referenceElement="d2" ,
			    referenceElementNumber=2,

The placement is generally with respect to the centre of the element described in the primary
beam line and along the direction it's pointing. Without any displacement, the geometry
would therefore overlap.  Here, an offset and rotation are specified for this particular placement.
An offset in `x` of -5 cm and -1 m in `z` is specified. The coordinate system is right-handed
with positive-z pointing along the direction of travel in the beam line. A negative-x
displacement is therefore to the right, looking along the direction of travel and 1 m in
`z` is towards the beginning of the element from the centre.  Rotations are described
in :ref:`placements`. Here, an axis angle rotation is used. The beam line is rotated about
the unit Y-axis (local to that element) by -0.2 rad.

The second placement uses a different sequence, but in a similar fashion.

The third placement doesn't specify a `referenceElement`, so the placement is with respect
to the beginning of the beam line.

The model is shown below.

.. figure:: figures/multiple_beamlines.png
	    :width: 90%
	    :align: center

The drift segments do not of course connect but are merely placed close to each other.
In future, continuous vacuum points will be provided.

.. figure:: figures/multiple_beamlines_junction.png
	    :width: 90%
	    :align: center


.. _detectors:

Detectors
---------

BDSIM allows the definition of detectors in the simulation. These are objects to represent
real detectors or measure a quantity at some place in the model including the effect of the
mass of (and therefore interaction with) the detector. Currently, there is one type of detector
that is a beam loss monitor.

.. _detectors-blms:

Beam Loss Monitors
^^^^^^^^^^^^^^^^^^

Beam loss monitors (BLMs) are simple detectors that give one integrated value per event. The
quantity to be generated can be chosen and the shape and location of the BLM can also be chosen.
Below are the available parameters. A BLM is created using the `blm` command.::

  detectorname, blm, parameter=value...

Either a simple geometric shape can be used, which is a single volume of one material, or a
user supplied geometry file can be used.

The placement parameters are the same as the general placements (see :ref:`placements`). So the
BLM can be placed with respect to a beam line element or generally in curvilinear coordinates, or
in global Cartesian coordinates.
  
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
| geometryFile            | Optional file to use for geometry of BLM including format          |
+-------------------------+--------------------------------------------------------------------+
| geometryType            | Name of simple geometry to use ("cylinder", "cube", "sphere")      |
+-------------------------+--------------------------------------------------------------------+
| blmMaterial             | Name of material to use for simple geometry                        |
+-------------------------+--------------------------------------------------------------------+
| blm1                    | BLM shape parameter 1 - different depending on the shape used      |
+-------------------------+--------------------------------------------------------------------+
| blm2                    | BLM shape parameter 2                                              |
+-------------------------+--------------------------------------------------------------------+
| blm3                    | BLM shape parameter 2                                              |
+-------------------------+--------------------------------------------------------------------+
| blm4                    | BLM shape parameter 2                                              |
+-------------------------+--------------------------------------------------------------------+

BLM Shapes
**********

For each shape, the shape parameters ("blm1", "blm2", "blm3", "blm4") have different meanings. These
are described below. NA means non-applicable to this shape.

+-----------------+--------------------------+-----------------------+-----------------------+---------------+
| **Shape**       | **blm1**                 | **blm2**              | **blm3**              | **blm4**      |
+=================+==========================+=======================+=======================+===============+
| cylinder        | half length along axis   | radius                | NA                    | NA            |
+-----------------+--------------------------+-----------------------+-----------------------+---------------+
| cube            | half length in x         | half length in y      | half length in z      | NA            |
+-----------------+--------------------------+-----------------------+-----------------------+---------------+
| sphere          | radius                   | NA                    | NA                    | NA            |
+-----------------+--------------------------+-----------------------+-----------------------+---------------+

Examples
********

1) A simple sphere made of nitrogen. It's placed at 2.3 metres along the beam line with a transverse offset
   (in curvilinear coordinates) of 40 cm horizontally and 25 cm vertically. The sphere radius is 20 cm.

   ::

      minidetector: blm, s=2.3*m, x=0.4*m, y=0.25*m,
	      	    geometryType="sphere",
		    blmMaterial="N",
		    blm1=20*cm;

2) A simple cylinder made of silicon. It's placed globally with an offset in x of 3.2 m and y of 25 cm.

   ::

      minidetector: blm, x=3.2*m, y=0.25*m,
	      	    geometryType="cylinder",
		    blmMaterial="Si",
		    blm1=20*cm,
		    blm2=5*cm;

3) User defined geometry in a GDML file.

   ::

      minidetector: blm, x=0.4*m, y=0.25*m,
                    geometryFile="gdml:simpleshape.gdml",
		    blmMaterial="N",
		    blm1=20*cm,
		    blm2=5*cm;

