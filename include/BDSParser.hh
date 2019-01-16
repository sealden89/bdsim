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
#ifndef BDSPARSER_H
#define BDSPARSER_H

#include <string>
#include <vector>

#include "parser/parser.h"

/**
 * @brief Const entry to parser objects
 *
 * Singleton pattern
 *
 * @author Jochem Snuverink
 */
  
class BDSParser: private GMAD::Parser
{
public:
  /// No default constructor.
  BDSParser() = delete;
  /// Constructor method.
  static BDSParser* Instance(std::string filename);
  /// Access method.
  static BDSParser* Instance();
  /// Returns if parser is initialised.
  static bool IsInitialised();
  /// Destructor.
  virtual ~BDSParser();
  
  /// Return options
  const GMAD::Options& GetOptions() const {return options;}

  /// Return bare options base class.
  const GMAD::OptionsBase* GetOptionsBase() const {return dynamic_cast<const GMAD::OptionsBase*>(&options);}

  /// Return beam.
  const GMAD::Beam& GetBeam() const {return beam;}

  /// Return beam non-const. Required when loading a file and need to change beam parameters to match.
  GMAD::Beam& GetBeam() {return beam;}

  /// Return bare beam base class.
  const GMAD::BeamBase* GetBeamBase() const {return dynamic_cast<const GMAD::BeamBase*>(&beam);}

  /// Amalgamate the input options with the ones stored in the parser.
  void AmalgamateOptions(const GMAD::Options& optionsIn);
  /// Amalgamate the input beam definition with the ones stored in teh parser.
  void AmalgamateBeam(const GMAD::Beam& beamIn, bool recreate);
  /// Check options for consistency. This also checks the beam options.
  void CheckOptions();

  /// Return the vector of region objects.
  inline const std::vector<GMAD::Atom>& GetAtoms() const {return atom_list;}
  
  /// Return beamline.
  inline const GMAD::FastList<GMAD::Element>& GetBeamline() const {return beamline_list;}

  /// Return sequence.
  inline const GMAD::FastList<GMAD::Element>& GetSequence(std::string name) {return get_sequence(name);}
  
  /// Return biasing list.
  inline const GMAD::FastList<GMAD::PhysicsBiasing>& GetBiasing() const {return xsecbias_list;}

  /// Return cavity model list.
  inline const std::vector<GMAD::CavityModel>& GetCavityModels() const {return cavitymodel_list;}

  /// Return colour model list.
  inline const std::vector<GMAD::NewColour>& GetColours() const {return colour_list;}

  /// Return crystal model list.
  inline const std::vector<GMAD::Crystal>& GetCrystals() const {return crystal_list;}

  /// Return the vector of field objects.
  inline const std::vector<GMAD::Field>& GetFields() const {return field_list;}

  /// Return material list.
  inline const std::vector<GMAD::Material>& GetMaterials() const {return material_list;}
  
  /// Return the vector of placement objects.
  inline const std::vector<GMAD::Placement>& GetPlacements() const {return placement_list;}

  /// Query list.
  inline const std::vector<GMAD::Query>& GetQuery() const {return query_list;}
  
  /// Return region list.
  inline const std::vector<GMAD::Region>& GetRegions() const {return region_list;}

protected:
  /// Constructor from filename.
  explicit BDSParser(std::string filename);

private:
  /// Instance.
  static BDSParser* instance;
};

#endif
