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
#ifndef BDSACCELERATORCOMPONENTREGISTRY_H
#define BDSACCELERATORCOMPONENTREGISTRY_H

#include "globals.hh" // geant4 globals / types

#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

class BDSAcceleratorComponent;

/**
 * @brief A registry of constructed BDSAcceleratorComponent instances that
 * can be searched. 
 * 
 * Uses an std::map rather than unordered map as although 
 * slower to access (less often and only at construction), a map is faster 
 * for iterating than an unordered map, which will be required to apply 
 * wrapper physics processes - relatively common.
 * 
 * @author Laurie Nevay
 */

class BDSAcceleratorComponentRegistry
{
private:
  // Typedefs up first so we can declare public iterators.
  /// Use a typedef for this specific map implementation so we can easily
  /// define iterators and internal member variables without risking getting
  /// the exact map declaration wrong. 
  typedef std::map<G4String, BDSAcceleratorComponent*> RegistryMap;

  /// Registry is a map - note 'register' is a protected keyword.
  RegistryMap registry;
  
public:
  /// Singleton accessor
  static BDSAcceleratorComponentRegistry* Instance();

  /// Destructor resets instance to 0 so that the class can be reinstantiated
  /// if necessary.
  ~BDSAcceleratorComponentRegistry();

  /// Register a BDSAcceleratorComponent. Once in the registry, can be reused
  /// through GetComponent function. Optional flag isModified indicates that the
  /// component was modified beyond its original parser definition (ie likely to
  /// match the face of another component) and should not be reused even if it
  /// appears later in the lattice. In this case, it is stored here, purely for
  /// memory management.
  void RegisterComponent(BDSAcceleratorComponent* component,
			 bool                     isModified = false);

  /// Check whether an accelerator component is already registered.
  G4bool IsRegistered(BDSAcceleratorComponent* component);

  /// Check whether an accelerator component is already registered by name.
  G4bool IsRegistered(G4String componentName);

  /// Check if a unique component is registered in the allocatedComponents.
  G4bool IsRegisteredAllocated(const BDSAcceleratorComponent* componentName) const;

  /// Access an already constructed component - will return null if no such component found.
  /// This is safe as this registry is primarily used by BDSComponentFactory which can return
  /// nullptr to BDSDetectorConstruction safely if an invalid component is requested.
  BDSAcceleratorComponent* GetComponent(G4String name);

  /// Register a curvilinear component - purely to keep track of and delete at the
  /// end of the program.
  void RegisterCurvilinearComponent(BDSAcceleratorComponent* component);

  /// @{ Iterator mechanics
  typedef RegistryMap::iterator       iterator;
  typedef RegistryMap::const_iterator const_iterator;
  iterator       begin()       {return registry.begin();}
  iterator       end()         {return registry.end();}
  const_iterator begin() const {return registry.begin();}
  const_iterator end()   const {return registry.end();}
  G4bool         empty() const {return registry.empty();}
  /// @}

  /// Size of registry
  size_t size() const {return registry.size();}
  
  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSAcceleratorComponentRegistry const &r);

  /// Print out the number of each type of component registered.
  void PrintNumberOfEachType() const;

private:
  /// Default constructor is private as singleton
  BDSAcceleratorComponentRegistry();
  
  /// The singleton instane
  static BDSAcceleratorComponentRegistry* instance;

  /// assignment and copy constructor not implemented nor used
  BDSAcceleratorComponentRegistry& operator=(const BDSAcceleratorComponentRegistry&);
  BDSAcceleratorComponentRegistry(BDSAcceleratorComponentRegistry&);

  /// Vector of created components not in registry, for memory management
  std::vector<BDSAcceleratorComponent*> allocatedComponents;
  
  /// Vector curvilinear components - purely for memory management.
  std::vector<BDSAcceleratorComponent*> curvilinearComponents;

  /// Map to count the number of each type registered. We have to use std::string
  /// as G4String doesn't provide a hash for the unordered map. We use unordered map
  /// because it's faster for insertions and retrievals but slow for range iteration
  /// which we only do for debug print out or once.
  std::unordered_map<std::string, int> typeCounter;
};






#endif
