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
#ifndef BDSLASERCOMPTONSCATTERING_H
#define BDSLASERCOMPTONSCATTERING_H
#include "globals.hh"
#include "G4VDiscreteProcess.hh"

class BDSAuxiliaryNavigator;
class BDSComptonScatteringEngine;

/**
 * @brief Laser excitation of ion.
 *
 * @author Siobhan Alden
 */

class BDSLaserComptonScattering: public G4VDiscreteProcess
{
public:
  explicit BDSLaserComptonScattering(const G4String& processName = "laserPhotoDetachment");
  virtual ~BDSLaserComptonScattering();
  virtual G4double GetMeanFreePath(const G4Track& track,
				   G4double previousStepSize,
				   G4ForceCondition* forceCondition);
  G4VParticleChange* PostStepDoIt(const G4Track& track ,
				  const G4Step&  step);
private:
  BDSAuxiliaryNavigator* auxNavigator;
  BDSComptonScatteringEngine* comptonEngine;
  G4double photonFluxTimeSum;
  G4int previousEventID;
  G4int currentEventID;

};

#endif
