/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDTempRiver.c

wil.wollheim@unh.edu

Calculate the temperature in runoff from the local grid cell.  Weight groundwater and surface water temperatures.

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfRunoffID          = MFUnset;
static int _MDInBaseFlowID            = MFUnset;
static int _MDInWTempGrdWaterID       = MFUnset;
static int _MDInWTempSurfRunoffPoolID = MFUnset;		// RJS 060512
static int _MDInTotalSurfRunoffID     = MFUnset;		// RJS 082812

// Output
static int _MDOutWTempRiverID     = MFUnset;

static void _MDWTempRiver (int itemID) {
	 float RechargeT;
	 float GrdWaterT;
//	 float SurfaceRO;		// comment out 082812
	 float TotalSurfRunoff;		// RJS 082812
	 float GrdWaterRO;
	 float TemperatureRO;

	 RechargeT          = MFVarGetFloat (_MDInWTempSurfRunoffPoolID, itemID, 0.0);			// RJS 060512
  	 GrdWaterT          = MFVarGetFloat (_MDInWTempGrdWaterID,       itemID, 0.0);
//   	 SurfaceRO          = MFVarGetFloat (_MDInSurfRunoffID,          itemID, 0.0);			// comment out 082812
	 TotalSurfRunoff    = MFVarGetFloat (_MDInTotalSurfRunoffID,     itemID, 0.0);			// RJS 082812
 	 GrdWaterRO         = MFVarGetFloat (_MDInBaseFlowID,            itemID, 0.0);
 
//      SurfaceRO  = MDMaximum(0, SurfaceRO);								// comment out 082812
        GrdWaterRO = MDMaximum(0, GrdWaterRO);
	TotalSurfRunoff = MDMaximum(0, TotalSurfRunoff);						// RJS 082812
                
//   	TemperatureRO = MDMaximum((((SurfaceRO * RechargeT) + (GrdWaterRO * GrdWaterT)) / (SurfaceRO + GrdWaterRO)),0.0);		// commented out 082812
	TemperatureRO = MDMaximum((((TotalSurfRunoff * RechargeT) + (GrdWaterRO * GrdWaterT)) / (TotalSurfRunoff + GrdWaterRO)),0.0);	// RJS 082812
   	
   	
	MFVarSetFloat(_MDOutWTempRiverID,itemID,TemperatureRO); 


}

int MDWTempRiverDef () {

	if (_MDOutWTempRiverID != MFUnset) return (_MDOutWTempRiverID);

	MFDefEntering ("River temperature");

	if (((_MDInSurfRunoffID      = MDRainSurfRunoffDef ()) == CMfailed) ||
	    ((_MDInBaseFlowID        = MDBaseFlowDef       ()) == CMfailed) ||
//	    ((_MDInWTempSurfRunoffID = MDWTempSurfRunoffDef ()) == CMfailed) ||		// commented out RJS 060512
	    ((_MDInWTempSurfRunoffPoolID = MDWTempSurfRunoffPoolDef ()) == CMfailed) ||		// RJS 060512
	    ((_MDInWTempGrdWaterID   = MDWTempGrdWaterDef   ()) == CMfailed) ||
	    ((_MDInTotalSurfRunoffID = MFVarGetID (MDVarTotalSurfRunoff, "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||	//RJS 082812
	    ((_MDOutWTempRiverID     = MFVarGetID (MDVarWTempRiver,    "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	
	    (MFModelAddFunction (_MDWTempRiver) == CMfailed)) return (CMfailed);

	MFDefLeaving ("River temperature");
	return (_MDOutWTempRiverID);
}
