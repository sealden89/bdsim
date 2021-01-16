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
#ifndef BDSCOLOURWHEEL_H
#define BDSCOLOURWHEEL_H

#include "G4Colour.hh"

#include <list>
#include <iterator>

/**
 * @brief Three colours that are supplied sequentially.
 *
 * @author Lawrence Deacon.
 */

class BDSColourWheel
{
public:
  BDSColourWheel();
  ~BDSColourWheel();

  /// Return current colour.
  G4Colour Colour();

  /// Iterate colour choice from RGB.
  void Spin();

  /// Start sequence again.
  void Reset();
  
private:
  std::list<G4Colour> palette;
  std::list<G4Colour>::const_iterator colour_it;
};

#endif
