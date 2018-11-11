/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2018.

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

#ifndef BDSIONSTRIPPINGPROCESS_H
#define BDSIONSTRIPPINGPROCESS_H

#include "G4VDiscreteProcess.hh"

#include "globals.hh" // geant4 types / globals

/**
 * @brief A physics process that perform stripping of ions
 *
 * @author Andrey Abramov
 */

class BDSProcessIonStripping: public G4VDiscreteProcess
{
public:
    BDSProcessIonStripping(const G4String& ,
                           G4ProcessType   aType = fNotDefined);
    virtual ~BDSProcessIonStripping(){;}

    virtual G4double PostStepGetPhysicalInteractionLength(
            const G4Track& track,
            G4double   previousStepSize,
            G4ForceCondition* condition
    );

    virtual G4VParticleChange* PostStepDoIt(
            const G4Track& ,
            const G4Step&
    );

protected:
    virtual G4double GetMeanFreePath(const G4Track& aTrack,
                                     G4double   previousStepSize,
                                     G4ForceCondition* condition
    );
};

#endif