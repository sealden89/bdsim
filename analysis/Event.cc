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
#include "Event.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventAperture.hh"
#include "BDSOutputROOTEventCollimator.hh"
#include "BDSOutputROOTEventCoords.hh"
#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventLossWorld.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventSampler.hh"

#include <vector>

#include "TChain.h"

ClassImp(Event)

Event::Event():
  debug(false),
  processSamplers(false),
  dataVersion(0),
  usePrimaries(false)
{
  CommonCtor();
}

Event::Event(bool debugIn,
	     bool processSamplersIn,
	     int  dataVersionIn):
  debug(debugIn),
  processSamplers(processSamplersIn),
  dataVersion(dataVersionIn)
{
  CommonCtor();
}

Event::~Event()
{
  delete Primary;
  delete PrimaryGlobal;
  delete Eloss;
  delete ElossVacuum;
  delete ElossTunnel;
  delete ElossWorld;
  delete ElossWorldExit;
  delete PrimaryFirstHit;
  delete PrimaryLastHit;
  delete TunnelHit;
  delete Trajectory;
  delete Histos;
  delete Summary;
  delete Info;
  delete Aperture;
  for (auto s : Samplers)
    {delete s;}
  for (auto c : collimators)
    {delete c;}
}

void Event::CommonCtor()
{
#ifdef __ROOTDOUBLE__
  Primary         = new BDSOutputROOTEventSampler<double>();
#else
  Primary         = new BDSOutputROOTEventSampler<float>();
#endif
  PrimaryGlobal      = new BDSOutputROOTEventCoords();
  Eloss              = new BDSOutputROOTEventLoss();
  ElossVacuum        = new BDSOutputROOTEventLoss();
  ElossTunnel        = new BDSOutputROOTEventLoss();
  ElossWorld         = new BDSOutputROOTEventLossWorld();
  ElossWorldContents = new BDSOutputROOTEventLossWorld();
  ElossWorldExit     = new BDSOutputROOTEventLossWorld();
  PrimaryFirstHit    = new BDSOutputROOTEventLoss();
  PrimaryLastHit     = new BDSOutputROOTEventLoss();
  TunnelHit          = new BDSOutputROOTEventLoss();
  Trajectory         = new BDSOutputROOTEventTrajectory();
  Histos             = new BDSOutputROOTEventHistograms();
  Summary            = new BDSOutputROOTEventInfo();
  Info               = new BDSOutputROOTEventInfo();
  Aperture           = new BDSOutputROOTEventAperture();
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(const std::string& name)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(const std::string& name)
#endif
{
  auto found = samplerMap.find(name);
  if (found != samplerMap.end())
    {return found->second;}
  else
    {return nullptr;}
}

#ifdef __ROOTDOUBLE__
BDSOutputROOTEventSampler<double>* Event::GetSampler(int index)
#else
BDSOutputROOTEventSampler<float>* Event::GetSampler(int index)
#endif
{
  if (index >= (int) Samplers.size())
    {return nullptr;}
  else
    {return Samplers[index];}
}

BDSOutputROOTEventCollimator* Event::GetCollimator(const std::string& name)
{
  // help the user out with some variations on the naming that can be created
  // due to our storage format
  std::vector<std::string> variations = {name,
                                         "COLL_" + name,
                                         "COLL_" + name + "_0"};
  for (const auto& var : variations)
    {
      std::cout << (var == collimatorNames[0]) << std::endl;
      auto found = collimatorMap.find(var);
      if (found != collimatorMap.end())
        {return found->second;}
    }
  return nullptr; // wasn't found
}

BDSOutputROOTEventCollimator* Event::GetCollimator(int index)
{
  if (index >= (int) collimators.size())
    {return nullptr;}
  else
    {return collimators[index];}
}

void Event::SetBranchAddress(TTree* t,
			     const RBDS::VectorString* samplerNamesIn,
			     bool                      allBranchesOn,
			     const RBDS::VectorString* branchesToTurnOn,
			     const RBDS::VectorString* collimatorNamesIn)
{
  if(debug)
    {std::cout << "Event::SetBranchAddress" << std::endl;}

  // turn off all branches except standard output branches.
  t->SetBranchStatus("*", 0);

  // turn on only what we need to speed up analysis as with more things
  // on, more data is loaded from the file for each GetEntry().
  // these objects are small - always load
  // the primary is optionally stored (e.g. not stored for tracking comparison files)
  if (((*t).GetListOfBranches()->FindObject("Primary.")) != nullptr)
    {
      usePrimaries = true;
      t->SetBranchStatus("Primary*",  1);
      t->SetBranchAddress("Primary.", &Primary);
    }

  // turn on summary, primary first and last hit as they're not big -> low overhead
  if (dataVersion < 4)
    {// used to be called info but this clashes with root functions in TObject
      t->SetBranchStatus("Info*", 1);
      t->SetBranchAddress("Info.", &Info);
    }
  else
    {
      t->SetBranchStatus("Summary*", 1);
      t->SetBranchAddress("Summary.", &Summary);
    }
  
  t->SetBranchStatus("PrimaryFirstHit*", 1);
  t->SetBranchAddress("PrimaryFirstHit.", &PrimaryFirstHit);

  t->SetBranchStatus("PrimaryLastHit*", 1);
  t->SetBranchAddress("PrimaryLastHit.", &PrimaryLastHit);
  
  if (allBranchesOn)
    {
      t->SetBranchStatus("*", 1);
      t->SetBranchAddress("Eloss.", &Eloss);
      t->SetBranchAddress("Histos.", &Histos);
      if (((*t).GetListOfBranches()->FindObject("TunnelHit.")) != nullptr)
	{t->SetBranchAddress("TunnelHit.", &TunnelHit);}
      else
	{t->SetBranchAddress("ElossTunnel.", &ElossTunnel);}
      t->SetBranchAddress("Trajectory.", &Trajectory);

      if (dataVersion > 3)
	{
	  // PrimaryGlobal is optional and tied to 
	  if (((*t).GetListOfBranches()->FindObject("PrimaryGlobal.")) != nullptr)
	    {t->SetBranchAddress("PrimaryGlobal.",  &PrimaryGlobal);}
	  t->SetBranchAddress("ElossVacuum.",    &ElossVacuum);
	  t->SetBranchAddress("ElossWorld.",     &ElossWorld);
	  if (((*t).GetListOfBranches()->FindObject("ElossWorldContents.")) != nullptr)
	    {t->SetBranchAddress("ElossWorldContents", &ElossWorldContents);}
	  t->SetBranchAddress("ElossWorldExit.", &ElossWorldExit);
	  SetBranchAddressCollimators(t, collimatorNamesIn);
	}
    }
  else if (branchesToTurnOn)
    {
      for (const auto& name : *branchesToTurnOn)
	{
	  std::string nameStar = name + "*";
	  if (debug)
	    {std::cout << "Event::SetBranchAddress> Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);

	  // we can't automatically do this as SetBranchAddress must use the pointer
	  // of the object type and not the base class (say TObject) so there's no
	  // way to easily map these -> ifs
	  if (name == "PrimaryGlobal")
	    {t->SetBranchAddress("PrimaryGlobal.", &PrimaryGlobal);}
	  else if (name == "Eloss")
	    {t->SetBranchAddress("Eloss.", &Eloss);}
	  else if (name == "ElossVacuum")
	    {t->SetBranchAddress("ElossVacuum.", &ElossVacuum);}
	  else if (name == "ElossTunnel")
	    {t->SetBranchAddress("ElossTunnel.", &ElossTunnel);}
	  else if (name == "ElossWorld")
	    {t->SetBranchAddress("ElossWorld.",  &ElossWorld);}
	  else if (name == "ElossWorldContents")
	    {t->SetBranchAddress("ElossWorldContents.", &ElossWorldContents);}
	  else if (name == "ElossWorldExit")
	    {t->SetBranchAddress("ElossWorldExit.", &ElossWorldExit);}
	  else if (name == "Histos")
	    {t->SetBranchAddress("Histos.", &Histos);}
	  else if (name == "TunnelHit")
	    {t->SetBranchAddress("TunnelHit.", &TunnelHit);}
	  else if (name == "Trajectory")
	    {t->SetBranchAddress("Trajectory.", &Trajectory);}
	  else if (name.substr(0,4) == "COLL")
	    {SetBranchAddressCollimatorSingle(t, name);}
	}
    }

  if (debug)
    {
      std::cout << "Event::SetBranchAddress> Primary.            " << Primary            << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryGlobal.      " << PrimaryGlobal      << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.              " << Eloss              << std::endl;
      std::cout << "Event::SetBranchAddress> ElossTunnel.        " << ElossTunnel        << std::endl;
      std::cout << "Event::SetBranchAddress> ElossVacuum.        " << ElossVacuum        << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorld.         " << ElossWorld         << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorldContents. " << ElossWorldContents << std::endl;
      std::cout << "Event::SetBranchAddress> ElossWorldExit.     " << ElossWorldExit     << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit.    " << PrimaryFirstHit    << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.     " << PrimaryLastHit     << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.          " << TunnelHit          << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.         " << Trajectory         << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.             " << Histos             << std::endl;
      std::cout << "Event::SetBranchAddress> Info.               " << Info               << std::endl;
    }

  if (processSamplers && samplerNamesIn)
    {
      unsigned int nrSamplers = samplerNamesIn->size();
      Samplers.resize(nrSamplers); // reserve and nominally instantiate instances.
      for (unsigned int i=0; i < nrSamplers; ++i)
	{
	  const auto sampName = (*samplerNamesIn)[i];
#ifdef __ROOTDOUBLE__
	  Samplers[i] = new BDSOutputROOTEventSampler<double>(sampName);
#else
	  Samplers[i] = new BDSOutputROOTEventSampler<float>(sampName);
#endif
	  samplerNames.push_back(sampName);  // cache the name in a vector
	  samplerMap[sampName] = Samplers[i];// cache the sampler in a map
	    
	  t->SetBranchAddress(sampName.c_str(), &Samplers[i]);
	  t->SetBranchStatus((sampName+"*").c_str(), 1);
	  if(debug)
	    {std::cout << "Event::SetBranchAddress> " << (*samplerNamesIn)[i] << " " << Samplers[i] << std::endl;}
	}
    }
}

void Event::RegisterCollimator(std::string collimatorName)
{
  BDSOutputROOTEventCollimator* collimator = new BDSOutputROOTEventCollimator();
  collimatorNames.push_back(collimatorName);
  collimators.push_back(collimator);
  collimatorMap[collimatorName] = collimator;
}

void Event::RegisterSampler(std::string samplerName)
{
  BDSOutputROOTEventSampler<float>* sampler = new BDSOutputROOTEventSampler<float>();
  samplerNames.push_back(samplerName);
  Samplers.push_back(sampler);
  samplerMap[samplerName] = sampler;
}

void Event::SetBranchAddressCollimators(TTree* t,
					const RBDS::VectorString* collimatorNamesIn)
{
  if (collimatorNamesIn)
    {
      for (const auto& name : *collimatorNamesIn)
	{
	  collimators.resize((unsigned int)collimatorNamesIn->size());
	  SetBranchAddressCollimatorSingle(t, name);
	}
    }
}

void Event::SetBranchAddressCollimatorSingle(TTree* t,
					     const std::string& name)
{
  BDSOutputROOTEventCollimator* col = new BDSOutputROOTEventCollimator();
  collimators.push_back(col);
  collimatorNames.push_back(name);
  collimatorMap[name] = col;

  t->SetBranchAddress((name + ".").c_str(), &collimators.back());
  if (debug)
    {std::cout << "Event::SetBranchAddress> " << name << " " << col << std::endl;}
}

void Event::Fill(Event* other)
{
  Primary->Fill(other->Primary);
  PrimaryGlobal->Fill(other->PrimaryGlobal);
  Eloss->Fill(other->Eloss);
  ElossVacuum->Fill(other->ElossVacuum);
  ElossTunnel->Fill(other->ElossTunnel);
  ElossWorld->Fill(other->ElossWorld);
  ElossWorldContents->Fill(other->ElossWorldContents);
  ElossWorldExit->Fill(other->ElossWorldExit);
  PrimaryFirstHit->Fill(other->PrimaryFirstHit);
  PrimaryLastHit->Fill(other->PrimaryLastHit);
  TunnelHit->Fill(other->TunnelHit);
  Trajectory->Fill(other->Trajectory);
  Histos->FillSimple(other->Histos);
  Summary->Fill(other->Summary);
  Info->Fill(other->Info);
  Aperture->Fill(other->Aperture);

  for (unsigned long i = 0; i < Samplers.size(); i++)
    {Samplers[i]->Fill(other->Samplers[i]);}

  for (unsigned long i = 0; i < collimators.size(); i++)
    {collimators[i]->Fill(other->collimators[i]);}
}

void Event::Flush()
{
  Primary->Flush();
  PrimaryGlobal->Flush();
  Eloss->Flush();
  ElossVacuum->Flush();
  ElossTunnel->Flush();
  ElossWorld->Flush();
  ElossWorldContents->Flush();
  ElossWorldExit->Flush();
  PrimaryFirstHit->Flush();
  PrimaryLastHit->Flush();
  TunnelHit->Flush();
  Trajectory->Flush();
  Histos->Flush();
  Summary->Flush();
  Info->Flush();
  FlushCollimators();
  FlushSamplers();
}

void Event::FlushSamplers()
{
  for (auto s : Samplers)
    {s->Flush();}
}

void Event::FlushCollimators()
{
  for (auto c : collimators)
    {c->Flush();}

}
