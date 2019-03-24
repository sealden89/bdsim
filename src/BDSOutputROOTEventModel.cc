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
#include "BDSOutputROOTEventModel.hh"

#ifndef __ROOTBUILD__
#include "BDSAcceleratorModel.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetStrength.hh"
#include "BDSSamplerRegistry.hh"

#include "G4Types.hh"
#include "G4String.hh"

#include <map>
#include <string>
#include <vector>
#endif

ClassImp(BDSOutputROOTEventModel)

BDSOutputROOTEventModel::BDSOutputROOTEventModel():
  n(0),
  storeCollimatorInfo(false)
{
  Flush();
}

BDSOutputROOTEventModel::~BDSOutputROOTEventModel()
{;}

int BDSOutputROOTEventModel::findNearestElement(TVector3 vPoint)
{
  // TODO : Better search using lower
  double dMin = 1e50;
  int iMin = -1;
  for(int i=0; i < (int)midRefPos.size(); i++)
    {
      const TVector3& vRef = midRefPos[i];
      double d = (vRef-vPoint).Mag();
      if(d < dMin)
	{
	  iMin = i;
	  dMin = d;
	}
    } 
  return iMin;
}

void BDSOutputROOTEventModel::Flush()
{
  n = 0;
  samplerNamesUnique.clear();
  componentName.clear();
  placementName.clear();
  componentType.clear();
  length.clear();
  staPos.clear();
  midPos.clear();
  endPos.clear();
  staRot.clear();
  midRot.clear();
  endRot.clear();
  staRefPos.clear();
  midRefPos.clear();
  endRefPos.clear();
  staRefRot.clear();
  midRefRot.clear();
  endRefRot.clear();
  tilt.clear();
  offsetX.clear();
  offsetY.clear();
  staS.clear();
  midS.clear();
  endS.clear();
  beamPipeType.clear();
  beamPipeAper1.clear();
  beamPipeAper2.clear();
  beamPipeAper3.clear();
  beamPipeAper4.clear();
  material.clear();
  k1.clear();
  k2.clear();
  k3.clear();
  k4.clear();
  k5.clear();
  k6.clear();
  k7.clear();
  k8.clear();
  k9.clear();
  k10.clear();
  k11.clear();
  k12.clear();
  k1s.clear();
  k2s.clear();
  k3s.clear();
  k4s.clear();
  k5s.clear();
  k6s.clear();
  k7s.clear();
  k8s.clear();
  k9s.clear();
  k10s.clear();
  k11s.clear();
  k12s.clear();
  ks.clear();
  hkick.clear();
  vkick.clear();
  bField.clear();
  eField.clear();
  e1.clear();
  e2.clear();
  fint.clear();
  fintx.clear();
  fintk2.clear();
  fintxk2.clear();

  storeCollimatorInfo = false;
  collimatorIndices.clear();
  collimatorIndicesByName.clear();
  nCollimators = 0;
  collimatorInfo.clear();
  collimatorBranchNamesUnique.clear();
}

#ifndef __ROOTBUILD__
BDSOutputROOTEventModel::BDSOutputROOTEventModel(G4bool storeCollimatorInfoIn):
  n(0),
  storeCollimatorInfo(storeCollimatorInfoIn)
{;}

void BDSOutputROOTEventModel::Fill(const std::vector<G4int>& collimatorIndicesIn,
				   const std::map<G4String, G4int>& collimatorIndicesByNameIn,
				   const std::vector<BDSOutputROOTEventCollimatorInfo>& collimatorInfoIn,
				   const std::vector<G4String>& collimatorBranchNamesIn)
{  
  for (const auto name : BDSSamplerRegistry::Instance()->GetUniqueNames())
    {samplerNamesUnique.push_back(std::string(name) + ".");}

  for (const auto& name : collimatorBranchNamesIn)
    {collimatorBranchNamesUnique.push_back(std::string(name) + ".");}
  
  // get accelerator model
  const BDSBeamline* beamline = BDSAcceleratorModel::Instance()->BeamlineMain();
  if (!beamline)
    {return;} // in case of generatePrimariesOnly there is no model - return

  if (storeCollimatorInfo)
    {
      for (const auto value : collimatorIndicesIn)
	{collimatorIndices.push_back((int)value);}
      
      nCollimators = (int)collimatorIndices.size();
      
      for (const auto& kv : collimatorIndicesByNameIn)
	{collimatorIndicesByName[(std::string)kv.first] = (int)kv.second;}

      collimatorInfo = collimatorInfoIn;
    }

  double angle;
  CLHEP::Hep3Vector axis;

  n = (int)beamline->size();
  
  // iterate over flat beamline
  for (auto i = beamline->begin(); i != beamline->end(); ++i)
  {
    // Name
    componentName.push_back((*i)->GetName());
    placementName.push_back((*i)->GetPlacementName());
    componentType.push_back((*i)->GetType());

    // Length
    length.push_back((float &&) (*i)->GetAcceleratorComponent()->GetArcLength() / CLHEP::m);

    // Positions
    G4ThreeVector p;
    p = (*i)->GetPositionStart();
    staPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetPositionMiddle();
    midPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetPositionEnd();
    endPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));

    // Rotations
    G4RotationMatrix *gr;
    gr = (*i)->GetRotationStart();
    TRotation rr = TRotation();
    rr.SetToIdentity();
    gr->getAngleAxis(angle,axis);
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    staRot.push_back(rr);

    gr = (*i)->GetRotationMiddle();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    //G4cout << (*i)->GetName() << " " << angle << " " << axis.x() << " " << axis.y() << " " << axis.z() << G4endl;
    midRot.push_back(rr);

    gr = (*i)->GetRotationEnd();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    endRot.push_back(rr);

    // Reference orbit positions
    p = (*i)->GetReferencePositionStart();
    staRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetReferencePositionMiddle();
    midRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));
    p = (*i)->GetReferencePositionEnd();
    endRefPos.push_back(TVector3(p.getX() / CLHEP::m, p.getY() / CLHEP::m, p.getZ() / CLHEP::m));

    // Reference orbit rotations
    gr = (*i)->GetReferenceRotationStart();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    staRefRot.push_back(rr);

    gr = (*i)->GetReferenceRotationMiddle();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    midRefRot.push_back(rr);

    gr = (*i)->GetReferenceRotationEnd();
    gr->getAngleAxis(angle,axis);
    rr.SetToIdentity();
    rr.Rotate(angle,TVector3(axis.x(),axis.y(),axis.z()));
    endRefRot.push_back(rr);

    // tilt and offset
    BDSTiltOffset* to = (*i)->GetTiltOffset();
    if (to)
      {
	tilt.push_back((float)to->GetTilt() / CLHEP::rad);
	offsetX.push_back((float)to->GetXOffset() / CLHEP::m);
	offsetY.push_back((float)to->GetYOffset() / CLHEP::m);
      }
    else
      {
	tilt.push_back(0);
	offsetX.push_back(0);
	offsetY.push_back(0);
      }

    // S positions
    staS.push_back((float &&) (*i)->GetSPositionStart()  / CLHEP::m);
    midS.push_back((float &&) (*i)->GetSPositionMiddle() / CLHEP::m);
    endS.push_back((float &&) (*i)->GetSPositionEnd()    / CLHEP::m);

    // beam pipe
    BDSBeamPipeInfo *beampipeinfo = (*i)->GetBeamPipeInfo();
    beamPipeType.push_back(beampipeinfo ?
			   beampipeinfo->beamPipeType.ToString() : "");
    beamPipeAper1.push_back(beampipeinfo ?
			    beampipeinfo->aper1 / CLHEP::m : 0);
    beamPipeAper2.push_back(beampipeinfo ?
			    beampipeinfo->aper2 / CLHEP::m : 0);
    beamPipeAper3.push_back(beampipeinfo ?
			    beampipeinfo->aper3 / CLHEP::m : 0);
    beamPipeAper4.push_back(beampipeinfo ?
			    beampipeinfo->aper4 / CLHEP::m : 0);

    // associated material if any
    const auto accComp = (*i)->GetAcceleratorComponent();
    material.push_back(accComp->Material());

    // helper shortcuts to all the memeber vectors
    std::vector<std::vector<float>*> localNorm = {
      &k1,&k2,&k3,&k4,&k5,&k6,&k7,&k8,&k9,&k10,&k11,&k12};
    std::vector<std::vector<float>*> localSkew = {
      &k1s,&k2s,&k3s,&k4s,&k5s,&k6s,&k7s,&k8s,&k9s,&k10s,&k11s,&k12s};

    // helper lambda to avoid duplication
    auto fillzero = [&]
      {
	for (int j = 0; j < (int)localNorm.size(); j++)
	  {localNorm[j]->push_back(0);}
	for (int j = 0; j < (int)localSkew.size(); j++)
	  {localSkew[j]->push_back(0);}
	ks.push_back(0);
	hkick.push_back(0);
	vkick.push_back(0);
	bField.push_back(0);
	eField.push_back(0);
	e1.push_back(0);
	e2.push_back(0);
	hgap.push_back(0);
	fint.push_back(0);
	fintx.push_back(0);
	fintk2.push_back(0);
	fintxk2.push_back(0);
      };
        
    // fill magnet strength data
    if (BDSMagnet* mag = dynamic_cast<BDSMagnet*>(accComp))
      {
	const BDSMagnetStrength* ms = mag->MagnetStrength();
	if (!ms)
	  {
	    fillzero();
	    continue;
	  }
	// assume localNorm and normComponents are same size
	std::vector<G4double> normComponents = ms->NormalComponents();
	for (int j = 0; j < (int)localNorm.size(); j++)
	  {localNorm[j]->push_back((float)normComponents[j]);}
	std::vector<G4double> skewComponents = ms->SkewComponents();
	for (int j = 0; j < (int)localSkew.size(); j++)
	  {localSkew[j]->push_back((float)skewComponents[j]);}

	ks.push_back((float)(*ms)["ks"]/BDSMagnetStrength::Unit("ks"));
	hkick.push_back((float)(*ms)["hkick"]/BDSMagnetStrength::Unit("hkick"));
	vkick.push_back((float)(*ms)["vkick"]/BDSMagnetStrength::Unit("vkick"));
	bField.push_back((float)(*ms)["field"]/BDSMagnetStrength::Unit("field"));
	eField.push_back((float)(*ms)["efield"]/BDSMagnetStrength::Unit("efield"));
	
	// these are mangled in BDSMagnetStrength so can't write them out just now
	e1.push_back(0); // / CLHEP::rad
	e2.push_back(0); // / CLHEP::rad
	hgap.push_back(0); // / CLHEP::m
	fint.push_back(0);
	fintx.push_back(0);
	fintk2.push_back(0);
	fintxk2.push_back(0);
      }
    else
      {// not a magnet
	fillzero();
      }    
  }
}
#endif
