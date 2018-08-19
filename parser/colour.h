/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#ifndef COLOUR_H
#define COLOUR_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Colour definition for parser.
   * 
   * @author Laurie Nevay
   */
  
  class Colour: public Published<Colour>
  {
  public:
    std::string name;                 ///< Name of object.
    double red;
    double green;
    double blue;
    double alpha;
    
    /// Constructor
    Colour();
    /// Reset
    void clear();
    /// Print some properties
    void print()const;
    /// Set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
    void Colour::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "colour> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
        set(this,property,value);
      }
      catch(std::runtime_error) {
        std::cerr << "Error: crystal> unknown option \"" << property << "\" with value " << value  << std::endl;
        exit(1);
      }
    }
}

#endif
