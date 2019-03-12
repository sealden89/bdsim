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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSIonDefinition.hh"
#include "BDSParticleDefinition.hh"
#if G4VERSION_NUMBER > 1039
#include "BDSPhysicsChannelling.hh"
#endif
#include "BDSPhysicsCutsAndLimits.hh"
#include "BDSPhysicsEMDissociation.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSUtilities.hh"
#include "BDSEmStandardPhysicsOp4Channelling.hh" // included with bdsim

#include "globals.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutron.hh"
#include "G4AntiProton.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"
#include "G4GenericBiasingPhysics.hh"
#include "G4GenericIon.hh"
#include "G4IonTable.hh"
#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4NeutrinoE.hh"
#include "G4Neutron.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTableIterator.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4Positron.hh"
#include "G4ProductionCutsTable.hh"
#include "G4Proton.hh"
#include "G4PhysListFactory.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"

#include "FTFP_BERT.hh"

#include <map>

G4VModularPhysicsList* BDS::BuildPhysics(const G4String& physicsList)
{
  G4VModularPhysicsList* result = nullptr;

  BDS::ConstructMinimumParticleSet();
  G4String physicsListNameLower = physicsList; // make lower case copy
  physicsListNameLower.toLower();
  G4bool useGeant4Physics = physicsListNameLower.contains("g4");
  G4bool completePhysics  = physicsListNameLower.contains("complete");
  if (useGeant4Physics)
    {
      // strip off G4_ prefix - from original as G4 factory case sensitive
      G4String geant4PhysicsList = physicsList.substr(2);
      G4PhysListFactory factory;
      if (!factory.IsReferencePhysList(geant4PhysicsList))
        {
          G4cerr << "Unknown Geant4 physics list \"" << geant4PhysicsList << "\"" << G4endl;
          G4cout << "Available Geant4 hadronic physics lists:" << G4endl;
          for (const auto &name : factory.AvailablePhysLists())
            {G4cout << "\"" << name << "\"" << G4endl;}
          G4cout << "Available Geant4 EM physics lists:" << G4endl;
          for (const auto &name : factory.AvailablePhysListsEM())
            {G4cout << "\"" << name << "\"" << G4endl;}
          throw BDSException(__METHOD_NAME__, "Unknown Geant4 physics list \"" + geant4PhysicsList + "\"");
        }
      else
        {
	  result = factory.GetReferencePhysList(geant4PhysicsList);
	  if (BDSGlobalConstants::Instance()->G4PhysicsUseBDSIMRangeCuts())
	    {BDS::SetRangeCuts(result);}
	  if (BDSGlobalConstants::Instance()->MinimumKineticEnergy() > 0 ||
	      BDSGlobalConstants::Instance()->G4PhysicsUseBDSIMCutsAndLimits())
	    {
	      G4cout << "\nWARNING - adding cuts and limits physics process to Geant4 reference physics list" << G4endl;
	      G4cout << "This is to enforce BDSIM range cuts and the minimumKinetic energy option.\n";
	      G4cout << "This can be turned off by setting option, g4PhysicsUsesBDSIMCutsAndLimits=0;\n" << G4endl;
	      result->RegisterPhysics(new BDSPhysicsCutsAndLimits());
	    }
	}
    }
  else if (completePhysics)
    {// we test one by one for the exact name of very specific physics lists
      if (physicsListNameLower == "completechannelling" || physicsListNameLower == "completechannellingemd")
	{
#if G4VERSION_NUMBER > 1039
	  G4bool useEMD = physicsListNameLower.contains("emd");
	  // we don't assign 'result' variable or proceed as that would result in the
	  // range cuts being set for a complete physics list that we wouldn't use
	  return BDS::ChannellingPhysicsComplete(useEMD);
#else
	  throw BDSException(__METHOD_NAME__, "Channel physics is not supported with Geant4 versions less than 10.4");
#endif
	}
      else
	{throw BDSException(__METHOD_NAME__, "Unknown 'complete' physics list \"" + physicsList + "\"");}
    }
  else
    {
      result = new BDSModularPhysicsList(physicsList);
      BDS::SetRangeCuts(result); // always set our range cuts for our physics list
    }
  BDS::CheckAndSetEnergyValidityRange();
  result->ConstructParticle(); // force construction of the particles
  return result;
}

void BDS::ConstructDesignAndBeamParticle(const GMAD::BeamBase& beamDefinition,
					 G4double ffact,
					 BDSParticleDefinition*& designParticle,
					 BDSParticleDefinition*& beamParticle,
					 G4bool& beamDifferentFromDesignParticle)
{
  G4String designParticleName = G4String(beamDefinition.particle);
  G4double designTotalEnergy = G4double(beamDefinition.beamEnergy)*CLHEP::GeV;
  designParticle = BDS::ConstructParticleDefinition(designParticleName, designTotalEnergy, ffact);
  if ((beamDefinition.particle == beamDefinition.beamParticleName) &&
      (beamDefinition.beamEnergy == beamDefinition.E0))
    {// copy definition
      beamParticle = new BDSParticleDefinition(*designParticle);
      beamDifferentFromDesignParticle = false;
    }
  else
    {
      G4String beamParticleName = G4String(beamDefinition.beamParticleName);
      G4double beamTotalEnergy  = G4double(beamDefinition.E0)*CLHEP::GeV;
      beamParticle = BDS::ConstructParticleDefinition(beamParticleName,
						      beamTotalEnergy,
						      ffact);
      beamDifferentFromDesignParticle = true;
    }
}

BDSParticleDefinition* BDS::ConstructParticleDefinition(G4String particleNameIn,
							G4double totalEnergy,
							G4double ffact)
{
  BDSParticleDefinition* particleDefB = nullptr; // result
  G4String particleName = particleNameIn; // copy the name
  particleName.toLower();

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  if (particleName.contains("ion"))
    {
      G4GenericIon::GenericIonDefinition(); // construct general ion particle
      auto ionDef = new BDSIonDefinition(particleName); // parse the ion definition

      G4IonTable* ionTable = particleTable->GetIonTable();
      G4double mass   = ionTable->GetIonMass(ionDef->Z(), ionDef->A());
      G4double charge = ionDef->Charge(); // correct even if overridden
      particleDefB = new BDSParticleDefinition(particleName, mass, charge,
					       totalEnergy, ffact, ionDef);
      // this particle definition takes ownership of the ion definition
    }
  else
    {
      BDS::ConstructBeamParticleG4(particleName);
      auto particleDef = particleTable->FindParticle(particleName);
      if (!particleDef)
	{
	  G4cout << "Available particles are:" << G4endl;
	  auto pt = G4ParticleTable::GetParticleTable();
	  auto it = pt->GetIterator();
	  while ((*it)()) // iterate over all particles defined and print out names
	    {G4cout << it->value()->GetParticleName() << G4endl;}
	  throw BDSException(__METHOD_NAME__, "Particle \"" + particleName + "\" not found.");
	}
      particleDefB = new BDSParticleDefinition(particleDef, totalEnergy, ffact);
    }
  return particleDefB;
}

void BDS::ConstructBeamParticleG4(G4String name)
{
  if (name == "proton")
    {G4Proton::ProtonDefinition();}
  else if (name == "antiproton")
    {G4AntiProton::AntiProtonDefinition();}
  else if (name == "e-")
    {G4Electron::ElectronDefinition();}
  else if (name == "e+")
    {G4Positron::PositronDefinition();}
  else if (name == "pi-")
    {G4PionMinus::PionMinusDefinition();}
  else if (name == "pi+")
    {G4PionPlus::PionPlusDefinition();}
  else if (name == "neutron")
    {G4Neutron::NeutronDefinition();}
  else if (name == "photon" || name == "gamma")
    {G4Gamma::Gamma();}
  else if (name == "mu-")
    {G4MuonMinus::MuonMinusDefinition();}
  else if (name == "mu+")
    {G4MuonPlus::MuonPlusDefinition();}
  else
    {
      G4cout << "Unknown common particle type \"" << name << "\"" << G4endl;
      G4cout << "Attempting to search physics list particles." << G4endl;
    }
}

void BDS::ConstructMinimumParticleSet()
{
  // e-, e+, v_e, v_e(bar)
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();

  // p, pbar, neutron, anti-neutron
  G4Proton::ProtonDefinition();
  G4AntiProton::AntiProtonDefinition();
  G4Neutron::NeutronDefinition();
  G4AntiNeutron::AntiNeutronDefinition();

  // photon
  G4Gamma::Gamma();
}

void BDS::PrintPrimaryParticleProcesses(const G4String& primaryParticleName)
{
  G4cout << "Register physics processes by name for the primary particle \""
	 << primaryParticleName << "\":" << G4endl;

  auto particle = G4ParticleTable::GetParticleTable()->FindParticle(primaryParticleName);
  if (!particle)
    {// could be ion that isn't defined
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "primary particle not defined yet - could be ion" << G4endl;
#endif
      return;
    }
  auto pl = particle->GetProcessManager()->GetProcessList();
  for (G4int i = 0; i < pl->length(); i++)
    {G4cout << "\"" << (*pl)[i]->GetProcessName() << "\"" << G4endl;}
}


G4GenericBiasingPhysics* BDS::BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases)
{
  // particles we know we can bias
  std::map<G4String, G4bool> particlesToBias =
    {
      {"e-"     , false},
      {"e+"     , false},
      {"gamma"  , false},
      {"proton" , false},
      {"mu-"    , false},
      {"mu+"    , false},
      {"pi-"    , false},
      {"pi+"    , false}
    };

  // iterate through bias structures and turn on biasing for that particle if it's in the
  // map of acceptable particle definitions.
  for (auto b : biases)
    {
      G4String name = G4String(b.particle);
      if (particlesToBias.find(name) != particlesToBias.end())
	{particlesToBias[name] = true;}
      else
	{throw BDSException(__METHOD_NAME__, "Not possible to bias \"" + name + "\"");}
    }

  // check whether we need to construct or attach biasing at all
  typedef std::pair<const G4String, G4bool> mapvalue;
  G4bool anyBiases = std::any_of(particlesToBias.begin(),
				 particlesToBias.end(),
				 [](mapvalue i){return i.second;});

  if (!anyBiases)
    {return nullptr;}

  // there are biases
  G4GenericBiasingPhysics* physBias = new G4GenericBiasingPhysics();
  for (auto part : particlesToBias)
    {
      if (part.second)
        {
          G4cout << __METHOD_NAME__ << "wrapping \"" << part.first << "\" for biasing" << G4endl;
          physBias->Bias(part.first);
        }
    }
  return physBias;
}

void BDS::PrintDefinedParticles()
{
  G4cout << __METHOD_NAME__ << "Defined particles: " << G4endl;
  auto it = G4ParticleTable::GetParticleTable()->GetIterator();
  it->reset();
  while ((*it)())
    {G4cout <<  it->value()->GetParticleName() << " ";}
  G4cout << G4endl;
}

#if G4VERSION_NUMBER > 1039
G4VModularPhysicsList* BDS::ChannellingPhysicsComplete(const G4bool useEMD)
{
  G4VModularPhysicsList* physlist = new FTFP_BERT();
  G4GenericBiasingPhysics* biasingPhysics = new G4GenericBiasingPhysics();
  physlist->RegisterPhysics(new BDSPhysicsChannelling());
  // replace the EM physics in the Geant4 provided FTFP_BERT composite physics list
  physlist->ReplacePhysics(new BDSEmStandardPhysicsOp4Channelling());

  // optional electromagnetic dissociation that isn't in FTFP_BERT by default
  if (useEMD)
    {physlist->RegisterPhysics(new BDSPhysicsEMDissociation());}

  biasingPhysics->PhysicsBiasAllCharged();
  physlist->RegisterPhysics(biasingPhysics);
  return physlist;
}
#endif

void BDS::SetRangeCuts(G4VModularPhysicsList* physicsList)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();

  // set default value
  physicsList->SetDefaultCutValue(globals->DefaultRangeCut());

  // overwrite when explicitly set in options
  if (globals->ProdCutPhotonsSet())
    {physicsList->SetCutValue(globals->ProdCutPhotons(),  "gamma");}
  if (globals->ProdCutElectronsSet())
    {physicsList->SetCutValue(globals->ProdCutElectrons(),"e-");}
  if (globals->ProdCutPositronsSet())
    {physicsList->SetCutValue(globals->ProdCutPositrons(),"e+");}
  if (globals->ProdCutProtonsSet())
    {physicsList->SetCutValue(globals->ProdCutProtons(),  "proton");}

  G4cout << __METHOD_NAME__ << "Default production range cut  " << physicsList->GetDefaultCutValue()  << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Photon production range cut   " << physicsList->GetCutValue("gamma")  << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Electron production range cut " << physicsList->GetCutValue("e-")     << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Positron production range cut " << physicsList->GetCutValue("e+")     << " mm" << G4endl;
  G4cout << __METHOD_NAME__ << "Proton production range cut   " << physicsList->GetCutValue("proton") << " mm" << G4endl;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "List of all constructed particles by physics lists" << G4endl;
  for (auto particle : *G4ParticleTable::fDictionary)
    {G4cout << particle.second->GetParticleName() << ", ";}
  G4cout << G4endl;
#endif

  physicsList->DumpCutValuesTable();
}

void BDS::CheckAndSetEnergyValidityRange()
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  G4double energyLimitLow     = globals->PhysicsEnergyLimitLow();
  G4double energyLimitHigh    = globals->PhysicsEnergyLimitHigh();
  G4bool   setEnergyLimitLow  = BDS::IsFinite(energyLimitLow);
  G4bool   setEnergyLimitHigh = BDS::IsFinite(energyLimitHigh);
  if (setEnergyLimitLow || setEnergyLimitHigh)
    {
      auto table = G4ProductionCutsTable::GetProductionCutsTable();
      G4double defaultEnergyLimitLow  = table->GetLowEdgeEnergy();
      G4double defaultEnergyLimitHigh = table->GetHighEdgeEnergy();
      G4double elLow  = setEnergyLimitLow  ? energyLimitLow  : defaultEnergyLimitLow;
      G4double elHigh = setEnergyLimitHigh ? energyLimitHigh : defaultEnergyLimitHigh;
      table->SetEnergyRange(elLow, elHigh);
      if (setEnergyLimitLow)
	{
	  G4cout << __METHOD_NAME__ << "set low energy limit:  "
		 << elLow/CLHEP::MeV  << " MeV" << G4endl;
	}
      if (setEnergyLimitHigh)
	{
	  G4cout << __METHOD_NAME__ << "set high energy limit: "
		 << elHigh/CLHEP::TeV << " TeV" << G4endl;
	}
    }
}
