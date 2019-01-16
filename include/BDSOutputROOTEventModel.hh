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
#ifndef BDSOUTPUTROOTEVENTMODEL_H
#define BDSOUTPUTROOTEVENTMODEL_H

#include <vector>
#include <string>

#include "TROOT.h"
#include "TObject.h"
#include "TVector3.h"
#include "TRotation.h"

/**
 * @brief Information stored per model representing accelerator.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventModel: public TObject
{
public:
  std::vector<std::string> samplerNamesUnique;

  std::vector<std::string> componentName;
  std::vector<std::string> placementName;
  std::vector<std::string> componentType;
  std::vector<float>       length;
  std::vector<TVector3>    staPos;
  std::vector<TVector3>    midPos;
  std::vector<TVector3>    endPos;
  std::vector<TRotation>   staRot;
  std::vector<TRotation>   midRot;
  std::vector<TRotation>   endRot;
  std::vector<TVector3>    staRefPos;
  std::vector<TVector3>    midRefPos;
  std::vector<TVector3>    endRefPos;
  std::vector<TRotation>   staRefRot;
  std::vector<TRotation>   midRefRot;
  std::vector<TRotation>   endRefRot;
  std::vector<float>       tilt;
  std::vector<float>       offsetX;
  std::vector<float>       offsetY;
  std::vector<float>       staS;
  std::vector<float>       midS;
  std::vector<float>       endS;
  std::vector<std::string> beamPipeType;
  std::vector<double>      beamPipeAper1;
  std::vector<double>      beamPipeAper2;
  std::vector<double>      beamPipeAper3;
  std::vector<double>      beamPipeAper4;
  std::vector<std::string> material;       ///< Material associated with element if any.
  std::vector<float>       k1;
  std::vector<float>       k2;
  std::vector<float>       k3;
  std::vector<float>       k4;
  std::vector<float>       k5;
  std::vector<float>       k6;
  std::vector<float>       k7;
  std::vector<float>       k8;
  std::vector<float>       k9;
  std::vector<float>       k10;
  std::vector<float>       k11;
  std::vector<float>       k12;
  std::vector<float>       k1s;
  std::vector<float>       k2s;
  std::vector<float>       k3s;
  std::vector<float>       k4s;
  std::vector<float>       k5s;
  std::vector<float>       k6s;
  std::vector<float>       k7s;
  std::vector<float>       k8s;
  std::vector<float>       k9s;
  std::vector<float>       k10s;
  std::vector<float>       k11s;
  std::vector<float>       k12s;
  std::vector<float>       ks;     ///< Solenoid strength.
  std::vector<float>       hkick;  ///< Horizontal fractional momentum kick.
  std::vector<float>       vkick;  ///< Vertical fractional momentum kick.
  std::vector<float>       bField; ///< B field in T.
  std::vector<float>       eField; ///< E field in V/m.
  std::vector<float>       e1;
  std::vector<float>       e2;
  std::vector<float>       hgap;
  std::vector<float>       fint;
  std::vector<float>       fintx;
  std::vector<float>       fintk2;
  std::vector<float>       fintxk2;


  /// Default constructor
  BDSOutputROOTEventModel();
  /// Destructor
  virtual ~BDSOutputROOTEventModel();
  /// Find element index closest to vPoint
  int findNearestElement(TVector3 vPoint);

  void Flush();
  
#ifndef __ROOTBUILD__
  /// Fill root output
  virtual void Fill();
#endif

  ClassDef(BDSOutputROOTEventModel,3);
};

#endif
