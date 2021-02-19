/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrologic Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDTempGrdWater.c

wil.wollheim@unh.edu

EDITED: amiara@ccny.cuny.edu
EDITED: ariel.miara@nrel.gov Feb11 2021

Calculate groundwater temperature by mixing existing groundwater, rain recharge, and irrigation return flow.
Rain recharge temperature is calculated in MDWTempSurfRunoff
Irrigation return flow is assumed to have air temperature.
*******************************************************************************/

#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_AirTemperatureID = MFUnset;
static int _MDInTP2M_GW_TempID   = MFUnset;
// Output
static int _MDOutTP2M_WTempGrdWaterID  = MFUnset;

static void _MDWTP2M_TempGrdWater (int itemID) {
	float airT;
	float Gw_Temp;
	airT               = MFVarGetFloat (_MDInCommon_AirTemperatureID,         itemID, 0.0);
	Gw_Temp            = MFVarGetFloat (_MDInTP2M_GW_TempID,         itemID, 0.0);
//   if (Gw_Temp == 0) { Gw_Temp = airT - 5.0; }

//High‐Resolution Global Water Temperature Modeling 2019.
Gw_Temp = MDMaximum(5.0, airT); 


    MFVarSetFloat (_MDOutTP2M_WTempGrdWaterID,itemID,Gw_Temp);
}

int MDTP2M_WTempGrdWaterDef () {

	if (_MDOutTP2M_WTempGrdWaterID != MFUnset) return (_MDOutTP2M_WTempGrdWaterID);

	MFDefEntering ("Groundwater temperature");

	if (((_MDInCommon_AirTemperatureID = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInTP2M_GW_TempID          = MFVarGetID (MDVarTP2M_GW_Temp,          "degC", MFInput,  MFState, MFBoundary)) == CMfailed) ||
        ((_MDOutTP2M_WTempGrdWaterID   = MFVarGetID (MDVarTP2M_WTempGrdWater,    "degC", MFOutput, MFState, MFInitial))  == CMfailed) ||
		(MFModelAddFunction(_MDWTP2M_TempGrdWater) == CMfailed)) return (CMfailed);

	MFDefLeaving ("Groundwater temperature");
	return (_MDOutTP2M_WTempGrdWaterID);
}
