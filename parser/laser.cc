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
#include "laser.h"

Laser::Laser()
{
    clear();
    PublishMembers();
}

void Laser::clear()
{

    name             = "";
    waveLength       = 0;
    M2               = 0;
    pulseEnergy      = 0;
    pulseDuration    = 0;
    Waist            = 0;


}

void Laser::PublishMembers()
{
    publish("name",               &Element::name);
    publish("waveLength"),        &Element::waveLength);
    publish("M2",                 &Element::M2);
    publish("pulseEnergy",        &Element::pulseEnergy);
    publish("pulseDuration",      &Element::pulseDuration);
    publish("waist",              &Element::waist);

}

void Laser::print()const
{
    std::cout   << "Laser: "
                << "name "             << name             <<std::endl
                << "waveLength "       << waveLength       << std::endl
                << "M2 "               << M2               << std::endl
                << "pulseEnergy "      << pulseEnergy      << std::endl
                << "pulseDuration "    << pulseDuration    << std::endl
                << "waist "            << waist                 << std::endl;
}