/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDInfiltration.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <string.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInRainWaterSurplusID       = MFUnset;
// Output
static int _MDOutRainSurfRunoffID        = MFUnset;
static int _MDOutRainInfiltrationID      = MFUnset;
static int _MDInRainInfiltrationID       = MFUnset;

static float _MDInfiltrationFrac = 0.5;

static void _MDRainInfiltrationSimple (int itemID) {

	float surplus;
	float surfRunoff;
	float infiltration;

	surplus = MFVarGetFloat(_MDInRainWaterSurplusID, itemID, 0.0);
	surfRunoff   = surplus * (1.0 - _MDInfiltrationFrac);
	infiltration = surplus *_MDInfiltrationFrac;
	MFVarSetFloat (_MDOutRainSurfRunoffID,       itemID, surfRunoff);
	MFVarSetFloat (_MDOutRainInfiltrationID,     itemID, infiltration);
}

int MDRainInfiltrationDef () {
	const char *optStr;
	float par;

	if (_MDOutRainInfiltrationID != MFUnset) return (_MDOutRainInfiltrationID);

	if (((optStr = MFOptionGet (MDParInfiltrationFrac))  != (char *) NULL) && (sscanf (optStr,"%f",&par) == 1)) _MDInfiltrationFrac = par;
	
	MFDefEntering ("Rainfed Infiltration");
	if (((_MDInRainWaterSurplusID = MDRainWaterSurplusDef ()) == CMfailed) ||
        ((_MDOutRainSurfRunoffID   = MFVarGetID (MDVarCore_RainSurfRunoff, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        ((_MDOutRainInfiltrationID = MFVarGetID (MDVarCore_RainInfiltration, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
        (MFModelAddFunction (_MDRainInfiltrationSimple) == CMfailed)) return (CMfailed);
	MFDefLeaving  ("Rainfed Infiltration");
	return (_MDOutRainInfiltrationID);
}