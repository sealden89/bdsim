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
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"
#include "BDSRunManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4TwoVector.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <signal.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include <sys/resource.h>
#include <sys/stat.h>

#ifdef __APPLE__
#include <mach-o/dyld.h> // for executable path
#endif


G4bool BDS::non_alpha::operator()(char c)
{
  return !isalpha(c);
}

G4String BDS::PrepareSafeName(G4String name)
{
  //remove white space
  name.erase(std::remove_if(name.begin(),name.end(),isspace),name.end());
  //remove non alpha numeric characters
  std::replace_if(name.begin(),name.end(),BDS::non_alpha(),'_');
  
  return name;
}

G4int BDS::CalculateOrientation(G4double angle)
{
  G4int orientation;
  if (angle < 0)
    {orientation = 1;}
  else
    {orientation = -1;}
  return orientation;
}

std::pair<G4ThreeVector,G4ThreeVector> BDS::CalculateFaces(G4double angleIn,
							   G4double angleOut)
{
  /// orientation -1,0,1 value - always use |angle| with trigonometric and then
  /// multiply by this factor, 0 by default
  G4int orientationIn  = BDS::CalculateOrientation(angleIn);
  G4int orientationOut = BDS::CalculateOrientation(angleOut);
  
  G4double in_z  = std::cos(std::abs(angleIn)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x  = std::sin(std::abs(angleIn)); // note full angle here as it's the exit angle
  G4double out_z = std::cos(std::abs(angleOut));
  G4double out_x = std::sin(std::abs(angleOut));
  G4ThreeVector inputface  = G4ThreeVector(orientationIn*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  G4ThreeVector outputface = G4ThreeVector(orientationOut*out_x, 0.0, out_z);   // no output face angle
  return std::make_pair(inputface,outputface);
}

void BDS::EnsureInLimits(G4double& value, G4double lowerLimit, G4double upperLimit)
{
  if (value < lowerLimit)
    {value = lowerLimit;}
  if (value > upperLimit)
    {value = upperLimit;}
}

G4bool BDS::FileExists(G4String fileName)
{
  std::ifstream infile(fileName.c_str());
  return infile.good();
  // note the destructor of ifstream will close the stream
}

G4bool BDS::DirectoryExists(G4String path)
{
  struct stat sb;  
  bool result = (stat(path.c_str(), &sb) == 0) && S_ISDIR(sb.st_mode);
  return G4bool(result);
}

std::string BDS::GetCurrentDir()
{ 
  char currentPath[PATH_MAX]; // defined in <limits>
  std::string currentPathString;

  if (getcwd(currentPath, sizeof(currentPath)) != NULL)
    {currentPathString = std::string(currentPath);}
  else
    {G4cerr << "Cannot determine current working directory" << G4endl; exit(1);}

  return currentPathString;
}

std::string BDS::GetBDSIMExecPath()
{
  // get path to executable (platform dependent)
  char path[1024];
#ifdef __linux__
  // get path from /proc/self/exe
  ssize_t len = ::readlink("/proc/self/exe", path, sizeof(path) - 1);
  if (len != -1)
    {path[len] = '\0';}
#elif __APPLE__
  uint32_t size = sizeof(path);
  if (_NSGetExecutablePath(path, &size) != 0)
    {std::cout << "buffer too small; need size " << size << std::endl;}
#endif
  std::string bdsimPath(path);
  // remove executable from path
  std::string::size_type found = bdsimPath.rfind("/"); // find the last '/'
  if (found != std::string::npos)
    {bdsimPath = bdsimPath.substr(0,found+1);} // the path is the bit before that, including the '/'
  return bdsimPath;
}

G4String BDS::GetFullPath(G4String fileName, bool excludeNameFromPath)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << fileName << " strip name off?: " << excludeNameFromPath << G4endl;
#endif
  //Return fullPath of a file:
  //mirror what is done in parser.l (i.e. if no environment variable set, assume base filename path is that of the gmad file).
  // 1) if absolute path (starting with a slash) return that
  // 2) if relative path, then
  // 2a) return path relative to environment variable BDSIMPATH (if set)
  // 2b) return path relative to main BDSIM input gmad file

  // return value
  G4String fullPath;

  // protect against unneeded ./ at beginning of filename - strip it off
  if (fileName.substr(0,2) == "./")
    {fileName = fileName.substr(2);}

  // split input into path and filename
  G4String inputFilepath, inputFilename;
  BDS::SplitPathAndFileName(fileName, inputFilepath, inputFilename);
  
  // need to know whether it's an absolute or relative path
  if ((fileName.substr(0,1)) == "/")
    {fullPath = inputFilepath;}
  else // the main file has a relative path or just the file name, add bdsimpath
    {
      if (inputFilepath == "./") // don't insert a ./ in path
	{fullPath = BDSGlobalConstants::Instance()->BDSIMPath();}
      else
	{fullPath = BDSGlobalConstants::Instance()->BDSIMPath() + inputFilepath;}
    }
  
  if (fullPath.back() != '/') // ensure ends in '/'
    {fullPath += "/";} // only add if needed

  // add filename if not excluded
  if (!excludeNameFromPath)
    {fullPath += inputFilename;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined full path to be: " << fullPath << G4endl;
#endif
  return fullPath;
}

void BDS::SplitPathAndFileName(const G4String& filePath,
			       G4String&       path,
			       G4String&       fileName)
{
  G4String::size_type found = filePath.rfind("/"); // find the last '/'
  if (found != G4String::npos)
    {
      path     = filePath.substr(0,found) + "/"; // the path is the bit before that
      fileName = filePath.substr(found+1); // the rest
    }
  else
    {// no slash, only file name
      path     = "./";     // use current directory
      fileName = filePath;
    }
}

void BDS::SplitFileAndExtention(const G4String& fileName,
				G4String&       file,
				G4String&       extension)
{
  G4String::size_type found = fileName.rfind("."); // fine the last '.'
  if (found != G4String::npos)
    {
      file      = fileName.substr(0, found);
      extension = fileName.substr(found); // the rest
    }
  else
    {
      file      = fileName;
      extension = "";
    }
}

void BDS::HandleAborts(int signal_number)
{
  /** 
      Try to catch abort signals. This is not guaranteed to work.
      Main goal is to close output stream / files.
  */
  // prevent recursive calling
  static int nrOfCalls=0;
  if (nrOfCalls>0)
    {exit(1);}
  nrOfCalls++;
  std::cout << std::endl << "BDSIM is about to crash or was interrupted! " << std::endl;
  std::cout << "With signal: " << strsignal(signal_number) << std::endl;
  
  BDSRunManager::GetRunManager()->AbortRun();
  std::cout << "Ave, Imperator, morituri te salutant!" << std::endl;
}

G4bool BDS::IsFinite(G4double value,
		     G4double tolerance)
{
  return std::abs(value) > tolerance;
}

G4bool BDS::NonZero(G4double value)
{
  return std::abs(value) > std::numeric_limits<double>::min();
}

G4bool BDS::IsFiniteStrength(G4double value)
{
  return IsFinite(value, 1e-50);
}

G4bool BDS::IsFinite(const G4ThreeVector& value,
		     G4double tolerance)
{
  G4bool resultX = BDS::IsFinite(value.x(), tolerance);
  G4bool resultY = BDS::IsFinite(value.y(), tolerance);
  G4bool resultZ = BDS::IsFinite(value.z(), tolerance);
  return resultX || resultY || resultZ;
}

G4bool BDS::IsInteger(const char* ch, int& convertedInteger)
{
  // from http://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
  // convert to string
  std::string s(ch);
  if(s.empty() || ((!std::isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
    {return false;}
  
  char * p;
  convertedInteger = std::strtol(ch, &p, 10);
  
  return (*p == 0);
}

G4bool BDS::IsNumber(const char* ch, double& convertedNumber)
{
  // from http://stackoverflow.com/questions/2844817/how-do-i-check-if-a-c-string-is-an-int
  // convert to string
  std::string s(ch);
  if(s.empty() || ((!std::isdigit(s[0])) && (s[0] != '-') && (s[0] != '+')))
    {return false;}
  
  char * p;
  convertedNumber = std::strtod(ch, &p);
  
  return (*p == 0);
}

void BDS::PrintRotationMatrix(G4RotationMatrix* rm, G4String keyName)
{
  G4cout << "Rotation matrix - reference: \"" << keyName << "\"" << *rm << G4endl;
  G4cout << "unit x -> " << G4ThreeVector(1,0,0).transform(*rm) << G4endl;
  G4cout << "unit y -> " << G4ThreeVector(0,1,0).transform(*rm) << G4endl;
  G4cout << "unit z -> " << G4ThreeVector(0,0,1).transform(*rm) << G4endl;
}

G4bool BDS::Geant4EnvironmentIsSet()
{
  std::vector<G4String> variables = {//"G4ABLADATA",
				     "G4NEUTRONHPDATA",
				     "G4RADIOACTIVEDATA",
				     "G4LEDATA",
#if G4VERSION_NUMBER < 1049
				     "G4NEUTRONXSDATA",
#else
				     "G4PARTICLEXSDATA",
#endif
				     "G4REALSURFACEDATA",
				     "G4LEVELGAMMADATA",
				     "G4PIIDATA",
				     "G4SAIDXSDATA"};

  G4bool result = true;
  for (auto it = variables.begin(); it != variables.end(); ++it)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "testing for variable: " << *it;
#endif
      const char* env_p = std::getenv( (*it).c_str() );
      if (!env_p)
	{
	  result = false;
#ifdef BDSDEBUG
	  G4cout << " - not found" << G4endl;
	}
      else
	{
	  G4cout << " - found" << G4endl;
#endif
	}
    }
  return result;
}

void BDS::CheckHighPrecisionDataExists(const G4String& physicsListName)
{
  const char* envHPData = std::getenv("G4PARTICLEHPDATA");
  if (!envHPData)
    {
      G4cerr << "The G4TENDL low energy data is not available!" << G4endl;
      G4cout << "G4TENDL data is required through the environmental variable "
	     << "\"G4PARTICLEHPDATA\"" << G4endl;
      G4cout << "This is required for the \"" << physicsListName << "\" physics list." << G4endl;
      G4cout << "This data is an optional download from the Geant4 website. Please "
	     << "download from the Geant4 website and export the environmental variable." << G4endl;
      exit(1);
    }
}

void BDS::CheckLowEnergyNeutronDataExists(const G4String& physicsListName)
{
  const char* envHPData = std::getenv("G4LENDDATA");
  if (!envHPData)
    {
      G4cerr << "The Low Energy Neutron Data ('LEND') is not available!" << G4endl;
      G4cout << "Data is required through the environmental variable "
	     << "\"G4LENDDATA\"" << G4endl;
      G4cout << "This is required for the \"" << physicsListName << "\" physics list." << G4endl;
      G4cout << "This data is an optional download from the Geant4 website. Please "
	     << "download from the Geant4 website and export the environmental variable." << G4endl;
      exit(1);
    }
}

G4double BDS::GetParameterValueDouble(G4String spec, G4String name)
{
  try
    {return (G4double)std::stol(GetParameterValueString(spec,name).c_str());}
  catch(const std::invalid_argument& e)
    {throw;}
}

G4int BDS::GetParameterValueInt(G4String spec, G4String name)
{
  try
    {return (G4int)std::stoi(GetParameterValueString(spec,name).c_str());}
  catch (const std::invalid_argument& e)
    {throw;}
}

G4String BDS::GetParameterValueString(G4String spec, G4String name)
{
  G4String value;
  std::string param = name + "=";

  int pos = spec.find(param);
  if( pos >= 0 )
    {
      
      int pos2 = spec.find("&",pos);
      int pos3 = spec.length();
      int tend = pos2 < 0 ? pos3 : pos2; 
      int llen = tend - pos - param.length();
      
      value = spec.substr(pos + param.length(), llen);
  }
  return value;
}

G4TwoVector BDS::Rotate(const G4TwoVector& vec, const G4double& angle)
{
  G4double xp = vec.x()*std::cos(angle) - vec.y()*std::sin(angle);
  G4double yp = vec.x()*std::sin(angle) + vec.y()*std::cos(angle);
  return G4TwoVector(xp,yp);
}

G4bool BDS::WillIntersect(const G4ThreeVector& incomingNormal,
			  const G4ThreeVector& outgoingNormal,
			  const G4double&      zSeparation,
			  const BDSExtent&     incomingExtent,
			  const BDSExtent&     outgoingExtent)
{  
  // for any two normal vectors of planes - if their cross
  // product is zero, then they're (anti) parallel and will
  // never intersect
  G4ThreeVector cross = incomingNormal.cross(outgoingNormal);
  G4double det = cross.mag2();
  if (!BDS::IsFinite(det))
    {return false;}

  // shortcuts / copies - OG for outgoing and IC for incoming
  const BDSExtent& eog = outgoingExtent;
  const BDSExtent& eic = incomingExtent;

  // z coordinate of points at maximum extents in x,y
  G4double xNegYNegOG = BDS::GetZOfPointOnPlane(outgoingNormal, eog.XNeg(), eog.YNeg());
  G4double xNegYPosOG = BDS::GetZOfPointOnPlane(outgoingNormal, eog.XNeg(), eog.YPos());
  G4double xPosYPosOG = BDS::GetZOfPointOnPlane(outgoingNormal, eog.XPos(), eog.YPos());
  G4double xPosYNegOG = BDS::GetZOfPointOnPlane(outgoingNormal, eog.XPos(), eog.YNeg());
  G4double xNegYNegIC = BDS::GetZOfPointOnPlane(incomingNormal, eic.XNeg(), eic.YNeg());
  G4double xNegYPosIC = BDS::GetZOfPointOnPlane(incomingNormal, eic.XNeg(), eic.YPos());
  G4double xPosYPosIC = BDS::GetZOfPointOnPlane(incomingNormal, eic.XPos(), eic.YPos());
  G4double xPosYNegIC = BDS::GetZOfPointOnPlane(incomingNormal, eic.XPos(), eic.YNeg());
 
  // test of they'd overlap
  G4bool xNegYNegFail = xNegYNegIC > (zSeparation + xNegYNegOG);
  G4bool xNegYPosFail = xNegYPosIC > (zSeparation + xNegYPosOG);
  G4bool xPosYPosFail = xPosYPosIC > (zSeparation + xPosYPosOG);
  G4bool xPosYNegFail = xPosYNegIC > (zSeparation + xPosYNegOG);

  if (xNegYNegFail || xNegYPosFail || xPosYPosFail || xPosYNegFail)
    {return true;}
  else // shouldn't happen
    {return false;}
}

G4bool BDS::WillIntersect(const G4double& angleIn,
			  const G4double& angleOut,
			  const G4double& horizontalWidth,
			  const G4double& length)
{
  // Calculate the z component of triangle with each angle and
  // axis along length.
  G4double dzIn  = horizontalWidth * tan(angleIn);
  G4double dzOut = horizontalWidth * tan(angleOut);
  if (dzIn > length - dzOut)
    {return true;}
  else
    {return false;}
}

G4double BDS::GetZOfPointOnPlane(G4ThreeVector normal, G4double x, G4double y)
{
  // equation of a plane with offset v_0, normal unit n and any point on plane v
  // n.(v-v_0) = 0
  // for equation of plane that intercepts 0,0,0
  // n.v = 0;
  // exanding dot product
  // n.x()*v.x() + n.y()*v.y() + n.z()*v.z() = 0;
  // for given x and y can solve for z
  // v.z = (-n.x*v.x - n.y*v.y ) / n.z;

  return (-normal.x()*x - normal.y()*y) / normal.z();
}

G4ThreeVector BDS::RotateToReferenceFrame(G4ThreeVector faceNormal, G4double fullAngle)
{
  if (!BDS::IsFinite(fullAngle))
    {return faceNormal;} // no angle -> no rotation
  G4RotationMatrix rm = G4RotationMatrix();
  rm.rotateY(fullAngle*0.5);
  return faceNormal.transform(rm);
}

std::pair<G4String, G4String> BDS::SplitOnColon(G4String formatAndPath)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << formatAndPath << G4endl;
#endif
  if(!formatAndPath.empty())
    {
      std::size_t found = formatAndPath.find(":");
      if (found == std::string::npos)
	{
	  G4cerr << __METHOD_NAME__ << "invalid specifier \""
		 << formatAndPath << "\"" << G4endl;
	  G4cerr << "Missing \":\" to separate format and file path" << G4endl;
	  exit(1);
	}
      else
	{
	  G4String format   = formatAndPath.substr(0,found);
	  G4String filePath = formatAndPath.substr(found+1); // get everything after ":"
#ifdef BDSDEBUG
	G4cout << __METHOD_NAME__ << "format: " << format   << G4endl;
	G4cout << __METHOD_NAME__ << "file:   " << filePath << G4endl;
#endif
	return std::make_pair(format,filePath);
	}
    }
  return std::make_pair("","");
}

G4UserLimits* BDS::CreateUserLimits(G4UserLimits*  defaultUL,
				    const G4double length,
				    const G4double fraction)
{
  G4UserLimits* result = nullptr;
  // construct a dummy G4Track that typically isn't used for the check
  G4Track t = G4Track();
  if (defaultUL->GetMaxAllowedStep(t) > length)
    {// copy and change length in UL
      result = new G4UserLimits(*defaultUL);
      result->SetMaxAllowedStep(length * fraction);
    }
  else
    {result = defaultUL;} // stick with length in defaultUL
  return result;
}

G4double BDS::GetMemoryUsage()
{
  struct rusage r_usage;
  G4double result = getrusage(RUSAGE_SELF,&r_usage);
  return result;
}

std::map<G4String, G4String> BDS::GetUserParametersMap(G4String userParameters)
{
  // split by white space then by colon
  std::istringstream iss(userParameters);
  std::vector<std::string> paramaterPairs(std::istream_iterator<std::string>{iss},
					  std::istream_iterator<std::string>{});

  std::map<G4String, G4String> result;
  for (auto& pair : paramaterPairs)
    {
      auto index = pair.find(":");
      std::string key = pair.substr(0, index);
      std::string value = pair.substr(index+1);
      result[G4String(key)] = G4String(value);
    }
  return result;
}
