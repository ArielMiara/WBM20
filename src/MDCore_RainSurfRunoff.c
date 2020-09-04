/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDRainSurfRunoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Output
static int _MDOutRainInfiltrationID = MFUnset;
static int _MDOutRainSurfRunoffID   = MFUnset;

int MDCore_RainSurfRunoffDef () {

	if (_MDOutRainSurfRunoffID != MFUnset) return (_MDOutRainSurfRunoffID);
	
	if (((_MDOutRainInfiltrationID = MDCore_RainInfiltrationDef()) == CMfailed) ||
        ((_MDOutRainSurfRunoffID   = MFVarGetID (MDVarCore_RainSurfRunoff, "mm", MFInput, MFFlux, MFBoundary)) == CMfailed))
		return (CMfailed);
	return (_MDOutRainSurfRunoffID);
}
