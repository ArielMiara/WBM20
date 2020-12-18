/******************************************************************************
Calculating the Particulate Organic Carbon using Beusen et al., (2005; Global Biogeochemical Cycles):
Eq. 2: POCpc = -0.16[log(Qc)]^3 + 2.83[log(Qc)]^2 - 13.6log(Qc) + 20.3
Qc in mg/L

MDPOCarbonFlux.c
May 2013
sagy.cohen@as.ua.edu
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <cm.h>
#include <MF.h>
#include <MD.h>
#include <math.h>
#include <stdlib.h>

// Input
static int _MDInSedimentFluxID = MFUnset;
static int _MDInQsConcID = MFUnset;
static int _MDInQsYieldID = MFUnset;
static int _MDInSedimentFluxID = MFUnset;

// Output
static int _MDOutPOCarbonPercentID 			= MFUnset;
static int _MDOutPOCarbonConcentrationID 	= MFUnset;
static int _MDOutPOCarbonYieldID	 		= MFUnset;
static int _MDOutPOCarbonFluxID 			= MFUnset;


static void _MDPOCarbonFlux (int itemID) {
	float Qc,Qsy,Qs, POCpc, POCc,POCy,POCf;

//Geting the suspended sediment concentration (from MDSedimentFlux.c) and converting from g/L to mg/L 
	Qc = MFVarGetFloat (_MDInQsConcID, 	itemID, 0.0) * 1000;	// in mg/L	
	Qsy= MFVarGetFloat (_MDInQsYieldID, 	itemID, 0.0) ;	// in kg/km2/s
	Qs = MFVarGetFloat (_MDInSedimentFluxID, 	itemID, 0.0) ;	// in mg/L

// Calculating percent POC from Qc
	POCpc = -0.16*pow(log(Qc),3) + 2.83*pow(log(Qc),2) - 13.6*log(Qc) + 20.3;

//Calculating actual POC
	POCc = (POCpc/100) * (Qc*1000) // in units of mg/L 
 	POCy = (POCpc/100) * (Qsy) // in units of kg/km2/s
 	POCf = (POCpc/100) * (Qs) // in units of kg/s
//returning the value to pixel
	MFVarSetFloat (_MDOutPOCarbonPercentID, itemID, POCc);	
	MFVarSetFloat (_MDOutPOCarbonConcentrationID, itemID, POCc);	
	MFVarSetFloat (_MDOutPOCarbonYieldID, itemID, POCy);	
	MFVarSetFloat (_MDOutPOCarbonFluxID, itemID, POCf);	
}

enum { MDinput, MDcalculate, MDcorrected };

int MDSediment_POCarbonFluxDef() {
	
	MFDefEntering ("POCarbonFlux");
	
	if (((_MDInSedimentFluxID   = MDSedimentFluxDef   ()) == CMfailed) || 
	((_MDInQsConcID				= MFVarGetID (MDQsConc 			,"g/L",MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInQsYieldID			= MFVarGetID (MDQsYield 		,"kg/s/km2",MFInput, MFState, MFBoundary)) == CMfailed) ||
	((_MDInSedimentFluxID		= MFVarGetID (MDSedimentFlux 		,"kg/s",MFInput, MFState, MFBoundary)) == CMfailed) ||
    
	// output
	((_MDOutPOCarbonPercentID		= MFVarGetID (MDPOCarbonPercent			,"%",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonConcentrationID	= MFVarGetID (MDPOCarbonConcentration	,"mg/L",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonYieldID			= MFVarGetID (MDPOCarbonYield			,"kg/km2/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	((_MDOutPOCarbonFluxID			= MFVarGetID (MDPOCarbonFlux			,"kg/s",MFOutput, MFState, MFBoundary)) == CMfailed) ||
	(MFModelAddFunction (_MDPOCarbonFlux) == CMfailed)) return (CMfailed);

	MFDefLeaving  ("POCarbonFlux");
	return (_MDOutPOCarbonFluxID);
}
