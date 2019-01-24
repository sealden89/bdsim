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

#include "BDSPhysicsLaserPhotoDetachment.hh"
#include "G4LeptonConstructor.hh"
#include "G4ParticleDefinition.hh"
#include "BDSLaserPhotoDetachment.hh"


BDSPhysicsLaserPhotoDetachment::BDSPhysicsLaserPhotoDetachment()
{;}

BDSPhysicsLaserPhotoDetachment::~BDSPhysicsLaserPhotoDetachment()
{;}

void BDSPhysicsLaserPhotoDetachment::ConstructParticle()
{
    G4LeptonConstructor leptons;
    leptons.ConstructELeptons();
}

void BDSPhysicsLaserPhotoDetachment::ConstructProcess()
{
    if(Activated())
    {return;}

    BDSLaserPhotoDetachment* photoDetach = new BDSLaserPhotoDetachment();

    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
#if G4VERSION_NUMBER > 1029
    auto aParticleIterator = GetParticleIteractor();
#endif
    aParticleIteractor->reset();
    while(  (*aParticleIterator)()  )
        G4ParticleDefinition* particle = aParticleIteractor->value();
//if condition for probability of stripping greater than randomly generated value
    //or else if in laser vac?
    if()
    {ph->RegisterProcess()}

}