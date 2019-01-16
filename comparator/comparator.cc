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
/**
 * @file comparator.cc
 */

#include "Compare.hh"

#include "analysis/FileMapper.hh"

#include <cmath>
#include <iostream>
#include <vector>

#include "TFile.h"
#include "TH1.h"
#include "TTree.h"

class Result;

void usage();

enum EXIT_CODE {
  _EXIT_SUCCESS        = 0,
  _EXIT_FAILED         = 1,
  _EXIT_INCORRECT_ARGS = 2,
  _EXIT_FILE_NOT_FOUND = 3,
  _EXIT_BAD_FILE       = 4
};

int main(int argc, char* argv[])
{
  if(argc != 3)
    { 
      usage();
      return EXIT_CODE::_EXIT_INCORRECT_ARGS;    
    }

  // try to open files - check validity
  TFile* f1 = new TFile(argv[1]);
  TFile* f2 = new TFile(argv[2]);
  if(f1->IsZombie())
    {
      std::cout << "error : could not open " << argv[1] << std::endl;    
      return EXIT_CODE::_EXIT_FILE_NOT_FOUND;
    }
  if(f2->IsZombie())
    {
      std::cout << "error : could not open " << argv[2] << std::endl;    
      return EXIT_CODE::_EXIT_FILE_NOT_FOUND;
    }

  std::vector<Result*> results = Compare::Files(f1,f2);

  bool allPassed = Compare::Summarise(results);
  if (!allPassed)
    {
      std::cout << "TESTS_FAILED" << std::endl; // key to look for
      return EXIT_CODE::_EXIT_FAILED;
    }
  else
    {return EXIT_CODE::_EXIT_SUCCESS;}
}

void usage()
{ 
  std::cout << "Usage: comparator <rootfile1> <rootfile2>" << std::endl;
  std::cout << "Compares <rootfile2> to <rootfile1> - ie <rootfile1> is the reference." << std::endl;
}
