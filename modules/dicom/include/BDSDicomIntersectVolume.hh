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

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#ifndef BDSDICOMINTERSECTVOLUME_H
#define BDSDICOMINTERSECTVOLUME_H

#include "G4PhantomParameterisation.hh"
#include "G4RotationMatrix.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4UImessenger.hh"

#include <fstream>
#include <vector>

class G4LogicalVolume;
class G4PhantomParameterisation;
class G4UIcommand;
class G4UIcmdWithAString;
class G4VPhysicalVolume;
class G4VSolid;

/**
 * @brief Manages intersections of DICOM files with volumes.
 *
 */

class BDSDicomIntersectVolume: public G4UImessenger
{
public:
  BDSDicomIntersectVolume();
  virtual ~BDSDicomIntersectVolume();
  
  virtual void SetNewValue(G4UIcommand* command,
			   G4String newValues);
  
private:
  void BuildUserSolid(std::vector<G4String> params);
  void BuildG4Solid(std::vector<G4String> params);
  G4PhantomParameterisation* GetPhantomParam(G4bool bMustExist);
  G4bool IsPhantomVolume(G4VPhysicalVolume* pv);
  std::vector<G4VPhysicalVolume*> GetPhysicalVolumes(const G4String& name,
						     G4bool exists,
						     G4int nVols);
  std::vector<G4LogicalVolume*> GetLogicalVolumes(const G4String&name,
						  G4bool exists,
						  G4int nVols);
  std::vector<G4String> GetWordsInString(const G4String& stemp);
  
private:
  G4UIcmdWithAString* fUserVolumeCmd;
  G4UIcmdWithAString* fG4VolumeCmd;
  G4VSolid* fSolid;
  std::ofstream fout;
  G4bool* fVoxelIsInside;
};
#endif
