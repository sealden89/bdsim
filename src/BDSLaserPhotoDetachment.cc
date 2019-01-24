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


/*
 *
 *
 * author Siobhan Alden
 */

#include "BDSLaserPhotoDetachment.hh"

BDSLaserPhotoDetachment::BDSLaserPhotoDetachment()
{
}

G4double BDSLaserPhotoDetachment::GetMeanFreePath(G4Track& track,
                                                  G4double previousStepSize,
                                            G4ForceCondition* forceCondition)
{
    // need to compare track to see if in "laservac" then evaluate photon density
    // at that position to evaluate the next
    //if(track.)


}

G4VParticleChange BDSLaserPhotoDetachment::PostStepDoIt()
{

    //G4Material* aMaterial = trackData.GetMaterial();

 //   if(aMaterial == BDSMaterials::Instance()->GetMaterial("laserVac"))
  //  {



    //}

}