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
#include "placement.h"

using namespace GMAD;

Placement::Placement()
{
  clear();
  PublishMembers();
}

void Placement::clear()
{
  name         = "";
  geometryFile = "";
  sequence     = "";
  referenceElement = "";
  referenceElementNumber = 0;
  s     = 0;
  x     = 0;
  y     = 0;
  z     = 0;
  phi   = 0;
  theta = 0;
  psi   = 0;
  axisX = 0;
  axisY = 0;
  axisZ = 0;
  angle = 0;
  sensitive     = true;
  axisAngle     = false;
}

void Placement::PublishMembers()
{
  publish("name",          &Placement::name);
  publish("geometryFile",  &Placement::geometryFile);
  publish("sequence",      &Placement::sequence);
  publish("referenceElement", &Placement::referenceElement);
  publish("referenceElementNumber", &Placement::referenceElementNumber);
  publish("s",             &Placement::s);
  publish("x",             &Placement::x);
  publish("y",             &Placement::y);
  publish("z",             &Placement::z);
  publish("phi",           &Placement::phi);
  publish("theta",         &Placement::theta);
  publish("psi",           &Placement::psi);
  publish("axisX",         &Placement::axisX);
  publish("axisY",         &Placement::axisY);
  publish("axisZ",         &Placement::axisZ);
  publish("angle",         &Placement::angle);
  publish("sensitive",     &Placement::sensitive);
  publish("axisAngle",     &Placement::axisAngle);
}

void Placement::print()const
{
  std::cout << "Placement: "
	    << "name "          << name          << std::endl
	    << "geometryFile "  << geometryFile  << std::endl
	    << "sequence "      << sequence      << std::endl
	    << "referenceElement" << referenceElement << std::endl
	    << "referenceElementNumber" << referenceElementNumber << std::endl
	    << "s"              << s             << std::endl
	    << "x "             << x             << std::endl
    	    << "y "             << y             << std::endl
    	    << "z "             << z             << std::endl
    	    << "phi "           << phi           << std::endl
    	    << "theta "         << theta         << std::endl
    	    << "psi "           << psi           << std::endl
    	    << "axisX "         << axisX         << std::endl
    	    << "axisY "         << axisY         << std::endl
    	    << "axisZ "         << axisZ         << std::endl
    	    << "angle "         << angle         << std::endl
	    << "sensitive "     << sensitive     << std::endl
	    << "axisAngle "     << axisAngle     << std::endl;
}
