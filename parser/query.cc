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
#include "query.h"

using namespace GMAD;

Query::Query()
{
  clear();
  PublishMembers();
}

void Query::clear()
{
  name        = "";
  fieldObject = "";
  nDimensions = 1;
  nx   = 1;
  ny   = 1;
  nz   = 1;
  nt   = 1;
  xmin = 0;
  xmax = 1;
  ymin = 0;
  ymax = 1;
  zmin = 0;
  zmax = 1;
  tmin = 0;
  tmax = 0;
  outfile = "";
}

void Query::PublishMembers()
{
  publish("name",        &Query::name);
  publish("fieldObject", &Query::fieldObject);
  publish("nDimensions", &Query::nDimensions);
  publish("nx",          &Query::nx);
  publish("ny",          &Query::ny);
  publish("nz",          &Query::nz);
  publish("nt",          &Query::nt);
  publish("xmin",        &Query::xmin);
  publish("ymin",        &Query::ymin);
  publish("zmin",        &Query::zmin);
  publish("tmin",        &Query::tmin);
  publish("xmax",        &Query::xmax);
  publish("ymax",        &Query::ymax);
  publish("zmax",        &Query::zmax);
  publish("tmax",        &Query::tmax);
  publish("outfile",     &Query::outfile);
}

void Query::print()const
{
  std::cout << "query: "
	    << "name "        << name        << std::endl
	    << "fieldObject " << fieldObject << std::endl
	    << "nDimensions " << nDimensions << std::endl
	    << "nx "  << nx   << std::endl
    	    << "ny "  << ny   << std::endl
    	    << "nz "  << nz   << std::endl
    	    << "nt "  << ny   << std::endl
	    << "x: (" << xmin << ", " << xmax << ")" << std::endl
    	    << "y: (" << ymin << ", " << ymax << ")" << std::endl
    	    << "z: (" << zmin << ", " << zmax << ")" << std::endl
    	    << "t: (" << tmin << ", " << tmax << ")" << std::endl;
}
