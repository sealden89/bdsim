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
#include "BDSHitThinThing.hh"

#include "G4Allocator.hh"

G4Allocator<BDSHitThinThing> BDSAllocatorThinThing;

BDSHitThinThing::BDSHitThinThing(G4int trackIDIn,
				 G4int parentIDIn,
				 G4int turnsTakenIn,
				 BDSTrajectoryPoint* hitIn):
  trackID(trackIDIn),
  parentID(parentIDIn),
  turnsTaken(turnsTakenIn),
  hit(hitIn)
{;}

BDSHitThinThing::~BDSHitThinThing()
{;}

std::vector<const BDSTrajectoryPoint*> BDSHitThinThing::TrajectoryPointsFromHC(BDSHitsCollectionThinThing* hits)
{
  std::vector<const BDSTrajectoryPoint*> result;
  if (!hits)
    {return result;}
  else
    {
      for (G4int i = 0; i < hits->entries(); i++)
	{result.push_back((*hits)[i]->hit);}
      return result;
    }
}
