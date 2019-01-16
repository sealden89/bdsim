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
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSDump.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "BDSDumpSD.hh"
#include "G4SDManager.hh"

BDSDumpSD* BDSDumpSensDet;

int BDSDump::nDumps=0;

BDSDump::BDSDump (G4String name, G4double length):
  BDSAcceleratorComponent(name, length, 0, "dump")
{
  //SetName("Dump_"+std::to_string(nDumps)+"_"+itsName);
  ++nDumps;
  //BDSRoot->SetDumpNumber(nDumps);
}

void BDSDump::BuildContainerLogicalVolume()
{
  G4double radius = BDSGlobalConstants::Instance()->SamplerDiameter() * 0.5;
  
  containerSolid = new G4Box(name + "_container_solid",
			     radius,
			     radius,
			     chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

#ifndef NOUSERLIMITS
  G4UserLimits* itsOuterUserLimits = new G4UserLimits();
  itsOuterUserLimits->SetMaxAllowedStep(chordLength);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  containerLogicalVolume->SetUserLimits(itsOuterUserLimits);
  RegisterUserLimits(itsOuterUserLimits);
#endif
  // Sensitive Detector:
  if(nDumps==0)
    {
      G4SDManager* SDMan = G4SDManager::GetSDMpointer();
      BDSDumpSensDet=new BDSDumpSD(name,"plane");
      SDMan->AddNewDetector(BDSDumpSensDet);
    }
  containerLogicalVolume->SetSensitiveDetector(BDSDumpSensDet);
}

BDSDump::~BDSDump()
{
  nDumps--;
}
