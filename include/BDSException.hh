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
#ifndef BDSEXCEPTION_H
#define BDSEXCEPTION_H

#include <exception>
#include <string>

/**
 * @brief General exception with possible name of object and message.
 * 
 * Provide optional name of objec the message is associated with. This
 * Can be appended later with SetName() function. 
 *
 * @author Laurie Nevay
 */

class BDSException: public std::exception
{
public:
  BDSException(const std::string& messageIn):
    name(""),
    message(messageIn)
  {;}
  BDSException(const std::string& nameIn, const std::string& messageIn):
    name(nameIn),
    message(messageIn)
  {;}
  virtual ~BDSException(){;}

  /// Override message in std::exception.
  const char* what() const noexcept override {
    if (name.empty())
      {return message.c_str();}
    else
      {
	std::string m = name + " : " + message;
	return m.c_str();
      }
  }

  /// Allow setting of name later.
  void SetName(const std::string& nameIn) {name = nameIn;}
  void AppendToMessage(const std::string& messageIn) {message += " " + messageIn;}

  /// @{ Data to print.
  std::string name;
  std::string message;
  /// @}
};

#endif
