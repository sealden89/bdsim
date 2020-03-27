! standard options for runing bdsim

option, beampipeRadius = 10 * cm,
        outerDiameter = 1.9 * m,
        beampipeThickness = 0.5 * mm,
	defaultRangeCut=0.1*m,
	prodCutPhotons=0.1*m,
	prodCutElectrons=0.1*m,
	prodCutPositrons=0.1*m,
	elossHistoBinWidth = 0.01;

option,physicsList="__PHYSICSLISTS__";
option,storeTrajectories=1;
option,storeTrajectoryEnergyThreshold=-1;
!option,trajNoTransportation=1;
!option,stopTracks=0;
option,stopSecondaries=1;