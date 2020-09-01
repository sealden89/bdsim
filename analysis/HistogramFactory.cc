/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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

#include <string>
#include <vector>

ClassImp(HistogramFactory)

HistogramFactory::HistogramFactory()
{;}

HistogramFactory::~HistogramFactory()
{;}

TH1* HistogramFactory::CreateHistogram(const HistogramDef* definition,
				       const std::string&  overRideName,
				       const std::string&  overRideTitle)
{
  TH1* result = nullptr;
  const int nDimensions = definition->nDimensions;
  switch (nDimensions)
    {
    case 1:
      {
	const HistogramDef1D* d = dynamic_cast<const HistogramDef1D*>(definition);
	result = CreateHistogram1D(d, overRideName, overRideTitle);
	break;
      }
    case 2:
      {
	const HistogramDef2D* d = dynamic_cast<const HistogramDef2D*>(definition);
	result = CreateHistogram2D(d, overRideName, overRideTitle);
	break;
      }
    case 3:
      {
	const HistogramDef3D* d = dynamic_cast<const HistogramDef3D*>(definition);
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
  if (!overRideName.empty())
    {name = overRideName;}
  if (!overRideTitle.empty())
    {title = overRideTitle;}
}

TH1D* HistogramFactory::CreateHistogram1D(const HistogramDef1D* d,
					  const std::string& overRideName,
					  const std::string& overRideTitle)
{
  TH1D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);
  const BinSpecification& xbs = d->xBinning;
  if (xbs.edges)
    {result = new TH1D(name.c_str(), title.c_str(), xbs.n, xbs.edges->data());}
  else
    {result = new TH1D(name.c_str(), title.c_str(), xbs.n, xbs.low, xbs.high);}
  return result;
}

TH2D* HistogramFactory::CreateHistogram2D(const HistogramDef2D* d,
					  const std::string& overRideName,
					  const std::string& overRideTitle)
{
  TH2D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);

  const BinSpecification& xbs = d->xBinning;
  const BinSpecification& ybs = d->yBinning;
  if (xbs.edges && ybs.edges)
    {
      result = new TH2D(name.c_str(), title.c_str(),
			xbs.n, xbs.edges->data(),
			ybs.n, ybs.edges->data());
    }
  else if (xbs.edges)
    {
      result = new TH2D(name.c_str(), title.c_str(),
			xbs.n, xbs.edges->data(),
			ybs.n, ybs.low, ybs.high);
    }
  else if (ybs.edges)
    {
      result = new TH2D(name.c_str(), title.c_str(),
			xbs.n, xbs.low, xbs.high,
			ybs.n, ybs.edges->data());
    }
  else
    {
      result = new TH2D(name.c_str(), title.c_str(),
			xbs.n, xbs.low, xbs.high,
			ybs.n, ybs.low, ybs.high);
    }
  return result;
}

TH3D* HistogramFactory::CreateHistogram3D(const HistogramDef3D* d,
					  const std::string& overRideName,
					  const std::string& overRideTitle)
{
  TH3D* result = nullptr;
  std::string name  = d->histName;
  std::string title = name;
  CheckNameAndTitle(name, title, overRideName, overRideTitle);

  const BinSpecification& xbs = d->xBinning;
  const BinSpecification& ybs = d->yBinning;
  const BinSpecification& zbs = d->zBinning;
  
  if (xbs.edges || ybs.edges || zbs.edges)
    {
      std::vector<double> binEdgesX = xbs.edges ? *(xbs.edges) : RBDS::LinSpace(xbs.low, xbs.high, xbs.n);
      std::vector<double> binEdgesY = ybs.edges ? *(ybs.edges) : RBDS::LinSpace(ybs.low, ybs.high, ybs.n);
      std::vector<double> binEdgesZ = zbs.edges ? *(zbs.edges) : RBDS::LinSpace(zbs.low, zbs.high, zbs.n);
      result = new TH3D(name.c_str(), title.c_str(),
			xbs.n, binEdgesX.data(),
			ybs.n, binEdgesY.data(),
			zbs.n, binEdgesZ.data());
    }
  else
    {
      result = new TH3D(name.c_str(), title.c_str(),
			xbs.n, xbs.low, xbs.high,
			ybs.n, ybs.low, ybs.high,
			zbs.n, zbs.low, zbs.high);
    }
  return result;
}
      
