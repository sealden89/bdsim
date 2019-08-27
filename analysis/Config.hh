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
#ifndef CONFIG_H
#define CONFIG_H

#include "Rtypes.h" // for classdef

#include "RebdsimTypes.hh"

#include <string>
#include <vector>
#include <map>

class HistogramDef;

/**
 * @brief Configuration and configuration parser class.
 * 
 * @author Stewart Boogert
 */

class Config
{
  /// Private members first as required in accessors.
private:
  /// Optional version of option keys in file.
  std::map<std::string, std::string> alternateKeys;

  /// @{ Storage of options.
  std::map<std::string, bool>        optionsBool;
  std::map<std::string, std::string> optionsString;
  std::map<std::string, double>      optionsNumber;
  /// @}

  /// Storage of histogram options. This owns th HistogramDef objects.
  std::map<std::string, std::vector<HistogramDef*> > histoDefs;

  /// Copy of defintion used to identify only 'simple' histogram definitions. Doesn't own.
  std::map<std::string, std::vector<HistogramDef*> > histoDefsSimple;

  /// Copy of defintion used to identify only 'per entry' histogram definitions. Doesn't own.
  std::map<std::string, std::vector<HistogramDef*> > histoDefsPerEntry;
  
public:
  virtual ~Config();

  /// Singleton accessor
  static Config* Instance(std::string fileName = "",
			  std::string inputFilePath = "",
			  std::string outputFileName = "");

  void ParseInputFile();

  /// @{ General accessor for option.
  inline std::string GetOptionString(std::string key) const {return optionsString.at(key);}
  inline bool        GetOptionBool(std::string key)   const {return optionsBool.at(key);}
  inline double      GetOptionNumber(std::string key) const {return optionsNumber.at(key);}
  /// @}

  /// Access all histogram definitions.
  inline const std::vector<HistogramDef*>& HistogramDefinitions(std::string treeName) const
  {return histoDefs.at(treeName);}

  /// Access all simple histogram definitions - throws exception if out of range.
  inline const std::vector<HistogramDef*>& HistogramDefinitionsSimple(std::string treeName) const
  {return histoDefsSimple.at(treeName);}

  /// Access all per entry histogram definitions - throws exception if out of range.
  inline const std::vector<HistogramDef*>& HistogramDefinitionsPerEntry(std::string treeName) const
  {return histoDefsPerEntry.at(treeName);}

  /// Access all branches that are required for activation. This does not specialise on the
  /// leaf inside the branch and if one variable is required, the whole branch will be activated
  /// as there isn't much difference.  This can of course be revised in future.
  const RBDS::VectorString& BranchesToBeActivated(std::string treeName) const
  {return branches.at(treeName);}

  /// Access the map of all branches to be activated per tree.
  inline const RBDS::BranchMap& BranchesToBeActivated() const {return branches;}

  /// Boolean whether all branches should be turned on irrespective of map of
  /// individual branches to turn on.
  inline bool AllBranchesToBeActivated() const {return allBranchesActivated;}

  /// Set a branch to be activated if not already.
  void SetBranchToBeActivated(std::string treeName, std::string branchName);

  /// @{ Accessor.
  inline std::string InputFilePath() const             {return optionsString.at("inputfilepath");}
  inline std::string OutputFileName() const            {return optionsString.at("outputfilename");}
  inline std::string CalculateOpticalFunctionsFileName() const {return optionsString.at("opticslfilename");}
  inline bool   Debug() const                     {return optionsBool.at("debug");}
  inline bool   CalculateOpticalFunctions() const {return optionsBool.at("calculateoptics");}
  inline bool   ProcessSamplers() const           {return optionsBool.at("processsamplers");}
  inline double PrintModuloFraction() const       {return optionsNumber.at("printmodulofraction");}
  /// @}
  /// @{ Whether per entry loading is needed. Alternative is only TTree->Draw().
  inline bool   PerEntryBeam()   const {return optionsBool.at("perentrybeam");}
  inline bool   PerEntryEvent()  const {return optionsBool.at("perentryevent");}
  inline bool   PerEntryRun()    const {return optionsBool.at("perentryrun");}
  inline bool   PerEntryOption() const {return optionsBool.at("perentryoption");}
  inline bool   PerEntryModel()  const {return optionsBool.at("perentrymodel");}
  /// @}
  
 protected:
  /// Private constructor for singleton pattern.
  Config() = delete;
  /// Constructor used when mergine only.
  Config(const std::string& inputFilePathIn,
	 const std::string& outputFileNameIn);
  /// Desired constructor, also private for singleton pattern.
  Config(std::string fileNameIn,
	 std::string inputFilePathIn,
	 std::string outputFileNameIn);

  /// Set defaults in member maps for all options so that the keys can
  /// always be accessed.
  void InitialiseOptions(std::string analysisFile);

  /// Parse a line beginning with histogram. Uses other functions if appropriately defined.
  void ParseHistogramLine(const std::string& line);

  /// Parse everything after the histogram declaration and check all parameters.
  void ParseHistogram(const std::string line, const int nDim);

  /// Check whether a histogram definition word contains the world 'simple' and
  /// if so, it's not a per-entry histogram.
  void ParsePerEntry(const std::string& name, bool& perEntry) const;

  /// Parse whether each dimension is log or linear.
  void ParseLog(const std::string& definition,
		bool& xLog,
		bool& yLog,
		bool& zLog) const;

  /// Update the vector of required branches for a particular tree to be
  /// activated for analysis. Note this is not required for simple histograms
  /// that will be used with TTree->Draw(). Only per-entry histograms require
  /// loading the data.
  void UpdateRequiredBranches(const HistogramDef* def);

  /// Update the vector of required branches for a particular tree to be
  /// activated for anlysis based on a single string defintion such as Primary.x.
  void UpdateRequiredBranches(const std::string treeName,
			      const std::string var);

  /// Check if the supplied tree name is one of the static member vector of
  /// allowed tree names.
  bool InvalidTreeName(const std::string& treeName) const;

  /// Check whether the tree name ends in a '.' or not and fix it (simple mistake.
  /// Then apply InvalidTreeName and throw std::string error if it's a problem.
  void CheckValidTreeName(std::string& treeName) const;

  /// Parse the bin substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBins(const std::string bins,
		 const int nDim,
		 int& xBins,
		 int& yBins,
		 int& zBins) const;

  /// Parse binning substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBinning(const std::string binning,
		    const int nDim,
		    double& xLow, double& xHigh,
		    double& yLow, double& yHigh,
		    double& zLow, double& zHigh) const;

  /// Parse a settings line in input file and appropriate update member map.
  void ParseSetting(const std::string& line);

  /// Return a lower case copy of a string.
  std::string LowerCase(const std::string& st) const;

  static Config* instance;

  /// Vector of permitted tree names.
  static std::vector<std::string> treeNames;

  /// Index of which line in the file we're on while parsing - for feedback.
  int lineCounter = 0;

  /// Cache of which branches need to be activated for this analysis.
  RBDS::BranchMap branches;

  /// Whether all branches will be activatd - ie for optics.
  bool allBranchesActivated;

  ClassDef(Config,1);
};

#endif
