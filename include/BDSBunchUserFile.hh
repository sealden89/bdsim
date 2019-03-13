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
  G4int nlinesIgnore;     ///< Number of lines that will be ignored after each particle (default 0).
  G4double particleMass; ///< Cache of nominal beam particle mass.
  
  void ParseFileFormat();
  void OpenBunchFile();
  void SkipLines();
  void CloseBunchFile();
  void skip(G4int nvalues);
  T InputBunchFile;
	template <typename Type> G4bool ReadValue(Type &value);
  struct Doublet {
    G4String name;
    G4double unit; ///< relative to SI units, i.e. mm=0.001 etc.
  };
  std::list<Doublet> fields;
  void SetDistrFile(G4String filename);
  void SetBunchFormat(G4String val) {bunchFormat=val;}
  void SetNLinesIgnore(G4int val)   {nlinesIgnore=val;}
  G4double ParseEnergyUnit(G4String &fmt);
  G4double ParseLengthUnit(G4String &fmt);
  G4double ParseAngleUnit(G4String &fmt);
  G4double ParseTimeUnit(G4String &fmt);

  /// Print out warning we're looping and reopen file from beginning. Includes skipping
  /// lines. Put in a function as used in multiple places.
  void EndOfFileAction();
  
  G4double ffact;
};

#endif
