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
#include <algorithm>
#include <bitset>
#include <iostream>
#include <iomanip>

#include "BDSOutputROOTEventTrajectory.hh"

#ifndef __ROOTBUILD__
#include "G4VPhysicalVolume.hh"

#include "BDSHitEnergyDeposition.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSTrajectory.hh"
#include "BDSTrajectoryOptions.hh"

#include <cmath>
#endif

ClassImp(BDSOutputROOTEventTrajectory)
BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory():
  auxNavigator(nullptr),
  n(0)
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{
#ifndef __ROOTBUILD__
  delete auxNavigator;
#endif
}

#ifndef __ROOTBUILD__
int findPrimaryStepIndex(BDSTrajectory* traj)
{
  if (!traj->GetParent())
    {return -1;}

  if (traj->GetParent()->GetTrackID() == 1)
    {return traj->GetParentStepIndex();}
  else
    {return findPrimaryStepIndex(traj->GetParent());}
}

void BDSOutputROOTEventTrajectory::Fill(const BDSTrajectoriesToStore* trajectories,
                                        int  storeStepPointsN,
                                        bool storeStepPointLast,
                                        const BDS::TrajectoryOptions& storageOptions)
{
  if(!auxNavigator)
    {// navigator for converting coordinates to curvilinear coordinate system
      auxNavigator = new BDSAuxiliaryNavigator();
    }

  G4bool stEK = storageOptions.storeLinks || storageOptions.storeKineticEnergy;
  G4bool stMo = storageOptions.storeMomentumVector;
  G4bool stPr = storageOptions.storeProcesses;
  G4bool stTi = storageOptions.storeTime;
  
  // assign trajectory indices
  int idx = 0;
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj = trajFlag.first;
      if (trajFlag.second) // ie we want to save this trajectory
	{
	  traj->SetTrajIndex(idx);
	  idx++;
	}
      else // we don't want to save it
	{traj->SetTrajIndex(-1);}
    }

  // assign parent (and step) indices
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj   = trajFlag.first;
      BDSTrajectory* parent = traj->GetParent();
      if (trajFlag.second && parent)
	{ // to store and not primary
	  traj->SetParentIndex(parent->GetTrajIndex());

	  // search for parent step index
	  if (parent->GetTrajIndex() != -1)
	    {
	      auto trajStartPos = traj->GetPoint(0)->GetPosition();
	      traj->SetParentStepIndex(-1);
	      for (auto i = 0; i < parent->GetPointEntries(); ++i)
		{
		  if(parent->GetPoint(i)->GetPosition() == trajStartPos)
		    {
		      traj->SetParentStepIndex(i);
		      break;
		    }
		}
	    }
	  else
	    {parent->SetParentStepIndex(-1);}
	}
      else
	{traj->SetParentIndex(-1);}
    }

  n = 0;
  for (auto trajFlag : trajectories->trajectories)
    {
      BDSTrajectory* traj = trajFlag.first;

      // check if the trajectory is to be stored
      if (!trajFlag.second) // ie false, then continue and don't store
	{continue;}

      partID.push_back((int &&) traj->GetPDGEncoding());
      trackID.push_back((unsigned int) std::abs(traj->GetTrackID()));
      parentID.push_back((unsigned int) std::abs(traj->GetParentID()));
      parentIndex.push_back((unsigned int) std::abs(traj->GetParentIndex()));
      parentStepIndex.push_back((unsigned int) std::abs(traj->GetParentStepIndex()));

      // now we convert the geant4 type based BDSTrajectory information into
      // basic C++ and ROOT types for the output
      IndividualTrajectory itj;
      if (storeStepPointsN > 0)
	{// store specific number of step points along the trajectory
	  G4int nSteps = traj->GetPointEntries();
	  G4int nPoints = std::min(nSteps, storeStepPointsN);
	  for (int i = 0; i < nPoints; ++i)
	    {FillIndividualTrajectory(itj, traj, i);}
	  // optionally include the last point if required and not already stored
	  if (storeStepPointLast && (nPoints < nSteps))
	    {FillIndividualTrajectory(itj, traj, nSteps-1);}
	}
      else
	{// store all points as usual
	  for (int i = 0; i < traj->GetPointEntries(); ++i)
	    {FillIndividualTrajectory(itj, traj, i);}
	}
      
      // record the filters that were matched for this trajectory
      filters.push_back(trajectories->filtersMatched.at(traj));
      
      XYZ.push_back(itj.XYZ);
      modelIndicies.push_back(itj.modelIndex);
      
      if (stMo)
	{PXPYPZ.push_back(itj.PXPYPZ);}
      
      S.push_back(itj.S);
      
      if (stPr)
	{
	  preProcessTypes.push_back(itj.preProcessType);
	  preProcessSubTypes.push_back(itj.preProcessSubType);
	  postProcessTypes.push_back(itj.postProcessType);
	  postProcessSubTypes.push_back(itj.postProcessSubType);
	}
      
      preWeights.push_back(itj.preWeight);
      postWeights.push_back(itj.postWeight);
      energyDeposit.push_back(itj.energyDeposit);
      
      if (stTi)
	{T.push_back(itj.T);}
      
      if (stEK)
        {kineticEnergy.push_back(itj.kineticEnergy);}

      if (!itj.xyz.empty())
	{
          xyz.push_back(itj.xyz);
          pxpypz.push_back(itj.pxpypz);
	}

      if (!itj.charge.empty())
	{
          charge.push_back(itj.charge);
          turnsTaken.push_back(itj.turn);
          mass.push_back(itj.mass);
          rigidity.push_back(itj.rigidity);
	}
      
      if (!itj.isIon.empty())
	{
	  isIon.push_back(itj.isIon);
	  ionA.push_back(itj.ionA);
          ionZ.push_back(itj.ionZ);
          nElectrons.push_back(itj.nElectrons);
	}
      
      // recursively search for primary interaction step
      primaryStepIndex.push_back(findPrimaryStepIndex(traj));

      // geant4 trackID to trackIndex in this table
      trackID_trackIndex.insert(std::pair<int,int>(traj->GetTrackID(),n));
      
      n++;
    }
  
#if 0
  // Fill maps for later analysis
  int trackIndex = 0;
  for (auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
    {
      BDSTrajectory* traj = *iT;

      // map of trackID to trackIndex
      trackID_trackIndex.insert(std::pair<int, int>(traj->GetTrackID(),trackIndex));

      std::cout << trajVec.size() << " " << parentID.size() << " " << parentIndex.size() << " "
		<< traj->GetTrackID() << " " << traj->GetParentID() << " " << trackIndex << std::endl;

      // map of trackIndex to trackProcess
      auto processPair = findParentProcess(trackIndex);
      trackIndex_trackProcess.insert(std::pair<int,std::pair<int,int>>(trackIndex,processPair));

      // map of modelIndex to trackProcess
      if(processPair.first != -1)
	{
	  int mi = modelIndicies[processPair.first][processPair.second];
	  trackIndex_modelIndex.insert(std::pair<int,int>(trackIndex, mi));
	  try
	    {modelIndex_trackIndex.at(mi).push_back(trackIndex);}
	  catch(const std::exception&)
	    {
	      modelIndex_trackIndex.insert(std::pair<int,std::vector<int>>(mi,std::vector<int>()));
	      modelIndex_trackIndex.at(mi).push_back(trackIndex);
	    }
	}

      ++trackIndex;
    }
#endif
}

void BDSOutputROOTEventTrajectory::FillIndividualTrajectory(IndividualTrajectory& itj,
							    BDSTrajectory*        traj,
							    int                   i)
{
  BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
  
  // Position
  G4ThreeVector pos = point->GetPosition();
  itj.XYZ.emplace_back(TVector3(pos.getX() / CLHEP::m,
			     pos.getY() / CLHEP::m,
			     pos.getZ() / CLHEP::m));
  
  G4VPhysicalVolume* vol = auxNavigator->LocateGlobalPointAndSetup(pos,nullptr,true,true,true);
  BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(vol);
  if (theInfo)
    {itj.modelIndex.push_back(theInfo->GetBeamlineIndex());}
  else
    {itj.modelIndex.push_back(-1);}
  
  // Process types
  itj.preProcessType.push_back(point->GetPreProcessType());
  itj.preProcessSubType.push_back(point->GetPreProcessSubType());
  itj.postProcessType.push_back(point->GetPostProcessType());
  itj.postProcessSubType.push_back(point->GetPostProcessSubType());
  
  itj.preWeight.push_back(point->GetPreWeight());
  itj.postWeight.push_back(point->GetPostWeight());
  itj.energyDeposit.push_back(point->GetEnergyDeposit() / CLHEP::GeV);
  G4ThreeVector mom = point->GetPreMomentum() / CLHEP::GeV;
  itj.PXPYPZ.emplace_back(TVector3(mom.getX(),
				mom.getY(),
				mom.getZ()));
  itj.S.push_back(point->GetPreS() / CLHEP::m);
  itj.T.push_back(point->GetPreGlobalTime() / CLHEP::ns);
  itj.kineticEnergy.push_back(point->GetKineticEnergy() / CLHEP::GeV);
  
  if (point->extraLocal)
    {
      G4ThreeVector localPos = point->GetPositionLocal() / CLHEP::m;
      G4ThreeVector localMom = point->GetMomentumLocal() / CLHEP::GeV;
      itj.xyz.emplace_back(TVector3(localPos.getX(),
				 localPos.getY(),
				 localPos.getZ()));
      itj.pxpypz.emplace_back(TVector3(localMom.getX(),
				    localMom.getY(),
				    localMom.getZ()));
    }
  
  if (point->extraLink)
    {
      itj.charge.push_back(point->GetCharge() / (G4double)CLHEP::eplus);
      itj.turn.push_back(point->GetTurnsTaken());
      itj.mass.push_back(point->GetMass() / CLHEP::GeV);
      itj.rigidity.push_back(point->GetRigidity() / (CLHEP::tesla*CLHEP::m));
    }
  
  if (point->extraIon)
    {
      itj.isIon.push_back(point->GetIsIon());
      itj.ionA.push_back(point->GetIonA());
      itj.ionZ.push_back(point->GetIonZ());
      itj.nElectrons.push_back(point->GetNElectrons());
    }	  
}
  
void BDSOutputROOTEventTrajectory::Fill(const BDSHitsCollectionEnergyDeposition* phc)
{
  G4cout << phc->GetSize() << G4endl;
}

#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  n = 0;
  filters.clear();
  partID.clear();
  trackID.clear();
  parentID.clear();
  parentIndex.clear();
  parentStepIndex.clear();
  primaryStepIndex.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  preWeights.clear();
  postWeights.clear();
  energyDeposit.clear();
  XYZ.clear();
  S.clear();
  PXPYPZ.clear();
  modelIndicies.clear();
  trackID_trackIndex.clear();
  T.clear();

  xyz.clear();
  pxpypz.clear();
  charge.clear();
  kineticEnergy.clear();
  turnsTaken.clear();
  mass.clear();
  rigidity.clear();
  isIon.clear();
  ionA.clear();
  ionZ.clear();
  nElectrons.clear();

  // trackIndex_trackProcess.clear();
  // trackIndex_modelIndex.clear();
  // modelIndex_trackIndex.clear();
}

void BDSOutputROOTEventTrajectory::Fill(const BDSOutputROOTEventTrajectory* other)
{
  if (!other)
    {return;}

  n = 0;
  filters             = other->filters;
  partID              = other->partID;
  trackID             = other->trackID;
  parentID            = other->parentID;
  parentIndex         = other->parentIndex;
  parentStepIndex     = other->parentStepIndex;
  primaryStepIndex    = other->primaryStepIndex;
  preProcessTypes     = other->preProcessTypes;
  preProcessSubTypes  = other->preProcessSubTypes;
  postProcessTypes    = other->postProcessTypes;
  postProcessSubTypes = other->postProcessSubTypes;
  preWeights          = other->preWeights;
  postWeights         = other->postWeights;
  energyDeposit       = other->energyDeposit;
  XYZ                 = other->XYZ;
  S                   = other->S;
  PXPYPZ              = other->PXPYPZ;
  modelIndicies       = other->modelIndicies;
  trackID_trackIndex  = other->trackID_trackIndex;
  T                   = other->T;

  xyz                 = other->xyz;
  pxpypz              = other->pxpypz;
  charge              = other->charge;
  kineticEnergy       = other->kineticEnergy;
  turnsTaken          = other->turnsTaken;
  rigidity            = other->rigidity;
  isIon               = other->isIon;
  ionA                = other->ionA;
  ionZ                = other->ionZ;
  nElectrons          = other->nElectrons;
}

#if 0
std::pair<int,int> BDSOutputROOTEventTrajectory::findParentProcess(int trackIndex)
{
  std::cout << "BDSOutputROOTEventTrajectory::findParentProcess> "
	    << trackIndex << " " << parentID.size() << " " << parentIndex.size() << std::endl;
  int tid = trackIndex;
  int pid = parentID.at(tid);
  std::cout << pid << std::endl;
  int pin = parentIndex.at(tid);
  std::cout << pin << std::endl;

  if (pin == -1)
    {return std::pair<int,int>(-1,-1);}
  int sin = parentStepIndex.at(tid);
  std::cout << sin << std::endl;

  while (pid > 0)
    {
      if(pin == 0)
	{break;}
      tid = pin;
      pid = parentID.at(tid);
      pin = parentIndex.at(tid);
      sin = parentStepIndex.at(tid);

      std::cout << tid << " " << pid << " " << pin << " " << sin << " " << std::endl;
    }

  return std::pair<int,int>(pin,sin);
}
#endif

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::trackInteractions(int trackid)
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackid) == trackID_trackIndex.end())
    {return std::vector<BDSOutputROOTEventTrajectoryPoint>();}
  
  int ti = trackID_trackIndex.at(trackid);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv; // trajectory point vector - result

  int nstep = XYZ[ti].size();
  for (int i = 0; i < nstep; ++i)
    {
      int ppt = postProcessTypes[ti][i];
      if (ppt != -1 && ppt != 1 && ppt != 10)
	{
	  BDSOutputROOTEventTrajectoryPoint p(partID[ti],
					      trackID[ti],
					      parentID[ti],
					      parentIndex[ti],
					      postProcessTypes[ti][i],
					      postProcessSubTypes[ti][i],
					      postWeights[ti][i],
					      energyDeposit[ti][i],
					      XYZ[ti][i],
					      PXPYPZ[ti][i],
					      modelIndicies[ti][i],
					      T[ti][i],
					      xyz[ti][i],
					      pxpypz[ti][i],
					      charge[ti][i],
					      kineticEnergy[ti][i],
					      turnsTaken[ti][i],
					      rigidity[ti][i],
					      mass[ti][i],
					      isIon[ti][i],
					      ionA[ti][i],
					      ionZ[ti][i],
					      nElectrons[ti][i]);
	  tpv.push_back(p);
	}
    }
  return tpv;
}

BDSOutputROOTEventTrajectoryPoint BDSOutputROOTEventTrajectory::primaryProcessPoint(int trackid)
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackid) == trackID_trackIndex.end())
    {return BDSOutputROOTEventTrajectoryPoint();}
  
  int ti = trackID_trackIndex.at(trackid);  // get track index
  int si = parentStepIndex.at(ti);          // get primary index

  BDSOutputROOTEventTrajectoryPoint p(partID[ti],
				      trackID[ti],
                                      parentID[ti],
				      parentIndex[ti],
                                      postProcessTypes[ti][si],
				      postProcessSubTypes[ti][si],
                                      postWeights[ti][si],
				      energyDeposit[ti][si],
                                      XYZ[ti][si],
				      PXPYPZ[ti][si],
                                      modelIndicies[ti][si],
				      T[ti][si],
				      xyz[ti][si],
				      pxpypz[ti][si],
				      charge[ti][si],
				      kineticEnergy[ti][si],
				      turnsTaken[ti][si],
				      rigidity[ti][si],
				      mass[ti][si],
				      isIon[ti][si],
				      ionA[ti][si],
				      ionZ[ti][si],
				      nElectrons[ti][si]);
  return p;
}

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::processHistory(int trackid)
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackid) == trackID_trackIndex.end())
    {return std::vector<BDSOutputROOTEventTrajectoryPoint>();}
  
  int ti = trackID_trackIndex.at(trackid);

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv;      // trajectory point vector
  while (ti != 0)
    {
      unsigned int pi  = parentIndex.at(ti);
      unsigned int psi = parentStepIndex.at(ti);
      
      BDSOutputROOTEventTrajectoryPoint p(partID[pi],
					  trackID[pi],
					  parentID[pi],
					  parentIndex[pi],
					  postProcessTypes[pi][psi],
					  postProcessSubTypes[pi][psi],
					  postWeights[pi][psi],
					  energyDeposit[pi][psi],
					  XYZ[pi][psi],
					  PXPYPZ[pi][psi],
					  modelIndicies[pi][psi],
					  T[pi][psi],
					  xyz[ti][psi],
					  pxpypz[ti][psi],
					  charge[ti][psi],
					  kineticEnergy[ti][psi],
					  turnsTaken[ti][psi],
					  rigidity[ti][psi],
					  mass[ti][psi],
					  isIon[ti][psi],
					  ionA[ti][psi],
					  ionZ[ti][psi],
					  nElectrons[ti][psi]);
      tpv.push_back(p);
      ti = (int)pi;
    }
  std::reverse(tpv.begin(),tpv.end());
  return tpv;
}

void BDSOutputROOTEventTrajectory::printTrajectoryInfo(int i)
{
  int wdt = 11; // width of columns for print out
  
  if (i+1 > n) // safety
    {std::cout << "Index chosen is greater than maximum index of: " << n-1 << std::endl; return;}
  
  // print out regarding the trajectory generally
  std::cout << "Trajectory index " << std::setw(wdt) << i
            << ", PDG ID "         << std::setw(wdt) << partID[i]
            << ", Track ID "       << std::setw(wdt) << trackID[i]
            << ", Parent ID "      << std::setw(wdt) << parentID[i] << std::endl;
  std::cout << "Created by Track ID " << parentID[i] << ", with index " << parentIndex[i] << ", and at step index " << parentStepIndex[i] << std::endl;
  
  // print out regarding each step of the trajectory
  std::cout << std::setw(wdt) << "step ind"  << " "
            << std::setw(wdt) << "prePrcT"   << " " << std::setw(wdt) << "prePrcST" << " "
            << std::setw(wdt) << "pstPrcT"   << " " << std::setw(wdt) << "pstPrcST" << " "
            << std::setw(wdt) << "X"         << " " << std::setw(wdt) << "Y"        << " "
            << std::setw(wdt) << "Z"         << " " << std::setw(wdt) << "E"        << " "
            << std::setw(wdt) << "p"         << " " << std::setw(wdt) << "p_x"      << " "
            << std::setw(wdt) << "p_y"       << " " << std::setw(wdt) << "p_z"      << " "
            << std::setw(wdt) << "t"         << std::endl;

  for (size_t j=0; j<XYZ[i].size(); ++j)
    {
      std::cout << std::setw(wdt) << j << " "
      << std::setw(wdt) << preProcessTypes[i][j]  << " " << std::setw(wdt) << preProcessSubTypes[i][j]  << " "
		  << std::setw(wdt) << postProcessTypes[i][j] << " " << std::setw(wdt) << postProcessSubTypes[i][j] << " "
		  << std::setw(wdt) << XYZ[i][j].X()          << " " << std::setw(wdt) << XYZ[i][j].Y()             << " "
		  << std::setw(wdt) << XYZ[i][j].Z()          << " " << std::setw(wdt) << energyDeposit[i][j]       << " "
		  << std::setw(wdt) << PXPYPZ[i][j].Mag()     << " " << std::setw(wdt) << PXPYPZ[i][j].X()          << " "
		  << std::setw(wdt) << PXPYPZ[i][j].Y()       << " " << std::setw(wdt) << PXPYPZ[i][j].Z()          << " "
		  << std::setw(wdt) << T[i][j]                << std::endl;
    }
}

bool BDSOutputROOTEventTrajectory::parentIsPrimary(int trackIDIn)
{
  // prevent a bad access
  if (trackID_trackIndex.find(trackIDIn) == trackID_trackIndex.end())
    {return false;}
  
  unsigned int storageIndex = (unsigned int)trackID_trackIndex.at(trackIDIn);
  unsigned int parentStorageIndex = parentIndex[storageIndex];
  return parentID[parentStorageIndex] == 0;
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for (int i=0; i< (int)t.preProcessTypes.size();++i)
    {
      for (int j=0; j< (int)t.preProcessTypes[i].size(); ++j)
	{
	  //if(t.preProcessTypes[i][j] != 1 && t.preProcessTypes[i][j] != 7)
	  //{
	  out << i << " " << j
	      << " " << t.preProcessTypes[i][j]   << " " << t.preProcessSubTypes[i][j]
	      << " " << t.postProcessTypes[i][j]  << " " << t.postProcessSubTypes[i][j]
	      << " " << t.preWeights[i][j]        << " " << t.postWeights[i][j]
	      << " " << t.energyDeposit[i][j]     << " " << t.T[i][j]
	      << std::endl;
	  //}
	}
    }
  return out;
}
