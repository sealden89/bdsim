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
#ifndef BDSBUNCH_H
#define BDSBUNCH_H

#include "BDSBunchType.hh"
#include "BDSParticleCoords.hh"
#include "BDSParticleCoordsFull.hh"
#include "BDSParticleCoordsFullGlobal.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

class BDSBeamline;
class BDSParticleDefinition;

namespace GMAD
{
  class Beam;
}

/**
 * @brief The base class for bunch distribution generators.
 *
 * @author Stewart Boogert
 */

class BDSBunch
{
public:
  BDSBunch();
  virtual ~BDSBunch(){;}

  /// Make BDSHepMC3Reader a friend so it can use the protected ApplyTransform function.
  friend class BDSHepMC3Reader;

  /// Extract and set the relevant options from the beam definition. The distribution
  /// type is explicitly required as this function may be used inside a nested bunch distribution.
  /// This argument is for the most part ignored, but there's no way to have a default for it.
  /// Also, some classes can cover multiple input distributions so need to know which one they're
  /// meant to be.
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  
  /// Check the parameters for the given bunch distribution and exit if they're
  /// problematic or unphysical.
  virtual void CheckParameters();

  /// Main interface. Calls GetNextParticleLocal() and then applies the curvilinear
  /// transform if required.
  BDSParticleCoordsFullGlobal GetNextParticle();

  /// An action that is called at the beginning of a run when we know the number of
  /// events that'll be generated. By default this is nothing, but can be used to
  /// calculate sample mean offsets in some derived classes.
  virtual void BeginOfRunAction(G4int numberOfEvents);

  /// An action that is called at the end of a run. By default, does nothing.
  virtual void EndOfRunAction();

  /// Access whether the particle generated may be different from the design particle.
  inline G4bool ParticleCanBeDifferentFromBeam() const {return particleCanBeDifferent;}

  /// Access the beam particle definition.
  inline const BDSParticleDefinition* ParticleDefinition() const {return particleDefinition;}

  virtual void SetGeneratePrimariesOnly(G4bool generatePrimariesOnlyIn);

  /// Each derived class can override this default method of reference
  /// position. If S0 > 0 or derived class changes member bool 'curvilinear'
  /// z0 will be treated as S and the global z0 be calculated.
  virtual BDSParticleCoordsFull GetNextParticleLocal();

  G4double GetCentralMomentum() {return P0;}
  G4bool GetUseCurvilinear() {return useCurvilinear;}

  /// When recreating events, it's possible that setting the seed state may not
  /// be sufficient for the bunch to get the right distribution. This is true when
  /// the bunch coordinates are based on an external source of data i.e. user bunch
  /// file. This default method allows such a distribution to advance to the correct
  /// event number.
  virtual void RecreateAdvanceToEvent(G4int /*eventOffset*/){;}

protected:
  /// Apply either the curivilinear transform if we're using curvilinear coordinates or
  /// just apply the general beam line offset in global coordinates to the 'local'
  /// curvilinear coordinates.
  BDSParticleCoordsFullGlobal ApplyTransform(const BDSParticleCoordsFull& localIn) const;

  /// Calculate the global coordinates from curvilinear coordinates of a beam line.
  BDSParticleCoordsFullGlobal ApplyCurvilinearTransform(const BDSParticleCoordsFull& localIn) const;

  /// Calculate zp safely based on other components.
  G4double CalculateZp(G4double xp, G4double yp, G4double Zp0) const;

  /// Convert a momentum to a total energy given the beam particle mass.
  G4double EFromP(const G4double &pIn) const;

  ///@{ Centre of distributions
  G4double X0;
  G4double Y0;
  G4double Z0;
  G4double S0;
  G4double T0; 
  G4double Xp0; 
  G4double Yp0;
  G4double Zp0;
  G4double E0;
  G4double P0;     ///< central momentum
  G4double sigmaT;
  G4double sigmaP;
  G4double sigmaE;
  ///@}
  
  /// Whether to ignore z and use s and transform for curvilinear coordinates
  G4bool useCurvilinear;

  /// Wether the bunch distribution can specify a particle that's different
  /// from the one used for the reference particle that created the beam line.
  /// Derived class must change explicitly.
  G4bool particleCanBeDifferent;

  /// Optional particle definition that can be used. Does not own.
  const BDSParticleDefinition* particleDefinition;

  /// @{ Flags to ignore random number generator in case of no finite E or T.
  G4bool finiteSigmaE;
  G4bool finiteSigmaT;
  /// @}

  /// Whether we're only generating the primaries only and therefore there's no beamline
  /// to get a transform from and we shouldn't try.
  G4bool generatePrimariesOnly;
  
private:
  /// Transform that beam line starts with that will also be applied to coordinates.
  G4Transform3D beamlineTransform;

  /// Beamline initial S position
  G4double beamlineS;
  
  G4double mass2; ///< Cache of mass squared as required to convert from p to E.
  
  /// A reference to the fully constructed beamline that's lazily instantiated.
  mutable const BDSBeamline* beamline;
};

#endif
