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
#include "BDSOutput.hh"
#include "BDSOutputROOTEventBeam.hh"
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
#include "BDSPrimaryVertexInformation.hh"
#include "BDSSamplerHit.hh"
#include "BDSStackingAction.hh"
#include "BDSTrajectoryPoint.hh"
#include "BDSUtilities.hh"
#include "BDSVolumeExitHit.hh"

#include "globals.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

#include "parser/beamBase.h"
#include "parser/optionsBase.h"

#include <cmath>
#include <ostream>
#include <set>
#include <vector>

#include "CLHEP/Units/SystemOfUnits.h"

const std::set<G4String> BDSOutput::protectedNames = {
  "Event", "Histos", "Info", "Primary", "PrimaryGlobal", "Eloss",
  "PrimaryFirstHit", "PrimaryLastHit", "TunnelHit", "ElossWorld",
  "ElossWorldExit", "Trajectory"
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
  energyDepositedWorld(0),
  energyDepositedTunnel(0),
  energyWorldExit(0)
{
  const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
  numberEventPerFile = g->NumberOfEventsPerNtuple();
  writePrimaries     = g->WritePrimaries();
  useScoringMap      = g->UseScoringMap();

  storeGeant4Data      = g->StoreGeant4Data();
  storeModel           = g->StoreModel();
  storeSamplerPolarCoords   = g->StoreSamplerPolarCoords();
  storeSamplerCharge   = g->StoreSamplerCharge();
  storeSamplerKineticEnergy = g->StoreSamplerKineticEnergy();
  storeSamplerMass     = g->StoreSamplerMass();
  storeSamplerRigidity = g->StoreSamplerRigidity();
  storeSamplerIon      = g->StoreSamplerIon();

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
  CreateHistograms();
  InitialiseSamplers();
}

void BDSOutput::FillHeader()
{
  headerOutput->Flush();
  headerOutput->Fill(); // updates time stamp
  WriteHeader();
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
      modelOutput->Fill();
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

void BDSOutput::FillEvent(const BDSEventInfo*                   info,
			  const G4PrimaryVertex*                vertex,
			  const BDSSamplerHitsCollection*       samplerHitsPlane,
			  const BDSSamplerHitsCollection*       samplerHitsCylinder,
			  const BDSEnergyCounterHitsCollection* energyLoss,
			  const BDSEnergyCounterHitsCollection* tunnelLoss,
			  const BDSEnergyCounterHitsCollection* worldLoss,
			  const BDSVolumeExitHitsCollection*    worldExitHits,
			  const BDSTrajectoryPoint*             primaryHit,
			  const BDSTrajectoryPoint*             primaryLoss,
			  const std::map<BDSTrajectory*,bool>&  trajectories,
			  const G4int                           turnsTaken)
{
  // Clear integrals in this class -> here instead of BDSOutputStructures as
  // looped over here -> do only once as expensive as lots of hits
  energyDeposited       = 0;
  energyDepositedWorld  = 0;
  energyDepositedTunnel = 0;
  energyWorldExit       = 0;
  
  if (vertex)
    {FillPrimary(vertex, turnsTaken);}
  if (samplerHitsPlane)
    {FillSamplerHits(samplerHitsPlane, BDSOutput::HitsType::plane);}
  if (samplerHitsCylinder)
    {FillSamplerHits(samplerHitsCylinder, BDSOutput::HitsType::cylinder);}
  if (energyLoss)
    {FillEnergyLoss(energyLoss, BDSOutput::LossType::energy);}
  if (tunnelLoss)
    {FillEnergyLoss(tunnelLoss, BDSOutput::LossType::tunnel);}
  if (worldLoss)
    {FillEnergyLoss(worldLoss,  BDSOutput::LossType::world);}
  if (worldExitHits)
    {FillElossWorldExitHits(worldExitHits);}

  // we do this after energy loss as the energy loss is integrated for
  // putting in event info
  if (info)
    {FillEventInfo(info);}
  
  if (primaryHit)
    {FillPrimaryHit(primaryHit);}
  if (primaryLoss)
    {FillPrimaryLoss(primaryLoss);}
  FillTrajectories(trajectories);
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
  const G4double binWidth = BDSGlobalConstants::Instance()->ElossHistoBinWidth();
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
  Create1DHistogram("PhitsHisto","Primary Hits", nbins,smin,smax); // 1
  Create1DHistogram("PlossHisto","Primary Loss", nbins,smin,smax); // 2
  Create1DHistogram("ElossHisto","Energy Loss",  nbins,smin,smax); // 3
  // prepare bin edges for a by-element histogram
  std::vector<G4double> binedges;
  const BDSBeamline* flatBeamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (flatBeamline) // can be nullptr in case of generate primaries only
    {binedges = flatBeamline->GetEdgeSPositions();}
  else
    {binedges = {0,1};}
  // create per element ("pe") bin width histograms
  Create1DHistogram("PhitsPEHisto","Primary Hits per Element", binedges); // 4
  Create1DHistogram("PlossPEHisto","Primary Loss per Element", binedges); // 5
  Create1DHistogram("ElossPEHisto","Energy Loss per Element" , binedges); // 6

  // only create tunnel histograms if we build the tunnel
  const BDSBeamline* tunnelBeamline = BDSAcceleratorModel::Instance()->TunnelBeamline();
  if (tunnelBeamline)
    {
      binedges = tunnelBeamline->GetEdgeSPositions();
      Create1DHistogram("ElossTunnelHisto",   "Energy Loss in Tunnel", nbins, smin,smax); // 7
      Create1DHistogram("ElossTunnelPEHisto", "Energy Loss in Tunnel per Element", binedges); // 8
    }
  
  if (useScoringMap)
    {
      const BDSGlobalConstants* g = BDSGlobalConstants::Instance();
      evtHistos->Create3DHistogram("ScoringMap", "Energy Deposition",
				   g->NBinsX(), g->XMin()/CLHEP::m, g->XMax()/CLHEP::m,
				   g->NBinsY(), g->YMin()/CLHEP::m, g->YMax()/CLHEP::m,
				   g->NBinsZ(), g->ZMin()/CLHEP::m, g->ZMax()/CLHEP::m);
    }
}

void BDSOutput::FillEventInfo(const BDSEventInfo* info)
{
  if (info)
    {*evtInfo = *(info->GetInfo());}
  evtInfo->energyDeposited       = energyDeposited;
  evtInfo->energyDepositedWorld  = energyDepositedWorld;
  evtInfo->energyDepositedTunnel = energyDepositedTunnel;
  evtInfo->energyWorldExit       = energyWorldExit;
  G4double ek = BDSStackingAction::energyKilled / CLHEP::GeV;
  evtInfo->energyKilled = ek;
  evtInfo->energyTotal =  energyDeposited + energyDepositedWorld + energyDepositedTunnel + energyWorldExit + ek;
}

void BDSOutput::FillSamplerHits(const BDSSamplerHitsCollection* hits,
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
      G4int samplerID = (*hits)[i]->samplerID;
      samplerID += 1; // offset index by one due to primary branch.
      samplerTrees[samplerID]->Fill((*hits)[i], storeSamplerCharge,
                                    storeSamplerPolarCoords);
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

void BDSOutput::FillEnergyLoss(const BDSEnergyCounterHitsCollection* hits,
			       const LossType lossType)
{
  G4int n_hit = hits->entries();
  for (G4int i=0;i<n_hit;i++)
    {
      BDSEnergyCounterHit* hit = (*hits)[i];
      G4double sHit = hit->GetSHit()/CLHEP::m;
      G4double eW   = hit->GetEnergyWeighted()/CLHEP::GeV;
      switch (lossType)
	{
	case BDSOutput::LossType::energy:
	  {// number - 1 for the index
	    eLoss->Fill(hit);
	    energyDeposited += eW;
	    runHistos->Fill1DHistogram(2, sHit, eW);
	    evtHistos->Fill1DHistogram(2, sHit, eW);
	    runHistos->Fill1DHistogram(5, sHit, eW);
	    evtHistos->Fill1DHistogram(5, sHit, eW);
	    break;
	  }
	case BDSOutput::LossType::tunnel:
	  {
	    energyDepositedTunnel += eW;
	    tunnelHit->Fill(hit);
	    runHistos->Fill1DHistogram(6, sHit, eW);
	    evtHistos->Fill1DHistogram(6, sHit, eW);
	    runHistos->Fill1DHistogram(7, sHit, eW);
	    evtHistos->Fill1DHistogram(7, sHit, eW);
	    break;
	  }
	case BDSOutput::LossType::world:
	  {
	    //G4cout << __METHOD_NAME__ << "energyDepositedWorld " << energyDepositedWorld << G4endl;
	    energyDepositedWorld += eW;
	    eLossWorld->Fill(hit);

	    break;
	  }
	default:
	  {break;} // only to prevent compiler warning
	}
      
      if (useScoringMap)
	{
	  G4double x = hit->Getx()/CLHEP::m;
	  G4double y = hit->Gety()/CLHEP::m;
	  evtHistos->Fill3DHistogram(0, x, y, sHit, eW);
	}
    }
}

void BDSOutput::FillElossWorldExitHits(const BDSVolumeExitHitsCollection* hits)
{
  G4int nHits = hits->entries();
  for (G4int i = 0; i < nHits; i++)
    {
      BDSVolumeExitHit* hit = (*hits)[i];
      energyWorldExit += hit->TotalEnergyWeighted()/CLHEP::GeV;
      eLossWorldExit->Fill(hit);
    }
}

void BDSOutput::FillPrimaryHit(const BDSTrajectoryPoint* phit)
{
  pFirstHit->Fill(phit);
  const G4double preStepSPosition = phit->GetPreS() / CLHEP::m;
  runHistos->Fill1DHistogram(0, preStepSPosition);
  evtHistos->Fill1DHistogram(0, preStepSPosition);
  runHistos->Fill1DHistogram(3, preStepSPosition);
  evtHistos->Fill1DHistogram(3, preStepSPosition);
}

void BDSOutput::FillPrimaryLoss(const BDSTrajectoryPoint* ploss)
{
  pLastHit->Fill(ploss);
  const G4double postStepSPosition = ploss->GetPostS() / CLHEP::m;
  runHistos->Fill1DHistogram(1, postStepSPosition);
  evtHistos->Fill1DHistogram(1, postStepSPosition);
  runHistos->Fill1DHistogram(4, postStepSPosition);
  evtHistos->Fill1DHistogram(4, postStepSPosition);
}

void BDSOutput::FillTrajectories(const std::map<BDSTrajectory*, bool>& trajectories)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " ntrajectory=" << trajectories.size() << G4endl;
#endif
  traj->Fill(trajectories);
}

void BDSOutput::FillRunInfo(const BDSEventInfo* info)
{
  if (info)
    {*runInfo = BDSOutputROOTEventRunInfo(info->GetInfo());}
}
