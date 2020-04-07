/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSMaterials.hh"
#include "BDSScorerInfo.hh"
#include "BDSScorerType.hh"
#include "BDSUtilities.hh"

#include "parser/scorer.h"

#include "G4ParticleTable.hh"
#include "G4String.hh"
#include "G4Types.hh"


#include "CLHEP/Units/SystemOfUnits.h"

#include <map>
#include <string>
#include <vector>

BDSScorerInfo::BDSScorerInfo(const GMAD::Scorer& scorer,
			     G4bool upgradeTo3D):
  particle(nullptr)
{
  const std::map<std::string, std::string> replacements = {
							   {"cellcharge",      "cellcharge3d"},
							   {"depositeddose",   "depositeddose3d"},
							   {"depositedenergy", "depositedenergy3d"},
							   {"population",      "population3d"},
							   {"cellflux",        "cellflux3d"},
                               {"cellfluxscaled",  "cellfluxscaled3d"},
                               {"cellfluxscaledperparticle", "cellfluxscaledperparticle3d"}
  };
  
  std::string scorerTypeNameOriginal = scorer.type;
  std::string scorerTypeName         = scorerTypeNameOriginal; // default copy
  if (upgradeTo3D)
    {
      auto search = replacements.find(scorerTypeNameOriginal);
      if (search != replacements.end())
	{scorerTypeName = search->second;}
      else if (G4String(scorerTypeNameOriginal).contains("3d"))
	{throw BDSException(__METHOD_NAME__, "3D scorer required but a non-3D one specified.");}
    }
  
  scorerType    = BDS::DetermineScorerType(G4String(scorerTypeName));
  name          = scorer.name;
  minimumKineticEnergy = scorer.minimumKineticEnergy * CLHEP::GeV;
  maximumKineticEnergy = scorer.maximumKineticEnergy * CLHEP::GeV;
  filename      = scorer.conversionFactorFile;
  pathname      = scorer.conversionFactorPath;
  minimumTime   = scorer.minimumTime*CLHEP::second;
  maximumTime   = scorer.maximumTime*CLHEP::second;
  worldVolumeOnly = scorer.scoreWorldVolumeOnly;

  if (scorer.particlePDGID != 0)
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      particle = particleTable->FindParticle(scorer.particlePDGID);
      CheckParticle(particle, scorer.name);

    }
  if (!(scorer.particleName.empty()))
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      particle = particleTable->FindParticle(scorer.particleName);
      CheckParticle(particle, scorer.name);
    }

  for (const auto& mi : BDS::GetWordsFromString(scorer.materialToInclude))
    {materialsToInclude.push_back(BDSMaterials::Instance()->GetMaterial(mi));}

  for (const auto& me : BDS::GetWordsFromString(scorer.materialToExclude))
    {materialsToExclude.push_back(BDSMaterials::Instance()->GetMaterial(me));} 
}

void BDSScorerInfo::CheckParticle(G4ParticleDefinition* particleIn,
				  const G4String& nameIn)
{
  if (!particleIn)
    {
      G4cout << __METHOD_NAME__ << "Note, only 1 particle can be specified." << G4endl;
      throw BDSException(__METHOD_NAME__, "Particle not found for scorer " + nameIn);
    }
}
