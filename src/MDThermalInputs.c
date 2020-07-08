/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDThermalInputs.c

rob.stewart@unh.edu

Thermal Inputs and withdrawals

*******************************************************************************/

#include <string.h>
#include <MF.h>
#include <MD.h>
#include <math.h>


// Input
static int _MDInDischargeID     = MFUnset;
static int _MDFlux_QxTID		= MFUnset;
static int _MDFluxMixing_QxTID	= MFUnset;
static int _MDInNamePlate1ID	= MFUnset;
static int _MDInNamePlate2ID	= MFUnset;
static int _MDInNamePlate3ID	= MFUnset;
static int _MDInNamePlate4ID	= MFUnset;
static int _MDInNamePlate5ID	= MFUnset;
static int _MDInNamePlate6ID	= MFUnset;
static int _MDInNamePlate7ID	= MFUnset;
static int _MDInNamePlate8ID	= MFUnset;

static int _MDInFuelType1ID		= MFUnset;
static int _MDInFuelType2ID		= MFUnset;
static int _MDInFuelType3ID		= MFUnset;
static int _MDInFuelType4ID		= MFUnset;
static int _MDInFuelType5ID		= MFUnset;
static int _MDInFuelType6ID		= MFUnset;
static int _MDInFuelType7ID		= MFUnset;
static int _MDInFuelType8ID		= MFUnset;

static int _MDInOptDeltaTID		= MFUnset;
static int _MDInTempLimitID		= MFUnset;
static int _MDPlaceHolderID		= MFUnset;

static int _MDInDischargeIncomingID = MFUnset;

// Output
static int _MDWTemp_QxTID            = MFUnset;
static int _MDWTempMixing_QxTID      = MFUnset;
static int _MDOutTotalThermalWdlsID  = MFUnset;
static int _MDOutTotalOptThermalWdlsID = MFUnset;
static int _MDOutPowerOutput1ID		 = MFUnset;
static int _MDOutPowerOutput2ID		 = MFUnset;
static int _MDOutPowerOutput3ID		 = MFUnset;
static int _MDOutPowerOutput4ID		 = MFUnset;
static int _MDOutPowerOutput5ID		 = MFUnset;
static int _MDOutPowerOutput6ID		 = MFUnset;
static int _MDOutPowerOutput7ID		 = MFUnset;
static int _MDOutPowerOutput8ID		 = MFUnset;
static int _MDOutPowerDeficit1ID	 = MFUnset;
static int _MDOutPowerDeficit2ID	 = MFUnset;
static int _MDOutPowerDeficit3ID	 = MFUnset;
static int _MDOutPowerDeficit4ID	 = MFUnset;
static int _MDOutPowerDeficit5ID	 = MFUnset;
static int _MDOutPowerDeficit6ID	 = MFUnset;
static int _MDOutPowerDeficit7ID	 = MFUnset;
static int _MDOutPowerDeficit8ID	 = MFUnset;
static int _MDOutPowerPercent1ID	 = MFUnset;
static int _MDOutPowerPercent2ID	 = MFUnset;
static int _MDOutPowerPercent3ID	 = MFUnset;
static int _MDOutPowerPercent4ID	 = MFUnset;
static int _MDOutPowerPercent5ID	 = MFUnset;
static int _MDOutPowerPercent6ID	 = MFUnset;
static int _MDOutPowerPercent7ID	 = MFUnset;
static int _MDOutPowerPercent8ID	 = MFUnset;
static int _MDOutPowerOutputTotalID  = MFUnset;
static int _MDOutPowerDeficitTotalID = MFUnset;
static int _MDOutPowerPercentTotalID = MFUnset;

static void _MDThermalInputs (int itemID) {

float Q;			// discharge (m3/s)
float Q_incoming;	// discharge including runoff (m3/s)

float NamePlate1;	// 1st Powerplant's nameplate in grid cell (MW)
float NamePlate2;	// 2nd Powerplant's nameplate in grid cell (MW)
float NamePlate3;	// 3rd Powerplant's nameplate in grid cell (MW)
float NamePlate4;	// 4th Powerplant's nameplate in grid cell (MW)
float NamePlate5;	// 5th Powerplant's nameplate in grid cell (MW)
float NamePlate6;	// 6th Powerplant's nameplate in grid cell (MW)
float NamePlate7;	// 7th Powerplant's nameplate in grid cell (MW)
float NamePlate8;	// 8th Powerplant's nameplate in grid cell (MW)
	
float FuelType1;		// 1st Powerplant's fueltype in grid cell  (Coal = 1, Gas = 2, Oil = 3, Nuclear = 4, Other = 5)
float FuelType2;		// 2nd Powerplant's fueltype in grid cell
float FuelType3;		// 3rd Powerplant's fueltype in grid cell
float FuelType4;		// 4th Powerplant's fueltype in grid cell
float FuelType5;		// 5th Powerplant's fueltype in grid cell
float FuelType6;		// 6th Powerplant's fueltype in grid cell
float FuelType7;		// 7th Powerplant's fueltype in grid cell
float FuelType8;		// 8th Powerplant's fueltype in grid cell
	
float optDeltaT;		// degrees C
float tempLimit; 		// degrees C
	
float optWdl_1 = 0.0;  // m3/s
float optWdl_2 = 0.0;  // m3/s
float optWdl_3 = 0.0;  // m3/s
float optWdl_4 = 0.0;  // m3/s
float optWdl_5 = 0.0;  // m3/s
float optWdl_6 = 0.0;  // m3/s
float optWdl_7 = 0.0;  // m3/s
float optWdl_8 = 0.0;  // m3/s
	
float beta1 = 0.0;		// unitless
float beta2 = 0.0;		// unitless
float beta3 = 0.0;		// unitless
float beta4 = 0.0;		// unitless
float beta5 = 0.0;		// unitless
float beta6 = 0.0;		// unitless
float beta7 = 0.0;		// unitless
float beta8 = 0.0;		// unitless
	
float conv_coal	 = 0.03837975;		// m3/s per MW
float conv_gas   = 0.012618;		// m3/s per MW
float conv_oil   = 0.03732825;		// m3/s per MW
float conv_nuc   = 0.046266;		// m3/s per MW
float conv_other = 0.033648;		// m3/s per MW
	
float temp_effluent1 = 0.0;		// degrees C
float temp_effluent2 = 0.0;		// degrees C
float temp_effluent3 = 0.0;		// degrees C
float temp_effluent4 = 0.0;		// degrees C
float temp_effluent5 = 0.0;		// degrees C
float temp_effluent6 = 0.0;		// degrees C
float temp_effluent7 = 0.0;		// degrees C
float temp_effluent8 = 0.0;		// degrees C

float temp_effluent1_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent2_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent3_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent4_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent5_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent6_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent7_mix = 0.0;	// degrees C (mixing scenario)
float temp_effluent8_mix = 0.0;	// degrees C (mixing scenario)

float realDeltaT_1 = 0.0;			// degrees C
float realDeltaT_2 = 0.0;			// degrees C
float realDeltaT_3 = 0.0;			// degrees C
float realDeltaT_4 = 0.0;			// degrees C
float realDeltaT_5 = 0.0;			// degrees C
float realDeltaT_6 = 0.0;			// degrees C
float realDeltaT_7 = 0.0;			// degrees C
float realDeltaT_8 = 0.0;			// degrees C

float wdl_coeff_1 = 0.0;			// unitless
float wdl_coeff_2 = 0.0;			// unitless
float wdl_coeff_3 = 0.0;			// unitless
float wdl_coeff_4 = 0.0;			// unitless
float wdl_coeff_5 = 0.0;			// unitless
float wdl_coeff_6 = 0.0;			// unitless
float wdl_coeff_7 = 0.0;			// unitless
float wdl_coeff_8 = 0.0;			// unitless

float realWdl_1 = 0.0;			// m3/s
float realWdl_2 = 0.0;			// m3/s
float realWdl_3 = 0.0;			// m3/s
float realWdl_4 = 0.0;			// m3/s
float realWdl_5 = 0.0;			// m3/s
float realWdl_6 = 0.0;			// m3/s
float realWdl_7 = 0.0;			// m3/s
float realWdl_8 = 0.0;			// m3/s
	
float riverTemp_post		= 0.0;	// degrees C
float riverTempMixing_post  = 0.0;	// degrees C
float totalWdl				= 0.0;	// m3/s
float totalWdl_m3d			= 0.0;  // m3/d
float totalOptWdl			= 0.0;	// m3/s
float totalOptWdl_m3d		= 0.0;	// m3/d

//float wdl_QxT;
//float QxT_mix;
//	float QxT_post;
//	float QxT_mix_post;
float Q_WTemp	   = 0.0;
float Q_WTemp_mix  = 0.0;

float Q_WTemp_out1 = 0.0;
float Q_WTemp_out2 = 0.0;
float Q_WTemp_out3 = 0.0;
float Q_WTemp_out4 = 0.0;
float Q_WTemp_out5 = 0.0;
float Q_WTemp_out6 = 0.0;
float Q_WTemp_out7 = 0.0;
float Q_WTemp_out8 = 0.0;

float Q_WTemp_mix_out1 = 0.0;
float Q_WTemp_mix_out2 = 0.0;
float Q_WTemp_mix_out3 = 0.0;
float Q_WTemp_mix_out4 = 0.0;
float Q_WTemp_mix_out5 = 0.0;
float Q_WTemp_mix_out6 = 0.0;
float Q_WTemp_mix_out7 = 0.0;
float Q_WTemp_mix_out8 = 0.0;

float flux_QxT 		 = 0.0;
float fluxmixing_QxT = 0.0;

float flux_QxT_new   	 = 0.0;
float fluxmixing_QxT_new = 0.0;

float placeHolder		 = 0.0;

float power_Output_1     = 0.0;		// MW
float power_Output_2     = 0.0;		// MW
float power_Output_3     = 0.0;		// MW
float power_Output_4     = 0.0;		// MW
float power_Output_5     = 0.0;		// MW
float power_Output_6     = 0.0;		// MW
float power_Output_7     = 0.0;		// MW
float power_Output_8     = 0.0;		// MW
float power_Output_total = 0.0;		// MW

float power_Deficit_1     = 0.0;	// MW
float power_Deficit_2     = 0.0;	// MW
float power_Deficit_3     = 0.0;	// MW
float power_Deficit_4     = 0.0;	// MW
float power_Deficit_5     = 0.0;	// MW
float power_Deficit_6     = 0.0;	// MW
float power_Deficit_7     = 0.0;	// MW
float power_Deficit_8     = 0.0;	// MW
float power_Deficit_total = 0.0;	// MW

float power_Percent_1     = 0.0;	// MW
float power_Percent_2     = 0.0;	// MW
float power_Percent_3     = 0.0;	// MW
float power_Percent_4     = 0.0;	// MW
float power_Percent_5     = 0.0;	// MW
float power_Percent_6     = 0.0;	// MW
float power_Percent_7     = 0.0;	// MW
float power_Percent_8     = 0.0;	// MW
float power_Percent_total = 0.0;	// MW


//float Q_WTemp_post;
//float Q_WTemp_mix_post;
//float thermal_wdl;	// water withdrawals of thermal power stations (m3/s)
//    float thermal_con;	// consumptive water use of thermal power stations (m3/s)
//    float warmingTemp;	// temperature which river water will be warmed (deg C)

	placeHolder			  = MFVarGetFloat (_MDPlaceHolderID,         itemID, 0.0);	// running MDWTempRiverRoute, value is previous water Temp
	flux_QxT			  = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);	// reading in discharge * temp (m3*degC/day)
	fluxmixing_QxT		  = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);	// reading in discharge * tempmixing (m3*degC/day)
//	Q_WTemp				  = MFVarGetFloat (_MDWTemp_QxTID,           itemID, 0.0);	// water temperature entering grid cell (deg C)
//	Q_WTemp_mix			  = MFVarGetFloat (_MDWTempMixing_QxTID,     itemID, 0.0);	// 'mixing only' water temperature entering grid cell (deg C)
  	Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
  	Q_incoming			  = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0);	// already includes local runoff
  	NamePlate1            = MFVarGetFloat (_MDInNamePlate1ID,         itemID, 0.0);
  	NamePlate2            = MFVarGetFloat (_MDInNamePlate2ID,         itemID, 0.0);
  	NamePlate3            = MFVarGetFloat (_MDInNamePlate3ID,         itemID, 0.0);
  	NamePlate4            = MFVarGetFloat (_MDInNamePlate4ID,         itemID, 0.0); 
  	NamePlate5            = MFVarGetFloat (_MDInNamePlate5ID,         itemID, 0.0);
  	NamePlate6            = MFVarGetFloat (_MDInNamePlate6ID,         itemID, 0.0);
  	NamePlate7            = MFVarGetFloat (_MDInNamePlate7ID,         itemID, 0.0);
  	NamePlate8            = MFVarGetFloat (_MDInNamePlate8ID,         itemID, 0.0);
  	  	
  	FuelType1            = MFVarGetFloat (_MDInFuelType1ID,         itemID, 0.0);
  	FuelType2            = MFVarGetFloat (_MDInFuelType2ID,         itemID, 0.0);
  	FuelType3            = MFVarGetFloat (_MDInFuelType3ID,         itemID, 0.0);
  	FuelType4            = MFVarGetFloat (_MDInFuelType4ID,         itemID, 0.0); 
  	FuelType5            = MFVarGetFloat (_MDInFuelType5ID,         itemID, 0.0);
  	FuelType6            = MFVarGetFloat (_MDInFuelType6ID,         itemID, 0.0);
  	FuelType7            = MFVarGetFloat (_MDInFuelType7ID,         itemID, 0.0);
  	FuelType8            = MFVarGetFloat (_MDInFuelType8ID,         itemID, 0.0);
  	  
  	optDeltaT			 = MFVarGetFloat (_MDInOptDeltaTID,   		itemID, 0.0);
  	tempLimit 			 = MFVarGetFloat (_MDInTempLimitID,			itemID, 0.0);

//    QxT                   = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);
//   QxT_mix               = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);
//  thermal_wdl			  = MFVarGetFloat (_MDInThermalWdlID,        itemID, 0.0) * 1000000 / 365 / 24 / 86400;
//	thermal_con			  = MFVarGetFloat (_MDInThermalConID,        itemID, 0.0) * 1000000 / 365 / 24 / 86400;
//	warmingTemp			  = MFVarGetFloat (_MDInWarmingTempID,       itemID, 0.0);

//	if (Q <= thermal_con) {
//		thermal_con = 0.95 * Q;
//		thermal_wdl = 0.05 * Q;
//	}

//	Q_post = Q - thermal_con;			// RJS Not factoring in consumptive water yet
//	Q_post = Q;

//	wdl_QxT = thermal_wdl * warmingTemp;
	
//	QxT_post     = (thermal_wdl * (Q_WTemp + warmingTemp)) + ((Q_post - thermal_wdl) * Q_WTemp);
//	QxT_mix_post = (thermal_wdl * (Q_WTemp_mix + warmingTemp)) + ((Q_post - thermal_wdl) * Q_WTemp_mix);
//	Q_WTemp_post     = QxT_post / Q_post;
//	Q_WTemp_mix_post = QxT_mix_post / Q_post;

	
	if (FuelType1 == 1) optWdl_1 = NamePlate1 * conv_coal;		// coal
	if (FuelType1 == 2) optWdl_1 = NamePlate1 * conv_gas;		// gas
	if (FuelType1 == 3) optWdl_1 = NamePlate1 * conv_oil;		// oil
	if (FuelType1 == 4) optWdl_1 = NamePlate1 * conv_nuc;		// nuclear
	if (FuelType1 == 5) optWdl_1 = NamePlate1 * conv_other;	// other

	if (FuelType2 == 1) optWdl_2 = NamePlate2 * conv_coal;		// coal
	if (FuelType2 == 2) optWdl_2 = NamePlate2 * conv_gas;		// gas
	if (FuelType2 == 3) optWdl_2 = NamePlate2 * conv_oil;		// oil
	if (FuelType2 == 4) optWdl_2 = NamePlate2 * conv_nuc;		// nuclear
	if (FuelType2 == 5) optWdl_2 = NamePlate2 * conv_other;	// other

	if (FuelType3 == 1) optWdl_3 = NamePlate3 * conv_coal;		// coal
	if (FuelType3 == 2) optWdl_3 = NamePlate3 * conv_gas;		// gas
	if (FuelType3 == 3) optWdl_3 = NamePlate3 * conv_oil;		// oil
	if (FuelType3 == 4) optWdl_3 = NamePlate3 * conv_nuc;		// nuclear
	if (FuelType3 == 5) optWdl_3 = NamePlate3 * conv_other;	// other

	if (FuelType4 == 1) optWdl_4 = NamePlate4 * conv_coal;		// coal
	if (FuelType4 == 2) optWdl_4 = NamePlate4 * conv_gas;		// gas
	if (FuelType4 == 3) optWdl_4 = NamePlate4 * conv_oil;		// oil
	if (FuelType4 == 4) optWdl_4 = NamePlate4 * conv_nuc;		// nuclear
	if (FuelType4 == 5) optWdl_4 = NamePlate4 * conv_other;	// other

	if (FuelType5 == 1) optWdl_5 = NamePlate5 * conv_coal;		// coal
	if (FuelType5 == 2) optWdl_5 = NamePlate5 * conv_gas;		// gas
	if (FuelType5 == 3) optWdl_5 = NamePlate5 * conv_oil;		// oil
	if (FuelType5 == 4) optWdl_5 = NamePlate5 * conv_nuc;		// nuclear
	if (FuelType5 == 5) optWdl_5 = NamePlate5 * conv_other;	// other

	if (FuelType6 == 1) optWdl_6 = NamePlate6 * conv_coal;		// coal
	if (FuelType6 == 2) optWdl_6 = NamePlate6 * conv_gas;		// gas
	if (FuelType6 == 3) optWdl_6 = NamePlate6 * conv_oil;		// oil
	if (FuelType6 == 4) optWdl_6 = NamePlate6 * conv_nuc;		// nuclear
	if (FuelType6 == 5) optWdl_6 = NamePlate6 * conv_other;	// other

	if (FuelType7 == 1) optWdl_7 = NamePlate7 * conv_coal;		// coal
	if (FuelType7 == 2) optWdl_7 = NamePlate7 * conv_gas;		// gas
	if (FuelType7 == 3) optWdl_7 = NamePlate7 * conv_oil;		// oil
	if (FuelType7 == 4) optWdl_7 = NamePlate7 * conv_nuc;		// nuclear
	if (FuelType7 == 5) optWdl_7 = NamePlate7 * conv_other;	// other

	if (FuelType8 == 1) optWdl_8 = NamePlate8 * conv_coal;		// coal
	if (FuelType8 == 2) optWdl_8 = NamePlate8 * conv_gas;		// gas
	if (FuelType8 == 3) optWdl_8 = NamePlate8 * conv_oil;		// oil
	if (FuelType8 == 4) optWdl_8 = NamePlate8 * conv_nuc;		// nuclear
	if (FuelType8 == 5) optWdl_8 = NamePlate8 * conv_other;	// other

	
beta1 = NamePlate1 > 0.0 ? NamePlate1 / (NamePlate1 + (optDeltaT * optWdl_1 * 4.186)) : 0.0;
beta2 = NamePlate2 > 0.0 ? NamePlate2 / (NamePlate2 + (optDeltaT * optWdl_2 * 4.186)) : 0.0;
beta3 = NamePlate3 > 0.0 ? NamePlate3 / (NamePlate3 + (optDeltaT * optWdl_3 * 4.186)) : 0.0;
beta4 = NamePlate4 > 0.0 ? NamePlate4 / (NamePlate4 + (optDeltaT * optWdl_4 * 4.186)) : 0.0;
beta5 = NamePlate5 > 0.0 ? NamePlate5 / (NamePlate5 + (optDeltaT * optWdl_5 * 4.186)) : 0.0;
beta6 = NamePlate6 > 0.0 ? NamePlate6 / (NamePlate6 + (optDeltaT * optWdl_6 * 4.186)) : 0.0;
beta7 = NamePlate7 > 0.0 ? NamePlate7 / (NamePlate7 + (optDeltaT * optWdl_7 * 4.186)) : 0.0;
beta8 = NamePlate8 > 0.0 ? NamePlate8 / (NamePlate8 + (optDeltaT * optWdl_8 * 4.186)) : 0.0;

totalWdl	    = 0.0;
totalWdl_m3d    = 0.0;
totalOptWdl	    = 0.0;
totalOptWdl_m3d = 0.0;

if (Q_incoming > 0.000001) {
	
Q_WTemp				 = flux_QxT / (Q_incoming * 86400);			// degC RJS 013112
Q_WTemp_mix			 = fluxmixing_QxT / (Q_incoming * 86400);	// degC RJS 013112

riverTemp_post 		 = Q_WTemp;
riverTempMixing_post = Q_WTemp_mix;

// entering Power plant 1	

if (NamePlate1 > 0.0 ) {
		
	if (Q_WTemp > tempLimit) {
		Q_WTemp_out1 	 = Q_WTemp;
		Q_WTemp_mix_out1 = Q_WTemp_mix;																			//mix #1
		realWdl_1 		 = 0.0;
	}
	
	else {	
		
	realDeltaT_1 	   = optDeltaT;
	temp_effluent1     = Q_WTemp + realDeltaT_1;
	temp_effluent1_mix = Q_WTemp_mix + realDeltaT_1;															//mix #2
	wdl_coeff_1 	   = pow((optDeltaT / realDeltaT_1),(1/29.05));
	realWdl_1  		   = realWdl_1 > Q_incoming ? 0.0 : optWdl_1 * wdl_coeff_1;
	Q_WTemp_out1       = (((Q_incoming - realWdl_1) * Q_WTemp) + (realWdl_1 * temp_effluent1)) / Q_incoming;
	Q_WTemp_mix_out1   = (((Q_incoming - realWdl_1) * Q_WTemp_mix) + (realWdl_1 * temp_effluent1_mix)) / Q_incoming;				//mix #3

	while (Q_WTemp_out1 > tempLimit) {
		
	realDeltaT_1 	 = realDeltaT_1 - 0.5;
	
	if (realDeltaT_1 < 0.01) {
		Q_WTemp_out1 = Q_WTemp;
		Q_WTemp_mix_out1 = Q_WTemp_mix;																			//mix #4
		realWdl_1    = 0.0;
		}
		
	else {
		temp_effluent1   = Q_WTemp + realDeltaT_1;
		temp_effluent1_mix = Q_WTemp_mix + realDeltaT_1;														//mix #5
		wdl_coeff_1 	 = pow((optDeltaT / realDeltaT_1),(1/29.05));
		realWdl_1  		  = realWdl_1 > Q_incoming ? 0.0 : optWdl_1 * wdl_coeff_1;
		Q_WTemp_out1     = (((Q_incoming - realWdl_1) * Q_WTemp) + (realWdl_1 * temp_effluent1)) / Q_incoming;
		Q_WTemp_mix_out1 = (((Q_incoming - realWdl_1) * Q_WTemp_mix) + (realWdl_1 * temp_effluent1_mix)) / Q_incoming;			//mix #6
	}
	
	}
	
	}
	
}

riverTemp_post 		 = NamePlate1 > 0.0 ? Q_WTemp_out1 : Q_WTemp;
riverTempMixing_post = NamePlate1 > 0.0 ? Q_WTemp_mix_out1: Q_WTemp_mix;										//mix #7
Q_WTemp_out1		 = riverTemp_post;
Q_WTemp_mix_out1	 = riverTempMixing_post;
totalWdl	   		 = totalWdl + realWdl_1;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_1;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 2	
	
if (NamePlate2 > 0.0 ) {
	
	if (Q_WTemp_out1 > tempLimit) {
		Q_WTemp_out2 = Q_WTemp_out1;
		Q_WTemp_mix_out2 = Q_WTemp_mix_out1;																	//mix #1
		realWdl_2 	 = 0.0;
	}
	
	else {		
		
	realDeltaT_2 	   = optDeltaT;
	temp_effluent2     = Q_WTemp_out1 + realDeltaT_2;
	temp_effluent1_mix = Q_WTemp_mix_out1 + realDeltaT_2;														//mix #2
	wdl_coeff_2 	   = pow((optDeltaT / realDeltaT_2),(1/29.05));
	realWdl_2  		   = realWdl_2 > Q_incoming ? 0.0 : optWdl_2 * wdl_coeff_2;
	Q_WTemp_out2       = (((Q_incoming - realWdl_2) * Q_WTemp_out1) + (realWdl_2 * temp_effluent2)) / Q_incoming;
	Q_WTemp_mix_out2   = (((Q_incoming - realWdl_2) * Q_WTemp_mix_out1) + (realWdl_2 * temp_effluent2_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out2 > tempLimit) {
		
	realDeltaT_2 	 = realDeltaT_2 - 0.5;
	
	if (realDeltaT_2 < 0.01) {
		Q_WTemp_out2     = Q_WTemp_out1;
		Q_WTemp_mix_out2 = Q_WTemp_mix_out1;																	//mix #4
		realWdl_2        = 0.0;
		}
		
	else {
		temp_effluent2     = Q_WTemp_out1 + realDeltaT_2;
		temp_effluent2_mix = Q_WTemp_mix_out1 + realDeltaT_2;													//mix #5
		wdl_coeff_2 	   = pow((optDeltaT / realDeltaT_2),(1/29.05));
		realWdl_2  		   = realWdl_2 > Q_incoming ? 0.0 : optWdl_2 * wdl_coeff_2;
		Q_WTemp_out2       = (((Q_incoming - realWdl_2) * Q_WTemp_out1) + (realWdl_2 * temp_effluent2)) / Q_incoming;
		Q_WTemp_mix_out2   = (((Q_incoming - realWdl_2) * Q_WTemp_mix_out1) + (realWdl_2 * temp_effluent2_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	

riverTemp_post       = NamePlate2 > 0.0 ? Q_WTemp_out2 : riverTemp_post;
riverTempMixing_post = NamePlate2 > 0.0 ? Q_WTemp_mix_out2: riverTempMixing_post;								//mix #7
Q_WTemp_out2		 = riverTemp_post;
Q_WTemp_mix_out2	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_2;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_2;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 3	
	
if (NamePlate3 > 0.0 ) {
	
	if (Q_WTemp_out2 > tempLimit) {
		Q_WTemp_out3     = Q_WTemp_out2;
		Q_WTemp_mix_out3 = Q_WTemp_mix_out2;																	//mix #1
		realWdl_3 	 	 = 0.0;
	}
	
	else {	
	
	realDeltaT_3 	   = optDeltaT;
	temp_effluent3     = Q_WTemp_out2 + realDeltaT_3;
	temp_effluent3_mix = Q_WTemp_mix_out2 + realDeltaT_3;														//mix #2
	wdl_coeff_3 	   = pow((optDeltaT / realDeltaT_3),(1/29.05));
	realWdl_3  		   = realWdl_3 > Q_incoming ? 0.0 : optWdl_3 * wdl_coeff_3;
	Q_WTemp_out3       = (((Q_incoming - realWdl_3) * Q_WTemp_out2) + (realWdl_3 * temp_effluent3)) / Q_incoming;
	Q_WTemp_mix_out3   = (((Q_incoming - realWdl_3) * Q_WTemp_mix_out2) + (realWdl_3 * temp_effluent3_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out3 > tempLimit) {
		
	realDeltaT_3 	 = realDeltaT_3 - 0.5;
	
	if (realDeltaT_3 < 0.01) {
		Q_WTemp_out3     = Q_WTemp_out2;
		Q_WTemp_mix_out3 = Q_WTemp_mix_out2;																	//mix #4
		realWdl_3        = 0.0;
		}
		
	else {
		temp_effluent3     = Q_WTemp_out2 + realDeltaT_3;
		temp_effluent3_mix = Q_WTemp_mix_out2 + realDeltaT_3;													//mix #5
		wdl_coeff_3 	   = pow((optDeltaT / realDeltaT_3),(1/29.05));
		realWdl_3  	       = realWdl_3 > Q_incoming ? 0.0 : optWdl_3 * wdl_coeff_3;
		Q_WTemp_out3       = (((Q_incoming - realWdl_3) * Q_WTemp_out2) + (realWdl_3 * temp_effluent3)) / Q_incoming;
		Q_WTemp_mix_out3   = (((Q_incoming - realWdl_3) * Q_WTemp_mix_out2) + (realWdl_3 * temp_effluent3_mix)) / Q_incoming;		//mix #6
	}
	
	}

	}

}	
	
	
riverTemp_post       = NamePlate3 > 0.0 ? Q_WTemp_out3 : riverTemp_post;
riverTempMixing_post = NamePlate3 > 0.0 ? Q_WTemp_mix_out3: riverTempMixing_post;								//mix #7
Q_WTemp_out3		 = riverTemp_post;
Q_WTemp_mix_out3	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_3;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_3;
totalOptWdl_m3d		 = (totalOptWdl * 86400);


// entering Power plant 4	
	
if (NamePlate4 > 0.0 ) {
	
	if (Q_WTemp_out3 > tempLimit) {
		Q_WTemp_out4     = Q_WTemp_out3;
		Q_WTemp_mix_out4 = Q_WTemp_mix_out3;																	//mix #1
		realWdl_4 	     = 0.0;
	}
	
	else {	
	
	realDeltaT_4 	   = optDeltaT;
	temp_effluent4     = Q_WTemp_out3 + realDeltaT_4;
	temp_effluent4_mix = Q_WTemp_mix_out3 + realDeltaT_4;														//mix #2
	wdl_coeff_4 	   = pow((optDeltaT / realDeltaT_4),(1/29.05));
	realWdl_4  		   = realWdl_4 > Q_incoming ? 0.0 : optWdl_4 * wdl_coeff_4;
	Q_WTemp_out4       = (((Q_incoming - realWdl_4) * Q_WTemp_out3) + (realWdl_4 * temp_effluent4)) / Q_incoming;
	Q_WTemp_mix_out4   = (((Q_incoming - realWdl_4) * Q_WTemp_mix_out3) + (realWdl_4 * temp_effluent4_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out4 > tempLimit) {
		
	realDeltaT_4 	 = realDeltaT_4 - 0.5;
	
	if (realDeltaT_4 < 0.01) {
		Q_WTemp_out4     = Q_WTemp_out3;
		Q_WTemp_mix_out4 = Q_WTemp_mix_out3;																	//mix #4
		realWdl_4        = 0.0;
		}
		
	else {
		temp_effluent4     = Q_WTemp_out3 + realDeltaT_4;
		temp_effluent4_mix = Q_WTemp_mix_out3 + realDeltaT_4;													//mix #5
		wdl_coeff_4 	   = pow((optDeltaT / realDeltaT_4),(1/29.05));
		realWdl_4  		   = realWdl_4 > Q_incoming ? 0.0 : optWdl_4 * wdl_coeff_4;
		Q_WTemp_out4       = (((Q_incoming - realWdl_4) * Q_WTemp_out3) + (realWdl_4 * temp_effluent4)) / Q_incoming;
		Q_WTemp_mix_out4   = (((Q_incoming - realWdl_4) * Q_WTemp_mix_out3) + (realWdl_4 * temp_effluent4_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	
	
		
riverTemp_post       = NamePlate4 > 0.0 ? Q_WTemp_out4 : riverTemp_post;
riverTempMixing_post = NamePlate4 > 0.0 ? Q_WTemp_mix_out4: riverTempMixing_post;								//mix #7
Q_WTemp_out4		 = riverTemp_post;
Q_WTemp_mix_out4	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_4;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_4;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 5	
	
if (NamePlate5 > 0.0 ) {
	
	if (Q_WTemp_out4 > tempLimit) {
		Q_WTemp_out5     = Q_WTemp_out4;
		Q_WTemp_mix_out5 = Q_WTemp_mix_out4;																	// mix #1
		realWdl_5 	     = 0.0;
	}
	
	else {	
	
	realDeltaT_5 	   = optDeltaT;
	temp_effluent5     = Q_WTemp_out4 + realDeltaT_5;
	temp_effluent5_mix = Q_WTemp_mix_out4 + realDeltaT_5;														//mix #2
	wdl_coeff_5 	   = pow((optDeltaT / realDeltaT_5),(1/29.05));
	realWdl_5  		   = realWdl_5 > Q_incoming ? 0.0 : optWdl_5 * wdl_coeff_5;
	Q_WTemp_out5       = (((Q_incoming - realWdl_5) * Q_WTemp_out4) + (realWdl_5 * temp_effluent5)) / Q_incoming;
	Q_WTemp_mix_out5   = (((Q_incoming - realWdl_5) * Q_WTemp_mix_out4) + (realWdl_5 * temp_effluent5_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out5 > tempLimit) {
	
	realDeltaT_5 	 = realDeltaT_5 - 0.5;
	
	if (realDeltaT_5 < 0.01) {
		Q_WTemp_out5     = Q_WTemp_out4;
		Q_WTemp_mix_out5 = Q_WTemp_mix_out4;																	//mix #4
		realWdl_5        = 0.0;
		}
		
	else {
		temp_effluent5     = Q_WTemp_out4 + realDeltaT_5;
		temp_effluent5_mix = Q_WTemp_mix_out4 + realDeltaT_4;													//mix #5
		wdl_coeff_5 	   = pow((optDeltaT / realDeltaT_5),(1/29.05));
		realWdl_5  		   = realWdl_5 > Q_incoming ? 0.0 : optWdl_5 * wdl_coeff_5;
		Q_WTemp_out5       = (((Q_incoming - realWdl_5) * Q_WTemp_out4) + (realWdl_5 * temp_effluent5)) / Q_incoming;
		Q_WTemp_mix_out5   = (((Q_incoming - realWdl_5) * Q_WTemp_mix_out4) + (realWdl_5 * temp_effluent5_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	

riverTemp_post       = NamePlate5 > 0.0 ? Q_WTemp_out5 : riverTemp_post;
riverTempMixing_post = NamePlate5 > 0.0 ? Q_WTemp_mix_out5: riverTempMixing_post;								//mix #7
Q_WTemp_out5		 = riverTemp_post;
Q_WTemp_mix_out5	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_5;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_5;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 6	
	
if (NamePlate6 > 0.0 ) {
	
	if (Q_WTemp_out5 > tempLimit) {
		Q_WTemp_out6     = Q_WTemp_out5;
		Q_WTemp_mix_out6 = Q_WTemp_mix_out5;																	//mix #1
		realWdl_6 	     = 0.0;
	}
	
	else {	
	
	realDeltaT_6 	   = optDeltaT;
	temp_effluent6     = Q_WTemp_out5 + realDeltaT_6;
	temp_effluent6_mix = Q_WTemp_mix_out5 + realDeltaT_6;														//mix #2
	wdl_coeff_6 	   = pow((optDeltaT / realDeltaT_6),(1/29.05));
	realWdl_6  		   = realWdl_6 > Q_incoming ? 0.0 : optWdl_6 * wdl_coeff_6;
	Q_WTemp_out6       = (((Q_incoming - realWdl_6) * Q_WTemp_out5) + (realWdl_6 * temp_effluent6)) / Q_incoming;
	Q_WTemp_mix_out6   = (((Q_incoming - realWdl_6) * Q_WTemp_mix_out5) + (realWdl_6 * temp_effluent6_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out6 > tempLimit) {
	
	realDeltaT_6 	 = realDeltaT_6 - 0.5;
	
	if (realDeltaT_6 < 0.01) {
		Q_WTemp_out6     = Q_WTemp_out5;
		Q_WTemp_mix_out6 = Q_WTemp_mix_out5;																	//mix #4
		realWdl_6        = 0.0;
		}
		
	else {
		temp_effluent6     = Q_WTemp_out5 + realDeltaT_6;
		temp_effluent6_mix = Q_WTemp_mix_out5 + realDeltaT_6;													//mix #5
		wdl_coeff_6 	   = pow((optDeltaT / realDeltaT_6),(1/29.05));
		realWdl_6  		   = realWdl_6 > Q_incoming ? 0.0 : optWdl_6 * wdl_coeff_6;
		Q_WTemp_out6       = (((Q_incoming - realWdl_6) * Q_WTemp_out5) + (realWdl_6 * temp_effluent6)) / Q_incoming;
		Q_WTemp_mix_out6   = (((Q_incoming - realWdl_6) * Q_WTemp_mix_out5) + (realWdl_6 * temp_effluent6_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	
	
riverTemp_post       = NamePlate6 > 0.0 ? Q_WTemp_out6 : riverTemp_post;
riverTempMixing_post = NamePlate6 > 0.0 ? Q_WTemp_mix_out6: riverTempMixing_post;								//mix #7
Q_WTemp_out6		 = riverTemp_post;
Q_WTemp_mix_out6	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_6;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_6;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 7	
	
if (NamePlate7 > 0.0 ) {
	
	if (Q_WTemp_out6 > tempLimit) {
		Q_WTemp_out7     = Q_WTemp_out6;
		Q_WTemp_mix_out7 = Q_WTemp_mix_out6;																	//mix #1
		realWdl_7 	     = 0.0;
	}
	
	else {	
	
	realDeltaT_7 	   = optDeltaT;
	temp_effluent7     = Q_WTemp_out6 + realDeltaT_7;
	temp_effluent7_mix = Q_WTemp_mix_out6 + realDeltaT_7;														//mix #2
	wdl_coeff_7 	   = pow((optDeltaT / realDeltaT_7),(1/29.05));
	realWdl_7  		   = realWdl_7 > Q_incoming ? 0.0 : optWdl_7 * wdl_coeff_7;
	Q_WTemp_out7       = (((Q_incoming - realWdl_7) * Q_WTemp_out6) + (realWdl_7 * temp_effluent7)) / Q_incoming;
	Q_WTemp_mix_out7   = (((Q_incoming - realWdl_7) * Q_WTemp_mix_out6) + (realWdl_7 * temp_effluent7_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out7 > tempLimit) {
	
	realDeltaT_7 	 = realDeltaT_7 - 0.5;
	
	if (realDeltaT_7 < 0.01) {
		Q_WTemp_out7     = Q_WTemp_out6;
		Q_WTemp_mix_out7 = Q_WTemp_mix_out6;																	//mix #4
		realWdl_7        = 0.0;
		}
		
	else {
		temp_effluent7     = Q_WTemp_out6 + realDeltaT_7;
		temp_effluent7_mix = Q_WTemp_mix_out6 + realDeltaT_7;													//mix #5
		wdl_coeff_7 	   = pow((optDeltaT / realDeltaT_7),(1/29.05));
		realWdl_7  		   = realWdl_7 > Q_incoming ? 0.0 : optWdl_7 * wdl_coeff_7;
		Q_WTemp_out7       = (((Q_incoming - realWdl_7) * Q_WTemp_out6) + (realWdl_7 * temp_effluent7)) / Q_incoming;
		Q_WTemp_mix_out7   = (((Q_incoming - realWdl_7) * Q_WTemp_mix_out6) + (realWdl_7 * temp_effluent7_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	
	
	
riverTemp_post       = NamePlate7 > 0.0 ? Q_WTemp_out7 : riverTemp_post;
riverTempMixing_post = NamePlate7 > 0.0 ? Q_WTemp_mix_out7: riverTempMixing_post;								//mix #7
Q_WTemp_out7		 = riverTemp_post;
Q_WTemp_mix_out7	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_7;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_7;
totalOptWdl_m3d		 = (totalOptWdl * 86400);

// entering Power plant 8	
	
if (NamePlate8 > 0.0 ) {
	
	if (Q_WTemp_out7 > tempLimit) {
		Q_WTemp_out8     = Q_WTemp_out7;
		Q_WTemp_mix_out8 = Q_WTemp_mix_out7;																	//mix #1
		realWdl_8 	     = 0.0;
	}
	
	else {	
	
	realDeltaT_8 	   = optDeltaT;
	temp_effluent8     = Q_WTemp_out7 + realDeltaT_8;
	temp_effluent8_mix = Q_WTemp_mix_out7 + realDeltaT_8;														//mix #2
	wdl_coeff_8 	   = pow((optDeltaT / realDeltaT_8),(1/29.05));
	realWdl_8  		   = realWdl_8 > Q_incoming ? 0.0 : optWdl_8 * wdl_coeff_8;
	Q_WTemp_out8       = (((Q_incoming - realWdl_8) * Q_WTemp_out7) + (realWdl_8 * temp_effluent8)) / Q_incoming;
	Q_WTemp_mix_out8   = (((Q_incoming - realWdl_8) * Q_WTemp_mix_out7) + (realWdl_8 * temp_effluent8_mix)) / Q_incoming;			//mix #3
	
	while (Q_WTemp_out8 > tempLimit) {
	
	realDeltaT_8 	 = realDeltaT_8 - 0.5;
	
	if (realDeltaT_8 < 0.01) {
		Q_WTemp_out8     = Q_WTemp_out7;
		Q_WTemp_mix_out8 = Q_WTemp_mix_out7;																	//mix #4
		realWdl_8   	 = 0.0;
		}
		
	else {
		temp_effluent8     = Q_WTemp_out7 + realDeltaT_8;
		temp_effluent8_mix = Q_WTemp_mix_out7 + realDeltaT_8;													//mix #5
		wdl_coeff_8 	   = pow((optDeltaT / realDeltaT_8),(1/29.05));
		realWdl_8  		   = realWdl_8 > Q_incoming ? 0.0 : optWdl_8 * wdl_coeff_8;
		Q_WTemp_out8       = (((Q_incoming - realWdl_8) * Q_WTemp_out7) + (realWdl_8 * temp_effluent8)) / Q_incoming;
		Q_WTemp_mix_out8   = (((Q_incoming - realWdl_8) * Q_WTemp_mix_out7) + (realWdl_8 * temp_effluent8_mix)) / Q_incoming;		//mix #6
	}
	
	}
	
	}

}	
	
	
riverTemp_post       = NamePlate8 > 0.0 ? Q_WTemp_out8 : riverTemp_post;
riverTempMixing_post = NamePlate8 > 0.0 ? Q_WTemp_mix_out8: riverTempMixing_post;								//mix #7
Q_WTemp_out8		 = riverTemp_post;
Q_WTemp_mix_out8	 = riverTempMixing_post;
totalWdl	         = totalWdl + realWdl_8;
totalWdl_m3d		 = (totalWdl * 86400);
totalOptWdl			 = totalOptWdl + optWdl_8;
totalOptWdl_m3d		 = (totalOptWdl * 86400);


flux_QxT_new   	   = riverTemp_post * Q_incoming * 86400.0;
fluxmixing_QxT_new = riverTempMixing_post * Q_incoming * 86400.0;

} // end if Q_incoming > 0.000001

else {

	riverTemp_post       = 0.0;
	riverTempMixing_post = 0.0;
	totalWdl             = 0.0;
	totalWdl_m3d		 = 0.0;
	totalOptWdl			 = optWdl_1 + optWdl_2 + optWdl_3 + optWdl_4 + optWdl_5 + optWdl_6 + optWdl_7 + optWdl_8;
	totalOptWdl_m3d		 = (totalOptWdl * 86400);
	flux_QxT_new         = 0.0;
	fluxmixing_QxT_new   = 0.0;
	
}


// powerplant energy generation

if (NamePlate1 > 0.0) {	
	
	power_Output_1 = (-realWdl_1 * 4.186 * realDeltaT_1) / (1.0 - (1.0 / beta1)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_1 = (NamePlate1 * 24) - power_Output_1;
	power_Percent_1 = (power_Output_1 / (power_Deficit_1 + power_Output_1)) * 100;

}

//if (itemID == 15) printf("Output_1 = %f, Deficit_1 = %f, Percent_1 = %f, realWdl_1 = %f, realDeltaT_1 = %f, beta1 = %f\n", power_Output_1, power_Deficit_1, power_Percent_1, realWdl_1, realDeltaT_1, beta1);

if (NamePlate2 > 0.0) {

	power_Output_2 = (-realWdl_2 * 4.186 * realDeltaT_2) / (1.0 - (1.0 / beta2)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_2 = (NamePlate2 * 24) - power_Output_2;
	power_Percent_2 = (power_Output_2 / (power_Deficit_2 + power_Output_2)) * 100;

}

//if (itemID == 15) printf("Output_2 = %f, Deficit_2 = %f, Percent_2 = %f, realWdl_2 = %f, realDeltaT_2 = %f, beta2 = %f\n", power_Output_2, power_Deficit_2, power_Percent_2, realWdl_2, realDeltaT_2, beta2);

if (NamePlate3 > 0.0) {

	power_Output_3 = (-realWdl_3 * 4.186 * realDeltaT_3) / (1.0 - (1.0 / beta3)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_3 = (NamePlate3 * 24) - power_Output_3;
	power_Percent_3 = (power_Output_3 / (power_Deficit_3 + power_Output_3)) * 100;

}

//if (itemID == 15) printf("Output_3 = %f, Deficit_3 = %f, Percent_3 = %f, beta3 = %f\n", power_Output_3, power_Deficit_3, power_Percent_3, beta3);

if (NamePlate4 > 0.0) {

	power_Output_4 = (-realWdl_4 * 4.186 * realDeltaT_4) / (1.0 - (1.0 / beta4)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_4 = (NamePlate4 * 24) - power_Output_4;
	power_Percent_4 = (power_Output_4 / (power_Deficit_4 + power_Output_4)) * 100;

}

//if (itemID == 15) printf("Output_4 = %f, Deficit_4 = %f, Percent_4 = %f, beta4 = %f\n", power_Output_4, power_Deficit_4, power_Percent_4, beta4);

if (NamePlate5 > 0.0) {

	power_Output_5 = (-realWdl_5 * 4.186 * realDeltaT_5) / (1.0 - (1.0 / beta5)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_5 = (NamePlate5 * 24) - power_Output_5;
	power_Percent_5 = (power_Output_5 / (power_Deficit_5 + power_Output_5)) * 100;
	
}

//if (itemID == 15) printf("Output_5 = %f, Deficit_5 = %f, Percent_5 = %f, beta5 = %f\n", power_Output_5, power_Deficit_5, power_Percent_5, beta5);

if (NamePlate6 > 0.0) {

	power_Output_6 = (-realWdl_6 * 4.186 * realDeltaT_6) / (1.0 - (1.0 / beta6)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_6 = (NamePlate6 * 24) - power_Output_6;
	power_Percent_6 = (power_Output_6 / (power_Deficit_6 + power_Output_6)) * 100;

}

//if (itemID == 15) printf("Output_6 = %f, Deficit_6 = %f, Percent_6 = %f, beta6 = %f\n", power_Output_6, power_Deficit_6, power_Percent_6, beta6);

if (NamePlate7 > 0.0) {
	
	power_Output_7 = (-realWdl_7 * 4.186 * realDeltaT_7) / (1.0 - (1.0 / beta7)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_7 = (NamePlate7 * 24) - power_Output_7;
	power_Percent_7 = (power_Output_7 / (power_Deficit_7 + power_Output_7)) * 100;

}

//if (itemID == 15) printf("Output_7 = %f, Deficit_7 = %f, Percent_7 = %f, beta7 = %f\n", power_Output_7, power_Deficit_7, power_Percent_7, beta7);

if (NamePlate8 > 0.0) {
	
	power_Output_8 = (-realWdl_8 * 4.186 * realDeltaT_8) / (1.0 - (1.0 / beta8)) * 24;	//4.186 is thermal heat capacity, MWh (for the day)
	power_Deficit_8 = (NamePlate8 * 24) - power_Output_8;
	power_Percent_8 = (power_Output_8 / (power_Deficit_8 + power_Output_8)) * 100;
	
}

//if (itemID == 15) printf("Output_8 = %f, Deficit_8 = %f, Percent_8 = %f, beta8 = %f\n", power_Output_8, power_Deficit_8, power_Percent_8, beta8);

power_Output_total = power_Output_1 + power_Output_2 + power_Output_3 + power_Output_4 + power_Output_5 + power_Output_6 + power_Output_7 + power_Output_8;  
power_Deficit_total = power_Deficit_1 + power_Deficit_2 + power_Deficit_3 + power_Deficit_4 + power_Deficit_5 + power_Deficit_6 + power_Deficit_7 + power_Deficit_8;
power_Percent_total = power_Output_total / (power_Output_total + power_Deficit_total);

//if (itemID == 15) {
//	printf("*** itemID = %d, Y = %d, M = %d, D = %d, Q_incoming = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q_incoming);
//	printf("1 = %f, 2 = %f, 3 = %f, 4 = %f, 5 = %f, 6 = %f, 7 = %f, 8 = %f\n", NamePlate1, NamePlate2, NamePlate3, NamePlate4, NamePlate5, NamePlate6, NamePlate7, NamePlate8);
//	printf("Output_total = %f, Deficit_total = %f, Percent_total = %f\n", power_Output_total, power_Deficit_total, power_Percent_total);
//	printf("#1 - placeHolder = %f, Q_WTemp = %f, Q_WTemp_out1 = %f, realDeltaT_1 = %f, temp_effluent1 = %f\n", placeHolder, Q_WTemp, Q_WTemp_out1, realDeltaT_1, temp_effluent1);
//	printf("#1 - optWdl_1 = %f, realWdl_1 = %f, wdl_coeff_1 = %f\n", optWdl_1, realWdl_1, wdl_coeff_1);
//	printf("#2 - Q_WTemp_out1 = %f, Q_WTemp_out2 = %f, realDeltaT_2 = %f, temp_effluent2 = %f\n", Q_WTemp_out1, Q_WTemp_out2, realDeltaT_2, temp_effluent2);
//	printf("#2 - optWdl_2 = %f, realWdl_2 = %f, wdl_coeff_2 = %f\n", optWdl_2, realWdl_2, wdl_coeff_2);
//	printf("#3 - Q_WTemp_out2 = %f, Q_WTemp_out3 = %f, realDeltaT_3 = %f, temp_effluent3 = %f\n", Q_WTemp_out2, Q_WTemp_out3, realDeltaT_3, temp_effluent3);
//	printf("#3 - optWdl_3 = %f, realWdl_3 = %f, wdl_coeff_3 = %f\n", optWdl_3, realWdl_3, wdl_coeff_3);
//}

	 MFVarSetFloat(_MDWTemp_QxTID,            itemID, riverTemp_post);
	 MFVarSetFloat(_MDFlux_QxTID,             itemID, flux_QxT_new);
	 MFVarSetFloat(_MDFluxMixing_QxTID,       itemID, fluxmixing_QxT_new);
	 MFVarSetFloat(_MDOutTotalThermalWdlsID,  itemID, totalWdl_m3d);
	 MFVarSetFloat(_MDOutTotalOptThermalWdlsID, itemID, totalOptWdl_m3d);
	 MFVarSetFloat(_MDOutPowerOutput1ID,      itemID, power_Output_1);
	 MFVarSetFloat(_MDOutPowerOutput2ID,      itemID, power_Output_2);
	 MFVarSetFloat(_MDOutPowerOutput3ID,      itemID, power_Output_3);
	 MFVarSetFloat(_MDOutPowerOutput4ID,      itemID, power_Output_4);
	 MFVarSetFloat(_MDOutPowerOutput5ID,      itemID, power_Output_5);
	 MFVarSetFloat(_MDOutPowerOutput6ID,      itemID, power_Output_6);
	 MFVarSetFloat(_MDOutPowerOutput7ID,      itemID, power_Output_7);
	 MFVarSetFloat(_MDOutPowerOutput8ID,      itemID, power_Output_8);
	 MFVarSetFloat(_MDOutPowerDeficit1ID,     itemID, power_Deficit_1);
	 MFVarSetFloat(_MDOutPowerDeficit2ID,     itemID, power_Deficit_2);
	 MFVarSetFloat(_MDOutPowerDeficit3ID,     itemID, power_Deficit_3);
	 MFVarSetFloat(_MDOutPowerDeficit4ID,     itemID, power_Deficit_4);
	 MFVarSetFloat(_MDOutPowerDeficit5ID,     itemID, power_Deficit_5);
	 MFVarSetFloat(_MDOutPowerDeficit6ID,     itemID, power_Deficit_6);
	 MFVarSetFloat(_MDOutPowerDeficit7ID,     itemID, power_Deficit_7);
	 MFVarSetFloat(_MDOutPowerDeficit8ID,     itemID, power_Deficit_8);
	 MFVarSetFloat(_MDOutPowerPercent1ID,     itemID, power_Percent_1);
	 MFVarSetFloat(_MDOutPowerPercent2ID,     itemID, power_Percent_2);
	 MFVarSetFloat(_MDOutPowerPercent3ID,     itemID, power_Percent_3);
	 MFVarSetFloat(_MDOutPowerPercent4ID,     itemID, power_Percent_4);
	 MFVarSetFloat(_MDOutPowerPercent5ID,     itemID, power_Percent_5);
	 MFVarSetFloat(_MDOutPowerPercent6ID,     itemID, power_Percent_6);
	 MFVarSetFloat(_MDOutPowerPercent7ID,     itemID, power_Percent_7);
	 MFVarSetFloat(_MDOutPowerPercent8ID,     itemID, power_Percent_8);
	 MFVarSetFloat(_MDOutPowerOutputTotalID,  itemID, power_Output_total);
	 MFVarSetFloat(_MDOutPowerDeficitTotalID, itemID, power_Deficit_total);
	 MFVarSetFloat(_MDOutPowerPercentTotalID, itemID, power_Percent_total);

//	 MFVarSetFloat(_MDWTempMixing_QxTID, itemID, Q_WTemp_mix_post);
//	 MFVarSetFloat(_MDOutWdl_QxTID,        itemID, wdl_QxT);
//	 MFVarSetFloat(_MDFluxMixing_QxTID,  itemID, QxT_mix_post);
//	 MFVarSetFloat(_MDInDischargeID,     itemID, Q_post);

}


enum { MDnone, MDinput };

int MDThermalInputsDef () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptThermalInputs;
	const char *options [] = { MDNoneStr, MDInputStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Thermal Inputs");
	switch (optID) {
		case MDinput:
			if (((_MDPlaceHolderID          = MDWTempRiverRouteDef ()) == CMfailed) ||
				((_MDInDischargeID          = MDDischLevel2Def ()) == CMfailed) ||
				((_MDInDischargeIncomingID  = MFVarGetID (MDVarDischarge0,          "m3/s", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDFluxMixing_QxTID       = MFVarGetID (MDVarFluxMixing_QxT, "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
				((_MDFlux_QxTID             = MFVarGetID (MDVarFlux_QxT,       "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
				((_MDInNamePlate1ID         = MFVarGetID (MDVarNamePlate1,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate2ID         = MFVarGetID (MDVarNamePlate2,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate3ID         = MFVarGetID (MDVarNamePlate3,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate4ID         = MFVarGetID (MDVarNamePlate4,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate5ID         = MFVarGetID (MDVarNamePlate5,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate6ID         = MFVarGetID (MDVarNamePlate6,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate7ID         = MFVarGetID (MDVarNamePlate7,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate8ID         = MFVarGetID (MDVarNamePlate8,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType1ID          = MFVarGetID (MDVarFuelType1,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType2ID          = MFVarGetID (MDVarFuelType2,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType3ID          = MFVarGetID (MDVarFuelType3,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType4ID          = MFVarGetID (MDVarFuelType4,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType5ID          = MFVarGetID (MDVarFuelType5,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType6ID          = MFVarGetID (MDVarFuelType6,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType7ID          = MFVarGetID (MDVarFuelType7,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType8ID          = MFVarGetID (MDVarFuelType8,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInOptDeltaTID          = MFVarGetID (MDVarOptDeltaT,          "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimitID          = MFVarGetID (MDVarTempLimit,          "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutTotalThermalWdlsID  = MFVarGetID (MDVarTotalThermalWdls, "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalOptThermalWdlsID = MFVarGetID (MDVarTotalOptThermalWdls, "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDWTempMixing_QxTID      = MFVarGetID (MDVarWTempMixing_QxT,   "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDWTemp_QxTID            = MFVarGetID (MDVarWTemp_QxT,         "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 013112
				((_MDOutPowerOutput1ID      = MFVarGetID (MDVarPowerOutput1,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput2ID      = MFVarGetID (MDVarPowerOutput2,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput3ID      = MFVarGetID (MDVarPowerOutput3,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput4ID      = MFVarGetID (MDVarPowerOutput4,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput5ID      = MFVarGetID (MDVarPowerOutput5,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput6ID      = MFVarGetID (MDVarPowerOutput6,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput7ID      = MFVarGetID (MDVarPowerOutput7,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput8ID      = MFVarGetID (MDVarPowerOutput8,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit1ID     = MFVarGetID (MDVarPowerDeficit1,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit2ID     = MFVarGetID (MDVarPowerDeficit2,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit3ID     = MFVarGetID (MDVarPowerDeficit3,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit4ID     = MFVarGetID (MDVarPowerDeficit4,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit5ID     = MFVarGetID (MDVarPowerDeficit5,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit6ID     = MFVarGetID (MDVarPowerDeficit6,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit7ID     = MFVarGetID (MDVarPowerDeficit7,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit8ID     = MFVarGetID (MDVarPowerDeficit8,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent1ID     = MFVarGetID (MDVarPowerPercent1,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent2ID     = MFVarGetID (MDVarPowerPercent2,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent3ID     = MFVarGetID (MDVarPowerPercent3,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent4ID     = MFVarGetID (MDVarPowerPercent4,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent5ID     = MFVarGetID (MDVarPowerPercent5,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent6ID     = MFVarGetID (MDVarPowerPercent6,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent7ID     = MFVarGetID (MDVarPowerPercent7,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent8ID     = MFVarGetID (MDVarPowerPercent8,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutputTotalID  = MFVarGetID (MDVarPowerOutputTotal,    "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficitTotalID = MFVarGetID (MDVarPowerDeficitTotal,   "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercentTotalID = MFVarGetID (MDVarPowerDeficitTotal,   "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||

//			    ((_MDInThermalWdlID     = MFVarGetID (MDVarThermalWdl,            "m3", MFInput, MFState, MFBoundary)) == CMfailed) ||
//			    ((_MDOutWdl_QxTID	    = MFVarGetID (MDVarWithdrawal_QxT,	      "m3*degC/d", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
//			    ((_MDInWarmingTempID    = MFVarGetID (MDVarWarmingTemp,            "degC", MFInput, MFState, MFBoundary)) == CMfailed)	||

			    (MFModelAddFunction (_MDThermalInputs) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Thermal Inputs");
	return (_MDWTemp_QxTID);
}

