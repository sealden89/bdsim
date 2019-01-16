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
#ifndef BDSRUNACTION_H
#define BDSRUNACTION_H

#include "globals.hh" // geant4 types / globals
#include "G4UserRunAction.hh"

#include <ctime>
#include <string>

class BDSBunch;
class BDSEventInfo;
class BDSOutput;
class G4Run;

/**
 * @brief Control over the beginning and end of run actions.
 *
 * Unlike the regular Geant4 run action we call a beginning of run
 * action on the bunch distribution (when we know the number of events
 * to run).
 */

class BDSRunAction: public G4UserRunAction
{
public:
  explicit BDSRunAction(BDSOutput* outputIn,
			BDSBunch*  bunchGeneratorIn,
			const G4bool& usingIonsIn);
  virtual ~BDSRunAction();
  
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

private:
  BDSOutput*    output;         ///< Cache of output instance. Not owned by this class.
  time_t        starttime;
  std::string   seedStateAtStart; ///< Seed state at start of the run.
  BDSEventInfo* info;
  BDSBunch*     bunchGenerator;   ///< Cache of bunch generator.
  const G4bool  usingIons;        ///< Cache of whether ions are being used (for particle table writing out).
};

#endif

