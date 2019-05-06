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
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSImportanceFileLoader.hh"

#include "globals.hh"
#include "G4String.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <exception>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

template <class T>
BDSImportanceFileLoader<T>::BDSImportanceFileLoader()
{;}

template <class T>
BDSImportanceFileLoader<T>::~BDSImportanceFileLoader()
{;}

template <class T>
std::map<G4String, G4double> BDSImportanceFileLoader<T>::Load(const G4String& fileName)
{
  T file;

  file.open(fileName);
  
  // test if file is valid
#ifdef USE_GZSTREAM
  bool validFile = file.rdbuf()->is_open();
#else
  bool validFile = file.is_open();
#endif

  if (!validFile)
    {
      G4cerr << "Invalid file name or no such file named \"" << fileName << "\"" << G4endl;
      exit(1);
    }
  else
    {G4cout << "BDSImportanceFileLoader::Load> loading \"" << fileName << "\"" << G4endl;}

  std::string line;
  std::map<G4String, G4double> importance;
  G4int linenum = 1;
  while (std::getline(file, line))
    { // read a line only if it's not a blank one
      std::istringstream liness(line);
      std::string volume;
      std::string importanceValueString;
      std::string remainder;

      // Skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;}

      // exit if anything after the importance value
      if (!(std::istringstream(line) >> volume >> importanceValueString).eof())
        {
          liness >> volume >> importanceValueString >> remainder;
          G4String message = "Error: Unknown value \"" + remainder + "\" in line "+ std::to_string(linenum) +" of the importanceMapFile";
          throw BDSException(message);
        }

      liness >> volume >> importanceValueString;

      // exit if no importance value is supplied
      if (importanceValueString.empty())
        {
          G4String message = "No importance value was found for cell \"" + volume + "\" in the importanceMapFile.";
          throw BDSException(message);
        }

      G4double importanceValue = 0;
      // read importance value string as a double but check if numeric
      auto isNumeric = (std::istringstream(importanceValueString) >> importanceValue >> std::ws).eof();
      // exit if non-numeric.
      if (!isNumeric)
        {
          G4String message = "Error: Cell \"" + volume + "\" has importance value \"" + importanceValueString + "\",";
          message += " importance value must be numeric.";
          throw BDSException(message);
        }
      
      importance[volume] = importanceValue;

      linenum += 1;
    }

  file.close();

  G4cout << "BDSImportanceFileLoader::Load> loaded " << importance.size() << " importance entries" << G4endl;

  return importance;
}

template class BDSImportanceFileLoader<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSImportanceFileLoader<igzstream>;
#endif
