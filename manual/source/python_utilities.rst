.. _python-utilities:

****************
Python Utilities
****************

Several python packages are provided to aid preparation and conversion
of models to and from BDSIM's gmad format and are described in the following
sections.

* pybdsim - conversion and building of input as well as ASCII data analysis
* pymadx  - file loader for MADX TFS file format
* pymad8  - file loader for MAD8 file format
* pytransport - file loader for Transport file format

Installing Python Utilities
===========================

Installation is described in the setup section in :ref:`setup-python-utilities`.


Documentation
=============

In the following sections is individual documentation for each python package.
These are also available when using ipython by typing '?' after any module, class
or function.

   * `<http://www.pp.rhul.ac.uk/bdsim/pybdsim/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pymadx/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pymad8/>`_
   * `<http://www.pp.rhul.ac.uk/bdsim/pytransport/>`_

.. _python-geometry-preparation:
     
Geometry Preparation
====================

For preparation of custom geometry, we have developed a Python geometry package
called `pyg4ometry`. This allows scripting of geometry creation in Python, visualisation
and overlap checking outside Geant4. THe emitted GDML files can be loaded in BDSIM.

Please visit `<https://bitbucket.org/jairhul/pyg4ometry>`_
