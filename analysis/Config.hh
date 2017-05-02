#ifndef CONFIG_H
#define CONFIG_H

#include "TROOT.h"

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

  /// Storage of histogram options.
  std::map<std::string, std::vector<HistogramDef*> > histoDefs;
  
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

  inline std::vector<HistogramDef*> HistogramDefinitions(std::string treeName) const {return histoDefs.at(treeName);}

  /// @{ Accessor.
  inline std::string InputFilePath() const             {return optionsString.at("inputfilepath");}
  inline std::string OutputFileName() const            {return optionsString.at("outputfilename");}
  inline std::string CalculateOpticalFunctionsFileName() const {return optionsString.at("opticslfilename");}
  inline bool   Debug() const                     {return optionsBool.at("debug");}
  inline bool   CalculateOpticalFunctions() const {return optionsBool.at("calculateoptics");}
  inline bool   ProcessSamplers() const           {return optionsBool.at("processsamplers");}
  inline bool   ProcessLosses() const             {return optionsBool.at("processlosses");}
  inline bool   ProcessAllTrees() const           {return optionsBool.at("processalltrees");}
  inline double PrintModuloFraction() const       {return optionsNumber.at("printmodulofraction");}
  /// @}
  
 protected:
  /// Private constructor for singleton pattern.
  Config();
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

  /// Check if the supplied tree name is one of the static member vector of
  /// allowed tree names.
  bool InvalidTreeName(const std::string& treeName) const;

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

  ClassDef(Config,1);
};

#endif
