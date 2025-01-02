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
#ifndef BDSCOLOURMAP_H
#define BDSCOLOURMAP_H

#include "G4String.hh"
#include "G4VisAttributes.hh"

#include <iterator>
#include <map>
#include <utility>

class G4Colour;

/**
 * @brief Map of colour and drawing style.
 *
 * Has the mechanics to upgrade a map of just name to colour, to
 * also include a default drawing style. This does not own the
 * G4Colour instances. These are created through BDSColours which
 * is a singleton and managed there.
 *
 * @author Laurie Nevay
 */

class BDSColourMap
{
public:
  typedef std::pair<G4Colour*, G4VisAttributes::ForcedDrawingStyle> Value;

  BDSColourMap();
  BDSColourMap(const std::map<G4String, G4Colour*>& mapIn);
  ~BDSColourMap();

  /// G4VisAttributes has no method to set the style directly but instead
  /// a function for each style. Switch on the style and call the right function.
  static void SetForceDrawStyle(G4VisAttributes* vis,
                                G4VisAttributes::ForcedDrawingStyle style);

  /// Merge a map of string : colour into string : (colour, style).
  void MergeSimpleMap(const std::map<G4String, G4Colour*>* mapIn,
                      G4VisAttributes::ForcedDrawingStyle defaultStyle = G4VisAttributes::ForcedDrawingStyle::solid);

  /// Merge a map of string : colour into string : (colour, style).
  void MergeSimpleMap(const std::map<G4String, G4Colour*>& mapIn,
                      G4VisAttributes::ForcedDrawingStyle defaultStyle = G4VisAttributes::ForcedDrawingStyle::solid);

  /// Non-const access / set a single element.
  Value& operator[](const G4String& key) {return data[key];}

  /// Const access a single element.
  const Value& operator[](const G4String& key) const {return data.at(key);}

  ///@{ Iterator mechanics
  typedef std::map<G4String, Value>::iterator iterator;
  typedef std::map<G4String, Value>::const_iterator const_iterator;
  iterator               begin()        {return data.begin();}
  iterator               end()          {return data.end();}
  const_iterator         begin()  const {return data.begin();}
  const_iterator         end()    const {return data.end();}
  G4bool                 empty()  const {return data.empty();}
  size_t                 size()   const {return data.size();}
  ///@}

private:
  std::map<G4String, Value> data;
};

#endif
