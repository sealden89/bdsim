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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputStructures.hh"
#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventLossWorld.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTParticleData.hh"
#include "BDSHitSampler.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

BDSOutputStructures::BDSOutputStructures(const BDSGlobalConstants* globals):
  nCollimators(0),
  localSamplersInitialised(false),
  localCollimatorsInitialised(false)
{
  G4bool storeCollimatorInfo = globals->StoreCollimatorInfo();
  G4bool storeTurn       = globals->StoreELossTurn();
  G4bool storeLinks      = globals->StoreELossLinks();
  G4bool storeLocal      = globals->StoreELossLocal();
  G4bool storeGlobal     = globals->StoreELossGlobal();
  G4bool storeTime       = globals->StoreELossTime();
  G4bool storeStepLength = globals->StoreELossStepLength();
  G4bool storePreStepKineticEnergy = globals->StoreELossPreStepKineticEnergy();
  G4bool storeModelID    = globals->StoreELossModelID();
  G4bool storeELPhysics  = globals->StoreELossPhysicsProcesses();
  // store the model id if either modelID requested or store links
  storeModelID = storeModelID || storeLinks;

  particleDataOutput = new BDSOutputROOTParticleData();
  headerOutput  = new BDSOutputROOTEventHeader();
  beamOutput    = new BDSOutputROOTEventBeam();
  optionsOutput = new BDSOutputROOTEventOptions();
  modelOutput   = new BDSOutputROOTEventModel(storeCollimatorInfo);

  eLoss       = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy, storeELPhysics);
  eLossVacuum = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy, storeELPhysics);
  eLossTunnel = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy, storeELPhysics);
  eLossWorld         = new BDSOutputROOTEventLossWorld();
  eLossWorldExit     = new BDSOutputROOTEventLossWorld();
  eLossWorldContents = new BDSOutputROOTEventLossWorld();

  pFirstHit  = new BDSOutputROOTEventLoss(true, true,  true, true,  true, true,  false, true, true);
  pLastHit   = new BDSOutputROOTEventLoss(true, true,  true, true,  true, true,  false, true, true);

  apertureImpacts = new BDSOutputROOTEventAperture();
  
  traj       = new BDSOutputROOTEventTrajectory();
  evtHistos  = new BDSOutputROOTEventHistograms();
  evtInfo    = new BDSOutputROOTEventInfo();
  runHistos  = new BDSOutputROOTEventHistograms();
  runInfo    = new BDSOutputROOTEventRunInfo();

#ifndef __ROOTDOUBLE__
  primary = new BDSOutputROOTEventSampler<float>("Primary");
#else
  primary = new BDSOutputROOTEventSampler<double>("Primary");
#endif
  primaryGlobal = new BDSOutputROOTEventCoords();
}

BDSOutputStructures::~BDSOutputStructures()
{
  delete particleDataOutput;
  delete headerOutput;
  delete beamOutput;
  delete optionsOutput;
  delete modelOutput;
  delete primaryGlobal;
  delete eLoss;
  delete eLossVacuum;
  delete eLossTunnel;
  delete eLossWorld;
  delete eLossWorldExit;
  delete eLossWorldContents;
  delete pFirstHit;
  delete pLastHit;
  delete apertureImpacts;
  delete traj;
  delete evtHistos;
  delete evtInfo;
  delete runHistos;
  delete runInfo;
  for (auto sampler : samplerTrees)
    {delete sampler;}
  for (auto collimator : collimators)
    {delete collimator;}
  delete primary;
}

G4int BDSOutputStructures::Create1DHistogram(G4String name, G4String title,
					     G4int nbins, G4double xmin, G4double xmax)
{
  G4int result = evtHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
  // index from runHistos will be the same as used only through interfaces in this class
  runHistos->Create1DHistogram(name, title, nbins, xmin, xmax);
  return result;
}

G4int BDSOutputStructures::Create1DHistogram(G4String name, G4String title,
					     std::vector<double>& edges)
{
  G4int result = evtHistos->Create1DHistogram(name,title,edges);
  runHistos->Create1DHistogram(name,title,edges);
  return result;
}

G4int BDSOutputStructures::Create3DHistogram(G4String name, G4String title,
					     G4int nBinsX, G4double xMin, G4double xMax,
					     G4int nBinsY, G4double yMin, G4double yMax,
					     G4int nBinsZ, G4double zMin, G4double zMax)
{
  G4int result = evtHistos->Create3DHistogram(name, title,
					      nBinsX, xMin, xMax,
					      nBinsY, yMin, yMax,
					      nBinsZ, zMin, zMax);
  // index from runHistos will be the same as used only through interfaces in this class
  runHistos->Create3DHistogram(name, title,
			       nBinsX, xMin, xMax,
			       nBinsY, yMin, yMax,
			       nBinsZ, zMin, zMax);
  return result;
}

void BDSOutputStructures::InitialiseSamplers()
{
  if (!localSamplersInitialised)
    {
#ifdef USE_SIXTRACKLINK
      // TODO hardcoded because of sixtrack dynamic buildup
      // Sixtrack does lazy initialisation for collimators in link to Geant4 so we don't know
      // a priori how many link elements there'll be. If we allow it to be dynamically built up
      // we risk the vector expanding and moving in memory, therefore breaking all the && (address
      // of pointer) links of TTree::SetBranchAddress in the output. Therefore, we reserve a size
      // of 300 in the hope that this is less than the LHC 120 collimators for both beams. Ideally,
      // the sixtrack interface should be rewritten so we know at construction time how many will
      // be built.
      samplerTrees.reserve(300);
#endif
      localSamplersInitialised = true;
      for (const auto& samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
        {// create sampler structure
#ifndef __ROOTDOUBLE__
	  BDSOutputROOTEventSampler<float>*  res = new BDSOutputROOTEventSampler<float>(samplerName);
#else
	  BDSOutputROOTEventSampler<double>* res = new BDSOutputROOTEventSampler<double>(samplerName);
#endif
	  samplerTrees.push_back(res);
	  samplerNames.push_back(samplerName);
        }
    }
}

G4int BDSOutputStructures::UpdateSamplerStructures()
{
  G4int result = 0;
  for (auto const& samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {// only put it in if it doesn't exist already
      if (std::find(samplerNames.begin(), samplerNames.end(), samplerName) == samplerNames.end())
	{
	  result++;
#ifndef __ROOTDOUBLE__
	  BDSOutputROOTEventSampler<float>* res = new BDSOutputROOTEventSampler<float>(samplerName);
#else
	  BDSOutputROOTEventSampler<double>* res = new BDSOutputROOTEventSampler<double>(samplerName);
#endif
	  samplerTrees.push_back(res);
	  samplerNames.push_back(samplerName);
	}
    }
  return result;
}

void BDSOutputStructures::PrepareCollimatorInformation()
{
  const G4String collimatorPrefix = "COLL_";
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  collimatorIndices = flatBeamline->GetIndicesOfCollimators();
  nCollimators = (G4int)collimatorIndices.size();
  
  for (auto index : collimatorIndices)
    {
      // prepare output structure name
      const BDSBeamlineElement* el = flatBeamline->at(index);
      // use the 'placement' name for a unique name (with copy number included)
      G4String collimatorName = collimatorPrefix + el->GetPlacementName();
      collimatorNames.push_back(collimatorName);
      collimatorIndicesByName[el->GetName()]          = index;
      collimatorIndicesByName[el->GetPlacementName()] = index;
      
      BDSOutputROOTEventCollimatorInfo info;
      info.Fill(el);
      collimatorInfo.push_back(info);

      // cache difference in apertures for efficient interpolation and avoid
      // repeated calculation. not required in info for output though.
      G4double xDiff = info.xSizeOut - info.xSizeIn;
      G4double yDiff = info.ySizeOut - info.ySizeIn;
      collimatorDifferences.emplace_back(xDiff, yDiff); // construct in place
    }
}

void BDSOutputStructures::InitialiseCollimators()
{
  if (!localCollimatorsInitialised)
    {
      localCollimatorsInitialised = true;
      for (int i = 0; i < (int)collimatorIndices.size(); i++)
	{collimators.push_back(new BDSOutputROOTEventCollimator());}
    }
}

void BDSOutputStructures::ClearStructuresParticleData()
{
  particleDataOutput->Flush();
}

void BDSOutputStructures::ClearStructuresHeader()
{
  headerOutput->Flush();
}

void BDSOutputStructures::ClearStructuresModel()
{
  modelOutput->Flush();
}

void BDSOutputStructures::ClearStructuresBeam()
{
  *beamOutput = BDSOutputROOTEventBeam(); // default
}

void BDSOutputStructures::ClearStructuresOptions()
{
  *optionsOutput = BDSOutputROOTEventOptions(); // default
}

void BDSOutputStructures::ClearStructuresEventLevel()
{
  primary->Flush();
  for (auto sampler : samplerTrees)
    {sampler->Flush();}
  for (auto collimator : collimators)
    {collimator->Flush();}
  primaryGlobal->Flush();
  eLoss->Flush();
  eLossVacuum->Flush();
  eLossTunnel->Flush();
  eLossWorld->Flush();
  eLossWorldExit->Flush();
  eLossWorldContents->Flush();
  pFirstHit->Flush();
  pLastHit->Flush();
  apertureImpacts->Flush();
  traj->Flush();
  evtHistos->Flush();
  evtInfo->Flush();
}

void BDSOutputStructures::ClearStructuresRunLevel()
{
  runInfo->Flush();
}
