/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrigation.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <MF.h>
#include <MD.h>

#define MDParIrrigationCropFileName "CropParameterFileName"
 
typedef struct {
    int   ID;
    int   DW_ID;
    char  cropName [80];
    char  cropDistrFileName [80];
    int   cropIsRice;
    float cropSeasLength [4];
    float cropKc [4 - 1];
    float cropRootingDepth;
    float cropDepletionFactor;
    float cropLeachReq;
} MDIrrigatedCrop;

static MDIrrigatedCrop *_MDirrigCropStruct = (MDIrrigatedCrop *) NULL;

//Input
static int  _MDInIrrAreaFracID          = MFUnset;
static int *_MDInCropFractionIDs        = (int *) NULL;
static int  _MDInPrecipID               = MFUnset;
static int  _MDInSPackChgID             = MFUnset;
static int  _MDInIrrRefEvapotransID     = MFUnset;
static int  _MDInFldCapaID              = MFUnset;
static int  _MDInWltPntID               = MFUnset;
static int  _MDInGrowingSeason1ID       = MFUnset;
static int  _MDInGrowingSeason2ID       = MFUnset;
static int  _MDInRicePoindingDepthID    = MFUnset;
static int  _MDInRicePercolationRateID  = MFUnset;
static int  _MDInIrrIntensityID         = MFUnset;
static int  _MDInIrrEfficiencyID        = MFUnset;
//Output
static int  _MDOutIrrNetDemandID        = MFUnset;
static int  _MDOutIrrEvapotranspID      = MFUnset;
static int  _MDOutIrrGrossDemandID      = MFUnset;
static int  _MDOutIrrReturnFlowID       = MFUnset;
static int  _MDOutIrrSoilMoistID        = MFUnset;
static int  _MDOutIrrSMoistChgID        = MFUnset;
static int *_MDOutCropETIDs             = (int *) NULL;
static int *_MDOutCropSMoistIDs         = (int *) NULL;
static int *_MDOutCropActSMoistIDs      = (int *) NULL;
static int *_MDOutCropGrossDemandIDs    = (int *) NULL;
static int  _MDOutNonIrrFractionID      = MFUnset;

static int  _MDNumberOfIrrCrops;

static int getDaysSincePlanting (int dayOfYearModel,int numGrowingSeasons, int *dayOfYearPlanting, int crop) {
	int i, ret = 0;
	int daysSincePlanted;

	if (crop >= _MDNumberOfIrrCrops) return (ret); // Bare soil
	for (i = 0; i < numGrowingSeasons; i++) {
		daysSincePlanted = dayOfYearModel - dayOfYearPlanting [i];
		if (daysSincePlanted < 0)  daysSincePlanted = 365 + (dayOfYearModel - dayOfYearPlanting [i]);
		if (daysSincePlanted < _MDirrigCropStruct [crop].cropSeasLength[0]
		                     + _MDirrigCropStruct [crop].cropSeasLength[1]
							 + _MDirrigCropStruct [crop].cropSeasLength[2]
							 + _MDirrigCropStruct [crop].cropSeasLength[3]) ret = daysSincePlanted;
	}
	return (ret);
}

static int getCropStage (int daysSincePlanted, int crop) {
	int ret = 0;

	if (crop >= _MDNumberOfIrrCrops) return (ret); // Bare soil
    if      (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]) ret = 1;
    else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]) ret = 2;
    else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]
	                           + _MDirrigCropStruct [crop].cropSeasLength[2]) ret = 3;
	else if (daysSincePlanted <= _MDirrigCropStruct [crop].cropSeasLength[0]
	                           + _MDirrigCropStruct [crop].cropSeasLength[1]
	                           + _MDirrigCropStruct [crop].cropSeasLength[2]
	                           + _MDirrigCropStruct [crop].cropSeasLength[3]) ret = 4;
	return (ret);
}

static float getCropKc (int daysSincePlanted, int crop) {
	float kc = 0.0;

   //Returns kc depending on the current stage of the growing season
	if (crop >= _MDNumberOfIrrCrops) return (kc); // Bare soil
	switch (getCropStage (daysSincePlanted, crop)) {
		default:
		case 0: kc = 0.0; break; //crop is not currently grown
		case 1: kc = _MDirrigCropStruct [crop].cropKc [0]; break;
		case 2: kc = _MDirrigCropStruct [crop].cropKc [0]
		           + (_MDirrigCropStruct [crop].cropKc [1] - _MDirrigCropStruct [crop].cropKc [0])
		           * (daysSincePlanted - _MDirrigCropStruct [crop].cropSeasLength [0])
		           / _MDirrigCropStruct [crop].cropSeasLength [1];
			break;
 		case 3: kc = _MDirrigCropStruct [crop].cropKc [1]; break;
		case 4: kc = _MDirrigCropStruct [crop].cropKc [1]
		           + (_MDirrigCropStruct [crop].cropKc [2] - _MDirrigCropStruct [crop].cropKc [1])
		           * (daysSincePlanted - _MDirrigCropStruct [crop].cropSeasLength [0]
		                               - _MDirrigCropStruct [crop].cropSeasLength [1]
		                               - _MDirrigCropStruct [crop].cropSeasLength [2]) / _MDirrigCropStruct [crop].cropSeasLength [3];
			break;
	}
 	return (kc);
}

static float getCurCropRootingDepth (int daysSincePlanted, int crop) {
	float cropRootingDepth = 0.0;
	float totalSeasonLenth;

	if (crop >= _MDNumberOfIrrCrops) return (cropRootingDepth); // Bare soil
	totalSeasonLenth = _MDirrigCropStruct [crop].cropSeasLength [0]
                     + _MDirrigCropStruct [crop].cropSeasLength [1]
	                 + _MDirrigCropStruct [crop].cropSeasLength [2]
	                 + _MDirrigCropStruct [crop].cropSeasLength [3];

    cropRootingDepth = _MDirrigCropStruct [crop].cropRootingDepth * (0.5 + 0.5 * sin (3.03 * (daysSincePlanted  /  totalSeasonLenth) - 1.47));
 	if (0.15 > cropRootingDepth) cropRootingDepth = 0.15; // TODO
	return (cropRootingDepth);
}

static float getCorrDeplFactor (float cropETP, int crop) {
	float cropDeplFactor = 0.0;

	if (crop >= _MDNumberOfIrrCrops) return (cropDeplFactor); // Bare soil
	cropDeplFactor = _MDirrigCropStruct [crop].cropDepletionFactor + 0.04 * (5.0 - cropETP);
    if (0.1 >= cropDeplFactor) cropDeplFactor = 0.1;
	if (0.8 <= cropDeplFactor) cropDeplFactor = 0.8;
	return (cropDeplFactor);
}

static int readCropParameters (const char *filename) {
	FILE *inputCropFile;
	char buffer [512];
	int  i = 0;

	if ((inputCropFile = fopen (filename, "r")) == (FILE *) NULL) {
		CMmsgPrint (CMmsgUsrError,"Crop Parameter file could not be opned, filename: %s\n", filename);
		return (CMfailed);
	}
	else {
		// read headings..
		fgets (buffer,sizeof (buffer),inputCropFile);

		while (fgets(buffer, sizeof (buffer), inputCropFile) != NULL) {
			_MDirrigCropStruct = (MDIrrigatedCrop *) realloc (_MDirrigCropStruct, (i + 1) * sizeof (MDIrrigatedCrop));
			_MDInCropFractionIDs     = (int *) realloc (_MDInCropFractionIDs,     (i + 1) * sizeof (int));
			_MDOutCropSMoistIDs      = (int *) realloc (_MDOutCropSMoistIDs,      (i + 1) * sizeof (int));
			_MDOutCropActSMoistIDs   = (int *) realloc (_MDOutCropSMoistIDs,      (i + 1) * sizeof (int));
			_MDOutCropETIDs          = (int *) realloc (_MDOutCropETIDs,          (i + 1) * sizeof (int));
			_MDOutCropGrossDemandIDs = (int *) realloc (_MDOutCropGrossDemandIDs, (i + 1) * sizeof (int));
			_MDInCropFractionIDs [i] = _MDOutCropETIDs[i] =  _MDOutCropSMoistIDs [i] = _MDOutCropGrossDemandIDs[i] = MFUnset;
			if (sscanf (buffer, "%i" "%i" "%s" "%s" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f" "%f",
		       &(_MDirrigCropStruct [i].ID),
		       &(_MDirrigCropStruct [i].DW_ID),
		         _MDirrigCropStruct [i].cropName,
		         _MDirrigCropStruct [i].cropDistrFileName,
		       &(_MDirrigCropStruct [i].cropKc [0]),
		       &(_MDirrigCropStruct [i].cropKc [1]),
		       &(_MDirrigCropStruct [i].cropKc [2]),
		       &(_MDirrigCropStruct [i].cropSeasLength [0]),
		       &(_MDirrigCropStruct [i].cropSeasLength [1]),
		       &(_MDirrigCropStruct [i].cropSeasLength [2]),
		       &(_MDirrigCropStruct [i].cropSeasLength [3]),
		       &(_MDirrigCropStruct [i].cropRootingDepth),
		       &(_MDirrigCropStruct [i].cropDepletionFactor)) != 13) return (CMfailed);
			_MDirrigCropStruct [i].cropIsRice = strcmp (_MDirrigCropStruct [i].cropName , "Rice") == 0 ? 1 : 0;
			i += 1;
		}
	}
	return (i);
}

static void _MDIrrGrossDemand (int itemID) {
//Input
	float precip;
	float snowpackChg;
	float wltPnt;
	float fldCap;
	float irrAreaFrac;
	float cropFraction [_MDNumberOfIrrCrops + 1];
	int   seasStart [2];
	float ricePercolation;
	float riceReqPondingDepth;
    float irrIntensity;
	float irrEfficiency;
//Output:
	float irrGrossDemand;
	float irrNetDemand;
	float irrCropETP;
	float irrReturnFlow;
	float irrSMoistChg;
	float irrSMoist;
// Local
	int   cropID, curDay, numGrowingSeasons, daysSincePlanted;
	float ricePondingDepth;
	float cropAvlWater, cropMinSMoist;
	float cropETP;
	float cropReturnFlow;
	float cropNetDemand, cropGrossDemand;
	float cropCurRootingDepth, cropPrevRootingDepth, cropMaxRootingDepth;
	float cropSMoist, cropActSMoist, cropPrevSMoist, cropPrevActSMoist;
	float cropSMoistChg;
	float bareSoil;
	float loss;
	float sumOfCropFractions;
	float nonRiceWaterBalance;
	float refETP;
	
	curDay = MFDateGetDayOfYear ();

	irrAreaFrac = MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0);
	
	if (0.0 < irrAreaFrac) {
		sumOfCropFractions = 0.0;
		for (cropID = 0; cropID < _MDNumberOfIrrCrops; ++cropID) {
			cropFraction [cropID] =  MFVarGetFloat (_MDInCropFractionIDs [cropID],itemID, 0.0);
			sumOfCropFractions += cropFraction [cropID];
		}
		// default to bare soil when there is no irrigated crop in grid cell
		if (0.0 >= sumOfCropFractions) cropFraction [cropID] = 0.0;
		else cropFraction [cropID] = sumOfCropFractions = irrAreaFrac;

		for (cropID = 0; cropID <= _MDNumberOfIrrCrops; ++cropID) cropFraction [cropID] = cropFraction [cropID] / sumOfCropFractions;

		riceReqPondingDepth = MFVarGetFloat (_MDInRicePoindingDepthID,   itemID,   2.00);
		seasStart [0]       = MFVarGetFloat (_MDInGrowingSeason1ID,      itemID, -100);
		seasStart [1]       = MFVarGetFloat (_MDInGrowingSeason2ID,      itemID, -100);
		irrIntensity        = MFVarGetFloat (_MDInIrrIntensityID,        itemID, 100.00) / 100.0;
		irrEfficiency       = MFVarGetFloat (_MDInIrrEfficiencyID,       itemID,  38.00);
		precip              = MFVarGetFloat (_MDInPrecipID,              itemID,   0.00);
		snowpackChg         = MFVarGetFloat (_MDInSPackChgID,            itemID,   0.00);
	 	ricePercolation     = MFVarGetFloat (_MDInRicePercolationRateID, itemID,   3.00);
	 	wltPnt              = MFVarGetFloat (_MDInWltPntID,              itemID,   0.15);
		fldCap              = MFVarGetFloat (_MDInFldCapaID,             itemID,   0.25);
		refETP              = MFVarGetFloat (_MDInIrrRefEvapotransID,    itemID,   0.00);
		if (0.0 >= refETP)               refETP =  0.0;
		if (0.0 >= irrEfficiency) irrEfficiency = 38.0;
		if (2.0 < irrIntensity)    irrIntensity =  2.0; // TODO irrIntensity dictates cropping seasons this limits it to 2
		if (0.0 >= fldCap) { fldCap = 0.35; wltPnt = 0.2; }

		precip = 0.0 >= snowpackChg ? precip + fabs (snowpackChg) : 0.0;
		numGrowingSeasons = ceil (irrIntensity);
		irrNetDemand = irrGrossDemand = irrReturnFlow = irrSMoistChg = irrCropETP = 0.0;
		for (cropID = 0; cropID <= _MDNumberOfIrrCrops; ++cropID) {
			daysSincePlanted = getDaysSincePlanting (curDay, numGrowingSeasons, seasStart, cropID);
			if (0 < daysSincePlanted) { // Growing season
				if (curDay < seasStart [1] || (daysSincePlanted > seasStart [1] - seasStart [0])) // First growing season
					bareSoil = 1.0 > irrIntensity ? cropFraction [cropID] * (1.0 - irrIntensity) : 0.0;
				else  // Second growing season
					bareSoil = 1.0 < irrIntensity ? cropFraction [cropID] * (2.0 - irrIntensity) : cropFraction [cropID];
				cropFraction [cropID] -= bareSoil;
				cropFraction [_MDNumberOfIrrCrops] += bareSoil;
				if (0.0 < cropFraction [cropID]) {
					cropETP = refETP * getCropKc (daysSincePlanted, cropID);
/* Rice */			if (_MDirrigCropStruct [cropID].cropIsRice == 1) {
	/* Rainfed */		if (precip - cropETP - ricePercolation >= 0.0) {
							cropNetDemand  = cropGrossDemand = 0.0;
							cropReturnFlow = precip - cropETP; // actual percolation can be higher then the rice percolation input
						}
	/* Irrigated */		else {
							cropNetDemand  = cropGrossDemand = cropETP + ricePercolation - precip;
							cropReturnFlow = ricePercolation;
						}
						cropActSMoist = cropSMoist = ricePondingDepth;
						cropSMoistChg = 0.0;
/* Non-rice */		} else {
						cropPrevSMoist       = MFVarGetFloat (_MDOutCropSMoistIDs    [cropID], itemID, 0.0);
						cropPrevActSMoist    = MFVarGetFloat (_MDOutCropActSMoistIDs [cropID], itemID, 0.0);

						cropMaxRootingDepth  = _MDirrigCropStruct [cropID].cropRootingDepth;
						cropPrevRootingDepth = getCurCropRootingDepth (daysSincePlanted - 1, cropID);
						cropCurRootingDepth  = getCurCropRootingDepth (daysSincePlanted, cropID);

						if (cropCurRootingDepth < cropPrevRootingDepth) CMmsgPrint (CMmsgWarning, "Negative rooting depth growth!");
						if (cropPrevSMoist - cropPrevActSMoist)         CMmsgPrint (CMmsgWarning, "Negative incactive soil moisture!");
						cropPrevActSMoist += (cropCurRootingDepth - cropPrevRootingDepth) * (cropPrevSMoist - cropPrevActSMoist) / (cropMaxRootingDepth - cropPrevRootingDepth);

						cropAvlWater  = (fldCap - wltPnt) * cropCurRootingDepth;
						cropMinSMoist = cropAvlWater * getCorrDeplFactor (cropETP, cropID);
	/* Rainfed */		if (0.0 > precip + cropPrevSMoist - cropETP - cropMinSMoist) {
							cropActSMoist = cropPrevSMoist + precip - cropETP > cropMinSMoist? cropPrevSMoist + precip - cropETP : cropMinSMoist;
							if (cropActSMoist > cropAvlWater) cropActSMoist = cropAvlWater;
							cropNetDemand  = cropGrossDemand = 0.0;
							cropReturnFlow = precip + cropPrevActSMoist - cropETP - cropActSMoist;
	/* Irrigated */		} else {
							cropActSMoist   = cropMinSMoist;
							cropNetDemand   = cropMinSMoist + cropETP - precip - cropPrevActSMoist;
							cropGrossDemand = cropNetDemand / irrEfficiency;
							cropReturnFlow  = cropGrossDemand - cropNetDemand;
						}
						cropSMoist    = cropActSMoist + (cropMaxRootingDepth - cropCurRootingDepth) * (cropPrevSMoist - cropPrevActSMoist) / (cropMaxRootingDepth - cropPrevRootingDepth);
						cropSMoistChg = cropPrevSMoist - cropSMoist;
					}
				 	MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, cropActSMoist);
				 	MFVarSetFloat (_MDOutCropSMoistIDs    [cropID], itemID, cropSMoist);
/* Nothing */	} else {
					cropNetDemand   =
					cropGrossDemand =
					cropReturnFlow  =
					cropSMoist      =
					cropSMoistChg   = 0.0;
				}
/* Bare */	} else {
				if (0.0 < cropFraction [cropID]) {
					cropETP = precip < refETP ? precip : refETP;
					cropNetDemand  = cropGrossDemand = 0.0;
					cropReturnFlow = precip - cropETP;
					cropSMoist = MFVarGetFloat (_MDOutCropSMoistIDs [cropID], itemID, 0.0);
					cropSMoistChg = 0.0;
				 	MFVarSetFloat (_MDOutCropActSMoistIDs [cropID], itemID, 0.0);
				}
			}
			irrCropETP     += cropETP         * cropFraction [cropID];
			irrNetDemand   += cropNetDemand   * cropFraction [cropID];
			irrGrossDemand += cropGrossDemand * cropFraction [cropID];
			irrReturnFlow  += cropReturnFlow  * cropFraction [cropID];
			irrSMoist      += cropSMoist      * cropFraction [cropID];
			irrSMoistChg   += cropSMoistChg   * cropFraction [cropID];
		}
		MFVarSetFloat (_MDInIrrRefEvapotransID, itemID, refETP         * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrEvapotranspID,  itemID, irrCropETP     * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrSMoistChgID,    itemID, irrSMoistChg   * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrNetDemandID,    itemID, irrNetDemand   * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrGrossDemandID,  itemID, irrGrossDemand * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrReturnFlowID,  itemID, irrReturnFlow * irrAreaFrac);
	}
	else { // cell is not irrigated
		MFVarSetFloat (_MDInIrrRefEvapotransID, itemID, 0.0);
		MFVarSetFloat (_MDOutIrrEvapotranspID,  itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSoilMoistID,    itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSMoistChgID,    itemID, 0.0);
 		MFVarSetFloat (_MDOutIrrNetDemandID,    itemID, 0.0);
		MFVarSetFloat (_MDOutIrrGrossDemandID,  itemID, 0.0);
		MFVarSetFloat (_MDOutIrrReturnFlowID,   itemID, 0.0);
		for (cropID = 0; cropID < _MDNumberOfIrrCrops; ++cropID) { MFVarSetFloat (_MDOutCropETIDs [cropID], itemID, 0.0); }
	}
}

enum { MDnone, MDinput, MDcalculate };

int MDIrrGrossDemandDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptIrrigation;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };
	const char *cropParameterFileName;
	int i;
	char varname [32];
	char cropETName [32];
	char cropGrossDemandName [32];

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options,optStr,true);

	if ((optID == MDnone) || (_MDOutIrrGrossDemandID != MFUnset)) return (_MDOutIrrGrossDemandID);

	MFDefEntering ("Irrigation Gross Demand");

	switch (optID) {
		case MDinput:
			 if(((_MDOutIrrGrossDemandID = MFVarGetID (MDVarIrrGrossDemand,        "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
		    	((_MDOutIrrReturnFlowID  = MFVarGetID (MDVarIrrReturnFlow,         "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDOutIrrEvapotranspID = MFVarGetID (MDVarIrrEvapotranspiration, "mm", MFInput,  MFFlux,  MFBoundary)) == CMfailed))
				return (CMfailed);
			break;
		case MDcalculate:
			if ((optStr = MFOptionGet (MDParIrrigationCropFileName)) != (char *) NULL) cropParameterFileName = optStr;
			if ((_MDNumberOfIrrCrops = readCropParameters (cropParameterFileName)) == CMfailed) {
				CMmsgPrint(CMmsgUsrError,"Error reading crop parameter file: %s \n", cropParameterFileName);
				return CMfailed;
			}
			if (((_MDInPrecipID              = MDPrecipitationDef    ()) == CMfailed) ||	 
			    ((_MDInSPackChgID            = MDSPackChgDef         ()) == CMfailed) ||
			    ((_MDInIrrRefEvapotransID    = MDIrrRefEvapotransDef ()) == CMfailed) ||
			    ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ()) == CMfailed) ||
				((_MDInIrrIntensityID        = MFVarGetID (MDVarIrrIntensity,               MFNoUnit, MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInWltPntID              = MFVarGetID (MDVarSoilWiltingPoint,           "mm/m",   MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInFldCapaID             = MFVarGetID (MDVarSoilFieldCapacity,          "mm/m",   MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRicePoindingDepthID   = MFVarGetID (MDVarIrrRicePondingDepth,        "mm",     MFInput,   MFState, MFBoundary)) == CMfailed) ||
				((_MDInRicePercolationRateID = MFVarGetID (MDVarIrrDailyRicePerolationRate, "mm/day", MFInput ,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInIrrEfficiencyID       = MFVarGetID (MDVarIrrEfficiency,              MFNoUnit, MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrEvapotranspID     = MFVarGetID (MDVarIrrEvapotranspiration,      "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrGrossDemandID     = MFVarGetID (MDVarIrrGrossDemand,             "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrReturnFlowID      = MFVarGetID (MDVarIrrReturnFlow,              "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrNetDemandID       = MFVarGetID (MDVarIrrNetWaterDemand,          "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSoilMoistID       = MFVarGetID (MDVarIrrSoilMoisture,            "mm",     MFOutput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSMoistChgID       = MFVarGetID (MDVarIrrSoilMoistChange,         "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDOutNonIrrFractionID     = MFVarGetID (MDNonIrrigatedFraction,          MFNoUnit, MFOutput,  MFState, MFBoundary)) == CMfailed))
				return (CMfailed);

			for (i = 0; i < _MDNumberOfIrrCrops; i++) {
				sprintf (varname, "CropFraction_%02d", i + 1); // Input Fraction of crop type per cell
				if ((_MDInCropFractionIDs [i] = MFVarGetID (varname, "mm", MFInput, MFState, MFBoundary)) == CMfailed) {
					CMmsgPrint (CMmsgUsrError, "CMfailed in MDInCropFractionID \n");
					return CMfailed;
				}
			}
			for (i = 0; i < _MDNumberOfIrrCrops + 1;i++) {
				sprintf (cropETName,          "CropET%02d",              i + 1);  // Keep track of crop ET for each crop
				sprintf (cropGrossDemandName, "CropGrossDemand%02d",     i + 1);  // Keep track of gross demand
				sprintf (varname,             "CropSoilMoisture%02d",    i + 1);  // Output Soil Moisture
				sprintf (varname,             "CropActSoilMoisture%02d", i + 1);  // Output Active Soil Moisture
			    if (((_MDOutCropETIDs [i]          = MFVarGetID (cropETName,          "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			        ((_MDOutCropGrossDemandIDs [i] = MFVarGetID (cropGrossDemandName, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
			        ((_MDOutCropSMoistIDs [i]      = MFVarGetID (varname,             "mm", MFOutput, MFState, MFInitial)) == CMfailed) ||
			        ((_MDOutCropActSMoistIDs [i]   = MFVarGetID (varname,             "mm", MFOutput, MFState, MFInitial)) == CMfailed)) return (CMfailed);
			}
			if (MFModelAddFunction (_MDIrrGrossDemand) == CMfailed) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving("Irrigation Gross Demand");
	return (_MDOutIrrGrossDemandID);
}

int MDIrrReturnFlowDef() {
	int ret;

	if (_MDOutIrrReturnFlowID != MFUnset) return (_MDOutIrrReturnFlowID);

	if ((ret = MDIrrGrossDemandDef ()) == CMfailed) return (CMfailed);
	if (ret == MFUnset) return (MFUnset);
	_MDOutIrrReturnFlowID = MFVarGetID (MDVarIrrReturnFlow,     "mm",   MFInput, MFFlux,  MFBoundary);
    return (_MDOutIrrReturnFlowID);
}