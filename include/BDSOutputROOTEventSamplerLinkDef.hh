/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link off all namespaces;
#pragma link C++ class std::vector<double>+;
#pragma link C++ class std::vector<int>+;
//#pragma link C++ class BDSOutputROOTEventSampler+;
#pragma link C++ class BDSOutputROOTEventSampler<float>+;
#pragma link C++ class BDSOutputROOTEventSampler<double>+;
#pragma link C++ namespace BDSOutputROOTEventSampler<float>;
#pragma link C++ function BDSOutputROOTEventSampler<float>::charge;
#pragma link C++ function BDSOutputROOTEventSampler<double>::charge;
#pragma link C++ function mass;
#pragma link C++ function rigidity;

#endif
