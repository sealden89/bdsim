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
#ifndef BDSTRAJECTORYPOINT_H
#define BDSTRAJECTORYPOINT_H

#include "globals.hh" // geant4 types / globals
#include "G4Allocator.hh"
#include "G4TrajectoryPoint.hh"

#include <ostream>

class G4Step;
class G4Track;

class BDSAuxiliaryNavigator;
class BDSBeamline;

/**
 * @brief A Point in a trajectory with extra information.
 *
 * @author S. Boogert
 */

class BDSTrajectoryPoint: public G4TrajectoryPoint
{
public:
  /// Default constructor.
  BDSTrajectoryPoint();
  
  /// This constructor is used to construct a point from a step intended to
  /// be appended to a trajectory. It uses the post step point as the main position.
  explicit BDSTrajectoryPoint(const G4Step* step);
  
  /// This constructor is required for the beginning of each track
  /// and produces the initial vertex point.
  explicit BDSTrajectoryPoint(const G4Track* track);

  virtual ~BDSTrajectoryPoint();

  inline void *operator new(size_t);
  inline void operator delete(void *aTrajectoryPoint);
  inline int operator==(const BDSTrajectoryPoint& right) const
  {return (this==&right);};

  /// Check to see if point is a scattering point (from a physics point of view). Uses
  /// static functions defined below. This is defined by whether the processes that defined
  /// the step length is non-transportation or the energy loss along step is greater than
  /// 1e-9 MeV, which could be due to continuous energy loss processes along step that might
  /// not limit the step length.
  G4bool IsScatteringPoint() const;

  /// Return true if step isn't defined by transportation processes.
  G4bool NotTransportationLimitedStep() const;
  
  /// @{ Static function to determine whether a step corresponds to scattering.
  static G4bool IsScatteringPoint(const G4Step* step);
  static G4bool IsScatteringPoint(G4int    postProcessType,
				  G4int    postProcessSubType,
				  G4double totalEnergyDeposit);
  /// @}
  
  /// @{ Accessor
  inline G4int    GetPreProcessType()          const {return preProcessType;}
  inline G4int    GetPreProcessSubType()       const {return preProcessSubType;}
  inline G4int    GetPostProcessType()         const {return postProcessType;}
  inline G4int    GetPostProcessSubType()      const {return postProcessSubType;}
  inline G4double GetPreWeight()               const {return preWeight;}
  inline G4double GetPostWeight()              const {return postWeight;}
  inline G4double GetPreEnergy()               const {return preEnergy;}
  inline G4double GetPostEnergy()              const {return postEnergy;}
  inline G4double GetEnergy()                  const {return energy;}
  inline G4ThreeVector GetPreMomentum()        const {return preMomentum;}
  inline G4ThreeVector GetPostMomentum()       const {return postMomentum;}
  inline G4double GetPreS()                    const {return preS;}
  inline G4double GetPostS()                   const {return postS;}
  inline G4double GetPreGlobalTime()           const {return preGlobalTime;}
  inline G4double GetPostGlobalTime()          const {return postGlobalTime;}
  inline G4int    GetBeamLineIndex()           const {return beamlineIndex;}
  inline BDSBeamline* GetBeamLine()            const {return beamline;}
  inline G4int    GetTurnsTaken()              const {return turnstaken;}
  inline G4ThreeVector GetPrePosLocal()        const {return prePosLocal;}
  inline G4ThreeVector GetPostPosLocal()       const {return postPosLocal;}
  /// @}

  /// @{ Return the transverse local radius in x,y.
  G4double PrePosR() const;
  G4double PostPosR() const;
  /// @}

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSTrajectoryPoint const &p);
  
private:
  /// Initialisation of variables in separate function to reduce duplication in
  /// multiple constructors.
  void InitialiseVariables();
  
  G4int preProcessType;           ///< Process type of pre-step point
  G4int preProcessSubType;        ///< Process sub type of pre-step point
  G4int postProcessType;          ///< Process type of post step point
  G4int postProcessSubType;       ///< Process sub type of post step point

  G4double preWeight;             ///< Weight associated with pre-step point
  G4double postWeight;            ///< Weight associtaed with post step point
  G4double preEnergy;             ///< Kinetic energy of pre-step point
  G4double postEnergy;            ///< Kinetic energy of post step point
  G4ThreeVector preMomentum;      ///< Momentum of pre-step point
  G4ThreeVector postMomentum;     ///< Momentum of post-step point
  G4double energy;                ///< Total energy deposited during step
  G4double preS;                  ///< Global curvilinear S coordinate of pre-step point
  G4double postS;                 ///< Global curvilinear S coordinate of post step point
  G4double preGlobalTime;         ///< Time since event started of pre-step point.
  G4double postGlobalTime;        ///< Time since event started of post-step point.
  G4int    beamlineIndex;         ///< Index to beam line element in the mass world beam line.
  BDSBeamline* beamline;          ///< Beam line (if any) point belongs to (always mass world).
  G4int    turnstaken;            ///< Number of turns taken
  G4ThreeVector prePosLocal;      ///< Local coordinates of pre-step point
  G4ThreeVector postPosLocal;     ///< Local coordinates of post-step point

  /// An auxilliary navigator to get curvilinear coordinates. Lots of points, but only
  /// need one navigator so make it static.
  static BDSAuxiliaryNavigator* auxNavigator;
};

extern G4Allocator<BDSTrajectoryPoint> bdsTrajectoryPointAllocator;

inline void* BDSTrajectoryPoint::operator new(size_t)
{
  void *aTrajectoryPoint;
  aTrajectoryPoint = (void *) bdsTrajectoryPointAllocator.MallocSingle();
  return aTrajectoryPoint;
}

inline void BDSTrajectoryPoint::operator delete(void *aTrajectoryPoint)
{
  bdsTrajectoryPointAllocator.FreeSingle((BDSTrajectoryPoint *) aTrajectoryPoint);
}

#endif
