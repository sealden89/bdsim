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
#include "BDSAcceleratorComponent.hh"
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSAcceleratorModel.hh"
#include "BDSApertureInfo.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineBLMBuilder.hh"
#include "BDSBeamlineEndPieceBuilder.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamlinePlacementBuilder.hh"
#include "BDSBeamlineSet.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBOptrMultiParticleChangeCrossSection.hh"
#include "BDSComponentFactory.hh"
#include "BDSComponentFactoryUser.hh"
#include "BDSCurvilinearBuilder.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"
#include "BDSException.hh"
#include "BDSExtent.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldObjects.hh"
#include "BDSGap.hh"
#include "BDSGeometryComponent.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSIntegratorSet.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSPhysicalVolumeInfo.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSRegion.hh"
#include "BDSSamplerType.hh"
#include "BDSSDEnergyDeposition.hh"
#include "BDSSDManager.hh"
#include "BDSSDType.hh"
#include "BDSSurvey.hh"
#include "BDSTeleporter.hh"
#include "BDSTunnelBuilder.hh"

#include "parser/blmplacement.h"
#include "parser/element.h"
#include "parser/fastlist.h"
#include "parser/options.h"
#include "parser/physicsbiasing.h"
#include "parser/placement.h"
#include "parser/samplerplacement.h"

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4Navigator.hh"
#include "G4ProductionCuts.hh"
#include "G4PVPlacement.hh"
#include "G4Region.hh"
#include "G4Transform3D.hh"
#include "G4Version.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#if G4VERSION_NUMBER > 1039
#include "G4ChannelingOptrMultiParticleChangeCrossSection.hh"
#endif

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Vector/EulerAngles.h"

#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <vector>

BDSDetectorConstruction::BDSDetectorConstruction(BDSComponentFactoryUser* userComponentFactoryIn):
  placementBL(nullptr),
  designParticle(nullptr),
  userComponentFactory(userComponentFactoryIn),
  nSamplers(0)
{
  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verbose       = globals->Verbose();
  checkOverlaps = globals->CheckOverlaps();
  circular      = globals->Circular();
  
  // instantiate the accelerator model holding class
  acceleratorModel = BDSAcceleratorModel::Instance();
  canSampleAngledFaces = true;
  BDSIntegratorSetType integratorSetType = globals->IntegratorSet();
  if (   (integratorSetType == BDSIntegratorSetType::bdsimtwo)
      || (integratorSetType == BDSIntegratorSetType::geant4)
#if G4VERSION_NUMBER > 1039
      || (integratorSetType == BDSIntegratorSetType::geant4dp)
#endif
      )
    { // set to be value of option, default is false.
      canSampleAngledFaces = globals->SampleElementsWithPoleface();
    }

  UpdateSamplerDiameterAndCountSamplers();
}

void BDSDetectorConstruction::UpdateSamplerDiameterAndCountSamplers()
{
  nSamplers = 0;
  auto beamline = BDSParser::Instance()->GetBeamline(); // main beam line
  G4double maxBendingRatio = 1e-9;
  for (auto elementIt = beamline.begin(); elementIt != beamline.end(); ++elementIt)
    {
      // count number of samplers
      auto st = BDS::DetermineSamplerType((*elementIt).samplerType);
      if (st != BDSSamplerType::none)
        {nSamplers++;}

      G4double length = elementIt->l;
      G4double angle  = elementIt->angle;
      if (!BDS::IsFinite(length))
	{continue;} // avoid divide by 0
      G4double ratio  = angle / length;
      maxBendingRatio = std::max(maxBendingRatio, ratio);
    }
  
  G4double curvilinearRadius = BDSGlobalConstants::Instance()->SamplerDiameter()*0.5;
  if (maxBendingRatio > 0.4) // max ratio for a 2.5m sampler diameter
    {
      G4double curvilinearRadiusBends = (0.9 / maxBendingRatio)*CLHEP::m; // 90% of theoretical maximum radius

      // check it's smaller - the user may have already specified a smaller sampler diameter
      // and htat should take precedence
      if (curvilinearRadiusBends < curvilinearRadius)
	{
	  curvilinearRadius = curvilinearRadiusBends;
	  G4cout << __METHOD_NAME__ << "Reducing sampler diameter from "
		 << BDSGlobalConstants::Instance()->SamplerDiameter()/CLHEP::m << "m to "
		 << 2*curvilinearRadius/CLHEP::m << "m" << G4endl;
	  BDSGlobalConstants::Instance()->SetSamplerDiameter(curvilinearRadius);
	}
    }

    // add number of sampler placements to count of samplers
    nSamplers += (G4int)BDSParser::Instance()->GetSamplerPlacements().size();
}

G4VPhysicalVolume* BDSDetectorConstruction::Construct()
{
  if (verbose || debug)
    {G4cout << __METHOD_NAME__ << "starting accelerator geometry construction\n" << G4endl;}
  
  // construct all parser defined regions
  InitialiseRegions();

  // construct all parser defined aperture objects
  InitialiseApertures();
  
  // construct the main beam line and any other secondary beam lines
  BuildBeamlines();

  // construct placement geometry from parser
  BDSBeamline* mainBeamLine = BDSAcceleratorModel::Instance()->BeamlineSetMain().massWorld;
  placementBL = BDS::BuildPlacementGeometry(BDSParser::Instance()->GetPlacements(),
					    mainBeamLine);
  BDSAcceleratorModel::Instance()->RegisterPlacementBeamline(placementBL); // Acc model owns it

  BDSBeamline* blms = BDS::BuildBLMs(BDSParser::Instance()->GetBLMs(),
				     mainBeamLine);
  if (blms)
    {BDSAcceleratorModel::Instance()->RegisterBLMs(blms);} // Acc model owns it
  
  // build the tunnel and supports
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {BuildTunnel();}

  // build world and calculate coordinates
  auto worldPV = BuildWorld();

  // placement procedure - put everything in the world
  ComponentPlacement(worldPV);
  
  if (verbose || debug)
    {G4cout << __METHOD_NAME__ << "detector Construction done" << G4endl;}

#ifdef BDSDEBUG
  G4cout << G4endl << __METHOD_NAME__ << "printing material table" << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl << G4endl;
  if (verbose || debug) {G4cout << "Finished listing materials, returning physiWorld" << G4endl;} 
#endif
  return worldPV;
}

BDSDetectorConstruction::~BDSDetectorConstruction()
{
#if G4VERSION_NUMBER > 1009
  // delete bias objects
  for (auto i : biasObjects)
    {delete i;}
#endif
}

void BDSDetectorConstruction::InitialiseRegions()
{
  BDSRegion* defaultRegion = new BDSRegion("default");
  for (const GMAD::Region& r : BDSParser::Instance()->GetRegions())
    {
      BDSRegion* reg = new BDSRegion(r, defaultRegion);
      G4cout << "New region defined: " << G4endl << *reg << G4endl;
      acceleratorModel->RegisterRegion(reg);
    }
  delete defaultRegion;
}

void BDSDetectorConstruction::InitialiseApertures()
{
  std::map<G4String, BDSApertureInfo*> apertures;
  for (const GMAD::Aperture& a : BDSParser::Instance()->GetApertures())
    {
      BDSApertureInfo* ap = new BDSApertureInfo(a.apertureType,
						a.aper1 * CLHEP::m,
						a.aper2 * CLHEP::m,
						a.aper3 * CLHEP::m,
						a.aper4 * CLHEP::m,
						a.name);
      apertures[a.name] = ap;
    }
  acceleratorModel->RegisterApertures(apertures);
}

void BDSDetectorConstruction::BuildBeamlines()
{
  // build main beam line
  if (verbose || debug)
    {G4cout << "parsing the beamline element list..."<< G4endl;}
  G4Transform3D initialTransform = BDSGlobalConstants::Instance()->BeamlineTransform();
  G4double      initialS         = BDSGlobalConstants::Instance()->BeamlineS();
  
  BDSBeamlineSet mainBeamline = BuildBeamline(BDSParser::Instance()->GetBeamline(),
					      "main beam line",
					      initialTransform,
					      initialS,
					      circular);

#ifdef BDSDEBUG
  G4cout << "Registry size "
	 << BDSAcceleratorComponentRegistry::Instance()->size() << G4endl;
  G4cout << "Parser beam line size "
	 << BDSParser::Instance()->GetBeamline().size() << G4endl;
  BDSAcceleratorComponentRegistry::Instance()->PrintNumberOfEachType();
#endif
  
  if (mainBeamline.massWorld->empty())
    {throw BDSException(__METHOD_NAME__, "BDSIM requires the sequence defined with the use command to have at least one element");}

  // print warning if beam line is approximately circular but flag isn't specified
  if (!circular && mainBeamline.massWorld->ElementAnglesSumToCircle())
    {
      G4cerr << __METHOD_NAME__ << "WARNING: Total sum of all element angles is approximately 2*pi"
             << " but the circular option was not specified, this simulation may run indefinitely" << G4endl;
    }

  // register the beam line in the holder class for the full model
  acceleratorModel->RegisterBeamlineSetMain(mainBeamline);

  // build secondary beam lines
  // loop over placements and check if any are beam lines (have sequences specified)
  auto placements = BDSParser::Instance()->GetPlacements();
  for (const auto& placement : placements)
    {
      if (placement.sequence.empty())
	{continue;} // no sequence specified -> just a placement
      auto parserLine = BDSParser::Instance()->GetSequence(placement.sequence);

      // determine offset in world for extra beam line
      const BDSBeamline* mbl = mainBeamline.massWorld;
      // TODO - so by default if placement.s is finite, it'll be made w.r.t. the main beam line
      // but this could be any beam line in future if we find the right beam line to pass in.
      G4Transform3D startTransform = CreatePlacementTransform(placement, mbl);
      G4double      startS         = mbl->back()->GetSPositionEnd(); 

      // aux beam line must be non-circular by definition to branch off of beam line (for now)
      BDSBeamlineSet extraBeamline = BuildBeamline(parserLine,
						   placement.sequence,
						   startTransform,
						   startS);
      
      acceleratorModel->RegisterBeamlineSetExtra(placement.sequence, extraBeamline);
    }
}

BDSBeamlineSet BDSDetectorConstruction::BuildBeamline(const GMAD::FastList<GMAD::Element>& beamLine,
						      G4String             name,
						      const G4Transform3D& initialTransform,
						      G4double             initialS,
						      G4bool               beamlineIsCircular)
{
  if (userComponentFactory)
    {userComponentFactory->SetDesignParticle(designParticle);}
  BDSComponentFactory* theComponentFactory = new BDSComponentFactory(designParticle, userComponentFactory);
  BDSBeamline* massWorld = new BDSBeamline(initialTransform, initialS);
    
  if (beamlineIsCircular)
    {
      G4bool unsuitable = UnsuitableFirstElement(beamLine.begin());
      if (unsuitable)
	{
	  G4cerr << "The first element in the beam line is unsuitable for a circular "
		 << "model as the first element will " << G4endl << "overlap with the "
		 << "teleporter and terminator - the necessary mechanics for a circular "
		 << "model in Geant4" << G4endl;
	  throw BDSException(__METHOD_NAME__, "check construction for circular machine");
	}
      if (beamLine.size() <= 1) // if an empty LINE it still has 1 item in it
        {throw BDSException(__METHOD_NAME__, "BDSIM requires the sequence defined with the use command to have at least one element for a circular machine.");}
    }  

  for (auto elementIt = beamLine.begin(); elementIt != beamLine.end(); ++elementIt)
    {
      // find next and previous element, but ignore special elements or thin multipoles.
      const GMAD::Element* prevElement = nullptr;
      auto prevIt = elementIt;
      while (prevIt != beamLine.begin())
	{
	  --prevIt;
	  if (prevIt->isSpecial() == false && prevIt->type != GMAD::ElementType::_THINMULT)
	    {
	      prevElement = &(*prevIt);
	      break;
	    }
	}

      const GMAD::Element* nextElement = nullptr;
      auto nextIt = elementIt;
      ++nextIt;
      G4double nextElementInputFace = 0; // get poleface angle for next element whilst testing if next element exists
      while (nextIt != beamLine.end())
	{
	  if (nextIt->isSpecial() == false && nextIt->type != GMAD::ElementType::_THINMULT)
	    {
	      nextElement = &(*nextIt);
          //rotated entrance face of the next element may modify the exit face of the current element.
          nextElementInputFace = nextElement->e1;
          break;
	    }
	  ++nextIt;
	}
      G4double currentArcLength = massWorld->GetTotalArcLength();
      BDSAcceleratorComponent* temp = theComponentFactory->CreateComponent(&(*elementIt),
									   prevElement,
									   nextElement,
									   currentArcLength);
      if(temp)
	{
          BDSSamplerType sType = BDS::DetermineSamplerType((*elementIt).samplerType);
          if ((!canSampleAngledFaces) && (BDS::IsFinite((*elementIt).e2)))
            {sType = BDSSamplerType::none;}
          if ((!canSampleAngledFaces) && (BDS::IsFinite(nextElementInputFace)))
            {sType = BDSSamplerType::none;}
          if (temp->GetType() == "dump") // don't sample after a dump as there'll be nothing
            {sType = BDSSamplerType::none;}
          BDSTiltOffset* tiltOffset = theComponentFactory->CreateTiltOffset(&(*elementIt));
          massWorld->AddComponent(temp, tiltOffset, sType, elementIt->samplerName);
	}
    }

  // Special circular machine bits
  // Add terminator to do ring turn counting logic and kill particles
  // Add teleporter to account for slight ring offset
  if (beamlineIsCircular && !massWorld->empty())
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Circular machine - creating terminator & teleporter" << G4endl;
#endif
      G4double teleporterLength = 0;
      G4Transform3D teleporterTransform = BDS::CalculateTeleporterDelta(massWorld, teleporterLength);

      auto hasBeamPipeInfo = [](BDSBeamlineElement* ble) {return ble->GetBeamPipeInfo() != nullptr;};
      auto firstElementWithBPInfo = std::find_if(massWorld->begin(),  massWorld->end(),  hasBeamPipeInfo);
      auto lastElementWithBPInfo  = std::find_if(massWorld->rbegin(), massWorld->rend(), hasBeamPipeInfo);

      G4double firstbeamPipeMaxExtent = (*firstElementWithBPInfo)->GetBeamPipeInfo()->Extent().MaximumAbsTransverse();
      G4double lastbeamPipeMaxExtent  = (*lastElementWithBPInfo)->GetBeamPipeInfo()->Extent().MaximumAbsTransverse();

      // the extent is a half width, so we double it - also the terminator width.
      G4double teleporterHorizontalWidth = 2 * std::max(firstbeamPipeMaxExtent, lastbeamPipeMaxExtent);
      
      BDSAcceleratorComponent* terminator = theComponentFactory->CreateTerminator(teleporterHorizontalWidth);
      if (terminator)
	{
	  terminator->Initialise();
	  massWorld->AddComponent(terminator);
	}
      
      BDSAcceleratorComponent* teleporter = theComponentFactory->CreateTeleporter(teleporterLength,
										  teleporterHorizontalWidth,
										  teleporterTransform);
      if (teleporter)
	{
	  teleporter->Initialise();
	  massWorld->AddComponent(teleporter);
	}
    }
  
  if (BDSGlobalConstants::Instance()->Survey())
    {
      BDSSurvey* survey = new BDSSurvey(BDSGlobalConstants::Instance()->SurveyFileName() + ".dat");
      survey->Write(massWorld);
      delete survey;
    }
  delete theComponentFactory;

  // print summary
  G4cout << __METHOD_NAME__ << "\"" << name << "\" " << G4endl << *massWorld;

  // Build curvilinear geometry w.r.t. beam line.
  BDSCurvilinearBuilder* clBuilder = new BDSCurvilinearBuilder();
  BDSBeamline* clBeamline = clBuilder->BuildCurvilinearBeamLine1To1(massWorld, beamlineIsCircular);
  BDSBeamline* clBridgeBeamline = clBuilder->BuildCurvilinearBridgeBeamLine(clBeamline);
  delete clBuilder;

  // construct beamline of end pieces
  BDSBeamline* endPieces = BDS::BuildEndPieceBeamline(massWorld, circular);
  BDSBeamlineSet beamlineSet;
  beamlineSet.massWorld              = massWorld;
  beamlineSet.curvilinearWorld       = clBeamline;
  beamlineSet.curvilinearBridgeWorld = clBridgeBeamline;
  beamlineSet.endPieces              = endPieces;
  return beamlineSet;
}

void BDSDetectorConstruction::BuildTunnel()
{
  const BDSBeamline* mainBeamLine = acceleratorModel->BeamlineMain();
  BDSBeamline* tunnelBeamline;
  BDSTunnelBuilder* tb = new BDSTunnelBuilder();
  tunnelBeamline = tb->BuildTunnelSections(mainBeamLine);
  delete tb;
  
  acceleratorModel->RegisterTunnelBeamline(tunnelBeamline);
}

G4VPhysicalVolume* BDSDetectorConstruction::BuildWorld()
{
  // calculate extents of everything we need to place in the world first
  std::vector<BDSExtentGlobal> extents;

  // these beam lines should always exist so are safe to access.
  const auto& blMain = acceleratorModel->BeamlineSetMain();
  blMain.GetExtentGlobals(extents);

  // check optional placement beam line (likevector of placements)
  BDSBeamline* plBeamline = acceleratorModel->PlacementBeamline();
  if (plBeamline) // optional - may be nullptr
    {extents.push_back(plBeamline->GetExtentGlobal());}

  // check tunnel beam line
  BDSBeamline* tunnelBeamline = acceleratorModel->TunnelBeamline();
  if (tunnelBeamline)
    {extents.push_back(tunnelBeamline->GetExtentGlobal());}

  // check extra beam lines
  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  // extras is a map, so iterator has first and second for key and value
  for (const auto& bl : extras)
    {bl.second.GetExtentGlobals(extents);}

  // inspect any sampler placements and calculate their extent without constructing them
  extents.push_back(CalculateExtentOfSamplerPlacements(blMain.massWorld));

  // Expand to maximum extents of each beam line.
  G4ThreeVector worldR;
  // loop over all extents from all beam lines
  for (const auto& ext : extents)
    {
      for (G4int i = 0; i < 3; i++)
	{worldR[i] = std::max(worldR[i], ext.GetMaximumExtentAbsolute()[i]);} // expand with the maximum
    }

  G4String    worldName         = "World";
  G4String    worldMaterialName = BDSGlobalConstants::Instance()->WorldMaterial();
  G4Material* worldMaterial     = BDSMaterials::Instance()->GetMaterial(worldMaterialName);
  G4VSolid*        worldSolid   = nullptr;
  G4LogicalVolume* worldLV      = nullptr;

  G4String worldGeometryFile = BDSGlobalConstants::Instance()->WorldGeometryFile();
  if (!worldGeometryFile.empty())
    {
      BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(worldName,
										worldGeometryFile,
										nullptr,
										0, 0, true, BDSSDType::energydepworldcontents);
      worldExtent = geom->GetExtent();

      BDSExtentGlobal worldExtentGlobal = BDSExtentGlobal(worldExtent, G4Transform3D());
      G4bool worldContainsAllBeamlines = worldExtentGlobal.Encompasses(extents);

      // cannot construct world if any beamline extent is greater than the world extents
      if (!worldContainsAllBeamlines)
        {
	  G4String message = "Beamlines cannot be constructed, beamline extents are larger than \n";
	  message += "the extents of the external world";
	  throw BDSException(__METHOD_NAME__, message);
        }

      worldSolid = geom->GetContainerSolid();
      worldLV    = geom->GetContainerLogicalVolume();

      // make the world sensitive to energy deposition with its own unique hits collection
      // this will be a nullptr depending on the options.
      // make world sensitive if importance sampling is needed
      if (BDSGlobalConstants::Instance()->StoreELossWorld()
	  || BDSGlobalConstants::Instance()->UseImportanceSampling()
	  || BDSGlobalConstants::Instance()->StoreELossWorldContents())
        {
          geom->AttachSensitiveDetectors();
          // override the logical volume itself with a specific SD
          worldLV->SetSensitiveDetector(BDSSDManager::Instance()->WorldComplete());
        }  
    }
  else
    {
      // add on margin for constructed world volume
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "world extent absolute: " << worldR      << G4endl;
#endif
      G4double margin = BDSGlobalConstants::Instance()->WorldVolumeMargin();
      margin = std::max(margin, 2*CLHEP::m); // minimum margin of 2m.
      worldR += G4ThreeVector(margin,margin,margin); //add 5m extra in every dimension
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "with " << margin << "m margin, it becomes in all dimensions: " << worldR << G4endl;
#else
      G4cout << __METHOD_NAME__ << "World dimensions: " << worldR / CLHEP::m << " m" << G4endl;
#endif

      worldExtent = BDSExtent(worldR);
      worldSolid = new G4Box(worldName + "_solid", worldR.x(), worldR.y(), worldR.z());


      worldLV = new G4LogicalVolume(worldSolid,              // solid
                                    worldMaterial,           // material
                                    worldName + "_lv");      // name

      // make the world sensitive to energy deposition with its own unique hits collection
      if (BDSGlobalConstants::Instance()->StoreELossWorld())
        {worldLV->SetSensitiveDetector(BDSSDManager::Instance()->WorldComplete());}
    }

  // visual attributes
  // copy the debug vis attributes but change to force wireframe
  G4VisAttributes* debugWorldVis = new G4VisAttributes(*(BDSGlobalConstants::Instance()->ContainerVisAttr()));
  debugWorldVis->SetForceWireframe(true);//just wireframe so we can see inside it
  worldLV->SetVisAttributes(debugWorldVis);
  
  // set limits
  worldLV->SetUserLimits(BDSGlobalConstants::Instance()->DefaultUserLimits());

  // place the world
  G4VPhysicalVolume* worldPV = new G4PVPlacement((G4RotationMatrix*)0, // no rotation
						 (G4ThreeVector)0,     // at (0,0,0)
						 worldLV,	            // its logical volume
						 worldName,            // its name
						 nullptr,		    // its mother  volume
						 false,		    // no boolean operation
						 0,                    // copy number
						 checkOverlaps);       // overlap checking

  // Register the lv & pvs to the our holder class for the model
  acceleratorModel->RegisterWorldPV(worldPV);
  acceleratorModel->RegisterWorldLV(worldLV);
  acceleratorModel->RegisterWorldSolid(worldSolid);

  // Register world PV with our auxiliary navigator so steppers and magnetic
  // fields know which geometry to navigate to get local / global transforms.
  // This is the regular world used as a backup to the curvilinear world.
  BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(worldPV);

  /// Give the pv info registry a heads up that these volumes don't have info (optimisation).
  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterExcludedPV(worldPV);
  
  return worldPV;
}

void BDSDetectorConstruction::ComponentPlacement(G4VPhysicalVolume* worldPV)
{
  // We musn't place parallel world geometry here - their world is produced by
  // Geant4 at the right time, so we have a separate placement call for them
  BDSBeamlineSet mainBL = BDSAcceleratorModel::Instance()->BeamlineSetMain();
  PlaceBeamlineInWorld(mainBL.massWorld,
		       worldPV, checkOverlaps, true);
  PlaceBeamlineInWorld(mainBL.endPieces,
		       worldPV, checkOverlaps);
  if (BDSGlobalConstants::Instance()->BuildTunnel())
    {
      PlaceBeamlineInWorld(acceleratorModel->TunnelBeamline(),
			   worldPV, checkOverlaps);
    }
  // No energy counter SD added here as individual placements have that attached
  // during construction time
  PlaceBeamlineInWorld(placementBL, worldPV, checkOverlaps);

  // Place BLMs. Similarly no sensitivity set here - done at construction time.
  PlaceBeamlineInWorld(BDSAcceleratorModel::Instance()->BLMsBeamline(),
		       worldPV,
		       checkOverlaps,
		       false,
		       false,
		       false,
		       true); // use incremental copy nubmers 

  const auto& extras = BDSAcceleratorModel::Instance()->ExtraBeamlines();
  for (auto const& bl : extras)
    {// extras is map so iterator has first and second for key and value
      // note these are currently not sensitive as there's no CL frame for them
      PlaceBeamlineInWorld(bl.second.massWorld, worldPV, checkOverlaps);
      PlaceBeamlineInWorld(bl.second.endPieces, worldPV, checkOverlaps);
    }
}

void BDSDetectorConstruction::PlaceBeamlineInWorld(BDSBeamline*          beamline,
						   G4VPhysicalVolume*    containerPV,
						   G4bool                checkOverlaps,
						   G4bool                setRegions,
						   G4bool                registerInfo,
						   G4bool                useCLPlacementTransform,
						   G4bool                useIncrementalCopyNumbers)
{
  if (!beamline)
    {return;}

  G4int i = 0;
  for (auto element : *beamline)
    {
      // Do nothing for gap element
      if (dynamic_cast<BDSGap*>(element->GetAcceleratorComponent()))
       {continue;}

      if (setRegions)
	{
	  auto accComp = element->GetAcceleratorComponent();
	  const G4String regionName = accComp->GetRegion();
	  if(!regionName.empty()) // ie string is defined so we should attach region
	    {
	      G4Region* region = BDSAcceleratorModel::Instance()->Region(regionName);
	      auto contLV = accComp->GetContainerLogicalVolume();
	      contLV->SetRegion(region);
	      region->AddRootLogicalVolume(contLV);
	    }
	}

      // setup the sensitivity
      element->GetAcceleratorComponent()->AttachSensitiveDetectors();
      
      G4String placementName = element->GetPlacementName() + "_pv";
      G4Transform3D* placementTransform = element->GetPlacementTransform();
      if (useCLPlacementTransform)
	{placementTransform = element->GetPlacementTransformCL();}
      G4int copyNumber = useIncrementalCopyNumbers ? i : element->GetCopyNo();
      auto pv = new G4PVPlacement(*placementTransform,                  // placement transform
				  placementName,                        // placement name
				  element->GetContainerLogicalVolume(), // volume to be placed
				  containerPV,                          // volume to place it in
				  false,                                // no boolean operation
				  copyNumber,                           // copy number
				  checkOverlaps);                       // overlap checking

      if (registerInfo)
        {
	  BDSPhysicalVolumeInfo* theinfo = new BDSPhysicalVolumeInfo(element->GetName(),
								     placementName,
								     element->GetSPositionMiddle(),
								     element->GetIndex(),
								     beamline);
	  
	  BDSPhysicalVolumeInfoRegistry::Instance()->RegisterInfo(pv, theinfo, true);
        }
      i++; // for increuemental copy numbers
    }
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::SamplerPlacement& samplerPlacement,
								const BDSBeamline*            beamline,
								G4double*                     S)
{
  // convert a sampler placement to a general placement for generation of the transform.
  GMAD::Placement convertedPlacement(samplerPlacement); 
  return CreatePlacementTransform(convertedPlacement, beamline, S);
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::BLMPlacement& blmPlacement,
								const BDSBeamline*        beamline,
								G4double*                 S)
{
  // convert a sampler placement to a general placement for generation of the transform.
  GMAD::Placement convertedPlacement(blmPlacement); 
  return CreatePlacementTransform(convertedPlacement, beamline, S);
}

G4Transform3D BDSDetectorConstruction::CreatePlacementTransform(const GMAD::Placement& placement,
								const BDSBeamline*     beamLine,
								G4double*              S)
{
  G4Transform3D result;

  // 3 scenarios
  // 1) global placement X,Y,Z + rotation
  // 2) w.r.t. beam line placement x,y,S + rotation
  // 3) w.r.t. element in beam line placement elementName + x,y,s + rotation
  
  // in all cases, need the rotation
  G4RotationMatrix rm = G4RotationMatrix();
  if (placement.axisAngle)
    {
      G4ThreeVector axis = G4ThreeVector(placement.axisX,
					 placement.axisY,
					 placement.axisZ);
      rm = G4RotationMatrix(axis, placement.angle*CLHEP::rad);
    }
  else
    {
      if (BDS::IsFinite(placement.phi)   ||
	  BDS::IsFinite(placement.theta) ||
	  BDS::IsFinite(placement.psi))
	{// only build if finite
	  CLHEP::HepEulerAngles ang = CLHEP::HepEulerAngles(placement.phi*CLHEP::rad,
							    placement.theta*CLHEP::rad,
							    placement.psi*CLHEP::rad);
	  rm = G4RotationMatrix(ang);
	}
    } 

  // create a tranform from w.r.t. the beam line if s is finite and it's not w.r.t a
  // particular element. If it's w.r.t. a particular element, treat s as local curvilinear
  // s and use as local 'z' in the transform.
  if (!placement.referenceElement.empty())
    {// scenario 3
      const BDSBeamlineElement* element = beamLine->GetElement(placement.referenceElement,
							 placement.referenceElementNumber);
      if (!element)
	{
	  G4cerr << __METHOD_NAME__ << "No element named \""
		 << placement.referenceElement << "\" found for placement number "
		 << placement.referenceElementNumber << G4endl;
	  G4cout << "Note, this may be because the element is a bend and split into " << G4endl;
	  G4cout << "multiple sections with unique names. Run the visualiser to get " << G4endl;
	  G4cout << "the name of the segment, or place w.r.t. the element before / after." << G4endl;
	  throw BDSException(__METHOD_NAME__, "invalid element for placement");
	}
      // in this case we should use s for longitudinal offset - warn user if mistakenly using z
      if (BDS::IsFinite(placement.z))
	{
	  G4cout << "WARNING: placement \"" << placement.name << "\" is placed using a referenceElement but the z offset is" << G4endl;
	  G4cout << "non zero. Note, s should be used to offset the placement in this case and z will have no effect." << G4endl << G4endl;
	}
      G4double sCoordinate = element->GetSPositionMiddle(); // start from middle of element
      sCoordinate += placement.s * CLHEP::m; // add on (what's considered) 'local' s from the placement
      if (S)
	{*S = sCoordinate;}
      G4Transform3D beamlinePart = beamLine->GetGlobalEuclideanTransform(sCoordinate,
									 placement.x*CLHEP::m,
									 placement.y*CLHEP::m);
      G4Transform3D localRotation(rm, G4ThreeVector());
      result = beamlinePart * localRotation;      
    }
  else if (BDS::IsFinite(placement.s))
    {// scenario 2
      G4Transform3D beamlinePart =  beamLine->GetGlobalEuclideanTransform(placement.s*CLHEP::m,
									  placement.x*CLHEP::m,
									  placement.y*CLHEP::m);
      G4Transform3D localRotation(rm, G4ThreeVector());
      result = beamlinePart * localRotation;
      if (S)
	{*S = placement.s*CLHEP::m;}
    }
  else
    {// scenario 1
      G4ThreeVector translation = G4ThreeVector(placement.x*CLHEP::m,
						placement.y*CLHEP::m,
						placement.z*CLHEP::m);
      
      
      result = G4Transform3D(rm, translation);
      if (S)
	{*S = -1000;} // default
    }
  
  return result;
}

BDSExtent BDSDetectorConstruction::CalculateExtentOfSamplerPlacement(const GMAD::SamplerPlacement& sp) const
{
  BDSExtent apertureExtent;
  if (sp.apertureModel.empty())
    {
      BDSApertureInfo aperture = BDSApertureInfo(sp.shape,
						 sp.aper1 * CLHEP::m,
						 sp.aper2 * CLHEP::m,
						 sp.aper3 * CLHEP::m,
						 sp.aper4 * CLHEP::m,
						 sp.name);
      apertureExtent = aperture.Extent();
    }
  else
    {
      BDSApertureInfo* aperture = BDSAcceleratorModel::Instance()->Aperture(sp.apertureModel);
      apertureExtent = aperture->Extent();
    }
  
  // aperture is only transverse - fiddle z
  BDSExtent result = BDSExtent(apertureExtent.XNeg(), apertureExtent.XPos(),
                               apertureExtent.YNeg(), apertureExtent.YPos(),
                               1*CLHEP::um, 1*CLHEP::um);
  return result;
}

BDSExtentGlobal BDSDetectorConstruction::CalculateExtentOfSamplerPlacements(const BDSBeamline* beamLine) const
{
  BDSExtentGlobal result;
  std::vector<GMAD::SamplerPlacement> samplerPlacements = BDSParser::Instance()->GetSamplerPlacements();
  for (const auto& samplerPlacement : samplerPlacements)
    {
      BDSExtent samplerExtent = CalculateExtentOfSamplerPlacement(samplerPlacement);
      G4Transform3D placementTransform = CreatePlacementTransform(samplerPlacement, beamLine);
      BDSExtentGlobal samplerExtentG = BDSExtentGlobal(samplerExtent, placementTransform);
      result = result.ExpandToEncompass(samplerExtentG);
    }
  return result;
}

#if G4VERSION_NUMBER > 1009
BDSBOptrMultiParticleChangeCrossSection*
BDSDetectorConstruction::BuildCrossSectionBias(const std::list<std::string>& biasList,
					       G4String defaultBias,
					       G4String elementName)
{
  // no accelerator components to bias
  if (biasList.empty())
    {return nullptr;}

  // loop over all physics biasing
  BDSBOptrMultiParticleChangeCrossSection* eg = new BDSBOptrMultiParticleChangeCrossSection();

  const auto& biasObjectList = BDSParser::Instance()->GetBiasing();
  for (std::string const & bs : biasList)
    {
      if (bs.empty() && defaultBias.empty())
	{continue;} // no bias specified and no default

      // if no bias, use default else copy name of bias to use
      G4String bias = bs.empty() ? defaultBias : bs;
      
      auto result = biasObjectList.find(bias);
      if (result == biasObjectList.end())
	{throw BDSException("Error: bias named \"" + bias + "\" not found for element named \"" + elementName + "\"");}
      const GMAD::PhysicsBiasing& pb = *result;
      
      if(debug)
	{G4cout << __METHOD_NAME__ << "bias loop : " << bs << " " << pb.particle << " " << pb.process << G4endl;}
      
      eg->AddParticle(pb.particle);
      
      // loop through all processes
      for (unsigned int p = 0; p < pb.processList.size(); ++p)
	{eg->SetBias(pb.particle,pb.processList[p],pb.factor[p],(G4int)pb.flag[p]);}
    }

  biasObjects.push_back(eg);
  return eg;
}
#endif

void BDSDetectorConstruction::BuildPhysicsBias() 
{
#if G4VERSION_NUMBER > 1009
  BDSAcceleratorComponentRegistry* registry = BDSAcceleratorComponentRegistry::Instance();
  if (debug)
    {G4cout << __METHOD_NAME__ << "registry=" << registry << G4endl;}

#if G4VERSION_NUMBER > 1039
  // only available in 4.10.4 onwards
  // crystal biasing necessary for implementation of variable density
  std::set<G4LogicalVolume*>* crystals = BDSAcceleratorModel::Instance()->VolumeSet("crystals");
  if (!crystals->empty())
    {
      G4cout << __METHOD_NAME__ << "Using crystal biasing: true" << G4endl; // to match print out style further down
      auto crystalBiasing = new G4ChannelingOptrMultiParticleChangeCrossSection();
      for (auto crystal : *crystals)
	{crystalBiasing->AttachTo(crystal);}
    }
#endif

  G4String defaultBiasVacuum   = BDSParser::Instance()->GetOptions().defaultBiasVacuum;
  G4String defaultBiasMaterial = BDSParser::Instance()->GetOptions().defaultBiasMaterial;
  
  G4bool useDefaultBiasVacuum   = !defaultBiasVacuum.empty();
  G4bool useDefaultBiasMaterial = !defaultBiasMaterial.empty();
  const auto& biasObjectList    = BDSParser::Instance()->GetBiasing();
  G4bool biasesDefined          = !biasObjectList.empty();
  G4bool overallUseBiasing      = useDefaultBiasVacuum || useDefaultBiasMaterial || biasesDefined;
  G4cout << __METHOD_NAME__ << "Using generic biasing: " << BDS::BoolToString(overallUseBiasing) << G4endl;
  if (!overallUseBiasing)
    {return;} // no biasing used -> dont attach as just overhead for no reason
  
  // apply per element biases
  std::map<G4String, BDSAcceleratorComponent*> allAcceleratorComponents = registry->AllComponentsIncludingUnique();
  for (auto const & item : allAcceleratorComponents)
    {
      if (debug)
        {G4cout << __METHOD_NAME__ << "checking component named: " << item.first << G4endl;}
      BDSAcceleratorComponent* accCom = item.second;
      G4String accName = accCom->GetName();
      auto vacuumLV = accCom->GetAcceleratorVacuumLogicalVolume();
      
      // Build vacuum bias object based on vacuum bias list in the component
      auto vacuumBiasList = accCom->GetBiasVacuumList();
      if (!vacuumBiasList.empty() && vacuumLV)
        {
          auto egVacuum = BuildCrossSectionBias(accCom->GetBiasVacuumList(), defaultBiasVacuum, accName);
	      if (debug)
		{
		  G4cout << __METHOD_NAME__ << "vacuum volume name: " << vacuumLV
			 << " " << vacuumLV->GetName() << G4endl;
		}
	      egVacuum->AttachTo(vacuumLV);
	}
      
      // Build material bias object based on material bias list in the component
      auto materialBiasList = accCom->GetBiasMaterialList();
      if (!materialBiasList.empty())
	{
	  auto egMaterial = BuildCrossSectionBias(materialBiasList, defaultBiasMaterial, accName);
	  auto allLVs     = accCom->GetAllBiasingVolumes();
	  if (debug)
	    {G4cout << __METHOD_NAME__ << "All logical volumes " << allLVs.size() << G4endl;}
	  for (auto materialLV : allLVs)
	    {
	      if (materialLV != vacuumLV)
		{
		  if (debug)
		    {
		      G4cout << __METHOD_NAME__ << "All logical volumes " << materialLV
			     << " " << (materialLV)->GetName() << G4endl;
		    }
		  egMaterial->AttachTo(materialLV);
		}
	    }
	}
    }
#endif
}

void BDSDetectorConstruction::ConstructSDandField()
{
  auto flds = BDSFieldBuilder::Instance()->CreateAndAttachAll(); // avoid shadowing 'fields'
  acceleratorModel->RegisterFields(flds);
}

G4bool BDSDetectorConstruction::UnsuitableFirstElement(GMAD::FastList<GMAD::Element>::FastListConstIterator element)
{
  // skip past any line elements in parser to find first non-line element
  while ((*element).type == GMAD::ElementType::_LINE)
    {element++;}
  
  if ((*element).type == GMAD::ElementType::_RBEND)
    {return true;}  // unsuitable
  else if (BDS::IsFinite((*element).e1))
    {return true;}  // unsuitable
  else
    {return false;} // suitable
}
