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
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutputStructures.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventExit.hh"
#include "BDSOutputROOTEventHeader.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventModel.hh"
#include "BDSOutputROOTEventOptions.hh"
#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventSampler.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTGeant4Data.hh"
#include "BDSSamplerHit.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSTrajectoryPoint.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

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
  // store the model id if either modelID requested or store links
  storeModelID = storeModelID || storeLinks;

  geant4DataOutput = new BDSOutputROOTGeant4Data();
  headerOutput  = new BDSOutputROOTEventHeader();
  beamOutput    = new BDSOutputROOTEventBeam();
  optionsOutput = new BDSOutputROOTEventOptions();
  modelOutput   = new BDSOutputROOTEventModel(storeCollimatorInfo);

  eLoss       = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy);
  eLossVacuum = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy);
  eLossTunnel = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy);
  eLossWorld  = new BDSOutputROOTEventLoss(storeTurn, storeLinks, storeModelID, storeLocal,
					   storeGlobal, storeTime, storeStepLength,
					   storePreStepKineticEnergy);
  
  eLossWorldExit = new BDSOutputROOTEventExit();

  pFirstHit  = new BDSOutputROOTEventLoss(true, true,  true, true,  true, true,  false, false);
  pLastHit   = new BDSOutputROOTEventLoss(true, true,  true, true,  true, true,  false, false);
  
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
  samplerTrees.push_back(primary);
  samplerNames.push_back("Primary");
  primaryGlobal = new BDSOutputROOTEventCoords();
}

BDSOutputStructures::~BDSOutputStructures()
{
  delete geant4DataOutput;
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
  delete pFirstHit;
  delete pLastHit;
  delete traj;
  delete evtHistos;
  delete evtInfo;
  delete runHistos;
  delete runInfo;
  for (auto sampler : samplerTrees)
    {delete sampler;}
  for (auto collimator : collimators)
    {delete collimator;}
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

void BDSOutputStructures::InitialiseSamplers()
{
  if (!localSamplersInitialised)
    {
      localSamplersInitialised = true;
      for (auto const samplerName : BDSSamplerRegistry::Instance()->GetUniqueNames())
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
      // use the 'placement' name for a unique name (with copynumer included)
      G4String collimatorName = collimatorPrefix + el->GetPlacementName();
      collimatorNames.push_back(collimatorName);
      collimatorIndicesByName[el->GetName()]          = index;
      collimatorIndicesByName[el->GetPlacementName()] = index;
      
      BDSOutputROOTEventCollimatorInfo info;
      info.Fill(el);
      collimatorInfo.push_back(info);

      // cache difference in apertures for efficient interpolation and avoid
      // repeated calcualtion. not requried in info for output though.
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

void BDSOutputStructures::ClearStructuresGeant4Data()
{
  geant4DataOutput->Flush();
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
  pFirstHit->Flush();
  pLastHit->Flush();
  traj->Flush();
  evtHistos->Flush();
}

void BDSOutputStructures::ClearStructuresRunLevel()
{
  runInfo->Flush();
}
