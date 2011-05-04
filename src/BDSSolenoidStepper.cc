//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.4
//  
//
//
//   Stepper for Solenoid magnetic field
//
//
//   History
//
//     21 Oct 2007 by Marchiori,  v.0.4
//
//

#include "BDSGlobalConstants.hh" 
#include "BDSSolenoidStepper.hh"
#include "G4ThreeVector.hh"
#include "G4TransportationManager.hh"

using std::max;
extern G4double BDSLocalRadiusOfCurvature;
extern G4int event_number;

BDSSolenoidStepper::BDSSolenoidStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6)  // integrate over 6 variables only !!
                                     // position & velocity
{
  fPtrMagEqOfMot = EqRhs;
}

void BDSSolenoidStepper::AdvanceHelix( const G4double  yIn[],
				       G4ThreeVector,
				       G4double  h,
				       G4double  yOut[])
{
  //
  // Compute charge of particle (FCof = particleCharge*eplus*c_light)
  //
  G4double charge = (fPtrMagEqOfMot->FCof())/c_light;

  
  //
  // Get B field
  //
  G4double Bz;
  if(BDSGlobals->GetSynchRescale())
    {
      G4double B[3];
      fPtrMagEqOfMot->GetFieldValue(yIn, B);
      Bz = B[2];
    }
  else
    Bz = itsBField;


#ifdef DEBUG 
  G4cout << "BDSSolenoidStepper: step= " << h/m << " m" << G4endl;
  G4cout << "BDSSolenoidStepper: initial point in global coordinates:" << G4endl
         << " x= " << yIn[0]/m << "m" << G4endl
         << " y= " << yIn[1]/m << "m" << G4endl
         << " z= " << yIn[2]/m << "m" << G4endl
         << " px= " << yIn[3]/GeV << "GeV/c" << G4endl
         << " py= " << yIn[4]/GeV << "GeV/c" << G4endl
         << " pz= " << yIn[5]/GeV << "GeV/c" << G4endl
         << " q= " << charge/eplus << "e" << G4endl
         << " B= " << Bz/tesla << "T" << G4endl
         << G4endl; 
#endif


  //
  // compute initial R, P, R' in global coordinates
  //
  const G4double *pIn = yIn+3;
  G4ThreeVector GlobalR = G4ThreeVector( yIn[0], yIn[1], yIn[2]);  
  G4ThreeVector GlobalP = G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  G4ThreeVector GlobalRp = GlobalP.unit();
  G4double InitPMag = GlobalP.mag();


  //
  // transform global to local coordinates
  //
  G4Navigator* HelixNavigator=
    G4TransportationManager::GetTransportationManager()->
    GetNavigatorForTracking();
  
  G4AffineTransform GlobalAffine = HelixNavigator->GetGlobalToLocalTransform();
  G4ThreeVector LocalR = GlobalAffine.TransformPoint(GlobalR); 
  G4ThreeVector LocalRp = GlobalAffine.TransformAxis(GlobalRp);

#ifdef DEBUG
  G4cout << "BDSSolenoidStepper: initial point in local coordinates:" << G4endl
         << " x= " << LocalR[0]/m << "m" << G4endl
         << " y= " << LocalR[1]/m << "m" << G4endl
         << " z= " << LocalR[2]/m << "m" << G4endl
         << " x'= " << LocalRp[0] << G4endl
         << " y'= " << LocalRp[1] << G4endl
         << " z'= " << LocalRp[2] << G4endl
         << G4endl; 
#endif


  //
  // compute radius of helix
  //
  G4double R;
  if (Bz!=0)
    R = -(InitPMag*LocalRp.perp()/GeV)/(0.299792458 * Bz/tesla) * m;
  else
    R=DBL_MAX;

  // include the sign of the charge of the particles
  // FCof = particleCharge*eplus*c_light
  if( charge<0 ) R*=-1.;
  else if ( charge==0 ) R=DBL_MAX;

  // Save for Synchrotron Radiation calculations:
  BDSLocalRadiusOfCurvature=R;

  // check that the approximations are valid, else do a linear step
  G4ThreeVector itsFinalR, itsFinalRp;
  if(fabs(R)<DBL_MAX)
    {

      //
      // compute pitch of helix
      //
      G4double pitch;
      pitch = fabs(2*pi*R*LocalRp[2]/LocalRp.perp());

      //
      // compute center of helix
      //
      G4ThreeVector Bhat(0.,0.,Bz/fabs(Bz));
      G4ThreeVector vhat=LocalRp;
      G4ThreeVector Rhat=(charge*vhat.cross(Bhat)).unit();
      G4ThreeVector center=LocalR+fabs(R)*Rhat; //occhio che anche R ha il segno!
      //
      // compute step length in z and theta (h is the helix arc length)
      //
      G4double dz = h / sqrt(1. + pow(2.*pi*R/pitch,2));
      G4double dtheta = 2*pi*dz/pitch*R/fabs(R);
      
#ifdef DEBUG 
      G4cout << "Parameters of helix: " << G4endl
             << " R= " << R/m << " m" << G4endl
             << " pitch= " << pitch/m << " m" <<G4endl
             << " center= " << center/m << " m"<<G4endl
             << " step length= " << h/m << " m"<<G4endl
             << " step dz= " << dz/m << " m"<<G4endl
             << " step dtheta= " << dtheta/radian << " rad"<<G4endl;
#endif


      //
      // advance the orbit (in local coordinates)
      // h is the helix length
      //
      G4ThreeVector itsInitialR = LocalR;
      G4ThreeVector itsInitialRp = LocalRp;
      
      //G4double CosT = cos(dtheta);
      //G4double SinT = sin(dtheta);  
      //G4RotationMatrix r;
      //r.rotateZ(dtheta);
      
      itsFinalRp = itsInitialRp.rotateZ(dtheta);
      itsFinalR = 
	center + (itsInitialR-center).rotateZ(dtheta) + G4ThreeVector(0,0,dz);


      //
      // compute max distance between chord from yIn to yOut and helix
      //
      G4double Ang = fabs(dtheta);
      if(Ang<=pi){
	itsDist = fabs(R)*(1 - cos(0.5*Ang));
      } else
	if(Ang<twopi){
	  itsDist = fabs(R)*(1 + cos(pi-0.5*Ang));
	} else
	  itsDist = 2*fabs(R);
    }      
  else
    {
      itsFinalR = LocalR + h * LocalRp;
      itsFinalRp = LocalRp;
      itsDist=0.;
    }
  
#ifdef DEBUG 
  G4cout << "BDSSolenoidStepper: final point in local coordinates:" << G4endl
         << " x= " << itsFinalR[0]/m << "m" << G4endl
         << " y= " << itsFinalR[1]/m << "m" << G4endl
         << " z= " << itsFinalR[2]/m << "m" << G4endl
         << " x'= " << itsFinalRp[0] << G4endl
         << " y'= " << itsFinalRp[1] << G4endl
         << " z'= " << itsFinalRp[2] << G4endl
         << G4endl; 
#endif

  //
  // transform local to global coordinates
  //
  G4AffineTransform LocalAffine = HelixNavigator->GetLocalToGlobalTransform();
  GlobalR = LocalAffine.TransformPoint(itsFinalR); 
  GlobalRp = LocalAffine.TransformAxis(itsFinalRp);
  GlobalP = InitPMag*GlobalRp;

  yOut[0] = GlobalR.x(); 
  yOut[1] = GlobalR.y(); 
  yOut[2] = GlobalR.z(); 
  
  yOut[3] = GlobalP.x();
  yOut[4] = GlobalP.y();
  yOut[5] = GlobalP.z();

#ifdef DEBUG 
  G4cout << "BDSSolenoidStepper: final point in global coordinates:" << G4endl
         << " x= " << yOut[0]/m << "m" << G4endl
         << " y= " << yOut[1]/m << "m" << G4endl
         << " z= " << yOut[2]/m << "m" << G4endl
         << " px= " << yOut[3]/GeV << "GeV/c" << G4endl
         << " py= " << yOut[4]/GeV << "GeV/c" << G4endl
         << " pz= " << yOut[5]/GeV << "GeV/c" << G4endl
         << G4endl; 
#endif
}    


void BDSSolenoidStepper::Stepper( const G4double yInput[],
				  const G4double[],
				  const G4double hstep,
				  G4double yOut[],
				  G4double yErr[]      )
{  
  const G4int nvar = 6 ;

  for(G4int i=0;i<nvar;i++) yErr[i]=0;
  AdvanceHelix(yInput,(G4ThreeVector)0,hstep,yOut);
  return ;
}

G4double BDSSolenoidStepper::DistChord()   const 
{

  return itsDist;
  // This is a class method that gives distance of Mid 
  //  from the Chord between the Initial and Final points.
}

BDSSolenoidStepper::~BDSSolenoidStepper()
{}
