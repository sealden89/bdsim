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
#include "samplerplacement.h"

using namespace GMAD;

SamplerPlacement::SamplerPlacement()
{
  clear();
  PublishMembers();
}

void SamplerPlacement::clear()
{
  name         = "";
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
  axisAngle = false;
  apertureModel = "";
  shape = "circular";
  aper1 = 0;
  aper2 = 0;
  aper3 = 0;
  aper4 = 0;
}

void SamplerPlacement::PublishMembers()
{
  publish("name",          &SamplerPlacement::name);
  publish("referenceElement", &SamplerPlacement::referenceElement);
  publish("referenceElementNumber", &SamplerPlacement::referenceElementNumber);
  publish("s",             &SamplerPlacement::s);
  publish("x",             &SamplerPlacement::x);
  publish("y",             &SamplerPlacement::y);
  publish("z",             &SamplerPlacement::z);
  publish("phi",           &SamplerPlacement::phi);
  publish("theta",         &SamplerPlacement::theta);
  publish("psi",           &SamplerPlacement::psi);
  publish("axisX",         &SamplerPlacement::axisX);
  publish("axisY",         &SamplerPlacement::axisY);
  publish("axisZ",         &SamplerPlacement::axisZ);
  publish("angle",         &SamplerPlacement::angle);
  publish("axisAngle",     &SamplerPlacement::axisAngle);
  publish("apertureModel", &SamplerPlacement::apertureModel);
  publish("shape",         &SamplerPlacement::shape);
  publish("apertureType",  &SamplerPlacement::shape);
  publish("aper1",         &SamplerPlacement::aper1);
  publish("aper2",         &SamplerPlacement::aper2);
  publish("aper3",         &SamplerPlacement::aper3);
  publish("aper4",         &SamplerPlacement::aper4);
}

void SamplerPlacement::print()const
{
  std::cout << "SamplerPlacement: "
	    << "name "          << name          << std::endl
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
	    << "axisAngle "     << axisAngle     << std::endl
    	    << "apertureMode "  << apertureModel << std::endl
	    << "shape "         << shape         << std::endl
	    << "aper1 "         << aper1         << std::endl
	    << "aper2 "         << aper2         << std::endl
	    << "aper3 "         << aper3         << std::endl
	    << "aper4 "         << aper4         << std::endl;
}
