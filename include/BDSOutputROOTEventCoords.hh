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
#ifndef BDSOUTPUTROOTEVENTCOORDS_H
#define BDSOUTPUTROOTEVENTCOORDS_H

#include "TROOT.h"
#include "TObject.h"

#ifndef __ROOTBUILD__
#include "BDSParticleCoords.hh"
class BDSPrimaryVertexInformationV;
#endif

/**
 * @brief A set of coordinates.
 *
 * @author Laurie Nevay
 */

class BDSOutputROOTEventCoords: public TObject
{  
public:
  BDSOutputROOTEventCoords();

  virtual ~BDSOutputROOTEventCoords();
  virtual void Flush();

  /// Fill from another instance.
  void Fill(const BDSOutputROOTEventCoords* other);

  int n;
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  std::vector<double> xp;
  std::vector<double> yp;
  std::vector<double> zp;
  std::vector<double> T;

#ifndef __ROOTBUILD__
  void Fill(const BDSParticleCoords& coords);
  void Fill(const BDSPrimaryVertexInformationV* vertexInfos);
#endif

  ClassDef(BDSOutputROOTEventCoords,2);
};

#endif
