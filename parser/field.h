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
#ifndef FIELD_H
#define FIELD_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  /**
   * @brief Field class for parser
   * 
   * @author Laurie Nevay
   */
  
  class Field : public Published<Field>
  {
  public:
    std::string name;                 ///< Name.
    std::string type;                 ///< Type.
    double      eScaling;             ///< E Field Scaling factor.
    double      bScaling;             ///< B Field Scaling factor.
    std::string integrator;           ///< Which integrator to use.
    bool        globalTransform;      ///< Provide global to curvilinear coordinate transform.
    std::string magneticFile;         ///< File for magnetic field map.
    std::string magneticInterpolator; ///< Interpolator for the magnetic field.
    std::string electricFile;         ///< File for the electric field map.
    std::string electricInterpolator; ///< Interpolator for the electric field.

    double x; ///< Offset in x.
    double y; ///< Offset in y.
    double z; ///< Offset in z.
    double t; ///< Offset in t.
    /// @{ Euler angle for rotation.
    double phi;
    double theta;
    double psi;
    /// @}
    /// @{ Axis angle rotation.
    double axisX;
    double axisY;
    double axisZ;
    double angle;
    /// @}
    bool   axisAngle; ///< Flag to use the axis angle construction of rotation.

    bool   autoScale; ///< Scale the field to the magnet it's attached to.

    /// Maximum permitted step length in the volumes the field is attached to.
    double maximumStepLength;

    std::string magneticSubField;
    std::string electricSubField;
    
    /// Constructor
    Field();
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
    void Field::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "field> Setting value " << std::setw(25) << std::left << property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try {
        set(this,property,value);
      }
      catch(const std::runtime_error&) {
        std::cerr << "Error: field> unknown option \"" << property << "\" with value " << value  << std::endl;
        exit(1);
      }
    }
}

#endif
