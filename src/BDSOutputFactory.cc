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
#include "BDSOutputFactory.hh"
#include "BDSOutputType.hh"
#include "BDSOutput.hh"
#include "BDSOutputNone.hh"
#include "BDSOutputROOT.hh"


BDSOutput* BDSOutputFactory::CreateOutput(BDSOutputType format,
					  G4String      fileName,
					  G4int         fileNumberOffset)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "setting up output." << G4endl;
  G4cout << __METHOD_NAME__ << "output format = " << format << G4endl;
#endif
  BDSOutput* result = nullptr;
  switch (format.underlying())
    {
    case BDSOutputType::none:
      {result = new BDSOutputNone(); break;}
    case BDSOutputType::rootevent:
      {result = new BDSOutputROOT(fileName, fileNumberOffset); break;}
    default:
      {result = new BDSOutputNone(); break;} // absolute default - should not reach this
    }
  return result;
}
