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
#include "physicsbiasing.h"

#include <iomanip>
#include <iostream>
#include <sstream>

#include "array.h"

using namespace GMAD;

PhysicsBiasing::PhysicsBiasing(){}

void PhysicsBiasing::clear()
{
  name = "";
  particle = "";
  process = "";
  processList.clear();
  factor.clear();
  flag.clear();
}

void PhysicsBiasing::print() const
{
  std::cout << "physicsbiasing: " << std::endl
	    << "name "     << name     << " " << std::endl
	    << "particle " << particle << " " << std::endl
	    << "process "  << process  << " " << std::endl
	    << "factor ";  
  for (const auto& i : factor) std::cout << i << " ";
  std::cout << std::endl << "flag ";
  for (const auto& i : flag) std::cout << static_cast<int>(i) << " ";
  std::cout << std::endl;
}

void PhysicsBiasing::set_value(const std::string& property, double value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="flag") {flag.push_back(static_cast<PhysicsBiasingType>((int)value)); return;}
  if (property=="xsecfact") {factor.push_back(value); return;}
  
  std::cerr << "Error: parser> unknown physicsbiasing option \"" << property << "\" with value " << value << std::endl; 
  exit(1);
}

void PhysicsBiasing::set_value(const std::string& property, Array* value )
{
#ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property;
  for (const auto& i : value->GetData()) std::cout << i << " ";
  std::cout << std::endl;
#endif

  if (property=="flag") {
    for (const auto& i : value->GetData()) {
      flag.push_back(static_cast<PhysicsBiasingType>((int)i));
    }
    return;
  }
  if (property=="xsecfact") {
    value->set_vector(factor);
    return;
  }
  
  std::cerr << "Error: parser> unknown physicsbiasing option \"" << property << "\" with value ";
  for (const auto& i : value->GetData()) std::cout << i << " ";
  std::cout << std::endl;
  exit(1);
}

void PhysicsBiasing::set_value(const std::string& property, std::string value )
{
  #ifdef BDSDEBUG
  std::cout << "parser> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif

  if (property=="name")           {name     = value; return;}
  if (property=="particle")       {particle = value; return;}
  if ((property=="proc") || (property=="process"))
    {
      process = value; 
      std::stringstream ss(process);
      std::string tok;
      while(ss >> tok)
	{processList.push_back(tok);}
      return;
    }

  std::cerr << "Error: parser> unknown physicsbiasing option \"" << property << "\" with value " << value << std::endl;
  exit(1);
}
