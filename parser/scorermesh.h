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
#ifndef SCORERMESH_H
#define SCORERMESH_H

#include <iomanip>
#include <iostream>
#include <string>

#include "published.h"

namespace GMAD
{
  class SamplerPlacement;
  
  /**
   * @brief ScorerMesh class for parser
   * 
   * @author Laurie Nevay
   */
  class ScorerMesh : public Published<ScorerMesh>
  {
  public:
    std::string name;          ///< Name of this placement.
    std::string scoreQuantity; ///< Name of scorer object to use.
    
    int nx;             ///< Number of bins in x.
    int ny;             ///< Number of bins in y.
    int nz;             ///< Number of bins in z.
    double xsize;       ///< X total width.
    double ysize;       ///< Y total width.
    double zsize;       ///< Z total width.

    // placement stuff
    std::string sequence;     ///< Name of sequence to place.
    std::string referenceElement; ///< Name of reference element w.r.t. to place to.
    int         referenceElementNumber; ///< Index of repetition of element if there are multiple uses.
    double s; ///< Curvilinear s position to place w.r.t..
    double x; ///< Offset in x.
    double y; ///< Offset in y.
    double z; ///< Offset in z.
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
    bool   axisAngle;     ///< Flag to use the axis angle construction of rotation.
    
    /// constructor
    ScorerMesh();
    /// reset
    void clear();
    /// print some properties
    void print()const;
    /// set methods by property name and value
    template <typename T>
    void set_value(std::string property, T value);

  private:
    /// publish members
    void PublishMembers();
  };
  
  template <typename T>
  void ScorerMesh::set_value(std::string property, T value)
    {
#ifdef BDSDEBUG
      std::cout << "scorermesh> Setting value " << std::setw(25) << std::left
		<< property << value << std::endl;
#endif
      // member method can throw runtime_error, catch and exit gracefully
      try
	{set(this,property,value);}
      catch (const std::runtime_error&)
	{
	  std::cerr << "Error: scorermesh> unknown option \"" << property
		    << "\" with value " << value  << std::endl;
	  exit(1);
	}
    }
}

#endif
