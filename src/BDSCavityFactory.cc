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
#include "BDSCavityFactory.hh"
#include "BDSCavityFactoryBase.hh"
#include "BDSCavityFactoryElliptical.hh"
#include "BDSCavityFactoryPillBox.hh"
#include "BDSCavityFactoryRectangular.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSDebug.hh"

#include "globals.hh"

BDSCavityFactory* BDSCavityFactory::instance = nullptr;

BDSCavityFactory* BDSCavityFactory::Instance()
{
  if (!instance)
    {instance = new BDSCavityFactory();}
  return instance;
}

BDSCavityFactory::BDSCavityFactory():
  elliptical(new BDSCavityFactoryElliptical()),
  rectangular(new BDSCavityFactoryRectangular()),
  pillBox(new BDSCavityFactoryPillBox())
{;}

BDSCavityFactory::~BDSCavityFactory()
{
  delete elliptical;
  delete rectangular;
  delete pillBox;
  instance = nullptr;
}

BDSCavityFactoryBase* BDSCavityFactory::GetAppropriateFactory(const BDSCavityType cavityType)
{
  switch (cavityType.underlying())
    {
    case BDSCavityType::elliptical:
      {return elliptical; break;}
    case BDSCavityType::rectangular:
      {return rectangular; break;}
    case BDSCavityType::pillbox:
      {return pillBox; break;}
    default:
      {
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "unknown type \"" << cavityType << "\" - pill box by default" << G4endl;
#endif
	return pillBox;
	break;
      }
    }
}

BDSCavity* BDSCavityFactory::CreateCavity(G4String             name,
					  G4double             totalChordLength,
					  const BDSCavityInfo* info,
					  G4Material*          vacuumMaterial)
{
  BDSCavityFactoryBase* factory = GetAppropriateFactory(info->cavityType);

  return factory->CreateCavity(name, totalChordLength, info, vacuumMaterial);
}
