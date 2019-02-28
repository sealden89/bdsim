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
#include "BDSHitCollimator.hh"
#include "BDSDebug.hh"
#include "BDSHitEnergyDeposition.hh"
#include "BDSHitEnergyDepositionGlobal.hh"
#include "BDSEventInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSOutput.hh"
#include "BDSOutputROOTEventBeam.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCollimatorInfo.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventLossWorld.hh"
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
#include "BDSPrimaryVertexInformation.hh"
#include "BDSHitSampler.hh"
#include "BDSStackingAction.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include <algorithm>
#include <cmath>
#include <ostream>
#include <set>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "PrimaryGlobal",
  "Eloss", "ElossVacuum", "ElossTunnel", "ElossWorld", "ElossWorldExit",
  "ElossWorldContents",
  "PrimaryFirstHit", "PrimaryLastHit", "Trajectory"
};

BDSOutput::BDSOutput(G4String baseFileNameIn,
		     G4String fileExtensionIn,
		     G4int    fileNumberOffset):
  BDSOutputStructures(BDSGlobalConstants::Instance()),
  baseFileName(baseFileNameIn),
  fileExtension(fileExtensionIn),
  outputFileNumber(fileNumberOffset),
  sMaxHistograms(0),
  nbins(0),
  energyDeposited(0),
  energyDepositedVacuum(0),
  energyDepositedWorld(0),
  energyDepositedWorldContents(0),
  energyDepositedTunnel(0),
  energyWorldExit(0),
  nCollimatorsInteracted(0)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  writePrimaries     = g->WritePrimaries();
  useScoringMap      = g->UseScoringMap();

  storeCollimatorLinks       = g->StoreCollimatorLinks();
  // automatically store ion info if generating ion hits - this option
  // controls generation and storage of the ion hits
  storeCollimatorHitsIons    = g->StoreCollimatorHitsIons();
  storeCollimatorInfo        = g->StoreCollimatorInfo();

  createCollimatorOutputStructures = storeCollimatorInfo || storeCollimatorLinks || storeCollimatorHitsIons;

  storeELoss                 = g->StoreELoss();
  // store histograms if storing general energy deposition as negligible in size
  storeELossHistograms       = g->StoreELossHistograms() || storeELoss;
  storeELossTunnel           = g->StoreELossTunnel();
  storeELossTunnelHistograms = g->StoreELossTunnelHistograms() || storeELossTunnel;
  storeELossVacuum           = g->StoreELossVacuum();
  storeELossVacuumHistograms = g->StoreELossVacuumHistograms() || storeELossVacuum;
  storeELossWorld            = g->StoreELossWorld();
  storeELossWorldContents    = g->StoreELossWorldContents() || g->UseImportanceSampling();
  storeGeant4Data            = g->StoreGeant4Data();
  storeModel                 = g->StoreModel();
  storeSamplerPolarCoords    = g->StoreSamplerPolarCoords();
  storeSamplerCharge         = g->StoreSamplerCharge();
  storeSamplerKineticEnergy  = g->StoreSamplerKineticEnergy();
  storeSamplerMass           = g->StoreSamplerMass();
  storeSamplerRigidity       = g->StoreSamplerRigidity();
  storeSamplerIon            = g->StoreSamplerIon();

  // polar coords don't require a look up of any other PDG info so it
  // doesn't need to involved in the following optimisation groupings
  // of various options

  // charge is required for rigidity calculation so force storage from sampler hits
  if (storeSamplerRigidity && !storeSamplerCharge)
    {storeSamplerCharge = true;}

  // charge + mass + rigidity - particle stuff
  storeOption1 = storeSamplerCharge && storeSamplerMass & storeSamplerRigidity;
  // charge + mass + rigidity + kinetic energy - particle stuff
  storeOption2 = storeOption1 && storeSamplerKineticEnergy;
  // everything except ion properties
  storeOption3 = storeOption1 && storeSamplerIon;
  // everything
  storeOption4 = storeOption2 && storeSamplerIon;

  // easy option for everything - overwrite bools we've just set individually
  if (g->StoreSamplerAll())
    {
      storeOption4       = true;
      storeSamplerCharge = true;
      storeSamplerPolarCoords = true;
    }
}

void BDSOutput::InitialiseGeometryDependent()
{
  PrepareCollimatorInformation();
  CreateHistograms();
  InitialiseSamplers();
  InitialiseCollimators();
}

void BDSOutput::FillHeader()
{
  headerOutput->Flush();
  headerOutput->Fill(); // updates time stamp
  WriteHeader();
  ClearStructuresHeader();
}

void BDSOutput::FillGeant4Data(const G4bool& writeIons)
{
  if (storeGeant4Data)
    {
      geant4DataOutput->Flush();
      geant4DataOutput->Fill(writeIons);
      WriteGeant4Data();
#ifdef __ROOTDOUBLE__
      BDSOutputROOTEventSampler<double>::particleTable = geant4DataOutput;
#else
      BDSOutputROOTEventSampler<float>::particleTable = geant4DataOutput;
#endif
      BDSOutputROOTEventCollimator::particleTable = geant4DataOutput;
    }
}

void BDSOutput::FillBeam(const GMAD::BeamBase* beam)
{
  *beamOutput = BDSOutputROOTEventBeam(beam);
  WriteBeam();
  ClearStructuresBeam();
}

void BDSOutput::FillOptions(const GMAD::OptionsBase* options)
{
  *optionsOutput = BDSOutputROOTEventOptions(options);
  WriteOptions();
  ClearStructuresOptions();
}

void BDSOutput::FillModel()
{
  if (storeModel)
    {
      modelOutput->Fill(collimatorIndices,
			collimatorIndicesByName,
			collimatorInfo,
			collimatorNames);
      WriteModel();
      ClearStructuresModel();
    }
}

void BDSOutput::FillPrimary(const G4PrimaryVertex* vertex,
			    const G4int            turnsTaken)
{
  auto vertexInfo    = vertex->GetUserInformation();
  auto vertexInfoBDS = dynamic_cast<const BDSPrimaryVertexInformation*>(vertexInfo);
  if (vertexInfoBDS)
    {
      primary->Fill(vertexInfoBDS->primaryVertex.local,
		    vertexInfoBDS->charge,
		    vertex->GetPrimary()->GetPDGcode(),
		    turnsTaken,
		    vertexInfoBDS->primaryVertex.beamlineIndex);
      primaryGlobal->Fill(vertexInfoBDS->primaryVertex.global);
    }
}

void BDSOutput::FillEventPrimaryOnly(const BDSParticleCoordsFullGlobal& coords,
				     const G4double charge,
				     const G4int pdgID)
{
  primary->Fill(coords.local, charge, pdgID, 0, 0);
  primaryGlobal->Fill(coords.global);
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::FillEvent(const BDSEventInfo*                            info,
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
			  const std::map<BDSTrajectory*,bool>&           trajectories,
			  const BDSHitsCollectionCollimator*             collimatorHits,
			  const G4int                                    turnsTaken)
{
  // Clear integrals in this class -> here instead of BDSOutputStructures as
  // looped over here -> do only once as expensive as lots of hits
  energyDeposited              = 0;
  energyDepositedVacuum        = 0;
  energyDepositedWorld         = 0;
  energyDepositedWorldContents = 0;
  energyDepositedTunnel        = 0;
  energyWorldExit              = 0;
  nCollimatorsInteracted       = 0;
  
  if (vertex)
    {FillPrimary(vertex, turnsTaken);}
  if (samplerHitsPlane)
    {FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::plane);}
  if (samplerHitsCylinder)
    {FillSamplerHits(samplerHitsCylinder, BDSOutput::HitsType::cylinder);}
  if (energyLoss)
    {FillEnergyLoss(energyLoss,        BDSOutput::LossType::energy);}
  if (energyLossFull)
    {FillEnergyLoss(energyLossFull,    BDSOutput::LossType::energy);}
  if (energyLossVacuum)
    {FillEnergyLoss(energyLossVacuum,  BDSOutput::LossType::vacuum);}
  if (energyLossTunnel)
    {FillEnergyLoss(energyLossTunnel,  BDSOutput::LossType::tunnel);}
  if (energyLossWorld)
    {FillEnergyLoss(energyLossWorld,   BDSOutput::LossType::world);}
  if (worldExitHits)
    {FillEnergyLoss(worldExitHits,     BDSOutput::LossType::worldexit);}
  if (energyLossWorldContents)
    {FillEnergyLoss(energyLossWorldContents, BDSOutput::LossType::worldcontents);}
  if (primaryHit)
    {FillPrimaryHit(primaryHit);}
  if (primaryLoss)
    {FillPrimaryLoss(primaryLoss);}
  FillTrajectories(trajectories);
  if (collimatorHits)
    {FillCollimatorHits(collimatorHits, primaryLoss);}

  // we do this after energy loss and collimator hits as the energy loss
  // is integrated for putting in event info and the number of colliamtors
  // interacted with counted
  if (info)
    {FillEventInfo(info);}
  
  WriteFileEventLevel();
  ClearStructuresEventLevel();
}

void BDSOutput::CloseAndOpenNewFile()
{
  CloseFile();
  NewFile();
  InitialiseGeometryDependent();
}

void BDSOutput::FillRun(const BDSEventInfo* info)
{
  FillRunInfo(info);
  WriteFileRunLevel();
  ClearStructuresRunLevel();
}

G4bool BDSOutput::InvalidSamplerName(const G4String& samplerName)
{
  return protectedNames.find(samplerName) != protectedNames.end();
}

void BDSOutput::PrintProtectedNames(std::ostream& out)
{
  out << "Protected names for output " << G4endl;
  for (auto key : protectedNames)
    {out << "\"" << key << "\"" << G4endl; }
}
 
G4String BDSOutput::GetNextFileName()
{
  outputFileNumber++;
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  // Base root file name 
  G4String newFileName = baseFileName;

  // if more than one file add number (starting at 0)
  // of numberEventPerFile is specified and the number already generated exceeds that
  if (numberEventPerFile > 0 && globalConstants->NGenerate() > numberEventPerFile)
    {newFileName += "_" + std::to_string(outputFileNumber);} // note underscore
  
  // policy: overwrite if output filename specifically set, otherwise increase number
  // always check in interactive mode
  if (!globalConstants->OutputFileNameSet() || !globalConstants->Batch())
    {// check if file exists
      G4String original = newFileName; // could have nper file number suffix too
      G4int nTimeAppended = 1;
      while (BDS::FileExists(newFileName + fileExtension)) // always test with extension
	{// if exists increment suffix integer
	  newFileName = original + "-" + std::to_string(nTimeAppended);
	  nTimeAppended +=1;
	}
    }

  // add extension now we've got the base part fixed
  newFileName += fileExtension;
  
  G4cout << __METHOD_NAME__ << "Setting up new file: " << newFileName << G4endl;

  return newFileName;
}

void BDSOutput::CalculateHistogramParameters()
{
  // rounding up so last bin definitely covers smax
  // (max - min) / bin width -> min = 0 here.
  const G4double binWidth = BDSGlobalConstants::Instance()->ELossHistoBinWidth();
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline)
    {// don't access a nullptr
      if (!flatBeamline->empty())
        {
	  G4double sMax = flatBeamline->GetLastItem()->GetSPositionEnd();
	  nbins = (int) std::ceil(sMax / binWidth); // round up to integer # of bins
        }
    }
  else
    {nbins = 1;} // can happen for generate primaries only
  
  sMaxHistograms = nbins * binWidth;
}

void BDSOutput::CreateHistograms()
{
  // construct output histograms
  // calculate histogram dimensions
  CalculateHistogramParameters();
  const G4double smin   = 0.0;
  const G4double smax   = sMaxHistograms / CLHEP::m;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "histogram parameters calculated to be: " << G4endl;
  G4cout << "s minimum: " << smin     << " m" << G4endl;
  G4cout << "s maximum: " << smax     << " m" << G4endl;
  G4cout << "# of bins: " << nbins    << G4endl;
#endif
  // create the histograms
  histIndices1D["Phits"] = Create1DHistogram("PhitsHisto","Primary Hits",nbins,smin,smax);
  histIndices1D["Ploss"] = Create1DHistogram("PlossHisto","Primary Loss",nbins,smin,smax);
  if (storeELossHistograms)
    {histIndices1D["Eloss"] = Create1DHistogram("ElossHisto","Energy Loss",nbins,smin,smax);}
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges;
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline) // can be nullptr in case of generate primaries only
    {binedges = flatBeamline->GetEdgeSPositions();}
  else
    {binedges = {0,1};}
  // create per element ("pe") bin width histograms
  histIndices1D["PhitsPE"] = Create1DHistogram("PhitsPEHisto",
					       "Primary Hits per Element",
					       binedges);
  histIndices1D["PlossPE"] = Create1DHistogram("PlossPEHisto",
					       "Primary Loss per Element",
					       binedges);
  if (storeELossHistograms)
    {
      histIndices1D["ElossPE"] = Create1DHistogram("ElossPEHisto",
						   "Energy Loss per Element" ,
						   binedges);
    }
  if (storeELossVacuumHistograms)
    {
      histIndices1D["ElossVacuum"] = Create1DHistogram("ElossVacuumHisto",
						       "Energy Loss in Vacuum",
						       nbins,smin,smax);
      histIndices1D["ElossVacuumPE"] = Create1DHistogram("ElossVaccumPEHisto",
							 "Energy Loss in Vacuum per Element" ,
							 binedges);
    }
  
  // only create tunnel histograms if we build the tunnel
  const BDSBeamline* tunnelBeamline = BDSAcceleratorModel::Instance()->TunnelBeamline();
  if (!tunnelBeamline)
    {
      storeELossTunnel = false;
      storeELossTunnelHistograms = false;
    }
  if (storeELossTunnelHistograms)
    {
      binedges = tunnelBeamline->GetEdgeSPositions();
      histIndices1D["ElossTunnel"] = Create1DHistogram("ElossTunnelHisto",
						       "Energy Loss in Tunnel",
						       nbins, smin,smax);
      histIndices1D["ElossTunnelPE"] = Create1DHistogram("ElossTunnelPEHisto",
							 "Energy Loss in Tunnel per Element",
							 binedges);
    }

  if (storeCollimatorInfo && nCollimators > 0)
    {
      std::vector<G4String> collHistNames = {"CollPhitsPE",
					     "CollPlossPE",
					     "CollElossPE",
					     "CollPInteractedPE"};
      std::vector<G4String> collHistDesciptions = {"Primary Hits per Coll",
						   "Primary Loss per Coll",
						   "Energy Loss per Collimator",
						   "Primary Interacted per Collimator"};
      for (G4int i = 0; i < (G4int)collHistNames.size(); i++)
	{
	  histIndices1D[collHistNames[i]] = Create1DHistogram(collHistNames[i],
							      collHistDesciptions[i],
							      nCollimators, 0, nCollimators);
	}
    }
  
  if (useScoringMap && storeELossHistograms)
    {
      const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
      G4int scInd = evtHistos->Create3DHistogram("ScoringMap", "Energy Deposition",
						 g->NBinsX(), g->XMin()/CLHEP::m, g->XMax()/CLHEP::m,
						 g->NBinsY(), g->YMin()/CLHEP::m, g->YMax()/CLHEP::m,
						 g->NBinsZ(), g->ZMin()/CLHEP::m, g->ZMax()/CLHEP::m);
      histIndices3D["ScoringMap"] = scInd;
    }
}

void BDSOutput::FillEventInfo(const BDSEventInfo* info)
{
  if (info)
    {*evtInfo = *(info->GetInfo());}
  evtInfo->energyDeposited              = energyDeposited;
  evtInfo->energyDepositedVacuum        = energyDepositedVacuum;
  evtInfo->energyDepositedWorld         = energyDepositedWorld;
  evtInfo->energyDepositedWorldContents = energyDepositedWorldContents;
  evtInfo->energyDepositedTunnel        = energyDepositedTunnel;
  evtInfo->energyWorldExit              = energyWorldExit;
  G4double ek = BDSStackingAction::energyKilled / CLHEP::GeV;
  evtInfo->energyKilled = ek;
  evtInfo->energyTotal =  energyDeposited
    + energyDepositedVacuum
    + energyDepositedWorld
    + energyDepositedWorldContents
    + energyDepositedTunnel
    + energyWorldExit
    + ek;

  evtInfo->nCollimatorsInteracted = nCollimatorsInteracted;
}

void BDSOutput::FillSamplerHits(const BDSHitsCollectionSampler* hits,
				const BDSOutput::HitsType)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << hits->entries() << std::endl;
#endif
  // Here, we don't switch on the type of the hits as the samplers are all
  // prepared and stored in one vector in the sampler registry.  The output
  // structures are based on this and cylinder output is no different from
  // plane output and indices will match.

  // TBC - cylinder output will have all the same z and S, which is wrong!
  if (!(hits->entries() > 0))
    {return;}
  for (int i = 0; i < hits->entries(); i++)
    {
      const BDSHitSampler* hit = (*hits)[i];
      G4int samplerID = hit->samplerID;
      samplerID += 1; // offset index by one due to primary branch.
      samplerTrees[samplerID]->Fill(hit, storeSamplerCharge, storeSamplerPolarCoords);
    }

  // extra information
  // choose by a few strategies for optimisation (reduced PDGid searching)
  // some options partially degenerate with lower numbered options - check first
  if (storeOption4) // everything
    {
      for (auto &sampler : samplerTrees)
        {sampler->FillMRIK();}
    }
  else if (storeOption3) // option1 + ion
    {
      for (auto &sampler : samplerTrees)
      {sampler->FillMRI();}
    }
  else if (storeOption2) // option1 + kinetic energy
    {
      for (auto &sampler : samplerTrees)
      {sampler->FillMRK();}
    }
  else if (storeOption1) // also applies for 2 and 3
    {
      for (auto &sampler : samplerTrees)
        {sampler->FillMR();}
    }
  else
    {// treat individually
      for (auto& sampler : samplerTrees)
        {
          if (storeSamplerKineticEnergy)
	    {sampler->FillKineticEnergy();}
          if (storeSamplerMass)
	    {sampler->FillMass();}
          if (storeSamplerRigidity)
	    {sampler->FillRigidity();}
          if (storeSamplerIon)
	    {sampler->FillIon();}
        }
    }
}

void BDSOutput::FillEnergyLoss(const BDSHitsCollectionEnergyDepositionGlobal* hits,
			       const LossType lossType)
{
  switch (lossType)
    {
    case BDSOutput::LossType::world:
    case BDSOutput::LossType::worldexit:
    case BDSOutput::LossType::worldcontents:
      {break;}
    default:
      {return; break;} // don't fill for other types of hits
    }

  G4int nHits = hits->entries();
  if (nHits == 0) // integer so ok to compare
    {return;}
  switch (lossType)
    {
    case BDSOutput::LossType::world:
      {
	for (G4int i=0; i < nHits; i++)
	  {
	    BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
	    energyDepositedWorld += hit->TotalEnergyWeighted()/CLHEP::GeV;
	    if (storeELossWorld)
	      {eLossWorld->Fill(hit);}
	  }
        break;
      }
    case BDSOutput::LossType::worldexit:
      {
	for (G4int i = 0; i < nHits; i++)
	  {
	    BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
	    energyWorldExit += hit->TotalEnergyWeighted()/CLHEP::GeV;
	    if (storeELossWorld)
	      {eLossWorldExit->Fill(hit);}
	  }
        break;
      }
    case BDSOutput::LossType::worldcontents:
      {
	for (G4int i = 0; i < nHits; i++)
	  {
	    BDSHitEnergyDepositionGlobal* hit = (*hits)[i];
	    energyDepositedWorldContents += hit->TotalEnergyWeighted()/CLHEP::GeV;
	    if (storeELossWorldContents)
	      {eLossWorldContents->Fill(hit);}
	  }
        break;
      }
    default:
      {break;} // only to prevent compiler warning
    }
}

void BDSOutput::FillEnergyLoss(const BDSHitsCollectionEnergyDeposition* hits,
			       const LossType lossType)
{
  G4int nHits            = hits->entries();
  if (nHits == 0)
    {return;}
  G4int indELoss         = histIndices1D["Eloss"];
  G4int indELossPE       = histIndices1D["ElossPE"];
  G4int indELossTunnel   = -1;
  G4int indELossTunnelPE = -1;
  if (storeELossTunnelHistograms)
    {
      indELossTunnel   = histIndices1D["ElossTunnel"];
      indELossTunnelPE = histIndices1D["ElossTunnelPE"];
    }
  G4int indELossVacuum   = -1;
  G4int indELossVacuumPE = -1;
  if (storeELossVacuumHistograms)
    {
      indELossVacuum   = histIndices1D["ElossVacuum"];
      indELossVacuumPE = histIndices1D["ElossVacuumPE"];
    }
  G4int indScoringMap    = -1;
  if (useScoringMap)
    {indScoringMap = histIndices3D["ScoringMap"];}
  for (G4int i=0; i < nHits; i++)
    {
      BDSHitEnergyDeposition* hit = (*hits)[i];
      G4double sHit = hit->GetSHit()/CLHEP::m;
      G4double eW   = hit->GetEnergyWeighted()/CLHEP::GeV;
      switch (lossType)
	{
	case BDSOutput::LossType::energy:
	  {
	    energyDeposited += eW;
	    if (storeELoss)
	      {eLoss->Fill(hit);}
	    if (storeELossHistograms)
	      {
		runHistos->Fill1DHistogram(indELoss,   sHit, eW);
		evtHistos->Fill1DHistogram(indELoss,   sHit, eW);
		runHistos->Fill1DHistogram(indELossPE, sHit, eW);
		evtHistos->Fill1DHistogram(indELossPE, sHit, eW);
	      }
	    break;
	  }
	case BDSOutput::LossType::vacuum:
	  {
	    energyDepositedVacuum += eW;
	    if (storeELossVacuum)
	      {eLossVacuum->Fill(hit);}
	    if (storeELossVacuumHistograms)
	      {
		evtHistos->Fill1DHistogram(indELossVacuum,   sHit, eW);
		runHistos->Fill1DHistogram(indELossVacuumPE, sHit, eW);
	      }
	    break;
	  }
	case BDSOutput::LossType::tunnel:
	  {
	    energyDepositedTunnel += eW;
	    if (storeELossTunnel)
	      {eLossTunnel->Fill(hit);}
	    if (storeELossTunnelHistograms)
	      {
		runHistos->Fill1DHistogram(indELossTunnel,   sHit, eW);
		evtHistos->Fill1DHistogram(indELossTunnel,   sHit, eW);
		runHistos->Fill1DHistogram(indELossTunnelPE, sHit, eW);
		evtHistos->Fill1DHistogram(indELossTunnelPE, sHit, eW);
	      }
	    break;
	  }
	default:
	  {break;} // only to prevent compiler warning
	}
      
      if (useScoringMap)
	{
	  G4double x = hit->Getx()/CLHEP::m;
	  G4double y = hit->Gety()/CLHEP::m;
	  evtHistos->Fill3DHistogram(indScoringMap, x, y, sHit, eW);
	}
    }

  if (storeCollimatorInfo &&
      nCollimators > 0 &&
      (lossType == BDSOutput::LossType::energy) &&
      storeELossHistograms)
    {CopyFromHistToHist1D("ElossPE", "CollElossPE", collimatorIndices);}
}

void BDSOutput::FillPrimaryHit(const BDSTrajectoryPoint* phit)
{
  pFirstHit->Fill(phit);
  const G4double preStepSPosition = phit->GetPreS() / CLHEP::m;
  runHistos->Fill1DHistogram(histIndices1D["Phits"],   preStepSPosition);
  evtHistos->Fill1DHistogram(histIndices1D["Phits"],   preStepSPosition);
  runHistos->Fill1DHistogram(histIndices1D["PhitsPE"], preStepSPosition);
  evtHistos->Fill1DHistogram(histIndices1D["PhitsPE"], preStepSPosition);
  
  if (storeCollimatorInfo && nCollimators > 0)
    {CopyFromHistToHist1D("PhitsPE", "CollPhitsPE", collimatorIndices);}
}

void BDSOutput::FillPrimaryLoss(const BDSTrajectoryPoint* ploss)
{
  pLastHit->Fill(ploss);
  const G4double postStepSPosition = ploss->GetPostS() / CLHEP::m;
  runHistos->Fill1DHistogram(histIndices1D["Ploss"],   postStepSPosition);
  evtHistos->Fill1DHistogram(histIndices1D["Ploss"],   postStepSPosition);
  runHistos->Fill1DHistogram(histIndices1D["PlossPE"], postStepSPosition);
  evtHistos->Fill1DHistogram(histIndices1D["PlossPE"], postStepSPosition);

  if (storeCollimatorInfo && nCollimators > 0)
    {CopyFromHistToHist1D("PlossPE", "CollPlossPE", collimatorIndices);}
}

void BDSOutput::FillTrajectories(const std::map<BDSTrajectory*, bool>& trajectories)
{
  traj->Fill(trajectories);
}

void BDSOutput::FillCollimatorHits(const BDSHitsCollectionCollimator* hits,
				   const BDSTrajectoryPoint* primaryLossPoint)
{
  G4int nHits = hits->entries();
  for (G4int i = 0; i < nHits; i++)
    {
      BDSHitCollimator* hit = (*hits)[i];
      G4int collimatorIndex = hit->collimatorIndex;      
      collimators[collimatorIndex]->Fill(hit,
					 collimatorInfo[collimatorIndex],
					 collimatorDifferences[collimatorIndex]);
    }

  // identify whether the primary loss point was in a collimator
  // only do if there's a beam line, ie finished in a beam line, and there are collimators
  if (primaryLossPoint->GetBeamLine() && nCollimators > 0)
    {
      G4int lossPointBLInd = primaryLossPoint->GetBeamLineIndex(); // always the mass world index
      auto result = std::find(collimatorIndices.begin(), collimatorIndices.end(), lossPointBLInd);
      if (result != collimatorIndices.end())
	{
	  G4int collIndex = (int) (result - collimatorIndices.begin());
	  collimators[collIndex]->SetPrimaryStopped(true);
	}
    }
  
  // if required loop over collimators and get them to calculate and fill extra information
  if (storeCollimatorLinks || storeCollimatorHitsIons)
    {
      for (auto collimator : collimators)
	{collimator->FillExtras(storeCollimatorHitsIons, storeCollimatorLinks);}
    }

  // after all collimator hits have been filled, we summarise whether the primary
  // interacted in a histogram
  G4int histIndex = histIndices1D["CollPInteractedPE"];
  for (G4int i = 0; i < (G4int)collimators.size(); i++)
    {
      evtHistos->Fill1DHistogram(histIndex, i,
				 (int)collimators[i]->primaryInteracted);
    }


  // loop over collimators and count the number that were interacted with in this event
  for (const auto collimator : collimators)
    {
      if (collimator->primaryInteracted)
	{nCollimatorsInteracted += 1;}
    }
}

void BDSOutput::FillRunInfo(const BDSEventInfo* info)
{
  if (info)
    {*runInfo = BDSOutputROOTEventRunInfo(info->GetInfo());}
}

void BDSOutput::CopyFromHistToHist1D(const G4String sourceName,
				     const G4String destinationName,
				     const std::vector<G4int> indices)
{
  TH1D* sourceEvt      = evtHistos->Get1DHistogram(histIndices1D[sourceName]);
  TH1D* destinationEvt = evtHistos->Get1DHistogram(histIndices1D[destinationName]);
  // for the run ones we are overwriting but this is ok
  TH1D* sourceRun      = runHistos->Get1DHistogram(histIndices1D[sourceName]);
  TH1D* destinationRun = runHistos->Get1DHistogram(histIndices1D[destinationName]);
  G4int binIndex = 1; // starts at 1 for TH1; 0 is underflow
  for (const auto index : indices)
    {
      destinationEvt->SetBinContent(binIndex, sourceEvt->GetBinContent(index + 1));
      destinationEvt->SetBinError(binIndex,   sourceEvt->GetBinError(index + 1));
      destinationRun->SetBinContent(binIndex, sourceRun->GetBinContent(index + 1));
      destinationRun->SetBinError(binIndex,   sourceRun->GetBinError(index + 1));
      binIndex++;
    }
}
