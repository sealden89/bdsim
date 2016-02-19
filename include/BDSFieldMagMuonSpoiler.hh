#ifndef BDSFIELDMAGMUONSPOILER_H
#define BDSFIELDMAGMUONSPOILER_H

#include "BDSFieldMag.hh"
#include "BDSMagnetStrength.hh"

#include "globals.hh" // geant4 types / globals
#include "G4ThreeVector.hh"

/** 
 * @brief Field of a Muon Spoiler.
 *
 */

class BDSFieldMagMuonSpoiler: public BDSFieldMag
{
public:
  BDSFieldMagMuonSpoiler(BDSMagnetStrength const* strength,
			 G4double          const  brho);

  virtual ~BDSFieldMagMuonSpoiler(){;}

  /// Access the field value.
  virtual G4ThreeVector GetFieldValue(const G4ThreeVector& position) const;

  /// Get the name of the field.
  virtual inline G4String Name() const;
  
private:
  G4double bField;
};

inline G4String BDSFieldMagMuonSpoiler::Name() const
{return "muon_spoiler";}

#endif
