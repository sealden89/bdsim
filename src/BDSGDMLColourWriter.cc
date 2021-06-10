#include "BDSGDMLColourWriter.hh.hh"

#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

G03ColorWriter::G03ColorWriter()
  : G4GDMLWriteStructure()
{
}

G03ColorWriter::~G03ColorWriter()
{
}

void G03ColorWriter::AddExtension(xercesc::DOMElement* volumeElement,
                               const G4LogicalVolume* const vol)
{
   const G4VisAttributes* vis = vol->GetVisAttributes();
   if (vis)  { ColorWrite(volumeElement, vis); }
}

void G03ColorWriter::ExtensionWrite(xercesc::DOMElement* element)
{
   G4cout << "G4GDML: Writing GDML extension..." << G4endl;

   // Mandatory calls the -first- time an extension is created
   //
   extElement = NewElement("extension");
   element->appendChild(extElement);
}

void G03ColorWriter::ColorWrite(xercesc::DOMElement* volumeElement,
                             const G4VisAttributes* const att)
{
   G4bool book = BookAttribute(att);
   G4Color color = att->GetColor();

   const G4String& name = GenerateName("test_color", att);
   G4double r=color.GetRed(), g=color.GetGreen(),
            b=color.GetBlue(), a=color.GetAlpha();

   if (book)
   {
     xercesc::DOMElement* colElement = NewElement("color");
     colElement->setAttributeNode(NewAttribute("name",name));
     colElement->setAttributeNode(NewAttribute("R",r));
     colElement->setAttributeNode(NewAttribute("G",g));
     colElement->setAttributeNode(NewAttribute("B",b));
     colElement->setAttributeNode(NewAttribute("A",a));
     extElement->appendChild(colElement);
   }

   xercesc::DOMElement* colorrefElement = NewElement("colorref");
   colorrefElement->setAttributeNode(NewAttribute("ref",name));
   volumeElement->appendChild(colorrefElement);

   G4cout << "Written color attribute (R,G,B,A) is: "
          << r << ", " << g << ", " << b << ", " << a << " !" << G4endl;
}

G4bool G03ColorWriter::BookAttribute(const G4VisAttributes* const ref)
{
  G4bool booking = true;
  std::vector<const G4VisAttributes*>::const_iterator pos =
     std::find(fAttribs.begin(), fAttribs.end(), ref);

  if (pos != fAttribs.end())  { booking = false; }
  else                       { fAttribs.push_back(ref); }

  return booking;
}
