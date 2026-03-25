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
#include "BDSPhysicsVectorLinear.hh"
#include "BDSWrapperScintillationBiasing.hh"

#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4Track.hh"
#include "G4Types.hh"
#include "G4VParticleChange.hh"
#include "G4VProcess.hh"
#include <cmath>
#include <G4OpticalPhoton.hh>
#include <limits>
#include <vector>


BDSWrapperScintillationBiasing::BDSWrapperScintillationBiasing(G4VProcess* originalProcess,
                                                 G4int reductionFactorIn):
  BDSWrapperProcess("ScintillationReductionWrapper"),
  reductionFactor(reductionFactorIn)
{
    RegisterProcess(originalProcess);
    theProcessSubType = originalProcess->GetProcessSubType();
    theProcessName = "ScintillationReductionWrapper("+originalProcess->GetProcessName()+")";

}

G4VParticleChange* BDSWrapperScintillationBiasing::PostStepDoIt(
    const G4Track& track,
    const G4Step& step)
{
    G4VParticleChange* particleChange = pRegProcess->PostStepDoIt(track, step);

    G4int numSecondaries = particleChange->GetNumberOfSecondaries();

    if (numSecondaries == 0)
    {return particleChange;}

    std::vector<G4Track*> secondariesToKeep;

    G4int nToKeep = numSecondaries/reductionFactor;
    // this works for high numbers of photons and where reduction factor is sensibly chosen
    // sensible to add a minimum number of photons as this could supress all optical photons.
    for (int i = 0; i < numSecondaries; i++)
      {
        G4Track* ithSecondary = particleChange->GetSecondary(i);
        if (ithSecondary->GetParticleDefinition() == G4OpticalPhoton::Definition())
          {
            // if particle is optical photon random reduce
            if (G4UniformRand()<= 1.0/reductionFactor)
              {
                // should keep 1/reduction factor number of photons
                // weight of kept ones is current weight times reduction factor and needs updating
                G4double weightUpdated = ithSecondary->GetWeight()*reductionFactor;
                ithSecondary->SetWeight(weightUpdated);
                secondariesToKeep.push_back(ithSecondary);
              }
            else
              {
                // otherwise delete that optical photon
                delete ithSecondary;
              }
          }
      else
        {
          // if not an optical photon don't mess with it, want to keep secondaries from other sources
          secondariesToKeep.push_back(ithSecondary);
        }
      }

  particleChange->Clear(); // doesn't delete the secondaries

  particleChange->SetNumberOfSecondaries(secondariesToKeep.size());

  G4bool originalSetSecondaryWeightByProcess = particleChange->IsSecondaryWeightSetByProcess();
  particleChange->SetSecondaryWeightByProcess(true);
  for (auto aSecondary : secondariesToKeep)
  {particleChange->AddSecondary(aSecondary);}
  particleChange->SetSecondaryWeightByProcess(originalSetSecondaryWeightByProcess);

  return particleChange;

}

BDSWrapperScintillationBiasing::~BDSWrapperScintillationBiasing() {}

