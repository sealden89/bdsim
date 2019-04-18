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
#include "G4Version.hh"
#if G4VERSION_NUMBER > 1009 // consistent with BDSBOptChangeCrossSection

#include "BDSBOptrChangeCrossSection.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "G4BiasingProcessInterface.hh"

#include "globals.hh"
#include "G4GenericIon.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"


BDSBOptrMultiParticleChangeCrossSection::BDSBOptrMultiParticleChangeCrossSection():
  G4VBiasingOperator("BDSIM General Biasing")
{
#ifdef BDSDEBUG
  debug = true;
#else
  debug = false;
#endif
}

BDSBOptrMultiParticleChangeCrossSection::~BDSBOptrMultiParticleChangeCrossSection() 
{}

void BDSBOptrMultiParticleChangeCrossSection::AddParticle(G4String particleName)
{
  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  
  if (!particle) 
    {throw BDSException(__METHOD_NAME__, "Particle \"" + particleName + "\" not found");}
  
  BDSBOptrChangeCrossSection* optr = new BDSBOptrChangeCrossSection(particleName,particleName);
  fParticlesToBias.push_back(particle);
  fBOptrForParticle[particle] = optr;
  optr->StartRun();
}

void BDSBOptrMultiParticleChangeCrossSection::SetBias(G4String particleName,
						      G4String process,
						      G4double dBias,
						      G4int iPrimary) 
{
  // important feedback for the user
  G4cout << "Biasing process \"" << process << "\" for particle \"" << particleName << "\" by factor " << dBias;
  G4String flagString = "all";
  if (iPrimary == 2)
    {flagString = "primary";}
  else if (iPrimary == 3)
    {flagString = "primary & secondary";}
  G4cout << ", for " << flagString << " particles" << G4endl;
  
  const G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle(particleName);
  if (!particle)
    {throw BDSException(__METHOD_NAME__, "Particle \"" + particleName + "\" no found");}
  
  fBOptrForParticle[particle]->SetBias(process,dBias,iPrimary);
}

G4VBiasingOperation* BDSBOptrMultiParticleChangeCrossSection::ProposeOccurenceBiasingOperation(const G4Track* track,
											       const G4BiasingProcessInterface* callingProcess)
{
  // -- examples of limitations imposed to apply the biasing:
  // -- limit application of biasing to primary particles only:
  //  if ( track->GetParentID() != 0 ) return 0;
  // -- limit to at most 5 biased interactions:
  //  if ( fnInteractions > 4 )        return 0;
  // -- and limit to a weight of at least 0.05:
  //  if ( track->GetWeight() < 0.05 ) return 0;
  
  if (fCurrentOperator)
    {return fCurrentOperator->GetProposedOccurenceBiasingOperation(track, callingProcess);}
  else
    {return nullptr;}
}

void BDSBOptrMultiParticleChangeCrossSection::StartTracking(const G4Track* track)
{
  // -- fetch the underneath biasing operator, if any, for the current particle type:
  const G4ParticleDefinition* definition = track->GetParticleDefinition();
  std::map <const G4ParticleDefinition*,BDSBOptrChangeCrossSection*>::iterator it = fBOptrForParticle.find(definition);
  fCurrentOperator = 0;
  if (it != fBOptrForParticle.end())
    {fCurrentOperator = (*it).second;}

  // try again for ions as they have a generic and specific definition
  // processes are attached to the generic one
  if (G4IonTable::IsIon(definition))
    {
      auto search = fBOptrForParticle.find(G4GenericIon::Definition());
      if (search != fBOptrForParticle.end())
	{fCurrentOperator = search->second;}
    }
  // -- reset count for number of biased interactions:
  fnInteractions = 0;
}

void  BDSBOptrMultiParticleChangeCrossSection::
OperationApplied(const G4BiasingProcessInterface*               callingProcess, 
		 G4BiasingAppliedCase                              biasingCase,
		 G4VBiasingOperation*                occurenceOperationApplied, 
		 G4double                        weightForOccurenceInteraction,
		 G4VBiasingOperation*               finalStateOperationApplied, 
		 const G4VParticleChange*               particleChangeProduced)
{
  // -- count number of biased interactions:
  fnInteractions++;
  
  // -- inform the underneath biasing operator that a biased interaction occured:
  if (fCurrentOperator)
    {
      fCurrentOperator->ReportOperationApplied(callingProcess,
					       biasingCase,
					       occurenceOperationApplied,
					       weightForOccurenceInteraction,
					       finalStateOperationApplied,
					       particleChangeProduced);
    }
}

#endif
