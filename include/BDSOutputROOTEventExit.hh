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
#ifndef BDSOUTPUTROOTEVENTEXIT_H
#define BDSOUTPUTROOTEVENTEXIT_H

#ifndef __ROOTBUILD__
class BDSVolumeExitHit;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for volume exit hits per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventExit: public TObject
{
public:
  int                n = 0;   ///< Number of entries
  std::vector<float> totalEnergy;  ///< Energy deposited in step
  std::vector<float> postStepKineticEnergy; ///< Kinetic energy in GeV at post step point.

  //// @{ Global coordinates
  std::vector<float> X;
  std::vector<float> Y;
  std::vector<float> Z;
  /// @}
  std::vector<float> T; ///< Global time (time since beginning of event).

  std::vector<int>   partID;  ///< ParticleID that create the deposit
  std::vector<int>   trackID; ///< TrackID that created the deposit
  std::vector<int>   parentID;///< ParentID that created the deposit
  std::vector<float> weight;  ///< Weight
  std::vector<int>   turn;    ///< Turn number
  
  BDSOutputROOTEventExit();
  virtual ~BDSOutputROOTEventExit();
#ifndef __ROOTBUILD__
  void Fill(const BDSVolumeExitHit* hit);
#endif
  virtual void Flush();

  ClassDef(BDSOutputROOTEventExit,1);
};


#endif
