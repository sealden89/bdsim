#ifndef BDSMAGNETGEOMETRYTYPE_H
#define BDSMAGNETGEOMETRYTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types

/**
 * @brief Geometry type definition for magnets - used for comparison
 * in factory methods. 
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

struct magnetgeometrytypes_def {
  enum type { none, cylindrical, polescircular, polessquare, polesfacet,
	      polesfacetcrop, lhcleft, lhcright };

};

typedef BDSTypeSafeEnum<magnetgeometrytypes_def, G4int> BDSMagnetGeometryType;

namespace BDS {
  /// function to determine the enum type of the magnet geometry (case-insensitive)
  BDSMagnetGeometryType DetermineMagnetGeometryType(G4String geometryType);
}

#endif
