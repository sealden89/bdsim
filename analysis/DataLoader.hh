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
#ifndef DATALOADER_H
#define DATALOADER_H

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"

#include <map>
#include <string>
#include <vector>

class Beam;
class Event;
class Geant4Data;
class Header;
class Options;
class Model;
class Run;
class TChain;

/**
 * @brief Loader for a ROOT file using classes used to generate the file.
 *
 * @author Stewart Boogert
 */

class DataLoader
{
public:
  DataLoader(std::string fileName,
	     bool        debugIn           = false,
	     bool        processSamplersIn = true,
	     bool        allBranchesOn     = true,
	     const RBDS::BranchMap* branchesToTurnOn = nullptr,
	     bool        backwardsCompatibleIn = true);
  virtual ~DataLoader();

  /// Create an instance of each class in the file to be overlaid by loading
  /// the ROOT file.
  void CommonCtor(std::string fileName);

  /// Build up the input file list.
  void BuildInputFileList(std::string inputPath);

  /// Open the first file in the file list and map the trees in it.
  void BuildTreeNameList();

  /// Inspect the first file (leaving it open...) and build a list of samplers
  /// by checking whether they have 'Sampler' in the name.
  void BuildEventBranchNameList();

  /// Create a tree for each sampler and add all the files to it.
  void ChainTrees();

  /// Map each chain to the member instance of each storage class in this class.
  void SetBranchAddress(bool allOn = true,
                        const RBDS::BranchMap* bToTurnOn = nullptr);

  inline int DataVersion() const {return dataVersion;}

  /// @{ Accessor
  std::vector<std::string>   GetTreeNames()      {return treeNames;};
  std::vector<std::string>   GetBranchNames()    {return branchNames;}
  std::vector<std::string>   GetSamplerNames()   {return samplerNames;}
  std::vector<std::string>   GetCollimatorNames(){return collimatorNames;}
  Header*                    GetHeader()         {return hea;}
  Geant4Data*                GetGeant4Data()     {return g4d;}
  Beam*                      GetBeam()           {return bea;}
  Options*                   GetOptions()        {return opt;}
  Model*                     GetModel()          {return mod;}
  Event*                     GetEvent()          {return evt;}
  Run*                       GetRun()            {return run;}
  TChain*                    GetHeaderTree()     {return heaChain;}
  TChain*                    GetGeant4DataTree() {return g4dChain;}
  TChain*                    GetBeamTree()       {return beaChain;}
  TChain*                    GetOptionsTree()    {return optChain;}
  TChain*                    GetModelTree()      {return modChain;}
  TChain*                    GetEventTree()      {return evtChain;}
  TChain*                    GetRunTree()        {return runChain;}
  /// @}

private:
  DataLoader();
  
  bool debug;
  bool processSamplers;
  bool allBranchesOn;
  const RBDS::BranchMap* branchesToTurnOn;
  bool backwardsCompatible;

  Header*     hea;
  Geant4Data* g4d;
  Beam*       bea;
  Options*    opt;
  Model*      mod;
  Event*      evt;
  Run*        run;

  std::vector<std::string> fileNames;
  std::vector<std::string> treeNames;
  std::vector<std::string> branchNames;  // non-sampler branch names
  std::vector<std::string> samplerNames; // sampler branch names
  std::vector<std::string> collimatorNames;

  TChain* heaChain;
  TChain* g4dChain;
  TChain* beaChain;
  TChain* optChain;
  TChain* modChain;
  TChain* evtChain;
  TChain* runChain;

  int dataVersion; ///< Integer version of data loaded.

  ClassDef(DataLoader, 2);
};

#endif
