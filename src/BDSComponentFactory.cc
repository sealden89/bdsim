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
#include "BDSComponentFactory.hh"

// elements
#ifdef USE_AWAKE
#include "BDSAwakeScintillatorScreen.hh"
#include "BDSAwakeSpectrometer.hh"
#endif
#include "BDSCavityElement.hh"
#include "BDSCollimatorCrystal.hh"
#include "BDSCollimatorElliptical.hh"
#include "BDSCollimatorJaw.hh"
#include "BDSCollimatorRectangular.hh"
#include "BDSColours.hh"
#include "BDSComponentFactoryUser.hh"
#include "BDSDegrader.hh"
#include "BDSDrift.hh"
#include "BDSDump.hh"
#include "BDSElement.hh"
#include "BDSLaserWire.hh"
#include "BDSLaserWireNew.hh"
#include "BDSLaserwireBuilder.hh"
#include "BDSLine.hh"
#include "BDSMagnet.hh"
#include "BDSSamplerPlane.hh"
#include "BDSScreen.hh"
#include "BDSShield.hh"
#include "BDSTeleporter.hh"
#include "BDSTerminator.hh"
#include "BDSTiltOffset.hh"
#include "BDSTransform3D.hh"
#include "BDSWireScanner.hh"
#include "BDSUndulator.hh"

// general
#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSBeamPipeType.hh"
#include "BDSBendBuilder.hh"
#include "BDSLine.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSCrystalInfo.hh"
#include "BDSCrystalType.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSExecOptions.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldType.hh"
#include "BDSGlobalConstants.hh"
#include "BDSGap.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"
#include "BDSIntegratorDipoleFringe.hh"
#include "BDSLaser.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetGeometryType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSParser.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4String.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

#include "parser/element.h"
#include "parser/elementtype.h"
#include "parser/cavitymodel.h"
#include "parser/newcolour.h"
#include "parser/crystal.h"
#include "parser/laser.h"

#include <cmath>
#include <limits>
#include <string>
#include <utility>

using namespace GMAD;

BDSComponentFactory::BDSComponentFactory(const BDSParticleDefinition* designParticleIn,
					 BDSComponentFactoryUser* userComponentFactoryIn):
  designParticle(designParticleIn),
  userComponentFactory(userComponentFactoryIn),
  lengthSafety(BDSGlobalConstants::Instance()->LengthSafety()),
  thinElementLength(BDSGlobalConstants::Instance()->ThinElementLength()),
  includeFringeFields(BDSGlobalConstants::Instance()->IncludeFringeFields()),
  yokeFields(BDSGlobalConstants::Instance()->YokeFields()),
  integratorSetType(BDSGlobalConstants::Instance()->IntegratorSet())
{
  brho  = designParticle->BRho();
  beta0 = designParticle->Beta();

  integratorSet = BDS::IntegratorSet(integratorSetType);
  G4cout << __METHOD_NAME__ << "Using \"" << integratorSetType << "\" set of integrators" << G4endl;

  PrepareColours();      // prepare colour definitions from parser
  PrepareCavityModels(); // prepare rf cavity model info from parser
  PrepareCrystals();     // prepare crystal model info from parser
  PrepareLasers();
}

BDSComponentFactory::~BDSComponentFactory()
{
  for (auto info : cavityInfos)
    {delete info.second;}
  for (auto info : crystalInfos)
    {delete info.second;}
  for (auto laser : lasers)
    {delete laser.second;}

  // Deleted here although not used directly here as new geometry can only be
  // created through this class.
  delete BDSBeamPipeFactory::Instance();
  delete BDSMagnetOuterFactory::Instance();
}

BDSAcceleratorComponent* BDSComponentFactory::CreateComponent(Element const* elementIn,
							      Element const* prevElementIn,
							      Element const* nextElementIn,
							      G4double currentArcLength)
{
  element = elementIn;
  prevElement = prevElementIn;
  nextElement = nextElementIn;
  G4double angleIn  = 0.0;
  G4double angleOut = 0.0;
  G4bool registered = false;
  // Used for multiple instances of the same element but different poleface rotations.
  // Ie only a drift is modified to match the pole face rotation of a magnet.
  G4bool differentFromDefinition = false;

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "named: \"" << element->name << "\"" << G4endl;  
#endif

  if (BDSAcceleratorComponentRegistry::Instance()->IsRegistered(element->name))
    {registered = true;}

  if (element->type == ElementType::_DRIFT)
    {
      // minuses are to go from 'strength convention' to 3d cartesian.
      if (prevElement)
        {angleIn  = OutgoingFaceAngle(prevElement);}
      if (nextElement)
        {angleOut = IncomingFaceAngle(nextElement);}
      
      //if drift has been modified at all
      if (BDS::IsFinite(angleIn) || BDS::IsFinite(angleOut))
	{differentFromDefinition = true;}
    }
  else if (element->type == ElementType::_RBEND)
    {// bend builder will construct it to match - but here we just know it's different
      // match a previous rbend with half the angle
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_RBEND) // also if includeFringeFields
	    {differentFromDefinition = true;}
	}
      // match the upcoming rbend with half the angle
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_RBEND) // also if includeFringeFields
	    {differentFromDefinition = true;}
	}
    }
  else if (element->type == ElementType::_SBEND)
    {
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_SBEND && includeFringeFields)
	    {differentFromDefinition = true;}
	}
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_SBEND && includeFringeFields)
	    {differentFromDefinition = true;}
	}
    }
  else if (element->type == ElementType::_THINMULT)
    {// thinmultipole only uses one angle - so angleIn
      if (prevElement && nextElement)
	{// both exist
	  ElementType prevType = prevElement->type;
	  ElementType nextType = nextElement->type;
	  if (prevType == ElementType::_DRIFT && nextType == ElementType::_DRIFT)
	    {angleIn = 0;} // between two drifts - flat
	  else if (prevType == ElementType::_DRIFT)
	    {angleIn = -IncomingFaceAngle(nextElement);} // previous is drift which will match next
	  else
	    {angleIn = OutgoingFaceAngle(prevElement);} // next is drift which will match prev
	}
      else if (prevElement)
	{angleIn = OutgoingFaceAngle(prevElement);} // only previous element - match it
      else
	{angleIn = IncomingFaceAngle(nextElement);} // only next element - match it

      // because thin multipoles adapt to what's around them, it's not possible to know
      // if, in the case the thin multipole already exists in the registry, it's been
      // modified or is flat, therefore we mark them all as unique.
      differentFromDefinition = true;
    }
  else if (element->type == ElementType::_SOLENOID)
    {// we build incoming / outgoing fringe fields for solenoids
      if (prevElement)
	{
	  if (prevElement->type == ElementType::_SOLENOID)
	    {differentFromDefinition = true;}
	}
      if (nextElement)
	{
	  if (nextElement->type == ElementType::_SOLENOID)
	    {differentFromDefinition = true;}
	}
    }
  
  // Check if the component already exists and return that.
  // Don't use the registry for output elements since reliant on unique name.
  if (registered && !differentFromDefinition)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "using already manufactured component" << G4endl;
#endif
      return BDSAcceleratorComponentRegistry::Instance()->GetComponent(element->name);
    }

  // Update name for this component
  elementName = element->name;

  if (differentFromDefinition)
    {
      G4int val;
      if (modifiedElements.find(elementName) == modifiedElements.end())
	{
	  modifiedElements[elementName] = 0;
	  val = 0;
	}
      else
	{
	  val = modifiedElements[elementName] + 1;
	  modifiedElements[elementName] = val;
	}
      elementName += "_mod_" + std::to_string(val);
    }

  BDSAcceleratorComponent* component = nullptr;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - creating \"" << elementName << "\"" << G4endl;
  element->print();
#endif
  switch(element->type)
    {
    case ElementType::_DRIFT:
      {component = CreateDrift(angleIn, angleOut); break;}
    case ElementType::_RF:
      {
	component = CreateRF(currentArcLength);
	differentFromDefinition = true; // unique phase for every placement in beam line
	break;
      }
    case ElementType::_SBEND:
      {component = CreateSBend(); break;}
    case ElementType::_RBEND:
      {component = CreateRBend(); break;}
    case ElementType::_HKICKER:
      {component = CreateKicker(KickerType::horizontal); break;}
    case ElementType::_VKICKER:
      {component = CreateKicker(KickerType::vertical); break;}
    case ElementType::_KICKER:
    case ElementType::_TKICKER:
      {component = CreateKicker(KickerType::general); break;}
    case ElementType::_QUAD:
      {component = CreateQuad(); break;}
    case ElementType::_SEXTUPOLE:
      {component = CreateSextupole(); break;}
    case ElementType::_OCTUPOLE:
      {component = CreateOctupole(); break;}
    case ElementType::_DECAPOLE:
      {component = CreateDecapole(); break;}
    case ElementType::_MULT:
      {component = CreateMultipole(); break;}
    case ElementType::_THINMULT:
      {component = CreateThinMultipole(angleIn); break;}
    case ElementType::_ELEMENT:
      {component = CreateElement(); break;}
    case ElementType::_SOLENOID:
      {component = CreateSolenoid(); break;} 
    case ElementType::_ECOL:
      {component = CreateEllipticalCollimator(); break;} 
    case ElementType::_RCOL:
      {component = CreateRectangularCollimator(); break;}
    case ElementType::_JCOL:
      {component = CreateJawCollimator(); break;}
    case ElementType::_MUONSPOILER:    
      {component = CreateMuonSpoiler(); break;}
    case ElementType::_SHIELD:
      {component = CreateShield(); break;}
    case ElementType::_DEGRADER:
      {component = CreateDegrader(); break;}
    case ElementType::_WIRESCANNER:
      {component = CreateWireScanner(); break;}
    case ElementType::_GAP:
      {component = CreateGap(); break;}
    case ElementType::_CRYSTALCOL:
      {component = CreateCrystalCollimator(); break;}
    case ElementType::_SCREEN:
      {component = CreateScreen(); break;} 
    case ElementType::_TRANSFORM3D:
      {component = CreateTransform3D(); break;}
    case ElementType::_THINRMATRIX:
      {component = CreateThinRMatrix(angleIn, elementName); break;}
    case ElementType::_PARALLELTRANSPORTER:
      {component = CreateParallelTransporter(); break;}
    case ElementType::_RMATRIX:
      {component = CreateRMatrix(); break;}
    case ElementType::_UNDULATOR:
      {component = CreateUndulator(); break;}
    case ElementType::_LASERWIRE:
      {component = CreateLaserWire(); break;}
    case ElementType::_USERCOMPONENT:
      {
	if (!userComponentFactory)
	  {
	    G4cerr << __METHOD_NAME__ << "Error - no user component factory registered" << G4endl;
	    exit(1);
	  }
	G4String typeName = G4String(element->userTypeName);
	if (!userComponentFactory->CanConstructComponentByName(typeName))
	  {
	    G4cerr << __METHOD_NAME__ << "Error - no such component \""
		   << element->userTypeName << "\" registered." << G4endl;
	    exit(1);
	  }
	else
	  {
	    component = userComponentFactory->ConstructComponent(typeName,
								 element,
								 prevElement,
								 nextElement,
								 currentArcLength);
	  }
	break;
      }
    case ElementType::_DUMP:
      {component = CreateDump(); break;}
    case ElementType::_AWAKESCREEN:
#ifdef USE_AWAKE
      {component = CreateAwakeScreen(); break;} 
#else
      G4cerr << __METHOD_NAME__ << "Awake Screen can't be used - not compiled with AWAKE module!" << G4endl;
      exit(1);
#endif
    case ElementType::_AWAKESPECTROMETER:
#ifdef USE_AWAKE
      {component = CreateAwakeSpectrometer(); break;}
#else
      G4cerr << __METHOD_NAME__ << "Awake Spectrometer can't be used - not compiled with AWAKE module!" << G4endl;
      exit(1);
#endif
      
      // common types, but nothing to do here
    case ElementType::_MARKER:
    case ElementType::_LINE:
    case ElementType::_REV_LINE:
      {component = nullptr; break;}
    default:
      {
	G4cerr << __METHOD_NAME__ << "unknown type " << element->type << G4endl;
	exit(1);
	break;
      }
    }

  // note this test will only be reached (and therefore the component registered)
  // if both the component didn't exist and it has been constructed
  if (component)
    {
      component->SetBiasVacuumList(element->biasVacuumList);
      component->SetBiasMaterialList(element->biasMaterialList);
      component->SetRegion(element->region);
      component->SetMinimumKineticEnergy(element->minimumKineticEnergy*CLHEP::GeV);

      // infinite absorbers for collimators - must be done after SetMinimumKineticEnergy and
      // specific to these elements. must be done before initialise too.
      switch (element->type)
	{
	case ElementType::_ECOL:
	case ElementType::_RCOL:
	case ElementType::_JCOL:
	  {
	    if (BDSGlobalConstants::Instance()->CollimatorsAreInfiniteAbsorbers())
	      {component->SetMinimumKineticEnergy(std::numeric_limits<double>::max());}
	    break;
	  }
	default:
	  {break;}	  
	}
      
      SetFieldDefinitions(element, component);
      component->Initialise();
      // register component and memory
      BDSAcceleratorComponentRegistry::Instance()->RegisterComponent(component,differentFromDefinition);
    }
  
  return component;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTeleporter(const G4double      teleporterLength,
							       const G4Transform3D transformIn)
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["length"] = teleporterLength; // convey length scale to integrator
  BDSFieldInfo* vacuumFieldInfo = new BDSFieldInfo(BDSFieldType::teleporter,
						   brho,
						   BDSIntegratorType::teleporter,
						   st,
						   true,
						   transformIn);
  
  G4cout << "---->creating Teleporter, "
	 << "l = " << teleporterLength/CLHEP::m << "m"
	 << G4endl;

  return( new BDSTeleporter(teleporterLength, vacuumFieldInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDrift(G4double angleIn, G4double angleOut)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  // Create normal vectors
  std::pair<G4ThreeVector,G4ThreeVector> faces = BDS::CalculateFaces(angleIn, angleOut);

  // current element tilt
  G4double currentTilt  = element->tilt * CLHEP::rad;
  G4double prevTilt = 0;
  G4double nextTilt = 0;
  if (prevElement)
    {prevTilt = prevElement->tilt * CLHEP::rad;}
  if (nextElement)
    {nextTilt = nextElement->tilt * CLHEP::rad;}
  G4ThreeVector inputFaceNormal  = faces.first.rotateZ(prevTilt - currentTilt);
  G4ThreeVector outputFaceNormal = faces.second.rotateZ(nextTilt - currentTilt);

  const G4double length = element->l*CLHEP::m;

  // Beampipeinfo needed here to get aper1 for check.
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, inputFaceNormal,
						      outputFaceNormal);

  const BDSExtent extent = beamPipeInfo->Extent();
  G4bool facesWillIntersect = BDS::WillIntersect(inputFaceNormal, outputFaceNormal,
						 length, extent, extent);

  if (facesWillIntersect)
    {
      G4cerr << __METHOD_NAME__ << "Drift \"" << elementName
	     << "\" between \"";
      if (prevElement)
	{G4cerr << prevElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" and \"";
      if (nextElement)
	{G4cerr << nextElement->name;}
      else
	{G4cerr << "none";}
      G4cerr << "\" is too short given its width and the angle of its faces." << G4endl;
      exit(1);
    }

  return (new BDSDrift( elementName,
			length,
			beamPipeInfo));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRF(G4double currentArcLength)
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::rfcavity);

  BDSFieldType fieldType = BDSFieldType::rf; // simple sinusoidal E field only
  if (!(element->fieldVacuum.empty()))
    {
      BDSFieldInfo* field = BDSFieldFactory::Instance()->GetDefinition(element->fieldVacuum);
      fieldType = field->FieldType();
    }

  BDSMagnetStrength* st = PrepareCavityStrength(element, currentArcLength);
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);
  
  BDSCavityInfo* cavityInfo = PrepareCavityModelInfo(element, (*st)["frequency"]);

  // update 0 point of field with geometry
  (*st)["equatorradius"] = cavityInfo->equatorRadius;
  G4Material* vacuumMaterial = PrepareVacuumMaterial(element);
    
  return new BDSCavityElement(elementName,
			      element->l*CLHEP::m,
			      vacuumMaterial,
			      vacuumField,
			      cavityInfo);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSBend()
  {
  if (!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double angle = 0;
  G4double field = 0;
  CalculateAngleAndFieldSBend(element, angle, field);
  (*st)["angle"]  = angle;
  (*st)["field"]  = field*element->scaling;
  (*st)["by"]     = 1;// bx,by,bz is unit field direction, so (0,1,0) here
  (*st)["length"] = element->l * CLHEP::m; // arc length
  (*st)["scaling"]= element->scaling;

  // quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->scaling*element->k1 / CLHEP::m2;}

#ifdef BDSDEBUG
  G4cout << "Angle (rad) " << (*st)["angle"] / CLHEP::rad   << G4endl;
  G4cout << "Field (T)   " << (*st)["field"] / CLHEP::tesla << G4endl;
#endif
  // geometric face angles (can be different from specification depending on integrator set used)
  G4double incomingFaceAngle = IncomingFaceAngle(element);
  G4double outgoingFaceAngle = OutgoingFaceAngle(element);

  auto sBendLine = BDS::BuildSBendLine(elementName, element, st, brho, integratorSet,
                                       incomingFaceAngle, outgoingFaceAngle,
				       includeFringeFields, prevElement, nextElement);
  
  return sBendLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRBend()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  PoleFaceRotationsNotTooLarge(element);

  // don't check here on whether the possibly next / previous sbend will clash with
  // pole face angles - let that be checked after element construction in the beamline

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double arcLength = 0, chordLength = 0, field = 0, angle = 0;
  CalculateAngleAndFieldRBend(element, arcLength, chordLength, field, angle);
  
  (*st)["angle"]  = angle;
  (*st)["field"]  = field * element->scaling;
  (*st)["by"]     = 1;// bx,by,bz is unit field direction, so (0,1,0) here
  (*st)["length"] = arcLength;
  (*st)["scaling"]= element->scaling;

  // Check the faces won't overlap due to too strong an angle with too short a magnet
  G4double horizontalWidth = PrepareHorizontalWidth(element);
  CheckBendLengthAngleWidthCombo(arcLength, (*st)["angle"], horizontalWidth, elementName);

  // Quadrupole component
  if (BDS::IsFinite(element->k1))
    {(*st)["k1"] = element->scaling * element->k1 / CLHEP::m2;}

  // geometric face angles (can be different from specification depending on integrator set used)
  G4double incomingFaceAngle = IncomingFaceAngle(element);
  G4double outgoingFaceAngle = OutgoingFaceAngle(element);

  // the above in / out face angles are not w.r.t. the local coords - subtract angle/2 to convert
  // this may seem like undoing the += in the functions, but they're used for the beam pipes
  // and matching.
  incomingFaceAngle -= 0.5*angle;
  outgoingFaceAngle -= 0.5*angle;

  BDSLine* rbendline = BDS::BuildRBendLine(elementName, element, prevElement, nextElement,
					   brho, st, integratorSet,
					   incomingFaceAngle, outgoingFaceAngle,
					   includeFringeFields);
  return rbendline;
}
void BDSComponentFactory::GetKickValue(G4double& hkick,
				       G4double& vkick,
				       const KickerType type) const
{
  G4bool kickFinite = BDS::IsFinite(element->kick);
  switch (type)
    {
    case KickerType::horizontal:
      {
	hkick = kickFinite ? element->kick : element->hkick;
	// backwards compatability - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(hkick) && BDS::IsFinite(element->angle))
          {hkick = element->angle;} //+ve to match hkick definition
	vkick = 0;
	break;
      }
    case KickerType::vertical:
      {
	vkick = kickFinite ? element->kick : element->vkick;
	// backwards compatability - if both are zero but angle if finite
	// for this element - use that.
	if (!BDS::IsFinite(vkick) && BDS::IsFinite(element->angle))
          {vkick = element->angle;} //+ve to match vkick definition
	hkick = 0;
	break;
      }
    case KickerType::general:
      {
	hkick = element->hkick;
	vkick = element->vkick;
	// element->kick will be ignored
	if (BDS::IsFinite(element->kick))
	  {
	    G4cout << __METHOD_NAME__ << "Warning: 'kick' defined in element"
		   << "\"" << elementName << "\" but will be ignored as general kicker"
		   << G4endl;
	  }
      }
    default:
      {break;}
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateKicker(KickerType type)
{
  const G4String baseName       = elementName;
  BDSMagnetStrength* st         = new BDSMagnetStrength();
  SetBeta0(st);
  BDSFieldType       fieldType  = BDSFieldType::dipole3d;
  BDSIntegratorType  intType    = BDSIntegratorType::g4classicalrk4; // default
  G4double           chordLength;
  G4double           scaling    = element->scaling;
  G4double           hkick      = 0;
  G4double           vkick      = 0;
  GetKickValue(hkick, vkick, type);
  (*st)["hkick"] = scaling * hkick;
  (*st)["vkick"] = scaling * vkick;

  // create fringe magnet strengths. Copies supplied strength object so it should contain all
  // the kicker strength information as well as the added fringe information
  BDSMagnetStrength* fringeStIn = BDS::GetFringeMagnetStrength(element,
                                                               st,
                                                               0,
                                                               element->e1,
                                                               element->e2,
                                                               element->fintx,
                                                               true);
  BDSMagnetStrength* fringeStOut = new BDSMagnetStrength(*fringeStIn);
  (*fringeStOut)["isentrance"] = false;

  // check if the fringe effect is finite
  G4bool finiteEntrFringe = false;
  G4bool finiteExitFringe = false;
  if (BDS::IsFinite(BDS::FringeFieldCorrection(fringeStIn, true)) ||
          BDS::IsFinite(BDS::SecondFringeFieldCorrection(fringeStIn, true)))
    {finiteEntrFringe = true;}
  if (BDS::IsFinite(BDS::FringeFieldCorrection(fringeStOut, true)) ||
        BDS::IsFinite(BDS::SecondFringeFieldCorrection(fringeStOut, true)))
    {finiteExitFringe = true;}

  // only build the fringe elements if the poleface rotation or fringe field correction terms are finite
  G4bool buildEntranceFringe = false;
  G4bool buildExitFringe     = false;
  if (BDS::IsFinite(element->e1) || finiteEntrFringe)
    {buildEntranceFringe = true;}
  if (BDS::IsFinite(element->e2) || finiteExitFringe)
    {buildExitFringe = true;}
  if (!includeFringeFields)
    {
      buildEntranceFringe = false;
      buildExitFringe = false;
    }

  if(!HasSufficientMinimumLength(element, false)) // false for don't print warning
    {// thin kicker
      fieldType   = BDSFieldType::bzero;
      intType     = BDSIntegratorType::kickerthin;
      chordLength = thinElementLength;

      // Fringe and poleface effects for a thin kicker require an effective bending radius, rho.
      // Lack of length and angle knowledge means the field is the only way rho can be calculated.
      // A zero field would lead to div by zero errors in the integrator constructor, therefore
      // do not replace the kicker magnetstrength with the fringe magnetstrength which should prevent
      // any fringe/poleface effects from ever being applied.
      if (!BDS::IsFinite(element->B))
        {
          // only print warning if a poleface or fringe field effect was specified
          if (buildEntranceFringe || buildExitFringe)
            {
              G4cout << __METHOD_NAME__ << "Warning - finite B field required for kicker pole face and fringe fields,"
                        " effects are unavailable for element ""\"" << elementName << "\"." << G4endl;
            }
        }
      // A thin kicker or tkicker element has possible hkick and vkick combination, meaning the
      // field direction cannot be assumed. Therefore we are unsure of poleface angle and fringe
      // effects so don't replace the kicker magnetstrength with the fringe magnetstrength
      else if (type == KickerType::general)
        {
          // only print warning if a poleface or fringe field effect was specified
          if (buildEntranceFringe || buildExitFringe)
            {
              G4cerr << __METHOD_NAME__ << " Poleface and fringe field effects are unavailable "
                     << "for thin the (t)kicker element ""\"" << elementName << "\"." << G4endl;
            }
        }
      // Good to apply fringe effects.
      else
        {
          // overwrite magnet strength with copy of fringe strength. Should be safe as it has the
          // kicker information from copying previously.
          st = new BDSMagnetStrength(*fringeStIn);
          // supply the field for a thin kicker field as it is required to calculate the
          // effective bending radius needed for fringe field and poleface effects
          (*st)["field"] = element->B * CLHEP::tesla;

          // set field for vertical kickers - element needs rotating as poleface rotation is assumed
          // to be about the vertical axis unless explicitly tilted.
          if (type == KickerType::vertical)
            {(*st)["bx"] = 1;}
          else
            {(*st)["by"] = 1;}
        }
    }
  else
    {// thick kicker
      chordLength = element->l*CLHEP::m;
      // sin(angle) = dP -> angle = sin^-1(dP)
      G4double angleX = std::asin(hkick * scaling);
      G4double angleY = std::asin(vkick * scaling);

      // Setup result variables - 'x' and 'y' refer to the components along the direction
      // the particle will change. These will therefore not be Bx and By.
      G4double fieldX = 0;
      G4double fieldY = 0;

      // if B is specified and hkick and vkick (including backwards compatible check on
      // 'angle') are not, then use the field for the appropriate component
      // can only be 1d in this case -> doesn't work for tkicker
      if (BDS::IsFinite(element->B) && (!BDS::IsFinite(hkick) && !BDS::IsFinite(vkick)))
	{
	  switch (type)
	    {// 'X' and 'Y' are the angle of bending here, not the B field direction.
	    case KickerType::horizontal:
	    case KickerType::general:
	      {fieldX = element->B * CLHEP::tesla; break;}
	    case KickerType::vertical:
	      {fieldY = element->B * CLHEP::tesla; break;}
	    default:
	      {break;} // do nothing - no field - just for compiler warnings
	    }
	}
      else
	{
	  if (BDS::IsFinite(angleX))
	    {// with comments
	      // calculate the chord length of the arc through the field from the straight
	      // ahead length for this element which here is 'chordLength'.
	      G4double fieldChordLengthX = chordLength / std::cos(0.5*angleX);
	      
	      // now calculate the bending radius
	      G4double bendingRadiusX = fieldChordLengthX * 0.5 / sin(std::abs(angleX) * 0.5);
	      
	      // no calculate the arc length of the trajectory based on each bending radius
	      G4double arcLengthX = std::abs(bendingRadiusX * angleX);
	      
	      // -ve here in horizontal only for convention matching
	      fieldX = FieldFromAngle(-angleX, arcLengthX);
	    } // else fieldX default is 0
	  
	  if (BDS::IsFinite(angleY))
	    {// same as x, no need for comments
	      G4double fieldChordLengthY = chordLength / std::cos(0.5*angleY);
	      G4double bendingRadiusY    = fieldChordLengthY * 0.5 / sin(std::abs(angleY) * 0.5);
	      G4double arcLengthY        = std::abs(bendingRadiusY * angleY);
	      fieldY = FieldFromAngle(angleY,  arcLengthY);
	    }
	}
      
      // note field for kick in x is unit Y, hence B = (y,x,0) here
      G4ThreeVector     field = G4ThreeVector(fieldY, fieldX, 0);
      G4double       fieldMag = field.mag();
      G4ThreeVector unitField = field.unit();
      
      (*st)["field"] = fieldMag;
      (*st)["bx"]    = unitField.x();
      (*st)["by"]    = unitField.y();
    }

  (*fringeStIn) ["bx"] = (*st)["bx"];
  (*fringeStIn) ["by"] = (*st)["by"];
  (*fringeStOut)["bx"] = (*st)["bx"];
  (*fringeStOut)["by"] = (*st)["by"];

  BDSMagnetType t;
  G4double defaultVHRatio = 1.5;
  switch (type)
    {
    case KickerType::horizontal:
    case KickerType::general:
      {t = BDSMagnetType::hkicker; break;}
    case KickerType::vertical:
      {
	t = BDSMagnetType::vkicker;
	defaultVHRatio = 1./defaultVHRatio; // inverted for vertical magnet
	break;
      }
    default:
      {t = BDSMagnetType::hkicker; break;}
    }

  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);

  G4bool yokeOnLeft = YokeOnLeft(element, st);
  auto bpInf = PrepareBeamPipeInfo(element);
  
  // Decide on a default horizontalWidth for the kicker - try 0.3x ie smaller kicker
  // than typical magnet, but if that would not fit around the beam pipe - go back to
  // the default horizontalWidth. Code further along will warn if it still doesn't fit.
  const G4double globalDefaultHW = BDSGlobalConstants::Instance()->HorizontalWidth();
  G4double defaultHorizontalWidth = 0.3 * globalDefaultHW;
  BDSExtent bpExt = bpInf->Extent();
  G4double bpDX = bpExt.DX();
  G4double bpDY = bpExt.DY();
  if (bpDX > defaultHorizontalWidth && bpDX < globalDefaultHW)
    {defaultHorizontalWidth = globalDefaultHW;}
  else if (bpDY > defaultHorizontalWidth && bpDY > globalDefaultHW)
    {defaultHorizontalWidth = globalDefaultHW;}
  
  auto magOutInf = PrepareMagnetOuterInfo(elementName, element, 0, 0, bpInf, yokeOnLeft,
					  defaultHorizontalWidth, defaultVHRatio, 0.9);

  if(!HasSufficientMinimumLength(element, false))
    {
      // fringe effect applied in integrator so nothing more to do.
      return new BDSMagnet(t,
			   baseName,
			   chordLength,
			   bpInf,
			   magOutInf,
			   vacuumField);
    }
  else
    {
      BDSLine* kickerLine = new BDSLine(baseName);
      // subtract fringe length from kicker to preserve element length
      G4double kickerChordLength = chordLength;
      if (buildEntranceFringe)
        {kickerChordLength -= thinElementLength;}
      if (buildExitFringe)
        {kickerChordLength -= thinElementLength;}

      if (buildEntranceFringe)
        {
          G4String entrFringeName = baseName + "_e1_fringe";
          BDSMagnet* startfringe = BDS::BuildDipoleFringe(element, 0, 0,
                                                          entrFringeName,
                                                          fringeStIn,
							  brho,
                                                          integratorSet,
							  fieldType);
          kickerLine->AddComponent(startfringe);
        }
      
      G4String kickerName = baseName + "_centre";
      BDSMagnet* kicker = new BDSMagnet(t,
					kickerName,
					kickerChordLength,
					bpInf,
					magOutInf,
					vacuumField);
      kickerLine->AddComponent(kicker);
      
      if (buildEntranceFringe)
	{
          G4String exitFringeName = baseName + "_e2_fringe";
          BDSMagnet* endfringe = BDS::BuildDipoleFringe(element, 0, 0,
                                                        exitFringeName,
                                                        fringeStOut, brho,
                                                        integratorSet, fieldType);
          kickerLine->AddComponent(endfringe);
        }
      return kickerLine;
    }
}

BDSAcceleratorComponent* BDSComponentFactory::CreateQuad()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["k1"] = element->k1 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::quadrupole, BDSMagnetType::quadrupole);
}  
  
BDSAcceleratorComponent* BDSComponentFactory::CreateSextupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["k2"] = element->k2 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::sextupole, BDSMagnetType::sextupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateOctupole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["k3"] = element->k3 * element->scaling;

  return CreateMagnet(element, st, BDSFieldType::octupole, BDSMagnetType::octupole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDecapole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["k4"] = element->k4 * element->scaling;
  
  return CreateMagnet(element, st, BDSFieldType::decapole, BDSMagnetType::decapole);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMultipole()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  
  return CreateMagnet(element, st, BDSFieldType::multipole, BDSMagnetType::multipole,
		      (*st)["angle"]); // multipole could bend beamline
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinMultipole(G4double angleIn)
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForMultipoles(element);
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, -angleIn);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(elementName, element,
							       -angleIn, angleIn, beamPipeInfo);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSIntegratorType intType = integratorSet->multipoleThin;
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::multipole,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);
  
  BDSMagnet* thinMultipole =  new BDSMagnet(BDSMagnetType::thinmultipole,
					    elementName,
					    thinElementLength,
					    beamPipeInfo,
					    magnetOuterInfo,
					    vacuumField);
  
  thinMultipole->SetExtent(BDSExtent(beamPipeInfo->aper1,
				     beamPipeInfo->aper1,
				     thinElementLength*0.5));

  return thinMultipole;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateElement()
{
  if(!HasSufficientMinimumLength(element)) 
    {return nullptr;}
  
  return (new BDSElement(elementName,
			 element->l * CLHEP::m,
			 PrepareHorizontalWidth(element),
			 element->geometryFile,
			 element->fieldAll,
			 element->angle * CLHEP::rad));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateSolenoid()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["bz"]    = 1;
  const G4double scaling = element->scaling;
  if (BDS::IsFinite(element->B))
    {
      (*st)["field"] = scaling * element->B * CLHEP::tesla;
      (*st)["ks"]    = (*st)["field"] / brho;
    }
  else
    {
      (*st)["field"] = (scaling * element->ks / CLHEP::m) * brho;
      (*st)["ks"]    = element->ks;
    }

  if (!BDS::IsFinite((*st)["field"]) || !includeFringeFields)
    {// ie no strength solenoid - don't bother with fringe effects
      return CreateMagnet(element, st, BDSFieldType::solenoid, BDSMagnetType::solenoid);
    }
  
  // lambda to help - sign convention - this is the 'entry' version
  auto strength = [](G4double phi){
		    BDSMagnetStrength* s = new BDSMagnetStrength();
		    (*s)["rmat11"] = 1;
		    (*s)["rmat22"] = 1;
		    (*s)["rmat33"] = 1;
		    (*s)["rmat44"] = 1;
		    (*s)["rmat41"] = -phi;
		    (*s)["rmat23"] = phi;
		    return s;
		  };

  G4bool buildIncomingFringe = true;
  if (prevElement) // could be nullptr
    {// only build fringe if previous element isn't another solenoid
      buildIncomingFringe = prevElement->type != ElementType::_SOLENOID;
    }
  G4bool buildOutgoingFringe = true;
  if (nextElement) // could be nullptr
    {// only build fringe if next element isn't another solenoid
      buildOutgoingFringe = nextElement->type != ElementType::_SOLENOID;
    }

  G4double solenoidBodyLength = element->l * CLHEP::m;

  if (buildIncomingFringe)
    {solenoidBodyLength -= thinElementLength;}
  if (buildOutgoingFringe)
    {solenoidBodyLength -= thinElementLength;}

  // scale factor to account for reduced body length due to fringe placement.
  G4double lengthScaling = solenoidBodyLength / (element->l * CLHEP::m);
  G4double s = 0.5*(*st)["ks"] * lengthScaling; // already includes scaling
  BDSLine* bLine = new BDSLine(elementName);

  if (buildIncomingFringe)
    {
      auto stIn        = strength(s);
      auto solenoidIn  = CreateThinRMatrix(0, stIn, elementName + "_fringe_in");
      bLine->AddComponent(solenoidIn);
    }

  // Do not use CreateMagnet method as solenoid body length needs to be reduced to conserve total
  // element length. The solenoid strength is scaled accordingly.

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::solenoid);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::solenoid,
                                               brho,
                                               intType,
                                               st,
                                               true,
                                               fieldTrans);

  BDSMagnetOuterInfo* outerInfo = PrepareMagnetOuterInfo(elementName + "_centre", element, st, bpInfo);
  vacuumField->SetScalingRadius(outerInfo->innerRadius); // purely for completeness of information - not required
  BDSFieldInfo* outerField = nullptr;

  // only make a default multipolar field if the yokeFields flag is on and
  // there isn't an 'outerField' specified for the element
  G4bool externalOuterField = !(element->fieldOuter.empty());
  if (yokeFields && !externalOuterField)
    {outerField = PrepareMagnetOuterFieldInfo(st, BDSFieldType::solenoid, bpInfo, outerInfo, fieldTrans);}

  auto solenoid = new BDSMagnet(BDSMagnetType::solenoid,
                         elementName + "_centre",
                         solenoidBodyLength,
                         bpInfo,
                         outerInfo,
                         vacuumField,
                         0,
                         outerField);

  bLine->AddComponent(solenoid);

  if (buildOutgoingFringe)
    {
      auto stOut       = strength(-s);
      auto solenoidOut = CreateThinRMatrix(0, stOut, elementName + "_fringe_out");
      bLine->AddComponent(solenoidOut);
    }
  
  return bLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateParallelTransporter()
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  return CreateMagnet(element, st, BDSFieldType::paralleltransporter, BDSMagnetType::paralleltransporter);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRectangularCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  return new BDSCollimatorRectangular(elementName,
				      element->l*CLHEP::m,
				      PrepareHorizontalWidth(element),
				      PrepareMaterial(element),
				      PrepareVacuumMaterial(element),
				      element->xsize*CLHEP::m,
				      element->ysize*CLHEP::m,
				      element->xsizeOut*CLHEP::m,
				      element->ysizeOut*CLHEP::m,
				      PrepareColour(element));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateEllipticalCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  return new BDSCollimatorElliptical(elementName,
				     element->l*CLHEP::m,
				     PrepareHorizontalWidth(element),
				     PrepareMaterial(element),
				     PrepareVacuumMaterial(element),
				     element->xsize*CLHEP::m,
				     element->ysize*CLHEP::m,
				     element->xsizeOut*CLHEP::m,
				     element->ysizeOut*CLHEP::m,
				     PrepareColour(element));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateJawCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  return new BDSCollimatorJaw(elementName,
			      element->l*CLHEP::m,
			      PrepareHorizontalWidth(element),
                              element->xsize*CLHEP::m,
                              element->ysize*CLHEP::m,
                              element->xsizeLeft*CLHEP::m,
                              element->xsizeRight*CLHEP::m,
			      true,
			      true,
			      PrepareMaterial(element),
			      PrepareVacuumMaterial(element),
			      PrepareColour(element));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateMuonSpoiler()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = element->scaling * element->B * CLHEP::tesla;
  BDSIntegratorType intType = integratorSet->Integrator(BDSFieldType::muonspoiler);
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  BDSFieldInfo* outerField = new BDSFieldInfo(BDSFieldType::muonspoiler,
					      brho,
					      intType,
					      st,
					      true,
					      fieldTrans);
  BDSFieldInfo* vacuumField = new BDSFieldInfo();

  auto bpInfo = PrepareBeamPipeInfo(element);
  
  return new BDSMagnet(BDSMagnetType::muonspoiler,
		       elementName,
		       element->l*CLHEP::m,
		       bpInfo,
		       PrepareMagnetOuterInfo(elementName, element, st, bpInfo),
		       vacuumField,
		       0,
		       outerField);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateShield()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);

  G4Colour* colour = PrepareColour(element);
  G4Material* material = PrepareMaterial(element, "concrete");

  BDSShield* shield = new BDSShield(elementName,
				    element->l*CLHEP::m,
				    PrepareHorizontalWidth(element),
				    element->xsize*CLHEP::m,
				    element->ysize*CLHEP::m,
				    material,
				    colour,
				    bpInfo);
  return shield;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDegrader()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  G4double degraderOffset;
  if ((element->materialThickness <= 0) && (element->degraderOffset <= 0))
    {
        G4cerr << __METHOD_NAME__ << "Error: Both \"materialThickness\" and \"degraderOffset\" are either undefined or <= 0" <<  G4endl;
        exit(1);
    }

  if ((element->materialThickness <= 0) && (element->degraderOffset > 0))
    {degraderOffset = element->degraderOffset*CLHEP::m;}
  else
    {
      //Width of wedge base
      G4double wedgeBasewidth = (element->l*CLHEP::m /element->numberWedges) - lengthSafety;
      
      //Angle between hypotenuse and height (in the triangular wedge face)
      G4double theta = std::atan(wedgeBasewidth / (2.0*element->wedgeLength*CLHEP::m));
      
      //Overlap distance of wedges
      G4double overlap = (element->materialThickness*CLHEP::m/element->numberWedges - wedgeBasewidth) * (std::sin(CLHEP::halfpi - theta) / std::sin(theta));
      
      degraderOffset = overlap * -0.5;
    }

  return (new BDSDegrader(elementName,
			  element->l*CLHEP::m,
			  PrepareHorizontalWidth(element),
			  element->numberWedges,
			  element->wedgeLength*CLHEP::m,
			  element->degraderHeight*CLHEP::m,
			  degraderOffset,
			  PrepareMaterial(element, "carbon"),
			  PrepareColour(element)));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateWireScanner()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  G4ThreeVector wireOffset = G4ThreeVector(element->wireOffsetX * CLHEP::m,
					   element->wireOffsetY * CLHEP::m,
					   element->wireOffsetZ * CLHEP::m);
  
  return (new BDSWireScanner(elementName,
			     element->l*CLHEP::m,
			     PrepareBeamPipeInfo(element),
			     PrepareMaterial(element, "carbon"),
			     element->wireDiameter*CLHEP::m,
			     element->wireLength*CLHEP::m,
			     element->angle*CLHEP::rad,
			     wireOffset));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateUndulator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  const BDSFieldType undField = BDSFieldType::undulator;

  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(undField);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  (*st)["length"] = element->undulatorPeriod * CLHEP::m;
  (*st)["field"] = element->scaling * element->B * CLHEP::tesla;

  BDSFieldInfo* vacuumFieldInfo = new BDSFieldInfo(undField,
                                                   brho,
                                                   intType,
                                                   st,
                                                   true,
                                                   fieldTrans);
  //BDSFieldInfo* outerFieldInfo = PrepareMagnetOuterFieldInfo(st, undField, bpInfo, 0, fieldTrans);
  BDSFieldInfo* outerFieldInfo = nullptr;
  // limit step length in field - crucial to this component
  // to get the motion correct this has to be less than one oscillation
  auto defaultUL = BDSGlobalConstants::Instance()->DefaultUserLimits();
  G4double limit = (*st)["length"] * 0.075;
  auto ul = BDS::CreateUserLimits(defaultUL, limit, 1.0);
  if (ul != defaultUL)
    {vacuumFieldInfo->SetUserLimits(ul);}

  G4Transform3D newFieldTransform = vacuumFieldInfo->Transform();
  vacuumFieldInfo->SetTransform(newFieldTransform);

  return (new BDSUndulator(elementName,
			   element->l * CLHEP::m,
			   element->undulatorPeriod * CLHEP::m,
			   element->undulatorMagnetHeight * CLHEP::m,
			   PrepareHorizontalWidth(element),
			   element->undulatorGap * CLHEP::m,
			   bpInfo,
			   vacuumFieldInfo,
			   outerFieldInfo,
			   element->material));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateDump()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  G4bool circular = false;
  G4String apertureType = G4String(element->apertureType);
  if (apertureType == "circular")
    {circular = true;}
  else if (apertureType != "rectangular" && !apertureType.empty())
    {
      G4cout << __METHOD_NAME__ << "unknown shape for dump: \"" << apertureType << "\"" << G4endl;
      exit(1);
    }

  G4double defaultHorizontalWidth = 40*CLHEP::cm;
  G4double horizontalWidth = PrepareHorizontalWidth(element, defaultHorizontalWidth);
  BDSDump* result = new BDSDump(elementName,
				element->l*CLHEP::m,
				horizontalWidth,
				circular);
  return result;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateGap()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  return (new BDSGap(elementName,
                     element->l*CLHEP::m,
                     element->angle*CLHEP::rad));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateCrystalCollimator()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

  BDSCrystalInfo* left = nullptr;
  BDSCrystalInfo* right = nullptr;
  if (!element->crystalBoth.empty())
    {
      left  = PrepareCrystalInfo(G4String(element->crystalBoth));
      right = PrepareCrystalInfo(G4String(element->crystalBoth));
    }
  else if (element->crystalBoth.empty() && !element->crystalRight.empty() && !element->crystalLeft.empty())
    {
      left  = PrepareCrystalInfo(G4String(element->crystalLeft));
      right = PrepareCrystalInfo(G4String(element->crystalRight));
    }
  else if (element->crystalRight.empty())
    {left  = PrepareCrystalInfo(G4String(element->crystalLeft));}
  else
    {right = PrepareCrystalInfo(G4String(element->crystalRight));}

  if (left && !right &&  BDS::IsFinite(element->crystalAngleYAxisRight))
    {
      G4cout << G4endl << G4endl << __METHOD_NAME__
             << "Left crystal being used but right angle set - perhaps check input for element "
             << elementName << G4endl << G4endl;
    }
  if (!left && right &&  BDS::IsFinite(element->crystalAngleYAxisLeft))
    {
      G4cout << G4endl << G4endl << __METHOD_NAME__
	     << "Right crystal being used but left angle set - perhaps check input for element "
	     << elementName << G4endl << G4endl;
    }
  
  return (new BDSCollimatorCrystal(elementName,
				   element->l*CLHEP::m,
				   PrepareBeamPipeInfo(element),
				   left,
				   right,
				   element->xsize*CLHEP::m, // symmetric for now
				   element->xsize*CLHEP::m,
				   element->crystalAngleYAxisLeft*CLHEP::rad,
				   element->crystalAngleYAxisRight*CLHEP::rad));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateLaser()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}

    BDSLaser* laser = PrepareLaser(element);
    G4double length = element->l*CLHEP::m;
    G4double lambda = laser->Wavelength()*CLHEP::m;
	
  G4ThreeVector direction = G4ThreeVector(element->xdir,element->ydir,element->zdir);
  G4ThreeVector position  = G4ThreeVector(0,0,0);
	
  return (new BDSLaserWire(elementName, length, lambda, direction) );       
}

BDSAcceleratorComponent* BDSComponentFactory::CreateScreen()
{
  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
	
  G4TwoVector size;
  size.setX(element->screenXSize*CLHEP::m);
  size.setY(element->screenYSize*CLHEP::m);
  G4cout << __METHOD_NAME__ << " - size = " << size << G4endl;
  
  BDSScreen* theScreen = new BDSScreen( elementName,
					element->l*CLHEP::m,
					PrepareBeamPipeInfo(element),
					size,
					element->angle); 
  if(element->layerThicknesses.size() != element->layerMaterials.size())
    {
      G4cerr << __METHOD_NAME__ << "Element \"" << elementName << "\" must have the "
	     << "same number of materials as layers - check 'layerMaterials'" << G4endl;
      exit(1);
    }

  if(element->layerThicknesses.size() == 0 )
    {
      G4cerr << __METHOD_NAME__ << "Element: \"" << elementName
	     << "\" has 0 screen layers" << G4endl;
      exit(1);
    }
  
  std::list<std::string>::const_iterator itm;
  std::list<double>::const_iterator itt;
  std::list<int>::const_iterator itIsSampler;
  for(itt = element->layerThicknesses.begin(),
	itm = element->layerMaterials.begin(),
	itIsSampler = element->layerIsSampler.begin();
      itt != element->layerThicknesses.end();
      ++itt, ++itm)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << " - screen layer: thickness: " 
	     << *(itt) << ", material "  << (*itm)
	     <<	", isSampler: "  << (*itIsSampler) << G4endl;
#endif
      if(element->layerIsSampler.size()>0)
	{
	  theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm, *itIsSampler);
	  ++itIsSampler;
	}
      else
	{theScreen->AddScreenLayer((*itt)*CLHEP::m, *itm);}
    }
  return theScreen;
}

#ifdef USE_AWAKE
BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeScreen()
{
  return (new BDSAwakeScintillatorScreen(elementName,
					 element->scintmaterial,
					 element->tscint*CLHEP::m,
					 element->windowScreenGap*CLHEP::m,
					 element->angle,
					 element->twindow*CLHEP::m,
					 element->windowmaterial));
}

BDSAcceleratorComponent* BDSComponentFactory::CreateAwakeSpectrometer()
{
  BDSFieldInfo* awakeField = nullptr;
  if (element->fieldAll.empty())
    {
      BDSMagnetStrength* awakeStrength = new BDSMagnetStrength();
      SetBeta0(awakeStrength);
      (*awakeStrength)["field"] = element->scaling * element->B * CLHEP::tesla;
      (*awakeStrength)["by"]    = 1; // bx,by,bz is unit field direction, so (0,1,0) here

      G4Transform3D fieldTrans = CreateFieldTransform(element);
      awakeField = new BDSFieldInfo(BDSFieldType::dipole,
				    brho,
				    BDSIntegratorType::g4classicalrk4,
				    awakeStrength,
				    true,
				    fieldTrans);
    }
  else
    {awakeField = BDSFieldFactory::Instance()->GetDefinition(element->fieldAll);}
  return (new BDSAwakeSpectrometer(elementName,
                                   element->awakeMagnetOffsetX*CLHEP::m,
				   element->l*CLHEP::m,
				   awakeField,
				   element->poleStartZ*CLHEP::m,
				   element->scintmaterial,
				   element->tscint*CLHEP::m,
				   element->screenPSize*CLHEP::m,
				   element->windowScreenGap*CLHEP::m,
				   element->angle,
				   element->twindow*CLHEP::m,
				   element->windowmaterial,
				   element->tmount*CLHEP::m,
				   element->mountmaterial,
				   element->screenEndZ*CLHEP::m,
				   element->spec,
				   element->screenWidth*CLHEP::m));
}
#endif // end of USE_AWAKE

BDSAcceleratorComponent* BDSComponentFactory::CreateTransform3D()
{
  return (new BDSTransform3D( elementName,
			      element->xdir *CLHEP::m,
			      element->ydir *CLHEP::m,
			      element->zdir *CLHEP::m,
			      element->phi *CLHEP::rad,
			      element->theta *CLHEP::rad,
			      element->psi *CLHEP::rad ) );
	
}

BDSAcceleratorComponent* BDSComponentFactory::CreateTerminator()
{
#ifdef BDSDEBUG
  G4cout << "---->creating Terminator" << G4endl;
#endif
  return new BDSTerminator();
}

BDSAcceleratorComponent* BDSComponentFactory::CreateRMatrix()
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForRMatrix(element);

  GMAD::Element* elementNew = new GMAD::Element(*element);
  elementNew->l = (element->l-thinElementLength)/2.0;

  auto parallelTransport1 = CreateMagnet(elementNew, st, BDSFieldType::paralleltransporter, BDSMagnetType::paralleltransporter);
  auto rmatrix            = CreateThinRMatrix(0, elementName + "_centre");
  auto parallelTransport2 = CreateMagnet(elementNew, st, BDSFieldType::paralleltransporter, BDSMagnetType::paralleltransporter);

  const G4String baseName = elementName;
  BDSLine* bLine = new BDSLine(baseName);
  bLine->AddComponent(parallelTransport1);
  bLine->AddComponent(rmatrix);
  bLine->AddComponent(parallelTransport2);

  return bLine;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinRMatrix(G4double angleIn,
								G4String name)
{
  BDSMagnetStrength* st = PrepareMagnetStrengthForRMatrix(element);
  return CreateThinRMatrix(angleIn, st, name);
}

BDSAcceleratorComponent* BDSComponentFactory::CreateThinRMatrix(G4double angleIn,
								const BDSMagnetStrength* st,
								G4String name)
{
  BDSBeamPipeInfo* beamPipeInfo = PrepareBeamPipeInfo(element, angleIn, -angleIn);
  beamPipeInfo->beamPipeType = BDSBeamPipeType::circularvacuum;
  BDSMagnetOuterInfo* magnetOuterInfo = PrepareMagnetOuterInfo(name, element,
                                                               -angleIn, angleIn, beamPipeInfo);
  magnetOuterInfo->geometryType = BDSMagnetGeometryType::none;

  BDSIntegratorType intType = integratorSet->rmatrixThin;
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(BDSFieldType::rmatrix,
                                               brho,
                                               intType,
                                               st,
                                               true,
                                               fieldTrans);
  vacuumField->SetBeamPipeRadius(beamPipeInfo->aper1);

  BDSMagnet* thinRMatrix =  new BDSMagnet(BDSMagnetType::rmatrix,
                                          name,
                                          thinElementLength,
                                          beamPipeInfo,
                                          magnetOuterInfo,
                                          vacuumField);

  thinRMatrix->SetExtent(BDSExtent(beamPipeInfo->aper1,
                                   beamPipeInfo->aper1,
                                   thinElementLength*0.5));

  return thinRMatrix;
}

BDSAcceleratorComponent* BDSComponentFactory::CreateLaserWire()
{

  if(!HasSufficientMinimumLength(element))
    {return nullptr;}
  
  BDSLaser* laser = PrepareLaser(element);
  
  G4ThreeVector laserOffset = G4ThreeVector(element->laserOffsetX * CLHEP::m,
					    element->laserOffsetY * CLHEP::m,
					    element->laserOffsetZ * CLHEP::m);


  return (new BDSLaserWireNew(elementName,
			      element->l*CLHEP::m,
			      PrepareBeamPipeInfo(element),
			      laser,
			      10.0*laser->Sigma0(),
			      element->wireLength*CLHEP::m,
			      element->laserOffsetTheta*CLHEP::rad,
			      element->laserOffsetPhi*CLHEP::rad,
			      laserOffset,
			      BDSColours::Instance()->GetColour("red"))
	  );
}

BDSMagnet* BDSComponentFactory::CreateMagnet(const GMAD::Element* el,
					     BDSMagnetStrength*   st,
					     BDSFieldType         fieldType,
					     BDSMagnetType        magnetType,
					     G4double             angle,
					     G4String             nameSuffix) const
{
  BDSBeamPipeInfo* bpInfo = PrepareBeamPipeInfo(element);
  BDSIntegratorType intType = integratorSet->Integrator(fieldType);
  G4Transform3D fieldTrans  = CreateFieldTransform(element);
  BDSFieldInfo* vacuumField = new BDSFieldInfo(fieldType,
					       brho,
					       intType,
					       st,
					       true,
					       fieldTrans);

  BDSMagnetOuterInfo* outerInfo = PrepareMagnetOuterInfo(elementName + nameSuffix, element, st, bpInfo);
  vacuumField->SetScalingRadius(outerInfo->innerRadius); // purely for completeness of information - not required
  BDSFieldInfo* outerField = nullptr;

  // only make a default multipolar field if the yokeFields flag is on and
  // there isn't an 'outerField' specified for the element
  G4bool externalOuterField = !(el->fieldOuter.empty());
  if (yokeFields && !externalOuterField)
    {outerField = PrepareMagnetOuterFieldInfo(st, fieldType, bpInfo, outerInfo, fieldTrans);}

  return new BDSMagnet(magnetType,
		       elementName + nameSuffix,
		       el->l * CLHEP::m,
		       bpInfo,
		       outerInfo,
		       vacuumField,
		       angle,
		       outerField);
}

G4bool BDSComponentFactory::HasSufficientMinimumLength(Element const* el,
						       const G4bool& printWarning)
{
  if(el->l < 1e-7) // 'l' already in metres from parser
    {
      if (printWarning)
	{
	  G4cerr << "---> NOT creating element \"" << el->name << "\""
		 << " LENGTH TOO SHORT:" << " l = " << el->l << "m" << G4endl; // already in m
	}
      return false;
    }
  else
    {return true;}
}

void BDSComponentFactory::PoleFaceRotationsNotTooLarge(Element const* element,
						       G4double maxAngle)
{
  if (std::abs(element->e1) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e1: " << element->e1 << " is greater than " << maxAngle << G4endl;
      exit(1);
    }
  if (std::abs(element->e2) > maxAngle)
    {
      G4cerr << __METHOD_NAME__ << "Pole face angle e2: " << element->e2 << " is greater than " << maxAngle << G4endl;
      exit(1);
    }
}

G4bool BDSComponentFactory::YokeOnLeft(const Element*           element,
				       const BDSMagnetStrength* st)
{
  G4double angle    = (*st)["angle"];
  G4double hkickAng = -(*st)["hkick"]; // not really angle but proportional in the right direction
  G4double vkickAng = -(*st)["vkick"];
  if (!BDS::IsFinite(angle) && BDS::IsFinite(hkickAng))
    {angle = hkickAng;}
  if (!BDS::IsFinite(angle) && BDS::IsFinite(vkickAng))
    {angle = vkickAng;}
  G4bool yokeOnLeft;
  if ((angle < 0) && (element->yokeOnInside))
    {yokeOnLeft = true;}
  else if ((angle > 0) && (!(element->yokeOnInside)))
    {yokeOnLeft = true;}
  else
    {yokeOnLeft = false;}
  return yokeOnLeft;
}

BDSFieldInfo* BDSComponentFactory::PrepareMagnetOuterFieldInfo(const BDSMagnetStrength*  vacuumSt,
							       const BDSFieldType&       fieldType,
							       const BDSBeamPipeInfo*    bpInfo,
							       const BDSMagnetOuterInfo* outerInfo,
							       const G4Transform3D&      fieldTransform) const
{  
  BDSFieldType outerType;
  switch (fieldType.underlying())
    {
    case BDSFieldType::dipole:
      {outerType = BDSFieldType::multipoleouterdipole;    break;}
    case BDSFieldType::quadrupole:
      {outerType = BDSFieldType::multipoleouterquadrupole; break;}
    case BDSFieldType::sextupole:
      {outerType = BDSFieldType::multipoleoutersextupole;  break;}
    case BDSFieldType::octupole:
      {outerType = BDSFieldType::multipoleouteroctupole;   break;}
    case BDSFieldType::decapole:
      {outerType = BDSFieldType::multipoleouterdecapole;   break;}
    case BDSFieldType::skewquadrupole:
      {outerType = BDSFieldType::skewmultipoleouterquadrupole; break;}
    case BDSFieldType::skewsextupole:
      {outerType = BDSFieldType::skewmultipoleoutersextupole;  break;}
    case BDSFieldType::skewoctupole:
      {outerType = BDSFieldType::skewmultipoleouteroctupole;   break;}
    case BDSFieldType::skewdecapole:
      {outerType = BDSFieldType::skewmultipoleouterdecapole;   break;}
    case BDSFieldType::dipole3d:
      {outerType = BDSFieldType::multipoleouterdipole3d; break;}
    case BDSFieldType::solenoid:
      {outerType = BDSFieldType::multipoleouterdipole3d; break;}
    default:
      {return nullptr; break;} // no possible outer field for any other magnet types
    }

  BDSMagnetStrength* stCopy = new BDSMagnetStrength(*vacuumSt);
  BDSIntegratorType intType = integratorSet->Integrator(outerType);
  BDSFieldInfo* outerField  = new BDSFieldInfo(outerType,
					       brho,
					       intType,
					       stCopy,
					       true,
					       fieldTransform);

  outerField->SetChordStepMinimum(BDSGlobalConstants::Instance()->ChordStepMinimumYoke());
  if (outerInfo)
    {outerField->SetScalingRadius(outerInfo->innerRadius);}
  if (bpInfo)
    {outerField->SetBeamPipeRadius(bpInfo->IndicativeRadius());}
  return outerField;
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const G4String& elementNameIn,
								const Element* el,
								const BDSMagnetStrength* st,
								const BDSBeamPipeInfo* beamPipe,
								G4double defaultHorizontalWidth,
								G4double defaultVHRatio,
								G4double defaultCoilWidthFraction,
								G4double defaultCoilHeightFraction)
{
  G4bool yokeOnLeft = YokeOnLeft(el,st);
  G4double    angle = (*st)["angle"];
  
  return PrepareMagnetOuterInfo(elementNameIn, el, 0.5*angle, 0.5*angle, beamPipe, yokeOnLeft,
				defaultHorizontalWidth, defaultVHRatio, defaultCoilWidthFraction,
				defaultCoilHeightFraction);
}

BDSMagnetOuterInfo* BDSComponentFactory::PrepareMagnetOuterInfo(const G4String& elementNameIn,
								const Element*  el,
								const G4double  angleIn,
								const G4double  angleOut,
								const BDSBeamPipeInfo* beamPipe,
								const G4bool    yokeOnLeft,
								G4double        defaultHorizontalWidth,
								G4double        defaultVHRatio,
								G4double        defaultCoilWidthFraction,
								G4double        defaultCoilHeightFraction)
{
  BDSMagnetOuterInfo* info = new BDSMagnetOuterInfo();

  const BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  // name
  info->name = elementNameIn;
  
  // magnet geometry type
 if (el->magnetGeometryType == "" || globals->IgnoreLocalMagnetGeometry())
   {info->geometryType = globals->MagnetGeometryType();}
  else
    {
      info->geometryType = BDS::DetermineMagnetGeometryType(el->magnetGeometryType);
      info->geometryTypeAndPath = el->magnetGeometryType;
    }

  // set face angles w.r.t. chord
  info->angleIn  = angleIn;
  info->angleOut = angleOut;
  
  // horizontal width
  info->horizontalWidth = PrepareHorizontalWidth(el, defaultHorizontalWidth);

  // inner radius of magnet geometry - TBC when poles can be built away from beam pipe
  info->innerRadius = beamPipe->IndicativeRadius();

  // outer material
  G4Material* outerMaterial;
  if(el->material == "")
    {
      G4String defaultMaterialName = globals->OuterMaterialName();
      outerMaterial = BDSMaterials::Instance()->GetMaterial(defaultMaterialName);
    }
  else
    {outerMaterial = BDSMaterials::Instance()->GetMaterial(el->material);}
  info->outerMaterial = outerMaterial;

  // yoke direction
  info->yokeOnLeft = yokeOnLeft;

  if (el->hStyle < 0) // it's unset
    {info->hStyle = globals->HStyle();}
  else
    {info->hStyle = G4bool(el->hStyle);} // convert from int to bool

  if (el->vhRatio > 0)
    {info->vhRatio = G4double(el->vhRatio);}
  else if (globals->VHRatio() > 0)
    {info->vhRatio = globals->VHRatio();}
  else if (defaultVHRatio > 0) // allow calling function to supply optional default
    {info->vhRatio = defaultVHRatio;}
  else
    {info->vhRatio = info->hStyle ? 0.8 : 1.0;} // h default : c default
  
  if (el->coilWidthFraction > 0)
    {info->coilWidthFraction = G4double(el->coilWidthFraction);}
  else if (globals->CoilWidthFraction() > 0)
    {info->coilWidthFraction = globals->CoilWidthFraction();}
  else if (defaultCoilHeightFraction > 0) // allow calling function to supply optional default
    {info->coilWidthFraction = defaultCoilWidthFraction;}
  else
    {info->coilWidthFraction = info->hStyle ? 0.8 : 0.65;} // h default : c default

  if (el->coilHeightFraction > 0)
    {info->coilHeightFraction = G4double(el->coilHeightFraction);}
  else if (globals->CoilHeightFraction() > 0)
    {info->coilHeightFraction = globals->CoilHeightFraction();}
  else if (defaultCoilHeightFraction > 0) // allow calling function to supply optional default
    {info->coilHeightFraction = defaultCoilHeightFraction;}
  else
    {info->coilHeightFraction = 0.8;} // default for both h and c type

  info->colour = PrepareColour(el);
  
  return info;
}

G4double BDSComponentFactory::PrepareHorizontalWidth(Element const* el,
						     G4double defaultHorizontalWidth)
{
  G4double horizontalWidth = el->horizontalWidth*CLHEP::m;
  if (horizontalWidth < 1e-6)
    {//horizontalWidth not set - use either global or specified default
      if (defaultHorizontalWidth > 0)
	{horizontalWidth = defaultHorizontalWidth;}
      else
	{horizontalWidth = BDSGlobalConstants::Instance()->HorizontalWidth();}
    }
  return horizontalWidth;
}

G4Material* BDSComponentFactory::PrepareVacuumMaterial(Element const* el) const
{
  G4Material* result = nullptr;
  if (el->vacuumMaterial == "")
    {result = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->VacuumMaterial());}
  else
    {result = BDSMaterials::Instance()->GetMaterial(element->vacuumMaterial);}
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4ThreeVector inputFaceNormalIn,
							  const G4ThreeVector outputFaceNormalIn)
{
  BDSBeamPipeInfo* defaultModel = BDSGlobalConstants::Instance()->DefaultBeamPipeModel();
  BDSBeamPipeInfo* result; 
  if (!BDSGlobalConstants::Instance()->IgnoreLocalAperture())
    {
      result = new BDSBeamPipeInfo(defaultModel,
				   el->apertureType,
				   el->aper1 * CLHEP::m,
				   el->aper2 * CLHEP::m,
				   el->aper3 * CLHEP::m,
				   el->aper4 * CLHEP::m,
				   el->vacuumMaterial,
				   el->beampipeThickness * CLHEP::m,
				   el->beampipeMaterial,
				   inputFaceNormalIn,
				   outputFaceNormalIn);
    }
  else
    {// ignore the aperture model from the element and use the global one
      result = new BDSBeamPipeInfo(*defaultModel); // ok as only pointers to materials
      result->inputFaceNormal  = inputFaceNormalIn;
      result->outputFaceNormal = outputFaceNormalIn;
    }
  return result;
}

BDSBeamPipeInfo* BDSComponentFactory::PrepareBeamPipeInfo(Element const* el,
							  const G4double angleIn,
							  const G4double angleOut)
{
  auto faces = BDS::CalculateFaces(angleIn, angleOut);
  BDSBeamPipeInfo* info = PrepareBeamPipeInfo(el, faces.first, faces.second);
  return info;
}

BDSTiltOffset* BDSComponentFactory::CreateTiltOffset(Element const* el)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "offsetX,Y: " << el->offsetX << " " << el->offsetY << " tilt: " << el->tilt << G4endl;
#endif
  G4double xOffset = el->offsetX * CLHEP::m;
  G4double yOffset = el->offsetY * CLHEP::m;
  G4double tilt    = el->tilt    * CLHEP::rad;

  BDSTiltOffset* result = nullptr;
  if (BDS::IsFinite(xOffset) || BDS::IsFinite(yOffset) || BDS::IsFinite(tilt))
    {result = new BDSTiltOffset(xOffset, yOffset, tilt);}
  return result;
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(const BDSTiltOffset* tiltOffset)
{
  return tiltOffset ? tiltOffset->Transform3D() : G4Transform3D();
}

G4Transform3D BDSComponentFactory::CreateFieldTransform(Element const* el)
{
  BDSTiltOffset* to = CreateTiltOffset(el);
  G4Transform3D trans = CreateFieldTransform(to);
  delete to;
  return trans;
}

void BDSComponentFactory::CheckBendLengthAngleWidthCombo(G4double arcLength,
							 G4double angle,
							 G4double horizontalWidth,
							 G4String name)
{
  G4double radiusFromAngleLength =  std::abs(arcLength / angle); // s = r*theta -> r = s/theta
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "radius from angle and length: " << radiusFromAngleLength << G4endl;
#endif
  if (horizontalWidth > 2*radiusFromAngleLength)
    {
      G4cerr << "Error: the combination of length, angle and horizontalWidth in element named \""
	     << name
	     << "\" will result in overlapping faces!" << G4endl << "Please correct!" << G4endl;
      exit(1);
    }
}

void BDSComponentFactory::PrepareCavityModels()
{  
  for (auto model : BDSParser::Instance()->GetCavityModels())
    {
      // material can either be specified in 
      G4Material* material = nullptr;
      if (!model.material.empty())
	{material = BDSMaterials::Instance()->GetMaterial(model.material);}
	  
      auto info = new BDSCavityInfo(BDS::DetermineCavityType(model.type),
				    material,
				    model.irisRadius*CLHEP::m,
				    model.thickness*CLHEP::m,
				    model.equatorRadius*CLHEP::m,
				    model.halfCellLength*CLHEP::m,
				    model.numberOfPoints,
				    model.numberOfCells,
				    model.equatorHorizontalAxis*CLHEP::m,
				    model.equatorVerticalAxis*CLHEP::m,
				    model.irisHorizontalAxis*CLHEP::m,
				    model.irisVerticalAxis*CLHEP::m,
				    model.tangentLineAngle);
      
      cavityInfos[model.name] = info;
    }
}

void BDSComponentFactory::PrepareLasers()
{
  for (const auto& laser : BDSParser::Instance()->GetLasers())
    {
      G4double sigma0 = 0;
      if (BDS::IsFinite(laser.w0))
	{sigma0 = 0.5 * laser.w0;}
      else if (BDS::IsFinite(laser.sigma0))
	{sigma0 = laser.sigma0;}
      else
	{
	  G4cerr << __METHOD_NAME__ << "Neither \"w0\" or \"sigma0\" are defined "
		 << "for laser definition \"" << laser.name << "\"" << G4endl;
	  exit(1);
	}
      sigma0 *= CLHEP::m;
      
      BDSLaser* las = new BDSLaser(laser.wavelength*CLHEP::m,
                                   laser.m2,
                                   laser.pulseDuration*CLHEP::s,
                                   laser.pulseEnergy*CLHEP::joule,
                                   sigma0);
      lasers[laser.name] = las;
    }
}

BDSLaser* BDSComponentFactory::PrepareLaser(GMAD::Element const* el) const
{

  G4String laserBeam = G4String(el->laserBeam);
  auto result = lasers.find(laserBeam);
  if (result == lasers.end())
    {
      G4cout << "Unknown laser \"" << laserBeam << "\" - please define it" << G4endl;
      exit(1);
    }

  // prepare a copy so the component can own that recipe
  BDSLaser* laser = new BDSLaser(*(result->second));
  return laser;
}

void BDSComponentFactory::PrepareColours()
{
  BDSColours* allColours = BDSColours::Instance();
  for (auto colour : BDSParser::Instance()->GetColours())
    {
      allColours->DefineColour(G4String(colour.name),
			       (G4double)colour.red,
			       (G4double)colour.green,
			       (G4double)colour.blue,
			       (G4double)colour.alpha);
    }
}

void BDSComponentFactory::PrepareCrystals()
{
  for (auto& model : BDSParser::Instance()->GetCrystals())
    {
      G4Material* material = BDSMaterials::Instance()->GetMaterial(model.material);

      auto info = new BDSCrystalInfo(material,
				     G4String(model.data),
				     BDS::DetermineCrystalType(model.shape),
				     G4double(model.lengthX)*CLHEP::m,
				     G4double(model.lengthY)*CLHEP::m,
				     G4double(model.lengthZ)*CLHEP::m,
				     G4double(model.sizeA)*CLHEP::m,
				     G4double(model.sizeB)*CLHEP::m,
				     G4double(model.sizeC)*CLHEP::m,
				     G4double(model.alpha)*CLHEP::halfpi,
				     G4double(model.beta)*CLHEP::halfpi,
				     G4double(model.gamma)*CLHEP::halfpi,
				     G4int   (model.spaceGroup),
				     G4double(model.bendingAngleYAxis)*CLHEP::rad,
				     G4double(model.bendingAngleZAxis)*CLHEP::rad);
      crystalInfos[model.name] = info;
    }

}

BDSCrystalInfo* BDSComponentFactory::PrepareCrystalInfo(const G4String& crystalName) const
{
  auto result = crystalInfos.find(crystalName);
  if (result == crystalInfos.end())
    {
      G4cout << "Unknown crystal \"" << crystalName << "\" - please define it" << G4endl;
      exit(1);
    }

  // prepare a copy so the component can own that recipe
  BDSCrystalInfo* info = new BDSCrystalInfo(*(result->second));
  return info;
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfo(Element const* el,
							   G4double frequency) const
{
  // If the cavity model name (identifier) has been defined, return a *copy* of
  // that model - so that the component will own that info object.

  G4String modelName = G4String(el->cavityModel);

  // no specific model - prepare a default based on element parameters
  if (modelName == "")
    {return PrepareCavityModelInfoForElement(el, frequency);}

  // cavity model name specified - match up with parser object already translated here
  auto result = cavityInfos.find(modelName);
  if (result == cavityInfos.end())
    {
      G4cout << "Unknown cavity model identifier \"" << el->cavityModel << "\" - please define it" << G4endl;
      exit(1);
    }

  // we make a per element copy of the definition
  BDSCavityInfo* info = new BDSCavityInfo(*(result->second));

  // If no material specified, we take the material from the element. If no material at
  // all, we exit with warning.
  if (!info->material)
    {
      if (el->material.empty())
	{
	  G4cout << "ERROR: Cavity material is not defined for cavity \"" << elementName << "\""
		 << "or for cavity model \"" << el->cavityModel << "\" - please define it" << G4endl;
	  exit(1);
	}
      else
	{info->material = BDSMaterials::Instance()->GetMaterial(el->material);}
    }

  return info;
}

BDSCavityInfo* BDSComponentFactory::PrepareCavityModelInfoForElement(Element const* el,
								     G4double frequency) const
{
  /// prepare aperture information for this element to base default cavity on.
  BDSBeamPipeInfo* aperture = PrepareBeamPipeInfo(el);

  G4double aper1     = aperture->aper1;
  G4double horizontalWidth = PrepareHorizontalWidth(el);
  G4double defaultHorizontalWidth = 20*CLHEP::cm;
  if (aper1 < defaultHorizontalWidth) // only do if the aperture will fit
    {horizontalWidth = std::min(defaultHorizontalWidth, horizontalWidth);} // better default
  G4double thickness = aperture->beamPipeThickness;
  G4double equatorRadius = horizontalWidth - thickness;
  if (equatorRadius <= 0)
    {
      G4cerr << __METHOD_NAME__ << "Combination of horizontalWidth and beampipeThickness for"
	     << " element \"" << el->name << "\" produce 0 size cavity" << G4endl;
      exit(1);
    }

  G4double cellLength = 2*CLHEP::c_light / frequency; // half wavelength
  G4double length     = el->l * CLHEP::m;
  G4double nCavities  = length / cellLength;
  G4int nCells = G4int(std::floor(nCavities));
  if (nCells == 0) // protect against long wavelengths or cavities
    {
      nCells = 1;
      cellLength = length;
    }
  
  BDSCavityInfo* defaultCI = new BDSCavityInfo(BDSCavityType::pillbox,
					       BDSMaterials::Instance()->GetMaterial("Copper"),
					       aperture->aper1,
					       thickness,
					       equatorRadius,
					       cellLength*0.5);

  delete aperture;
  return defaultCI;
}

BDSMagnetStrength* BDSComponentFactory::PrepareCavityStrength(Element const* el,
							      G4double currentArcLength) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double chordLength = el->l * CLHEP::m;
  G4double scaling     = el->scaling;
  
  if (BDS::IsFinite(el->gradient))
    {(*st)["efield"] = scaling * el->gradient * CLHEP::MeV / CLHEP::m;}
  else
    {(*st)["efield"] = scaling * el->E * CLHEP::volt / chordLength;}

  (*st)["frequency"] = el->frequency * CLHEP::hertz;

  // phase - construct it so that phase is w.r.t. the centre of the cavity
  // and that it's 0 by default
  G4double frequency = (*st)["frequency"];
  G4double period    = 1. / frequency;
  G4double tOffset   = 0;
  if (BDS::IsFinite(el->tOffset)) // use the one specified
    {tOffset = el->tOffset * CLHEP::s;}
  else // this gives 0 phase at the middle of cavity
    {tOffset = (currentArcLength + 0.5*chordLength) / CLHEP::c_light;}

  G4double nPeriods       = tOffset / period;
  // phase is the remainder from total phase / N*2pi, where n is unknown.
  G4double integerPart    = 0;
  G4double fractionalPart = std::modf(nPeriods, &integerPart);
  G4double phaseOffset    = fractionalPart * CLHEP::twopi;

  G4double phase = el->phase * CLHEP::rad;
  if (BDS::IsFinite(el->phase)) // phase specified - use that
    {(*st)["phase"] = phaseOffset + phase;}
  else
    {(*st)["phase"] = phaseOffset;}
  (*st)["equatorradius"] = 1*CLHEP::m; // to prevent 0 division - updated later on in createRF
  (*st)["length"] = chordLength;
  return st;
}

G4Colour* BDSComponentFactory::PrepareColour(Element const* el)
{
  G4String colour = el->colour;
  if (colour.empty())
    {return BDSColours::Instance()->GetColour(GMAD::typestr(el->type));}
  else
    {return BDSColours::Instance()->GetColour(colour);}
}

G4Material* BDSComponentFactory::PrepareMaterial(Element const* el,
						 G4String defaultMaterialName)
{
  G4String materialName = el->material;
  if (materialName.empty())
    {return BDSMaterials::Instance()->GetMaterial(defaultMaterialName);}
  else
    {return BDSMaterials::Instance()->GetMaterial(materialName);}
}

G4Material* BDSComponentFactory::PrepareMaterial(Element const* el)
{
  G4String materialName = el->material;
  if (materialName.empty())
    {G4cout << __METHOD_NAME__ << "element \"" << el->name << "\" has no material specified." << G4endl; exit(1);}
  else
    {return BDSMaterials::Instance()->GetMaterial(materialName);}
}

void BDSComponentFactory::SetFieldDefinitions(Element const* el,
					      BDSAcceleratorComponent* component) const
{
  // Test for a line. And if so apply to each sub-component.
  // TBC - for a sbend split into segments, a BDSLine would be used - how would setting
  // an outer magnetic field work for this??
  G4Transform3D fieldTrans = CreateFieldTransform(element);
  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
      for (auto comp : *line)
	{SetFieldDefinitions(el, comp);}
    }
  if (BDSMagnet* mag = dynamic_cast<BDSMagnet*>(component))
    {
      if (!(el->fieldAll.empty()))
	{
	  G4cerr << "Error: Magnet named \"" << elementName
		 << "\" is a magnet, but has fieldAll defined." << G4endl
		 << "Can only have fieldOuter and or fieldVacuum specified." << G4endl;
	  exit(1);
	}
      if (!(el->fieldOuter.empty())) // ie variable isn't ""
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldOuter)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  mag->SetOuterField(info);
	}
      if (!(el->fieldVacuum.empty()))
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldVacuum)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  mag->SetVacuumField(info);
	}
    }
  else
    {
      if (!(el->fieldAll.empty()))
	{
	  BDSFieldInfo* info = new BDSFieldInfo(*(BDSFieldFactory::Instance()->GetDefinition(el->fieldAll)));
	  if (info->ProvideGlobal())
	    {info->SetTransform(info->Transform() * fieldTrans);}
	  component->SetField(info);
	}
    }
}

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForMultipoles(Element const* el) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  SetBeta0(st);
  G4double scaling = el->scaling;
  G4double length = el->l;
  // component strength is only normalised by length for thick multipoles
  if (el->type == ElementType::_THINMULT)
    {length = 1;}
  auto kn = el->knl.begin();
  auto ks = el->ksl.begin();
  std::vector<G4String> normKeys = st->NormalComponentKeys();
  std::vector<G4String> skewKeys = st->SkewComponentKeys();
  auto nkey = normKeys.begin();
  auto skey = skewKeys.begin();
  //Separate loops for kn and ks. The length of knl and ksl is determined by the input in the gmad file.
  //A single loop for both kn and ks using only one of their end iterators can end the loop
  //prematurely for the other, potentially missing higher order components.
  for (; kn != el->knl.end(); kn++, nkey++)
    {(*st)[*nkey] = scaling * (*kn) / length;}
  for (; ks != el->ksl.end(); ks++, skey++)
    {(*st)[*skey] = scaling * (*ks) / length;}

  return st;
}

BDSMagnetStrength* BDSComponentFactory::PrepareMagnetStrengthForRMatrix(Element const* el) const
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  G4double scaling = el->scaling;
  // G4double length  = el->l;

  (*st)["kick1"] = scaling * el->kick1;
  (*st)["kick2"] = scaling * el->kick2;
  (*st)["kick3"] = scaling * el->kick3;
  (*st)["kick4"] = scaling * el->kick4;

  (*st)["rmat11"] = scaling * el->rmat11;
  (*st)["rmat12"] = scaling * el->rmat12;
  (*st)["rmat13"] = scaling * el->rmat13;
  (*st)["rmat14"] = scaling * el->rmat14;

  (*st)["rmat21"] = scaling * el->rmat21;
  (*st)["rmat22"] = scaling * el->rmat22;
  (*st)["rmat23"] = scaling * el->rmat23;
  (*st)["rmat24"] = scaling * el->rmat24;

  (*st)["rmat31"] = scaling * el->rmat31;
  (*st)["rmat32"] = scaling * el->rmat32;
  (*st)["rmat33"] = scaling * el->rmat33;
  (*st)["rmat34"] = scaling * el->rmat34;

  (*st)["rmat41"] = scaling * el->rmat41;
  (*st)["rmat42"] = scaling * el->rmat42;
  (*st)["rmat43"] = scaling * el->rmat43;
  (*st)["rmat44"] = scaling * el->rmat44;

  return st;
}

G4double BDSComponentFactory::FieldFromAngle(const G4double angle,
					     const G4double arcLength) const
{
  if (!BDS::IsFinite(angle))
    {return 0;}
  else
    {return brho * angle / arcLength;}
}

G4double BDSComponentFactory::AngleFromField(const G4double field,
					     const G4double arcLength) const
{
  if (!BDS::IsFinite(field))
    {return 0;}
  else
    {return field * arcLength / brho;}
}

void BDSComponentFactory::CalculateAngleAndFieldSBend(Element const* el,
						      G4double&      angle,
						      G4double&      field) const
{
  G4double arcLength = el->l * CLHEP::m;
  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      field = el->B * CLHEP::tesla;
      angle = el->angle * CLHEP::rad;
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      angle = AngleFromField(field, arcLength); // length is arc length for an sbend
    }
  else
    {// only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      field = FieldFromAngle(angle, arcLength);
    }
  // un-split sbends are effectively rbends - don't construct a >pi/2 degree rbend
  // also cannot construct sbends with angles > pi/2
  if (BDSGlobalConstants::Instance()->DontSplitSBends() && (std::abs(angle) > CLHEP::pi/2.0))
    {throw BDSException("Error: the unsplit sbend "+ el->name + " cannot be constucted as its bending angle is defined to be greater than pi/2.");}

  else if (std::abs(angle) > CLHEP::pi*2.0)
    {throw BDSException("Error: the sbend "+ el->name +" cannot be constucted as its bending angle is defined to be greater than 2 pi.");}
}

void BDSComponentFactory::CalculateAngleAndFieldRBend(const Element* el,
						      G4double& arcLength,
						      G4double& chordLength,
						      G4double& field,
						      G4double& angle) const
{
  // 'l' in the element represents the chord length for an rbend - must calculate arc length
  // for the field calculation and the accelerator component.
  chordLength = el->l * CLHEP::m;
  G4double arcLengthLocal = chordLength; // default for no angle
  
  if (BDS::IsFinite(el->B) && el->angleSet)
    {// both are specified and should be used - under or overpowered dipole by design
      field = el->B * CLHEP::tesla;
      // note, angle must be finite for this part to be used so we're protected against
      // infinite bending radius and therefore nan arcLength.
      angle = el->angle * CLHEP::rad;
      G4double bendingRadius = brho / field;

      // protect against bad calculation from 0 angle and finite field
      if (BDS::IsFinite(angle))
        {arcLengthLocal = bendingRadius * angle;}
      else
        {arcLengthLocal = chordLength;}
    }
  else if (BDS::IsFinite(el->B))
    {// only B field - calculate angle
      field = el->B * CLHEP::tesla;
      G4double bendingRadius = brho / field; // in mm as brho already in g4 units
      angle = 2.0*std::asin(chordLength*0.5 / bendingRadius);
      if (std::isnan(angle))
        {throw BDSException("Field too strong for element " + el->name + ", magnet bending angle will be greater than pi.");}

      arcLengthLocal = bendingRadius * angle;
    }
  else
    {// (assume) only angle - calculate B field
      angle = el->angle * CLHEP::rad;
      if (BDS::IsFinite(angle))
	{
	  // sign for bending radius doesn't matter (from angle) as it's only used for arc length.
	  // this is the inverse equation of that in BDSAcceleratorComponent to calculate
	  // the chord length from the arclength and angle.
	  G4double bendingRadius = chordLength * 0.5 / std::sin(std::abs(angle) * 0.5);
	  arcLengthLocal = bendingRadius * angle;
	  field = brho * angle / std::abs(arcLengthLocal);
        }
      else
	{field = 0;} // 0 angle -> chord length and arc length the same; field 0
    }

  // Ensure positive length despite sign of angle.
  arcLength = std::abs(arcLengthLocal);

  if (std::abs(angle) > CLHEP::pi/2.0)
    {throw BDSException("Error: the rbend " + el->name + " cannot be constucted as its bending angle is defined to be greater than pi/2.");}
}

G4double BDSComponentFactory::BendAngle(const Element* el) const
{
  G4double bendAngle = 0;
  if (el->type == ElementType::_RBEND)
    {
      G4double arcLength = 0, chordLength = 0, field = 0;
      CalculateAngleAndFieldRBend(el, arcLength, chordLength, field, bendAngle);
    }
  else if (el->type == ElementType::_SBEND)
    {
      G4double field = 0; // required by next function.
      CalculateAngleAndFieldSBend(el, bendAngle, field);
    }
  // else the default is 0
  return bendAngle;
}

G4double BDSComponentFactory::OutgoingFaceAngle(const Element* el) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a previous element
  // - it'll be skipped while looking backwards.
  G4double outgoingFaceAngle = 0;
  G4double bendAngle         = BendAngle(el);

  if (el->type == ElementType::_RBEND)
    {
      if (integratorSet->IsMatrixIntegratorSet())
	{return outgoingFaceAngle;}
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      outgoingFaceAngle += 0.5 * bendAngle;
    }

  // if we're using the matrix integrator set, we build the bends flat irrespective of parameters
  // if we have a finite k1 for a bend, we're forced to use the bdsimmatrix integrator set
  if (integratorSet->IsMatrixIntegratorSet() || BDS::IsFinite(el->k1))
    {return outgoingFaceAngle;}

  // for an sbend, the output face is nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes
  
  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e2 = el->e2*CLHEP::rad;
  if (BDS::IsFinite(e2))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      outgoingFaceAngle += factor * e2;
    }
  
  return outgoingFaceAngle;
}

G4double BDSComponentFactory::IncomingFaceAngle(const Element* el) const
{
  // note thin multipoles will match the faces of any magnets, but not contain
  // the face angles themselves in the GMAD::Element. this is ok though as
  // detector construction will not give a thin multipole as a next element
  // - it'll be skipped while looking forwards.
  G4double incomingFaceAngle = 0;
  G4double bendAngle         = BendAngle(el);

  if (el->type == ElementType::_RBEND)
    {
      if (integratorSet->IsMatrixIntegratorSet())
	{return incomingFaceAngle;}
      // angle is w.r.t. outgoing reference trajectory so rbend face is angled
      // by half the bend angle
      incomingFaceAngle += 0.5 * bendAngle;
    }

  // if we're using the matrix integrator set, we build the bends flat irrespective of parameters
  // if we have a finite k1 for a bend, we're forced to use the bdsimmatrix integrator set
  if (integratorSet->IsMatrixIntegratorSet() || BDS::IsFinite(el->k1))
    {return incomingFaceAngle;}

  // for an sbend, the output face or nominally normal to the outgoing
  // reference trajectory - so zero here - only changes with e1/e2.
  // we need angle though to decide which way it goes

  // +ve e1/e2 shorten the outside of the bend - so flips with angle
  G4double e1 = el->e1*CLHEP::rad;
  if (BDS::IsFinite(e1))
    {// so if the angle is 0, +1 will be returned
      G4double factor = bendAngle < 0 ? -1 : 1;
      incomingFaceAngle += factor * e1;
    }
  
  return incomingFaceAngle;
}
