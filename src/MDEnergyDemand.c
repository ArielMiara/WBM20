/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDEnergyDemand.c

amiara00@CCNY.cuny.edu & rob.stewart@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInTotal2008ID         = MFUnset;

// Output
static int _MDOutEnergyDemandID		= MFUnset;


static void _MDEnergyDemand (int itemID) {

float total_2008 = 0.0;		// total annual (2008) energy demand (MWhrs) for each power plant
float month      = 0.0;
float year       = 0.0;

float cA_2000 = 0.9314;
float cA_2001 = 0.9541;
float cA_2002 = 0.9728;
float cA_2003 = 0.9874;
float cA_2004 = 0.9980;
float cA_2005 = 1.0046;
float cA_2006 = 1.0071;
float cA_2007 = 1.0056;
float cA_2008 = 1.0;
float cA_2009 = 0.9904;
float cA_2010 = 0.9768;
float cA_XXXX = 0.9844;

float cM_01 = 1.0740;
float cM_02 = 0.9721;
float cM_03 = 0.9731;
float cM_04 = 0.8617;
float cM_05 = 0.8795;
float cM_06 = 1.0946;
float cM_07 = 1.1922;
float cM_08 = 1.0815;
float cM_09 = 0.9891;
float cM_10 = 0.9144;
float cM_11 = 0.9354;
float cM_12 = 1.0325;

float cA = 0.0;
float cM = 0.0;
float Ann_demand = 0.0;
float Mon_demand = 0.0;
float Day_demand = 0.0;


	total_2008			  = MFVarGetFloat (_MDInTotal2008ID, itemID, 0.0) * 365;		// this value is multiplied by 365 to account for automatic division by 365 by model
	month 				  = MFDateGetCurrentMonth ();
	year     			  = MFDateGetCurrentYear ();

	if (year == 2000) cA = cA_2000;
	if (year == 2001) cA = cA_2001;
	if (year == 2002) cA = cA_2002;
	if (year == 2003) cA = cA_2003;
	if (year == 2004) cA = cA_2004;
	if (year == 2005) cA = cA_2005;
	if (year == 2006) cA = cA_2006;
	if (year == 2007) cA = cA_2007;
	if (year == 2008) cA = cA_2008;
	if (year == 2009) cA = cA_2009;
	if (year == 2010) cA = cA_2010;
	if (year == -9999) cA = cA_XXXX;

	if (month == 1)  cM = cM_01;
	if (month == 2)  cM = cM_02;
	if (month == 3)  cM = cM_03;
	if (month == 4)  cM = cM_04;
	if (month == 5)  cM = cM_05;
	if (month == 6)  cM = cM_06;
	if (month == 7)  cM = cM_07;
	if (month == 8)  cM = cM_08;
	if (month == 9)  cM = cM_09;
	if (month == 10) cM = cM_10;
	if (month == 11) cM = cM_11;
	if (month == 12) cM = cM_12;

	Ann_demand = cA * total_2008;
	Mon_demand = (Ann_demand / 12) * cM;
	Day_demand = (Mon_demand / 30);

//	if (itemID == 19) {
//	printf("cA = %f, cM = %f, A-Demand = %f, M-Demand = %f, D-Demand = %f\n", cA, cM, Ann_demand, Mon_demand, Day_demand);
//	}

	MFVarSetFloat(_MDOutEnergyDemandID,     itemID, Day_demand);


}


int MDEnergyDemandDef () {


	MFDefEntering ("Energy Demand");

			if (((_MDInTotal2008ID 	   = MFVarGetID (MDVarTotal2008,   	"MW", MFInput,  MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutEnergyDemandID = MFVarGetID (MDVarEnergyDemand,   "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||

			  (MFModelAddFunction (_MDEnergyDemand) == CMfailed)) return (CMfailed);


	MFDefLeaving ("Energy Demand");

	return (_MDOutEnergyDemandID);
}


