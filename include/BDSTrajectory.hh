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
#ifndef BDSTRAJECTORY_H
#define BDSTRAJECTORY_H

#include "BDSTrajectoryPoint.hh"
#include "G4Trajectory.hh"

#include <ostream>
#include <vector>

class G4Step;
class G4Track;
class G4TrajectoryContainer;
class G4VTrajectoryPoint;

typedef std::vector<BDSTrajectoryPoint*>  BDSTrajectoryPointsContainer;

/**
 * @brief Trajectory information from track including last scatter etc.
 * 
 * BDSTrajectory stores BDSTrajectoryPoints
 *
 * @author S. Boogert
 */

class BDSTrajectory: public G4Trajectory
{
public:
  BDSTrajectory(const G4Track* aTrack,
		const G4bool& interactiveIn,
		const G4bool& suppressTransportationSteps);
  /// copy constructor is not needed
  BDSTrajectory(BDSTrajectory &) = delete;

  virtual ~BDSTrajectory();

  inline void* operator new(size_t);
  inline void operator delete(void*);
  inline int operator == (const BDSTrajectory& right) const {return (this==&right);}

  /// Append a step point to this trajectory. This is required for the trajectory
  /// points to show up in the visualisation correctly.
  virtual void AppendStep(const G4Step* aStep);

  /// Append a step point. Use a pre-made BDSTrajectoryPoint to save creating
  /// it again, which involves coordinate transforms.
  void AppendStep(const BDSTrajectoryPoint* pointIn);

  /// Merge another trajectory into this one.
  virtual void MergeTrajectory(G4VTrajectory* secondTrajectory);

  /// Access a point - use this class's container.
  virtual G4VTrajectoryPoint* GetPoint(G4int i) const {return (*fpBDSPointsContainer)[i];}

  /// Get number of trajectory points in this trajectory.
  virtual int GetPointEntries() const {return fpBDSPointsContainer->size();}

  /// Method to identify which one is a primary. Overridden in derived class.
  virtual G4bool IsPrimary() const {return false;}

  void  SetTrajIndex(G4int trajIndex) {fTrajIndex = trajIndex;}
  G4int GetTrajIndex() {return fTrajIndex;}

  void  SetParentIndex(G4int parentIndex)  {fParentIndex = parentIndex;}
  G4int GetParentIndex() const {return fParentIndex;}

  void  SetParentStepIndex(G4int parentStepIndex)  {fParentStepIndex = parentStepIndex;}
  G4int GetParentStepIndex() const {return fParentStepIndex;}

  void  SetParent(BDSTrajectory* parent)   {fParent = parent;}
  BDSTrajectory* GetParent()      const {return fParent;}

  G4int GetCreatorProcessType()    const {return creatorProcessType;}
  G4int GetCreatorProcessSubType() const {return creatorProcessSubType;}

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectory const &t);

  /// Find the first point in a trajectory where the post step process isn't fTransportation
  /// AND the post step process isn't fGeneral in combination with the post step process subtype
  /// isn't step_limiter.
  BDSTrajectoryPoint* FirstInteraction() const;
  BDSTrajectoryPoint* LastInteraction()  const;

protected:
  G4int          creatorProcessType;
  G4int          creatorProcessSubType;
  G4double       weight;
  G4bool         interactive;
  const G4bool   suppressTransportationSteps;
  BDSTrajectory* fParent;
  G4int          fTrajIndex;
  G4int          fParentIndex;
  G4int          fParentStepIndex;

  /// Container of all points. This is really a vector so all memory is dynamically
  /// allocated and there's no need to make this dynamically allocated itself a la
  /// all Geant4 examples.
  BDSTrajectoryPointsContainer* fpBDSPointsContainer;

private:
  BDSTrajectory();
};

extern G4Allocator<BDSTrajectory> bdsTrajectoryAllocator;

inline void* BDSTrajectory::operator new(size_t)
{
  void* aTrajectory;
  aTrajectory = (void*)bdsTrajectoryAllocator.MallocSingle();
  return aTrajectory;
}

inline void BDSTrajectory::operator delete(void* aTrajectory)
{bdsTrajectoryAllocator.FreeSingle((BDSTrajectory*)aTrajectory);}


#endif
