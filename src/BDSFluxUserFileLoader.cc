/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2024.

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

author :: S Alden
*/

#include "BDSFluxUserFileLoader.hh"

#include "BDSDebug.hh"
#include "BDSException.hh"

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
#include "BDSOctree.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

template <class T>
BDSFluxUserFileLoader<T>::BDSFluxUserFileLoader()
{;}

template <class T>
BDSFluxUserFileLoader<T>::~BDSFluxUserFileLoader()
{;}

template <class T>
BDSOctree* BDSFluxUserFileLoader<T>::Load(const G4String& fileName,G4ThreeVector& lowerBoundsIn, G4ThreeVector& upperBoundsIn)
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
    {throw BDSException(__METHOD_NAME__, "Cannot open file \"" + fileName + "\"");}
  else
    {G4cout << "BDSFluxUserFileLoader::Load> loading \"" << fileName << "\"" << G4endl;}

  std::string line;

  BDSOctree* data = new BDSOctree(lowerBoundsIn, upperBoundsIn);
  G4int nPoints = 0;
  G4int lineNum = 1;
  while (std::getline(file, line))
    { // read a line only if it's not a blank one
      std::istringstream lines(line);
      std::string xcoord;
      std::string ycoord;
      std::string zcoord;

      std::string intensityValueString;

      // skip a line if it's only whitespace
      if (std::all_of(line.begin(), line.end(), isspace))
        {continue;}

      lines >> xcoord >> ycoord >> zcoord >> intensityValueString;

      // exit if anything after the importance value
      if (!lines.eof())
	{
	  std::string remainder;
	  lines >> remainder;
	  G4String message = "Error: Unknown value \"" + remainder + "\" in line " + std::to_string(lineNum) + " of the fluxUserFile";
          throw BDSException(message);
	}

      // exit if no importance value is supplied
      if (intensityValueString.empty())
        {
          G4String message = "No intensity value was found for coordinate: x= \"" + xcoord + "\", y= \"" + ycoord + "\", z= \"" + zcoord + "\" in the importanceMapFile.";
          throw BDSException(message);
        }

      G4double intensityValue = 0;
      try
	{intensityValue = std::stod(intensityValueString);}
      catch (...)
	{
	  G4String message = "Error: Coordinate \" +  x= \"" + xcoord + "\", y= \"" + ycoord + "\", z= \"" + zcoord + "\" has intensity value \"" + intensityValueString + "\",";
          message += " importance value must be numeric.";
          throw BDSException(message);
	}
      G4double x = std::stod(xcoord);
      G4double y = std::stod(ycoord);
      G4double z = std::stod(zcoord);
      G4ThreeVector coordinate(x,y,z);
      G4double intensity = std::stod(intensityValueString);
      data->insert(coordinate, intensity);
      nPoints++;
      lineNum += 1;
    }

  file.close();

  G4cout << "BDSFluxUserFileLoader::Load> loaded " << nPoints << " intensity entries" << G4endl;

  return data;
}

template class BDSFluxUserFileLoader<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSFluxUserFileLoader<igzstream>;
#endif



