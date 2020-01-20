/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSOUTPUT_H
#define BDSOUTPUT_H 

#include "BDSOutputStructures.hh"

#include "globals.hh"

#include <ctime>
#include <ostream>
#include <set>
#include <vector>
#include <map>

// forward declarations
template <class T> class G4THitsCollection;
class BDSHitApertureImpact;
typedef G4THitsCollection<BDSHitApertureImpact> BDSHitsCollectionApertureImpacts;
class BDSHitCollimator;
typedef G4THitsCollection<BDSHitCollimator> BDSHitsCollectionCollimator;
class BDSHitEnergyDeposition;
typedef G4THitsCollection<BDSHitEnergyDeposition> BDSHitsCollectionEnergyDeposition;
class BDSEventInfo;
class BDSParticleCoordsFullGlobal;
class BDSParticleDefinition;
class BDSHitSampler;
typedef G4THitsCollection<BDSHitSampler> BDSHitsCollectionSampler;
class BDSTrajectory;
class BDSTrajectoryPoint;
class BDSHitEnergyDepositionGlobal;
typedef G4THitsCollection<BDSHitEnergyDepositionGlobal> BDSHitsCollectionEnergyDepositionGlobal;
class BDSTrajectoriesToStore;
class G4PrimaryVertex;

namespace GMAD
{
  class BeamBase;
  class OptionsBase;
}

/**
 * @brief Output base class that defines interface for all output types.
 */

class BDSOutput: protected BDSOutputStructures
{
public:
  /// Constructor with base file name (without extension or number suffix).
  BDSOutput(G4String baseFileNameIn,
	    G4String fileExtentionIn,
	    G4int    fileNumberOffset);
  virtual ~BDSOutput(){;}

  /// Open a new file. This should call WriteHeader() in it.
  virtual void NewFile() = 0;

  /// Write any unwritten contents and close the currently open file. The instance
  /// should be safe to delete after calling this method.
  virtual void CloseFile() = 0;

  /// Setup any geometry dependent output structures in the output file - such
  /// as samplers. This is run after the geometry has been constructed and 'closed'.
  /// This also sets up histograms based along S now the beam line is known.
  virtual void InitialiseGeometryDependent();
  
  /// Fill the local structure header with information - updates time stamp.
  void FillHeader();

  /// Fill the local structure geant4 data with information. Also calls WriteGeant4Data().
  void FillGeant4Data(G4bool writeIons);

  /// Fill the local structure beam with the original ones from the parser.
  /// This also calls WriteBeam().
  void FillBeam(const GMAD::BeamBase* beam);
  
  /// Fill the local structure options with the original ones from the parser.
  /// This also calls WriteOptions().
  void FillOptions(const GMAD::OptionsBase* options);

  /// Fill the local structure model from the beam line and sampler registry.
  /// This also calls WriteModel(). After writing the model it clears the structure.
  void FillModel();

  /// Fill the local structure with primary vertex information. A unique interface
  /// for the case of generating primaries only. This fills the primary structure,
  /// and calls WriteFileEventLevel() and then clears the structures. It therefore
  /// should not be used in conjunction with FillEvent().
  void FillEventPrimaryOnly(const BDSParticleCoordsFullGlobal& coords,
			    const BDSParticleDefinition*       particle);
  
  /// Copy event information from Geant4 simulation structures to output structures.
  void FillEvent(const BDSEventInfo*                            info,
		 const G4PrimaryVertex*                         vertex,
		 const BDSHitsCollectionSampler*                samplerHitsPlane,
		 const BDSHitsCollectionSampler*                samplerHitsCylinder,
		 const BDSHitsCollectionEnergyDeposition*       energyLoss,
		 const BDSHitsCollectionEnergyDeposition*       energyLossFull,
		 const BDSHitsCollectionEnergyDeposition*       energyLossVacuum,
		 const BDSHitsCollectionEnergyDeposition*       energyLossTunnel,
		 const BDSHitsCollectionEnergyDepositionGlobal* energyLossWorld,
		 const BDSHitsCollectionEnergyDepositionGlobal* energyLossWorldContents,
		 const BDSHitsCollectionEnergyDepositionGlobal* worldExitHits,
		 const BDSTrajectoryPoint*                      primaryHit,
		 const BDSTrajectoryPoint*                      primaryLoss,
		 const BDSTrajectoriesToStore*                  trajectories,
		 const BDSHitsCollectionCollimator*             collimatorHits,
		 const BDSHitsCollectionApertureImpacts*        apertureImpactHits,
		 const G4int                                    turnsTaken);

  /// Close a file and open a new one.
  void CloseAndOpenNewFile();

  /// Copy run information to output structure.
  void FillRun(const BDSEventInfo* info);
  
  /// Test whether a sampler name is invalid or not.
  static G4bool InvalidSamplerName(const G4String& samplerName);

  /// Feedback for protected names.
  static void PrintProtectedNames(std::ostream& out);

protected:
  /// Get the next file name based on the base file name and the accrued number of files.
  G4String GetNextFileName();

  /// Whether primaries are to be written to file or not.
  inline G4bool WritePrimaries() const {return writePrimaries;}

  /// Whether to create the collimator structures in the output or not.
  inline G4bool CreateCollimatorOutputStructures() const {return createCollimatorOutputStructures;}

  /// @{ Options for dynamic bits of output.
  G4bool storeELossWorldContents;
  G4bool storeApertureImpacts;
  /// @}

private:
  /// Enum for different types of sampler hits that can be written out.
  enum class HitsType {plane, cylinder};

  /// Enum for different types of energy loss that can be written out.
  enum class LossType {energy, vacuum, tunnel, world, worldexit, worldcontents};

  /// Write the header.
  virtual void WriteHeader() = 0;

  /// Write the geant4 information.
  virtual void WriteGeant4Data() = 0;

  /// Write the beam.
  virtual void WriteBeam() = 0;
  
  /// Write the options.
  virtual void WriteOptions() = 0;

  /// Write a representation of the samplers and beamline.
  virtual void WriteModel() = 0;

  /// Copy from local event structures to the actual file. Only event level
  /// structures are copied.
  virtual void WriteFileEventLevel() = 0;

  /// Copy from local run structures to the actual file. Only run level
  /// structures are copied.
  virtual void WriteFileRunLevel() = 0;

  /// Calculate the number of bins and required maximum s.
  void CalculateHistogramParameters();
  
  /// Create histograms.
  void CreateHistograms();
  
  /// Fill the local structure with primary vertex information. Utility function
  /// to translate from G4PrimaryVertex to simple numbers.
  void FillPrimary(const G4PrimaryVertex* vertex,
		   const G4int            turnsTaken);
  
  /// Fill event summary information.
  void FillEventInfo(const BDSEventInfo* info);

  /// Fill sampler hits into output structures.
  void FillSamplerHits(const BDSHitsCollectionSampler* hits,
		       const HitsType hType);

  /// Fill the hit where the primary particle impact.
  void FillPrimaryHit(const BDSTrajectoryPoint* phits);

  /// Fill a collection of energy hits into the appropriate output structure.
  void FillEnergyLoss(const BDSHitsCollectionEnergyDeposition* loss,
		      const LossType type);

  void FillEnergyLoss(const BDSHitsCollectionEnergyDepositionGlobal* loss,
		      const LossType type);

  /// Fill a collection volume exit hits into the approprate output structure.
  //void FillELossWorldExitHits(const BDSHitsCollectionVolumeExit* worldExitHits);
  
  /// Fill the hit where the primary stopped being a primary.
  void FillPrimaryLoss(const BDSTrajectoryPoint* ploss);

  /// Copy a set of trajectories to the output structure.
  void FillTrajectories(const BDSTrajectoriesToStore* trajectories);

  /// Fill collimator hits.
  void FillCollimatorHits(const BDSHitsCollectionCollimator* hits,
			  const BDSTrajectoryPoint* primaryLossPoint);

  /// Fill aperture impact hits.
  void FillApertureImpacts(const BDSHitsCollectionApertureImpacts* hits);

  /// Fill run level summary information.
  void FillRunInfo(const BDSEventInfo* info);

  /// Utility function to copy out select bins from one histogram to another for 1D
  /// histograms only.
  void CopyFromHistToHist1D(const G4String sourceName,
			    const G4String destinationName,
			    const std::vector<G4int> indices);

  /// No default constructor.
  BDSOutput() = delete;
  
  const G4String baseFileName;  ///< Base file name.
  const G4String fileExtension; ///< File extension to add to each file.
  G4int numberEventPerFile; ///< Number of events stored per file.
  G4int outputFileNumber;   ///< Number of output file.
  G4bool writePrimaries;    ///< Whether to write primaries or not.

  /// Invalid names for samplers - kept here as this is where the output structures are created.
  const static std::set<G4String> protectedNames;

  /// Whether the single 3D histogram will be built.
  G4bool useScoringMap;

  /// The maximum s in mm such that there is an integer number of
  /// elossHistoBinWidths along the line. Used for histogramming purposes.
  G4double sMaxHistograms;

  /// Number of bins for each histogram required.
  G4int nbins;

  /// @{ Storage option.
  G4bool storeCollimatorInfo;
  G4bool storeCollimatorHits;
  G4bool storeCollimatorHitsLinks;
  G4bool storeCollimatorHitsIons;
  G4bool storeELoss;
  G4bool storeELossHistograms;
  G4bool storeELossTunnel;
  G4bool storeELossTunnelHistograms;
  G4bool storeELossVacuum;
  G4bool storeELossVacuumHistograms;
  G4bool storeELossWorld; // for both world and world exit
  G4bool storeGeant4Data;
  G4bool storeModel;
  G4bool storeSamplerPolarCoords;
  G4bool storeSamplerCharge;
  G4bool storeSamplerKineticEnergy;
  G4bool storeSamplerMass;
  G4bool storeSamplerRigidity;
  G4bool storeSamplerIon;
  /// @}

  /// Whether to create collimator output structures or not - based on
  /// several collimator storage options.
  G4bool createCollimatorOutputStructures;

  /// @{ Integral when filling hit.
  G4double energyDeposited;
  G4double energyDepositedVacuum;
  G4double energyDepositedWorld;
  G4double energyDepositedWorldContents;
  G4double energyDepositedTunnel;
  G4double energyWorldExit;
  G4int    nCollimatorsInteracted;
  /// @}

  /// @{ Map of histogram name (short) to index of histogram in output.
  std::map<G4String, G4int> histIndices1D;
  std::map<G4String, G4int> histIndices3D;
  /// @}
};

#endif
