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

/**
 * @brief Electron Occupancy for more than just n quantum number
 *
 * @author Siobhan Alden
 */

class BDSElectronOccupancy: public G4ElectronOccupancy
{
public:
    BDSElectronOccupancy(G4int sizeOrbit);
    BDSElectronOccupancy(const G4ElectronOccupancy &right);
    BDSElectronOccupancy(const BDSElectronOccupancy &right);
	~BDSElectronOccupancy();

	inline void SetTheTotalOccupancy(const G4int theTotalOccupancyIn) {theTotalOccupancy=theTotalOccupancyInl;};
	G4int AddElectron(G4int orbit, G4int shape, G4int number=1);
	G4int RemoveElectron(G4int orbit, G4int shape, G4int number=1);


private:
    G4int theShapeOfOrbit;
	G4int theSizeOfOrbit;
	G4int theTotalOccupancy;
	G4int* theOccupancies;
	G4int sMaxOccupancy=2;
	G4int pMaxOccupancy=6;
	Gtint dMaxOccupancy=10;



};

#endif
