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
#ifndef BDSOUTPUTROOTEVENTHISTOGRAMS_H
#define BDSOUTPUTROOTEVENTHISTOGRAMS_H

#include <string>
#include <vector>

#ifndef __ROOTBUILD__
#include "globals.hh"
#endif

#include "Rtypes.h"
#include "TObject.h"

class TH1D;
class TH2D;
class TH3D;

/**
 * @brief Holder for a set of histograms to be stored.
 *
 * @author Stewart Boogert
 */

class BDSOutputROOTEventHistograms: public TObject
{
public:
  BDSOutputROOTEventHistograms();
  BDSOutputROOTEventHistograms(const BDSOutputROOTEventHistograms &h);
  BDSOutputROOTEventHistograms& operator=(const BDSOutputROOTEventHistograms&) = delete;
  virtual ~BDSOutputROOTEventHistograms();

  /// Interface function to create a 1D histogram using only standard types.
  int Create1DHistogramSTD(std::string name, std::string title,
			   int nbins, double xmin, double xmax);

#ifndef __ROOTBUILD__
  G4int Create1DHistogram(G4String name, G4String title,
                          G4int nbins, G4double xmin, G4double xmax);
  G4int Create1DHistogram(G4String name, G4String title,
                          std::vector<double>& edges);
  G4int Create2DHistogram(G4String name, G4String title,
                          G4int nxbins, G4double xmin, G4double xmax,
                          G4int nybins, G4double ymin, G4double ymax);
  G4int Create2DHistogram(G4String name, G4String title,
                          std::vector<double>& xedges,
                          std::vector<double>& yedges);
  G4int Create3DHistogram(G4String name, G4String title,
			  G4int nxbins, G4double xmin, G4double xmax,
			  G4int nybins, G4double ymin, G4double ymax,
			  G4int nzbins, G4double zmin, G4double zmax);
  G4int Create3DHistogram(G4String name, G4String title,
			  std::vector<double>& xedges,
			  std::vector<double>& yedges,
			  std::vector<double>& zedges);
  void Fill1DHistogram(G4int histoId, G4double value, G4double weight = 1.0);
  void Fill2DHistogram(G4int histoId, G4double xValue, G4double yValue, G4double weight = 1.0);
  void Fill3DHistogram(G4int histoId, G4double xValue, G4double yValue, G4double zValue, G4double weight = 1.0);
#endif
  void Flush();
  /// Copy (using the TH->Clone) method from another instance.
  void Fill(const BDSOutputROOTEventHistograms* rhs);

  /// Copy (without using the TH->Clone) method from another instance. (Quicker).
  void FillSimple(const BDSOutputROOTEventHistograms* rhs);

  std::vector<TH1D*>& Get1DHistograms() {return histograms1D;}
  std::vector<TH2D*>& Get2DHistograms() {return histograms2D;}
  std::vector<TH3D*>& Get3DHistograms() {return histograms3D;}

  TH1D* Get1DHistogram(int iHisto) const {return histograms1D[iHisto];}
  TH2D* Get2DHistogram(int iHisto) const {return histograms2D[iHisto];}
  TH3D* Get3DHistogram(int iHisto) const {return histograms3D[iHisto];}

  /// TBC
  void Add(BDSOutputROOTEventHistograms* h);

private:
  std::vector<TH1D*> histograms1D;
  std::vector<TH2D*> histograms2D;
  std::vector<TH3D*> histograms3D;

  ClassDef(BDSOutputROOTEventHistograms,3);
};

#endif
