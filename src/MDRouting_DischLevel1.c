/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischLevel1.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInRouting_DischLevel2ID   = MFUnset;
static int _MDInRouting_DischReleasedID = MFUnset;

// Output
static int _MDOutRouting_DischLevel1ID = MFUnset;

static void _MDRouting_DischLevel1 (int itemID) {
	float discharge;

	if ((_MDInRouting_DischReleasedID != MFUnset) && (!MFVarTestMissingVal (_MDInRouting_DischReleasedID, itemID)))
		 discharge = MFVarGetFloat (_MDInRouting_DischReleasedID, itemID, 0.0);
	else discharge = MFVarGetFloat (_MDInRouting_DischLevel2ID,   itemID, 0.0);

	MFVarSetFloat (_MDOutRouting_DischLevel1ID, itemID, discharge);
}

int MDRouting_DischLevel1Def () {

   const char *optStr;
   const char *options [] = { MDNoneStr, (char *) NULL };

   
	if (_MDOutRouting_DischLevel1ID != MFUnset) return (_MDOutRouting_DischLevel1ID);

	MFDefEntering ("Discharge Level 1");
	if ((_MDInRouting_DischLevel2ID = MDRouting_DischLevel2Def()) == CMfailed) return (CMfailed);

	if (((optStr = MFOptionGet (MDOptConfig_Reservoirs)) != (char *) NULL) && (CMoptLookup (options, optStr, true) == CMfailed)) {
		if ((_MDInRouting_DischReleasedID = MDReservoir_OperationDef()) == CMfailed) return (CMfailed);
	}
	if (((_MDOutRouting_DischLevel1ID = MFVarGetID ("__DischLevel1", "m3/s",  MFOutput,  MFState, MFBoundary)) == CMfailed) ||
	    (MFModelAddFunction (_MDRouting_DischLevel1) == CMfailed)) return (CMfailed);
	MFDefLeaving ("Discharge Level 1");
	return (_MDOutRouting_DischLevel1ID);
}
