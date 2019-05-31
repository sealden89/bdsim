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

#include "BDSOutputROOTGeant4Data.hh"

#include <map>

#ifndef __ROOTBUILD__
#include "G4IonTable.hh"
#include "G4NuclideTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"

#include <string>
#endif

ClassImp(BDSOutputROOTGeant4Data)

BDSOutputROOTGeant4Data::BDSOutputROOTGeant4Data()
{;}

void BDSOutputROOTGeant4Data::Flush()
{
  particles.clear();
  ions.clear();
}

const BDSOutputROOTGeant4Data::ParticleInfo BDSOutputROOTGeant4Data::GetParticleInfo(const int& pdgID) const
{
  auto result = particles.find(pdgID);
  if (result != particles.end())
    {return result->second;}
  else
    {return ParticleInfo();}
}

const BDSOutputROOTGeant4Data::IonInfo BDSOutputROOTGeant4Data::GetIonInfo(const int& pdgID) const
{
  auto result = ions.find(pdgID);
  if (result != ions.end())
    {return result->second;}
  else
    {return IonInfo();}
}

int BDSOutputROOTGeant4Data::Charge(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.charge;}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.charge;}
      else
	{return 0;}
    }
}

double BDSOutputROOTGeant4Data::Mass(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.mass;}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.mass;}
      else
	{return 0;}
    }
}

double BDSOutputROOTGeant4Data::Rigidity(const int&    pdgID,
					 const double& totalEnergy) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.rigidity(totalEnergy);}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.rigidity(totalEnergy);}
      else
	{return 0;}
    }
}

double BDSOutputROOTGeant4Data::KineticEnergy(const int&    pdgID,
					      const double& totalEnergy) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
        {return totalEnergy - result->second.mass;}
      else
        {return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
        {return totalEnergy - result->second.mass;}
      else
        {return 0;}
    }
}

std::string BDSOutputROOTGeant4Data::Name(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.name;}
      else
	{return "";}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.name;}
      else
	{return "";}
    }
}

int BDSOutputROOTGeant4Data::IonA(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.a;}
      else
	{return 0;}
    }
  else if (pdgID == 2212)
    {return 1;}
  else
    {return 0;}
}

int BDSOutputROOTGeant4Data::IonZ(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.z;}
      else
	{return 0;}
    }
  else if (pdgID == 2212)
    {return 1;}
  else
    {return 0;}
}

#ifndef __ROOTBUILD__
void BDSOutputROOTGeant4Data::Fill(const G4bool& fillIons)
{
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();

  // iterator over particles - non-standard iterator type
  auto it = pt->GetIterator();
  it->reset();
  while ((*it)() )
    {
      const G4ParticleDefinition* particle = it->value();
      const G4String& particleName = particle->GetParticleName();

      int pdgID = static_cast<int>(particle->GetPDGEncoding());

      BDSOutputROOTGeant4Data::ParticleInfo info = {(std::string)particleName,
						    (int)particle->GetPDGCharge(),
						    (double)particle->GetPDGMass()/CLHEP::GeV};
      particles[pdgID] = info;
    }

  if (fillIons)
    {// proceed to fill ion information as it was used in the simulation
      // A frankly ridiculous interface to get all possible ions with pdg encoding and mass.
      // G4IonTable uses G4NuclideTable that loads file data with Geant4. We then iterate
      // over the nuclide table and query the ion table, which in turn creates the definitions
      // of the ions required as we query them and can then calcualte and provide the mass
      // and pdg encoding. Normally, the ion table would only provide just a few light ions.
      G4IonTable* ionTable = pt->GetIonTable();
      G4NuclideTable* table = G4NuclideTable::GetInstance();
      unsigned int number = (unsigned int)table->GetSizeOfIsotopeList();
      for (unsigned int i = 0; i < number; i++)
	{
	  G4IsotopeProperty* iso = table->GetIsotopeByIndex(i);
	  int atmass = iso->GetAtomicMass();
	  int atnum  = iso->GetAtomicNumber();
	  double eng = iso->GetEnergy();
	  // using the excited energy always works. using the (int)lvl doesn't
	  G4ParticleDefinition* def = ionTable->GetIon(atnum, atmass, eng);

	  // package the information we need
	  BDSOutputROOTGeant4Data::IonInfo ionDef = {(std::string)def->GetParticleName(),
						     (int)def->GetPDGCharge(),
						     (double)def->GetPDGMass()/CLHEP::GeV,
						     (int)def->GetAtomicMass(),
						     (int)def->GetAtomicNumber()};
	  ions[def->GetPDGEncoding()] = ionDef;
	}
    }

}
#endif
