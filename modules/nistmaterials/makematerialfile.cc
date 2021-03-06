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
/**
 * @file makematerialfile.cc
 */
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4IsotopeVector.hh"
#include "G4NistManager.hh"
#include "G4String.hh"
#include "G4Types.hh"
#include "G4Version.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <vector>

int main(int, char**)
{
  std::ofstream efile;
  efile.open("nistelements.txt");
  
  G4double gcm3 = CLHEP::g / CLHEP::cm3;
  
  // calling this forces construction of all elements and NIST materials
  G4NistManager* nm = G4NistManager::Instance();
  
  efile << "# V1.0\n";
  efile << "# NIST Elements from BDSIM for pyg4ometry\n";
  efile << "# Geant4 version: " << G4Version << "\n";
  efile << "# element\tZ\tName\tdensity(g/cm^3)\tI(eV)\n";
  efile << "# for each isotope\n";
  efile << "# \tA\tfractional abundance\n";

  //int nElements = (int)nm->GetNistElementNames().size(); // nope
  int nElements = 99;
  efile << "# numberOfZ " << nElements << "\n";
  // the hard-coded number in G4NistMaterialBuilder (needed for density)
  // this number is not accessible in the code, so hard coded here
  for (G4int Z = 1; Z < nElements; Z++)
    {
      G4Element* el = nm->FindOrBuildElement(Z);
      G4String name = "G4_" + el->GetName();
      G4Material* mat = nm->FindOrBuildMaterial(name);
      G4double meanIonisationEnergy = nm->GetMeanIonisationEnergy(Z);
      
      efile << "element"
	    << "\t" << Z
	    << "\t" << name
	    << "\t" << mat->GetDensity() / gcm3
	    << "\t" << meanIonisationEnergy / CLHEP::eV
	    << "\n";
      
      G4int nIsotopes = el->GetNumberOfIsotopes();
      G4double* abundances = el->GetRelativeAbundanceVector();
      G4IsotopeVector* isotopes = el->GetIsotopeVector();
      if (nIsotopes > 1 && isotopes)
	{
	  int i = 0;
	  for (const auto& iso : *isotopes)
	    {
	      efile << "\t" << iso->GetN() // number of nucleons
		    << "\t" << abundances[i]
		    << "\n";
	      i++;
	    }
	}
      
    }
  efile.close();
  
  std::ofstream mfile;
  mfile.open("nistmaterials.txt");
  
  mfile << "# V1.0\n";
  mfile << "# NIST Materials from BDSIM for pyg4ometry\n";
  mfile << "# Geant4 version: " << G4Version << "\n";
  mfile << "# compmass\tZ\tName\tdensity(g/cm^3)\tI(eV)\n";
  mfile << "# for each element\n";
  mfile << "# \tName\tZ\tA\tfraction of mass\n";
  
  std::vector<G4String> materialNames = nm->GetNistMaterialNames();
  
  // build a map of names to indices as much quicker and easier to search / use
  // pretty rubbish design, but G4NistManager *only* allows access to the mean ionisation
  // energy and the nominal density via an index of the material in its internal vector
  // which it gives no access to...  So we build a map of indices from the names and assume
  // the materials don't change with what we do here. Rubbish.
  std::map<G4String, G4int> materialNameToIndex;
  for (int i = 0; i < (int)materialNames.size(); i++)
    {materialNameToIndex[materialNames[i]] = i;}
  
  // filter out elements from the names as no way to access them separately
  std::vector<G4String> elementNames = nm->GetNistElementNames();
  std::set<G4String> g4ElementNames;
  for (const auto& name : elementNames)
    {
      if (!name.empty()) // apparently some are just empty...
	{g4ElementNames.insert("G4_"+name);}
    }
  std::set<G4String> materialSet = {materialNames.begin(), materialNames.end()};
  for (const auto& name : g4ElementNames)
    {materialSet.erase(name);}
  
  // write the file
  for (const auto& materialName : materialSet)
    {
      G4Material* mat = nm->FindOrBuildMaterial(materialName);
      // "Z" as the argument to this function is actually just the index in the material vector
      G4double meanIonisationEnergy = nm->GetMeanIonisationEnergy(materialNameToIndex[mat->GetName()]);
      mfile << "compmass"
	    << "\t" << mat->GetNumberOfElements()
	    << "\t" << mat->GetName()
	    << "\t" << mat->GetDensity() / gcm3
	    << "\t" << meanIonisationEnergy / CLHEP::eV
	    << "\n";
      
      const G4ElementVector* elementArray = mat->GetElementVector();
      const G4double* fractionArray = mat->GetFractionVector();
      for (int i = 0; i < (int)elementArray->size(); i++)
	{
	  const auto element = (*elementArray)[i];
	  mfile << "\t" << element->GetName()
		<< "\t" << element->GetZ()
		<< "\t" << element->GetN()
		<< "\t" << fractionArray[i]
		<< "\n";
	}
    }
  mfile.close();
  return 0;
}
