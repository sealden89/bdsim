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
#ifndef BDSBUNCHUSERFILE_H
#define BDSBUNCHUSERFILE_H 

#include "BDSBunch.hh"

#include <fstream>
#include <list>
#include <sstream>
#include <string>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

/**
 * @brief A bunch distribution that reads a user specified column file.
 * 
 * @author Lawrence Deacon
 */

template <class T>
class BDSBunchUserFile: public BDSBunch
{
public: 
  BDSBunchUserFile();
  virtual ~BDSBunchUserFile();
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity,
			  const G4double beamlineS = 0);
  virtual void CheckParameters();

  /// Advance to the correct event number in the file for recreation. The implementation
  /// is brute-force getting of the lines - this could be more efficient (certainly for
  /// a file that is looped over multiple times) but this is simple, clear and the time
  /// penalty is on the order of 1 minute for ~100k events.
  virtual void RecreateAdvanceToEvent(G4int eventOffset);

  /// Get the next particle.
  virtual BDSParticleCoordsFull GetNextParticleLocal();
  
private:
  G4String distrFile;     ///< Bunch file.
  G4String distrFilePath; ///< Bunch file including absolute path.
  G4String bunchFormat;   ///< Format of the file.
  G4int    nlinesIgnore;  ///< Number of lines that will be ignored after each particle (default 0).
  G4double particleMass;  ///< Cache of nominal beam particle mass.
  G4int    lineCounter;   ///< Line counter.

  void ParseFileFormat(); ///< Parse the column tokens and units factors
  void OpenBunchFile();   ///< Open the file and check it's open.
  void SkipLines();       ///< Read lines according to nlinesIgnore.
  void CloseBunchFile();  ///< Close the file handler

  /// Read a word out of the string stream, in effect advancing the internal
  /// string stream iterator.
  void skip(std::stringstream& stream, G4int nvalues);

  /// The file handler. Templated as could be std::ifstream or igzstream for example.
  T InputBunchFile;

  /// Read a word into a value. Templated so we can cleverly use different types
  /// to read into directly such as int or double.
  template <typename Type> void ReadValue(std::stringstream& stream, Type& value);

  /// Struct for name and unit pair.
  struct Doublet {
    G4String name;
    G4double unit; ///< relative to SI units, i.e. mm=0.001 etc.
  };

  /// List of variables to parse on each line.
  std::list<Doublet> fields;

  /// @{ Utility function to parse variable and unit string.
  G4double ParseEnergyUnit(const G4String& fmt);
  G4double ParseLengthUnit(const G4String& fmt);
  G4double ParseAngleUnit(const G4String& fmt);
  G4double ParseTimeUnit(const G4String& fmt);
  /// @}
  
  /// Print out warning we're looping and reopen file from beginning. Includes skipping
  /// lines. Put in a function as used in multiple places.
  void EndOfFileAction();
  
  G4double ffact; ///< Cache of flip factor from global constants.
};

#endif
