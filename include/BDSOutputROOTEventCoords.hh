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
  void Flush();

  double x;
  double y;
  double z;
  double xp;
  double yp;
  double zp;
  double T;

#ifndef __ROOTBUILD__
  void Fill(const BDSParticleCoords& coords);
#endif

  ClassDef(BDSOutputROOTEventCoords,1);
};

#endif
