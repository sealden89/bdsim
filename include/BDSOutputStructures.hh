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
#ifndef BDSOUTPUTSTRUCTURES_H
#define BDSOUTPUTSTRUCTURES_H

#include "globals.hh"

#include <string>
#include <vector>

// forward declarations
class BDSGlobalConstants;
class BDSOutputROOTEventBeam;
class BDSOutputROOTEventCoords;
class BDSOutputROOTEventExit;
class BDSOutputROOTEventHeader;
class BDSOutputROOTEventHistograms;
class BDSOutputROOTEventInfo;
class BDSOutputROOTEventLoss;
class BDSOutputROOTEventModel;
class BDSOutputROOTEventOptions;
class BDSOutputROOTEventRunInfo;
template<class T> class BDSOutputROOTEventSampler;
class BDSOutputROOTEventTrajectory;
class BDSOutputROOTGeant4Data;

/**
 * @brief Holder for output information.
 * 
 * This holds instances of the output classes that can be filled
 * and directly copied to the output file.  It also holds and fills
 * the histograms.
 *
 * @author S. Boogert, L. Nevay
 */

class BDSOutputStructures
{
protected:
  explicit BDSOutputStructures(const BDSGlobalConstants* globals);
  virtual ~BDSOutputStructures();

  /// Construct samplers.
  void InitialiseSamplers();

  /// Clear the local geant4 data structure.
  void ClearStructuresGeant4Data();

  /// Clear the local header structure.
  void ClearStructuresHeader();

  /// Clear the local model structure.
  void ClearStructuresModel();

  /// Clear the local beam structure.
  void ClearStructuresBeam();

  /// Clear the local options structure.
  void ClearStructuresOptions();
  
  /// Clear the local structures in this class in preparation for a new event.
  void ClearStructuresEventLevel();

  /// Clear the local structures in this class in preparation for a new run.
  void ClearStructuresRunLevel();
  
  ///@{ Create histograms for evtHistos and runHistos.
  void Create1DHistogram(G4String name,
                         G4String title,
                         G4int    nbins,
                         G4double xmin,
                         G4double xmax);
  void Create1DHistogram(G4String name,
                         G4String title,
                         std::vector<double>& edges);
  ///@}

  BDSOutputROOTGeant4Data*   geant4DataOutput; ///< Geant4 information / particle tables.
  BDSOutputROOTEventHeader*  headerOutput;     ///< Information about the file.
  BDSOutputROOTEventBeam*    beamOutput;       ///< Beam output.
  BDSOutputROOTEventOptions* optionsOutput;    ///< Options output.
  BDSOutputROOTEventModel*   modelOutput;      ///< Model output.

  /// Primary sampler structure.
#ifdef __ROOTDOUBLE__
  BDSOutputROOTEventSampler<double>* primary;
#else 
  BDSOutputROOTEventSampler<float>* primary;
#endif
  BDSOutputROOTEventCoords* primaryGlobal; /// Global coordinates of primary particle.
  /// Sampler structures.
#ifdef __ROOTDOUBLE__
  std::vector<BDSOutputROOTEventSampler<double>*> samplerTrees;
#else
  std::vector<BDSOutputROOTEventSampler<float>*> samplerTrees;
#endif
  std::vector<std::string> samplerNames; ///< Sampler names to use.
  
  BDSOutputROOTEventRunInfo*    runInfo;    ///< Run information.
  BDSOutputROOTEventHistograms* runHistos;  ///< Run level histograms
  BDSOutputROOTEventLoss*       eLoss;      ///< General energy deposition.
  BDSOutputROOTEventLoss*       pFirstHit;  ///< Primary hit point.
  BDSOutputROOTEventLoss*       pLastHit;   ///< Primary loss point.
  BDSOutputROOTEventLoss*       tunnelHit;  ///< Tunnel energy deposition.
  BDSOutputROOTEventLoss*       eLossWorld; ///< World energy deposition.
  BDSOutputROOTEventExit*       eLossWorldExit;///< World exit hits.
  BDSOutputROOTEventTrajectory* traj;       ///< Trajectories.
  BDSOutputROOTEventHistograms* evtHistos;  ///< Event level histograms.
  BDSOutputROOTEventInfo*       evtInfo;    ///< Event information.
  
private:
  /// Whether we've set up the member vector of samplers. Can only be done once the geometry
  /// is constructed so it's done in initialisation. This happens with each new file, but we
  /// should only prepare the local samplers once, hence this cache variable.
  G4bool localSamplersInitialised;

  ///@{ Unused default constructors
  BDSOutputStructures() = delete;
  BDSOutputStructures(const BDSOutputStructures&) = delete;
  BDSOutputStructures& operator=(const BDSOutputStructures&) = delete;
  ///@}
};

#endif
