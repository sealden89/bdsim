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
#ifndef BDSSTEPPINGACTION_H
#define BDSSTEPPINGACTION_H

#include "G4UserSteppingAction.hh"
#include "G4Types.hh"

/**
 * @brief Provide extra output for Geant4 through a verbose stepping action.
 */

class BDSSteppingAction: public G4UserSteppingAction
{
public:
  BDSSteppingAction();
  BDSSteppingAction(G4bool verboseStep,
		    G4int  verboseEventNumber = -1);
  virtual ~BDSSteppingAction();

  virtual void UserSteppingAction(const G4Step* step);

private:
  void VerboseSteppingAction(const G4Step* step);

  const G4bool verboseStep; ///< Cache of whether to print out verbose step information
  const G4int  verboseEventNumber; ///< Event number of desired increased verbosity.
};

#endif
