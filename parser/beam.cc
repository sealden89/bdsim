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
#include "beam.h"

#include <algorithm>
#include <iostream>

using namespace GMAD;

Beam::Beam():
  BeamBase()
{
  PublishMembers();
}

Beam::Beam(const GMAD::BeamBase& options):
  BeamBase(options)
{
  PublishMembers();
}

double Beam::get_value(std::string property_name) const
{
  double value;
  try
    {value = get<double>(this,property_name);}
  catch (const std::runtime_error&)
    {
      try
	{value = (double)get<int>(this,property_name);}	// try int and convert
      catch (const std::runtime_error&)
	{
	  try
	    {value = (double)get<long>(this,property_name);} // try long and convert
	  catch (const std::runtime_error&)
	    {
	      std::cerr << "beam.cc> Error: unknown property \"" << property_name
			<< "\" (only works on numerical properties)" << std::endl;
	      exit(1);
	    }
	}
    }
  return value;
}

void Beam::Amalgamate(const Beam& beamIn, bool override, int startFromEvent)
{
  if (override)
    {
      for (auto const key : beamIn.setKeys)
	{
	  try
	    {
          set(this, &beamIn, key);
          setKeys.push_back(key);
        }
	  catch (const std::runtime_error&)
	    {
	      std::cerr << "Error: Amalgate unknown beam option \"" << key << "\"" << std::endl;
	      exit(1);
	    }
	}
      // if we're recreating from a file, still load external file but
      // advance to the event number
      nlinesIgnore += startFromEvent;
    }
  else
    {// don't override - ie give preference to ones set in this instance
      for (auto const key : beamIn.setKeys)
	{
	  auto const& ok = setKeys; // shortcut
	  auto result = std::find(ok.begin(), ok.end(), key);
	  if (result == ok.end())
	    {//it wasn't found so ok to copy
	      try
		{
		  set(this, &beamIn, key);
		  setKeys.push_back(key);
		}
	      catch (const std::runtime_error&)
		{
		  std::cerr << "Error: Amalgate unknown beam option \"" << key << "\""
			    << std::endl;
		  exit(1);
		}
	    }
	}
    }
}

bool Beam::HasBeenSet(std::string name) const
{
  auto result = std::find(setKeys.begin(), setKeys.end(), name);
  if (result == setKeys.end())
    {return false;}
  else
    {return true;}
}

void Beam::PublishMembers()
{
  publish("particle",             &Beam::particle);
  publish("particleName",         &Beam::particle);
  publish("beamParticle",         &Beam::beamParticleName);
  publish("beamParticleName",     &Beam::beamParticleName);
  publish("energy",               &Beam::beamEnergy);
  publish("distrType",            &Beam::distrType);
  publish("xDistrType",           &Beam::xDistrType);
  publish("yDistrType",           &Beam::yDistrType);
  publish("zDistrType",           &Beam::zDistrType);
  publish("distrFile",            &Beam::distrFile);
  publish("distrFileFormat",      &Beam::distrFileFormat);
  publish("matchDistrFileLength", &Beam::matchDistrFileLength);
  publish("nlinesIgnore",         &Beam::nlinesIgnore);
  publish("nLinesIgnore",         &Beam::nlinesIgnore); // for consistency
  publish("nlinesSkip",           &Beam::nlinesSkip);
  publish("nLinesSkip",           &Beam::nlinesSkip);   // for consistency

  // aliases
  publish("distribution",         &Beam::distrType);
  publish("xDistribution",        &Beam::xDistrType);
  publish("yDistribution",        &Beam::yDistrType);
  publish("zDistribution",        &Beam::zDistrType);

  // central values
  publish("X0",    &Beam::X0);
  publish("Y0",    &Beam::Y0);
  publish("Z0",    &Beam::Z0);
  publish("S0",    &Beam::S0);
  publish("Xp0",   &Beam::Xp0);
  publish("Yp0",   &Beam::Yp0);
  publish("Zp0",   &Beam::Zp0);
  publish("tilt",  &Beam::tilt);
  publish("T0",    &Beam::T0);
  publish("E0",    &Beam::E0);

  publish("sigmaT", &Beam::sigmaT);
  publish("sigmaE", &Beam::sigmaE);

  // for gausstwiss
  publish("betx",  &Beam::betx);
  publish("bety",  &Beam::bety);
  publish("alfx",  &Beam::alfx);
  publish("alfy",  &Beam::alfy);
  publish("emitx", &Beam::emitx);
  publish("emity", &Beam::emity);
  publish("dispx", &Beam::dispx);
  publish("dispy", &Beam::dispy);
  publish("dispxp",&Beam::dispxp);
  publish("dispyp",&Beam::dispyp);

  // aliases
  publish("betaX",  &Beam::betx);
  publish("betaY",  &Beam::bety);
  publish("alphaX", &Beam::alfx);
  publish("alphaY", &Beam::alfy);
  publish("emitX",  &Beam::emitx);
  publish("emitY",  &Beam::emity);
  publish("dispX",  &Beam::dispx);
  publish("dispXp", &Beam::dispxp);
  publish("dispY",  &Beam::dispy);
  publish("dispYp", &Beam::dispyp);
  
  // options for beam distrType="gauss"
  publish("sigmaX", &Beam::sigmaX);
  publish("sigmaXp",&Beam::sigmaXp);
  publish("sigmaY", &Beam::sigmaY);
  publish("sigmaYp",&Beam::sigmaYp);

  // options for beam distrType="square" or distrType="circle"
  publish("envelopeX", &Beam::envelopeX);
  publish("envelopeXp",&Beam::envelopeXp);
  publish("envelopeY", &Beam::envelopeY);
  publish("envelopeYp",&Beam::envelopeYp);
  publish("envelopeT", &Beam::envelopeT);
  publish("envelopeE", &Beam::envelopeE);
  publish("envelopeR", &Beam::envelopeR);
  publish("envelopeRp",&Beam::envelopeRp);

  // options for beam distrType="gaussmatrix"
  publish("sigma11",&Beam::sigma11);
  publish("sigma12",&Beam::sigma12);
  publish("sigma13",&Beam::sigma13);
  publish("sigma14",&Beam::sigma14);
  publish("sigma15",&Beam::sigma15);
  publish("sigma16",&Beam::sigma16);
  publish("sigma22",&Beam::sigma22);
  publish("sigma23",&Beam::sigma23);
  publish("sigma24",&Beam::sigma24);
  publish("sigma25",&Beam::sigma25);
  publish("sigma26",&Beam::sigma26);
  publish("sigma33",&Beam::sigma33);
  publish("sigma34",&Beam::sigma34);
  publish("sigma35",&Beam::sigma35);
  publish("sigma36",&Beam::sigma36);
  publish("sigma44",&Beam::sigma44);
  publish("sigma45",&Beam::sigma45);
  publish("sigma46",&Beam::sigma46);
  publish("sigma55",&Beam::sigma55);
  publish("sigma56",&Beam::sigma56);
  publish("sigma66",&Beam::sigma66);

  // options for beam distrType="eshell"
  publish("shellX",      &Beam::shellX);
  publish("shellXp",     &Beam::shellXp);
  publish("shellY",      &Beam::shellY);
  publish("shellYp",     &Beam::shellYp);
  publish("shellXWidth", &Beam::shellXWidth);
  publish("shellXpWidth",&Beam::shellXpWidth);
  publish("shellYWidth", &Beam::shellYWidth);
  publish("shellYpWidth",&Beam::shellYpWidth);

  // options for beam distrType="ring"
  publish("Rmin",&Beam::Rmin);
  publish("Rmax",&Beam::Rmax);

  // options for beam distrType="halo"
  publish("haloNSigmaXInner",      &Beam::haloNSigmaXInner);
  publish("haloNSigmaXOuter",      &Beam::haloNSigmaXOuter);
  publish("haloNSigmaYInner",      &Beam::haloNSigmaYInner);
  publish("haloNSigmaYOuter",      &Beam::haloNSigmaYOuter);
  publish("haloXCutInner",         &Beam::haloXCutInner);
  publish("haloYCutInner",         &Beam::haloYCutInner);
  publish("haloPSWeightParameter", &Beam::haloPSWeightParameter);
  publish("haloPSWeightFunction",  &Beam::haloPSWeightFunction);

  publish("offsetSampleMean",      &Beam::offsetSampleMean);
}
