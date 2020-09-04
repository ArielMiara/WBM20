/******************************************************************************
 GHAAS Water Balance/Transport Model V2.0
 Global Hydrologic Archive and Analysis System
 Copyright 1994-2020, UNH - ASRC/CUNY

 MDRelativeHumidity.c

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
static int _MDInSpecificHumidityID = MFUnset;
static int _MDInAirPressureID = MFUnset;

// Output
static int _MDOutRelativeHumidityID = MFUnset;

static void _MDRelativeHumidity (int itemID) {
    float airtemp;
    float sh;
    float airpressure;
    float es;
    float e;
    float relativehumidity;
    float v1;
    float v2;

    airtemp     = MFVarGetFloat (_MDInAirTemperatureID,   itemID, 0.0) + 273.16;
    airpressure = MFVarGetFloat (_MDInAirPressureID,      itemID, 0.0); //pressure (Pa)
    sh          = MFVarGetFloat (_MDInSpecificHumidityID, itemID, 0.0); // jan27 2017 - divide by 100 for NCEP

    v1 = exp( (17.67 * (airtemp - 273.16) / (airtemp - 29.65) ) );
    v2 = 0.263 * airpressure;
    relativehumidity = (sh * v2)/v1;
    MFVarSetFloat(_MDOutRelativeHumidityID, itemID, relativehumidity);
}

enum { MDnone, MDinput, MDcalculate };

int MDCommon_HumidityRelativeDef () {
    int optID = MFUnset;
    const char *optStr, *optName = MDOptWeather_RelativeHumidity;
    const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL};
    
    if ((optStr = MFOptionGet(optName)) != (char *) NULL) optID = CMoptLookup(options, optStr, true);
    if ((optID == MDnone) || (_MDOutRelativeHumidityID != MFUnset)) return (_MDOutRelativeHumidityID);
    
    MFDefEntering ("RelativeHumidity");
    
    switch (optID) {
        case MDinput:
            if      ((_MDOutRelativeHumidityID = MFVarGetID (MDVarCommon_HumidityRelative, "degC", MFInput, MFState, MFBoundary)) == CMfailed) return (CMfailed);
            break;
        case MDcalculate:
            if (((_MDInAirTemperatureID    = MFVarGetID (MDVarCommon_AirTemperature, "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDInSpecificHumidityID  = MFVarGetID (MDVarCommon_HumiditySpecific, "%", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDInAirPressureID       = MFVarGetID (MDVarCommon_AirPressure, "g/kg", MFInput, MFState, MFBoundary)) == CMfailed) ||
                ((_MDOutRelativeHumidityID = MFVarGetID (MDVarCommon_HumidityRelative, "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDRelativeHumidity) == CMfailed))) return (CMfailed);
            break;
        default: MFOptionMessage (optName, optStr, options);
            return (CMfailed);
    }
    MFDefLeaving ("RelativeHumidity");
    return (_MDOutRelativeHumidityID);
}
