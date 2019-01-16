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
#include "BDSVisManager.hh"

#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#ifdef G4VIS_USE
#include "G4UImanager.hh"        // G4 session managers
#endif
#include "G4UIExecutive.hh"
#endif

#include "G4TrajectoryDrawByCharge.hh"
#include "G4Version.hh"

#include "BDSDebug.hh"
#include "BDSMessenger.hh"
#include "BDSUtilities.hh"

BDSVisManager::BDSVisManager(G4String visMacroFileNameIn,
			     G4String geant4MacroFileNameIn):
  visMacroFileName(visMacroFileNameIn),
  geant4MacroFileName(geant4MacroFileNameIn)
{;}

void BDSVisManager::StartSession(G4int argc, char** argv)
{
  /// Create BDS UI messenger
  BDSMessenger* bdsMessenger = new BDSMessenger();

#ifdef G4UI_USE_TCSH
  G4UIsession* session = new G4UIterminal(new G4UItcsh);
#else
  G4UIsession* session = new G4UIterminal();
#endif

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
  G4cout<< __METHOD_NAME__ << "Initializing Visualisation Manager"<<G4endl;
#endif
  // initialize visualisation
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  
  // setup trajectory colouring
  G4TrajectoryDrawByCharge* trajModel1 = new G4TrajectoryDrawByCharge("trajModel1");
  visManager->RegisterModel(trajModel1);
  visManager->SelectTrajectoryModel(trajModel1->Name());
#endif
 
#ifdef G4UI_USE
  G4UIExecutive* session2 = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE

  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  // setup paths to look for macros for the install then the build directory
  UIManager->ApplyCommand("/control/macroPath @CMAKE_INSTALL_PREFIX@/share/bdsim/vis:@CMAKE_BINARY_DIR@/vis");

  G4String visMacName = visMacroFileName;
  G4String visMacPath = visMacName; // by default just copy it
  if (visMacName.empty()) // none specified - use default in BDSIM
    {
#ifdef G4VIS_USE_OPENGLQT
      visMacName = "vis.mac";
#else
      visMacName = "dawnfile.mac";
#endif
      // check if we find the file to at least let the user know what's being executed
      visMacPath = UIManager->FindMacroPath(visMacName);
      G4cout << __METHOD_NAME__ << "Visualisation macro name: " << visMacName << G4endl;
      G4cout << __METHOD_NAME__ << "Visualisation macro path: " << visMacPath << G4endl;
      if (visMacPath == visMacName) // this happens when the geant4 ui manager doesn't find the file in any directory
        {// behaviour found from geant4 source code inspection...
          G4cout << __METHOD_NAME__ << "vis.mac missing from BDSIM installation directory." << G4endl;
          return;
        }
    }
  else
    {// user specified visualisation macro - check if it exists
      if (BDS::FileExists(visMacPath) == false)
        {
          std::cout << __METHOD_NAME__ << "ERROR: visualisation file "
                    << visMacPath << " not present!" << G4endl;
          return;
        }
    }
  // execute the macro
  UIManager->ApplyCommand("/control/execute " + visMacPath);

  // apply optional macro if file name not empty
  if (!geant4MacroFileName.empty())
    {UIManager->ApplyCommand("/control/execute " + geant4MacroFileName);}
  
  
#if G4VERSION_NUMBER < 1030
  if (session2->IsGUI())
    {// these were added by default in Geant4.10.3 onwards
      UIManager->ApplyCommand("/control/execute icons.mac"); // add icons
      UIManager->ApplyCommand("/gui/addIcon \"Run beam on\" user_icon \"/run/beamOn 1\" run.png"); // add run icon
      UIManager->ApplyCommand("/control/execute gui.mac");   // add menus
    }
#endif
#endif
  // run gui
  session2->SessionStart();
  delete session2;
#endif
  delete session;
  delete bdsMessenger;
}
