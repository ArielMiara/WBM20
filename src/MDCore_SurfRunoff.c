/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDSurfRunoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainSurfRunoffID = MFUnset;
static int _MDInSmallResUptakeID = MFUnset;
static int _MDInWetlandUptakeID  = MFUnset;

// Output
static int _MDOutSurfRunoffID    = MFUnset;

static void _MDSurfRunoff (int itemID) {	
// Input
	float surfRunoff; // Surface runoff [mm/dt]
	// TODO: The small reservoir uptake might exceed rain surface runoff
	surfRunoff = MFVarGetFloat (_MDInRainSurfRunoffID,     itemID, 0.0)
	           - (_MDInSmallResUptakeID != MFUnset ? MFVarGetFloat (_MDInSmallResUptakeID, itemID, 0.0) : 0.0);
	MFVarSetFloat (_MDOutSurfRunoffID,  itemID, surfRunoff);
}

int MDCore_SurfRunoffDef () {
	int ret;
	if (_MDOutSurfRunoffID != MFUnset) return (_MDOutSurfRunoffID);

	MFDefEntering ("Surface runoff");
	 
	if (((ret = MDReservoir_FarmPondReleaseDef()) != MFUnset) &&
        ((ret == CMfailed) ||
	     ((_MDInSmallResUptakeID = MFVarGetID (MDVarReservoir_FarmPontUptake, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed)))
	     return (CMfailed);
	
	if (((_MDInRainSurfRunoffID  = MDCore_RainSurfRunoffDef()) == CMfailed) ||
        ((_MDOutSurfRunoffID     = MFVarGetID (MDVarCore_SurfRunoff, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDSurfRunoff) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Surface runoff");
	return (_MDOutSurfRunoffID);
}
