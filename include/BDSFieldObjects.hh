/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSFIELDOBJECTS_H
#define BDSFIELDOBJECTS_H

#include "globals.hh" // geant4 types / globals
#include "G4Version.hh"

#include <vector>

class BDSFieldInfo;

class G4ChordFinder;
class G4ElectroMagneticField;
class G4EquationOfMotion;
class G4Field;
class G4FieldManager;
class G4LogicalVolume;
class G4MagneticField;
class G4MagInt_Driver;
class G4MagIntegratorStepper;

/**
 * @brief A holder for all the Geant4 field related objects.
 *
 * These are the objects that
 * are required for a magnetic field other than the  magnetic field
 * itself. If owns all objects apart from the G4MagneticField. This
 * is used purely to construct the other objects or as a reference
 * to which field they're based on.
 * 
 * @author Laurie Nevay
 */

class BDSFieldObjects
{
public:
  /// A field is required to build the required objects to manage and use it.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4Field*                fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn,
		  G4ChordFinder*          chordFinderIn,
		  G4FieldManager*         fieldManagerIn);

  /// Alternative constructor for the special case of an electro-magnetic field
  /// which doesn't follow the same hierarchy of classes as magnetic fields.
  /// This can also be used for a second argument of G4MagneticField as this inherits
  /// G4ElectroMagneticField.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4ElectroMagneticField* fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn);

#if G4VERSION_NUMBER > 1049
  /// In Geant4.10.5.b01, the field class inheritance has changed and G4MagneticField
  /// no longer inherits G4ElectroMagneticField.
  BDSFieldObjects(const BDSFieldInfo*     infoIn,
		  G4MagneticField*        fieldIn,
		  G4EquationOfMotion*     equationOfMotionIn,
		  G4MagIntegratorStepper* magIntegratorStepperIn);
#endif

  /// Destructor deletes all objects apart from the magnetic field
  ~BDSFieldObjects();

  ///@{ Acessor.
  inline const BDSFieldInfo*     GetInfo()             const {return info;}
  inline G4Field*                GetField()            const {return field;}
  inline G4EquationOfMotion*     GetEquationOfMotion() const {return equationOfMotion;}
  inline G4MagIntegratorStepper* GetIntegrator()       const {return magIntegratorStepper;}
  inline G4ChordFinder*          GetChordFinder()      const {return chordFinder;}
  inline G4FieldManager*         GetFieldManager()     const {return fieldManager;}
  inline G4MagInt_Driver*        GetMagIntDriver()     const {return magIntDriver;}
  ///@}

  /// Interface to easily attach to logical volume.
  void AttachToVolume(G4LogicalVolume* volume,
		      G4bool penetrateToDaughterVolumes = true);

  /// Interface to easily attach to logical volumes.
  void AttachToVolume(std::vector<G4LogicalVolume*> volumes,
		      G4bool penetrateToDaughterVolumes = true);
  
private:
  /// Private default constructor to force use of non-default constructor
  BDSFieldObjects();

  /// Private copy constructor to avoid shallow pointer copy and possible double deletion.
  /// Could possibly implement this with the info and the field factory.
  BDSFieldObjects(const BDSFieldObjects& other);

  /// The complete information required to build this field.
  const BDSFieldInfo* info;
  
  /// Reference to field this instance is based on.
  G4Field* field;
  
  /// Equation of motion, typically G4Mag_UsualEqRhs instance
  G4EquationOfMotion* equationOfMotion;

  /// Stepper, selectable depending on smoothness of the field etc.
  G4MagIntegratorStepper* magIntegratorStepper;

  /// Chord manager
  G4ChordFinder* chordFinder;

  /// Field manager
  G4FieldManager* fieldManager;

  /// EM field integrator driver (optional) - only for EM fields.
  G4MagInt_Driver* magIntDriver;
};

#endif
