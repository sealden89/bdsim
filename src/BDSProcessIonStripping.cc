/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSProcessIonStripping.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AtomicShells.hh"
#include "G4AutoDelete.hh"
#include "G4DynamicParticle.hh"
#include "G4MaterialCutsCouple.hh"
#include "G4ParticleDefinition.hh"
#include "G4VDiscreteProcess.hh"

#include <cmath>

// Cross section calculation from: I Yu Tolstikhina, V P Shevelko, Physics - Uspekhi 61 (3) 247 - 279 (2018)

BDSProcessIonStripping::BDSProcessIonStripping(const G4String& processName, G4ProcessType processType):
  G4VDiscreteProcess(processName, processType)
{;}

G4bool BDSProcessIonStripping::IsApplicable(const G4ParticleDefinition& p)
{
  return (p.GetPDGCharge() != 0.0 && !p.IsShortLived() &&
	  p.GetParticleType() == "nucleus");
}

G4double BDSProcessIonStripping::GetMeanFreePath(const G4Track&    track,
						 G4double          /*previousStepSize*/,
						 G4ForceCondition* /*condition*/)
{
  const G4MaterialCutsCouple* couple = track.GetMaterialCutsCouple();
  
  G4double interactionLength = DBL_MAX;
  
  if (track.GetDynamicParticle()->GetTotalOccupancy() > 0)
    {
      ///G4double crossSection = ComputeCrossSection(track, couple->GetMaterial())
      interactionLength = 1. / ComputeCrossSection(track, couple->GetMaterial());
    }
  
  //G4cout << "Interaction length: " << interactionLength/CLHEP::mm << " mm" << G4endl; //DEBUG
  ///G4cout << "------------------------" << G4endl; //DEBUG
  return interactionLength;
}

G4double BDSProcessIonStripping::ComputeCrossSection(const G4Track& aTrack, const G4Material* aMaterial)
{
  const G4DynamicParticle* ionIn = aTrack.GetDynamicParticle();
  const G4ElectronOccupancy* electronOccupancy = ionIn->GetElectronOccupancy();
  
  // We want stripping cross section so look at a frame of reference where the
  // incoming ion is the target and the material atoms are the projectiles
  G4double projectileEnergy = ionIn->GetTotalEnergy();
  
  G4double projectileZ = ionIn->GetDefinition()->GetAtomicNumber();
  G4double projectileCharge = ionIn->GetCharge();
  G4double projectileMass = ionIn->GetMass();
  
  // Obtain the dynamic qunatities needed
  G4int totalElectrons = electronOccupancy->GetTotalOccupancy();
  ///G4cout << "Total electron occupancy: " << totalElectrons << G4endl;
  
  /*
  // (!) Now using the Geant4 ionisation energy tables
  // Get the number of electrons and the orbit number
  G4int orbitNumber = 0;
  G4int orbitOccupancy = 0;
  for (int i = (int)electronOccupancy->GetSizeOfOrbit(); i>=0; i--)
  {
  if (electronOccupancy->GetOccupancy(i))
  {
  orbitNumber= i;
  orbitOccupancy = electronOccupancy->GetOccupancy(i);
  
  G4cout << "Electron occupancy in shell "<< orbitNumber << " : " << orbitOccupancy << G4endl;
  //break; //Only look in the outermost orbit
  }
  }
  */
  
  const G4ElementVector* theElementVector = aMaterial->GetElementVector();
  const G4double* NbOfAtomsPerVolume = aMaterial->GetVecNbOfAtomsPerVolume();
  
  //G4cout << "Charge of ion : " << projectileCharge << G4endl; //DEBUG
  ///G4cout << "Mass of ion : " << projectileMass/CLHEP::MeV << G4endl; //DEBUG
  ///G4cout << "Energy of ion : " << projectileEnergy/CLHEP::GeV << G4endl; //DEBUG
  ///G4cout << "Number of electrons : " << electronOccupancy->GetTotalOccupancy() << G4endl; //DEBUG
  ///G4cout << "Orbit number :"  << orbitNumber << G4endl; //DEBUG
  
  G4double sigma = 0.0;
  for (size_t i=0; i < aMaterial->GetNumberOfElements(); ++i)
    {
      G4double target_Z = (*theElementVector)[i]->GetZ();
      
      ///G4cout << "Number density [1/cm3]: "<< NbOfAtomsPerVolume[i]*CLHEP::cm3 <<G4endl; //DEBUG
      sigma += NbOfAtomsPerVolume[i] * ComputeCrossSectionPerAtom(projectileEnergy,
								  projectileMass,
								  totalElectrons,
								  target_Z,
								  projectileZ,
								  projectileCharge);
    }
  
  return sigma;
}

G4double BDSProcessIonStripping::ComputeCrossSectionPerAtom(G4double projectileEnergy,
                                                            G4double projectileMass,
                                                            G4double totalElectrons,
                                                            G4double target_Z,
                                                            G4double projectileZ,
                                                            G4double projectileCharge)
{
  // constants
  G4double unitRy   = 13.606*CLHEP::eV;//13.6*CLHEP::eV;
  G4double cLightAU = 137; //Speed of light in atomic units. Equal to 1/alpha (fine structure constant)
  
  // dynamic quantities
  G4double gamma = projectileEnergy / projectileMass;
  G4double beta  = std::sqrt(1. - 1./std::pow(gamma, 2));
  G4double v     = beta*cLightAU;
  
  // Ionisation potential of target, the 2 in the denominator comes from E0=2Ry
  // G4double I_nl_0 = unitRy*std::pow(projectileZ,2);//(2.*std::pow(orbitNumber+1,2));
  //[APPROXIMATION], only for hydrogen-like ions
  
  // Look up the geant4 tables for ionisation energy
  G4int nShells = G4AtomicShells::GetNumberOfShells(projectileZ);
  G4int nElectronsTotal = totalElectrons;
  G4double iNL = 0.0;
  G4double orbitN = 0.0;
  for (G4int i=0; i < nShells; ++i)
    {
      nElectronsTotal -= G4AtomicShells::GetNumberOfElectrons(projectileZ, i);
      if (nElectronsTotal <= 0)
        {
	  iNL = G4AtomicShells::GetBindingEnergy(projectileZ, i); // In units of eV
	  if (i < 1)
            {orbitN = 1;} // Pick the n quantum number of the orbital correctly
	  else if (i < 4)
            {orbitN = 2;}
	  else if (i < 9)
            {orbitN = 3;}
	  else if (i < 16)
            {orbitN = 4;}
	  else if (i < 21)
            {orbitN = 5;}
	  else if (i < 24)
            {orbitN = 6;}
	  
	  break;
        }
    }
  
  G4double u = std::pow(v, 2) / (iNL/unitRy); // Reduced energy
  
  ///G4cout << "===================================" << G4endl; //DEBUG
  ///G4cout << "ion mass : " << projectileMass/CLHEP::GeV << G4endl; //DEBUG
  ///G4cout << "ion tot energy : " << projectileEnergy/CLHEP::GeV << G4endl; //DEBUG
  ///G4cout << "beta : " << beta << G4endl; //DEBUG
  ///G4cout << "gamma : " << gamma << G4endl; //DEBUG
  ///G4cout << "u : " << u << G4endl; //DEBUG
  ///G4cout << "q : " << projectileCharge << G4endl; //DEBUG
  ///G4cout << "Inl [eV]: " << iNL << G4endl; //DEBUG
  ///G4cout << "Inl [Ry]: " << iNL/unitRy << G4endl; //DEBUG
  ///G4cout << "Ry : "<< unitRy << G4endl; //DEBUG
  ///G4cout << "Z target : " << target_Z << G4endl; //DEBUG
  ///G4cout << "Orbit n : " << orbitN << G4endl; //DEBUG
  
  G4double xSectionPerAtom = 0.88E-16 * std::pow(target_Z + 1, 2) * (u / (u * u + 3.5))
    * std::pow(unitRy / iNL, 1 + 0.01 * projectileCharge)
    * (4. + (1.31 / orbitN) * std::log(4 * u + 1.)) * CLHEP::cm2; //cross_section in cm^2/atom
  
  ///G4cout << "sig [cm^2/atom] = " << xSectionPerAtom/CLHEP::cm2 << G4endl; //DEBUG
  return xSectionPerAtom;
}

G4VParticleChange* BDSProcessIonStripping::PostStepDoIt(const G4Track& aTrack,
							const G4Step&  aStep)
{
  aParticleChange.Initialize(aTrack);
  
  const G4DynamicParticle *ionIn = aTrack.GetDynamicParticle();
  
  G4DynamicParticle* ionOut = new G4DynamicParticle(*ionIn);
  
  const G4ElectronOccupancy* electronOccupancy = ionOut->GetElectronOccupancy();
  for (int i = (int)electronOccupancy->GetSizeOfOrbit(); i >= 0; i--)
    {
      if (electronOccupancy->GetOccupancy(i))
        {
	  ionOut->RemoveElectron(i);
	  break; //Only remove the first electron encountered in the outermost orbit
        }
    }
  
  aParticleChange.SetNumberOfSecondaries(1);
  G4double localEnergyDeposit = 0.;
  
  aParticleChange.AddSecondary(ionOut);
  aParticleChange.ProposeLocalEnergyDeposit(localEnergyDeposit);
  
  // Stop the incident ion as the stripped one is emitted as a secondary
  aParticleChange.ProposeMomentumDirection(0.,0.,0.);
  aParticleChange.ProposeEnergy(0.);
  aParticleChange.ProposeTrackStatus(fStopAndKill);
  
  return G4VDiscreteProcess::PostStepDoIt(aTrack, aStep);
}
