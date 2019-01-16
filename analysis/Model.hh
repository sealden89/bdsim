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
#ifndef MODEL_H
#define MODEL_H

#include "TROOT.h"

#include "RebdsimTypes.hh"

class BDSOutputROOTEventModel;
class TTree;

/** 
 * @brief Model loader.
 *
 * @author Stuart Walker.
 */

class Model
{
public:
  Model();
  Model(bool debugIn);
  virtual ~Model();

  /// Access all the unique sampler names from the model.
  std::vector<std::string> SamplerNames() const;

  /// Set the branch addresses to address the contents of the file.
  void SetBranchAddress(TTree* t,
			bool                      allBranchesOn    = true,
			const RBDS::VectorString* branchesToTurnOn = nullptr);

  /// @{ Member that ROOT can map file data to locally.
  BDSOutputROOTEventModel* model;
  /// @}

private:
  bool debug;
  
  ClassDef(Model,1);
};

#endif
