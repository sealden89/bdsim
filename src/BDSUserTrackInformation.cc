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
#include "BDSElectronOccupancy.hh"
#include "BDSUserTrackInformation.hh"

#include "globals.hh"
#include "G4DynamicParticle.hh"

BDSUserTrackInformation::BDSUserTrackInformation(const G4DynamicParticle* particle):
  G4VUserTrackInformation("BDSUserTrackInformation"),
  electronOccupancy(nullptr)
{
    if(particle->GetTotalOccupancy()>0)
    {
        totalElectrons = particle->GetTotalOccupancy();
        if(totalElectrons <= 2)
        {
            electronOccupancy = new BDSElectronOccupancy(1);
            electronOccupancy->SetTotalElectrons(totalElectrons);
            electronOccupancy->PopulateLevels();
        }
        else if (totalElectrons <= 10)
        {
            electronOccupancy = new BDSElectronOccupancy(2);
            electronOccupancy->SetTotalElectrons(totalElectrons);
            electronOccupancy->PopulateLevels();
        }
        else if (totalElectrons <= 30)
        {
            electronOccupancy = new BDSElectronOccupancy(3);
            electronOccupancy->SetTotalElectrons(totalElectrons);
            electronOccupancy->PopulateLevels();
        }
        else if (totalElectrons <= 64)
        {
            electronOccupancy = new BDSElectronOccupancy(4);
            electronOccupancy->SetTotalElectrons(totalElectrons);
            electronOccupancy->PopulateLevels();
        }
        else
        {
            electronOccupancy = new BDSElectronOccupancy(7);
            electronOccupancy->SetTotalElectrons(totalElectrons);
            electronOccupancy->PopulateLevels();
        }
    }
}

BDSUserTrackInformation::~BDSUserTrackInformation()
{}


BDSElectronOccupancy* BDSUserTrackInformation::GetElectronOccupancy()
{
    return electronOccupancy;
}
