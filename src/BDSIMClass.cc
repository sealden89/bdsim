/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSIMClass.hh"

#include "BDSExecOptions.hh"     // executable command line options 
#include "BDSGlobalConstants.hh" //  global parameters

#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GenericBiasingPhysics.hh"
#include "G4GeometryManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4PhysicsListHelper.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4ParticleDefinition.hh"
#include "G4SteppingManager.hh"
#include "G4TrackingManager.hh"
#include "G4Version.hh"
#include "G4VModularPhysicsList.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSCavityFactory.hh"
#include "BDSColours.hh"
#include "BDSComponentFactoryUser.hh"
#include "BDSDebug.hh"
#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSException.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldLoader.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryWriter.hh"
#include "BDSIonDefinition.hh"
#include "BDSMaterials.hh"
#include "BDSOutput.hh" 
#include "BDSOutputFactory.hh"
#include "BDSParallelWorldUtilities.hh"
#include "BDSParser.hh" // Parser
#include "BDSParticleDefinition.hh"
#include "BDSPhysicsUtilities.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSTrackingAction.hh"
#include "BDSUtilities.hh"
#include "BDSVisManager.hh"
#include "BDSWarning.hh"

BDSIM::BDSIM():
  ignoreSIGINT(false),
  usualPrintOut(true),
  initialised(false),
  initialisationResult(1),
  argcCache(0),
  argvCache(nullptr),
  parser(nullptr),
  bdsOutput(nullptr),
  bdsBunch(nullptr),
  runManager(nullptr),
  userComponentFactory(nullptr)
{;}

BDSIM::BDSIM(int argc, char** argv, bool usualPrintOutIn):
  ignoreSIGINT(false),
  usualPrintOut(usualPrintOutIn),
  initialised(false),
  initialisationResult(1),
  argcCache(argc),
  argvCache(argv),
  parser(nullptr),
  bdsOutput(nullptr),
  bdsBunch(nullptr),
  runManager(nullptr),
  userComponentFactory(nullptr)
{
  initialisationResult = Initialise();
}

int BDSIM::Initialise(int argc, char** argv, bool usualPrintOutIn)
{
  argcCache = argc;
  argvCache = argv;
  usualPrintOut = usualPrintOutIn;
  initialisationResult = Initialise();
  return initialisationResult;
}

int BDSIM::Initialise()
{
  /// Print header & program information
  G4cout<<"BDSIM : version @BDSIM_VERSION@"<<G4endl;
  G4cout<<"        (C) 2001-@CURRENT_YEAR@ Royal Holloway University London"  << G4endl;
  G4cout<<G4endl;
  G4cout<<"        Reference: Computer Physics Communications, 107200 (2020)" << G4endl;
  G4cout<<"                   https://doi.org/10.1016/j.cpc.2020.107200"      << G4endl;
  G4cout<<"                   https://arxiv.org/abs/1808.10745"               << G4endl;
  G4cout<<"        Website:   http://www.pp.rhul.ac.uk/bdsim"<<G4endl;
  G4cout<<G4endl;

  /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = new BDSExecOptions(argcCache,argvCache);
  if (usualPrintOut)
    {execOptions->Print();}
  ignoreSIGINT = execOptions->IgnoreSIGINT(); // different sig catching for cmake
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "DEBUG mode is on." << G4endl;
#endif

  /// Parse lattice file
  parser = BDSParser::Instance(execOptions->InputFileName());
  /// Update options generated by parser with those from executable options.
  parser->AmalgamateOptions(execOptions->Options());
  parser->AmalgamateBeam(execOptions->Beam(), execOptions->Options().recreate);
  /// Check options for consistency
  parser->CheckOptions();
  
  /// Explicitly initialise materials to construct required materials before global constants.
  BDSMaterials::Instance()->PrepareRequiredMaterials(execOptions->Options().verbose);

  /// No longer needed. Everything can safely use BDSGlobalConstants from now on.
  delete execOptions; 

  /// Force construction of global constants after parser has been initialised (requires
  /// materials first). This uses the options and beam from BDSParser.
  /// Non-const as we'll update the particle definition.
  BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  /// Initialize random number generator
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options

  /// Construct output
  bdsOutput = BDSOutputFactory::CreateOutput(globalConstants->OutputFormat(),
					     globalConstants->OutputFileName());

  /// Check geant4 exists in the current environment
  if (!BDS::Geant4EnvironmentIsSet())
    {
      G4cerr << "No Geant4 environmental variables found - please source geant4.sh environment" << G4endl;
      G4cout << "A common fault is the wrong Geant4 environment as compared to the one BDSIM was compiled with." << G4endl;
      return 1;
    }

  /// Construct mandatory run manager (the G4 kernel) and
  /// register mandatory initialization classes.
  runManager = new BDSRunManager;

  /// Register the geometry and parallel world construction methods with run manager.
  BDSDetectorConstruction* realWorld = new BDSDetectorConstruction(userComponentFactory);
  
  /// Here the geometry isn't actually constructed - this is called by the runManager->Initialize()
  auto parallelWorldsRequiringPhysics = BDS::ConstructAndRegisterParallelWorlds(realWorld, realWorld->BuildSamplerWorld());
  runManager->SetUserInitialization(realWorld);  

  /// For geometry sampling, phys list must be initialized before detector.
  /// BUT for samplers we use a parallel world and this HAS to be before the physics
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "> Constructing physics processes" << G4endl;
#endif
  G4String physicsListName = parser->GetOptions().physicsList;

#if G4VERSION_NUMBER > 1049
  // from 10.5 onwards they have a looping particle killer that warnings and kills particles
  // deemed to be looping that are <100 MeV. This is unrelated to the primary energy so troublesome.
  // set to the 'low' limits here ~10keV. This must be done before any physics is created as the
  // parameters are copied into the transportation physics process for each particle and it's very
  // hard to sift through and fix afterwards
  G4PhysicsListHelper::GetPhysicsListHelper()->UseLowLooperThresholds();
#endif
  // sampler physics process for parallel world tracking must be instantiated BEFORE
  // regular physics.
  // Note, we purposively don't create a parallel world process for the curvilinear
  // world as we don't need the track information from it - unreliable that way. We
  // query the geometry directly using our BDSAuxiliaryNavigator class.
  auto parallelWorldPhysics = BDS::ConstructParallelWorldPhysics(parallelWorldsRequiringPhysics);
  G4int physicsVerbosity = BDSGlobalConstants::Instance()->PhysicsVerbosity();
  G4VModularPhysicsList* physList = BDS::BuildPhysics(physicsListName, physicsVerbosity);

  // create geometry sampler and register importance sampling biasing. Has to be here
  // before physicsList is "initialised" in run manager.
  if (BDSGlobalConstants::Instance()->UseImportanceSampling())
    {BDS::RegisterImportanceBiasing(parallelWorldsRequiringPhysics,physList);}

  // Construction of the physics lists defines the necessary particles and therefore
  // we can calculate the beam rigidity for the particle the beam is designed w.r.t. This
  // must happen before the geometry is constructed (which is called by
  // runManager->Initialize()).
  BDSParticleDefinition* designParticle = nullptr;
  BDSParticleDefinition* beamParticle = nullptr;
  G4bool beamDifferentFromDesignParticle = false;
  BDS::ConstructDesignAndBeamParticle(BDSParser::Instance()->GetBeam(),
				      globalConstants->FFact(),
				      designParticle,
				      beamParticle,
				      beamDifferentFromDesignParticle);
  G4double minEK = BDSGlobalConstants::Instance()->MinimumKineticEnergy();
  if (beamParticle->KineticEnergy() < minEK && BDS::IsFinite(minEK))
    {throw BDSException("option, minimumKineticEnergy is higher than kinetic energy of the beam - all primary particles wil be killed!");}
  if (usualPrintOut)
    {
      G4cout << "Design particle properties: " << G4endl << *designParticle;
      if (beamDifferentFromDesignParticle)
	{G4cout << "Beam particle properties: " << G4endl << *beamParticle;}
    }
  // update rigidity where needed
  realWorld->SetDesignParticle(designParticle);
  BDSFieldFactory::SetDesignParticle(designParticle);
  BDSGeometryFactorySQL::SetDefaultRigidity(designParticle->BRho()); // used for sql field loading

  BDS::RegisterSamplerPhysics(parallelWorldPhysics, physList);
  auto biasPhysics = BDS::BuildAndAttachBiasWrapper(parser->GetBiasing());
  if (biasPhysics)//could be nullptr and can't be passed to geant4 like this
    {physList->RegisterPhysics(biasPhysics);}
  runManager->SetUserInitialization(physList);

  /// Instantiate the specific type of bunch distribution.
  bdsBunch = BDSBunchFactory::CreateBunch(beamParticle,
					  parser->GetBeam(),
					  globalConstants->BeamlineTransform(),
					  globalConstants->BeamlineS(),
					  globalConstants->GeneratePrimariesOnly());
  /// We no longer need beamParticle so delete it to avoid confusion. The definition is
  /// held inside bdsBunch (can be updated dynamically).
  delete beamParticle;
  
  /// Optionally generate primaries only and exit
  /// Unfortunately, this has to be here as we can't query the geant4 particle table
  /// until after the physics list has been constructed and attached a run manager.
  if (globalConstants->GeneratePrimariesOnly())
    {      
      // output creation is duplicated below but with this if loop, we exit so ok.
      bdsOutput->NewFile();
      const G4int nToGenerate = globalConstants->NGenerate();
      const G4int printModulo = globalConstants->PrintModuloEvents();
      bdsBunch->BeginOfRunAction(nToGenerate);
      auto flagsCache(G4cout.flags());
      for (G4int i = 0; i < nToGenerate; i++)
	{
	  if (i%printModulo == 0)
	    {G4cout << "\r Primary> " << std::fixed << i << " of " << nToGenerate << G4endl;}
	  BDSParticleCoordsFullGlobal coords = bdsBunch->GetNextParticleValid();
	  // always pull particle definition in case it's updated
	  const BDSParticleDefinition* pDef = bdsBunch->ParticleDefinition();
	  bdsOutput->FillEventPrimaryOnly(coords, pDef);
	}
      G4cout.flags(flagsCache); // restore cout flags
      // Write options now the file is open
      const GMAD::OptionsBase* ob = BDSParser::Instance()->GetOptionsBase();
      bdsOutput->FillOptions(ob);
      
      // Write beam
      const GMAD::BeamBase* bb = BDSParser::Instance()->GetBeamBase();
      bdsOutput->FillBeam(bb);

      bdsOutput->CloseFile();
      return 2;
    }
  
  /// Print the geometry tolerance
  G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  if (usualPrintOut)
    {
      G4cout << __METHOD_NAME__ << "Geometry Tolerances: "     << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Surface: " << std::setw(7) << theGeometryTolerance->GetSurfaceTolerance() << " mm"   << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Angular: " << std::setw(7) << theGeometryTolerance->GetAngularTolerance() << " rad"  << G4endl;
      G4cout << __METHOD_NAME__ << std::setw(12) << "Radial: "  << std::setw(7) << theGeometryTolerance->GetRadialTolerance()  << " mm"   << G4endl;
    }
  /// Set user action classes
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Event Action" << G4endl;
#endif
  BDSEventAction* eventAction = new BDSEventAction(bdsOutput);
  runManager->SetUserAction(eventAction);

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Run Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction(bdsOutput,
					     bdsBunch,
					     bdsBunch->ParticleDefinition()->IsAnIon(),
					     eventAction,
					     globalConstants->StoreTrajectorySamplerID()));

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Stepping Action"<<G4endl;
#endif
  // Only add steppingaction if it is actually used, so do check here (for performance reasons)
  G4int verboseSteppingEventStart = globalConstants->VerboseSteppingEventStart();
  G4int verboseSteppingEventStop  = BDS::VerboseEventStop(verboseSteppingEventStart,
							  globalConstants->VerboseSteppingEventContinueFor());
  if (globalConstants->VerboseSteppingBDSIM())
    {
      runManager->SetUserAction(new BDSSteppingAction(true,
						      verboseSteppingEventStart,
						      verboseSteppingEventStop));
    }
  
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Tracking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSTrackingAction(globalConstants->Batch(),
						  globalConstants->StoreTrajectory(),
						  globalConstants->StoreTrajectoryLocal(),
						  globalConstants->StoreTrajectoryLinks(),
						  globalConstants->StoreTrajectoryIon(),
						  !globalConstants->StoreTrajectoryTransportationSteps(),
						  eventAction,
						  verboseSteppingEventStart,
						  verboseSteppingEventStop,
						  globalConstants->VerboseSteppingPrimaryOnly(),
						  globalConstants->VerboseSteppingLevel()));

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Stacking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction(globalConstants));

#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << "Registering user action - Primary Generator"<<G4endl;
#endif
  auto primaryGeneratorAction = new BDSPrimaryGeneratorAction(bdsBunch, parser->GetBeam());
  runManager->SetUserAction(primaryGeneratorAction);
  BDSFieldFactory::SetPrimaryGeneratorAction(primaryGeneratorAction);

  /// Initialize G4 kernel
  runManager->Initialize();

  /// Create importance store for parallel importance world
  if (BDSGlobalConstants::Instance()->UseImportanceSampling())
    {BDS::AddIStore(parallelWorldsRequiringPhysics);}

  /// Implement bias operations on all volumes only after G4RunManager::Initialize()
  realWorld->BuildPhysicsBias();

  if (usualPrintOut && BDSGlobalConstants::Instance()->PhysicsVerbose())
    {
      BDS::PrintPrimaryParticleProcesses(bdsBunch->ParticleDefinition()->Name());
      BDS::PrintDefinedParticles();
    }

  /// Set verbosity levels at run and G4 event level. Per event and stepping are controlled
  /// in event, tracking and stepping action. These have to be done here due to the order
  /// of construction in Geant4.
  runManager->SetVerboseLevel(std::min(globalConstants->VerboseRunLevel(), globalConstants->PhysicsVerbosity()));
  G4EventManager::GetEventManager()->SetVerboseLevel(globalConstants->VerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(globalConstants->VerboseTrackingLevel());
  
  /// Close the geometry in preparation for running - everything is now fixed.
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if (!bCloseGeometry)
    { 
      G4cerr << __METHOD_NAME__ << "error - geometry not closed." << G4endl;
      return 1;
    }

  if (globalConstants->ExportGeometry())
    {
      BDSGeometryWriter geometrywriter;
      geometrywriter.ExportGeometry(globalConstants->ExportType(),
				    globalConstants->ExportFileName());
    }

  initialised = true;
  return 0;
}

void BDSIM::BeamOn(int nGenerate)
{
  if (initialisationResult > 1 || !initialised)
    {return;} // a mode where we don't do anything

  G4cout.precision(10);
  /// Catch aborts to close output stream/file. perhaps not all are needed.
  struct sigaction act;
  act.sa_handler = &BDS::HandleAborts;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (!ignoreSIGINT)
    {sigaction(SIGINT,  &act, 0);}
  sigaction(SIGABRT, &act, 0);
  sigaction(SIGTERM, &act, 0);
  sigaction(SIGSEGV, &act, 0);
  
  /// Run in either interactive or batch mode
  try
    {
      if (!BDSGlobalConstants::Instance()->Batch())   // Interactive mode
	{
	  BDSVisManager visManager = BDSVisManager(BDSGlobalConstants::Instance()->VisMacroFileName(),
						   BDSGlobalConstants::Instance()->Geant4MacroFileName());
	  visManager.StartSession(argcCache, argvCache);
	}
      else
	{// batch mode
	  if (nGenerate < 0)
	    {runManager->BeamOn(BDSGlobalConstants::Instance()->NGenerate());}
	  else
	    {runManager->BeamOn(nGenerate);}
	}
    }
  catch (const BDSException& exception)
    {
      // don't do this for now in case it's dangerous and we try tracking with open geometry
      //G4GeometryManager::GetInstance()->OpenGeometry();
      throw exception;
    } 
}

BDSIM::~BDSIM()
{
  /// Termination & clean up.
  G4GeometryManager::GetInstance()->OpenGeometry();
    
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "deleting..." << G4endl;
#endif
  delete bdsOutput;

  try
    {
      // order important here because of singletons relying on each other
	  delete BDSSDManager::Instance();
      delete BDSBeamPipeFactory::Instance();
      delete BDSCavityFactory::Instance();
      delete BDSGeometryFactory::Instance();
      delete BDSAcceleratorModel::Instance();
      delete BDSTemporaryFiles::Instance();
      delete BDSFieldFactory::Instance(); // this uses BDSGlobalConstants which uses BDSMaterials
      delete BDSGlobalConstants::Instance();
      delete BDSMaterials::Instance();
      
      // instances not used in this file, but no other good location for deletion
      if (initialisationResult < 2)
	{
	  delete BDSColours::Instance();
	  delete BDSFieldLoader::Instance();
	  delete BDSSamplerRegistry::Instance();
	}
    }
  catch (...)
    {;} // ignore any exception as this is a destructor
      
  delete runManager;
  delete bdsBunch;
  delete parser;

  if (usualPrintOut)
    {G4cout << __METHOD_NAME__ << "End of Run. Thank you for using BDSIM!" << G4endl;}
}

void BDSIM::RegisterUserComponent(G4String componentTypeName,
				  BDSComponentConstructor* componentConstructor)
{
  if (initialised)
    {BDS::Warning(__METHOD_NAME__, "BDSIM kernel already initialised - this component will not be available");}
  if (!userComponentFactory)
    {userComponentFactory = new BDSComponentFactoryUser();}

  userComponentFactory->RegisterComponent(componentTypeName,
					  componentConstructor);
}
