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
#ifndef BDSTUNNELBUILDER_H
#define BDSTUNNELBUILDER_H

#include "globals.hh" // geant4 types / globals

#include "BDSBeamline.hh"

/**
 * @brief A class that constructs tunnel segments around a beamline. 
 *
 * It uses a beam line instance - the flat beam line
 * and creates a new beam line of tunnel segments that can then be placed.
 * 
 * @author Laurie Nevay
 */

class BDSTunnelBuilder
{
public:
  BDSTunnelBuilder();
  ~BDSTunnelBuilder();

  /// Main function of class - use tunnel factories and builds sections
  /// of tunnel around the flat beamline (of accelerator components).
  /// Tunnel segments are placed in order in a new beamline instance,
  /// which is returned.
  BDSBeamline* BuildTunnelSections(const BDSBeamline* flatBeamLine);

private:
  /// Decide whether this is a good point to break the tunnel based on
  /// various cumulative parameters.
  G4bool BreakTunnel(BDSBeamline::const_iterator proposedStart,
                     BDSBeamline::const_iterator proposedEnd,
		     const G4double& halfWidth);

  /// Maximum distance tolerable that the beamline can deviate from the previous
  /// tunnel break point before another tunnel break point is made.
  const G4double displacementTolerance;

  /// Maximum number of items before split.
  const G4double maxItems;

  /// Maximum length before split.
  const G4double maxLength;

  /// Maximum angle before split.
  const G4double maxAngle;

  /// Minimum length to angle ratio to allow a split.
  const G4double minLength;
};


#endif
