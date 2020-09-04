/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDischLevel3.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

enum { MDaccumulate, MDmuskingum, MDcascade };

static int _MDDischLevel3ID = MFUnset;

int MDRouting_DischLevel3Def() {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptConfig_Routing;
	const char *options []    = { "accumulate", "muskingum", "cascade", (char *) NULL };

	if (_MDDischLevel3ID != MFUnset) return (_MDDischLevel3ID);

	MFDefEntering ("Discharge Level 3");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	switch (optID) {
		case MDaccumulate: _MDDischLevel3ID = MDRouting_DischLevel3AccumulateDef(); break;
		case MDmuskingum:  _MDDischLevel3ID = MDRouting_DischLevel3MuskingumDef(); break;
		case MDcascade:    _MDDischLevel3ID = MDRouting_DischLevel3CascadeDef(); break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	if (_MDDischLevel3ID == CMfailed) return (CMfailed);
	MFDefLeaving ("Discharge Level 3");
	return (_MDDischLevel3ID);
}
