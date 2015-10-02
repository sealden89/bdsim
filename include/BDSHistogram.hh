#ifndef BDSHISTOGRAM_H
#define BDSHISTOGRAM_H

#include "globals.hh"       // geant4 globals / types
#include <vector>
#include <utility>
#include <ostream>

#include "BDSBin.hh"

/**
 * @brief a 1d-histogram class
 * 
 * @author Laurie Nevay <Laurie.Nevay@rhul.ac.uk>
 */
class BDSHistogram1D
{
public:
  /// constructor for equidistant bins
  BDSHistogram1D(G4double xmin,
		 G4double xmax,
		 G4int    nbins,
		 G4String nameIn="histogram",
		 G4String titleIn="histogram",
		 G4String xlabelIn="",
		 G4String ylabelIn="");
  /// constructor for user-defined bin widths
  BDSHistogram1D(std::vector<G4double> binEdges,
		 G4String name="histogram",
		 G4String titleIn="histogram",
		 G4String xlabelIn="",
		 G4String ylabelIn="");
  ~BDSHistogram1D();
  
  void                                        Empty();
  /// Fill method single number
  void                                        Fill(G4double x, G4double weight=1.0);
  /// Fill over a range, weight get equally distributed over bin lengths
  void                                        Fill(std::pair<G4double,G4double> range, G4double weight=1.0);
  std::vector<BDSBin*>                        GetBins() const;
  std::vector<G4double>                       GetBinValues() const;
  std::vector<std::pair<G4double, G4double> > GetBinXMeansAndTotals() const;
  std::vector<G4double>                       GetBinLowerEdges() const;
  /// print value of all bins to cout
  void                                        PrintBins() const;
  std::pair<BDSBin*,BDSBin*>                  GetUnderOverFlowBins() const;
  std::pair<G4double,G4double>                GetUnderOverFlowBinValues() const;
  BDSBin*                                     GetUnderflowBin() const;
  BDSBin*                                     GetOverflowBin()  const;
  BDSBin*                                     GetFirstBin() const;
  BDSBin*                                     GetLastBin() const;
  G4String                                    GetName() const;
  G4String                                    GetTitle() const;
  G4String                                    GetXLabel() const;
  G4String                                    GetYLabel() const;
  size_t                                      GetNBins() const;
  G4int                                       GetNEntries() const;

  // iterators
  BDSBin* currentBin();
  void    first();
  G4bool  isLastBin();
  G4bool  isDone();
  void    next();
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist);
  
private:
  BDSHistogram1D();
  /// returns bin number in vector for value x
  /// smaller than 0 means underflow, larger or equal than nbins means overflow
  /// only accurate if equidistantBins = true, otherwise indication
  int GetBinNumber(G4double value)const;
  /// get Bin corresponding to value
  BDSBin* GetBin(G4double value)const;
  /// get Bin corresponding to binnumber
  BDSBin* GetBin(G4int binNumber)const;
  
  /// vector of bins
  std::vector<BDSBin*> bins;
  /// overflow bin
  BDSBin*  overflow;
  /// underflow bin
  BDSBin*  underflow;
  G4String name;
  G4String title;
  G4String xlabel;
  G4String ylabel;
  /// number of entries  
  G4int    entries;
  /// equidistant binning
  G4bool   equidistantBins;

  /// helper variables for cpu speedup
  G4double xmin;
  G4double xmax;
  G4double nbins;
  G4double binwidth;
  
  std::vector<BDSBin*>::const_iterator _iterBins;
};

#endif
