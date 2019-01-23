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
#include <cstdlib>

#include "BDSGlobalConstants.hh"

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSFieldInfo.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSOutputType.hh"
#include "BDSPTCOneTurnMap.hh"
#include "BDSParser.hh"
#include "BDSSamplerPlane.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTunnelInfo.hh"

#include "globals.hh"
#include "G4Colour.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

BDSGlobalConstants* BDSGlobalConstants::instance = nullptr;

BDSGlobalConstants* BDSGlobalConstants::Instance()
{
  if (!instance)
    {instance = new BDSGlobalConstants(BDSParser::Instance()->GetOptions(),
				       BDSParser::Instance()->GetBeam());}
  return instance;
}

BDSGlobalConstants::BDSGlobalConstants(const GMAD::Options& opt,
				       GMAD::Beam&          beamIn):
  options(opt),
  beam(beamIn),
  turnsTaken(0)
{
  ResetTurnNumber();
  outputType = BDS::DetermineOutputType(options.outputFormat);

  numberToGenerate = G4int(options.nGenerate);

  samplerDiameter = G4double(options.samplerDiameter)*CLHEP::m;

  // beampipe
  defaultBeamPipeModel = new BDSBeamPipeInfo(options.apertureType,
					     options.aper1 * CLHEP::m,
					     options.aper2 * CLHEP::m,
					     options.aper3 * CLHEP::m,
					     options.aper4 * CLHEP::m,
					     options.vacMaterial,
					     options.beampipeThickness * CLHEP::m,
					     options.beampipeMaterial);
  
  // magnet geometry
  G4double horizontalWidth = options.horizontalWidth * CLHEP::m;
  if (horizontalWidth < 2*(defaultBeamPipeModel->beamPipeThickness + defaultBeamPipeModel->aper1))
    {
      G4cerr << __METHOD_NAME__ << "Error: option \"horizontalWidth\" " << horizontalWidth
	     << " must be greater than 2x (\"aper1\" + \"beamPipeThickness\") ("
	     << defaultBeamPipeModel->aper1 << " + " << defaultBeamPipeModel->beamPipeThickness << ")" << G4endl;
      exit(1);
    }
  magnetGeometryType = BDS::DetermineMagnetGeometryType(options.magnetGeometryType);

  // tunnel
  tunnelInfo = new BDSTunnelInfo(options.tunnelType,
				 options.tunnelThickness     * CLHEP::m,
				 options.tunnelSoilThickness * CLHEP::m,
				 options.tunnelMaterial,
				 options.soilMaterial,
				 options.buildTunnelFloor,
				 options.tunnelFloorOffset   * CLHEP::m,
				 options.tunnelAper1         * CLHEP::m,
				 options.tunnelAper2         * CLHEP::m,
				 options.storeElossTunnel,
				 options.tunnelVisible);
  
  // defaults - parameters of the laserwire process
  itsLaserwireWavelength = 0.532 * CLHEP::micrometer;
  itsLaserwireDir = G4ThreeVector(1,0,0);
  itsLaserwireTrackPhotons = true;
  itsLaserwireTrackElectrons = true;

  // initialise the default vis attributes and user limits that
  // can be copied by various bits of geometry
  InitVisAttributes();
  InitDefaultUserLimits();

  integratorSet = BDS::DetermineIntegratorSetType(options.integratorSet);

  InitialiseBeamlineTransform();

  BDSSamplerPlane::chordLength = LengthSafety();

  ProcessTrajectorySamplerIDs();
  ProcessTrajectoryELossSRange();
}

void BDSGlobalConstants::InitialiseBeamlineTransform()
{  
  G4ThreeVector offset = G4ThreeVector(options.beamlineX * CLHEP::m,
				       options.beamlineY * CLHEP::m,
				       options.beamlineZ * CLHEP::m);
  
  G4RotationMatrix rm;
  if (options.beamlineAxisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(options.beamlineAxisX,
					 options.beamlineAxisY,
					 options.beamlineAxisZ);

      rm = G4RotationMatrix(axis, options.beamlineAngle * CLHEP::rad);
    }
  else
    {
      G4double phi   = options.beamlinePhi   * CLHEP::rad;
      G4double theta = options.beamlineTheta * CLHEP::rad;
      G4double psi   = options.beamlinePsi   * CLHEP::rad;
      CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(phi, theta, psi);
      rm = G4RotationMatrix(ang);
    }
  
  beamlineTransform = G4Transform3D(rm, offset);
}

void BDSGlobalConstants::InitVisAttributes()
{
  //for vacuum volumes
  invisibleVisAttr = new G4VisAttributes(G4Colour::Black());
  invisibleVisAttr->SetVisibility(false);
  invisibleVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);

  //for normally invisible volumes like marker / container volumes in debug mode
  visibleDebugVisAttr = new G4VisAttributes(); //green
  visibleDebugVisAttr->SetColour(0,0.6,0,0.1);
  visibleDebugVisAttr->SetVisibility(true);
  visibleDebugVisAttr->SetForceLineSegmentsPerCircle(options.nSegmentsPerCircle);
}

void BDSGlobalConstants::InitDefaultUserLimits()
{
  defaultUserLimits = new G4UserLimits("default_cuts");
  const G4double maxTime = MaxTime();
  if (maxTime > 0)
    {
      G4cout << __METHOD_NAME__ << "Setting maximum tracking time to " << maxTime << " ns" << G4endl;
      defaultUserLimits->SetUserMaxTime(maxTime);
    }
  defaultUserLimits->SetMaxAllowedStep(MaxStepLength());
  defaultUserLimits->SetUserMaxTrackLength(MaxTrackLength());
  defaultUserLimits->SetUserMinEkine(MinimumKineticEnergy());
  defaultUserLimits->SetUserMinRange(MinimumRange());

  BDSFieldInfo::defaultUL = defaultUserLimits; // update static member for field definitions
}

G4int BDSGlobalConstants::PrintModuloEvents() const
{
  G4int nGenerate = NGenerate();
  G4double fraction = PrintFractionEvents();
  G4int printModulo = (G4int)ceil(nGenerate * fraction);
  if (printModulo < 0)
    {printModulo = 1;}

  if (!Batch())
    {printModulo = 1;} // interative -> print every event
  return printModulo;
}

G4int BDSGlobalConstants::PrintModuloTurns() const
{
  G4int nTurns = TurnsToTake();
  G4double fraction = PrintFractionTurns();
  G4int printModulo = (G4int)ceil(nTurns * fraction);
  if (printModulo < 0)
    {printModulo = 1;}

  if (!Batch())
    {printModulo = 1;} // interative -> print every turn
  return printModulo;
}

BDSGlobalConstants::~BDSGlobalConstants()
{  
  delete defaultBeamPipeModel;
  delete tunnelInfo;
  delete defaultUserLimits;
  delete invisibleVisAttr;
  delete visibleDebugVisAttr;

  instance = nullptr;
}

void BDSGlobalConstants::ProcessTrajectorySamplerIDs()
{
  if (options.storeTrajectorySamplerID.empty())
    {return;}
  std::istringstream is(options.storeTrajectorySamplerID);
  G4String tok;
  while (is >> tok)
    {
      BDSSamplerRegistry* samplerRegistry = BDSSamplerRegistry::Instance();
      G4int i = 0;
      for (const auto& info : *samplerRegistry)
        {
          if (info.UniqueName() == tok)
            {samplerIDs.push_back(i);}
	  i++;
        }
    }
}

void BDSGlobalConstants::ProcessTrajectoryELossSRange()
{
  if (options.storeTrajectoryELossSRange.empty())
    {return;}
  std::istringstream is(options.storeTrajectoryELossSRange);
  std::string tok;
  while (is >> tok)
    {
      G4int loc = tok.find(":",0);
      G4double rstart = std::stod(tok.substr(0, loc));
      G4double rend   = std::stod(tok.substr(loc+1,tok.size()));
      elossSRange.emplace_back(rstart, rend);
    }
}
