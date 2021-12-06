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
#include "HistSparse1D.hh"

templateClassImp(HistSparse1D)

template<class T>
HistSparse1D<T>::HistSparse1D():
  name("sparse_hist"),
  entries(0)
{;}

template<class T>
HistSparse1D<T>::~HistSparse1D()
{;}

template class HistSparse1D<long long int>;
