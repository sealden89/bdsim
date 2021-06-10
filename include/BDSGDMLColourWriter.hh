#ifndef BDSGDMLCOLOURWRITER_H
#define BDSGDMLCOLOURWRITER_H 1

#ifdef USE_GDML
#include "G4GDMLWriteStructure.hh"

#include <vector>

class G4LogicalVolume;
class G4VisAttributes;

namespace xercesc
{
  class DOMElement;
}

class BDSGDMLColourWriter: public G4GDMLWriteStructure
{
public:  
  BDSGDMLColourWriter();
  virtual ~BDSGDMLColourWriter();
  
  void AddExtension(xercesc::DOMElement* volumeElement,
		    const G4LogicalVolume* const vol);
  void ExtensionWrite(xercesc::DOMElement* element);
  void ColorWrite(xercesc::DOMElement* volumeElement,
		  const G4VisAttributes* const att);
  
  G4bool BookAttribute(const G4VisAttributes* const att);
  
private:
   std::vector<const G4VisAttributes*> fAttribs;
};
#endif

#endif
