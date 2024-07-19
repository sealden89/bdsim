/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2024.

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
#include "laser.h"

using namespace GMAD;

Laser::Laser()
{
  clear();
  PublishMembers();
}

void Laser::clear()
{
  name             = "";
  wavelength       = 0;
  m2               = 0;
  pulseEnergy      = 0;
  pulseDuration    = 0;
  w0               = 0;
  sigma0           = 0;
  laserArrivalTime = 0;
  polarization1    = 0;
  polarization2    = 0;
  polarization3    = 0;
  ignoreRayleighRange = false;
}

void Laser::PublishMembers()
{
  publish("name",               &Laser::name);
  publish("wavelength",         &Laser::wavelength);
  publish("m2",                 &Laser::m2);
  publish("pulseEnergy",        &Laser::pulseEnergy);
  publish("pulseDuration",      &Laser::pulseDuration);
  publish("w0",                 &Laser::w0);
  publish("sigma0",             &Laser::sigma0);
  publish("laserArrivalTime",   &Laser::laserArrivalTime);
  publish("polarization1",   &Laser::polarization1);
  publish("polarization2",   &Laser::polarization2);
  publish("polarization3",   &Laser::polarization3);
  publish("ignoreRayleighRange",&Laser::ignoreRayleighRange);
}

void Laser::print()const
{
  std::cout << "Laser: "
	    << "name "                << name          << std::endl
	    << "wavelength "          << wavelength    << std::endl
	    << "m2 "                  << m2            << std::endl
	    << "pulse energy "        << pulseEnergy   << std::endl
	    << "pulse duration "      << pulseDuration << std::endl
	    << "w0 "                  << w0            << std::endl
	    << "sigma0 "              << sigma0        << std::endl
        << "laserArrivalTime "    << laserArrivalTime  << std::endl
        << "polarization1 "       << polarization1  << std::endl
        << "polarization2 "       << polarization2  << std::endl
        << "polarization3 "       << polarization3  << std::endl
        << "ignoreRayleighRange " << ignoreRayleighRange << std::endl;
}
