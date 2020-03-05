include protonOptions.gmad 

!biasDef1: xsecBias, particle="proton", proc="protonInelastic CoulombScat hBrems", xsecfact={1,0,0}, flag={2,1,1};
!biasDef2: xsecBias, particle="proton", proc="eBrem eIoni msc", xsecfact={0,5,0}, flag={1,1,2};
!biasDef3: xsecBias, particle="ion 131 54", proc="all", xsecfact=30, flag=2;

beam,  particle="proton",
       energy= __ENERGY__*GeV,
       distrType="reference",
       X0=0.0*mm,
       Y0=0.0*mm,
       Xp0=0.0,
       Yp0=0.*mrad;

!myCol1  : rcol, l=__COLLENGTH__*m, xsize=0*mm, ysize=0*mm, material="__MATERIAL__", biasMaterial="biasDef3";
myCol1  : rcol, l=__COLLENGTH__*m, xsize=0*mm, ysize=0*mm, material="__MATERIAL__";

beamLine: line = (myCol1); !defines the beam line
use, period=beamLine; !Chooses the beam line to use

sample, all; 

option, ngenerate=__NGENERATE__;
