/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSLINE_H
#define BDSLINE_H 

#include "BDSAcceleratorComponent.hh"

#include <iterator>
#include <vector>

/**
 * @brief A class that hold multiple accelerator components.
 * 
 * Not physically used in Geant4, only as placeholder.
 * Therefore virtual methods are not specified, but could be in the future.
 *
 * Memory of components is not owned
 */

class BDSLine: public BDSAcceleratorComponent
{
private:
  /// Private typedef first so it can be used throughout. Typedef for member
  /// vector.
  typedef std::vector<BDSAcceleratorComponent*> BDSLineVector;

  /// Member vector to store components
  BDSLineVector line;
  
public:
  explicit BDSLine(G4String name);
  virtual ~BDSLine(){;}

  /// Add a component to the line.
  void AddComponent(BDSAcceleratorComponent* component);

  /// @{ Iterator mechanics
  typedef BDSLineVector::iterator       iterator;
  typedef BDSLineVector::const_iterator const_iterator;
  iterator       begin()       {return line.begin();}
  iterator       end()         {return line.end();}
  const_iterator begin() const {return line.begin();}
  const_iterator end()   const {return line.end();}
  G4bool         empty() const {return line.empty();}
  /// @}

  /// Size of line
  size_t size() const {return line.size();}

  /// Accessor for part - exposes functionality of the vector for iteration by index.
  BDSAcceleratorComponent* const& operator[](G4int index) const {return line.at(index);}
  
  /// Override the BDSAccelerator::Initialise() function to loop over the
  /// line and call that function belonging to each member
  virtual void Initialise();

  /// @{ Copy the bias list to each component.
  virtual void SetBiasVacuumList(std::list<std::string> biasVacuumList);
  virtual void SetBiasMaterialList(std::list<std::string> biasMaterialList);
  /// @}

  /// Set the region name for each component.
  virtual void SetRegion(G4String region);

private:
  /// define pure virtual method
  virtual void BuildContainerLogicalVolume(){};

  /// Private default constructor to force the use of the supplied one.
  BDSLine() = delete;
  
  /// @{ Assignment and copy constructor not implemented nor used.
  BDSLine& operator=(const BDSLine&) = delete;
  BDSLine(BDSLine&) = delete;
  /// @}
};

#endif
