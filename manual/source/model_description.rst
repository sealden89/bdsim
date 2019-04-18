.. macro for non breaking white space usefulf or units:
.. |nbsp| unicode:: 0xA0
   :trim:

.. _model-description:

================================
Model Description - Input Syntax
================================

The following sections describe how to prepare a BDSIM model. These sections are
provided in order of requirement.

* Basic Language
  
  - :ref:`lattice-description`
  - :ref:`gmad-syntax`
  - :ref:`mathematical-functions`
  - :ref:`coordinates-and-units`
    
* :ref:`circular-machines`
* :ref:`lattice-elements`
  
  - :ref:`aperture-parameters`
  - :ref:`magnet-geometry-parameters`
  - :ref:`offsets-and-tilts`
  - :ref:`cavity-geometry-parameters`
    
* External Fields & Geometry
  
  - :ref:`field-maps`
  - :ref:`externally-provided-geometry` Formats
  - :ref:`placements` of Geometry
    
* Sequence of Elements
  
  - :ref:`lattice-sequence`

* :ref:`sampler-output`

  - :ref:`sampler-syntax`
  - :ref:`sampler-dimensions`
  - :ref:`sampler-visualisation`
  - :ref:`user-sampler-placement`
    
* :ref:`physics-processes`

  - :ref:`physics-modular-physics-lists`
  - :ref:`physics-geant4-lists`
  - :ref:`physics-complete-lists`

* :ref:`physics-biasing`
    
* :ref:`bdsim-options`
* :ref:`beam-parameters`
* Advanced Topics
  
  - :ref:`tunnel-geometry`
  - :ref:`materials-and-atoms`
  - :ref:`crystals`
  - :ref:`regions`
  - More details about :ref:`bend-tracking-behaviour`
  - :ref:`colours`
  - :ref:`controlling-simulation-speed`
  - :ref:`multiple-beamlines`

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

These are described in the following sections. Aside from these standard parameters, more
detail may be added to the model through:

 * :ref:`magnet-geometry-parameters`.
 * Custom :ref:`field-maps`.
 * Adding :ref:`externally-provided-geometry`.
 * :ref:`offsets-and-tilts`.

.. _gmad-syntax:

GMAD Syntax
-----------

GMAD is a language specifically for BDSIM that is made to be human readable.
The name comes from the design intention of MADX syntax and extensions for Geant4.
While GMAD is very similar to MADX, not all MADX commands are supported.

* S.I. units are used except where explicitly specified
* Variables can be defined using :code:`name = value;` syntax
* Arithmetic expressions can be defined
* Binary operators +, -, \*, /, ^, are valid
* Unary operators +, -, are valid
* Boolean operators <, >, <=, >=, <> (not equal), ==, are valid
* Every expression **must** end with a semi-colon;
* No variable name can begin with a number
* !Comments start with an exclamation mark "!"
* A variable may inherit values (via copy) from another variable using :code:`newvariable : existingvariable;`

.. _mathematical-functions:
  
Mathematical Functions
^^^^^^^^^^^^^^^^^^^^^^

The following mathematical functions are provided:

* sqrt
* cos
* sin
* tan
* exp
* log
* acos
* asin
* atan
* abs

Other Commands
^^^^^^^^^^^^^^

* :code:`print;` Prints all elements
* :code:`print, line;` Prints all elements that are in the beam line defined by :code:`use`. See also `use - Defining which Line to Use`_
* :code:`print, option;` Prints the value of some options
* :code:`print, variable;` Prints the value of a numerical variable, which could be your own defined variable
* :code:`length = d1["l"];` A way to access properties of elements, in this case, length of element d1.
* :code:`stop;` or :code:`return;` Exists parser
* :code:`if () {};` 'if' construct
* :code:`if () {} else {};` 'if-else' construct
* :code:`include ../some/other/file.gmad;` Includes another file to be parsed. Note that the path provided must be relative, not absolute.

Examples
^^^^^^^^

Examples: ::

   x = 1;
   y = 2.5-x;
   z = sin(x) + log(y) - 8e5;
   mat = "copper";


Common Pitfalls
^^^^^^^^^^^^^^^

The following is an example of a common mistake that's not easy to spot: ::

  beam, particle="e-",
        energy=3*TeV;
	E0=1*TeV;

With this syntax we expect to create a design beam of 3 TeV electrons but the central energy of 1 TeV for the bunch. 3 TeV
is used to calculate the magnet strengths and it's expected to fire a 1 TeV electron. However, the E0 parameter here just
defines a variable called E0 that isn't used. The indentation (whitespace) is ignored. The error is the semi-colon at the
end of the second line. This is the correct version: ::

  beam, particle="e-",
        energy=3*TeV,
	E0=1*TeV;

   
.. _coordinates-and-units:
   
Coordinates & Units
-------------------

In Geant4, global Euclidean coordinates are used for tracking purposes. However,
in describing a lattice with BDSIM, curvilinear coordinates are used, as is common with
accelerators (X,Y,S).

**GMAD uses SI units**

==============================  =========================
Name                            Units
==============================  =========================
length                          [m] (metres)
time                            [s] (seconds)
angle                           [rad] (radians)
quadrupole coefficient          [m :math:`^{-2}` ]
multipole coefficient 2n poles  [m :math:`^{-n}` ]
electric voltage                [V] (Volts)
electric field strength         [V/m]
particle energy                 [GeV]
particle mass                   [GeV/c :math:`^2` ]
particle momentum               [GeV/c :math:`^2` ]
beam current                    [A] (Amperes)
particle charge                 [e] (elementary charges)
emittance                       [pi m mrad]
density                         [g/cm :math:`^{3}` ]
temperature                     [K] (Kelvin)
pressure                        [atm] (atmosphere)
frequency                       [Hz] (Hertz)
mass number                     [g/mol]
laser pulse energy              J
==============================  =========================

Some useful predefined values / units are:

==========  =================================
Name        Value
==========  =================================
pi          3.14159265358979
degrees     :math:`\pi` / 180
GeV         1
eV          :math:`10^{-9}`
keV         :math:`10^{-6}`
MeV         :math:`10^{-3}`
TeV         :math:`10^{3}`
PeV         :math:`10^{6}`
pJ          :math:`10^{-12}`
nJ          :math:`10^{-9}`
uJ          :math:`10^{-6}`
mJ          :math:`10^{-3}`
J           1
kJ          :math:`10^{3}`
MJ          :math:`10^{6}`
GJ          :math:`10^{9}`
PJ          :math:`10^{12}`
V           1
kV          :math:`10^{3}`
MV          :math:`10^{6}`
Tesla       1
T           1
rad         1
mrad        :math:`10^{-3}`
urad        :math:`10^{-6}`
clight      :math:`2.99792458 \times 10^{8}`
km          :math:`10^{3}`
m           1
cm          :math:`10^{-2}`
mm          :math:`10^{-3}`
um          :math:`10^{-6}`
mum         :math:`10^{-6}`
nm          :math:`10^{-9}`
ang         :math:`10^{-10}`
pm          :math:`10^{-12}`
s           1
ms          :math:`10^{-3}`
us          :math:`10^{-6}`
ns          :math:`10^{-9}`
ps          :math:`10^{-12}`
Hz          1
kHz         :math:`10^{3}`
MHz         :math:`10^{6}`
GHz         :math:`10^{9}`
==========  =================================

As an example, one can write either :code:`100*eV` or :code:`0.1*keV` to specify an energy value in GMAD.
Both are equivalent.

.. _circular-machines:

Circular Machines
-----------------

To simulate circular machines, BDSIM should be executed with the :code:`--circular` executable option
(see :ref:`executable-options`). This installs special beam line
elements called the `teleporter` and `terminator` at the end of the lattice that are described
below.

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

.. _one-turn-map:

One Turn Map
^^^^^^^^^^^^

Geant4 mandates that there are no overlaps between solids, which in
BDSIM means that a thin 1 |nbsp| nm gap is placed between each lattice
element.  Whilst these thin gaps have a negligible effect for a single
pass or turn, over several turns it introduces a sizeable inaccuracy
in the tracking (in the context of large circular models).
To correct for this, BDSIM models can be supplemented
with a one turn map which is applied at the end of each turn to right
the primary back onto the correct trajectory.  To ensure physical
results the one turn map is only applied to primaries, if they did not
interact on the previous turn, and if they are within 5% of the
reference momentum.  The one turn map is also not applied on the first
turn where there the beam is offset in S, but applied on following
turns, still accounting for the exceptions mentioned above.

The map must be of the format as written by MADX-PTC's ``PTC_NORMAL``
command.  A one turn map (in this case, 12th order) can be generated
in MAD-X with the following ::

  PTC_CREATE_UNIVERSE;
  PTC_CREATE_LAYOUT, model=2,method=6,nst=10, exact=true, resplit, xbend;
  PTC_NORMAL,maptable,icase=5,no=12;
  write, table="map_table", file="my_oneturn_map_file";
  PTC_END;

To use then use the one turn map with BDSIM ::

  option, ptcOneTurnMapFileName="path/to/my_oneturn_map_file";


* This can only be used with circular machines.


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

.. note:: BDSIM strictly follows the MADX definition of magnet strength parameter
	  `k` - a **positive** `k` corresponds to **horizontal focussing** for a
	  **positively** charged particle. This therefore indicates a positive `k`
	  corresponds to horizontal defocussing for a negatively charged particle.
	  However, MADX treats all particles as positively charged for tracking purposes.

.. warning:: BDSIM currently treats k absolutely, so to convert a MADX lattice for
	     negatively particles, the MADX k values must be multiplied by -1. The
	     pybdsim converter provides an option called `flipmagnets` for this
	     purpose. This may be revised in future releases depending on changes
	     to MADX.


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

* The `aperture parameters`_ may also be specified.

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
| `h1`            | input poleface curvature          | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h2`            | output poleface curvature         | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+

Notes:

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

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
   set by the option `thinElementLength` (see `options`_).
5) In the case of finite `fint` or `fintx` and `hgap`, a fringe field is used even
   if `e1` and `e2` have no angle.
6) The `fintK2` and `fintxK2` parameters are for a second fringe field correction term that are included to
   enable optics comparisons with TRANSPORT. Whilst this term is not available in MAD-X, the default values
   of 0 mean this second fringe field correction will not be applied unless `fintK2` or `fintxK2` are
   explicitly specified as non-zero.
7) The effect of pole face rotations and fringe field kicks can be turned off for all dipoles by setting
   the option `includeFringeFields=0` (see `options`_).
8) The poleface curvature does not construct the curved geometry. The effect is instead applied in the thin
   fringefield magnet.
9) Rbends are limited in angle to less than pi/2.

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
| `h1`            | input poleface curvature          | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+
| `h2`            | output poleface curvature         | 0         | no              |
|                 | :math:`[m^{-1}]`                  |           |                 |
+-----------------+-----------------------------------+-----------+-----------------+

Notes:

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.

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
   set by the option `thinElementLength` (see `options`_).
4) In the case of finite `fint` or `fintx` and `hgap` a fringe field is used even
   if `e1` and `e2` have no angle.
5) The `fintK2` and `fintxK2` parameters are for a second fringe field correction term that are included to
   enable optics comparisons with TRANSPORT. Whilst this term is not available in MAD-X, the default values
   of 0 mean this second fringe field correction will not be applied unless `fintK2` or `fintxK2` are
   explicitly specified as non-zero.
6) The effect of pole face rotations and fringe field kicks can be turned off for all dipoles by setting
   the option `includeFringeFields=0` (see `options`_).
7) The poleface curvature does not construct the curved geometry. The effect is instead applied in the thin
   fringefield magnet.
8) Sbends are limited in angle to less than 2 pi. If the sbends are not split with the option dontSplitSBends,
   an sbend will be limited in angle to a maximum of pi/2.

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

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
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

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
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

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
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

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.

Examples: ::

   MXDEC3: decapole, l=0.3*m, k4=32.9;

multipole
^^^^^^^^^

`multipole` defines a general multipole magnet. The strength parameter
:math:`knl` is a list defined as

.. math::

   knl[n] = \frac{1}{B \rho} \frac{dB^{n}_{y}}{dx^{n}}\,[m^{-(n+1)}]

starting with the quadrupole component. The skew strength parameter :math:`ksl`
is a list representing the skew coefficients.

================  ===========================  ==========  ===========
Parameter         Description                  Default     Required
`l`               Length [m]                   0           Yes
`knl`             List of normal coefficients  0           No
`ksl`             List of skew coefficients    0           No
`material`        Magnet outer material        Iron        No
================  ===========================  ==========  ===========

Notes:

* The `aperture parameters`_ may also be specified.
* The `magnet geometry parameters`_ may also be specified.
* See `Magnet Strength Polarity`_ for polarity notes.
* No yoke field is provided.

Examples: ::

   OCTUPOLE1 : multipole, l=0.5*m , knl={ 0,0,1 } , ksl={ 0,0,0 };

thinmultipole
^^^^^^^^^^^^^

`thinmultipole` is the same as multipole, but is set to have a default length of 1 micron.
For thin multipoles, the length parameter is not required. The element will appear as a thin length of drift
tube. A thin multipole can be placed next to a bending magnet with finite pole face rotation angles.

Examples: ::

   THINOCTUPOLE1 : thinmultipole , knl={ 0,0,1 } , ksl={ 0,0,0 };

.. note:: The length of the thin multipole can be changed by setting `thinElementLength` (see `options`_).

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


* The `aperture parameters`_ may also be specified.
* For a vkicker with a finite length, the `magnet geometry parameters`_ may also be specified.

.. note:: Pole face rotations and fringe fields can be applied to vkickers by supplying the same
      parameters that would be applied to an `rbend`_ or `sbend`_ . If the vkicker is zero length,
      the B field value must be supplied in order to calculate the bending radius which required
      to apply the effects correctly.

* Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the
  beginning or at the end of the vkicker. The length of the fringe field element can be set by the
  option `thinElementLength` (see `options`_).
* For zero length vkickers, the pole face and fringe field kicks are applied in the same thin element
  as the vkick.
* In the case of finite fint or fintx and hgap, a fringe field is used even if e1 and e2 have no angle.
* The effect of pole face rotations and fringe field kicks can be turned off for all magnets by setting
  the option `includeFringeFields=0` (see `options`_).
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

* The `aperture parameters`_ may also be specified.
* For a hkicker with a finite length, the `magnet geometry parameters`_ may also be specified.

.. note:: Pole face rotations and fringe fields can be applied to hkickers by supplying the same
      parameters that would be applied to an `rbend`_ or `sbend`_ . If the hkicker is zero length,
      the B field value must be supplied in order to calculate the bending radius which required
      to apply the effects correctly.

* Fringe field kicks are applied in a thin fringe field magnet (1 micron thick by default) at the
  beginning or at the end of the hkicker. The length of the fringe field element can be set by the
  option `thinElementLength` (see `options`_).
* For zero length hkickers, the pole face and fringe field kicks are applied in the same thin element
  as the hkick.
* In the case of finite fint or fintx and hgap, a fringe field is used even if e1 and e2 have no angle.
* The effect of pole face rotations and fringe field kicks can be turned off for all magnets by setting
  the option `includeFringeFields=0` (see `options`_).
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

BDSIM, like MADX, provides a `tkicker` element. This is an alias in BDSIM for a `kicker`_,
however MADX differentiates the two on the basis of fitting parameters. BDSIM does
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
numerical integrator is used to calculate the motion of particles in both cases.


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

If `tOffset` is specified, a phase offset is calculated from this time for the **speed
of light in a vacuum**. Otherwise, the curvilinear S-coordinate of the centre of the rf
element is used to find the phase offset.

.. note:: As the phase offset is calculated from the speed of light in a vacuum, this is
	  only correct for already relativisitic beams. Development is underway to improve
	  this calculation for sub-relativisitic beams.

If `phase` is specified, this is added to the calculated phase offset from either the lattice
position or `tOffset`.

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
half-axes respectively. When tapered, the ratio between the horizontal and vertical half-
axes of the entrance aperture must be the same ratio for the exit aperture.

* All the same conditions for `rcol` apply for `ecol`.

jcol
^^^^

.. figure:: figures/jcol.png
	    :width: 40%
	    :align: center


`jcol` defines a jaw collimator with two square blocks on either side in the horizontal plane.
If a vertical `jcol` is required, the `tilt` parameter should be used to rotate it by :math:`\pi/2`.
The horizontal position of each jaw can be set separately with the `xsizeLeft` and `xsizeRight`
apertures which are the distances from the center of element to the left and right jaws respectively.


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

* The `aperture parameters`_ may also be specified.

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
  d2: dump, l=0.4*m, apertureType="circular";
  d3: dump, l=0.3*m, apertureType="rectangular";


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
`angle`                angle of the wire w.r.t. vertical                0           no
`wireOffsetX`          x offset of the wire from the center [m]         0           no
`wireOffsetY`          y offset of the wire from the center [m]         0           no
`wireOffsetZ`          z offset of the wire from the center [m]         0           no
`wireMaterial`         material of wire                                 carbon      no
=====================  ===============================================  ==========  ==========

Notes:

* The angle is the rotation from vertical in the clock-wise direction looing in the
  positive S direction (the usualy direction of the beam).

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
* The `aperture parameters`_ may also be specified.
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

`geometryFile` should be of the format `format:filename`, where `format` is the geometry
format being used (`gdml` | `gmad` | `mokka`) and filename is the path to the geometry
file. See :ref:`externally-provided-geometry` for more details.

`fieldAll` should refer to the name of a field object the user has defined in the input
gmad file. The syntax for this is described in :ref:`field-maps`.

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

.. _aperture-parameters:

Aperture Parameters
-------------------

For elements that contain a beam pipe, several aperture models can be used. These aperture
parameters can be set as the default for every element using the :code:`option` command
(see `options`_ ) and
can be overridden for each element by specifying them with the element definition. The aperture
is controlled through the following parameters:

* `apertureType`
* `beampipeRadius` or `aper1`
* `aper2`
* `aper3`
* `aper4`
* `vacuumMaterial`
* `beampipeThickness`
* `beampipeMaterial`


For each aperture model, a different number of parameters are required. Here, we follow the MADX
convention and have four parameters. The user must specify them as required for that model.
BDSIM will check to see if the combination of parameters is valid. `beampipeRadius` and `aper1`
are degenerate.

Up to four parameters
can be used to specify the aperture shape (*aper1*, *aper2*, *aper3*, *aper4*).
These are used differently for each aperture model and match the MADX aperture definitions.
The required parameters and their meaning are given in the following table.

+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| Aperture Model    | # of         | `aper1`           | `aper2`         | `aper3`        | `aper4`          |
|                   | parameters   |                   |                 |                |                  |
+===================+==============+===================+=================+================+==================+
| `circular`        | 1            | radius            | NA              | NA             | NA               |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `rectangular`     | 2            | x half-width      | y half-width    | NA             | NA               |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `elliptical`      | 2            | x semi-axis       | y semi-axis     | NA             | NA               |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `lhc`             | 3            | x half-width of   | y half-width of | radius of      | NA               |
|                   |              | rectangle         | rectangle       | circle         |                  |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `lhcdetailed`     | 3            | x half-width of   | y half-width of | radius of      | NA               |
|                   |              | rectangle         | rectangle       | circle         |                  |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `rectellipse`     | 4            | x half-width of   | y half-width of | x semi-axis    | y semi-axis      |
|                   |              | rectangle         | rectangle       | of ellipse     | of ellipse       |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `racetrack`       | 3            | horizontal offset | vertical offset | radius of      | NA               |
|                   |              | of circle         | of circle       | circular part  |                  |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `octagonal`       | 4            | x half-width      | y half-width    | x point of     | y point of       |
|                   |              |                   |                 | start of edge  | start of edge    |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+
| `clicpcl`         | 4            | x half-width      | top ellipse     | bottom ellipse | y separation     |
|                   |              |                   | y half-height   | y half-height  | between ellipses |
+-------------------+--------------+-------------------+-----------------+----------------+------------------+

These parameters can be set with the *option* command, as the default parameters
and also on a per element basis that overrides the defaults for that specific element.

In the case of `clicpcl` (CLIC Post Collision Line), the beam pipe is asymmetric. The centre is
the same as the geometric centre of the bottom ellipse. Therefore, *aper4*, the y separation
between ellipses is added on to the 0 position. The parameterisation is taken from
Phys. Rev. ST Accel. Beams **12**, 021001 (2009).

.. _magnet-geometry-parameters:

Magnet Geometry Parameters
--------------------------

As well as the beam pipe, magnet beam line elements also have further outer geometry beyond the
beam pipe. This geometry typically represents the magnetic poles and yoke of the magnet but there
are several geometry types to choose from. The possible different styles are described below and
syntax **examples** can be found in *examples/features/geometry/4_magnets/*.

* Externally provided geometry can also be wrapped around the beam pipe (detailed below).

The magnet geometry is controlled by the following parameters.

.. note:: These can all be specified using the `option` command as well as on a per element
	  basis, but in this case they act as a default that will be used if none are
	  specified by the element.

.. note:: The option :code:`ignoreLocalMagnetGeometry` exists and if it is true (1), any
	  per-element magnet geometry definitions will be ignored and the ones specified
	  in Options will be used.

+-----------------------+--------------------------------------------------------------+---------------+-----------+
| Parameter             | Description                                                  | Default       | Required  |
+=======================+==============================================================+===============+===========+
| `magnetGeometryType`  | | The style of magnet geometry to use. One of:               | `polessquare` | No        |
|                       | | `cylindrical`, `polescircular`, `polessquare`,             |               |           |
|                       | | `polesfacet`, `polesfacetcrop`, `lhcleft`, `lhcright`,     |               |           |
|                       | | `none` and `format:path`.                                  |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `horizontalWidth`     | | **Full** horizontal width of the magnet (m)                | 0.6 m         | No        |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `outerMaterial`       | |  Material of the magnet                                    | "iron"        | No        |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `yokeOnInside`        | | Whether the yoke of a dipole appears on the inside of the  | 1             | No        |
|                       | | bend and if false, it's on the outside. Applicable only    |               |           |
|                       | | to dipoles.                                                |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `hStyle`              | | Whether a dipole (only a dipole) will be an H style one    | 0             | No        |
|                       | | or a C style one (c style by default. True ('1') or False  |               |           |
|                       | | ('0').                                                     |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `vhRatio`             | | The vertical to horizontal ratio of a magnet. The width    | 0.8           | No        |
|                       | | will always be the `horizontalWidth` and the height will   |               |           |
|                       | | scale according to this ratio. In the case of a vertical   |               |           |
|                       | | kicker it will be the height that is `horizontalWidth` (as |               |           |
|                       | | the geometry is simply rotated). Ranges from 0.1 to 10.    |               |           |
|                       | | This currently **only** applies to dipoles with poled      |               |           |
|                       | | geometry.                                                  |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `coilWidthFraction`   | | Fraction of the available horizontal space between the     | 0.9           | No        |
|                       | | pole and the yoke for dipole geometry that the coil will   |               |           |
|                       | | occupy. This currently only applies to dipoles with poled  |               |           |
|                       | | geometry. Ranges from 0.05 to 0.98.                        |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+
| `coilHeightFraction`  | | Fraction of the available vertical space between the pole  | 0.9           | No        |
|                       | | tip and the yoke for dipole geometry that the coil will    |               |           |
|                       | | occupy. This currently only applies to dipoles with poled  |               |           |
|                       | | geometry. Ranges from 0.05 to 0.98.                        |               |           |
+-----------------------+--------------------------------------------------------------+---------------+-----------+

Examples: ::

  option, magnetGeometryType = "polesfacetcrop",
          horizontalWidth = 0.5*m;

::

   m1: quadrupole, l=0.3*m,
                   k1=0.03,
		   magnetGeometryType="gdml:geometryfiles/quad.gdml",
		   horizontalWidth = 0.5*m;

.. warning:: The choice of magnet outer geometry will significantly affect the beam loss pattern in the
	     simulation, as particles and radiation may propagate much further along the beam line when
	     a magnet geometry with poles is used.

.. note:: Should a custom selection of various magnet styles be required for your simulation, please
	  contact us (see :ref:`feature-request`) and this can be added - it is a relatively simple process.

No Magnet Outer Geometry - "`none`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

No geometry for the magnet outer part is built at all and nothing is placed in the model. This results
in only a beam pipe with the correct fields being provided.

.. image:: figures/none_beamline.png
	   :width: 60%
	   :align: center

Cylindrical - "`cylindrical`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The beam pipe is surrounded by a cylinder of material (the default is iron) whose outer diameter
is controlled by the `horizontalWidth` parameter. In the case of beam pipes that are not circular
in cross-section, the cylinder fits directly against the outside of the beam pipe.

This geometry is useful when a specific geometry is not known. The surrounding
magnet volume acts to produce secondary radiation as well as act as material for energy deposition,
therefore this geometry is best suited for the most general studies.

.. figure:: figures/cylindrical_quadrupole.png
	    :width: 40%

.. figure:: figures/cylindrical_sextupole.png
	    :width: 40%


Poles Circular - "`polescircular`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry has simple iron poles according to the order of the magnet and the yoke is
represented by an annulus. Currently no coils are implemented. If a non-symmetric beam pipe
geometry is used, the larger of the horizontal and vertical dimensions of the beam pipe will be
used to create the circular aperture at the pole tips.

.. figure:: figures/polescircular_quadrupole.png
	    :width: 40%

.. figure:: figures/polescircular_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polescircular_sextupole.png
	    :width: 40%

.. figure:: figures/polescircular_sextupole_3d.png
	    :width: 40%


Poles Square (Default) - "`polessquare`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry has again, individual poles according to the order of the magnet but the
yoke is an upright square section to which the poles are attached. This geometry behaves in the
same way as `polescircular` with regard to the beam pipe size.

`horizontalWidth` is the full width of the magnet horizontally as shown in the figure below,
**not** the diagonal width.

.. figure:: figures/polessquare_quadrupole.png
	    :width: 40%

.. figure:: figures/polessquare_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polessquare_sextupole.png
	    :width: 40%

.. figure:: figures/polessquare_sextupole_3d.png
	    :width: 40%


Poles Faceted - "`polesfacet`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry is much like `polessquare`; however, the yoke is such that the pole always
joins at a flat piece of yoke and not in a corner. This geometry behaves in the
same way as `polescircular` with regards to the beam pipe size.

`horizontalWidth` is the full width as shown in the figure.

.. figure:: figures/polesfacet_quadrupole.png
	    :width: 40%

.. figure:: figures/polesfacet_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polesfacet_sextupole.png
	    :width: 40%

.. figure:: figures/polesfacet_sextupole_3d.png
	    :width: 40%


Poles Faceted with Crop - "`polesfacetcrop`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This magnet geometry is quite similar to `polesfacet`, but the yoke in between each
pole is cropped to form another facet. This results in the magnet geometry having
double the number of poles as sides.

`horizontalWidth` is the full width horizontally as shown in the figure.

.. figure:: figures/polesfacetcrop_quadrupole.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_quadrupole_3d.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_sextupole.png
	    :width: 40%

.. figure:: figures/polesfacetcrop_sextupole_3d.png
	    :width: 40%


LHC Left & Right - "`lhcleft`" | "`lhcright`"
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

`lhcleft` and `lhcright` provide more detailed magnet geometry appropriate for the LHC. Here, the
left and right suffixes refer to the shift of the magnet body with respect to the reference beam line.
Therefore, `lhcleft` has the magnet body shifted to the left in the direction of beam travel and the
'active' beam pipe is the right one. Vice versa for the `lhcright` geometry.

For this geometry, only the `sbend` and `quadrupole` have been implemented.  All other magnet geometry
defaults to the cylindrical set.

This geometry is parameterised to a degree regarding the beam pipe chosen.  Of course, parameters similar
to the LHC make most sense, as does use of the `lhcdetailed` aperture type. Examples are shown with various
beam pipes and both `sbend` and `quadrupole` geometries.


.. |lhcleft_sbend| image:: figures/lhcleft_sbend.png
			   :width: 60%

.. |lhcleft_quadrupole| image:: figures/lhcleft_quadrupole.png
				:width: 60%

.. |lhcleft_quadrupole_square| image:: figures/lhcleft_quadrupole_square.png
				       :width: 60%

.. |lhcleft_sextupole| image:: figures/lhcleft_sextupole.png
			       :width: 60%

+-----------------------------+-----------------------+
| |lhcleft_sbend|             | |lhcleft_quadrupole|  |
+-----------------------------+-----------------------+
| |lhcleft_quadrupole_square| | |lhcleft_sextupole|   |
+-----------------------------+-----------------------+

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
	  matches the behaviour of MAD8 and MADX.

.. note:: A right-handed coordinate system is used and the beamline is built along the `z` direction.

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


.. _cavity-geometry-parameters:

Cavity Geometry Parameters
--------------------------

A more detailed rf cavity geometry may be described by constructing a 'cavity' object
in gmad and attaching it by name to an element.  The following parameters may be added
to a cavity object:

.. tabularcolumns:: |p{3cm}|p{2cm}|p{5cm}|

+--------------------------+-----------------+-----------------------------------------------------------------+
| **Parameter**            | **Required**    | **Description**                                                 |
+==========================+=================+=================================================================+
| `name`                   | Yes             | Name of the object                                              |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `type`                   | Yes             | (elliptical | rectangular | pillbox)                            |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `material`               | Yes             | The material for the cavity                                     |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisRadius`             | No              | The radius of the narrowest part                                |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorRadius`          | No              | The radius of the widest part                                   |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `halfCellLength`         | No              | Half-length along a cell                                        |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorHorizontalAxis`  | Elliptical only | Horizontal semi-axis of the ellipse at the cavity equator       |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `equatorVerticalAxis`    | Elliptical only | Vertical semi-axis of the ellipse at the cavity equator         |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisHorizontalAxis`     | Elliptical only | Horizontal semi-axis of the ellipse at the iris                 |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `irisVerticalAxis`       | Elliptical only | Vertical semi-axis of the ellipse at the iris                   |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `tangentLineAngle`       | Elliptical only | Angle to the vertical line connecting two ellipses              |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `thickness`              | No              | Thickness of material                                           |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `numberOfPoints`         | No              | Number of points to generate around 2 :math:`\pi`.              |
+--------------------------+-----------------+-----------------------------------------------------------------+
| `numberOfCells`          | No              | Number of cells to construct                                    |
+--------------------------+-----------------+-----------------------------------------------------------------+

Example::

  shinyCavity: cavity, type="elliptical",
                       irisRadius = 35*mm,
	               equatorRadius = 103.3*mm,
	               halfCellLength = 57.7*mm,
		       equatorHorizontalAxis = 40*mm,
		       equatorVerticalAxis = 42*mm,
	               irisHorizontalAxis = 12*mm,
	               irisVerticalAxis = 19*mm,
	               tangentLineAngle = 13.3*pi/180,
	               thickness = 1*mm,
	               numberOfPoints = 24,
	               numberOfCells = 1;

.. figure:: figures/elliptical-cavity.pdf
	   :width: 40%
	   :align: center

The parametrisation used to define elliptical cavities in BDSIM.
The symbols used in the figure map to the cavity options according to the table below.

+-----------------------+-----------------------------+
| **Symbol**            | **BDSIM Cavity Parameter**  |
+=======================+=============================+
| :math:`R`             | equatorRadius               |
+-----------------------+-----------------------------+
| :math:`r`             | irisRadius                  |
+-----------------------+-----------------------------+
| :math:`A`             | equatorHorizontalAxis       |
+-----------------------+-----------------------------+
| :math:`B`             | equatorVerticalAxis         |
+-----------------------+-----------------------------+
| :math:`a`             | irisHorizontalAxis          |
+-----------------------+-----------------------------+
| :math:`b`             | irisVerticalAxis            |
+-----------------------+-----------------------------+
| :math:`\alpha`        | tangentLineAngle            |
+-----------------------+-----------------------------+
| :math:`L`             | halfCellLength              |
+-----------------------+-----------------------------+


.. _field-maps:

Fields
------

BDSIM provides the facility to overlay pure magnetic, pure electric or combined electromagnetic fields
on an element, as defined by an externally provided field map. This can be done for 1) only the vacuum
volume; 2) only the volume outside the vacuum (i.e. the yoke); 3) or one full map for the whole
element.  BDSIM allows any Geant4 integrator to be used to calculate the motion of the particle, which
can be chosen given knowledge of the smoothness of the field or the application. BDSIM also provides
a selection of 1-4D interpolators that are used to provide the field value in between the data points
in the supplied field map.

To overlay a field, one must define a field 'object' in the parser and then 'attach' it to an element.

* Fields are in a local Cartesian coordinate system with respect to the origin of the
  element they are attached to.
* The field may be attached to everything "fieldAll"; the vacuum volume "fieldVacuum", or the yoke "fieldOuter".
* Magnetic and electric field maps are specified in separate files and may have different interpolators.
* Fields may have up to four dimensions.

The dimensions are (by default) in order :math:`x,y,z,t`. For example, specifying a 3D field will be
:math:`x,y,z` and a 2D field :math:`x,y`.

For BDSIM format fields (see :ref:`model-description-field-formats`, :ref:`field-map-formats` and
:ref:`fields-different-dimensions`),
the user can however specify different dimension with the other dimensions being assumed constant.
For example, a field that varies in :math:`x,z` is possible (assumed constant in :math:`y`). For
BDSIM format fields, this is detected automatically by the column labelling and the keys in the
header of the file that specify the ranges in each dimension. The dimensions must however be in
ascending or descending order.

.. Note:: Currently only **regular** (evenly spaced) grids are supported with field maps. It would
	  require significant development to extend this to irregular grids. It's strongly
	  recommended the user re-sample any existing field map into a regular grid. A regular
	  grid is also much faster for tracking purposes.

Here is example syntax to define a field object named 'somefield' in the parser and overlay it onto
a drift pipe where it covers the full volume of the drift (not outside it though)::

  somefield: field, type="ebmap2d",
		    eScaling = 3.0,
		    bScaling = 0.4,
		    integrator = "g4classicalrk4",
		    magneticFile = "poisson2d:/Path/To/File.TXT",
		    magneticInterpolator = "nearest2D",
		    electricFile = "poisson2d:/Another/File.TXT",
		    electricInterpolator = "linear2D";

  d1: drift, l=0.5*m, aper1=4*cm, fieldAll="somefield";

Each beam line element will allow "fieldAll", "fieldVacuum" and "fieldOuter" to be specified.

When defining a field, the following parameters can be specified.

.. tabularcolumns:: |p{0.40\textwidth}|p{0.60\textwidth}|

+----------------------+-----------------------------------------------------------------+
| **Parameter**        | **Description**                                                 |
+======================+=================================================================+
| type                 | See type table below.                                           |
+----------------------+-----------------------------------------------------------------+
| eScaling             | A numerical scaling factor that all electric field vectors'     |
|                      | amplitudes will be multiplied by                                |
+----------------------+-----------------------------------------------------------------+
| bScaling             | A numerical scaling factor that all magnetic field vectors'     |
|                      | amplitudes will be multiplied by                                |
+----------------------+-----------------------------------------------------------------+
| integrator           | The integrator used to calculate the motion of the particle     |
|                      | in the field. See below for full list of supported integrators. |
+----------------------+-----------------------------------------------------------------+
| globalTransform      | Boolean. Whether a transform from local curvilinear coordinates |
|                      | to global coordinates should be provided (default true).        |
+----------------------+-----------------------------------------------------------------+
| magneticFile         | "format:filePath" - see formats below.                          |
+----------------------+-----------------------------------------------------------------+
| magneticInterpolator | Which interpolator to use - see below for a full list.          |
+----------------------+-----------------------------------------------------------------+
| electricFile         | "format:filePath" - see formats below.                          |
+----------------------+-----------------------------------------------------------------+
| electricInterpolator | Which interpolator to use - see below for a full list.          |
+----------------------+-----------------------------------------------------------------+
| x                    | x-offset from element it's attached to                          |
+----------------------+-----------------------------------------------------------------+
| y                    | y-offset from element it's attached to                          |
+----------------------+-----------------------------------------------------------------+
| z                    | z-offset from element it's attached to                          |
+----------------------+-----------------------------------------------------------------+
| t                    | t-offset from **Global** t in seconds                           |
+----------------------+-----------------------------------------------------------------+
| phi                  | Euler phi rotation from the element the field is attached to    |
+----------------------+-----------------------------------------------------------------+
| theta                | Euler theta rotation from the element the field is attached to  |
+----------------------+-----------------------------------------------------------------+
| psi                  | Euler psi rotation from the element the field is attached to    |
+----------------------+-----------------------------------------------------------------+
| axisAngle            | (Boolean) Use axis angle rotation variables. Default 0 (Euler). |
+----------------------+-----------------------------------------------------------------+
| axisX                | x-component of axis defining axis / angle rotation              |
+----------------------+-----------------------------------------------------------------+
| axisY                | y-component of axis defining axis / angle rotation              |
+----------------------+-----------------------------------------------------------------+
| axisZ                | z-component of axis defining axis / angle rotation              |
+----------------------+-----------------------------------------------------------------+
| angle                | angle (rad) of defining axis / angle rotation                   |
+----------------------+-----------------------------------------------------------------+
| autoScale            | This automatically calculates the field gradient at the origin  |
|                      | and the field magnitude will be automatically scaled according  |
|                      | to the normalised `k` strength (such as `k1` for a quadrupole)  |
|                      | for the magnet it's attached to. Only applicable for when       |
|                      | attached to magnets.                                            |
+----------------------+-----------------------------------------------------------------+
| maximumStepLength    | The maximum permitted step length through the field. (m)        |
+----------------------+-----------------------------------------------------------------+

.. Note:: Either axis angle (with unit axis 3-vector) or Euler angles can be used to provide
	  the rotation between the element the field maps are attached to and the coordinates
	  of the field map. Use `axisAngle=1` to use the axis angle rotation scheme.

.. Note:: A right-handed coordinate system is used in Geant4, so positive x is out of a ring.

Field Types
^^^^^^^^^^^

* These are not case sensitive.

.. tabularcolumns:: |p{0.40\textwidth}|p{0.60\textwidth}|

+------------------+----------------------------------+
| **Type String**  | **Description**                  |
+==================+==================================+
| bmap1d           | 1D magnetic only field map       |
+------------------+----------------------------------+
| bmap2d           | 2D magnetic only field map       |
+------------------+----------------------------------+
| bmap3d           | 3D magnetic only field map       |
+------------------+----------------------------------+
| bmap4d           | 4D magnetic only field map       |
+------------------+----------------------------------+
| emap1d           | 1D electric only field map       |
+------------------+----------------------------------+
| emap2d           | 2D electric only field map       |
+------------------+----------------------------------+
| emap3d           | 3D electric only field map       |
+------------------+----------------------------------+
| emap4d           | 4D electric only field map       |
+------------------+----------------------------------+
| ebmap1d          | 1D electric-magnetic field map   |
+------------------+----------------------------------+
| ebmap2d          | 2D electric-magnetic field map   |
+------------------+----------------------------------+
| ebmap3d          | 3D electric-magnetic field map   |
+------------------+----------------------------------+
| ebmap4d          | 4D electric-magnetic field map   |
+------------------+----------------------------------+

.. _model-description-field-formats:

Formats
^^^^^^^

.. tabularcolumns:: |p{0.40\textwidth}|p{0.60\textwidth}|

.. note:: BDSIM field maps by default have units :math:`cm,s`.

+------------------+--------------------------------------------+
| **Format**       | **Description**                            |
+==================+============================================+
| bdsim1d          | 1D BDSIM format file  (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim2d          | 2D BDSIM format file  (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim3d          | 3D BDSIM format file  (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| bdsim4d          | 4D BDSIM format file  (Units :math:`cm,s`) |
+------------------+--------------------------------------------+
| poisson2d        | 2D Poisson Superfish SF7 file              |
+------------------+--------------------------------------------+
| poisson2dquad    | 2D Poisson Superfish SF7 file              |
|                  | for 1/8th of quadrupole                    |
+------------------+--------------------------------------------+
| poisson2ddipole  | 2D Poisson Superfish SF7 file for positive |
|                  | quadrant that's reflected to produce a     |
|                  | full windowed dipole field                 |
+------------------+--------------------------------------------+

Field maps in the following formats are accepted:

  * BDSIM's own format (both uncompressed :code:`.dat` and gzip compressed files. :code:`gz` must be
    in the file name for this to load correctly.)
  * Superfish Poisson 2D SF7

These are described in detail below. More field formats can be added
relatively easily - see :ref:`feature-request`. A detailed description
of the formats is given in :ref:`field-map-formats`. A preparation guide
for BDSIM format files is provided here :ref:`field-map-file-preparation`.


Integrators
^^^^^^^^^^^

The following integrators are provided.  The majority are interfaces to Geant4 integrators.
*g4classicalrk4* is typically the recommended default and is very robust.
*g4cakskarprkf45* is similar but slightly less CPU-intensive. For version Geant4.10.4
onwards, *g4dormandprince745* is the default recommended by Geant4 (although not the
BDSIM default currently). Note: any integrator capable of operating on EM fields
will work on solely B- or E-fields.

We recommend looking at the source .hh files in the Geant4 source code for an
explanation of each, as this is where they are documented. The source files can
be found in `<geant4-source-dir>/source/geometry/magneticfield/include`.

+----------------------+----------+------------------+-----------------------------+
|  **String**          | **B/EM** | **Time Varying** | Required Geant4 Version (>) |
+======================+==========+==================+=============================+
| g4cashkarprkf45      | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4classicalrk4       | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4constrk4           | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4expliciteuler      | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4impliciteuler      | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4simpleheum         | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4simplerunge        | EM       | Y                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4exacthelixstepper  | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4helixexpliciteuler | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4helixheum          | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4heliximpliciteuler | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4helixmixedstepper  | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4helixsimplerunge   | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4nystromrk4         | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4rkg3stepper        | B        | N                | 10.0                        |
+----------------------+----------+------------------+-----------------------------+
| g4bogackishampine23  | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4bogackishampine45  | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4dolomcprik34       | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4dormandprince745   | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4dormandprincerk56  | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4dormandprincerk78  | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4tsitourasrk45      | EM       | Y                | 10.3                        |
+----------------------+----------+------------------+-----------------------------+
| g4rk547feq1          | EM       | Y                | 10.4                        |
+----------------------+----------+------------------+-----------------------------+
| g4rk547feq2          | EM       | Y                | 10.4                        |
+----------------------+----------+------------------+-----------------------------+
| g4rk547feq3          | EM       | Y                | 10.4                        |
+----------------------+----------+------------------+-----------------------------+


Interpolators
^^^^^^^^^^^^^

The field may be queried at any point inside the volume, so an interpolator is required
to provide a value of the field in between specified points in the field map.
There are many algorithms that can be used to interpolate the field map data. A
mathematical description of the ones provided in BDSIM as well as example plots
is shown in :ref:`field-interpolators`.

* This string is case-insensitive.

+------------+-------------------------------+
| **String** | **Description**               |
+============+===============================+
| nearest1d  | Nearest neighbour in 1D       |
+------------+-------------------------------+
| nearest2d  | Nearest neighbour in 2D       |
+------------+-------------------------------+
| nearest3d  | Nearest neighbour in 3D       |
+------------+-------------------------------+
| nearest4d  | Nearest neighbour in 4D       |
+------------+-------------------------------+
| linear1d   | Linear interpolation in 1D    |
+------------+-------------------------------+
| linear2d   | Linear interpolation in 2D    |
+------------+-------------------------------+
| linear3d   | Linear interpolation in 3D    |
+------------+-------------------------------+
| linear4d   | Linear interpolation in 4D    |
+------------+-------------------------------+
| cubic1d    | Cubic interpolation in 1D     |
+------------+-------------------------------+
| cubic2d    | Cubic interpolation in 2D     |
+------------+-------------------------------+
| cubic3d    | Cubic interpolation in 3D     |
+------------+-------------------------------+
| cubic4d    | Cubic interpolation in 4D     |
+------------+-------------------------------+

.. _externally-provided-geometry:

Externally Provided Geometry
----------------------------

BDSIM provides the ability to use externally provided geometry in the Geant4 model constructed
by BDSIM. A variety of formats are supported (see :ref:`geometry-formats`). External
geometry can be used in three ways:

1) A placement of a piece of geometry unrelated to the beam line.
2) Wrapped around the beam pipe in a BDSIM magnet element.
3) As a general element in the beam line where the geometry constitutes the whole object.
4) As the world volume in which the BDSIM beamline is placed.

These are discussed in order in :ref:`placements`, :ref:`external-magnet-geometry` and
:ref:`element-external-geometry`.

.. _geometry-formats:

Geometry Formats
^^^^^^^^^^^^^^^^

The following geometry formats are supported. More may be added in collaboration with the BDSIM
developers - please see :ref:`feature-request`. The syntax and preparation of these geometry
formats are described in more detail in :ref:`external-geometry-formats`.

+----------------------+---------------------------------------------------------------------+
| **Format String**    | **Description**                                                     |
+======================+=====================================================================+
| gdml                 | | Geometry Description Markup Language - Geant4's official geometry |
|                      | | persistency format - recommended                                  |
+----------------------+---------------------------------------------------------------------+
| ggmad                | | Simple text interface provided by BDSIM to some simple Geant4     |
|                      | | geometry classes                                                  |
+----------------------+---------------------------------------------------------------------+
| mokka                | | An SQL style description of geometry                              |
+----------------------+---------------------------------------------------------------------+

.. note:: BDSIM must be compiled with the GDML build option in CMake turned on for gdml loading to work.

.. note:: For GDML geometry, we preprocess the input file prepending all names with the name
	  of the element. This is to compensate for the fact that the Geant4 GDML loader does
	  not handle unique file names. However, in the case of very large files with many
	  vertices, the preprocessing can dominate. In this case, the option `preprocessGDML`
	  should be turned off. The loading will only work with one file in this case.

.. warning:: If a geometry file path is defined relative to the location of the GMAD file and that
	     GMAD file is included in a parent file in a different location, the file will not be
	     correctly located (i.e. main.gmad includes ../somedir/anotherfile.gmad, which defines
	     geometry in "../a/relative/path/geometryfile.gdml". The file will not be found). If all
	     GMAD files are located in the same directory, this will not be a problem. It is better / cleaner
	     overall to use multiple GMAD input files and include them.

.. _external-world-geometry:

External World Geometry
^^^^^^^^^^^^^^^^^^^^^^^

External geometry can be supplied as the world volume with the option `worldGeometryFile`
(see :ref:`options-geometry`). The BDSIM beamline will be placed inside this world volume
provided in the file.

Unlike the standard BDSIM world volume whose size is
set dynamically, the external world volume will have fixed dimensions, therefore the user should supply
a world volume of sufficient size so as to fully encompass the BDSIM beamline. Should the extents of the
BDSIM beamline be larger than the world extents, the beamline will not be constructed and BDSIM will exit.

`worldGeometryFile` should be of the format `format:filename`, where `format` is the geometry
format being used (`gdml` | `gmad` | `mokka`) and filename is the path to the geometry
file. See :ref:`externally-provided-geometry` for more details.

* See also :ref:`physics-bias-importance-sampling` for usage of this.

.. _placements:

Placements
----------

Geometry provided in an external file may be placed at any location in the world with
any rotation. This is intended to place geometry alongside the beam line and **not** inside
or as part of it. The user is responsible for ensuring that the geometry does not
overlap with any other geometry including the beam line. Only in special cases, such as
for a magnet yoke, can externally provided geometry be placed "inside" BDSIM geometry.

For geometry to be placed in the beam line, use the :ref:`element`.

.. warning:: If the geometry overlaps, tracking faults may occur from Geant4 as well as
	     incorrect results and there may not always be warnings provided. For this reason,
	     BDSIM will **always** use the Geant4 overlap checker when placing external geometry
	     into the world volume. This only ensures the container doesn't overlap with BDSIM
	     geometry, not that the internal geometry is valid.

.. warning:: You cannot place external geometry 'inside' an accelerator component with a
	     placement. Although it may appear OK in the visualiser, the hierarchy of the
	     geometry will be wrong and the tracking will not work as expected. Avoid this.

There are 3 possible ways to place a piece of geometry.

1) In global Cartesian coordinates.

   - `x`, `y`, `z` and any rotation are with respect to the world frame of reference.


2) In curvilinear coordinates.

   - `s`, `x`, `y` are used along with a rotation. The transform for the distance `s` along the beamline
     is first applied. `x`, `y` and the rotation are with respect to that frame.


3) In curvilinear coordinates with respect to a beam line element by name.

   - The name of an element is used to look up its `s` coordinate. `s`, `x`, `y` and the rotation
     are with respect to the centre of that element. **Therefore**, `s` in this case is `local` curvilinear
     `s`.

The scenario is automatically selected based on which parameters are set. If `s` is finite, then
it is either scenario 2 or 3. If `referenceElement` is specified, scenario 3 is assumed.

.. warning:: For both scenarios 2) and 3), a placement can only be made **inside** the S length of
	     the accelerator - it is not possible to place something beyond the accelerator currently.
	     In this case, the user should resort to a global placement.
	     
The following parameters may be specified with a placement in BDSIM:

+-------------------------+--------------------------------------------------------------------+
| **Parameter**           |  **Description**                                                   |
+-------------------------+--------------------------------------------------------------------+
| geometryFile            | :code:`format:file` - which geometry format and file to use        |
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

`referenceElementNumber` is the occurence of that element in the sequence. For example, if a sequence
was: ::

  l1: line=(d1,sb1,d2,qd1,d2,df1,d2,sb1,d1);

and we wanted to place with respect to the first element, we would use::

  p1: placement, referenceElement="d1",
                 referenceElementNumber=0;

If 0, the `referenceElementNumber` argument is optional. If we want to place with respect to
the third usage of "d2", we would use::

  p1: placement, referenceElement="d2",
                 referenceElementNumber=3;

.. note:: Dipoles are split in BDSIM into many small straight sections. These must have a unique
	  name to appear correctly in the Geant4 visualisation system. The splitting is done
	  dynamically based on the angle of the bend and if it has pole face rotations on one
	  or both sides. The names are mangled and so the original name will not be found.
	  The user should run the visualiser first
	  and identify the name of the segment of the dipole they wish to place with respect to.
	  Alternatively, in the case of low angle bends, the element before or after can be used
	  with a finite `s` offset.

* Examples can be found in :code:`bdsim/examples/features/geometry/13_placements`.
* The file path provided in :code:`geometryFile` should either be relative to where bdsim
  is executed from or an absolute path.
* The main beam line begins at (0,0,0) by default but may be offset.  See
  :ref:`beamline-offset` for more details.


Two styles of rotation can be used: either a set of three Euler angles, or the axis angle
rotation scheme, where a **unit** vector is provided in :math:`x,y,z` and an angle to
rotate about that. These variables are used to construct a :code:`G4RotationMatrix`
directly, which is also the same as a :code:`CLHEP::HepRotation`.

.. Note:: Geant4 uses a right-handed coordinate system and :math:`m` and :math:`rad` are
	  the default units for offsets and angles in BDSIM.

The following is an example syntax used to place a piece of geometry::

  leadblock: placement, x = 10*m,
                        y = 3*cm,
			z = 12*m,
			geometryFile="gdml:mygeometry/detector.gdml";

.. warning:: Care must be taken not to define the same placement name twice. If `leadblock`
	     were declared again here, the first definition would be updated with parameters
	     from the second, leading to possibly unexpected geometry.
	     
.. _external-magnet-geometry:

External Magnet Geometry
^^^^^^^^^^^^^^^^^^^^^^^^

A geometry file may be placed around a beam pipe inside a BDSIM magnet instance. The beam pipe
will be constructed as normal and will use the appropriate BDSIM tracking routines, but the
yoke geometry will be loaded from the file provided. The external geometry must have a cut out
in its container volume for the beam pipe to fit, i.e. both the beam pipe and the yoke exist
at the same level in the geometry hierarchy (both are placed in one container for the magnet).
The beam pipe is not placed 'inside' the yoke.

This will work for `solenoid`, `sbend`, `rbend`, `quadrupole`, `sextupole`, `octupole`,
`decapole`, `multipole`, `muonspoiler`, `vkicker`, `hkicker` element types in BDSIM.

Example::

  q1: quadrupole, l=20*cm, k1=0.0235, magnetGeometryType="gdml:mygeometry/atf2quad.gdml";

.. _element-external-geometry:

Element
^^^^^^^

A general piece of geometry may be placed in the beam line along with any externally provided
field map using the `element` beam line element.  See `element`_.

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
	  using the `use` command (see `use - Defining which Line to Use`_). Failure to do
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
	  is the correct behaviour, as unlike an optics program such as MADX, the sampler
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
the 2nd instnace of the d1 element (zero counting) in the main beam line with a rotation
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
define **only** the processess required so that the simulation covers the desired outcome
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

This last example turns off the minimum kinetic energy and also the the maximum step length
limit which is by default 110% the length of the element. If bad tracking behaviour
is experienced (stuck particles etc.) this should be considered. ::
	  
  option, physicsList="g4FTFP_BERT";

This following example will enforce a minimum kinetic energy but also limit the maximum step length
(consequently) to 110% the length of the component and provide more robust tracking. ::

  option, physicsList="g4FTFP_BERT",
          minimumKineticEnergy=20*GeV;

.. note:: "g4" is not case senstive but the remainder of the string is. The remainder is passed
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


.. _physics-biasing:

Physics Biasing
---------------

BDSIM currently provides two ways to artifically interfere with the physics processes
to make the desired outcome happen more often. In both cases the goal is more efficiently
simulate the correct physical outcome - variance reduction.

The two cases are :ref:`physics-bias-cross-section-biasing` and
:ref:`physics-bias-importance-sampling`, each described below.

.. _physics-bias-cross-section-biasing:

Cross-Section Biasing
^^^^^^^^^^^^^^^^^^^^^

The cross-section for a physics process for a specific particle can be artifically altered
by a numerical scaling factor using cross-section biasing. The biasing is defined with the
keyword **xsecbias**, to define a bias 'object'. This can then be attached to various bits
of the geometry or all of it. This is provided with the Geant4 generic biasing feature.

Geant4 automatically includes the reciprocal of the factor as a weighting, which is
recorded in the BDSIM output as "weight" in each relevant piece of data. Any data
used should be multiplied by the weight to achieve the correct physical result.

.. note:: This only works with Geant4 version 10.1 or higher.

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
  list. The physics list print out will name particles used.
* The process name should be exactly as they are in Geant4 (case-sensitive). Similarly,
  the best way to find these names is to run a single event with the desired physics
  list.
* A special particle name "all" will bias all defined particles. (case-sensitive).
* In the case of an **ion** beam, the particle name should be "GenericIon". The
  biasing will apply to all ions, so the flag should be used to select primary
  or secondary or all particles. This is because Geant4 uses the concept of a
  generic ion as there are so many possible ions.
* Examples can be found in :code:`bdsim/examples/features/processes/5_biasing`.

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
* `One Turn Map`_
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
described in `Tunnel Geometry`_.

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
|                                  | `circular`, `elliptical`, `square`, `rectangular`     |
|                                  | (more to come in v0.9)                                |
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
|                                  | magnets with finite poleface angles. The length of    |
|                                  | the total element is conserved. (default = false).    |
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

+-----------------------------------+--------------------------------------------------------------------+
| **Option**                        | **Function**                                                       |
+===================================+====================================================================+
| elossHistoBinWidth                | The width of the histogram bins [m]                                |
+-----------------------------------+--------------------------------------------------------------------+
| nperfile                          | Number of events to record per output file                         |
+-----------------------------------+--------------------------------------------------------------------+
| sensitiveOuter                    | Whether the outer part of each component (other than the beam      |
|                                   | pipe) records energy loss. `storeELoss` is required to be on for   |
|                                   | this to work. The user may turn off energy loss from the           |
|                                   | beam pipe and retain losses from the magnet outer in combination   |
|                                   | with the next option `sensitiveBeamPipe`. Both are stored together |
|                                   | in `Eloss` branch of the Event Tree in the output. Default on.     |
+-----------------------------------+--------------------------------------------------------------------+
| sensitiveBeamPipe                 | Whether the beam pipe records energy loss. This includes cavities. |
|                                   | This can be used in combination with the above option              |
|                                   | `sensitiveOuter`, to control which energy loss is recorded.        |
|                                   | Energy loss from this option is recorded in the `Eloss` branch     |
|                                   | of the Event Tree in the output. Default on.                       |
+-----------------------------------+--------------------------------------------------------------------+
| sensitiveVacuum                   | Whether energy deposition in the residual vacuum gas is recorded.  |
|                                   | Energy loss from this option is recorded in the `Eloss` branch     |
|                                   | of the Event Tree in the output. Default on.                       |
+-----------------------------------+--------------------------------------------------------------------+
| storeCollimatorHitsIons           | If `storeCollimatorInfo` is on and collimator hits are generated,  |
|                                   | `isIon`, `ionA` and `ionZ` variables are filled. Collimator hits   |
|                                   | will now also be generated for all ions whether primary or         |
|                                   | secondary. Default off.                                            |
+-----------------------------------+--------------------------------------------------------------------+
| storeCollimatorHitsAll            | If `storeCollimatorInfo` is on and collimator hits are generated,  |
|                                   | hits will be generated for all particles interacting with the      |
|                                   | collimators whether primary or secondary and whether ion or not.   |
|                                   | Default off.                                                       |
+-----------------------------------+--------------------------------------------------------------------+
| storeCollimatorInfo               | With this option on, summary information in the Model Tree about   |
|                                   | only collimators is filled. Collimator structures are created in   |
|                                   | the Event Tree of the output for each collimator and prefixed with |
|                                   | "COLL\_" and contain hits from (only) primary particles.           |
|                                   | Collimator summary histograms are also created and stored. Default |
|                                   | off.                                                               |
+-----------------------------------+--------------------------------------------------------------------+
| storeCollimatorLinks              | If `storeCollimatorInfo` is on and collimator hits are generated,  |
|                                   | extra information is stored for each collimator hit.               |
+-----------------------------------+--------------------------------------------------------------------+
| storeEloss                        | Whether to store the energy deposition hits. Default on. By        |
|                                   | turning off, `sensitiveBeamPipe`, `sensitiveOuter` and             |
|                                   | `sensitiveVacuum` have no effect. Saves run time memory and output |
|                                   | file size. See next option `storeEloss` for combination.           |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossHistograms              | Whether to store energy deposition histograms `Eloss` and          |
|                                   | `ElossPE`. This will automatically be on if `storeEloss` is on.    |
|                                   | With `storeEloss` off, this option can be turned on to retain the  |
|                                   | energy deposition histograms. If both this and `storeEloss` are    |
|                                   | off, no energy deposition hits will be generated saving memory.    |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossVacuum                  | Whether to store energy deposition from the vacuum volumes as hits |
|                                   | in the `ElossVacuum` branch and the corresponding summary          |
|                                   | histograms. Default off.                                           |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossVacuumHistograms        | Whether to generate summary histograms of energy deposition in the |
|                                   | vacuum volumes. If `storeElossVacuum` is on, this will be on. The  |
|                                   | user may turn off `storeElossVacuum` but turn this on to store     |
|                                   | the energy deposition histograms.                                  |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossTunnel                  | Whether to store energy deposition hits from the tunnel geometry   |
|                                   | in the `ElossTunnel` branch of the Event Tree. Default off.        |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossTunnelHistograms        | Whether to generate summary histograms of energy deposition in the |
|                                   | tunnel volumes. If `storeElossTunnel` is on, this will be on. The  |
|                                   | user may turn off `storeElossTunnel` but turn this on to store     |
|                                   | the energy deposition histograms.                                  |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossWorld                   | Whether to record energy deposition in the world volume and, in    |
|                                   | the case of using Geant4.10.3 or newer, the energy leaving the     |
|                                   | world volume as well. Default off.                                 |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossWorldContents           | Whether to record energy deposition in the daughter volumes within |
|                                   | the world volume when supplied as external world geometry.         |
|                                   | Default off.                                                       |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossGlobal                  | Global coordinates will be stored for each energy deposition hit   |
|                                   | and for each trajectory point. Default off.                        |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossLinks                   | For each energy deposition hit, the particle ID, track ID, parent  |
|                                   | ID and beam line index will be stored - this is intended to help   |
|                                   | 'link' the energy deposition back to other information. Default    |
|                                   | off.                                                               |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossLocal                   | Local coordinates will be stored for each energy deposition hit    |
|                                   | and for each trajectory point. Default off.                        |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossModelID                 | Store the beam line index of the object the energy deposition hit  |
|                                   | was in. If `storeElossLinks` is on, this will be on irrespective   |
|                                   | of this option.                                                    |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossTime                    | The time since the start of the event will be stored for each point|
|                                   | of energy deposition and trajectory. Default off.                  |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossTurn                    | Store the turn number of each energy deposition hit. Default off,  |
|                                   | but automatically on when using a circular machine with the        |
|                                   | (also executable) option :code:`circular`.                         |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossStepLength              | Stores the step length for each energy deposition hit or not.      |
|                                   | Default off.                                                       |
+-----------------------------------+--------------------------------------------------------------------+
| storeElossPreStepKineticEnergy    | Stores the kinetic energy of the particle causing energy deposition|
|                                   | as taken from the beginning of the step before it made it. Default |
|                                   | off.                                                               |
+-----------------------------------+--------------------------------------------------------------------+
| storeGeant4Data                   | Whether to store basic particle information for all particles used |
|                                   | in the simulation under Geant4Data in the output. This can be      |
|                                   | relatively large when ions are used as there are many thousands    |
|                                   | of ion definitions. Default on.                                    |
+-----------------------------------+--------------------------------------------------------------------+
| storeModel                        | Whether to store the model information in the output. Default on.  |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerAll                   | Convenience option to turn on all optional sampler output.         |
|                                   | Equivalent to turning on `storeSamplerCharge`,                     |
|                                   | `storeSamplerKineticEnergy`, `storeSamplerMass`,                   |
|                                   | `storeSamplerRigidity`, `storeSamplerIon`. Overrides these         |
|                                   | options even if they are explicitly set to off (0).                |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerCharge                | Stores corresponding charge of particle for every entry in sampler |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerKineticEnergy         | Stores corresponding kinetic energy of particle for every entry in |
|                                   | sampler.                                                           |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerMass                  | Stores corresponding mass (in GeV) of particle for every entry in  |
|                                   | the sampler.                                                       |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerPolarCoords           | Calculate and store the polar coordinates (r, phi) and (rp, phip)  |
|                                   | for the sampler data.                                              |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerRigidity              | Stores the rigidity (in Tm) of particle for every entry in sampler |
+-----------------------------------+--------------------------------------------------------------------+
| storeSamplerIon                   | Stores A, Z and Boolean whether the entry is an ion or not         |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectory                   | Whether to store trajectories. If turned on, all trajectories are  |
|                                   | stored. This must be turned on to store any trajectories at all.   |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectories                 | An alias to `storeTrajectory`                                      |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectoryDepth              | The depth of the particle tree to store the trajectories to  0 is  |
|                                   | the primary, 1 is the first generation of secondaries, etc.        |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectoryELossSRange        | Ranges in curvilinear S coordinate that if a particular track      |
|                                   | causes energy deposition in this range, its trajectory will be     |
|                                   | stored. The value should be a string inside which are pairs of     |
|                                   | numbers separated by a colon and ranges separated by whitespace    |
|                                   | such as "0.3:1.23 45.6:47.6".                                      |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectoryParticle           | The Geant4 name of particle(s) to only store trajectories for.     |
|                                   | This is case sensitive. Multiple particle names can be used with   |
|                                   | a space between them. e.g. "proton pi-".                           |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectoryParticleID         | The PDG ID of the particle(s) to only store trajectories for.      |
|                                   | Multiple particle IDs can be supplied with a space between them.   |
|                                   | e.g. "11 12 22 13".                                                |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectoryEnergyThreshold    | The threshold energy for storing trajectories. Trajectories for    |
|                                   | any particles with energy less than this amount (in GeV) will not  |
|                                   | be stored.                                                         |
+-----------------------------------+--------------------------------------------------------------------+
| storeTrajectorySamplerID          | If a trajectory reaches the name of these samplers, store that     |
|                                   | trajectory. This value supplied should be a whitespace separated   |
|                                   | string such as "cd1 qf32x".                                        |
+-----------------------------------+--------------------------------------------------------------------+
| trajConnect                       | Stores all the trajectories that connect a trajectory to be        |
|                                   | stored all the way to the primary particle. For example, if the    |
|                                   | filters from other trajectory options are to store only muons      |
|                                   | with an energy greater than 10 GeV, the few trajectories stored    |
|                                   | would appear unrelated. This option forces the storage of only     |
|                                   | the trajectories of any particles (irrespective of filters) that   |
|                                   | lead to the muon in question.                                      |
+-----------------------------------+--------------------------------------------------------------------+
| trajNoTransportation              | Suppresses trajectory points generated by transportation. When a   |
|                                   | particle hits a volume boundary, two trajectories would be created |
|                                   | for before and afterwards, even if it didn't interact or change.   |
|                                   | This option removes these points.                                  |
+-----------------------------------+--------------------------------------------------------------------+
| trajCutGTZ                        | Only stores trajectories whose *global* z-coordinate is greater    |
|                                   | than this value in metres [m].                                     |
+-----------------------------------+--------------------------------------------------------------------+
| trajCutLTR                        | Only stores trajectories whose *global* radius is from the start   |
|                                   | position (sqrt(x^2, y^2)).                                         |
+-----------------------------------+--------------------------------------------------------------------+


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

+----------------------------------+-------------------------------------------------------+
| **Option**                       | **Function**                                          |
+==================================+=======================================================+
| useScoringMap                    | Whether to create a scoring map                       |
+----------------------------------+-------------------------------------------------------+
| nbinsx                           | Number of bins in global X                            |
+----------------------------------+-------------------------------------------------------+
| nbinsy                           | Number of bins in global Y                            |
+----------------------------------+-------------------------------------------------------+
| nbinsz                           | Number of bins in global Z                            |
+----------------------------------+-------------------------------------------------------+
| xmin                             | Lower global X limit                                  |
+----------------------------------+-------------------------------------------------------+
| xmax                             | Upper global X limit                                  |
+----------------------------------+-------------------------------------------------------+
| ymin                             | Lower global Y limit                                  |
+----------------------------------+-------------------------------------------------------+
| ymax                             | Upper global Y limit                                  |
+----------------------------------+-------------------------------------------------------+
| zmin                             | Lower global Z limit                                  |
+----------------------------------+-------------------------------------------------------+
| zmax                             | Upper global Z limit                                  |
+----------------------------------+-------------------------------------------------------+

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

BDSIM starts each event by simulating one particle from a beam distribution. A distribution is
chosen by the user in the input GMAD and the particle coordinates are randomly generated according
to this distribution. To specify the input particle distribution, the :code:`beam` command is
used. This also specifies the particle species and **reference total energy**, which is the
design total energy of the machine. This is used along with the particle species to calculate
the momentum of the reference particle and therefore the magnetic rigidity that normalised magnetic
field strengths are calculated with respect to. For example, the field of dipole magnets
is calculated using this if only the `angle` parameter has been specified.

Apart from the design particle and energy, a beam of particles of a different species and total
energy may be specified. By default, if only one particle is specified this is assumed to be
both the design particle and the particle used for the beam distribution.

.. note:: The design energy is required to be specified, but the central energy, of say
	  a bunch with a Gaussian distribution, can be also be specified with `E0`.

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

Energy is the total energy in `GeV`. The beam particle may be one of the following:

* `e-` or `e+`
* `proton` or `antiproton`
* `gamma`
* `neutron`
* `mu-` or `mu+`
* `pi-` or `pi+`
* `photon` or `gamma`

In fact, the user may specify any particle that is available through the physics lists
used. The particle must be given by the Geant4 name. The ones above are always defined
and so can always safely be used irrespective of the physics lists used. If the particle
definition is not found, BDSIM will print a warning and exit.

Ion Beams
^^^^^^^^^

The user may also specify any ion with the following syntax::

  beam, particle="ion A Z";

or::

  beam, particle="ion A Z Q";

where `A`, `Z` and `Q` should be replaced by the atomic number, the number of protons
in the nucleus and the charge. The charge is optional and by default is Z (i.e. a fully
ionised ion). In this case, it is recommended to use the `ion` physics list.

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
| `T0`                             | Longitudinal position [s]                             | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Xp0`                            | Horizontal canonical momentum                         | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `Yp0`                            | Vertical canonical momentum                           | 0        |
+----------------------------------+-------------------------------------------------------+----------+
| `E0`                             | Central total energy of bunch distribution (GeV)      | 'energy' |
+----------------------------------+-------------------------------------------------------+----------+

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
| `matchDistrFileLength`           | Option for certain distributions to simulate the same |
|                                  | number of events as are in the file. Currently only   |
|                                  | for the `ptc` distribution.                           |
+----------------------------------+-------------------------------------------------------+

Acceptable tokens for the columns are:

+------------+------------------------+
| **Token**  |  **Description**       |
+============+========================+
| "E"        | Total energy           |
+------------+------------------------+
| "Ek"       | Kinetic energy         |
+------------+------------------------+
| "P"        | Momentum               |
+------------+------------------------+
| "t"        | Time                   |
+------------+------------------------+
| "x"        | Horizontal position    |
+------------+------------------------+
| "y"        | Vertical position      |
+------------+------------------------+
| "z"        | Longitudinal position  |
+------------+------------------------+
| "xp"       | Horizontal angle       |
+------------+------------------------+
| "yp"       | Vertical angle         |
+------------+------------------------+
| "zp"       | Longitudinal           |
+------------+------------------------+
| "pt"       | PDG particle ID        |
+------------+------------------------+
| "w"        | Weight                 |
+------------+------------------------+
| "-"        | Skip this column       |
+------------+------------------------+

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

.. _tunnel-geometry:

Tunnel Geometry
---------------

BDSIM can build a tunnel around the beam line. Currently, there are two main ways to control this.

1) The tunnel follows the beam line, bending automatically (recommended)
2) The tunnel is just built in a straight line - this may be useful for linear colliders but
   may also cause geometry overlaps (the user is responsible for checking this!)

.. warning:: With option 2, the user is entirely responsible to ensure no overlaps occur
	     (through good design). Also note that the samplers may overlap the tunnel
	     depending on the tunnel geometry (samplers are square with half-width of
	     `samplerRadius`). In practice, however, we haven't observed many ill effects
	     because of this. Problems would take the form of 'stuck particles' and
	     Geant4 would terminate that event.

Examples of tunnel geometry can be found with the BDSIM source code in */examples/features/geometry/tunnel*
and are described in :ref:`tunnel-examples`.

The automatic tunnel building is controlled through the following options used with the
:code:`option` command.

.. tabularcolumns:: |p{5cm}|p{10cm}|

+----------------------------------+-------------------------------------------------------+
| **Tunnel Parameters**            | **Description**                                       |
+----------------------------------+-------------------------------------------------------+
| buildTunnel                      | Whether to build a tunnel (default = 0)               |
+----------------------------------+-------------------------------------------------------+
| buildTunnelStraight              | Whether to build a tunnel, ignoring the beamline and  |
|                                  | just in a straight line (default = 0)                 |
+----------------------------------+-------------------------------------------------------+
| builTunnelFloor                  | Whether to add a floor to the tunnel                  |
+----------------------------------+-------------------------------------------------------+
| tunnelIsInfiniteAbsorber         | Whether all particles entering the tunnel material    |
|                                  | should be killed or not (default = false)             |
+----------------------------------+-------------------------------------------------------+
| tunnelType                       | Which style of tunnel to use - one of:                |
|                                  | `circular`, `elliptical`, `square`, `rectangular`     |
|                                  | (more to come in v0.9)                                |
+----------------------------------+-------------------------------------------------------+
| tunnelAper1                      | Tunnel aperture parameter #1 - typically              |
|                                  | horizontal (m)                                        |
+----------------------------------+-------------------------------------------------------+
| tunnelAper2                      | Tunnel aperture parameter #2 - typically              |
|                                  | vertical (m)                                          |
+----------------------------------+-------------------------------------------------------+
| tunnelThickness                  | Thickness of tunnel wall (m)                          |
+----------------------------------+-------------------------------------------------------+
| tunnelSoilThickness              | Soil thickness outside tunnel wall (m)                |
+----------------------------------+-------------------------------------------------------+
| tunnelMaterial                   | Material for tunnel wall                              |
+----------------------------------+-------------------------------------------------------+
| soilMaterial                     | Material for soil outside tunnel wall                 |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetX                    | Horizontal offset of the tunnel with respect to the   |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelOffsetY                    | Vertical offset of the tunnel with respect to the     |
|                                  | beam line reference trajectory                        |
+----------------------------------+-------------------------------------------------------+
| tunnelFloorOffset                | The offset of the tunnel floor from the centre of the |
|                                  | tunnel (**not** the beam line)                        |
+----------------------------------+-------------------------------------------------------+

These parameters are shown schematically in the figure below (gaps not to scale, elliptical
shown as an example).

.. figure:: figures/tunnel/tunnel_parameters.pdf
	    :width: 80%
	    :align: center

The soil around the tunnel is typically symmetric, with the `tunnelSoilThickness` being added to
the larger of the horizontal and vertical tunnel dimensions.

Construction of the tunnel geometry may fail in particular cases of different beam lines.
Beam lines with very strong bends ( > 0.5 rad) over a few metres may cause overlapping
geometry. In future, it will be possible to override the automatic algorithm between
certain elements in the beamline, but for now such situations must be avoided.

.. note:: Surrounding the beam line with a tunnel completely means that every particle simulated
	  will have to eventually hit something and not escape. This means that every single particle
	  will likely create a shower of particles down to 0 energy. This can increase simulation time.
	  To avoid this, or at least contorl this behaviour, it is recommended to use the options
	  :code:`minimumKineticEnergyTunnel` or :code:`tunnelIsInfiniteAbsorber`.

.. _materials-and-atoms:
	  
Materials and Atoms
-------------------

All chemical elements are available in BDSIM as well as the Geant4 NIST database
of materials for use. Custom materials and can also be added via the parser. All materials
available in BDSIM can be found by executing BDSIM with the :code:`--materials` option. ::

  bdsim --materials

Aside from these, several materials useful for accelerator applications are already defined
that are listed in :ref:`predefined-materials`.

Generally, each beam line element accepts an argument "material" that is the
material used for that element. It is used differently depending on the element. For example,
in the case of a magnet, it is used for the yoke and for a collimator for the collimator
block.

Single Element
^^^^^^^^^^^^^^

In the case of an element, the chemical symbol can be specified::

  rc1: rcol, l=0.6*m, xsize=1.2*cm, ysize=0.6*cm, material="W";

These are automatically prefixed with :code:`G4_` and retrieved from the NIST database of
materials.

The user can also define their own material and then refer to it by name when defining
a beam line element.

Custom Single Element Material
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If the material required is composed of a single element, but say of a different density or
state than the default NIST one provided, it can be defined using the **matdef**
command with the following syntax::

  materialname : matdef, Z=<int>, A=<double>, density=<double>, T=<double>, P=<double>, state=<char*>;

=========  ========================== =============
Parameter  Description                Default
Z          Atomic number
A          Mass number [g/mol]
density    Density in [g/cm3]
T          Temperature in [K]         300
P          Pressure [atm]             1
state      "solid", "liquid" or "gas" "solid"
=========  ========================== =============

Example::

  iron2 : matdef, Z=26, A=55.845, density=7.87;

A compound material can be specified in two manners:

Compound Material by Atoms
^^^^^^^^^^^^^^^^^^^^^^^^^^
If the number of atoms of each component in a material unit is known,
the following syntax can be used::

   <material> : matdef, density=<double>, T=<double>, P=<double>,
                state=<char*>, components=<[list<char*>]>,
                componentsWeights=<{list<int>}>;

================= ===================================================
Parameter         Description
density           Density in [g/cm3]
components        List of symbols for material components
componentsWeights Number of atoms for each component in material unit
================= ===================================================

Example::

  NbTi : matdef, density=5.6, T=4.0, components=["Nb","Ti"], componentsWeights={1,1};

Compound Material by Mass Fraction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

On the other hand, if the mass fraction of each component is known, the
following syntax can be used::

   <material> : matdef, density=<double>, T=<double>, P=<double>,
                state=<char*>, components=<[list<char*>]>,
                componentsFractions=<{list<double>}>;

=================== ================================================
Parameter           Description
components          List of symbols for material components
componentsFractions Mass fraction of each component in material unit
=================== ================================================

Example::

  SmCo : matdef, density=8.4, T=300.0, components=["Sm","Co"], componentFractions = {0.338,0.662};

The second syntax can also be used to define materials which are composed by
other materials (and not by atoms).

.. note:: Square brackets are required for the list of element symbols, curly
	  brackets for the list of weights or fractions.

New elements can be defined with the **atom** keyword::

  elementname : atom, Z=<int>, A=<double>, symbol=<char*>;

=========  =====================
Parameter  Description
Z          Atomic number
A          Mass number [g/mol]
symbol     Atom symbol
=========  =====================

Example::

  myNiobium  : atom, symbol="myNb", Z=41, A=92.906;
  myTitanium : atom, symbol="myTi", Z=22, A=47.867;
  myNbTi     : matdef, density=5.6, T=4.0, components=["myNb","myTi"], componentsWeights={1,1};

.. _predefined-materials:

Predefined Materials
^^^^^^^^^^^^^^^^^^^^

The following elements are available by full name that refer to the Geant4 NIST
elements:

* aluminium
* beryllium
* carbon
* chromium
* copper
* iron
* lead
* magnesium
* nickel
* nitrogen
* silicon
* titanium
* tungstem
* uranium
* vanadium
* zinc

The following materials are also defined in BDSIM. The user should consult
:code:`bdsim/src/BDSMaterials.cc` for the full definition of each including
elements, mass fractions, temperature and state.

* air (G4_AIR)
* airbdsim
* aralditef
* awakeplasma
* berylliumcopper
* bn5000
* bp_carbonmonoxide
* calciumcarbonate
* carbonfiber
* carbonmonoxide
* carbonsteel
* cellulose (G4_CELLULOSE_CELLOPHANE)
* clay
* clayousMarl
* concrete
* cu_4k
* dy061
* epoxyresin3
* fusedsilica
* gos_lanex
* gos_ri1
* graphite
* graphitefoam
* hy906
* invar
* kapton
* lanex
* lanex2
* laservac (same as vacuum but with different name)
* leadtungstate
* lhcconcrete
* lhc_rock
* lhe_1.9k
* limousmarl
* liquidhelium
* marl
* medex
* mild_steel
* niobium_2k
* nbti.1
* nbti_4k
* nbti_87k
* nb_2k (niobium_2k)
* nb_87k
* n-bk7
* perspex
* pet
* pet_lanex
* pet_opaque
* polyurethane
* quartz
* smco
* soil
* solidhydrogen
* solidnitrogen
* solidoxygen
* stainlesssteel
* stainless_steel_304L
* stainless_steel_304L_87K
* stainless_steel_304LN
* stainless_steel_304LN_87K
* ti_87k
* tungsten_heavy_alloy
* ups923a
* vacuum
* water (G4_WATER)
* weightiron
* yag

Vacuum and Air
^^^^^^^^^^^^^^

The default "vacuum" material used in all beam pipes is composed of H, C and O with the
following fractions:

+--------------+-------------------+
| **Element**  | **Mass Fraction** |
+==============+===================+
| H            | 0.482             |
+--------------+-------------------+
| C            | 0.221             |
+--------------+-------------------+
| O            | 0.297             |
+--------------+-------------------+

The default pressure is 1e-12 bar, the temperature is 300K and the density os 1.16336e-9 g/cm3.

"air" is the G4_AIR material. As of Geant4.10.04.p02
(see geant4/source/materials/src/G4NistMaterialBuilder.cc), it is composed of C, N, O, Ar
with the following fractions:

+--------------+-------------------+
| **Element**  | **Mass Fraction** |
+==============+===================+
| C            | 0.000124          |
+--------------+-------------------+
| N            | 0.755267          |
+--------------+-------------------+
| O            | 0.231781          |
+--------------+-------------------+
| Ar           | 0.012827          |
+--------------+-------------------+

It is a gas with density of 1.20479 mg/cm3.

.. _crystals:

Crystals
--------

To use various crystal components in BDSIM such as `crystalcol`_, a crystal definition
must first be made. This contains all of the required information to construct the
crystal. The following parameters are required:

+-------------------+------------------------------------------------------------+
| **Parameter**     | **Description**                                            |
+===================+============================================================+
| material          | Material that the crystal will be composed of              |
+-------------------+------------------------------------------------------------+
| data              | Path to data files, including first part of file name      |
+-------------------+------------------------------------------------------------+
| shape             | Geometry used - one of (box, cylinder, torus)              |
+-------------------+------------------------------------------------------------+
| lengthX           | X-dimension full length [m]                                |
+-------------------+------------------------------------------------------------+
| lengthY           | Y-dimension full length [m]                                |
+-------------------+------------------------------------------------------------+
| lengthZ           | Z-dimension full length [m]                                |
+-------------------+------------------------------------------------------------+
| sizeA             | Unit cell a dimension [m]*                                 |
+-------------------+------------------------------------------------------------+
| sizeB             | Unit cell b dimension [m]*                                 |
+-------------------+------------------------------------------------------------+
| sizeC             | Unit cell c dimension [m]*                                 |
+-------------------+------------------------------------------------------------+
| alpha             | Interaxial angle :math:`\alpha` in units of :math:`\pi/2`  |
+-------------------+------------------------------------------------------------+
| beta              | Interaxial angle :math:`\beta` in units of :math:`\pi/2`   |
+-------------------+------------------------------------------------------------+
| gamma             | Interaxial angle :math:`\gamma` in units of :math:`\pi/2`  |
+-------------------+------------------------------------------------------------+
| spaceGroup        | Space grouping of lattice (integer)                        |
+-------------------+------------------------------------------------------------+
| bendingAngleYAxis | Angle that the crystal is bent about Y-axis [rad].         |
+-------------------+------------------------------------------------------------+
| bendingAngleZAxis | Angle that the crystal is bent about Z-axis [rad].         |
+-------------------+------------------------------------------------------------+

* (*) Note, the units of metres may seem ridiculous, but the parser is consistently in S.I.
  (or as much as possible). We recommend using units in the parser such as Angstroms.
  See :ref:`coordinates-and-units`.

.. note:: Depending on the shape chosen, the geometry may or may not represent the bending angle.
	  The `bendingAngleYAxis` is always supplied to the channelling physics process
	  irrespective of the geometry. This is important to note that the crystal may be a box,
	  but the 'crystal' inside (in terms of the physics process) is not related to the geometry
	  and is bent. The physical geometry is merely a volume where the crystal parameters
	  apply.

.. note:: If there is no vertical bending angle, the torus geometry will reduce to the
	  cylinder geometry,  as this is faster for tracking. Similarly, if the cylinder is used
	  and there is no horizontal bending angle, a box will be used, as it's not possible
	  to construct a cylinder with an infinite bending radius.

It is entirely possible to add more shapes to the code. Please contact the developers
:ref:`feature-request`.

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

  uglycrystal: crystal, material = "G4_Si",
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
			bendingAngleYAxis = -0.1*rad,
			bendingAngleZAxis = 0;


More examples can be found in :ref:`crystal-examples`.

.. _regions:
			
Regions
-------

In Geant4, it is possible to drive different *regions*- each with their own production cuts and user limits.
In BDSIM, there is one default region to which the options prodCutXXXX apply (see `Options`_) and then
the user may define additional regions and attach them to the objects desired.  For example::

  precisionRegion: cutsregion, prodCutProtons=1*m,
                               prodCutElectrons=10*m,
			       prodCutPositrons=10*m,
			       prodCutPhotons = 1*mm;

  d1: drift, l=10*m, region="precisionRegion";


.. rubric:: Footnotes

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
the same default behaviour as MADX. MADX will write out a value of `fintx` as -1 in this case in
any output. BDSIM will write out the value used, even if it's equal to 0.

Pole Face Rotations
^^^^^^^^^^^^^^^^^^^

The `bdsimtwo` integrator set (see :ref:`integrator-sets`) provides tracking through a uniform
magnetic field in a dipole. The field exists wherever the magnet exists; in the case of pole
face rotations on the end of a dipole, the magnet is constructed with the appropriate angled face.
The field therefore also has a hard edge with exactly no field immediately outside the magnet volume.

The tracking routine for dipoles in the `bdsimtwo` integrator set (see :ref:`bdsim-dipole-rodrigues2`)
tracks the particle using the analytical helical solution in a pure magnetic field in Cartesian
coordinates. This however does not agree with the tracking provided by MADX. We therefore provide
an equivalent to MADX in `bdsimmatrix` integrator set (the default). The vertical focussing provided
by the fringe field is the same in both cases.

The difference between the two is negligible for small pole face angles - for example, the LHC lattice
shows no difference between the integrator sets (~14mrad bending angle and very low pole face angles).
However, with higher angle bends and stronger pole face angles (maximum is up to 45 degrees), the
difference is non-negligible.

The integrator for dipoles in `bdsimtwo` is computationally faster and should be used for lattices
like the LHC, where speed matters and the pole faces are not a strong feature.

.. note:: To provide equivalent tracking to MADX with the `bdsimmatrix` integrator set, the
	  magnet geometry is constructed with flat ends (i.e. always an sbend). Rbends are constructed
	  as sbends with additional poleface rotation angles equal to half the bend angle. Instead of
	  constructing the poleface geometry, the effect of a poleface rotation is applied in a thin
	  fringefield magnet (1 micron thick by default) at the beginning (for non-zero e1) or at the
	  end (for non-zero e2) of the dipole. In future, this will be decoupled to allow both the
	  physical angled faces in the model as well as accurate tracking, using the MADX style matrix
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

.. _colours:

Colours
-------

Most items allow you to define a custom colour for them to aid in visualisation. This includes
all magnets and collimators, the shield and degrader. The colour can be defined with red, green
and blue components, as well as a level of transparency, alpha. RGB values can range from 0
to 255. Once defined, a colour may not be redefined. The syntax to define a colour is

.. code-block:: none

		NAME: newcolour, red=#, green=#, blue=#, alpha=#

Examples: ::
  
  purple: newcolour, red=128, green=0, blue=128;
  col1: rcol, l=0.2*m, xsize=5*cm, ysize=4*cm, colour="purple", material="copper";


and::

  purple: newcolour, red=128, green=0, blue=128;
  orange: newcolour, red=255, green=140, blue=0;
  nicegreen: newcolour, red=0, green=128, blue=0;

  d1: drift, l=1*m;
  basebend: sbend, l=2*m, angle=0.9;
  sb1: basebend, colour="purple";
  sb3: basebend, colour="nicegreen";
  sb4: basebend, colour="yellow";
  sb5: basebend, colour="orange";
  sb6: basebend, colour="red";

  beamline: line=(d1,sb1,d1,basebend,d1,sb3,d1,sb4,d1,sb5,d1,sb6,d1);
  use, beamline;
  sample, all;

  beam,  particle="proton",
         energy= 50*GeV;

This examples if from `bdsim/examples/features/visualisation/coloured_sbend.gmad` and
produces the model shown below.

.. figure:: figures/visualisation/coloured_sbends.png
	    :width: 80%
	    :align: center


* Colours can only be specified on an element-by-element basis.
* Colour names are case-sensitive.
* New colour names must not clash with predefined BDSIM colour names.

All available colours in BDSIM can be found by running BDSIM with the :code:`--colours` command: ::

  bdsim --colours

For convenience the predefined colours in BDSIM are:

+---------------------+-----+-----+-----+-----+
| Name                |  R  |  G  |  B  |  A  |
+=====================+=====+=====+=====+=====+
|              LHCcoil| 229 | 191 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|            LHCcollar| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|        LHCcopperskin| 184 | 133 |  10 |   1 |
+---------------------+-----+-----+-----+-----+
|              LHCyoke|   0 | 127 | 255 |   1 |
+---------------------+-----+-----+-----+-----+
|           LHCyokered| 209 |  25 |  25 |   1 |
+---------------------+-----+-----+-----+-----+
|          awakescreen| 175 | 196 | 222 |   1 |
+---------------------+-----+-----+-----+-----+
|    awakespectrometer|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|             beampipe| 102 | 102 | 102 |   1 |
+---------------------+-----+-----+-----+-----+
|                black|   0 |   0 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|                 blue|   0 |   0 | 255 |   1 |
+---------------------+-----+-----+-----+-----+
|                brown| 114 |  63 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|                 coil| 184 | 115 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
|           collimator|  76 | 102 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
|              crystal| 175 | 196 | 222 |   1 |
+---------------------+-----+-----+-----+-----+
|                 cyan|   0 | 255 | 255 |   1 |
+---------------------+-----+-----+-----+-----+
|             decapole|  76 |  51 | 178 |   1 |
+---------------------+-----+-----+-----+-----+
|              default| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|             degrader| 159 | 159 | 159 |   1 |
+---------------------+-----+-----+-----+-----+
|         dipolefringe| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|                drift| 102 | 102 | 102 |   1 |
+---------------------+-----+-----+-----+-----+
|                 ecol|  76 | 102 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
|              element| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|                  gap| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|                 gdml| 102 |  51 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|                 gray| 127 | 127 | 127 |   1 |
+---------------------+-----+-----+-----+-----+
|                green|   0 | 255 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|                 grey| 127 | 127 | 127 |   1 |
+---------------------+-----+-----+-----+-----+
|              hkicker|  76 |  51 | 178 |   1 |
+---------------------+-----+-----+-----+-----+
|                 jcol|  76 | 102 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
|               kicker|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|              magenta| 255 |   0 | 255 |   1 |
+---------------------+-----+-----+-----+-----+
|               marker| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|            multipole| 118 | 135 | 153 |   1 |
+---------------------+-----+-----+-----+-----+
|          muonspoiler|   0 | 205 | 208 |   1 |
+---------------------+-----+-----+-----+-----+
|             octupole|   0 | 153 |  76 |   1 |
+---------------------+-----+-----+-----+-----+
|  paralleltransporter| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|           quadrupole| 209 |  25 |  25 |   1 |
+---------------------+-----+-----+-----+-----+
|                rbend|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|                 rcol|  76 | 102 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
| reallyreallydarkgrey|  51 |  51 |  51 |   1 |
+---------------------+-----+-----+-----+-----+
|      rectangularbend|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|                  red| 255 |   0 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|                   rf| 118 | 135 | 153 |   1 |
+---------------------+-----+-----+-----+-----+
|             rfcavity| 118 | 135 | 153 |   1 |
+---------------------+-----+-----+-----+-----+
|              rmatrix| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|                sbend|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|               screen| 175 | 196 | 222 |   1 |
+---------------------+-----+-----+-----+-----+
|          screenframe| 178 | 178 | 178 | 0.4 |
+---------------------+-----+-----+-----+-----+
|           sectorbend|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|            sextupole| 255 | 204 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|               shield| 138 | 135 | 119 |   1 |
+---------------------+-----+-----+-----+-----+
|                 soil| 138 |  90 |   0 | 0.4 |
+---------------------+-----+-----+-----+-----+
|             solenoid| 255 | 139 |   0 |   1 |
+---------------------+-----+-----+-----+-----+
|            srfcavity| 175 | 196 | 222 |   1 |
+---------------------+-----+-----+-----+-----+
|        thinmultipole| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|          thinrmatrix| 229 | 229 | 229 |   1 |
+---------------------+-----+-----+-----+-----+
|              tkicker|   0 | 102 | 204 |   1 |
+---------------------+-----+-----+-----+-----+
|               tunnel| 138 | 135 | 119 |   1 |
+---------------------+-----+-----+-----+-----+
|          tunnelfloor| 127 | 127 | 114 |   1 |
+---------------------+-----+-----+-----+-----+
|            undulator| 159 | 159 | 159 |   1 |
+---------------------+-----+-----+-----+-----+
|              vkicker| 186 |  84 | 211 |   1 |
+---------------------+-----+-----+-----+-----+
|              warning| 255 |  19 | 146 |   1 |
+---------------------+-----+-----+-----+-----+
|                white| 255 | 255 | 255 |   1 |
+---------------------+-----+-----+-----+-----+
|          wirescanner| 138 | 135 | 119 |   1 |
+---------------------+-----+-----+-----+-----+
|               yellow| 255 | 255 |   0 |   1 |
+---------------------+-----+-----+-----+-----+

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
be produced. These can be set globally or for a *region* (see `Regions`_) that is attached
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
