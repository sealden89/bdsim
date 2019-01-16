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
#ifndef BDSARRAY4DCOORDS_H
#define BDSARRAY4DCOORDS_H

#include "BDSArray4D.hh"
#include "BDSFourVector.hh"

#include "globals.hh"

#include <ostream>

/**
 * @brief Overlay of 4D array that provides uniform only spatial coordinate mapping.
 * 
 * This class overlays (through inheritance) a BDSArray4D instance and provides
 * a mapping from array index coordinates to real spatial coordinates that are real
 * numbers.
 * 
 * Various interfaces are provided to find the nearest entry in an array or convert
 * to the coordinate system. This may seem a little more complicated than is perhaps
 * required, but the flexibility is required for interpolators that will need to 
 * query the array in both array and spatial coordinates.
 * 
 * @author Laurie Nevay
 */

class BDSArray4DCoords: public BDSArray4D
{
public:
  /// Constructor similar to BDSArray4D but with spatial limits in each dimension.
  /// The distance between the UNIFORMLY spaced data in spatial coordinates is
  /// calculated using the extents and the number of entries.
  BDSArray4DCoords(G4int nXIn, G4int nYIn, G4int nZIn, G4int nTIn,
		   G4double xMinIn, G4double xMaxIn,
		   G4double yMinIn, G4double yMaxIn,
		   G4double zMinIn, G4double zMaxIn,
		   G4double tMinIn, G4double tMaxIn);

  virtual ~BDSArray4DCoords(){;} 

  /// @{ The distance in spatial coordinates between any two points in the array.
  inline G4double XStep() const {return xStep;}
  inline G4double YStep() const {return yStep;}
  inline G4double ZStep() const {return zStep;}
  inline G4double TStep() const {return tStep;}
  /// @}

  /// Whether the spatial coordinates lie outside the range of the array or not.
  virtual G4bool OutsideCoords(const G4double x,
			       const G4double y,
			       const G4double z,
			       const G4double t) const;

  /// Whether the spatial coordinates lie outside the range of the array or not and
  /// warn and exit if so.  Sses OutsideCoords but warns and exits if the coordinates
  /// are outside the array.
  virtual void OutsideCoordsWarn(const G4double x,
				 const G4double y,
				 const G4double z,
				 const G4double t) const;

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4double ArrayCoordsFromX(const G4double x) const {return (x - xMin) / xStep;}
  virtual G4double ArrayCoordsFromY(const G4double y) const {return (y - yMin) / yStep;}
  virtual G4double ArrayCoordsFromZ(const G4double z) const {return (z - zMin) / zStep;}
  virtual G4double ArrayCoordsFromT(const G4double t) const {return (t - tMin) / tStep;}
  /// @}

  /// Convenience function to easily get array coords in all dimensions at once.
  inline BDSFourVector<G4double> ArrayCoordsFromXYZT(const G4double x,
						     const G4double y,
						     const G4double z,
						     const G4double t) const
  {
    return BDSFourVector<G4double>(ArrayCoordsFromX(x),
				   ArrayCoordsFromY(y),
				   ArrayCoordsFromZ(z),
				   ArrayCoordsFromT(t));
  }

  /// @{ Return sptial value from a continuous array coordinate in one dimension.
  inline G4double XFromArrayCoords(const G4double xCoord) const {return xMin + xCoord*xStep;}
  inline G4double YFromArrayCoords(const G4double yCoord) const {return yMin + yCoord*yStep;}
  inline G4double ZFromArrayCoords(const G4double zCoord) const {return zMin + zCoord*zStep;}
  inline G4double TFromArrayCoords(const G4double tCoord) const {return tMin + tCoord*tStep;}
  /// @}

  /// Convenience function to get the spatial coordinates from continuous array coordinates
  /// for all dimensions at once.
  inline BDSFourVector<G4double> XYZTFromArrayCoords(const G4double x,
						     const G4double y,
						     const G4double z,
						     const G4double t) const
  {
    return BDSFourVector<G4double>(XFromArrayCoords(x),
				   YFromArrayCoords(y),
				   ZFromArrayCoords(z),
				   TFromArrayCoords(t));
  }

  /// @{ Not much point in being both virtual and inline (in our use case) but has to be virtual.
  virtual G4int NearestX(const G4double x) const {return (G4int)round((x - xMin) / xStep);}
  virtual G4int NearestY(const G4double y) const {return (G4int)round((y - yMin) / yStep);}
  virtual G4int NearestZ(const G4double z) const {return (G4int)round((z - zMin) / zStep);}
  virtual G4int NearestT(const G4double t) const {return (G4int)round((t - tMin) / tStep);}
  /// @}

  /// Return the index of the nearest field value in space.
  inline BDSFourVector<G4int> NearestXYZT(const G4double x,
					  const G4double y,
					  const G4double z,
					  const G4double t) const
  {
    return BDSFourVector<G4int>(NearestX(x),
				NearestY(y),
				NearestZ(z),
				NearestT(t));
  }

  /// Return the index of the nearest field value in space via four-vector.
  inline BDSFourVector<G4int> NearestXYZT(const BDSFourVector<G4double>& pos) const
  {
    return BDSFourVector<G4int>(NearestX(pos[0]),
				NearestY(pos[1]),
				NearestZ(pos[2]),
				NearestT(pos[3]));
  }

  /// Overloaded output function from BDSArray4D. Prints spatial map parameters, then
  /// underlying array.
  virtual std::ostream& Print(std::ostream& out) const;

  /// Output stream.
  friend std::ostream& operator<< (std::ostream& out, BDSArray4DCoords const &a);

protected:
  /// @{ Dimension parameter - protected for derived class access.
  G4double xMin;
  G4double xMax;
  G4double yMin;
  G4double yMax;
  G4double zMin;
  G4double zMax;
  G4double tMin;
  G4double tMax;

  G4double xStep;
  G4double yStep;
  G4double zStep;
  G4double tStep;
  /// @}
  
private:
  /// No default constructor as the array is not adjustable after construction and
  /// therefore the size must be known at construction time.
  BDSArray4DCoords() = delete;

  void CheckStep(G4double step, const G4String name) const;
};

#endif
