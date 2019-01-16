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
#ifndef COMPCOMPARE_H
#define COMPCOMPARE_H

#include <vector>

#include "ResultEvent.hh"

#include "BDSOutputROOTEventSampler.hh"

class Result;

class TDirectory;
class TFile;
class TH1;
class TTree;

/**
 * @brief Comparison utility functions
 * @author Stewart Boogert
 */

namespace Compare
{
  /// Compare two files.
  std::vector<Result*> Files(TFile* f1, TFile* f2);

  /// Compare two directories by changing into d1 and inspecting all objects.
  /// The results are recorded via a reference to a results vector. The
  /// original directory is changed back to at the end of the method.
  void Directories(TDirectory* d1,
		   TDirectory* d2,
		   std::vector<Result*>& results);

  /// Compare two histogams.
  void Histograms(TH1* h1, TH1* h2, std::vector<Result*>& results);
  
  /// Compare two TTrees.
  void Trees(TTree* t1, TTree* t2, std::vector<Result*>& results);

  /// Compare an optics TTree specifically. This relies on the known variable
  /// names in the tree and naming scheme.
  void Optics(TTree* t1, TTree* t2, std::vector<Result*>& results);

  void EventTree(TTree* t1, TTree* t2, std::vector<Result*>& results,
		 std::vector<std::string> samplerNames);

#ifdef __ROOTDOUBLE__
  void Sampler(BDSOutputROOTEventSampler<double>* e1,
	       BDSOutputROOTEventSampler<double>* e2,
	       ResultEvent* results);
#else
  void Sampler(BDSOutputROOTEventSampler<float>* e1,
	       BDSOutputROOTEventSampler<float>* e2,
	       ResultEvent* results);
#endif
  
#ifdef __ROOTDOUBLE__
  bool Diff(const std::vector<double>& v1, const std::vector<double>& v2, int i);
#else
  bool Diff(const std::vector<float>& v1, const std::vector<float>& v2, int i);
#endif

#ifdef __ROOTDOUBLE__
  bool Diff(const double& v1, const double& v2);
#else
  bool Diff(const float& v1, const float& v2);
#endif

  /// Simply print out feedback warning that a matching object wasn't found and
  /// no comparison is being done.
  void PrintNoMatching(std::string className, std::string objectName);

  /// Loop over results and print any failed ones. Returns true if all passed.
  bool Summarise(std::vector<Result*> results);

  /// Check whether a string is prefixed with another string.
  bool StringStartsWith(std::string aString, std::string prefix);

  bool IsInVector(std::string key, const std::vector<std::string>& vec);

  inline bool NanOrInf(const double& val) {return std::isnan(val) | std::isinf(val);}

  inline bool GTEZero(const double& val) {return val >= 0;}
  inline bool LTZero(const double& val)  {return val < 0;}

  /// Bool for checking if primaries were written to file.
  bool hasPrimaries;
}
  
#endif
