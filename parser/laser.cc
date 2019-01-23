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
    laserM2               = 0;
    laserPulseEnergy      = 0;
    laserPulseDuration    = 0;
    laserWaist            = 0;
    laserTime             = 0;
    laserOffsetTheta      = 0;
    laserOffsetPhi        = 0;
    laserOffsetX          = 0;
    laserOffsetY          = 0;
    laserOffsetZ          = 0;
}

void Laser::PublishMembers()
{
    publish("laserM2",                 &Element::laserM2);
    publish("laserPulseEnergy",        &Element::laserPulseEnergy);
    publish("laserPulseDuration",      &Element::laserPulseDuration);
    publish("laserTime",               &Element::laserTime);
    publish("laserWaist",              &Element::laserWaist);

}

void Laser::print()const
{
    std::cout   << "Laser: "
                << "laserM2 "               << laserM2               <<std::endl;
                << "laserPulseEnergy "      << laserPulseEnergy      <<std::endl;
                << "laserPulseDuration "    << laserPulseDuration    <<std::endl;
                << "laserTime "             << laserTime             <<std::endl;
}