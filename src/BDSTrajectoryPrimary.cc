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
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSTrajectoryPrimary.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Allocator.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrajectoryContainer.hh"

#include <ostream>
#include <set>

G4Allocator<BDSTrajectoryPrimary> bdsTrajectoryPrimaryAllocator;
G4bool BDSTrajectoryPrimary::hasScatteredThisTurn = false;

BDSTrajectoryPrimary* BDS::GetPrimaryTrajectory(G4TrajectoryContainer* trajCont)
{
  TrajectoryVector*     trajVec = trajCont->GetVector();
  BDSTrajectoryPrimary* primary = nullptr;
  for (const auto trajIt : *trajVec)
    {
      BDSTrajectory* traj = static_cast<BDSTrajectory*>(trajIt);
      if (traj->IsPrimary())
	{primary = static_cast<BDSTrajectoryPrimary*>(trajIt); break;}
    }
  return primary;
}

BDSTrajectoryPrimary::BDSTrajectoryPrimary(const G4Track* aTrack,
					   const G4bool& interactiveIn,
					   const G4bool& suppressTransportationStepsIn,
					   const G4bool& storeTrajectoryPointsIn):
  BDSTrajectory(aTrack, interactiveIn, suppressTransportationStepsIn),
  firstHit(nullptr),
  lastPoint(nullptr),
  storeTrajectoryPoints(storeTrajectoryPointsIn)
{;}

BDSTrajectoryPrimary::~BDSTrajectoryPrimary()
{
  delete firstHit;
  delete lastPoint;
}

void BDSTrajectoryPrimary::AppendStep(const G4Step* aStep)
{
  auto point = new BDSTrajectoryPoint(aStep);

  // check if scattering point and cache it if so
  if (!firstHit)
    {
      if (point->IsScatteringPoint())
	{
	  firstHit = point;
	  hasScatteredThisTurn = true;
	}
      else
	{delete point;} // don't store it
    }

  else if (!hasScatteredThisTurn)
    {
      if (point->IsScatteringPoint())
	{hasScatteredThisTurn = true;}
      delete point;
    }

  // update last point
  delete lastPoint; // clear old last point
  lastPoint = new BDSTrajectoryPoint(aStep); // construct separate copy as easier to manage.

  if (storeTrajectoryPoints)
    {BDSTrajectory::AppendStep(aStep);}
}

std::ostream& operator<< (std::ostream& out, BDSTrajectoryPrimary const& t)
{
  if (t.firstHit)
    {out << "First hit: " << t.firstHit << G4endl;}
  else
    {out << "No first hit" << G4endl;}
  if (t.lastPoint)
    {out << "Last point: " << t.lastPoint << G4endl;}
  else
    {out << "No last point" << G4endl;}

  const BDSTrajectory& b(t);
  out << b;
  return out;
}
