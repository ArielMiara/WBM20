/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDIrrigation.c

dominik.wisser@unh.edu

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
static int  _MDOutIrrPercolationID      = MFUnset;
static int  _MDOutIrrReturnFlowID       = MFUnset;
static int  _MDOutIrrSoilMoistID        = MFUnset;
static int  _MDOutIrrSMoistChgID        = MFUnset;
static int *_MDOutCropDeficitIDs        = (int *) NULL;
static int *_MDOutCropETIDs             = (int *) NULL;
static int *_MDOutCropGrossDemandIDs    = (int *) NULL;
static int  _MDOutNonIrrFractionID      = MFUnset;

static int  _MDNumberOfIrrCrops;

static int getDaysSincePlanting (int dayOfYearModel, int *dayOfYearPlanting,int numGrowingSeasons,const MDIrrigatedCrop * pIrrCrop) {
	int i, ret = 0;
	int daysSincePlanted; // Default -> crop is not grown!

	for (i = 0; i < numGrowingSeasons; i++) {
		daysSincePlanted = dayOfYearModel - dayOfYearPlanting [i];
		if (daysSincePlanted < 0)  daysSincePlanted = 365 + (dayOfYearModel - dayOfYearPlanting [i]);
		if (daysSincePlanted < pIrrCrop->cropSeasLength[0]
		                     + pIrrCrop->cropSeasLength[1]
							 + pIrrCrop->cropSeasLength[2]
							 + pIrrCrop->cropSeasLength[3]) ret = daysSincePlanted;
	}
	return (ret);
}

static int getCropStage (const MDIrrigatedCrop *pIrrCrop, int daysSincePlanted) {
	int stage = 0;

    if      (daysSincePlanted <= pIrrCrop->cropSeasLength[0]) stage = 1;
    else if (daysSincePlanted <= pIrrCrop->cropSeasLength[0]
	                           + pIrrCrop->cropSeasLength[1]) stage = 2;
    else if (daysSincePlanted <= pIrrCrop->cropSeasLength[0]
	                           + pIrrCrop->cropSeasLength[1]
	                           + pIrrCrop->cropSeasLength[2]) stage = 3;
	else if (daysSincePlanted <= pIrrCrop->cropSeasLength[0]
	                           + pIrrCrop->cropSeasLength[1]
	                           + pIrrCrop->cropSeasLength[2]
	                           + pIrrCrop->cropSeasLength[3]) stage = 4;
	return (stage);
}

static float getCropKc (const MDIrrigatedCrop *pIrrCrop, int daysSincePlanted, int curCropStage) {
	float kc;

   //Returns kc depending on the current stage of the growing season
   switch(curCropStage) {
		default:
		case 0: kc = 0.0; break; //crop is not currently grown
		case 1: kc = pIrrCrop->cropKc [0]; break;
		case 2: kc = pIrrCrop->cropKc [0] + (pIrrCrop->cropKc [1] - pIrrCrop->cropKc [0])
		                                  * (daysSincePlanted - pIrrCrop->cropSeasLength [0])
										  /  pIrrCrop->cropSeasLength [1];
			break;
 		case 3: kc = pIrrCrop->cropKc [1]; break;
		case 4: kc = pIrrCrop->cropKc [1] + (pIrrCrop->cropKc [2] - pIrrCrop->cropKc [1])
		                                  * (daysSincePlanted - (pIrrCrop->cropSeasLength [0] + pIrrCrop->cropSeasLength [1] + pIrrCrop->cropSeasLength [2]))
										  / pIrrCrop->cropSeasLength [3];
			break;
	}
 	return (kc);
}

static float getCurCropRootingDepth (MDIrrigatedCrop *pIrrCrop, int daysSincePlanted) {
	float rootDepth;
	float totalSeasonLenth = pIrrCrop->cropSeasLength [0]
	                       + pIrrCrop->cropSeasLength [1]
					       + pIrrCrop->cropSeasLength [2]
					       + pIrrCrop->cropSeasLength [3];

    rootDepth = pIrrCrop->cropRootingDepth * (0.5 + 0.5 * sin (3.03 * (daysSincePlanted  /  totalSeasonLenth) - 1.47));
 	if (0.15 > rootDepth) rootDepth = 0.15;
	return (rootDepth);
}

static float getCorrDeplFactor (const MDIrrigatedCrop *pIrrCrop, float dailyETP) {
	float cropDeplFactor = pIrrCrop->cropDepletionFactor + 0.04 * (5 - dailyETP);

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
			_MDOutCropDeficitIDs     = (int *) realloc (_MDOutCropDeficitIDs,     (i + 1) * sizeof (int));
			_MDOutCropETIDs          = (int *) realloc (_MDOutCropETIDs,          (i + 1) * sizeof (int));
			_MDOutCropGrossDemandIDs = (int *) realloc (_MDOutCropGrossDemandIDs, (i + 1) * sizeof (int));
			_MDInCropFractionIDs [i] = _MDOutCropETIDs[i] =  _MDOutCropDeficitIDs [i] = _MDOutCropGrossDemandIDs[i] = MFUnset;
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
	float ricePercolation;
	float wltPnt;
	float fldCap;
	float irrAreaFrac;
	float cropFraction [_MDNumberOfIrrCrops + 1];
	int   seasStart [2];
	float reqPondingDepth;
	float irrEffeciency;
    float irrIntensity;
//Output:
	float totGrossDemand;
	float totCropETP;
	float totNetIrrDemand;
	float totIrrPercolation;
// Local
	int   i, curDay, numGrowingSeasons, daysSincePlanted, stage;
	float pondingDepth;
	float cropDepletionFactor;
	float meanSMChange;
	float meanSMoist;
	float totAvlWater, readAvlWater;
	float cropDeficit, prevCropDeficit;
	float bareSoil;
	float cropCoeff;
	float cropWR;
	float deepPercolation;
	float loss;
	float netIrrDemand;
	float rootDepth;
	float sumOfCropFractions;
	float nonRiceWaterBalance;
	float smChange;
	float curCropFraction;
	float returnFlow;
	float refETP;
	
	curDay = MFDateGetDayOfYear ();

	irrAreaFrac = MFVarGetFloat (_MDInIrrAreaFracID, itemID, 0.0);
	
	if (0.0 < irrAreaFrac) {
		sumOfCropFractions = 0.0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {
			cropFraction [i] =  MFVarGetFloat (_MDInCropFractionIDs [i],itemID, 0.0);
			sumOfCropFractions += cropFraction[i];
		}
		cropFraction [i] = 0.0;
		if (0.0 >= sumOfCropFractions) { // No Cropdata for irrigated cell: default to some cereal crop
			MFVarSetFloat (_MDInCropFractionIDs [0], itemID, 1.0);
			sumOfCropFractions = irrAreaFrac;
		}
		reqPondingDepth  = MFVarGetFloat (_MDInRicePoindingDepthID,   itemID, 2.0);
		seasStart [0]    = MFVarGetFloat (_MDInGrowingSeason1ID,      itemID, -100);
		seasStart [1]    = MFVarGetFloat (_MDInGrowingSeason2ID,      itemID, -100);
		irrIntensity     = MFVarGetFloat (_MDInIrrIntensityID,      itemID, 100.0) / 100.0;
		irrEffeciency    = MFVarGetFloat (_MDInIrrEfficiencyID,     itemID, 38);
		precip      = MFVarGetFloat (_MDInPrecipID,            itemID, 0.0);
		snowpackChg      = MFVarGetFloat (_MDInSPackChgID,          itemID, 0.0);
	 	ricePercolation = MFVarGetFloat (_MDInRicePercolationRateID, itemID, 3.0);
	 	wltPnt           = MFVarGetFloat (_MDInWltPntID,            itemID, 0.15);
		fldCap           = MFVarGetFloat (_MDInFldCapaID,           itemID, 0.25);
		refETP           = MFVarGetFloat (_MDInIrrRefEvapotransID,  itemID, 0.0);
		if (0.0 >= refETP)               refETP = 0.01;
		if (0.0 >= irrEffeciency) irrEffeciency = 38.0;
		if (1.2 > irrIntensity && 1.0 < irrIntensity) irrIntensity = 1.0;
		if (2.0 < irrIntensity)                       irrIntensity = 2.0; // TODO irrIntensity dictates cropping seasons this limits it to 2
		if (0.0 >= fldCap) { fldCap = 0.35; wltPnt = 0.2; }

		precip = 0.0 >= snowpackChg ? precip + fabs (snowpackChg) : 0.0;

		numGrowingSeasons = ceil (irrIntensity);

		for (i = 0; i <= _MDNumberOfIrrCrops; ++i) { // cropFraction[_MDNumberOfIrrCrops] is bare soil Area!
		}

		cropDeficit = meanSMChange = totCropETP = 0.0;
		for (i = 0; i < _MDNumberOfIrrCrops; i++) {
			netIrrDemand = cropWR = deepPercolation = smChange = 0.0;
			daysSincePlanted = getDaysSincePlanting (curDay, seasStart, numGrowingSeasons, _MDirrigCropStruct + i);
			cropFraction [i] = cropFraction [i] / sumOfCropFractions;
			// try to grow all crops in Growing Season 1 (always assumed to be the first season!)
			if (0 < daysSincePlanted) { // Growing season
				if (curDay < seasStart [1] || (daysSincePlanted > seasStart [1] - seasStart [0])) // First or perennial growing season
					bareSoil = 1.0 > irrIntensity ? cropFraction [i] * (1.0 - irrIntensity) : 0.0;
				else  // second crop
					bareSoil = 1.0 < irrIntensity ? cropFraction [i] * (2.0 - irrIntensity) : 0.0;
				cropFraction [i] -= bareSoil;
				cropFraction [_MDNumberOfIrrCrops] += bareSoil;
			}
			else { //  Non-growing season
				cropFraction [_MDNumberOfIrrCrops] += cropFraction [i];
				cropFraction [i] = 0.0;
			}
			if (0.0 < cropFraction [i]) {
			 	if (0 < daysSincePlanted) {
					prevCropDeficit = MFVarGetFloat (_MDOutCropDeficitIDs [i],itemID, 0.0);
					stage     = getCropStage (_MDirrigCropStruct + i, daysSincePlanted);
					cropCoeff = getCropKc    (_MDirrigCropStruct + i, daysSincePlanted, stage);
					cropWR    = refETP * cropCoeff;
					rootDepth = getCurCropRootingDepth (_MDirrigCropStruct + i,daysSincePlanted);
					rootDepth = 400; // TODO
				    cropDepletionFactor = getCorrDeplFactor (_MDirrigCropStruct + i, cropWR);
					if (_MDirrigCropStruct [i].cropIsRice == 1) {
					    pondingDepth = prevCropDeficit + precip - cropWR - ricePercolation;
						if (pondingDepth >= reqPondingDepth) {
							deepPercolation = pondingDepth - reqPondingDepth;
							pondingDepth = reqPondingDepth;
						}
						if (pondingDepth < reqPondingDepth) {
							netIrrDemand = reqPondingDepth-pondingDepth;
							pondingDepth = reqPondingDepth;
						}
						cropDeficit  = pondingDepth; //so that current ponding depth gets set..
						smChange = cropDeficit - prevCropDeficit;
						deepPercolation += ricePercolation;
					}
					else {
						totAvlWater  = (fldCap - wltPnt) * rootDepth;
						readAvlWater = totAvlWater * cropDepletionFactor;
						cropDeficit  = prevCropDeficit - precip + cropWR;
						if (0.0 > cropDeficit) { cropDeficit = 0; deepPercolation = precip - prevCropDeficit -cropWR; }
						if (cropDeficit >= totAvlWater) {
							cropDeficit =totAvlWater;
						}
						if (cropDeficit >= readAvlWater) {
							netIrrDemand = cropDeficit;
							netIrrDemand = cropDeficit;
							cropDeficit = prevCropDeficit - netIrrDemand-precip+cropWR;
						}
						smChange = prevCropDeficit - cropDeficit;

						nonRiceWaterBalance = precip + netIrrDemand - cropWR - deepPercolation - smChange;
						smChange = precip + netIrrDemand - cropWR - deepPercolation - nonRiceWaterBalance;
					}
				 	MFVarSetFloat (_MDOutCropDeficitIDs [i], itemID, cropDeficit);
				}
				totNetIrrDemand   += netIrrDemand    * cropFraction [i];
				totCropETP        += cropWR          * cropFraction [i];
				meanSMChange      += smChange        * cropFraction [i];
				totIrrPercolation += deepPercolation * cropFraction [i];
	 		}
			MFVarSetFloat (_MDOutCropETIDs [i], itemID, netIrrDemand * cropFraction [i] * irrAreaFrac); 		
			MFVarSetFloat (_MDOutCropGrossDemandIDs [i], itemID, netIrrDemand * cropFraction [i] * irrAreaFrac * 100.0 / irrEffeciency);
		} // for all crops
		// Add Water Balance for bare soil
		netIrrDemand = cropWR = deepPercolation = smChange = 0.0;
		if (0.0 < cropFraction [_MDNumberOfIrrCrops]) { // Crop is not currently grown. ET from bare soil is equal to ET (initial)
			cropWR = 0.2 * refETP;
			prevCropDeficit = MFVarGetFloat (_MDOutCropDeficitIDs [_MDNumberOfIrrCrops], itemID, 0.0);
			totAvlWater = (fldCap - wltPnt) * 250; // assumed RD = 0.25 m
			deepPercolation = 0.0;
			cropDeficit  = prevCropDeficit - precip + cropWR;
			if (0.0 > cropDeficit) { cropDeficit = 0; deepPercolation = precip - prevCropDeficit - cropWR; }
			if (cropDeficit >= totAvlWater) {
				cropWR = totAvlWater - prevCropDeficit + precip;
				deepPercolation = 0.0;
				cropDeficit = totAvlWater;
			}
 			smChange = prevCropDeficit - cropDeficit;
  			MFVarSetFloat (_MDOutCropDeficitIDs [_MDNumberOfIrrCrops], itemID, cropDeficit);
		}
		MFVarSetFloat (_MDOutCropETIDs [_MDNumberOfIrrCrops], itemID, cropWR);
  		MFVarSetFloat (_MDOutNonIrrFractionID, itemID, cropFraction [_MDNumberOfIrrCrops]);
		totNetIrrDemand   += netIrrDemand    * cropFraction [_MDNumberOfIrrCrops];
		totCropETP        += cropWR          * cropFraction [_MDNumberOfIrrCrops];
		meanSMoist        += cropDeficit     * cropFraction [_MDNumberOfIrrCrops];
		meanSMChange      += smChange        * cropFraction [_MDNumberOfIrrCrops];
		totIrrPercolation += deepPercolation * cropFraction [_MDNumberOfIrrCrops];

		totGrossDemand = totNetIrrDemand * 100.0 / irrEffeciency;

		loss = totGrossDemand - totNetIrrDemand;
		returnFlow = totIrrPercolation + loss * 0.1;
		totCropETP += loss * 0.9;

		MFVarSetFloat (_MDInIrrRefEvapotransID, itemID, refETP            * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrSMoistChgID,    itemID, meanSMChange      * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrNetDemandID,    itemID, totNetIrrDemand   * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrGrossDemandID,  itemID, totGrossDemand    * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrPercolationID,  itemID, totIrrPercolation * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrReturnFlowID,   itemID, returnFlow        * irrAreaFrac);
		MFVarSetFloat (_MDOutIrrEvapotranspID,  itemID, totCropETP        * irrAreaFrac);
	}
	else { // cell is not irrigated
		MFVarSetFloat (_MDInIrrRefEvapotransID, itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSoilMoistID,    itemID, 0.0);
		MFVarSetFloat (_MDOutIrrSMoistChgID,    itemID, 0.0);
 		MFVarSetFloat (_MDOutIrrNetDemandID,    itemID, 0.0);
		MFVarSetFloat (_MDOutIrrGrossDemandID,  itemID, 0.0);
		MFVarSetFloat (_MDOutIrrPercolationID,  itemID, 0.0);
		MFVarSetFloat (_MDOutIrrReturnFlowID,   itemID, 0.0);
		MFVarSetFloat (_MDOutIrrEvapotranspID,  itemID, 0.0);
		for (i = 0; i < _MDNumberOfIrrCrops; i++) { MFVarSetFloat (_MDOutCropETIDs [i], itemID, 0.0); }
	}
}

enum { MDnone, MDinput, MDcalculate };

int MDIrrGrossDemandDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptIrrigation;
	const char *options [] = { MDNoneStr, MDInputStr, MDCalculateStr, (char *) NULL };
	const char *mapOptions   [] = { "FAO", "IWMI", (char *) NULL };
	const char *distrOptions [] = { "FirstSeason","Distributed", (char *) NULL };
	const char *cropParameterFileName;
	int i;
	char varname [20];
	char cropETName [20];
	char cropGrossDemandName [20];

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
			    ((_MDInIrrAreaFracID         = MDIrrigatedAreaDef    ())==  CMfailed) ||
				((_MDInIrrIntensityID        = MFVarGetID (MDVarIrrIntensity,               "-",      MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInWltPntID              = MFVarGetID (MDVarSoilWiltingPoint,           "mm/m",   MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInFldCapaID             = MFVarGetID (MDVarSoilFieldCapacity,          "mm/m",   MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInGrowingSeason1ID      = MFVarGetID (MDVarIrrGrowingSeason1Start,     "DoY",    MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInGrowingSeason2ID      = MFVarGetID (MDVarIrrGrowingSeason2Start,     "DoY",    MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDInRicePoindingDepthID   = MFVarGetID (MDVarIrrRicePondingDepth,        "mm",     MFInput,   MFState, MFBoundary)) == CMfailed) ||
				((_MDInRicePercolationRateID = MFVarGetID (MDVarIrrDailyRicePerolationRate, "mm/day", MFInput ,  MFState, MFBoundary)) == CMfailed) ||
				((_MDInIrrEfficiencyID       = MFVarGetID (MDVarIrrEfficiency,              "-",      MFInput,   MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrEvapotranspID     = MFVarGetID (MDVarIrrEvapotranspiration,      "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrGrossDemandID     = MFVarGetID (MDVarIrrGrossDemand,             "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrPercolationID     = MFVarGetID (MDVarIrrPercolation,             "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrReturnFlowID      = MFVarGetID (MDVarIrrReturnFlow,              "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrNetDemandID       = MFVarGetID (MDVarIrrNetWaterDemand,          "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSoilMoistID       = MFVarGetID (MDVarIrrSoilMoisture,            "mm",     MFOutput,  MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutIrrSMoistChgID       = MFVarGetID (MDVarIrrSoilMoistChange,         "mm",     MFOutput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDOutNonIrrFractionID     = MFVarGetID (MDNonIrrigatedFraction,          "-",      MFOutput,  MFState, MFBoundary)) == CMfailed))
				return (CMfailed);

			for (i = 0; i < _MDNumberOfIrrCrops; i++) {
				sprintf (varname, "CropFraction_%02d", i + 1); // Input Fraction of crop type per cell
				if ((_MDInCropFractionIDs [i] = MFVarGetID (varname, "mm", MFInput, MFState, MFBoundary)) == CMfailed) {
					CMmsgPrint (CMmsgUsrError, "CMfailed in MDInCropFractionID \n");
					return CMfailed;
				}
			}
			for (i = 0; i < _MDNumberOfIrrCrops + 1;i++) {
				sprintf (cropETName,          "CropET%02d",           i + 1);  // Keep track of crop ET for each crop seperately z
				sprintf (cropGrossDemandName, "CropGrossDemand%02d",  i + 1);  // Keep track of crop ET for each crop seperately z
				sprintf (varname,             "CropSMDeficiency%02d", i + 1);  // Output Soil Moisture Deficit, crop ET and WithDrawal per croptype
			    if ((_MDOutCropETIDs [i]          = MFVarGetID (cropETName,          "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) return (CMfailed);
			    if ((_MDOutCropGrossDemandIDs [i] = MFVarGetID (cropGrossDemandName, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) return (CMfailed);
			    if ((_MDOutCropDeficitIDs [i]     = MFVarGetID (varname,             "mm", MFOutput, MFState, MFInitial)) == CMfailed) return (CMfailed);
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