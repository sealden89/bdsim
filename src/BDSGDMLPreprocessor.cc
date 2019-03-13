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
#ifdef USE_GDML
#include "BDSDebug.hh"
#include "BDSException.hh"
#include "BDSGDMLPreprocessor.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSUtilities.hh"

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "globals.hh"

#include <algorithm>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

using namespace xercesc;

G4String BDS::PreprocessGDML(const G4String& file,
			     const G4String& prefix)
{
  BDSGDMLPreprocessor processor;
  G4String processedFile = processor.PreprocessFile(file, prefix);
  return processedFile;
}

G4String BDS::GDMLSchemaLocation()
{
  G4String result;
  G4String bdsimExecPath = BDS::GetBDSIMExecPath();
  G4String localPath = bdsimExecPath + "src-external/gdml/schema/gdml.xsd";
  G4String installPath = bdsimExecPath + "../share/bdsim/gdml/schema/gdml.xsd";
  if (FILE *file = fopen(localPath.c_str(), "r"))
    {
      fclose(file);
      return localPath;
    }
  else if ( (file = fopen(installPath.c_str(), "r")) )
    {
      fclose(file);
      return installPath;
    }
  else
   {throw BDSException(__METHOD_NAME__, "ERROR: local GDML schema could not be found!");}
}

BDSGDMLPreprocessor::BDSGDMLPreprocessor()
{
  //ignoreNodes = {"setup"};
  ignoreAttrs = {"formula"};
}

BDSGDMLPreprocessor::~BDSGDMLPreprocessor()
{;}

G4String BDSGDMLPreprocessor::PreprocessFile(const G4String& file,
					     const G4String& prefix)
{
  G4cout << __METHOD_NAME__ << "Preprocessing GDML file " << file << G4endl;
  
  try
    {XMLPlatformUtils::Initialize();}
  catch (const XMLException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.getMessage());
      std::stringstream messageSS;
      messageSS << "Error during initialization! :\n" << message << "\n";
      XMLString::release(&message);
      throw BDSException(__METHOD_NAME__, messageSS.str());
    }

  /// Update folder containing gdml file.
  G4String filename;
  BDS::SplitPathAndFileName(file, parentDir, filename);
  
  XercesDOMParser* parser = new XercesDOMParser();
  parser->setValidationScheme(XercesDOMParser::Val_Always);
  parser->setDoNamespaces(true);    // optional

  ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
  parser->setErrorHandler(errHandler);

  try
    {parser->parse(file.c_str());}
  catch (const XMLException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.getMessage());
      std::stringstream messageSS;
      messageSS << "Exception message is: \n" << message << "\n";
      XMLString::release(&message);
      throw BDSException(__METHOD_NAME__, messageSS.str());
    }
  catch (const DOMException& toCatch)
    {
      char* message = XMLString::transcode(toCatch.msg);
      std::stringstream messageSS;
      messageSS << "Exception message is: \n" << message << "\n";
      XMLString::release(&message);
      throw BDSException(__METHOD_NAME__, messageSS.str());
    }
  catch (...)
    {throw BDSException(__METHOD_NAME__, "Unexpected Exception");}
  
  // walk through all nodes to extract names and attributes
  DOMDocument* doc           = parser->getDocument();
  DOMElement* docRootNode    = doc->getDocumentElement();
  DOMNodeIterator* docWalker = doc->createNodeIterator(docRootNode, DOMNodeFilter::SHOW_ELEMENT,nullptr,true);
  // map structure and all names used
  ReadDoc(docWalker);

  // reset iterator
  docWalker->detach();
  docWalker = doc->createNodeIterator(docRootNode, DOMNodeFilter::SHOW_ELEMENT,nullptr,true);
  
  // rewrite all names in loaded structure.
  ProcessDoc(docWalker, prefix);

  // create new temporary file that modified gdml can be written to.
  G4String newFile = BDSTemporaryFiles::Instance()->CreateTemporaryFile(file, prefix);
  
  // write file from DOM
  DOMImplementation* pImplement        = DOMImplementationRegistry::getDOMImplementation(XMLString::transcode("LS"));
  DOMLSSerializer*   pSerializer       = (static_cast<DOMImplementationLS*>(pImplement))->createLSSerializer();
  DOMConfiguration*  pDomConfiguration = pSerializer->getDomConfig();
  pDomConfiguration->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
  XMLFormatTarget*   pTarget           = new LocalFileFormatTarget(newFile);
  DOMLSOutput*       pDomLsOutput      = (static_cast<DOMImplementationLS*>(pImplement))->createLSOutput();
  pDomLsOutput->setByteStream(pTarget);  
  pSerializer->write(doc, pDomLsOutput);
  pSerializer->release();

  G4cout << __METHOD_NAME__ << "Preprocessing complete" << G4endl;

  delete pTarget;
  delete parser;
  delete errHandler;

  return newFile;
}

void BDSGDMLPreprocessor::ReadDoc(DOMNodeIterator* docIterator)
{
  for (DOMNode* currentNode = docIterator->nextNode(); currentNode != 0; currentNode = docIterator->nextNode())
    {ReadNode(currentNode);}
}

void BDSGDMLPreprocessor::ReadNode(DOMNode* node)
{
  if (!node)
    {return;}

  std::string thisNodeName = XMLString::transcode(node->getNodeName());
  if (thisNodeName == "gdml")
    {// to update location of schema
      ProcessGDMLNode(node->getAttributes());
      return;
    }
  auto search = std::find(ignoreNodes.begin(), ignoreNodes.end(), thisNodeName);
  if (search != ignoreNodes.end())
    {return;} // ignore this node
  else
    {ReadAttributes(node->getAttributes());}
}

void BDSGDMLPreprocessor::ProcessGDMLNode(DOMNamedNodeMap* attributeMap)
{
  if (!attributeMap)
  {return;}

  for(XMLSize_t i = 0; i < attributeMap->getLength(); i++)
    {
      DOMNode* attr = attributeMap->item(i);
      std::string nodeName = XMLString::transcode(attr->getNodeName());
      if (nodeName == "xsi:noNamespaceSchemaLocation")
	{
	  G4String nodeValue = G4String(XMLString::transcode(attr->getNodeValue()));
	  G4String newNodeValue;
	  if (nodeValue.substr(0,2) == "./")
	    {
	      G4String remainder = nodeValue.substr(2); // strip off ./
	      newNodeValue = remainder.prepend(parentDir); // prepend parent directory
	    }
	  else
	    {newNodeValue = BDS::GDMLSchemaLocation();}
	  attr->setNodeValue(XMLString::transcode(newNodeValue.c_str()));
	} 
    }
}

void BDSGDMLPreprocessor::ReadAttributes(DOMNamedNodeMap* attributeMap)
{
  if (!attributeMap)
    {return;}
  
  for(XMLSize_t i = 0; i < attributeMap->getLength(); i++)
    {
      DOMNode* attr = attributeMap->item(i);
      std::string name = XMLString::transcode(attr->getNodeValue());
      auto search = std::find(ignoreAttrs.begin(), ignoreAttrs.end(), name);
      if (search != ignoreAttrs.end())
        {continue;} // ignore this attribute
      if(XMLString::compareIString(attr->getNodeName(), XMLString::transcode("name")) == 0)
	{
	  names.push_back(name);
	  count[name] = 0;
	}
    }
}

void BDSGDMLPreprocessor::ProcessDoc(DOMNodeIterator* docIterator,
				     const G4String&  prefix)
{
  for (DOMNode* currentNode = docIterator->nextNode(); currentNode != 0; currentNode = docIterator->nextNode())
    {ProcessNode(currentNode, prefix);}
}

void BDSGDMLPreprocessor::ProcessNode(DOMNode*        node,
				      const G4String& prefix)
{
  if (!node)
    {return;}

  std::string thisNodeName = XMLString::transcode(node->getNodeName());
  auto search = std::find(ignoreNodes.begin(), ignoreNodes.end(), thisNodeName);
  if (search != ignoreNodes.end())
    {return;} // ignore this node
  else
    {ProcessAttributes(node->getAttributes(), prefix);}
}

void BDSGDMLPreprocessor::ProcessAttributes(DOMNamedNodeMap* attributeMap,
					    const G4String&  prefix)
{
  if (!attributeMap)
    {return;}

  for(XMLSize_t i = 0; i < attributeMap->getLength(); i++)
    {
      DOMNode* attr = attributeMap->item(i);
      std::string name = XMLString::transcode(attr->getNodeValue());

      auto search = std::find(ignoreAttrs.begin(), ignoreAttrs.end(), name);
      if (search != ignoreAttrs.end())
        {continue;} // ignore this attribute

      if (XMLString::compareIString(attr->getNodeName(),
                                    XMLString::transcode("name")) == 0)
	{
	  std::string newName = prefix + "_" + name;
	  attr->setNodeValue(XMLString::transcode(newName.c_str()));
	}
      else
	{
	  std::string expression = XMLString::transcode(attr->getNodeValue());
	  // Iterate over all the names that have been defined.
	  for (auto defined_name : names)
	    {
	      // Check if whole name is found (don't match substrings).
	      // \\b = word boundary.  $& = the matched string.
	      std::regex whole_name(std::string("\\b") + defined_name + "\\b");
	      expression = std::regex_replace(expression, whole_name, prefix + "_$&");
	    }
	  attr->setNodeValue(XMLString::transcode((expression).c_str()));
	}
    }
}
#endif
