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
#include "BinGeneration.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "HistogramFactory.hh"

#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TROOT.h"

#include <string>
#include <vector>

ClassImp(HistogramFactory)

HistogramFactory::HistogramFactory()
{;}

HistogramFactory::~HistogramFactory()
{;}

TH1* HistogramFactory::CreateHistogram(const HistogramDef* definition,
				       std::string overRideName,
				       std::string overRideTitle)
{
  TH1* result = nullptr;
  const int nDimensions = definition->nDimensions;
  switch (nDimensions)
    {
    case 1:
      {
	const HistogramDef1D* d = static_cast<const HistogramDef1D*>(definition);
	result = CreateHistogram1D(d, overRideName, overRideTitle);
	break;
      }
    case 2:
      {
	const HistogramDef2D* d = static_cast<const HistogramDef2D*>(definition);
	result = CreateHistogram2D(d, overRideName, overRideTitle);
	break;
      }
    case 3:
      {
	const HistogramDef3D* d = static_cast<const HistogramDef3D*>(definition);
	result = CreateHistogram3D(d, overRideName, overRideTitle);
	break;
      }
    default:
      {break;}
    }
  return result;
}

void HistogramFactory::CheckNameAndTitle(std::string& name,
					 std::string& title,
					 const std::string& overRideName,
					 const std::string& overRideTitle)
{
  if (overRideName != "")
    {name = overRideName;}
  if (overRideTitle != "")
    {title = overRideTitle;}
}

TH1D* HistogramFactory::CreateHistogram1D(const HistogramDef1D* d,
					  std::string overRideName,
					  std::string overRideTitle)
{
  TH1D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);
  
  if (d->logarithmicX)
    {// note ROOT requires len(binEdges) = nBins + 1
      std::vector<double> binEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
      result = new TH1D(name.c_str(), title.c_str(), d->xNBins, binEdges.data());
    }
  else
    {
      result = new TH1D(name.c_str(), title.c_str(),
			d->xNBins, d->xLow, d->xHigh);
    }
  return result;
}

TH2D* HistogramFactory::CreateHistogram2D(const HistogramDef2D* d,
					  std::string overRideName,
					  std::string overRideTitle)
{
  TH2D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);
  
  if (d->logarithmicX && d->logarithmicY)
    {
      std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
      std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
      result = new TH2D(name.c_str(), title.c_str(),
			d->xNBins, xBinEdges.data(),
			d->yNBins, yBinEdges.data());
    }
  else if (d->logarithmicX)
    {
      std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
      result = new TH2D(name.c_str(), title.c_str(),
			d->xNBins, xBinEdges.data(),
			d->yNBins, d->yLow, d->yHigh);
    }
  else if (d->logarithmicY)
    {
      std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
      result = new TH2D(name.c_str(), title.c_str(),
			d->xNBins, d->xLow, d->xHigh,
			d->yNBins, yBinEdges.data());
    }
  else
    {
      result = new TH2D(name.c_str(), title.c_str(),
			d->xNBins, d->xLow, d->xHigh,
			d->yNBins, d->yLow, d->yHigh);
    }
  return result;
}

TH3D* HistogramFactory::CreateHistogram3D(const HistogramDef3D* d,
					  std::string overRideName,
					  std::string overRideTitle)
{
  TH3D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);
  
  if (d->logarithmicX || d->logarithmicY || d->logarithmicZ)
    {
      std::vector<double> xBinEdges;
      std::vector<double> yBinEdges;
      std::vector<double> zBinEdges;
      if (d->logarithmicX)
	{xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);}
      else
	{xBinEdges = RBDS::LinSpace(d->xLow, d->xHigh, d->xNBins);}
      if (d->logarithmicY)
	{yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);}
      else
	{yBinEdges = RBDS::LinSpace(d->yLow, d->yHigh, d->yNBins);}
      if (d->logarithmicZ)
	{zBinEdges = RBDS::LogSpace(d->zLow, d->zHigh, d->zNBins);}
      else
	{zBinEdges = RBDS::LinSpace(d->zLow, d->zHigh, d->zNBins);}
      result = new TH3D(name.c_str(), title.c_str(),
			d->xNBins, xBinEdges.data(),
			d->yNBins, yBinEdges.data(),
			d->zNBins, zBinEdges.data());
    }
  else
    {
      result = new TH3D(name.c_str(), title.c_str(),
			d->xNBins, d->xLow, d->xHigh,
			d->yNBins, d->yLow, d->yHigh,
			d->zNBins, d->zLow, d->zHigh);
    }
  return result;
}
      
