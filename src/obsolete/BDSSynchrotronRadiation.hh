/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#ifndef BDSSYNCHROTRONRADIATION_H
#define BDSSYNCHROTRONRADIATION_H

#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "globals.hh"
#include "G4VDiscreteProcess.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

class BDSSynchrotronRadiation: public G4VDiscreteProcess 
{ 
public:
  BDSSynchrotronRadiation(const G4String& processName = "BDSSynchRad");
 
  virtual ~BDSSynchrotronRadiation();

  G4bool IsApplicable(const G4ParticleDefinition&);
  
  virtual G4double GetMeanFreePath(const G4Track& track,
				   G4double previousStepSize,
				   G4ForceCondition* condition );
  
  virtual G4VParticleChange *PostStepDoIt(const G4Track& track,         
					  const G4Step&  step);                 

  G4double SynGenC(G4double xmin);
  G4double SynRadC(G4double x);

protected:

private:

  // assignment and copy constructor not implemented nor used
  BDSSynchrotronRadiation & operator=(const BDSSynchrotronRadiation &right);
  BDSSynchrotronRadiation(const BDSSynchrotronRadiation&);

  G4double nExpConst;
  G4double CritEngFac;
  G4int MeanFreePathCounter;
};

inline G4bool 
BDSSynchrotronRadiation::IsApplicable(const G4ParticleDefinition& particle)
{
  return(  (&particle == G4Electron::Electron())
	   ||(&particle == G4Positron::Positron()) );
}

#endif
