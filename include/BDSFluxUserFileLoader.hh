//
// Created by Siobhan Alden on 21/01/2025.
//

#ifndef BDSFLUXUSERFILELOADER_HH
#define BDSFLUXUSERFILELOADER_HH
/*
 * @brief A loader for importance values used in importance sampling.
 *
 * @author S Alden
 */

#include "G4String.hh"
#include "G4Types.hh"
#include "BDSOctree.hh"

#include <map>

template <class T>
class BDSFluxUserFileLoader
{
public:
    BDSFluxUserFileLoader();
    ~BDSFluxUserFileLoader();

    BDSOctree* Load(const G4String& fileName, G4ThreeVector& lowerBoundsIn, G4ThreeVector& upperBoundsIn);

};

#endif //BDSFLUXUSERFILELOADER_HH
