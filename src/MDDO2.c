/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDO2.c

ken.r.sheehan@gmail.com

*******************************************************************************/

#include <stdio.h>
#include <MF.h>
#include <MD.h>

// input
static int _MDInDischarge0ID				= MFUnset;
static int _MDInDischargeID					= MFUnset;
static int _MDInRunoffID					= MFUnset;
static int _MDInRunoffVolumeID				= MFUnset;
static int _MDInWTempRiverID				= MFUnset;
static int _MDInRiverStorageID				= MFUnset;
static int _MDInRiverStorageChgID			= MFUnset;


//output

static int _MDOutRiverMassDO2ID				 = MFUnset;
static int _MDOutRiverStorMassDO2ID			 = MFUnset;
static int _MDOutRiverStorMassChgDO2ID		 = MFUnset;
static int _MDOutRiverConcDO2ID				 = MFUnset;
static int _MDOutRiverMixingMassDO2ID		 = MFUnset;
static int _MDOutRiverMixingStorMassDO2ID	 = MFUnset;
static int _MDOutRiverMixingStorMassChgDO2ID = MFUnset;
static int _MDOutRiverMixingConcDO2ID		 = MFUnset;

static void _MDDO2 (int itemID) {

	// establishing local variables

	int day						 = 0;	// day of month
	int month					 = 0;	// month of year
	int year					 = 0;	// year

	float Q_in 					 = 0.0;	// incoming discharge (m3/s) includes runoff
	float Q_out					 = 0.0;	// outgoing discharge (m3/s)
	float Runoff				 = 0.0;	// local grid cell runoff (mm/d)
	float RunoffVolume 			 = 0.0;	// local grid cell runoff (m3/s)
	float RiverTemp				 = 0.0;	// River temperature (deg C)

	float do2_RO_conc 			 = 0.0;	// concentration of do2 in runoff (mg/L)
	float do2_RO_mass 			 = 0.0;	// mass of do2 in runoff (kg/d)
	float do2_Riv_conc_in 		 = 0.0;	// do2 oncentration of incoming river discharge (mg/l)
	float do2_Riv_mass_in 		 = 0.0;	// do2 mass in incoming river discharge (kg/d)
	float do2_Riv_conc_out 		 = 0.0;	// do2 concentration leaving in discharge (mg/L)
	float do2_Riv_mass_out 	 	 = 0.0;	// o2 mmass leaving in river grid cell (kg/d)
	float do2_Riv_mass_removed 	 = 0.0;	//(kg/d)
	float do2_Riv_mass_added 	 = 0.0;	//(kg/d)

	float do2_RivStor_mass_pre	 = 0.0; // mass of do2 remaining from yesterday in local river (kg/d)
	float do2_RivStor_mass_post	 = 0.0;	// mass of do2 to remain in river until tomorrow
	float do2_RivStor_conc_pre	 = 0.0;
	float do2_RivStor_conc_post  = 0.0;

	float water_RivStor_pre		 = 0.0; // volume of water remaining from yesterday in river grid cell (m3/d)
	float water_RivStor_post 	 = 0.0;	// volume of water that remains in grid cell until tomorrow (m3/d)
	float water_RivStor_chg 	 = 0.0; // change in volume of water stored in river over current time step

	float do2_Riv_mass_total_pre = 0.0;	// total mass of do2 in River before processing
	float water_Riv_total_in     = 0.0;	// total water in river
	float do2_Riv_conc_total_pre = 0.0; // total concentration of do2 in River before processing

	float do2_RivStor_mass_chg   		= 0.0;
	float do2_Riv_mixing_mass_in 		= 0.0;
	float do2_RivStor_mixing_mass_post 	= 0.0;
	float do2_RivStor_mixing_mass_chg 	= 0.0;
	float do2_Riv_mass_total_post 		= 0.0;
	float do2_Riv_conc_total_post 		= 0.0;
	float do2_RivStor_mass_chg2 		= 0.0;
	float do2_RivStor_mixing_mass_pre 	= 0.0;
	float do2_Riv_mixing_mass_total 	= 0.0;
	float do2_Riv_mixing_conc_total 	= 0.0;
	float do2_Riv_mixing_mass_out 		= 0.0;
	float do2_RivStor_mixing_mass_chg2 	= 0.0;

	// Read-in values from greater model

	day 			   = MFDateGetCurrentDay();		// reads in day of month
	month			   = MFDateGetCurrentMonth();	// reads in month of year
	year 			   = MFDateGetCurrentYear();	// reads in year

	Q_in			   = MFVarGetFloat (_MDInDischarge0ID,      itemID, 0.0);	// this includes local runoff
	Q_out			   = MFVarGetFloat (_MDInDischargeID,       itemID, 0.0);
	Runoff			   = MFVarGetFloat (_MDInRunoffID,          itemID, 0.0);
	RunoffVolume	   = MFVarGetFloat (_MDInRunoffVolumeID,    itemID, 0.0);
	RiverTemp		   = MFVarGetFloat (_MDInWTempRiverID,      itemID, 0.0);
	water_RivStor_post = MFVarGetFloat (_MDInRiverStorageID,    itemID, 0.0);
	water_RivStor_chg  = MFVarGetFloat (_MDInRiverStorageChgID, itemID, 0.0);

	do2_Riv_mass_in       = MFVarGetFloat (_MDOutRiverMassDO2ID,        itemID, 0.0);
	do2_RivStor_mass_post = MFVarGetFloat (_MDOutRiverStorMassDO2ID,    itemID, 0.0);
	do2_RivStor_mass_chg  = MFVarGetFloat (_MDOutRiverStorMassChgDO2ID, itemID, 0.0);

	do2_Riv_mixing_mass_in       = MFVarGetFloat (_MDOutRiverMixingMassDO2ID,        itemID, 0.0);
	do2_RivStor_mixing_mass_post = MFVarGetFloat (_MDOutRiverMixingStorMassDO2ID,    itemID, 0.0);
	do2_RivStor_mixing_mass_chg  = MFVarGetFloat (_MDOutRiverMixingStorMassChgDO2ID, itemID, 0.0);

	// loading calculations

	do2_RO_conc            = 1.0;
	do2_RO_mass            = do2_RO_conc * 1000 / 1000000 * RunoffVolume * 86400;
	water_RivStor_pre      = water_RivStor_post - water_RivStor_chg;
	do2_RivStor_mass_pre   = do2_RivStor_mass_post - do2_RivStor_mass_chg;			// note: this needs to be updated (ROB)
	do2_Riv_mass_total_pre = do2_Riv_mass_in + do2_RO_mass + do2_RivStor_mass_pre;
	water_Riv_total_in     = Q_in + water_RivStor_pre;	//m3/s

	do2_Riv_conc_total_pre = water_Riv_total_in > 0.0 ? (do2_Riv_mass_total_pre / (water_Riv_total_in * 86400)) * 1000000 / 1000 : 0.0; // mg/L

	do2_Riv_mass_removed   = 1.0;   // to be calculated
	do2_Riv_mass_added     = 1.0; 	// to be calculated

	do2_Riv_mass_total_post = do2_Riv_mass_total_pre + do2_Riv_mass_added - do2_Riv_mass_removed;	// kg/d
	do2_Riv_conc_total_post = water_Riv_total_in > 0.0 ? (do2_Riv_mass_total_post / (water_Riv_total_in * 86400)) * 1000000 / 1000 : 0.0; // mg/L

	do2_Riv_mass_out       = do2_Riv_conc_total_post * Q_out / 1000000 * 1000 * 86400;
	do2_RivStor_mass_post  = do2_Riv_conc_total_post * water_RivStor_post / 1000000 * 1000;
	do2_RivStor_mass_chg2  = do2_RivStor_mass_post - do2_RivStor_mass_pre;

	//mixing calcs

	do2_RivStor_mixing_mass_pre = do2_RivStor_mixing_mass_post - do2_RivStor_mixing_mass_chg;
	do2_Riv_mixing_mass_total   = do2_Riv_mixing_mass_in + do2_RO_mass + do2_RivStor_mixing_mass_pre;			// kg/d
	do2_Riv_mixing_conc_total   = water_Riv_total_in > 0.0 ? (do2_Riv_mixing_mass_total / water_Riv_total_in * 86400) * 1000000 / 1000 : 0.0;	// mg/L

	do2_Riv_mixing_mass_out      = do2_Riv_mixing_conc_total * Q_out / 1000000 * 1000 * 86400;
	do2_RivStor_mixing_mass_post = do2_Riv_mixing_conc_total * water_RivStor_post / 1000000 * 1000;
	do2_RivStor_mixing_mass_chg2 = do2_RivStor_mixing_mass_post - do2_RivStor_mixing_mass_pre;

	MFVarSetFloat (_MDOutRiverMassDO2ID, 		      itemID,             do2_Riv_mass_out);
	MFVarSetFloat (_MDOutRiverStorMassDO2ID,          itemID,        do2_RivStor_mass_post);
	MFVarSetFloat (_MDOutRiverStorMassChgDO2ID,       itemID,        do2_RivStor_mass_chg2);
	MFVarSetFloat (_MDOutRiverConcDO2ID,              itemID,      do2_Riv_conc_total_post);
	MFVarSetFloat (_MDOutRiverMixingMassDO2ID,        itemID,      do2_Riv_mixing_mass_out);
	MFVarSetFloat (_MDOutRiverMixingStorMassDO2ID,    itemID, do2_RivStor_mixing_mass_post);
	MFVarSetFloat (_MDOutRiverMixingStorMassChgDO2ID, itemID, do2_RivStor_mixing_mass_chg2);
	MFVarSetFloat (_MDOutRiverMixingConcDO2ID,        itemID,    do2_Riv_mixing_conc_total);

	if (itemID == 5) printf("itemID = %d, y = %d, m = %d, d = %d... riverMass = %f, riverConc = %f\n", itemID, year, month, day, do2_Riv_mass_out, do2_Riv_conc_total_post);


}


int MDDO2Def () {

	MFDefEntering ("Dissolved Oxygen2");

	if (((_MDInWTempRiverID                 = MDWTempRiverRouteDef ()) == CMfailed) ||
		((_MDInDischarge0ID                 = MFVarGetID (MDVarDischarge0,                "m3/s",  MFInput, MFState, MFBoundary)) == CMfailed) ||
		((_MDInDischargeID                  = MFVarGetID (MDVarDischarge,                 "m3/s",  MFInput, MFState, MFBoundary)) == CMfailed) ||
		((_MDInRunoffID                     = MFVarGetID (MDVarRunoff,                      "mm",  MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
		((_MDInRunoffVolumeID               = MFVarGetID (MDVarRunoffVolume,              "m3/s",  MFInput, MFState, MFBoundary)) == CMfailed) ||
		((_MDInRiverStorageID		        = MFVarGetID (MDVarRiverStorage,                "m3",  MFInput, MFState,  MFInitial)) == CMfailed) ||
		((_MDInRiverStorageChgID            = MFVarGetID (MDVarRiverStorageChg,           "m3/d",  MFInput, MFState,  MFInitial)) == CMfailed) ||
		((_MDOutRiverMassDO2ID              = MFVarGetID (MDVarRiverMassDO2,              "kg/d",  MFRoute,  MFFlux, MFBoundary)) == CMfailed) ||
		((_MDOutRiverConcDO2ID              = MFVarGetID (MDVarRiverConcDO2,              "mg/L", MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutRiverStorMassDO2ID          = MFVarGetID (MDVarRiverStorMassDO2,            "kg", MFOutput, MFState,  MFInitial)) == CMfailed) ||
		((_MDOutRiverStorMassChgDO2ID       = MFVarGetID (MDVarRiverStorMassChgDO2,       "kg/d", MFOutput,  MFFlux, MFBoundary)) == CMfailed) ||
		((_MDOutRiverMixingMassDO2ID        = MFVarGetID (MDVarRiverMixingMassDO2,        "kg/d",  MFRoute,  MFFlux, MFBoundary)) == CMfailed) ||
		((_MDOutRiverMixingConcDO2ID        = MFVarGetID (MDVarRiverMixingConcDO2,        "mg/L", MFOutput, MFState, MFBoundary)) == CMfailed) ||
		((_MDOutRiverMixingStorMassDO2ID    = MFVarGetID (MDVarRiverMixingStorMassDO2,      "kg", MFOutput, MFState,  MFInitial)) == CMfailed) ||
		((_MDOutRiverMixingStorMassChgDO2ID = MFVarGetID (MDVarRiverMixingStorMassChgDO2, "kg/d", MFOutput,  MFFlux, MFBoundary)) == CMfailed) ||

	    (MFModelAddFunction (_MDDO2) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Dissolved Oxygen2");
	return (_MDOutRiverMassDO2ID);
}
















