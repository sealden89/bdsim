/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "symbolmap.h"

#include "sym_table.h"

#include <cstdlib>
#include <iostream>

using namespace GMAD;

Symtab * SymbolMap::symcreate(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it!=symtab_map.end()) {
    std::cerr << "ERROR Variable " << s << " is already defined!" << std::endl;
    exit(1);
  }
    
  Symtab* sp = new Symtab(s);
  std::pair<std::map<std::string,Symtab*>::iterator,bool> ret = symtab_map.insert(std::make_pair(s,sp));
  return (*(ret.first)).second;
}
  
Symtab * SymbolMap::symlook(std::string s)
{
  std::map<std::string,Symtab*>::iterator it = symtab_map.find(s);
  if (it==symtab_map.end()) {
    return nullptr;
  } 
  return (*it).second;
}

void SymbolMap::clear()
{
  std::map<std::string,Symtab*>::iterator it;
  for(it=symtab_map.begin();it!=symtab_map.end();++it) {
    delete (*it).second;
  }
  symtab_map.clear();
}
