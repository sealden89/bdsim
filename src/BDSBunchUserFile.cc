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
#include "BDSBunchUserFile.hh"
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4String.hh"

#ifdef USE_GZSTREAM
#include "src-external/gzstream/gzstream.h"
#endif

#include <fstream>
#include <regex>
#include <string>
#include <sstream>
#include <vector>

template <class T>
BDSBunchUserFile<T>::BDSBunchUserFile():
  distrFile(""),
  distrFilePath(""),
  bunchFormat(""),
  nlinesIgnore(0),
  particleMass(0),
  lineCounter(0)
{
  ffact = BDSGlobalConstants::Instance()->FFact();
}

template<class T>
void BDSBunchUserFile<T>::CheckParameters()
{
  BDSBunch::CheckParameters();
  if (distrFile.empty())
    {G4cerr << __METHOD_NAME__ << "No input file specified for distribution" << G4endl; exit(1);}
}

template<class T>
BDSBunchUserFile<T>::~BDSBunchUserFile()
{
  CloseBunchFile();
}

template<class T>
void BDSBunchUserFile<T>::OpenBunchFile()
{
  lineCounter = 0;
  InputBunchFile.open(distrFilePath);
  if (!InputBunchFile.good())
    {throw BDSException(__METHOD_NAME__, "Cannot open bunch file " + distrFilePath);}
}

template<class T>
void BDSBunchUserFile<T>::CloseBunchFile()
{
  InputBunchFile.clear(); // igzstream doesn't reset eof flags when closing - do manually
  InputBunchFile.close();
}

template<class T>
void BDSBunchUserFile<T>::ParseFileFormat()
{
  struct BDSBunchUserFile::Doublet sd;

  std::regex wspace(":"); // any whitepsace
  std::vector<std::string> results;
  std::sregex_token_iterator iter(bunchFormat.begin(), bunchFormat.end(), wspace, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  for (auto const& token : results)
    {
    if(token.substr(0,1)=="E" || token.substr(0,1)=="P") {
      G4String name,rest;
      if(token.substr(0,2)=="Ek") {//Kinetic energy (longer name).
	name = token.substr(0,2);
	rest = token.substr(2);
      } else {
	name = token.substr(0,1);
	rest = token.substr(1);
      }
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = name;
	sd.unit = ParseEnergyUnit(fmt);
	fields.push_back(sd);
      }
    } else if(token.substr(0,1)=="t") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"t!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = "t"; 
        sd.unit = ParseTimeUnit(fmt);
	fields.push_back(sd);
      }
    } else if( ( token.substr(0,1)=="x" && token.substr(1,1)!="p" ) ||
	       ( token.substr(0,1)=="y" && token.substr(1,1)!="p" ) ||
	       ( token.substr(0,1)=="z" && token.substr(1,1)!="p" ) ) {
      G4String name = token.substr(0,1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 

	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name=name;
	sd.unit=ParseLengthUnit(fmt);	
	fields.push_back(sd);	
      }
    } else if ( (token.substr(0,2)=="xp") ||
		(token.substr(0,2)=="yp") ||
		(token.substr(0,2)=="zp") ) {
      G4String name = token.substr(0,2);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
      G4String rest = token.substr(2);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name=name;
	sd.unit=ParseAngleUnit(fmt);
	fields.push_back(sd);
      }
    }else if(token.substr(0,2)=="pt") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"pt!"<<G4endl;
#endif
      sd.name="pt";
      sd.unit=1;
      fields.push_back(sd);
    } else if(token.substr(0,1)=="w") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"weight!"<<G4endl;
#endif
      sd.name="weight";
      sd.unit=1;
      fields.push_back(sd);
    } else if (token.substr(0,1)=="-") {
      // skip
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << "skip token " <<G4endl;
#endif
      sd.name="skip";
      sd.unit=1;
      fields.push_back(sd);
    } else {
      G4cerr << "Cannot determine bunch data format" << G4endl; exit(1);
    }
  } 
  return;
}

template<class T>
void BDSBunchUserFile<T>::skip(std::stringstream& ss, G4int nValues)
{
  G4double dummyValue;
  for (G4int i = 0; i < nValues; i++)
    {ReadValue(ss, dummyValue);}
}

template<class T>
void BDSBunchUserFile<T>::SkipLines()
{
  if (BDS::IsFinite(nlinesIgnore))
    {
      G4cout << "BDSBunchUserFile> skipping " << nlinesIgnore << " lines" << G4endl;
      std::string line;
      for (G4int i = 0; i < nlinesIgnore; i++)
	{
	  std::getline(InputBunchFile, line);
	  lineCounter++;
	}
    }
}

template<class T>
void BDSBunchUserFile<T>::SetOptions(const BDSParticleDefinition* beamParticle,
				     const GMAD::Beam& beam,
				     const BDSBunchType& distrType,
				     G4Transform3D beamlineTransformIn,
				     const G4double beamlineSIn)
{
  BDSBunch::SetOptions(beamParticle, beam, distrType, beamlineTransformIn, beamlineSIn);
  particleMass = beamParticle->Mass();
  distrFile     = beam.distrFile;
  distrFilePath = BDS::GetFullPath(beam.distrFile);
  bunchFormat   = beam.distrFileFormat;
  nlinesIgnore  = beam.nlinesIgnore;
  ParseFileFormat();
  OpenBunchFile(); 
  SkipLines();
}

template<class T>
G4double BDSBunchUserFile<T>::ParseEnergyUnit(const G4String& fmt)
{
  G4double unit=1.;
  if (fmt=="TeV") unit=1.e3;
  else if(fmt=="GeV") unit=1;
  else if(fmt=="MeV") unit=1.e-3;
  else if(fmt=="KeV") unit=1.e-6;
  else if(fmt=="eV") unit=1.e-9;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised energy unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
G4double BDSBunchUserFile<T>::ParseLengthUnit(const G4String& fmt)
{
  G4double unit=1.;
  if(fmt=="m") unit=1;
  else if(fmt=="cm") unit=1.e-2;
  else if(fmt=="mm") unit=1.e-3;
  else if(fmt=="mum" || fmt=="um") unit=1.e-6;
  else if(fmt=="nm") unit=1.e-9;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised length unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
G4double BDSBunchUserFile<T>::ParseAngleUnit(const G4String& fmt)
{
  G4double unit=1.;
  if(fmt=="rad") unit=1;
  else if(fmt=="mrad") unit=1.e-3;
  else if(fmt=="murad" || fmt=="urad") unit=1.e-6;
  else if(fmt=="nrad") unit=1.e-9;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised angle unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}
template<class T>
G4double BDSBunchUserFile<T>::ParseTimeUnit(const G4String& fmt)
{
  G4double unit=1.;
  if(fmt=="s") unit=1;
  else if(fmt=="ms") unit=1.e-3;
  else if(fmt=="mus" || fmt=="us") unit=1.e-6;
  else if(fmt=="ns") unit=1.e-9;
  else if(fmt=="mm/c") unit=(CLHEP::mm/CLHEP::c_light)/CLHEP::s;
  else if(fmt=="nm/c") unit=(CLHEP::nm/CLHEP::c_light)/CLHEP::s;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised time unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
void BDSBunchUserFile<T>::EndOfFileAction()
{
  // If the end of the file is reached go back to the beginning of the file.
  // this re reads the same file again - must always print warning
  G4cout << "BDSBunchUserFile> End of file reached. Returning to beginning of file for next event." << G4endl;
  CloseBunchFile();
  OpenBunchFile();
  SkipLines();
}

template<class T>
void BDSBunchUserFile<T>::RecreateAdvanceToEvent(G4int eventOffset)
{
  G4cout << "BDSBunchUserFile::RecreateAdvanceToEvent> Advancing file to event: " << eventOffset << G4endl;
  if (InputBunchFile.eof())
    {EndOfFileAction();}
  std::string line;
  for (G4int i = 0; i < eventOffset; i++)
    {
      std::getline(InputBunchFile, line);
      lineCounter++;
      if (InputBunchFile.eof())
	{EndOfFileAction();}
    }
}

template<class T>
BDSParticleCoordsFull BDSBunchUserFile<T>::GetNextParticleLocal()
{
  if (InputBunchFile.eof())
    {EndOfFileAction();}

  G4double E = 0, x = 0, y = 0, z = 0, xp = 0, yp = 0, zp = 0, t = 0;
  G4double weight = 1;
  G4int type = 0;
  
  G4bool zpdef = false; //keeps record whether zp has been read from file
  G4bool tdef  = false; //keeps record whether t has been read from file

  // we only update the particle definition at the end so we continue to read
  // the rest of the line bit by bit - could be improved and read the whole line
  // at once for safety and robustness of moving on to the next event.
  G4bool updateParticleDefinition = false;
  std::string line;
  std::getline(InputBunchFile, line);
  lineCounter++;

  // skip empty lines and comment lines
  std::regex comment("^\\#.*");
  G4bool lineIsBad = true;
  while (lineIsBad)
    {
      if (std::all_of(line.begin(), line.end(), isspace) || std::regex_search(line, comment))
	{
	  if (InputBunchFile.eof())
	    {EndOfFileAction();}
	  else
	    {
	      std::getline(InputBunchFile, line);
	      lineCounter++;
	      continue;
	    }
	}
      else
	{lineIsBad = false;}
    }
  
  // no check the line has the right number of 'words' in it ->
  // split line on white space - doesn't inspect words themselves
  // checks number of words, ie number of columns is correct
  std::vector<std::string> results;
  std::regex wspace("\\s+"); // any whitepsace
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  
  if (results.size() < fields.size())
    {// ensure enough columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
	+ " - invalid number of columns";
      throw BDSException(__METHOD_NAME__, errString);
    } 

  std::stringstream ss(line);
  for (auto it=fields.begin();it!=fields.end();it++)
    {
      if(it->name=="Ek")
	{ 
	  ReadValue(ss, E);
	  E *= (CLHEP::GeV * it->unit);
	  E += particleMass;
	}
      else if(it->name=="E")
	{
	  ReadValue(ss, E);
	  E *= (CLHEP::GeV * it->unit);
	}
      else if(it->name=="P")
	{ 
	  G4double P=0;
	  ReadValue(ss, P); P *= (CLHEP::GeV * it->unit); //Paticle momentum
	  G4double totalEnergy = std::hypot(P,particleMass);
	  E = totalEnergy - particleMass;
	}
      else if(it->name=="t")
	{ReadValue(ss, t); t *= (CLHEP::s * it->unit); tdef = true;}
      else if(it->name=="x")
	{ReadValue(ss, x); x *= (CLHEP::m * it->unit);}
      else if(it->name=="y")
	{ReadValue(ss, y); y *= (CLHEP::m * it->unit);}
      else if(it->name=="z")
	{ReadValue(ss, z); z *= (CLHEP::m * it->unit);}
      else if(it->name=="xp") { ReadValue(ss, xp); xp *= ( CLHEP::radian * it->unit ); }
      else if(it->name=="yp") { ReadValue(ss, yp); yp *= ( CLHEP::radian * it->unit ); }
      else if(it->name=="zp") { ReadValue(ss, zp); zp *= ( CLHEP::radian * it->unit ); zpdef = true;}
      else if(it->name=="pt")
	{// particle type
	  // update base class flag - user file can specify different particles
	  if (!particleCanBeDifferent)
	    {particleCanBeDifferent = true;}
	  ReadValue(ss, type);
	  updateParticleDefinition = true; // update particle definition after reading line
	}
      else if(it->name=="weight")
	{ReadValue(ss, weight);}
      
      else if(it->name=="skip")
	{double dummy; ReadValue(ss, dummy);}
    }

  // coordinate checks
  // If energy isn't specified, use the central beam energy (kinetic for Geant4)
  if (!BDS::IsFinite(E))
    {E = E0;}
  
  // compute zp from xp and yp if it hasn't been read from file
  if (!zpdef)
    {zp = CalculateZp(xp,yp,1);}
  // compute t from z if it hasn't been read from file
  if (!tdef)
    {t=0;}
  
  if (updateParticleDefinition)
    {
      // type is an int so FindParticle(int) is used here
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4ParticleDefinition* particleDef = particleTable->FindParticle(type);
      if (!particleDef)
	{throw BDSException("BDSBunchUserFile> Particle \"" + std::to_string(type) + "\" not found");}
	      
      // Wrap in our class that calculates momentum and kinetic energy.
      // Requires that total energy 'E' already be set.
      delete particleDefinition;
      particleDefinition = new BDSParticleDefinition(particleDef, E, ffact); // update member
      updateParticleDefinition = false; // reset it back to false
    }

  return BDSParticleCoordsFull(x,y,Z0+z,xp,yp,zp,t,S0+z,E,weight);
}

template <class T>
template <typename Type>
void BDSBunchUserFile<T>::ReadValue(std::stringstream& stream, Type& value)
{
  stream >> value;
}

template class BDSBunchUserFile<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSBunchUserFile<igzstream>;
#endif
