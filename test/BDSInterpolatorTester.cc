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
#include "BDSArray2DCoords.hh"
#include "BDSArray2DCoordsRQuad.hh"
#include "BDSArray4D.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldType.hh"
#include "BDSFieldValue.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolatorType.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <ostream>
#include <string>

void Query(BDSFieldMag* field,
	   G4double ymin, G4double ymax, G4double xmin, G4double xmax,
	   G4int nX, G4int nY,
	   G4String outputName)
{
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (double)nX;
  double yStep = (ymax - ymin) / (double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName+".dat");

  int i = 0;
  for (double y = ymin; y < ymax; y += yStep)
    {
      for (double x = xmin; x < xmax; x += xStep)
	{
	  if (i%1000 == 0)
	    {std::cout << "\r" << i;}
	  G4ThreeVector result = field->GetField(G4ThreeVector(x,y,0));
	  ofile << x          << "\t"
		<< y          << "\t"
		<< result.x() / CLHEP::tesla << "\t"
		<< result.y() / CLHEP::tesla << "\t"
		<< result.z() / CLHEP::tesla << "\n";
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;

  std::ofstream ofile2;
  ofile2.open(outputName+"_raw.dat");
  auto r = dynamic_cast<BDSFieldMagInterpolated2D*>(field)->Interpolator()->Array();
  ofile2 << *r;
  ofile2.close();
}

int main(int /*argc*/, char** /*argv*/)
{
  double xmin = -25*CLHEP::cm;
  double xmax = 25*CLHEP::cm;
  int    nX   = 100;
  double ymin = -25*CLHEP::cm;
  double ymax = 25*CLHEP::cm;
  int    nY   = 100;

  // 2D Nearest Neighbour
  BDSFieldInfo* infoBiNearest = new BDSFieldInfo(BDSFieldType::bmap2d,
						 0,
						 BDSIntegratorType::g4classicalrk4,
						 nullptr,
						 false,
						 G4Transform3D(),
						 "square120x120_2mm.TXT",
						 BDSFieldFormat::poisson2dquad,
						 BDSInterpolatorType::nearest2d);
  
  BDSFieldMag* biNearest = BDSFieldLoader::Instance()->LoadMagField(*infoBiNearest);

  // 2D Linear
  BDSFieldInfo* infoBiLinear = new BDSFieldInfo(BDSFieldType::bmap2d,
						0,
						BDSIntegratorType::g4classicalrk4,
						nullptr,
						false,
						G4Transform3D(),
						"square120x120_2mm.TXT",
						BDSFieldFormat::poisson2dquad,
						BDSInterpolatorType::linear2d);
  
  BDSFieldMag* biLinear = BDSFieldLoader::Instance()->LoadMagField(*infoBiLinear);

  // 2D Cubic
  BDSFieldInfo* infoBiCubic = new BDSFieldInfo(BDSFieldType::bmap2d,
					       0,
					       BDSIntegratorType::g4classicalrk4,
					       nullptr,
					       false,
					       G4Transform3D(),
					       "square120x120_2mm.TXT",
					       BDSFieldFormat::poisson2dquad,
					       BDSInterpolatorType::cubic2d);
  
  BDSFieldMag* biCubic = BDSFieldLoader::Instance()->LoadMagField(*infoBiCubic);

  // Get the raw data
  BDSFieldMagInterpolated2D* fieldInterp = dynamic_cast<BDSFieldMagInterpolated2D*>(biNearest);
  auto interp = fieldInterp->Interpolator();
  auto arrCoords = interp->Array();
  const BDSArray4D* arr = dynamic_cast<const BDSArray4D*>(arrCoords);
  std::ofstream ofile;
  ofile.open("raw.dat");
  ofile << *arr;
  ofile.close();

  // Query across full range of magnet including just outside range too.
  Query(biNearest, ymin, ymax, xmin, xmax, nX, nY, "nearest");
  Query(biLinear, ymin, ymax, xmin, xmax, 3*nX, 3*nY, "linear");
  Query(biCubic, ymin, ymax, xmin, xmax, 3*nX, 3*nY, "cubic");

  // Now query in small region where there's large variation.
  Query(biNearest, 50, 110, 110, 170, nX, nY, "nearest_zoom");
  Query(biLinear, 50, 110, 110, 170, nX, nY, "linear_zoom");
  Query(biCubic, 50, 110, 110, 170, nX, nY, "cubic_zoom");

  G4cout << biNearest->GetField(G4ThreeVector(130, 74, 0)) << G4endl;
  G4cout << biLinear->GetField(G4ThreeVector(130, 74, 0)) << G4endl;
  G4cout << biCubic->GetField(G4ThreeVector(130, 74, 0)) << G4endl;

  return 0;
}
