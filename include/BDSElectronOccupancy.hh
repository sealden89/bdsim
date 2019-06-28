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
#ifndef BDSElectronOccupancy_H
#define BDSElectronOccupancy_H

#include "G4ElectronOccupancy.hh"
#include <vector>
#include <tuple>

/**
 * @brief Electron Occupancy for more than just n quantum number
 *
 * @author Siobhan Alden
 */

class BDSElectronOccupancy: public G4ElectronOccupancy {
public:
	BDSElectronOccupancy(G4int sizeOrbit);
	BDSElectronOccupancy(const G4ElectronOccupancy &right);

	BDSElectronOccupancy(const BDSElectronOccupancy &right);

	~BDSElectronOccupancy();

	inline void SetTheTotalOccupancy(const G4int theTotalOccupancyIn) { theTotalOccupancy = theTotalOccupancyIn; };

	G4int AddElectron(G4int orbit, G4int number = 1, G4int shape = 0);

	G4int RemoveElectron(G4int orbit, G4int number = 1, G4int shape = 0);

	void Populate2DOccupancy();
	void PopulateOccupancyOrder();


private:
	//mimicking G4ElectronOccupancy
	G4int theShapeOfOrbit;
	G4int theSizeOfOrbit;
	G4int theTotalOccupancy;
	G4int *theOccupancies;

	// 2d nl array for occupancy states
	G4int theOccupancies2D[8][4];


	//new values for BDS
	G4int sMaxOccupancy = 2;
	G4int pMaxOccupancy = 6;
	G4int dMaxOccupancy = 10;
	G4int fMaxOccupancy = 14;
	G4int notAllowed = 99;

	// order for population
	std::vector<std::tuple<G4int,G4int,G4int>> occupancyOrder;

};

#endif
