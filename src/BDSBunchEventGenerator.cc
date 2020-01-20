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
#include "BDSBunchEventGenerator.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSPhysicsUtilities.hh"

#include "parser/beam.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string> // for stoi
#include <vector>

BDSBunchEventGenerator::BDSBunchEventGenerator():
  BDSBunch(),
  eventGeneratorMinX(0),
  eventGeneratorMaxX(0),
  eventGeneratorMinY(0),
  eventGeneratorMaxY(0),
  eventGeneratorMinZ(0),
  eventGeneratorMaxZ(0),
  eventGeneratorMinXp(0),
  eventGeneratorMaxXp(0),
  eventGeneratorMinYp(0),
  eventGeneratorMaxYp(0),
  eventGeneratorMinZp(0),
  eventGeneratorMaxZp(0),
  eventGeneratorMinT(0),
  eventGeneratorMaxT(0),
  eventGeneratorMinEK(0),
  eventGeneratorMaxEK(0),
  firstTime(true),
  testOnParticleType(true),
  acceptedParticlesString("")
{;}

BDSBunchEventGenerator::~BDSBunchEventGenerator() 
{;}

void BDSBunchEventGenerator::SetOptions(const BDSParticleDefinition* beamParticle,
					const GMAD::Beam&            beam,
					const BDSBunchType&          distrType,
					G4Transform3D                beamlineTransformIn,
					const G4double               beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);

  eventGeneratorMinX  = beam.eventGeneratorMinX * CLHEP::m;
  eventGeneratorMaxX  = beam.eventGeneratorMaxX * CLHEP::m;
  eventGeneratorMinY  = beam.eventGeneratorMinY * CLHEP::m;
  eventGeneratorMaxY  = beam.eventGeneratorMaxY * CLHEP::m;
  eventGeneratorMinZ  = beam.eventGeneratorMinZ * CLHEP::m;
  eventGeneratorMaxZ  = beam.eventGeneratorMaxZ * CLHEP::m;
  eventGeneratorMinXp = beam.eventGeneratorMinXp;
  eventGeneratorMaxXp = beam.eventGeneratorMaxXp;
  eventGeneratorMinYp = beam.eventGeneratorMinYp;
  eventGeneratorMaxYp = beam.eventGeneratorMaxYp;
  eventGeneratorMinZp = beam.eventGeneratorMinZp;
  eventGeneratorMaxZp = beam.eventGeneratorMaxZp;
  eventGeneratorMinT  = beam.eventGeneratorMinT * CLHEP::s;
  eventGeneratorMaxT  = beam.eventGeneratorMaxT * CLHEP::s;
  eventGeneratorMinEK = beam.eventGeneratorMinEK * CLHEP::GeV;
  eventGeneratorMaxEK = beam.eventGeneratorMaxEK * CLHEP::GeV;
  acceptedParticlesString = beam.eventGeneratorParticles;
}

void BDSBunchEventGenerator::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (eventGeneratorMinX >= eventGeneratorMaxX)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinX >= eventGeneratorMaxX");}
  if (eventGeneratorMinY >= eventGeneratorMaxY)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinY >= eventGeneratorMaxY");}
  if (eventGeneratorMinZ >= eventGeneratorMaxZ)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinZ >= eventGeneratorMaxZ");}
  if (eventGeneratorMinXp >= eventGeneratorMaxXp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinXp >= eventGeneratorMaxXp");}
  if (eventGeneratorMinYp >= eventGeneratorMaxYp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinYp >= eventGeneratorMaxYp");}
  if (eventGeneratorMinZp >= eventGeneratorMaxZp)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinZp >= eventGeneratorMaxZp");}
  if (eventGeneratorMinT >= eventGeneratorMaxT)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinT >= eventGeneratorMaxT");}
  if (eventGeneratorMinEK >= eventGeneratorMaxEK)
    {throw BDSException(__METHOD_NAME__, "eventGeneratorMinEK >= eventGeneratorMaxEK");}
}

void BDSBunchEventGenerator::ParseAcceptedParticleIDs()
{
  if (!acceptedParticlesString.empty())
    {
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      std::string particleIDStr;
      std::stringstream ss(acceptedParticlesString);
      while (ss >> particleIDStr)
	{
	  G4ParticleDefinition* particleDef = nullptr;
	  // try and see if it's an integer and therefore PDG ID, if not search by string
	  try
	    {
	      // we try this because std::stoi can throw a std::invalid_argument or
	      // std::out_of_range exception, both of which inherit std::logic_error
	      int particleID = std::stoi(particleIDStr);
	      // we don't use the G4ParticleTable->FindParticle(int) because it unnecessarily
	      // checks for physics readiness and throws an exception. here we just inspect
	      // the encoding dictionary ourselve. it's all typedeffed but it's
	      // std::map<G4int, G4ParticleDefinition*>
	      G4ParticleTable::G4PTblEncodingDictionary* encoding = G4ParticleTable::fEncodingDictionary;
	      auto search = encoding->find(particleID);
	      if (search != encoding->end())
		{
		  particleDef = search->second;
		  acceptedParticles.push_back(particleID);
		}
	      else
		{throw BDSException(__METHOD_NAME__,"PDG ID \"" + particleIDStr + "not found in particle table");}
	    }
	  catch (const std::logic_error&) // else, usual way by string search
	    {
	      particleDef = particleTable->FindParticle(particleIDStr);
	      if (!particleDef)
		{
		  BDS::PrintDefinedParticles();
		  throw BDSException(__METHOD_NAME__, "Particle \"" + particleIDStr + "\" not found.");      
		}
	      else
		{acceptedParticles.push_back(particleDef->GetPDGEncoding());}
	    }	
	}
      std::sort(acceptedParticles.begin(), acceptedParticles.end());
    }
  else
    {testOnParticleType = false;}
}

G4bool BDSBunchEventGenerator::AcceptParticle(const BDSParticleCoordsFull& coords,
					      G4double kineticEnergy,
					      G4int    pdgID)
{
  if (firstTime)
    {ParseAcceptedParticleIDs(); firstTime = false;}

  G4bool x  = coords.x  > eventGeneratorMinX  && coords.x  < eventGeneratorMaxX;
  G4bool y  = coords.y  > eventGeneratorMinY  && coords.y  < eventGeneratorMaxY;
  G4bool z  = coords.z  > eventGeneratorMinZ  && coords.z  < eventGeneratorMaxZ;
  G4bool xp = coords.xp > eventGeneratorMinXp && coords.xp < eventGeneratorMaxXp;
  G4bool yp = coords.yp > eventGeneratorMinYp && coords.yp < eventGeneratorMaxYp;
  G4bool zp = coords.zp > eventGeneratorMinZp && coords.zp < eventGeneratorMaxZp;
  G4bool t  = coords.T  > eventGeneratorMinT  && coords.T  < eventGeneratorMaxT;
  G4bool ek = kineticEnergy > eventGeneratorMinEK && kineticEnergy < eventGeneratorMaxEK;
  
  G4bool allowedParticle = true;
  if (testOnParticleType)
    {allowedParticle = std::binary_search(acceptedParticles.begin(), acceptedParticles.end(), pdgID);}
  
  return x && y && z && xp && yp && zp && t && ek && allowedParticle;
}
