/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef EVENTANALYSIS_H
#define EVENTANALYSIS_H

#include <vector>

#include "Analysis.hh"
#include "TROOT.h"

class Event;
class SamplerAnalysis;
class TChain;
class TFile;

/**
 * @brief Event level analysis.
 *
 * @author Stewart Boogert
 */

class EventAnalysis: public Analysis
{
public:
  /// The default constructor is not intended for use and will not
  /// work for the purpose of analysis. It is required by the ROOT
  /// C++ reflection system in case you wish to save the object.
  EventAnalysis();
  
  /// Constructor intended for use to construct an event analysis object.
  EventAnalysis(Event*  eventIn,
		TChain* chain,
		bool    perEntryAnalysis    = true,
		bool    processSamplersIn   = true,
		bool    debugIn             = false,
		double  printModuloFraction = 0.01,
		bool    emittanceOnTheFlyIn = false);

  virtual ~EventAnalysis();

  /// Method which calls all other methods in order. Here we also test if we
  /// have to process the samplers on whether to call Process().
  virtual void Execute();

  /// Operate on each entry in the event tree.
  virtual void Process();

  /// Terminate each individual sampler analysis and append optical functions.
  virtual void Terminate();

  /// Write analysis including optical functions to an output file.
  virtual void Write(TFile* outputFileName);

protected:
  Event* event; ///< Event object that data loaded from the file will be loaded into.
  std::vector<SamplerAnalysis*> samplerAnalyses; ///< Holder for sampler analysis objects.
  std::vector<std::vector<std::vector<double> > > opticalFunctions; ///< Optical functions from all samplers.

private:
  /// Set how often to print out information about the event.
  void SetPrintModuloFraction(double fraction);

  /// Initialise each sampler analysis object in samplerAnalysis.
  void Initialise();

  /// Process each sampler analysis object.
  void ProcessSamplers(bool firstTime = false);

  int  printModulo;       ///< Cache of print modulo fraction
  bool processSamplers;   ///< Whether to process samplers.
  bool emittanceOnTheFly; ///< Whether to calculate emittance fresh at each sampler.
  
  ClassDef(EventAnalysis,1);
};

#endif
