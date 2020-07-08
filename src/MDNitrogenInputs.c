/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDNitrogenInputs.c  - Nitrogen Loading

rob.stewart@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>
#include <math.h>

// Input

static int _MDInLandUseID			= MFUnset;
static int _MDInRunoffID			= MFUnset;
static int _MDInRunoffVolID			= MFUnset;
static int _MDInRiverOrderID		= MFUnset;		//RJS 090508
static int _MDInDINLoadConcID		= MFUnset;		//RJS 102810
static int _MDInRunoffPoolMassRelID = MFUnset;		//RJS 050511
static int _MDInGrdWatMassRelID		= MFUnset;		//RJS 050511
static int _MDInLawnFractionID      = MFUnset;		//RJS 051111
static int _MDInLoadAdjustID		= MFUnset;		//RJS 112211
static int _MDInTotalPointID		= MFUnset;
static int _MDInLandUseSubID		= MFUnset;
static int _MDInLandUseAgID			= MFUnset;

// Output

static int _MDOutLocalLoad_DINID	   = MFUnset;		// RJS 090308
static int _MDOutLocalLoadnew_DINID    = MFUnset;		// RJS 061511	just to CHECK whether the new and old loading are the same
static int _MDOutDINLoadConcID		   = MFUnset;		// KYLE
static int _MDOutLocalLoad_Sub_DINID	   = MFUnset;
static int _MDOutLocalLoad_Ag_DINID		   = MFUnset;
static int _MDOutDINSubLoadConcID		   = MFUnset;
static int _MDOutDINAgLoadConcID		   = MFUnset;

static void _MDNitrogenInputsInput (int itemID) {

	//Input
	float runoff;		//mm/day
	float runoffVol; 	//m3/sec
	float LocalConc_DIN;
	float riverOrder;

	//Output
	float LocalLoad_DIN;

	runoff             = MFVarGetFloat (_MDInRunoffID,         		  itemID, 0.0); // mm / d
	runoffVol          = MFVarGetFloat (_MDInRunoffVolID,             itemID, 0.0); // m3/sec
	LocalConc_DIN	   = MFVarGetFloat (_MDInDINLoadConcID,           itemID, 0.0); // mg/L
	riverOrder		   = MFVarGetFloat (_MDInRiverOrderID,            itemID, 0.0);

	LocalLoad_DIN      = runoffVol * 86400 * LocalConc_DIN / 1000; // kg/day

	MFVarSetFloat (_MDOutLocalLoad_DINID,        itemID, LocalLoad_DIN);	  // RJS 090308

}

static void _MDNitrogenInputsPartitioned (int itemID) {

	//Input
	float runoffPoolMassRel = 0.0;	//kg/day	RJS 050511
	float grdWatMassRel		= 0.0;	//kg/day	RJS 050511
	float runoffVol			= 0.0;  //m3/sec	RJS 050511	need this to call runoffVol
	float temp				= 0.0;	//kg/day	RJS 061611

	//Output
	float LocalLoad_DIN;

	runoffVol		   = MFVarGetFloat (_MDInRunoffVolID,         itemID, 0.0);	// m3/s  RJS 050511
	runoffPoolMassRel  = MFVarGetFloat (_MDInRunoffPoolMassRelID, itemID, 0.0); // kg/d  RJS 050511
	grdWatMassRel      = MFVarGetFloat (_MDInGrdWatMassRelID,     itemID, 0.0); // kg/d	 RJS 050511

	LocalLoad_DIN      = runoffPoolMassRel + grdWatMassRel; // kg/day	RJS 050511
	temp               = LocalLoad_DIN;						// kg/day	RJS 061611

	MFVarSetFloat (_MDOutLocalLoad_DINID,    itemID, LocalLoad_DIN);	 // RJS 050511
	MFVarSetFloat (_MDOutLocalLoadnew_DINID, itemID, temp);				 // RJS 061611
}



static void _MDNitrogenInputsCalc (int itemID) {

	// Input

	float luSub = 0.0;
	float luAg	= 0.0;
	float runoff;			// mm/day
	float runoffVol;		// m3/sec

	float scale;
	float asym;
	float xMid;

	float LocalConc_DIN;
	float LocalLoad_DIN;

	float LocalConc_Sub_DIN;		//KAW 2013 05 08 Suburban Loading Concentration
	float LocalLoad_Sub_DIN;		//KAW 2013 05 08 Suburban Loading

	float LocalConc_Ag_DIN;			//KAW 2013 05 08 Agricultural Loading Concentration
	float LocalLoad_Ag_DIN;			//KAW 2013 05 08 Agricultural Loading

	float riverOrder;
	float loadAdjust = 0.0;			// RJS 112211
	float Total_point = 0.0;

// test

	if (_MDInRiverOrderID != MFUnset) riverOrder = MFVarGetFloat (_MDInRiverOrderID, itemID, 0.0);	// RJS 090508

//	if (_MDInLandUseID != MFUnset)	luSub = MFVarGetFloat (_MDInLandUseID, itemID, 0.0);
//	if (_MDInTotalPointID != MFUnset) Total_point = MFVarGetFloat (_MDInTotalPointID, itemID, 0.0);
	if (_MDInLandUseSubID != MFUnset)	luSub = MFVarGetFloat (_MDInLandUseSubID, itemID, 0.0);		//KAW 2013 05 08 Suburban Land Use
	if (_MDInLandUseAgID  != MFUnset)	luAg  = MFVarGetFloat (_MDInLandUseAgID, itemID, 0.0);		//KAW 2013 05 08 Agricultural Land Use

	luSub = luSub + luAg;
//	luSub = 2 * (luSub + luAg) > 100.0 ? 99.9 : 2 * (luSub + luAg);


	runoff             = MFVarGetFloat (_MDInRunoffID,         		  itemID, 0.0); 		// mm / d
	runoffVol          = MFVarGetFloat (_MDInRunoffVolID,             itemID, 0.0); 		// m3/sec
	loadAdjust		   = MFVarGetFloat (_MDInLoadAdjustID,            itemID, 1.0);			// RJS 112211, adjusts loads, keep at 1 if nodata

	scale          = 12.2;
	asym           = 1.4;

	if (runoff < 0.00001) {
		LocalConc_DIN = 0.0;
//		LocalConc_Sub_DIN = 0.0;		//KAW 2013 05 08
//		LocalConc_Ag_DIN = 0.0;			//KAW 2013 05 08
	}

	else {
		xMid           = 51.388 + 19.459 * log(runoff); //RJS 07-29-08	MMM changed from log10 to log 2013-03-13
		LocalConc_DIN  = asym / (1 + pow(2.718281828, (xMid - luSub) / scale)); // mg/L
//		LocalConc_Sub_DIN  = asym / (1 + pow(2.718281828, (xMid - luSub) / scale)); 		// mg/L //KAW 2013 05 08
//		LocalConc_Ag_DIN  = (asym * 3.5)/ (1 + pow(2.718281828, (xMid - luAg) / scale)) ; 	// mg/L //KAW 2013 05 08 Concentrations in Agriculture headwater stream are 3.5x the concentrations in suburban stream (Price, unpublished data)
	}


	LocalLoad_DIN  = runoffVol * 86400 * LocalConc_DIN / 1000; // kg/day
//	LocalLoad_Sub_DIN  = runoffVol * 86400 * LocalConc_Sub_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from suburban part of each grid cell
//	LocalLoad_Ag_DIN  = runoffVol * 86400 * LocalConc_Ag_DIN / 1000; 		// kg/day //KAW 2013 05 08 Loading from the agricultural part of each grid cell
//	LocalLoad_DIN  = LocalLoad_Ag_DIN + LocalLoad_Sub_DIN; 	// kg/day //KAW 2013 05 08: Sum of all N loadings  Total point is from MMM's Code for point source N inputs

	if (loadAdjust > 0.0) LocalLoad_DIN =  LocalLoad_DIN * loadAdjust; 	// RJS 112211


//	if(LocalConc_Sub_DIN > 1.4 || LocalConc_Ag_DIN > 4.9) printf("runoff = %f, luSub = %f, luAg = %f, xMid = %f, Sub_conc = %f, Ag_conc = %f\n", runoff, luSub, luAg, xMid, LocalConc_Sub_DIN, LocalConc_Ag_DIN);

//if (itemID == 809) printf("**** itemID = %d, y=%d, m=%d, d=%d, runoff=%f, luSub=%f, luAg=%f, xMid=%f, Sub_conc=%f, Ag_conc=%f, Subload=%f, Agload=%f, total=%f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), runoff, luSub, luAg, xMid, LocalConc_Sub_DIN, LocalConc_Ag_DIN, LocalLoad_Sub_DIN, LocalLoad_Ag_DIN, LocalLoad_DIN);

	//if (itemID == 31) printf("***** itemID = %d, year = %d, month = %d, day = %d, xMid = %f, luSub = %f, scale = %f, asym = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), xMid, luSub, scale, asym);
	//if (itemID == 31) printf("runoffVol = %f, runoff = %f, LocalLoad_DIN = %f, LocalConc_DIN = %f, Total_point= %f, loadAdjust = %f\n", runoffVol, runoff, LocalLoad_DIN, LocalConc_DIN, Total_point, loadAdjust);  //mmm commented out 2013-3-13


	MFVarSetFloat (_MDOutLocalLoad_DINID,        itemID, LocalLoad_DIN);	  	// RJS 090308

	MFVarSetFloat (_MDOutLocalLoad_Sub_DINID,    itemID, LocalLoad_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutDINSubLoadConcID, 		 itemID, LocalConc_Sub_DIN);  	// KAW 2013/03/15

	MFVarSetFloat (_MDOutLocalLoad_Ag_DINID,     itemID, LocalLoad_Ag_DIN);	  	// RJS 090308
	MFVarSetFloat (_MDOutDINAgLoadConcID, 		 itemID, LocalConc_Ag_DIN);		// KAW 2013/03/15
}

enum {MDcalculate, MDinput, MDnone};

int MDNitrogenInputsDef () {

			int  optID = MFUnset;													    //RJS 10-28-10
			const char *optStr, *optName = MDOptDINInputs;								//RJS 10-28-10
			const char *options [] = { MDCalculateStr, MDInputStr, MDNoneStr, (char *) NULL };		//RJS 10-28-10

	MFDefEntering ("Nitrogen Inputs");

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);  //RJS 02-11-10

	switch (optID) {	//RJS 02-11-10

	case MDnone:
	if (((_MDInLandUseID             = MFVarGetID (MDVarLandUseSpatial,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffID   			 = MFVarGetID (MDVarRunoff,              "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
	    ((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||
	    ((_MDInLawnFractionID        = MFVarGetID (MDVarLawnFraction,      "-", MFInput, MFState, MFBoundary)) == CMfailed) ||		//RJS 051111
	    ((_MDInRiverOrderID          = MFVarGetID (MDVarRiverOrder,  	  	  "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||	// RJS 090308
	    ((_MDInLoadAdjustID          = MFVarGetID (MDVarLoadAdjust,           "-", MFInput, MFState, MFBoundary)) == CMfailed)  ||  // RJS 112211
	    ((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,    "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||  // RJS 090308
	    ((_MDOutLocalLoadnew_DINID   = MFVarGetID (MDVarLocalLoadDINnew, "kg/day", MFOutput, MFFlux,  MFBoundary)) == CMfailed) ||	// RJS 061511	created to compare new and old loads ONLY.
	    (MFModelAddFunction (_MDNitrogenInputsPartitioned) == CMfailed)) return (CMfailed);
		break;

	case MDinput:
	if (((_MDInGrdWatMassRelID       = MFVarGetID (MDVarGroundWaterMassRel,   "kg/day", MFInput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
		((_MDInRunoffPoolMassRelID   = MFVarGetID (MDVarRunoffPoolMassRel,    "kg/day", MFInput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
		((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||															//RJS 050511
		((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,         "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||	//RJS 050511
		(MFModelAddFunction (_MDNitrogenInputsInput) == CMfailed)) return (CMfailed);
		break;

	case MDcalculate:
	if (((_MDInRunoffID   			 = MFVarGetID (MDVarRunoff,              "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||	//RJS 10-28-10
	   	((_MDInRunoffVolID           = MDRunoffVolumeDef ()) == CMfailed) ||			// RJS 10-28-10
//	   	((_MDInDINLoadConcID         = MFVarGetID (MDVarDINLoadConc,       "mg/L",  MFInput, MFFlux, MFBoundary))  == CMfailed) ||	//RJS 10-28-10
//	   	((_MDInLandUseID             = MFVarGetID (MDVarLandUseSpatial,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
		((_MDInLandUseSubID             = MFVarGetID (MDVarLandUseSpatialSub,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
		((_MDInLandUseAgID             	= MFVarGetID (MDVarLandUseSpatialAg,       "-",  MFInput, MFState, MFBoundary)) == CMfailed) ||
//	   	((_MDInTotalPointID			 = MDPointSourceDef ()) == CMfailed) ||															//MMM Added this so that Nitrogeninputs knows it must run MDPointSource in order to calculate localload
	    ((_MDInLoadAdjustID          = MFVarGetID (MDVarLoadAdjust,           "-", MFInput, MFState, MFBoundary)) == CMfailed)  ||  // RJS 112211
		((_MDOutLocalLoad_Sub_DINID     = MFVarGetID (MDVarLocalLoadSubDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//KAW 2013 05 08
		((_MDOutLocalLoad_Ag_DINID      = MFVarGetID (MDVarLocalLoadAgDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//KAW 2013 05 08
	   	((_MDOutDINSubLoadConcID        = MFVarGetID (MDVarDINSubLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	//KAW 2013 05 08
	   	((_MDOutDINAgLoadConcID        	= MFVarGetID (MDVarDINAgLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	//KAW 2013 05 08
		((_MDOutLocalLoad_DINID      = MFVarGetID (MDVarLocalLoadDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))  == CMfailed) ||	//RJS 10-28-10
		((_MDOutDINLoadConcID        = MFVarGetID (MDVarDINLoadConc,      "mg/L",  MFOutput, MFState, MFBoundary)) == CMfailed) || 	// KYLE
	   	((_MDInRiverOrderID          = MFVarGetID (MDVarRiverOrder,           "-", MFInput,  MFState, MFBoundary)) == CMfailed) ||  //RJS 10-29-10
	   	(MFModelAddFunction (_MDNitrogenInputsCalc) == CMfailed)) return (CMfailed);
	   break;

	default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}

	MFDefLeaving ("Nitrogen Inputs");
	return (_MDOutLocalLoad_DINID);

}


