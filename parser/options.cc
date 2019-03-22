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
#include "options.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

using namespace GMAD;

Options::Options():
  OptionsBase()
{
  PublishMembers();
}

Options::Options(const GMAD::OptionsBase& options):
  OptionsBase(options)
{
  PublishMembers();
}

double Options::get_value(std::string property_name)const{
  double value;
  try {
    value = get<double>(this,property_name);
  }
  catch (const std::runtime_error&) {
    try {
      // try int and convert
      value = (double)get<int>(this,property_name);
    }
    catch (const std::runtime_error&) {
      try {
	// try long and convert
	value = (double)get<long>(this,property_name);
      }
      catch (const std::runtime_error&) {
	std::cerr << "options.cc> Error: unknown property \"" << property_name << "\" (only works on numerical properties)" << std::endl;
	exit(1);
      }
    }
  }
  return value;
}

std::string Options::get_value_string(std::string property_name) const
{
  try {
    double value = get<double>(this, property_name);
    std::ostringstream strs;
    strs << value;
    return strs.str();
  }
  catch (...) {
    try {
      int value = get<int>(this, property_name);
      std::ostringstream strs;
      strs << value;
      return strs.str();
    }
    catch (...) {
      try {
	std::string value = get<std::string>(this, property_name);
	return value;
      }
      catch (...) {
	try {
	  bool value = get<bool>(this, property_name);
	  std::ostringstream strs;
	  strs << std::boolalpha << value;
	  return strs.str();
	}
	catch (...)
	  {std::cerr << "Error " << property_name << std::endl; exit(1);}
      }
    }
  }
}

void Options::Amalgamate(const Options& optionsIn, bool override)
{
  if (override)
    {
      for (auto const key : optionsIn.setKeys)
	{
	  try
	    {
          set(this, &optionsIn, key);
          setKeys.push_back(key);
        }
	  catch (const std::runtime_error&)
	    {
	      std::cerr << "Error: Amalgate unknown option \"" << key << "\"" << std::endl;
	      exit(1);
	    }
	}
    }
  else
    {// don't override - ie give preference to ones set in this instance
      for (auto const key : optionsIn.setKeys)
	{
	  auto const& ok = setKeys; // shortcut
	  auto result = std::find(ok.begin(), ok.end(), key);
	  if (result == ok.end())
	    {//it wasn't found so ok to copy
	      try
		{
          set(this, &optionsIn, key);
          setKeys.push_back(key);
        }
	      catch (const std::runtime_error&)
		{
		  std::cerr << "Error: Amalgate unknown option \"" << key << "\"" << std::endl;
		  exit(1);
		}
	    }
	}
    }
}

bool Options::HasBeenSet(std::string name) const
{
  auto result = std::find(setKeys.begin(), setKeys.end(), name);
  if (result == setKeys.end())
    {return false;}
  else
    {return true;}
}

void Options::PublishMembers()
{
  // executable options first
  publish("inputFileName",         &Options::inputFileName);
  publish("visMacroFileName",      &Options::visMacroFileName);
  publish("geant4MacroFileName",   &Options::geant4MacroFileName);
  publish("visDebug",              &Options::visDebug);
  publish("outputFileName",        &Options::outputFileName);
  publish("outputFormat",          &Options::outputFormat);
  publish("survey",                &Options::survey);
  publish("surveyFileName",        &Options::surveyFileName);
  publish("verbose",               &Options::verbose);
  publish("verboseEvent",          &Options::verboseEvent);
  publish("verboseStep",           &Options::verboseStep);
  publish("verboseEventNumber",    &Options::verboseEventNumber);
  publish("batch",                 &Options::batch);
  publish("verboseRunLevel",       &Options::verboseRunLevel);
  publish("verboseEventLevel",     &Options::verboseEventLevel);
  publish("verboseTrackingLevel",  &Options::verboseTrackingLevel);
  publish("verboseSteppingLevel",  &Options::verboseSteppingLevel);
  publish("circular",              &Options::circular);
  publish("seed",                  &Options::seed);
  publish("recreate",              &Options::recreate);
  publish("recreateFileName",      &Options::recreateFileName);
  publish("startFromEvent",        &Options::startFromEvent);
  publish("writeSeedState",        &Options::writeSeedState);
  publish("useASCIISeedState",     &Options::useASCIISeedState);
  publish("seedStateFileName",     &Options::seedStateFileName);
  publish("ngenerate",             &Options::nGenerate);
  publish("generatePrimariesOnly", &Options::generatePrimariesOnly);
  publish("exportGeometry",        &Options::exportGeometry);
  publish("exportType",            &Options::exportType);
  publish("exportFileName",        &Options::exportFileName);
  publish("bdsimPath",             &Options::bdsimPath);
  
  // physics
  publish("physicsList",       &Options::physicsList);
  publish("physicsVerbose",    &Options::physicsVerbose);
  publish("physicsEnergyLimitLow",          &Options::physicsEnergyLimitLow);
  publish("physicsEnergyLimitHigh",         &Options::physicsEnergyLimitHigh);
  publish("g4PhysicsUseBDSIMRangeCuts",     &Options::g4PhysicsUseBDSIMRangeCuts);
  publish("g4PhysicsUseBDSIMCutsAndLimits", &Options::g4PhysicsUseBDSIMCutsAndLimits);

  // reproducibility
  publish("eventOffset",       &Options::eventOffset);
  publish("recreateSeedState", &Options::recreateSeedState);

  publish("elossHistoBinWidth",&Options::elossHistoBinWidth);
  publish("defaultRangeCut",&Options::defaultRangeCut);
  publish("ffact",&Options::ffact);
  publish("bv",   &Options::ffact); // MadX naming

  publish("beamlineX",         &Options::beamlineX);
  publish("beamlineY",         &Options::beamlineY);
  publish("beamlineZ",         &Options::beamlineZ);
  publish("beamlinePhi",       &Options::beamlinePhi);
  publish("beamlineTheta",     &Options::beamlineTheta);
  publish("beamlinePsi",       &Options::beamlinePsi);
  publish("beamlineAxisX",     &Options::beamlineAxisX);
  publish("beamlineAxisY",     &Options::beamlineAxisY);
  publish("beamlineAxisZ",     &Options::beamlineAxisZ);
  publish("beamlineAngle",     &Options::beamlineAngle);
  publish("beamlineAxisAngle", &Options::beamlineAxisAngle);
  publish("beamlineS",         &Options::beamlineS);

  publish("checkOverlaps",     &Options::checkOverlaps);
  publish("eventNumberOffset", &Options::eventNumberOffset);
  publish("vacuumPressure",    &Options::vacuumPressure);
  publish("xsize",             &Options::xsize);
  publish("ysize",             &Options::ysize);
  
  // options which influence the geometry
  publish("magnetGeometryType",   &Options::magnetGeometryType);
  publish("outerMaterial",        &Options::outerMaterialName);
  publish("horizontalWidth",      &Options::horizontalWidth);
  publish("outerDiameter",        &Options::horizontalWidth); // for backwards compatability
  publish("boxSize",              &Options::horizontalWidth); // for backwards compatability
  publish("yokeFields",           &Options::yokeFields);
  publish("includeIronMagFields", &Options::yokeFields); // for backwards compatibility
  publish("includeFringeFields",  &Options::includeFringeFields);
  publish("beampipeRadius",       &Options::aper1);
  publish("beampipeThickness",    &Options::beampipeThickness);
  publish("apertureType",         &Options::apertureType);
  publish("aper1",                &Options::aper1);
  publish("aper2",                &Options::aper2);
  publish("aper3",                &Options::aper3);
  publish("aper4",                &Options::aper4);
  publish("beampipeMaterial",     &Options::beampipeMaterial);
  publish("ignoreLocalAperture",  &Options::ignoreLocalAperture);
  publish("vacuumMaterial",       &Options::vacMaterial);
  publish("emptyMaterial",        &Options::emptyMaterial);
  publish("worldMaterial",        &Options::worldMaterial);
  publish("worldGeometryFile",    &Options::worldGeometryFile);
  publish("importanceWorldGeometryFile",    &Options::importanceWorldGeometryFile);
  publish("importanceVolumeMap",  &Options::importanceVolumeMap);
  publish("worldVolumeMargin",    &Options::worldVolumeMargin);
  publish("dontSplitSBends",      &Options::dontSplitSBends);
  publish("thinElementLength",    &Options::thinElementLength);
  publish("hStyle",               &Options::hStyle);
  publish("vhRatio",              &Options::vhRatio);
  publish("coilWidthFraction",    &Options::coilWidthFraction);
  publish("coilHeightFraction",   &Options::coilHeightFraction);
  publish("ignoreLocalMagnetGeometry", &Options::ignoreLocalMagnetGeometry);

  publish("preprocessGDML",       &Options::preprocessGDML);
  publish("preprocessGDMLSchema", &Options::preprocessGDMLSchema);
  
  // tunnel options
  publish("buildTunnel",         &Options::buildTunnel);
  publish("buildTunnelStraight", &Options::buildTunnelStraight);
  publish("tunnelType",          &Options::tunnelType);
  publish("tunnelThickness",     &Options::tunnelThickness);
  publish("tunnelSoilThickness", &Options::tunnelSoilThickness);
  publish("tunnelMaterial",      &Options::tunnelMaterial);
  publish("soilMaterial",        &Options::soilMaterial);
  publish("buildTunnelFloor",    &Options::buildTunnelFloor);
  publish("tunnelFloorOffset",   &Options::tunnelFloorOffset);
  publish("tunnelAper1",         &Options::tunnelAper1);
  publish("tunnelAper2",         &Options::tunnelAper2);
  publish("tunnelRadius",        &Options::tunnelAper1); // for backwards compatability
  publish("tunnelVisible",       &Options::tunnelVisible);
  publish("showTunnel",          &Options::tunnelVisible); // for backwards compatability
  publish("tunnelOffsetX",       &Options::tunnelOffsetX);
  publish("tunnelOffsetY",       &Options::tunnelOffsetY);

  publish("removeTemporaryFiles", &Options::removeTemporaryFiles);

  publish("samplerDiameter",&Options::samplerDiameter);
  
  // options for beam loss monitor geometry
  publish("blmRad",    &Options::blmRad);
  publish("blmLength", &Options::blmLength);

  // physics processes
  publish("turnOnOpticalAbsorption",     &Options::turnOnOpticalAbsorption);
  publish("turnOnMieScattering",         &Options::turnOnMieScattering);
  publish("turnOnRayleighScattering",    &Options::turnOnRayleighScattering);
  publish("turnOnOpticalSurface",        &Options::turnOnOpticalSurface);
  publish("scintYieldFactor",            &Options::scintYieldFactor);
  publish("maximumPhotonsPerStep",       &Options::maximumPhotonsPerStep);
  publish("maximumBetaChangePerStep",    &Options::maximumBetaChangePerStep);
  publish("maximumTracksPerEvent",       &Options::maximumTracksPerEvent);
  publish("minimumKineticEnergy",        &Options::minimumKineticEnergy);
  publish("minimumKineticEnergyTunnel",  &Options::minimumKineticEnergyTunnel);
  publish("minimumRange",                &Options::minimumRange);
  
  publish("prodCutPhotons",              &Options::prodCutPhotons);
  publish("prodCutElectrons",            &Options::prodCutElectrons);
  publish("prodCutPositrons",            &Options::prodCutPositrons);
  publish("prodCutProtons",              &Options::prodCutProtons);
  publish("prodCutHadrons",              &Options::prodCutProtons); // backwards compatability
  publish("neutronTimeLimit",            &Options::neutronTimeLimit);
  publish("neutronKineticEnergyLimit",   &Options::neutronKineticEnergyLimit);
  publish("useLENDGammaNuclear",         &Options::useLENDGammaNuclear);
  publish("useElectroNuclear",           &Options::useElectroNuclear);
  publish("useMuonNuclear",              &Options::useMuonNuclear);
  publish("useGammaToMuMu",              &Options::useGammaToMuMu);
  publish("usePositronToMuMu",           &Options::usePositronToMuMu);
  publish("usePositronToHadrons",        &Options::usePositronToHadrons);
  publish("collimatorsAreInfiniteAbsorbers", &Options::collimatorsAreInfiniteAbsorbers);
  publish("tunnelIsInfiniteAbsorber",        &Options::tunnelIsInfiniteAbsorber);
  
  // bias options
  publish("defaultBiasVacuum",   &Options::defaultBiasVacuum);
  publish("defaultBiasMaterial", &Options::defaultBiasMaterial);

  // options which influence tracking
  publish("integratorSet",            &Options::integratorSet);
  publish("lengthSafety",             &Options::lengthSafety);
  publish("lengthSafetyLarge",        &Options::lengthSafetyLarge);
  publish("maximumTrackingTime",      &Options::maximumTrackingTime);
  publish("maximumStepLength",        &Options::maximumStepLength);
  publish("maximumStepSize",          &Options::maximumStepLength);
  publish("maximumTrackLength",       &Options::maximumTrackLength);
  publish("chordStepMinimum",         &Options::chordStepMinimum);
  publish("chordStepMinimumYoke",     &Options::chordStepMinimumYoke);
  publish("deltaIntersection",        &Options::deltaIntersection);
  publish("minimumEpsilonStep",       &Options::minimumEpsilonStep);
  publish("maximumEpsilonStep",       &Options::maximumEpsilonStep);
  publish("deltaOneStep",             &Options::deltaOneStep);
  publish("stopSecondaries",          &Options::stopSecondaries);
  publish("killNeutrinos",            &Options::killNeutrinos);
  publish("minimumRadiusOfCurvature", &Options::minimumRadiusOfCurvature);
  publish("sampleElementsWithPoleface",  &Options::sampleElementsWithPoleface);
  publish("nominalMatrixRelativeMomCut", &Options::nominalMatrixRelativeMomCut);
  publish("teleporterFullTransform",  &Options::teleporterFullTransform);

  // hit generation
  publish("sensitiveOuter",              &Options::sensitiveOuter);
  publish("sensitiveBeamlineComponents", &Options::sensitiveOuter); // backwards compatibility
  publish("sensitiveBeamPipe",           &Options::sensitiveBeamPipe);
  publish("sensitiveBeampipe",           &Options::sensitiveBeamPipe);
  publish("sensitiveTunnel",             &Options::storeElossTunnel);
  publish("tunnelSensitive",             &Options::storeElossTunnel);// backwards compatibility
  publish("sensitiveBLMs",               &Options::sensitiveBLMs);
  
  // output
  publish("nperfile",                       &Options::numberOfEventsPerNtuple);

  publish("storeCollimatorInfo",            &Options::storeCollimatorInfo);
  publish("storeCollimatorLinks",           &Options::storeCollimatorLinks);
  publish("storeCollimatorHitsIons",        &Options::storeCollimatorHitsIons);
  publish("storeCollimatorHitsAll",         &Options::storeCollimatorHitsAll);
  publish("storeEloss",                     &Options::storeEloss);
  publish("storeELoss",                     &Options::storeEloss);
  publish("storeElossHistograms",           &Options::storeElossHistograms);
  publish("storeELossHistograms",           &Options::storeElossHistograms);
  publish("storeElossVacuum",               &Options::storeElossVacuum);
  publish("storeELossVacuum",               &Options::storeElossVacuum);
  publish("storeElossVacuumHistograms",     &Options::storeElossVacuumHistograms);
  publish("storeELossVacuumHistograms",     &Options::storeElossVacuumHistograms);
  publish("storeElossTunnel",               &Options::storeElossTunnel);
  publish("storeELossTunnel",               &Options::storeElossTunnel);
  publish("storeElossTunnelHistograms",     &Options::storeElossTunnelHistograms);
  publish("storeELossTunnelHistograms",     &Options::storeElossTunnelHistograms);
  publish("storeElossWorld",                &Options::storeElossWorld);
  publish("storeELossWorld",                &Options::storeElossWorld);
  publish("storeElossWorldContents",        &Options::storeElossWorldContents);
  publish("storeELossWorldContents",        &Options::storeElossWorldContents);
  publish("storeElossTurn",                 &Options::storeElossTurn);
  publish("storeELossTurn",                 &Options::storeElossTurn);
  publish("storeElossLinks",                &Options::storeElossLinks);
  publish("storeELossLinks",                &Options::storeElossLinks);
  publish("storeElossLocal",                &Options::storeElossLocal);
  publish("storeELossLocal",                &Options::storeElossLocal);
  publish("storeElossGlobal",               &Options::storeElossGlobal);
  publish("storeELossGlobal",               &Options::storeElossGlobal);
  publish("storeElossTime",                 &Options::storeElossTime);
  publish("storeELossTime",                 &Options::storeElossTime);
  publish("storeElossStepLength",           &Options::storeElossStepLength);
  publish("storeELossStepLength",           &Options::storeElossStepLength);
  publish("storeElossPreStepKineticEnergy", &Options::storeElossPreStepKineticEnergy);
  publish("storeELossPreStepKineticEnergy", &Options::storeElossPreStepKineticEnergy);
  publish("storeElossModelID",              &Options::storeElossModelID);
  publish("storeELossModelID",              &Options::storeElossModelID);
  publish("storeGeant4Data",                &Options::storeGeant4Data);
  
  publish("storeTrajectory",                &Options::storeTrajectory);
  publish("storeTrajectories",              &Options::storeTrajectory);
  publish("storeTrajectoryDepth",           &Options::storeTrajectoryDepth);
  publish("storeTrajectoryParticle",        &Options::storeTrajectoryParticle);
  publish("storeTrajectoryParticleID",      &Options::storeTrajectoryParticleID);
  publish("storeTrajectoryEnergyThreshold", &Options::storeTrajectoryEnergyThreshold);
  publish("storeTrajectorySamplerID",       &Options::storeTrajectorySamplerID);
  publish("storeTrajectoryELossSRange",     &Options::storeTrajectoryELossSRange);

  publish("storeSamplerAll",                &Options::storeSamplerAll);
  publish("storeSamplerPolarCoords",        &Options::storeSamplerPolarCoords);
  publish("storeSamplerCharge",             &Options::storeSamplerCharge);
  publish("storeSamplerKineticEnergy",      &Options::storeSamplerKineticEnergy);
  publish("storeSamplerMass",               &Options::storeSamplerMass);
  publish("storeSamplerRigidity",           &Options::storeSamplerRigidity);
  publish("storeSamplerIon",                &Options::storeSamplerIon);

  publish("trajConnect",                    &Options::trajConnect);
  publish("trajCutGTZ",                     &Options::trajCutGTZ);
  publish("trajCutLTR",                     &Options::trajCutLTR);
  publish("trajNoTransportation",           &Options::trajNoTransportation);

  publish("writePrimaries",                 &Options::writePrimaries);
  publish("storeModel",                     &Options::storeModel);

  // circular options
  publish("nturns",                   &Options::nturns);
  publish("ptcOneTurnMapFileName",    &Options::ptcOneTurnMapFileName);

  publish("printModuloFraction",      &Options::printFractionEvents); // alternative name
  publish("printFractionEvents",      &Options::printFractionEvents);
  publish("printFractionTurns",       &Options::printFractionTurns);

  // visualisation
  publish("nSegmentsPerCircle",       &Options::nSegmentsPerCircle);

  // scoring map
  publish("nbinsx", &Options::nbinsx);
  publish("nbinsy", &Options::nbinsy);
  publish("nbinsz", &Options::nbinsz);
  publish("xmin",   &Options::xmin);
  publish("xmax",   &Options::xmax);
  publish("ymin",   &Options::ymin);
  publish("ymax",   &Options::ymax);
  publish("zmin",   &Options::zmin);
  publish("zmax",   &Options::zmax);
  publish("useScoringMap", &Options::useScoringMap);
}
