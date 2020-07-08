/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDThermalInputs2.c

amiara00@CCNY.cuny.edu & rob.stewart@unh.edu

Thermal Inputs, withdrawals, and energy of thermoelectric plants (second iteration)
*******************************************************************************/

#include <string.h>
#include <MF.h>
#include <MD.h>
#include <math.h>

// Input
static int _MDInDischargeID         = MFUnset;
static int _MDInDischargeIncomingID = MFUnset;
static int _MDFlux_QxTID		    = MFUnset;
static int _MDFluxMixing_QxTID	    = MFUnset;
static int _MDPlaceHolderID			= MFUnset;

static int _MDInApproachID			= MFUnset;
static int _MDInBypassPercentID		= MFUnset;
static int _MDInRiverThreshTempID	= MFUnset;
static int _MDInStateID				= MFUnset;

static int _MDInPlantCode1ID	= MFUnset;
static int _MDInNamePlate1ID	= MFUnset;
static int _MDInFuelType1ID		= MFUnset;
static int _MDInTechnology1ID	= MFUnset;
static int _MDInEfficiency1ID	= MFUnset;
static int _MDInLakeOcean1ID	= MFUnset;  // there may be more of these
static int _MDInEnergyDemand1ID	= MFUnset;

static int _MDInTempLimit_CTID		= MFUnset;
static int _MDInTempLimit_DEID		= MFUnset;
static int _MDInTempLimit_MAID		= MFUnset;
static int _MDInTempLimit_MDID		= MFUnset;
static int _MDInTempLimit_MEID		= MFUnset;
static int _MDInTempLimit_NHID		= MFUnset;
static int _MDInTempLimit_NJID		= MFUnset;
static int _MDInTempLimit_NYID		= MFUnset;
static int _MDInTempLimit_PAID		= MFUnset;
static int _MDInTempLimit_RIID		= MFUnset;
static int _MDInTempLimit_VAID		= MFUnset;
static int _MDInTempLimit_VTID		= MFUnset;
static int _MDInWetBulbTempID	  = MFUnset;


// Output
static int _MDWTemp_QxTID            = MFUnset;
//static int _MDWTempMixing_QxTID      = MFUnset;

static int _MDOutDischargeID		   = MFUnset;		// Late-night discharge test
static int _MDOutAvgEfficiencyID	   = MFUnset;
static int _MDOutAvgDeltaTempID		   = MFUnset;
static int _MDOutTotalEvaporationID	   = MFUnset;
static int _MDOutTotalThermalWdlsID	   = MFUnset;
static int _MDOutTotalOptThermalWdlsID = MFUnset;
static int _MDOutTotalEnergyDemandID   = MFUnset;
static int _MDOutPowerOutput1ID	 	   = MFUnset;
static int _MDOutPowerDeficit1ID	   = MFUnset;
static int _MDOutPowerPercent1ID	   = MFUnset;
static int _MDOutPowerPercentTotalID   = MFUnset;
static int _MDOutLHFractID			   = MFUnset;
static int _MDOutLHFractPostID		   = MFUnset;
static int _MDOutQpp1ID				   = MFUnset;
static int _MDOutOptQO1ID			   = MFUnset;
static int _MDOutPowerDeficitTotalID   = MFUnset;
static int _MDOutPowerOutputTotalID	   = MFUnset;
static int _MDOutTotalReturnFlowID	   = MFUnset;

static int _MDOutTempIn1ID			   = MFUnset;
static int _MDOutTpp1ID				   = MFUnset;


static void _MDThermalInputs2 (int itemID) {

float a							= 0.0;
float adj_dummy					= 0.0;
float alpha             		= 0.0;
float Approach					= 5.0;
float avgDaily_eff_dTemp_1		= 0.0;
float avgDaily_efficiency_1		= 0.0;
float b					 		= 0.0;
float blowdown_1		    	= 0.0;
float bypass_percent        	= 0.0;
float bypass_Q			    	= 0.0;
float calc_T_1			    	= 0.0;
float circulating_1		    	= 0.0;
float concentration 	    	= 4.5;		// constant
float cond_h2o_1    	    	= 0.0;
float cond_h2o_bm    	    	= 35000;	// this is median gallons of water req for 1 MWhr based on Macknick 2011
float cond_h2o_coal  	    	= 36350;
float cond_h2o_ng    	    	= 35000;
float cond_h2o_ngcc  	    	= 20000;
float cond_h2o_nuc   	    	= 44350;
float cond_h2o_oil   	   	 	= 35000;
float cond_h2o_other 	   	 	= 34283;
float deltaT_1			   	 	= 0.0;
float dTemp_1			  	 	= 0.0;
float dT_max_1			    	= 0.0;
float Efficiency_1		   		= 0.0;		// 1st Powerplant's Efficiency
float Eff_loss_1		 		= 0.0;
float energyDemand_1	 		= 0.0;	// 1st powerplant demand MWhrs/day
float evaporation_1		 		= 0.0;
float flux_QxT			 		= 0.0;
float flux_QxT_new		 		= 0.0;
float fluxmixing_QxT	 		= 0.0;
float FuelType_1		 		= 0.0;		// 1st Powerplant's fueltype in grid cell  (Biomass = 1, Coal = 2, Natural Gas = 3, Natural Gas w/CC = 35, Nuclear = 4, Oil = 5, Other = 6)
float LakeOcean			 		= 0.0;
float LH_vap			 		= 2260;		// latent heat vaporization (look up units)
float LH_fract			 		= 0.0;		// constant (was 0.9)
float LH_fract_post				= 0.0;
float makeup_1		     		= 0.0;
float makeup_bm   		 		= 878;	// wdl needed to replace evaporated water
float makeup_coal  		 		= 1005;
float makeup_ng    		 		= 1203;
float makeup_ngcc  		 		= 253;
float makeup_nuc  		 		= 1101;
float makeup_oil   		 		= 878;
float makeup_other 		 		= 886;
float NamePlate_1		 		= 0.0;	// 1st Powerplant's nameplate in grid cell (MW)
float opt_efficiency_1	 		= 0.0;
float optDeltaT_1		 		= 0.0;		// 1st Powerplant's deltaT
float opt_QO_1					= 0.0;
float output_1			 		= 0.0;
float OT_effluent_1		 		= 0.0;
float placeHolder		 		= 0.0;
float pot_WTemp_1		 		= 0.0;
float Q					 		= 0.0;			// discharge (m3/s)
float Q_incoming_1		 		= 0.0;			// discharge including runoff (m3/s)
float Q_in_1			 		= 0.0;
float Q_outgoing_1		 		= 0.0;			// discharge in grid cell post power plant evaporation
float Q_outgoing_WTemp_1 		= 0.0;			// water temperature post-powerplant with evaporation included (deg C)
float Qpp_1				 		= 0.0;
float Q_WTemp			 		= 0.0;			// water temperature pre-powerplant(deg C)
float riverThresh_temp			= 0.0;
float State						= 0.0;
float Technology_1				= 0.0;		// 1st Powerplant's Technology (Once thru = 1, Cooling tower = 2)
float Temp_In_1					= 0.0;
float TempLimit					= 0.0;
float Tpp_1						= 0.0;
float totalDaily_blowdown_1     = 0.0;
float totalDaily_deficit_1		= 0.0;
float totalDaily_demand_1		= 0.0;
float totalDaily_evap_1	        = 0.0;
float totalDaily_OTeffluent_1	= 0.0;
float totalDaily_output_1       = 0.0;	// 1st powerplant output MWhrs/day
float totalDaily_returnflow_1	= 0.0;
float totalDaily_percent_1		= 0.0;
float totalDaily_target_wdl_1	= 0.0;
float totalDaily_wdl_1			= 0.0;
float totalHours_run_1	        = 0.0;
float wdl_1						= 0.0;
float wdl_coeff_1				= 0.0;
float wetBulbT					= 0.0;

float cell						= 115;


	placeHolder			  = MFVarGetFloat (_MDPlaceHolderID,         itemID, 0.0);	// running MDWTempRiverRoute, value is previous water Temp
	flux_QxT			  = MFVarGetFloat (_MDFlux_QxTID,            itemID, 0.0);	// reading in discharge * temp (m3*degC/day)
	fluxmixing_QxT		  = MFVarGetFloat (_MDFluxMixing_QxTID,      itemID, 0.0);	// reading in discharge * tempmixing (m3*degC/day)
 	Q                     = MFVarGetFloat (_MDInDischargeID,         itemID, 0.0);
// 	Q_incoming_1		  = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0);	// already includes local runoff
  	Q_incoming_1		  = MFVarGetFloat (_MDOutDischargeID, itemID, 0.0);			// late-night discharge test (and one commented out line above)

  	NamePlate_1            = MFVarGetFloat (_MDInNamePlate1ID,         itemID, 0.0);
  	FuelType_1            = MFVarGetFloat (_MDInFuelType1ID,         itemID, 0.0);
 	Technology_1            = MFVarGetFloat (_MDInTechnology1ID,         itemID, 0.0);
 	Efficiency_1            = MFVarGetFloat (_MDInEfficiency1ID,         itemID, 0.0);
 	energyDemand_1            = MFVarGetFloat (_MDInEnergyDemand1ID,         itemID, 0.0);

 	opt_efficiency_1			= Efficiency_1;

 	bypass_percent			 = MFVarGetFloat (_MDInBypassPercentID,         itemID, 0.0);
 	Approach				 = MFVarGetFloat (_MDInApproachID,              itemID, 0.0);
  	riverThresh_temp		 = MFVarGetFloat (_MDInRiverThreshTempID,       itemID, 0.0);		// 22, needs to be in script
  	State					 = MFVarGetFloat (_MDInStateID, 				  itemID, 0.0);
  	wetBulbT				 = MFVarGetFloat (_MDInWetBulbTempID,             itemID, 0.0);
  	LakeOcean				 = MFVarGetFloat (_MDInLakeOcean1ID,               itemID, 0.0);		// 1 is lakeOcean, 0 is nothing

	if (State == 9) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_CTID,         itemID, 0.0);
	if (State == 10) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_DEID,         itemID, 0.0);
	if (State == 25) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_MAID,         itemID, 0.0);
	if (State == 24) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_MDID,         itemID, 0.0);
	if (State == 23) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_MEID,         itemID, 0.0);
	if (State == 33) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_NHID,         itemID, 0.0);
	if (State == 34) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_NJID,         itemID, 0.0);
	if (State == 36) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_NYID,         itemID, 0.0);
	if (State == 42) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_PAID,         itemID, 0.0);
	if (State == 44) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_RIID,         itemID, 0.0);
	if (State == 51) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_VAID,         itemID, 0.0);
	if (State == 50) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_VTID,         itemID, 0.0);

  	if (FuelType_1 == 1)   cond_h2o_1 = ((cond_h2o_bm    * 0.0037854) / 3600) * NamePlate_1;			// biomass, 0.0037854 converts gallons to m3. Units for cond_h2o_1 is m3/s
  	if (FuelType_1 == 2)   cond_h2o_1 = ((cond_h2o_coal  * 0.0037854) / 3600) * NamePlate_1;			// coal
  	if (FuelType_1 == 3)   cond_h2o_1 = ((cond_h2o_ng    * 0.0037854) / 3600) * NamePlate_1;			//
  	if (FuelType_1 == 35) cond_h2o_1 = ((cond_h2o_ngcc  * 0.0037854) / 3600) * NamePlate_1;			//  this is the amount of water that is required for the condenser
  	if (FuelType_1 == 4)   cond_h2o_1 = ((cond_h2o_nuc   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 5)   cond_h2o_1 = ((cond_h2o_oil   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 6)   cond_h2o_1 = ((cond_h2o_other * 0.0037854) / 3600) * NamePlate_1;			//

  	if (FuelType_1 == 1)   makeup_1 = ((makeup_bm    * 0.0037854) / 3600) * NamePlate_1;			// biomass, 0.0037854 converts gallons to m3. Units for cond_h2o_1 is m3/s
  	if (FuelType_1 == 2)   makeup_1 = ((makeup_coal  * 0.0037854) / 3600) * NamePlate_1;			// coal
  	if (FuelType_1 == 3)   makeup_1 = ((makeup_ng    * 0.0037854) / 3600) * NamePlate_1;			//
  	if (FuelType_1 == 35) makeup_1 = ((makeup_ngcc  * 0.0037854) / 3600) * NamePlate_1;			//  this is the amount of water required for wdl in recirculating systems due to losses
  	if (FuelType_1 == 4)   makeup_1 = ((makeup_nuc   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 5)   makeup_1 = ((makeup_oil   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 6)   makeup_1 = ((makeup_other * 0.0037854) / 3600) * NamePlate_1;			//

	optDeltaT_1 = (NamePlate_1 * ((1.0 / Efficiency_1) - 1.0)) / (cond_h2o_1 * 4.186);				// optimal temperature increase of condenser water (cost effective)
	Q_in_1		= NamePlate_1 / Efficiency_1;														// heat in
	opt_QO_1	= (1.0 - opt_efficiency_1) * Q_in_1;


	if (Q_incoming_1 > 0.000001) {

		Q_WTemp				 = flux_QxT / (Q_incoming_1 * 86400);			// degC RJS 013112
		pot_WTemp_1			 = NamePlate_1 > 0.0 ? 9999 : Q_WTemp;			// degC - this is the potential river temperature that power plant will produce based on its actions
//		Q_WTemp_mix			 = fluxmixing_QxT / (Q_incoming_1 * 86400);		// degC RJS 013112
		bypass_Q			 = bypass_percent * Q_incoming_1;				// amount of flow that cannot be withdrawn
		alpha				 = 1.0 / 20.0;									// exponent of withdrawal

//		if (itemID == cell) {
//		printf("**** y = %d, m = %d, d = %d, Q = %f, NameP = %f, FuelT = %f, Tech = %f, Eff = %f, Demand = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q_incoming_1, NamePlate_1, FuelType_1, Technology_1, Efficiency_1, energyDemand_1);
//		printf("flux_QxT = %f\n", flux_QxT);
//		printf("byp = %f, App = %f, Rthresh = %f, wbt = %f, L-O = %f\n", bypass_percent, Approach, riverThresh_temp, wetBulbT, LakeOcean);
//		printf("TempLimit = %f, cond_h2o = %f, makeup = %f, optDeltaT = %f, Q_in = %f\n", TempLimit, cond_h2o_1, makeup_1, optDeltaT_1, Q_in_1);
//		printf("Q_WTemp = %f, bypass_Q = %f, alpha = %f\n", Q_WTemp, bypass_Q, alpha);
//		}

	if (NamePlate_1 > 0.0) {


	if ((Q_WTemp >= TempLimit) && (TempLimit > 0.0)) {		// when CWA is on, and temperature is TOO warm
		a = 1.0;
	}

	if (LakeOcean == 1.0) {									//
		b = 1.0;
	}

	if (Technology_1 == 1.0) {										// once through technology
//			if (itemID == cell) printf("Entering once-through ...\n");
			wdl_1 = cond_h2o_1;



			if (((Q_incoming_1 - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_temp)) {		// enough water to satisfy optimal wdl, BUT water above 22
//				if (itemID == cell) printf("!!!!! Plenty of Water BUT Temperature TOO WARM (1)\n");
				calc_T_1     = Q_WTemp - riverThresh_temp;
				Eff_loss_1   = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
				Efficiency_1 = Efficiency_1 - Eff_loss_1;

				Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
				dTemp_1      = optDeltaT_1;
				wdl_1        = Qpp_1 / (dTemp_1 * 4.186);
				output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1       = wdl_1 * dTemp_1 * 4.186;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1       = Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;

					if (pot_WTemp_1 > TempLimit) {
						dTemp_1 = dTemp_1 - 0.5;
//						if (itemID == cell) printf("! dTemp DECREASING because of Limit (2)\n");
					}
					wdl_coeff_1 = pow((optDeltaT_1 / dTemp_1),alpha);
					wdl_1		= wdl_1 * wdl_coeff_1;
//					if (itemID == cell) printf(" wdl_coeff_1 = %f, dTemp_1 = %f, wdl_1 = %f, Qpp_1 = %f\n", wdl_coeff_1, dTemp_1, wdl_1, Qpp_1);
//					if (itemID == cell) printf("pot_WTemp_1 = %f, TempLimit = %f\n", pot_WTemp_1, TempLimit);
				}



				if (wdl_1 > (Q_incoming_1 - bypass_Q)) {						// if wdl is greater than what is in river
//					if (itemID == cell) printf("!!!!! ADJUSTING for withdrawal (3)\n");
					wdl_1       = Q_incoming_1 - bypass_Q;
					Qpp_1       = wdl_1 * dTemp_1 * 4.186;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;
					adj_dummy	= 1.0;
				}

				if (dTemp_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation

				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT 2 (4)\n");
					calc_T_1     = Q_WTemp - riverThresh_temp;
					Eff_loss_1   = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
					Efficiency_1 = Efficiency_1 - Eff_loss_1;

					Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
					dTemp_1      = optDeltaT_1;
					wdl_1        = Qpp_1 / (dTemp_1 * 4.186);
					output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

					Tpp_1		 = Q_WTemp + dTemp_1;
					pot_WTemp_1  = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;
				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "



			}			// this ends the "enough water for optimal wdl, but high temp" scenario ((Q_incoming - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_Temp))

			if ((Q_WTemp < riverThresh_temp) && (wdl_1 <= (Q_incoming_1 - bypass_Q))) {
//				if (itemID == cell) printf("!!!!! PLENTY of water AND temperature is GOOD (5)\n");
				dTemp_1 	= optDeltaT_1;
				wdl_1		= cond_h2o_1;
				Qpp_1		= (1.0 - Efficiency_1) * Q_in_1;

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1  		= wdl_1 * dTemp_1 * 4.186;
					output_1 	= (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= dTemp_1 + Q_WTemp;
					pot_WTemp_1  = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;

					if (pot_WTemp_1 > TempLimit) {
						dTemp_1 = dTemp_1 - 0.5;
//						if (itemID == cell) printf("! dTemp DECREASING because of Limit (6)\n");
					}
					wdl_coeff_1 = pow((optDeltaT_1 / dTemp_1),alpha);
					wdl_1		= wdl_1 * wdl_coeff_1;
				}

				if (dTemp_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation

				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT 3 (7)\n");
					output_1 	= (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= dTemp_1 + Q_WTemp;
					pot_WTemp_1 = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;

				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "


			}			// this ends the "(Q_WTemp < riverThresh_Temp) && (wdl_1 <= (Q_incoming - bypass_Q))"



			if (((Q_incoming_1 - bypass_Q) < wdl_1) && (adj_dummy != 1.0)) {								// less water available than optimal wdl
//				if (itemID == cell) printf("!!!!! Discharge is NOT enough for wdl (8)\n");
				wdl_1 = Q_incoming_1 - bypass_Q;

				if (Q_WTemp >= riverThresh_temp) {
//					if (itemID == cell) printf("!! Temperature is TOO warm (9)\n");
						calc_T_1     = Q_WTemp - riverThresh_temp;
						Eff_loss_1   = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
						Efficiency_1 = Efficiency_1 - Eff_loss_1;
				}

				Qpp_1 = (1.0 - Efficiency_1) * Q_in_1;			// heat out

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (wdl_1 > 0.0)){
						deltaT_1 = Qpp_1 / (wdl_1 * 4.186);
						if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
						else if ((Q_WTemp > 20) && (Q_WTemp <= 30)) dT_max_1 = 20.0;
						else dT_max_1 = optDeltaT_1;

						 if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
						 else dTemp_1 = deltaT_1;

						 Qpp_1       = dTemp_1 * 4.186 * wdl_1;
						 output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
						 Tpp_1	     = dTemp_1 + Q_WTemp;
						 pot_WTemp_1 = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;

						 if (pot_WTemp_1 > TempLimit) {
							 wdl_1 = wdl_1 - 0.01;
//							 if (itemID == cell) printf("! Wdl DECREASING because of Limit (10)\n");
						 }

				}

				if (wdl_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation


				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT (11)\n");
					deltaT_1 = Qpp_1 / (wdl_1 * 4.186);
					if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
					else if ((Q_WTemp > 20) && (Q_WTemp <= 30)) dT_max_1 = 20.0;
					else dT_max_1 = optDeltaT_1;

					if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
					else dTemp_1 = deltaT_1;

					Qpp_1       = dTemp_1 * 4.186 * wdl_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1	    = dTemp_1 + Q_WTemp;
					pot_WTemp_1 = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q_incoming_1;
				}

				wdl_1       = (wdl_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (wdl_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (wdl_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;	// "

			}				// this ends "less water available than optimal wdl" (Q_incoming - bypass_Q) < wdl_1)


			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;		// effluent from cooling towers
			OT_effluent_1 = wdl_1;		// return flow (or effluent) from power plant to river

//			if (itemID == cell) {
//			printf("Q_incoming_1 = %f, bypass_Q = %f, wdl_1 = %f\n", Q_incoming_1, bypass_Q, wdl_1);
//			printf("dTemp_1 = %f, Qpp_1 = %f\n", dTemp_1, Qpp_1);
//			printf("Tpp_1 = %f, Eff_loss_1 = %f, Efficiency_1 = %f\n", Tpp_1, Eff_loss_1, Efficiency_1);
//			printf("riverThresh_temp = %f, dTemp_1 = %f, calc_T_1 = %f\n", riverThresh_temp, dTemp_1, calc_T_1);
//			printf("wdl_coeff_1 = %f\n", wdl_coeff_1);
//			printf("output_1 = %f, pot_WTemp_1 = %f\n", output_1, pot_WTemp_1);
//			}

		}				// this ends "once through"

		if (Technology_1 == 2.0) {				// re-circulating technology
			wdl_1 		  = makeup_1;
			circulating_1 = cond_h2o_1;
			Temp_In_1	  = wetBulbT + Approach;
			blowdown_1	   = (1.0 / concentration) * wdl_1;
			evaporation_1 = blowdown_1 * (concentration - 1.0);
			LH_fract	   = LH_vap * (evaporation_1 / (opt_QO_1));			// temporary attempt


//				if (itemID == cell) printf("Entering Recirculating...\n");



			if (Temp_In_1 >= riverThresh_temp) {
//				if (itemID == cell) printf("!!!!! Cooling Water Temperature HIGHER than Thresh\n");
				dTemp_1 	  = optDeltaT_1;
				calc_T_1      = Q_WTemp - riverThresh_temp;
				Eff_loss_1    = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
				Efficiency_1  = Efficiency_1 - Eff_loss_1;
				Qpp_1		  = (1.0 - Efficiency_1) * Q_in_1;
				circulating_1 = Qpp_1 / dTemp_1 * 4.186;
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
			}

			if (((Q_incoming_1 - bypass_Q) >= wdl_1) && (Temp_In_1 < riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! Plenty of Water AND Temperature is good\n");
				dTemp_1		   = optDeltaT_1;
				wdl_1		   = makeup_1;
				blowdown_1	   = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);
//				LH_fract	   = LH_vap * (evaporation_1 / (1.0 - opt_efficiency_1) * Q_in_1);			// temporary attempt
				Qpp_1		   = evaporation_1 / (LH_fract / LH_vap);
				circulating_1  = Qpp_1 / (4.186 * dTemp_1);
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Q_WTemp;
				pot_WTemp_1   = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / Q_incoming_1;
			}



			if (((Q_incoming_1 - bypass_Q) >= wdl_1) && (Temp_In_1 >= riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! More than enough Q, Cooling Temperature is too warm\n");
				dTemp_1		  = optDeltaT_1;
				evaporation_1 = (LH_fract / LH_vap) * Qpp_1;
				blowdown_1	  = (evaporation_1 / (concentration - 1.0));
				wdl_1		  = (blowdown_1 / (1.0 / concentration));

				if ((Q_incoming_1 - bypass_Q) < wdl_1) {
//					if (itemID == cell) printf("!! AND Discharge minus bypass LESS than wdl\n");
					wdl_1 		  = Q_incoming_1- bypass_Q;
					dTemp_1		  = optDeltaT_1;
					blowdown_1	  = (1.0 / concentration) * wdl_1;
					evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
					Qpp_1		  = evaporation_1 / (LH_fract / LH_vap);
					circulating_1 = Qpp_1 / (4.186 * dTemp_1);
					output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		  = Q_WTemp;
					pot_WTemp_1   = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / Q_incoming_1;
				}

				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Q_WTemp;
				pot_WTemp_1   = (((Q_incoming_1 - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / Q_incoming_1;

			}

			if ((Q_incoming_1 - bypass_Q) < wdl_1) {
//				if (itemID == cell) printf("!!!!! Discharge minus bypass LESS than wdl\n");
				wdl_1 		  = Q_incoming_1 - bypass_Q;
				dTemp_1		  = optDeltaT_1;
				blowdown_1	  = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
				Qpp_1		  = evaporation_1 / (LH_fract / LH_vap);
				circulating_1 = Qpp_1 / (4.186 * dTemp_1);
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Q_WTemp;
				pot_WTemp_1   = (((Q_incoming_1- wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / Q_incoming_1;

			}

			OT_effluent_1 = 0.0;
			LH_fract_post = LH_fract * (evaporation_1 / Qpp_1);			// temporary attempt

		}		// this ends re-circulating technology






	if (b == 1.0) {
			pot_WTemp_1   = Q_WTemp;
			output_1	  = NamePlate_1;
//			wdl_1		  = wdl_1;				// (commented out because new eclipse (juno?) doesn't like assignment to itself
//			evaporation_1 = evaporation_1;		// (commented out because new eclipse (juno?) doesn't like assignment to itself
//			blowdown_1	  = blowdown_1;			// (commented out because new eclipse (juno?) doesn't like assignment to itself
//			OT_effluent_1 = OT_effluent_1;		// (commented out because new eclipse (juno?) doesn't like assignment to itself
	}





//	if (itemID == cell) {
//	printf("Q_incoming_1 = %f, bypass_Q = %f, wdl_1 = %f\n", Q_incoming_1, bypass_Q, wdl_1);
//	printf("dTemp_1 = %f, blowdown_1 = %f, evaporation_1 = %f, Qpp_1 = %f\n", dTemp_1, blowdown_1, evaporation_1, Qpp_1);
//	printf("circulating_1 = %f, Tpp_1 = %f, Eff_loss_1 = %f, Efficiency_1 = %f\n", circulating_1, Tpp_1, Eff_loss_1, Efficiency_1);
//	printf("Temp_In_1 = %f, riverThresh_temp = %f, dTemp_1 = %f, calc_T_1 = %f\n", Temp_In_1, riverThresh_temp, dTemp_1, calc_T_1);
//	printf("LH_fract_1 = %f, opt_QO_1 = %f\n", LH_fract, opt_QO_1);
//	printf("output_1 = %f, pot_WTemp_1 = %f\n", output_1, pot_WTemp_1);
//	}

	totalDaily_output_1 	  = (output_1 * 24)	> energyDemand_1 ? energyDemand_1 : (output_1 * 24);		// total MWhrs per day
	totalDaily_deficit_1	  = energyDemand_1 - totalDaily_output_1;		// energy deficit (MWhrs)
	totalDaily_percent_1	  = totalDaily_output_1 / energyDemand_1;		// percent of demand fulfilled
	totalHours_run_1		  = totalDaily_output_1 /output_1;			// hrs of operation run
	totalDaily_wdl_1		  = wdl_1 * totalHours_run_1 * 3600;			// total wdl in m3 per day
	totalDaily_target_wdl_1	  = wdl_1 * (energyDemand_1 / NamePlate_1) * 3600;	// total target withdrawal to meet demand based on standard wdl per MW
	avgDaily_eff_dTemp_1	  = dTemp_1 / optDeltaT_1;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
	avgDaily_efficiency_1	  = Efficiency_1 / opt_efficiency_1;				// percentage of optimal efficiency fulfilled
	totalDaily_evap_1		  = evaporation_1 * totalHours_run_1 * 3600;	// total evaporation in m3 per day
	totalDaily_blowdown_1	  = blowdown_1 * totalHours_run_1 * 3600;		// total blowdown (or discharge from power plant to river) in m3 per day
	totalDaily_OTeffluent_1   = OT_effluent_1 * totalHours_run_1 * 3600;		// total return flow from once through cooling to river (effluent)
	totalDaily_returnflow_1	  = totalDaily_blowdown_1 + totalDaily_OTeffluent_1;	// total (effluent + blowdown) from both "once through" and "recirc" back to river (m3)
	totalDaily_demand_1		  = energyDemand_1;

	Q_outgoing_1			  = (b == 1.0) ? Q_incoming_1: Q_incoming_1 - evaporation_1;
	Q_outgoing_WTemp_1		  = pot_WTemp_1;

//	flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_incoming_1 * 86400.0;		// we need to factor in evaporative loss
	flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
//	fluxmixing_QxT_new 		  = riverTempMixing_post * Q_incoming_1* 86400.0;

//	if (itemID == cell) {
//	printf("***** m = %d, d = %d, energyDemand_1 = %f, output_1 = %f, Q_WTtemp = %f, Q = %f, wbt = %f\n", MFDateGetCurrentMonth(), MFDateGetCurrentDay(), energyDemand_1, output_1, Q_WTemp, Q_incoming_1, wetBulbT);
//	printf("total_Daily_output = %f, totalDaily_deficit = %f, totalDaily_percent = %f, totalHours_run = %f\n", totalDaily_output_1, totalDaily_deficit_1, totalDaily_percent_1, totalHours_run_1);
//	printf("totalDaily_wdl = %f, totalDaily_target_wdl = %f, avgDaily_eff_dTemp = %f, avgDaily_efficiency = %f\n", totalDaily_wdl_1, totalDaily_target_wdl_1, avgDaily_eff_dTemp_1, avgDaily_efficiency_1);
//	printf("totalDaily_evap = %f, totalDaily_blowdown = %f, totalDaily_OTeffluent = %f, totalDaily_returnflow = %f\n", totalDaily_evap_1, totalDaily_blowdown_1, totalDaily_OTeffluent_1, totalDaily_returnflow_1);
//	}

//	if (itemID == cell) printf("pot_WTemp_1 = %f, flux_QxT_new = %f\n", pot_WTemp_1, flux_QxT_new);
//	if (itemID == cell) printf("Q_in = %f, Q_out = %f, T_in = %f, T_out = %f\n", Q_incoming_1, Q_outgoing_1, Q_WTemp, Q_outgoing_WTemp_1);
//	if (itemID == cell) {
//		printf("pot_WTemp_1 = %f, output_1 = %f, wdl_1 = %f\n", pot_WTemp_1, output_1, wdl_1);
//		printf("evaporation_1 = %f, blowdown_1 = %f, OT_effluent_1 = %f\n", evaporation_1, blowdown_1, OT_effluent_1);
//	}

	if (a == 1.0) {						// CWA is on, BUT Q_WTemp is TOO high - so NO WITHDRAWALS / NO OPERATION
			pot_WTemp_1   = Q_WTemp;
			output_1	  = 0.0;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;
			OT_effluent_1 = 0.0;
			totalDaily_output_1 	  = 0.0;		// total MWhrs per day
			totalDaily_deficit_1	  = 0.0;		// energy deficit (MWhrs)
			totalDaily_percent_1	  = 0.0;		// percent of demand fulfilled
			totalHours_run_1		  = 0.0;			// hrs of operation run
			totalDaily_wdl_1		  = 0.0;			// total wdl in m3 per day
			totalDaily_target_wdl_1	  = 0.0;	// total target withdrawal to meet demand based on standard wdl per MW
			avgDaily_eff_dTemp_1	  = 0.0;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
			avgDaily_efficiency_1	  = 0.0;				// percentage of optimal efficiency fulfilled
			totalDaily_evap_1		  = 0.0;	// total evaporation in m3 per day
			totalDaily_blowdown_1	  = 0.0;		// total blowdown (or discharge from power plant to river) in m3 per day
			totalDaily_OTeffluent_1   = 0.0;		// total return flow from once through cooling to river (effluent)
			totalDaily_returnflow_1	  = 0.0;	// total (effluent + blowdown) from both "once through" and "recirc" back to river
			totalDaily_demand_1		  = energyDemand_1;


			Q_outgoing_1			  = Q_incoming_1;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test

		}


	}	// this ends NamePlate > 0.0

	else {									// there is no power plant
			pot_WTemp_1   = Q_WTemp;
			output_1	  = 0.0;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;
			OT_effluent_1 = 0.0;
			totalDaily_output_1 	  = 0.0;		// total MWhrs per day
			totalDaily_deficit_1	  = 0.0;		// energy deficit (MWhrs)
			totalDaily_percent_1	  = 0.0;		// percent of demand fulfilled
			totalHours_run_1		  = 0.0;			// hrs of operation run
			totalDaily_wdl_1		  = 0.0;			// total wdl in m3 per day
			totalDaily_target_wdl_1	  = 0.0;	// total target withdrawal to meet demand based on standard wdl per MW
			avgDaily_eff_dTemp_1	  = 0.0;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
			avgDaily_efficiency_1	  = 0.0;				// percentage of optimal efficiency fulfilled
			totalDaily_evap_1		  = 0.0;	// total evaporation in m3 per day
			totalDaily_blowdown_1	  = 0.0;		// total blowdown (or discharge from power plant to river) in m3 per day
			totalDaily_OTeffluent_1   = 0.0;		// total return flow from once through cooling to river (effluent)
			totalDaily_returnflow_1	  = 0.0;	// total (effluent + blowdown) from both "once through" and "recirc" back to river
			totalDaily_demand_1		  = 0.0;

			Q_outgoing_1			  = Q_incoming_1;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test

	}

	}		// this ends Q_incoming_1> 0.000001

	else {								// there is no flow
			pot_WTemp_1	  = Q_WTemp;
			output_1	  = 0.0;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			blowdown_1    = 0.0;
			OT_effluent_1 			 = 0.0;
			totalDaily_output_1 	  = 0.0;		// total MWhrs per day
			totalDaily_deficit_1	  = 0.0;		// energy deficit (MWhrs)
			totalDaily_percent_1	  = 0.0;		// percent of demand fulfilled
			totalHours_run_1		  = 0.0;			// hrs of operation run
			totalDaily_wdl_1		  = 0.0;			// total wdl in m3 per day
			totalDaily_target_wdl_1	  = 0.0;	// total target withdrawal to meet demand based on standard wdl per MW
			avgDaily_eff_dTemp_1	  = 0.0;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
			avgDaily_efficiency_1	  = 0.0;				// percentage of optimal efficiency fulfilled
			totalDaily_evap_1		  = 0.0;	// total evaporation in m3 per day
			totalDaily_blowdown_1	  = 0.0;		// total blowdown (or discharge from power plant to river) in m3 per day
			totalDaily_OTeffluent_1   = 0.0;		// total return flow from once through cooling to river (effluent)
			totalDaily_returnflow_1	  = 0.0;	// total (effluent + blowdown) from both "once through" and "recirc" back to river
			totalDaily_demand_1		  = NamePlate_1 > 0.0 ? energyDemand_1 : 0.0;

			Q_outgoing_1			  = Q_incoming_1;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
	}


//	if (NamePlate_1 > 0.0) {
//		printf("NamePlate_1 = %f, totalDaily_output_1 = %f, energyDemand_1 = %f, totalDaily_deficit = %f, totalDaily_percent_1 = %f\n", NamePlate_1, totalDaily_output_1, energyDemand_1, totalDaily_deficit_1, totalDaily_percent_1);

//	}

	 MFVarSetFloat(_MDOutDischargeID,         itemID, Q_outgoing_1);			//late-night discharge test
	 MFVarSetFloat(_MDWTemp_QxTID,            itemID, Q_outgoing_WTemp_1);
	 MFVarSetFloat(_MDFlux_QxTID,             itemID, flux_QxT_new);
//	 MFVarSetFloat(_MDFluxMixing_QxTID,       itemID, fluxmixing_QxT_new);
	 MFVarSetFloat(_MDOutTotalThermalWdlsID,  itemID, totalDaily_wdl_1);
	 MFVarSetFloat(_MDOutTotalEvaporationID,  itemID, totalDaily_evap_1);
	 MFVarSetFloat(_MDOutTotalOptThermalWdlsID, itemID, totalDaily_target_wdl_1);
	 MFVarSetFloat(_MDOutAvgDeltaTempID,        itemID, avgDaily_eff_dTemp_1);
	 MFVarSetFloat(_MDOutAvgEfficiencyID,       itemID, avgDaily_efficiency_1);
	 MFVarSetFloat(_MDOutPowerOutput1ID,      itemID, totalDaily_output_1);
	 MFVarSetFloat(_MDOutPowerDeficit1ID,     itemID, totalDaily_deficit_1);
	 MFVarSetFloat(_MDOutPowerPercent1ID,     itemID, totalDaily_percent_1);
	 MFVarSetFloat(_MDOutPowerOutputTotalID,  itemID, totalDaily_output_1);			// there is only 1 layer of power plants, so this is same as above
	 MFVarSetFloat(_MDOutPowerDeficitTotalID, itemID, totalDaily_deficit_1);		// "
	 MFVarSetFloat(_MDOutPowerPercentTotalID, itemID, totalDaily_percent_1);		// "
	 MFVarSetFloat(_MDOutTotalEnergyDemandID, itemID, totalDaily_demand_1);
	 MFVarSetFloat(_MDOutTotalReturnFlowID,   itemID, totalDaily_returnflow_1);
	 MFVarSetFloat(_MDOutLHFractID,           itemID, LH_fract);
	 MFVarSetFloat(_MDOutLHFractPostID,       itemID, LH_fract_post);
	 MFVarSetFloat(_MDOutQpp1ID,              itemID, Qpp_1);
	 MFVarSetFloat(_MDOutOptQO1ID,            itemID, opt_QO_1);
	 MFVarSetFloat(_MDOutTpp1ID,              itemID, Tpp_1);	//RJS 111812
	 MFVarSetFloat(_MDOutTempIn1ID,           itemID, Temp_In_1); //RJS 111812
}

// end of code



enum { MDnone, MDinput };

int MDThermalInputs2Def () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptThermalInputs2;
	const char *options [] = { MDNoneStr, MDInputStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Thermal Inputs 2");
	switch (optID) {
		case MDinput:
			if (((_MDPlaceHolderID          = MDWTempRiverRouteDef ()) == CMfailed) ||
				((_MDInDischargeID          = MDDischLevel2Def ()) == CMfailed) ||
				((_MDInEnergyDemand1ID       = MDEnergyDemandDef ()) == CMfailed) ||
				((_MDInDischargeIncomingID  = MFVarGetID (MDVarDischarge0,          "m3/s", MFInput,  MFFlux,  MFBoundary)) == CMfailed) ||
				((_MDFluxMixing_QxTID       = MFVarGetID (MDVarFluxMixing_QxT, "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
				((_MDFlux_QxTID             = MFVarGetID (MDVarFlux_QxT,       "m3*degC/d", MFRoute,  MFFlux,  MFBoundary)) == CMfailed)   ||
		//		((_MDWTempMixing_QxTID      = MFVarGetID (MDVarWTempMixing_QxT,   "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDWTemp_QxTID            = MFVarGetID (MDVarWTemp_QxT,         "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 013112

				((_MDInBypassPercentID      = MFVarGetID (MDVarBypassPercent,        "%", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInRiverThreshTempID    = MFVarGetID (MDVarRiverThreshT,        "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInApproachID           = MFVarGetID (MDVarApproach,            "-", MFInput, MFState, MFBoundary)) == CMfailed)  ||
				((_MDInStateID         	    = MFVarGetID (MDVarState,                "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInWetBulbTempID        = MFVarGetID (MDVarWetBulbTemp,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_CTID		= MFVarGetID (MDVarTempLimitCT,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_DEID		= MFVarGetID (MDVarTempLimitDE,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_MAID		= MFVarGetID (MDVarTempLimitMA,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_MDID		= MFVarGetID (MDVarTempLimitMD,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_MEID		= MFVarGetID (MDVarTempLimitME,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_NHID		= MFVarGetID (MDVarTempLimitNH,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_NJID		= MFVarGetID (MDVarTempLimitNJ,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_NYID		= MFVarGetID (MDVarTempLimitNY,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_PAID		= MFVarGetID (MDVarTempLimitPA,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_RIID		= MFVarGetID (MDVarTempLimitRI,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_VAID		= MFVarGetID (MDVarTempLimitVA,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTempLimit_VTID		= MFVarGetID (MDVarTempLimitVT,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||

				((_MDInNamePlate1ID         = MFVarGetID (MDVarNamePlate1,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType1ID          = MFVarGetID (MDVarFuelType1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTechnology1ID        = MFVarGetID (MDVarTechnology1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInEfficiency1ID        = MFVarGetID (MDVarEfficiency1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInLakeOcean1ID         = MFVarGetID (MDVarLakeOcean1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInPlantCode1ID         = MFVarGetID (MDVarPlantCode1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||

				((_MDOutDischargeID    	      = MFVarGetID (MDVarDischarge,  "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||		// late night discharge test
				((_MDOutAvgEfficiencyID       = MFVarGetID (MDVarAvgEfficiency,  "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutAvgDeltaTempID        = MFVarGetID (MDVarAvgDeltaTemp,  "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutTotalEvaporationID    = MFVarGetID (MDVarTotalEvaporation,  "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalThermalWdlsID    = MFVarGetID (MDVarTotalThermalWdls, "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalOptThermalWdlsID = MFVarGetID (MDVarTotalOptThermalWdls, "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutput1ID        = MFVarGetID (MDVarPowerOutput1,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficit1ID       = MFVarGetID (MDVarPowerDeficit1,       "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercent1ID       = MFVarGetID (MDVarPowerPercent1,       "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutPowerOutputTotalID    = MFVarGetID (MDVarPowerOutputTotal,    "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerDeficitTotalID   = MFVarGetID (MDVarPowerDeficitTotal,   "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutPowerPercentTotalID   = MFVarGetID (MDVarPowerPercentTotal,   "MW", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutTotalEnergyDemandID   = MFVarGetID (MDVarTotalEnergyDemand,   "MW", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalReturnFlowID	  = MFVarGetID (MDVarTotalReturnFlow,     "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed)	||
				((_MDOutLHFractID             = MFVarGetID (MDVarLHFract,             "-",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutLHFractPostID         = MFVarGetID (MDVarLHFractPost,         "-",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutQpp1ID                = MFVarGetID (MDVarQpp1,                "-",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutOptQO1ID              = MFVarGetID (MDVarOptQO1,              "-",  MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutTpp1ID                = MFVarGetID (MDVarTpp1,              "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||		// RJS 111812
				((_MDOutTempIn1ID             = MFVarGetID (MDVarTempIn1,           "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||		// RJS 111812

			    (MFModelAddFunction (_MDThermalInputs2) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Thermal Inputs 2");
	return (_MDWTemp_QxTID);
}









