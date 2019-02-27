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

#ifndef BDSIONIONISATIONPROCESS_H
#define BDSIONIONISATIONPROCESS_H

#include "G4ionIonisation.hh"

#include "globals.hh" // geant4 types / globals

class G4Step;
class G4Track;

/**
 * @brief A physics process that wraps G4ionIonisation to prevent forcing of ion charge equilibrium with media
 *
 * @author Andrey Abramov
 */

class BDSProcessIonIonisation: public G4ionIonisation
{
public:
    using G4ionIonisation::G4ionIonisation;
    virtual ~BDSProcessIonIonisation(){;}

    // AlongStep computations
    virtual G4VParticleChange* AlongStepDoIt(const G4Track&,
                                             const G4Step&) override;
};

#endif