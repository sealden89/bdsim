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
#include "BDSBunch.hh"
#include "BDSDebug.hh"
#include "BDSEventInfo.hh"
#include "BDSOutput.hh"
#include "BDSParser.hh"
#include "BDSRunAction.hh"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include "globals.hh"               // geant4 globals / types
#include "G4Run.hh"

#include "CLHEP/Random/Random.h"

#include <sstream>
#include <string>

BDSRunAction::BDSRunAction(BDSOutput*    outputIn,
                           BDSBunch*     bunchGeneratorIn,
			   const G4bool& usingIonsIn):
  output(outputIn),
  starttime(time(nullptr)),
  seedStateAtStart(""),
  info(nullptr),
  bunchGenerator(bunchGeneratorIn),
  usingIons(usingIonsIn)
{;}

BDSRunAction::~BDSRunAction()
{
  delete info;
}

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  // Bunch generator beginning of run action (optional mean subtraction).
  bunchGenerator->BeginOfRunAction(aRun->GetNumberOfEventToBeProcessed());
  
  info = new BDSEventInfo();
  
  // save the random engine state
  std::stringstream ss;
  CLHEP::HepRandom::saveFullState(ss);
  seedStateAtStart = ss.str();
  info->SetSeedStateAtStart(seedStateAtStart);
  
  // get the current time
  starttime = time(nullptr);
  info->SetStartTime(starttime);
  
  // Output feedback
  G4cout << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " start. Time is " << asctime(localtime(&starttime)) << G4endl;

  output->InitialiseGeometryDependent();
  output->NewFile();

  // Write options now file open.
  const GMAD::OptionsBase* ob = BDSParser::Instance()->GetOptionsBase();
  output->FillOptions(ob);

  // Write beam
  const GMAD::BeamBase* bb = BDSParser::Instance()->GetBeamBase();
  output->FillBeam(bb);

  // Write model now file open.
  output->FillModel();

  // Write out geant4 data including particle tables.
  output->FillGeant4Data(usingIons);
}

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{
  // Get the current time
  time_t stoptime = time(nullptr);
  info->SetStopTime(stoptime);
  // Run duration
  G4float duration = difftime(stoptime, starttime);
  info->SetDuration(G4double(duration));

  // Possible end of run action - in some cases resets pregenerated coordinates
  bunchGenerator->EndOfRunAction();
  
  // Output feedback
  G4cout << G4endl << __METHOD_NAME__ << "Run " << aRun->GetRunID()
	 << " end. Time is " << asctime(localtime(&stoptime));
  
  // Write output
  output->FillRun(info);
  output->CloseFile();
  info->Flush();

  // note difftime only calculates to the integer second
  G4cout << __METHOD_NAME__ << "Run Duration >> " << (int)duration << " s" << G4endl;
}
