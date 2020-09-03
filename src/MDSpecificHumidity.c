/******************************************************************************
 GHAAS Water Balance/Transport Model V2.0
 Global Hydrologic Archive and Analysis System
 Copyright 1994-2020, UNH - ASRC/CUNY

 MDSpecificHumidity.c

 amiara@ccny.cuny.edu

 *******************************************************************************/
/********************************************************************************
 * Calculates Specific Humidity from pressure, air temperature and rh

 * ******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInAirTemperatureID = MFUnset;
static int _MDInRelativeHumidityID = MFUnset;
static int _MDInAirPressureID = MFUnset;

// Output
static int _MDOutSpecificHumidityID = MFUnset;

static void _MDSpecificHumidity(int itemID) {
    float airtemp;
    float rh;
    float airpressure;
    float es;
    float e;
    float specifichumidity;
    float v1;
    float v2;

    airtemp          = MFVarGetFloat(_MDInAirTemperatureID,   itemID, 0.0) + 273.16;
    airpressure      = MFVarGetFloat(_MDInAirPressureID,      itemID, 0.0); //pressure (Pa)

    rh = MFVarGetFloat(_MDInRelativeHumidityID, itemID, 0.0); // %

    v1 = exp( (17.67 * (airtemp - 273.16) / (airtemp - 29.65) ) );
    v2 = 0.263 * airpressure;
    specifichumidity = (rh * v1) / v2;
    MFVarSetFloat(_MDOutSpecificHumidityID, itemID, specifichumidity);
}

enum {
    MDnone, MDinput, MDcalculate
};

int MDSpecificHumidityDef () {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptWeather_SpecificHumidity;
    const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};
    
    if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
    if ((optID == MDnone) || (_MDOutSpecificHumidityID != MFUnset)) return (_MDOutSpecificHumidityID);
    
    MFDefEntering ("SpecificHumidity");
    
    switch (optID) {
        case MDinput:
            if ((_MDOutSpecificHumidityID = MFVarGetID (MDVarCommon_HumiditySpecific, "degC", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (((_MDInRelativeHumidityID  = MDRelativeHumidityDef ()) == CMfailed) ||
                ((_MDInAirTemperatureID    = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDInAirPressureID       = MFVarGetID (MDVarCommon_AirPressure, "g/kg", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutSpecificHumidityID = MFVarGetID (MDVarCommon_HumiditySpecific, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDSpecificHumidity) == CMfailed))) return (CMfailed);
            break;
        default: MFOptionMessage (optName, optStr, options); return (CMfailed);
    }
    MFDefLeaving ("SpecificHumidity");
    return (_MDOutSpecificHumidityID);
}

