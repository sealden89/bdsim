/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 25.12.2003
   Copyright (c) 2003 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSSextStepper.cc,v 1.5 2007/11/14 12:57:06 malton Exp $
// GEANT4 tag $Name:  $
//
#include "BDSSextStepper.hh"
#include "G4ThreeVector.hh"
#include "G4LineSection.hh"
#include "G4TransportationManager.hh"

extern G4double BDSLocalRadiusOfCurvature;

extern G4int event_number;


const G4int DEBUG = 0;

BDSSextStepper::BDSSextStepper(G4Mag_EqRhs *EqRhs)
  : G4MagIntegratorStepper(EqRhs,6)  // integrate over 6 variables only !!
                                       // position & velocity
{
  fPtrMagEqOfMot = EqRhs;
}


void BDSSextStepper::AdvanceHelix( const G4double  yIn[],
                                   G4ThreeVector Bfld,
				   G4double  h,
				   G4double  ySext[])
{

  const G4double *pIn = yIn+3;
  G4ThreeVector v0= G4ThreeVector( pIn[0], pIn[1], pIn[2]);  
  G4ThreeVector InitMomDir=v0.unit();

  G4ThreeVector GlobalPosition= G4ThreeVector( yIn[0], yIn[1], yIn[2]);  

  G4double InitMag=v0.mag();


  G4double kappa=  (-fPtrMagEqOfMot->FCof()*itsBDblPrime) /InitMag;
   

   if(DEBUG) G4cout<<"sextupole stepper:"<<G4endl; 
  //  G4cout << "kappa: " << kappa << G4endl;
//    G4cout << "InitMag: " << InitMag << G4endl;
//    G4cout << "g'': " <<itsBDblPrime<< G4endl;
//    G4cout << "fPtrMagEqOfMot->FCof(): " << fPtrMagEqOfMot->FCof() << G4endl << G4endl;
//    G4cout << "h=: " <<h<< G4endl;

   if(fabs(kappa)<1.e-12)
     {
       G4ThreeVector positionMove  = (h/InitMag) * v0;
       
       ySext[0]   = yIn[0] + positionMove.x(); 
       ySext[1]   = yIn[1] + positionMove.y(); 
       ySext[2]   = yIn[2] + positionMove.z(); 
				
       ySext[3] = v0.x();
       ySext[4] = v0.y();
       ySext[5] = v0.z();

       itsDist=0;
     }
   else 
     {      
       G4Navigator* SextNavigator=
	 G4TransportationManager::GetTransportationManager()->
	 GetNavigatorForTracking();
       
       G4AffineTransform LocalAffine=SextNavigator->
	 GetLocalToGlobalTransform();
       
       G4AffineTransform GlobalAffine=SextNavigator->
	 GetGlobalToLocalTransform();
       G4ThreeVector LocalR=GlobalAffine.TransformPoint(GlobalPosition); 
       G4ThreeVector LocalRp=GlobalAffine.TransformAxis(InitMomDir);
       
       
       G4double x0=LocalR.x(); 
       G4double y0=LocalR.y();

       // Evaluate field at the approximate midpoint of the step.
       x0=x0+LocalRp.x()*h/2;
       y0=y0+LocalRp.y()*h/2;
       
       G4double x02My02=(x0*x0-y0*y0);

       G4double xp=LocalRp.x();
       G4double yp=LocalRp.y();
       G4double zp=LocalRp.z();

       // local r'' (for curvature)
       G4ThreeVector LocalRpp;
       LocalRpp.setX(- zp*x02My02);
       LocalRpp.setY(2*zp*x0*y0);
       LocalRpp.setZ(xp*x02My02-2*yp*x0*y0);

       //G4cout << "LocalRpp: " <<LocalRpp<< G4endl;

       LocalRpp*=kappa/2; // 2 is actually a 2! factor.
       // determine effective curvature
       G4double R_1 = LocalRpp.mag();


       if(R_1>0.)
	 {    
	   G4double h2=h*h;
	   // chord distance (simple quadratic approx)
	   itsDist= h2*R_1/8;

	   // Save for Synchrotron Radiation calculations:
	   BDSLocalRadiusOfCurvature=1./R_1;
	   
           G4double dx=LocalRp.x()*h + LocalRpp.x()*h2 /2.; 
	   G4double dy=LocalRp.y()*h + LocalRpp.y()*h2 /2.;

           G4double dz=sqrt(h2*(1.-h2*R_1*R_1/12)-dx*dx-dy*dy);
	   // check for precision problems
           G4double ScaleFac=(dx*dx+dy*dy+dz*dz)/h2;
	   if(ScaleFac>1.0000001)
	     {
               ScaleFac=sqrt(ScaleFac);
               dx/=ScaleFac;
               dy/=ScaleFac;
               dz/=ScaleFac;
	     }

	 
           LocalR.setX(LocalR.x()+dx);
           LocalR.setY(LocalR.y()+dy);
           LocalR.setZ(LocalR.z()+dz);
	  
	   LocalRp = LocalRp+ h*LocalRpp;

	 }
       else
	 {LocalR += h*LocalRp;}
       
       GlobalPosition=LocalAffine.TransformPoint(LocalR); 
       G4ThreeVector GlobalTangent=LocalAffine.TransformAxis(LocalRp)*InitMag;
       
       ySext[0]   = GlobalPosition.x(); 
       ySext[1]   = GlobalPosition.y(); 
       ySext[2]   = GlobalPosition.z(); 
				
       ySext[3] = GlobalTangent.x();
       ySext[4] = GlobalTangent.y();
       ySext[5] = GlobalTangent.z();
     }
}

void BDSSextStepper::Stepper( const G4double yInput[],
			     const G4double dydx[],
			     const G4double hstep,
			     G4double yOut[],
			     G4double yErr[]      )
{  
  const G4int nvar = 6 ;
  
  G4int i;
 
  G4double yTemp[7], yIn[7];
  
  //  Saving yInput because yInput and yOut can be aliases for same array
  
  for(i=0;i<nvar;i++) yIn[i]=yInput[i];
  
  G4double h = hstep * 0.5; 
  
  // Do two half steps
  AdvanceHelix(yIn,   0,  h, yTemp);
  AdvanceHelix(yTemp, 0, h, yOut); 
  
  // Do a full Step
  h = hstep ;
  AdvanceHelix(yIn, 0, h, yTemp); 
  
  for(i=0;i<nvar;i++) yErr[i] = yOut[i] - yTemp[i] ;
  
  return ;
}

G4double BDSSextStepper::DistChord()   const 
{
  return itsDist;
}

BDSSextStepper::~BDSSextStepper()
{}
