
/******************************************************************************
GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDThermalInputs3.c

amiara00@CCNY.cuny.edu & rob.stewart@unh.edu

Thermal Inputs, withdrawals, and energy of thermoelectric plants (second iteration)
*******************************************************************************/

#include <stdio.h>
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

//static int _MDInPlantCode1ID		= MFUnset;
static int _MDInNamePlate1ID		= MFUnset;
static int _MDInFuelType1ID			= MFUnset;
static int _MDInTechnology1ID		= MFUnset;
static int _MDInEfficiency1ID		= MFUnset;
static int _MDInLakeOcean1ID		= MFUnset;  // there may be more of these
static int _MDInEnergyDemand1ID		= MFUnset;



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
static int _MDInTempLimit_DCID		= MFUnset;
static int _MDInWetBulbTempID	  	= MFUnset;
static int _MDInAirTemperatureID	= MFUnset;

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
static int _MDOutTotalExternalWaterID  = MFUnset;

static int _MDOutTotalHeatToOceanID	   = MFUnset;		// added 010412
static int _MDOutTotalHeatToRivID	   = MFUnset;
static int _MDOutTotalHeatToSinkID	   = MFUnset;
static int _MDOutTotalHeatToEngID	   = MFUnset;
static int _MDOutTotalHeatToElecID	   = MFUnset;
static int _MDOutTotalHeatToEvapID	   = MFUnset;
static int _MDOutCondenserInletID	   = MFUnset;		// added 122112
static int _MDOutSimEfficiencyID	   = MFUnset;		// added 122112

static int _MDOutTotalHoursRunID       = MFUnset;		// added 030212

static void _MDThermalInputs3 (int itemID) {

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
float cond_h2o_bm    	    	= 35000.0;	// this is median gallons per 1 mwh of water req for 1 MWhr based on Macknick 2011
float cond_h2o_coal  	    	= 36350.0;
float cond_h2o_ng    	    	= 35000.0;
float cond_h2o_ngccOT			= 11380.0;		// gncc = natural gas combined cycle, OT = once-through, RC = recirculating, DC = dry cooling (gallons per 1 mwh)
float cond_h2o_ngccRC			= 11380.0;
float cond_h2o_ngccDC			= 11380.0;
float cond_h2o_nuc   	    	= 44350.0;
float cond_h2o_oil   	   	 	= 35000.0;
float cond_h2o_other 	   	 	= 34283.0;
float deltaT_1			   	 	= 0.0;		// temperature difference between intake temperature and outlet temperature (C)
float dTemp_1			  	 	= 0.0;
float dT_max_1			    	= 0.0;
float Efficiency_1		   		= 0.0;		// 1st Powerplant's Efficiency
float Eff_loss_1		 		= 0.0;
float eff_gas					= 0.0;
float eff_steam					= 0.0;
float energyDemand_1	 		= 0.0;	// 1st powerplant demand MWhrs/day
float evaporation_1		 		= 0.0;
float flux_QxT			 		= 0.0;
float flux_QxT_new		 		= 0.0;
float fluxmixing_QxT	 		= 0.0;
float FuelType_1		 		= 0.0;		// 1st Powerplant's fueltype in grid cell  (Biomass = 1, Coal = 2, Natural Gas = 3, Nuclear = 4, Oil = 5, Other = 6)
float LakeOcean			 		= 0.0;
float LH_vap			 		= 2260.0;		// latent heat vaporization (look up units)
float LH_fract			 		= 0.0;		// constant (was 0.9)
float LH_fract_post				= 0.0;
float makeup_1		     		= 0.0;
float makeup_bm   		 		= 878.0;	// wdl needed to replace evaporated water
float makeup_coal  		 		= 1005.0;
float makeup_ng    		 		= 1203.0;
float makeup_ngccOT				= 11380.0;	//make up = withdrawal (gallons per 1 mwh)
float makeup_ngccRC				= 253.0;	// wdl needed to replace evaporated water
float makeup_ngccDC				= 2.0;
float makeup_nuc  		 		= 1101.0;
float makeup_oil   		 		= 878.0;
float makeup_other 		 		= 886.0;
float other_heat_sink_bm		= 0.12; 	///heat lost to other sinks (other than condenser  --  i.e. flue gas (through stack))
float other_heat_sink_coal		= 0.12; 	//proportion of total heat input that is lost to other sinks
float other_heat_sink_ng		= 0.2;
float other_heat_sink_nuc		= 0.0;
float other_heat_sink_oil		= 0.12;
float other_heat_sink_other		= 0.12;
float heat_sink_1				= 0.0;
float NamePlate_1		 		= 0.0;		// 1st Powerplant's nameplate in grid cell (MW)
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
float Technology_1				= 0.0;		// 1st Powerplant's Technology (Once thru = 1, Cooling tower = 2, Dry Cooling = 3, CC with OT = 4.1, CC with CT = 4.2, CC with DC = 4.3, Hybrid = 5)
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
float drybulbT					= 0.0;
float airT						= 0.0;
float ITD						= 12.0;
float output_2					= 0.0;
float DC_cond_heat_loss			= 0.0;
float totalDaily_heatToRiv	   = 0.0;
float totalDaily_heatToSink	   = 0.0;
float totalDaily_heatToEng	   = 0.0;
float eff_steam_loss1		   = 0.0;
float heatBalance1			   = 0.0;
float heatBalance2			   = 0.0;
float flux_GJ_new			   = 0.0;
float flux_GJ_old			   = 0.0;
float flux_GJ_old_wevap		   = 0.0;
float totalDaily_heat		   = 0.0;
float totalDaily_elec		   = 0.0;
float flux_GJ_old_prist		   = 0.0;
float non_operatingHrs_heat	   = 0.0;
float operatingHrs_heat		   = 0.0;
float total_cell_flux		   = 0.0;
float pHB2					   = 0.0;
float flux_GJ_old_prist2	   = 0.0;
float heatBalance3			   = 0.0;
float pHB3					   = 0.0;
float totalDaily_heatToElec	   = 0.0;
float totalGJ_heatToEng		   = 0.0;
float totalGJ_heatToSink	   = 0.0;
float totalGJ_heatToRiv		   = 0.0;
float totalGJ_heatToElec	   = 0.0;
float evaporated_heat		   = 0.0;
float totalGJ_heatToEvap	   = 0.0;

float cell						= 33;
float totalGJ_fluxNEW			= 0.0;
float totalGJ_fluxOLD			= 0.0;
float waterT_heatToRiv	 		= 0.0;
float percent_wTheatToRiv	    = 0.0;

float op_consumption		= 0.0;
float op_cons_bm			= 300;
float op_cons_coal			= 250;
float op_cons_ng			= 240;
float op_cons_nuc			= 269;
float op_cons_oil			= 300;
float op_cons_other			= 271.8;

float evaporation_2		     = 0.0;		// RJS 120912
float totalDaily_evap_2		 = 0.0;		// RJS 120912
float totalDaily_cons_2		 = 0.0;		// RJS 120912
float totalDaily_external_2	 = 0.0;		// RJS 120912
float totalDaily_heatToOcean = 0.0;		// RJS 010412

float Eff_loss_2			= 0.0;		// RJS 121912
float condenser_inlet		= 0.0;		// RJS 122112
float energy_Pen			= 0.0;		// RJS 122112
float year					= 0.0;		// RJS 122212
float exp_adj				= 0.0;		// RJS 052013 experiment adjust



	placeHolder			  = MFVarGetFloat (_MDPlaceHolderID,          itemID, 0.0);	// running MDWTempRiverRoute, value is previous water Temp
	flux_QxT			  = MFVarGetFloat (_MDFlux_QxTID,             itemID, 0.0);	// reading in discharge * temp (m3*degC/day)
	fluxmixing_QxT		  = MFVarGetFloat (_MDFluxMixing_QxTID,       itemID, 0.0);	// reading in discharge * tempmixing (m3*degC/day)
 	Q                     = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0);
 	Q_incoming_1		  = MFVarGetFloat (_MDInDischargeIncomingID, itemID, 0.0);	// already includes local runoff, uncommented 113012
// 	Q_incoming_1		  = MFVarGetFloat (_MDOutDischargeID,         itemID, 0.0);			// late-night discharge test (and one commented out line above), commented out 113012

	airT				  = MFVarGetFloat (_MDInAirTemperatureID,	  itemID, 0.0);	//read in air temperature (c)
  	NamePlate_1           = MFVarGetFloat (_MDInNamePlate1ID,         itemID, 0.0);
  	FuelType_1            = MFVarGetFloat (_MDInFuelType1ID,          itemID, 0.0);
 	Technology_1          = MFVarGetFloat (_MDInTechnology1ID,        itemID, 0.0);
 	Efficiency_1          = MFVarGetFloat (_MDInEfficiency1ID,        itemID, 0.0);
 	energyDemand_1        = MFVarGetFloat (_MDInEnergyDemand1ID,      itemID, 0.0);
	drybulbT			  = airT;
 	opt_efficiency_1	  = Efficiency_1;

 	bypass_percent			 = MFVarGetFloat (_MDInBypassPercentID,         itemID, 0.0);
 	Approach				 = MFVarGetFloat (_MDInApproachID,              itemID, 0.0);
  	riverThresh_temp		 = MFVarGetFloat (_MDInRiverThreshTempID,       itemID, 0.0);		// 22, needs to be in script
  	State					 = MFVarGetFloat (_MDInStateID, 				itemID, 0.0);
  	wetBulbT				 = MFVarGetFloat (_MDInWetBulbTempID,           itemID, 0.0);
  	LakeOcean				 = MFVarGetFloat (_MDInLakeOcean1ID,            itemID, 0.0);		// 1 is lakeOcean, 0 is nothing

  	year					 = MFDateGetCurrentYear();
  	year					 = year < 0.0 ? 2000 : year;
/****************************************************/

  //// Scenario 3, increased demand

  	// capacity change
//	if (Technology_1 > 4.00 && Technology_1 < 4.15)   NamePlate_1 = NamePlate_1 * 1.4626;			//
//  	if (Technology_1 > 4.16 && Technology_1 < 4.25)   NamePlate_1 = NamePlate_1 * 1.4626;			//
//  	if (Technology_1 > 4.26 && Technology_1 < 4.35)   NamePlate_1 = NamePlate_1 * 1.4626;			//

//	if (FuelType_1 == 2)   NamePlate_1 = NamePlate_1 * 0.7593;
//	if (FuelType_1 == 4)   NamePlate_1 = NamePlate_1 * 0.9779;
//	if (FuelType_1 == 5)   NamePlate_1 = NamePlate_1 * 0.7189;

	// demand change
//	if (Technology_1 > 4.00 && Technology_1 < 4.15)   energyDemand_1 = energyDemand_1 * ((0.0116 * (year - 2000.0)) + 1.3275);			//
//	if (Technology_1 > 4.16 && Technology_1 < 4.25)   energyDemand_1 = energyDemand_1 * ((0.0116 * (year - 2000.0)) + 1.3275);			//
//	if (Technology_1 > 4.26 && Technology_1 < 4.35)   energyDemand_1 = energyDemand_1 * ((0.0116 * (year - 2000.0)) + 1.3275);			//

//	if (FuelType_1 == 2)   energyDemand_1 = energyDemand_1 * ((0.0042 * (year - 2000.0)) + 0.9139);
//	if (FuelType_1 == 4)   energyDemand_1 = energyDemand_1 * ((0.0002 * (year - 2000.0)) + 0.9856);
//	if (FuelType_1 == 5)   energyDemand_1 = energyDemand_1 * ((-0.0039 * (year - 2000.0)) + 0.1296);

  //// End Scenario 3, increased demand

/****************************************************/

  	//// Scenario 4, upgraded cooling

//  	if (Technology_1 == 1.0) {
// 		Technology_1 = 2.0;
//  	  	energy_Pen   = 0.02 * NamePlate_1;
// 	}

//  	if (Technology_1 == 4.1) {
//  		Technology_1 = 4.2;
//  		energy_Pen   = 0.005 * NamePlate_1;
//  	}

  //// End Scenario 4, upgraded cooling

/***************************************************/

  // NABS Scenario
/*
  	if (itemID == 34)  {
  		NamePlate_1    = 600.0;
  		energyDemand_1 = energyDemand_1 * 17.0;
  	}

  	if (itemID == 30)  {
  		NamePlate_1    = 600.0;
  		energyDemand_1 = energyDemand_1 * 17.0;
  	}



	if (itemID == 104) NamePlate_1 = 0.00;
  	if (itemID == 379) NamePlate_1 = 0.00;
 // 	if (itemID == 34)  NamePlate_1 = 0.00;
 // 	if (itemID == 30)  NamePlate_1 = 0.00;
  	if (itemID == 22)  NamePlate_1 = 0.00;
  	if (itemID == 18)  NamePlate_1 = 0.00;
  	if (itemID == 10)  NamePlate_1 = 0.00;
  	if (itemID == 9)   NamePlate_1 = 0.00;
  	if (itemID == 6)   NamePlate_1 = 0.01;
  	if (itemID == 5)   NamePlate_1 = 0.01;
  	if (itemID == 130) NamePlate_1 = 0.00;

//  	if (itemID == 5)  {
 // 		NamePlate_1    = 600.0;
 // 		energyDemand_1 = energyDemand_1 * 17.0;
 // 	}

//  	if (itemID == 6)  {
//  		NamePlate_1    = 600.0;
//  		energyDemand_1 = energyDemand_1 * 17.0;
 // 	}
*/

/***************************************************/

// Experiment
/*
	if (State == 9) 	energyDemand_1 = 1.064 * energyDemand_1;	// CT	1x 1.353
	if (State == 10) 	energyDemand_1 = 0.844 * energyDemand_1;	// DE	1x 0.893
	if (State == 25) 	energyDemand_1 = 1.053 * energyDemand_1;	// MA	1x 1.752
	if (State == 24) 	energyDemand_1 = 0.955 * energyDemand_1;	// MD	1x 1.053
	if (State == 23) 	energyDemand_1 = 0.807 * energyDemand_1;	// ME	1x 1.386
	if (State == 33) 	energyDemand_1 = 1.119 * energyDemand_1;	// NH	1x 1.234
	if (State == 34) 	energyDemand_1 = 1.143 * energyDemand_1;	// NJ	1x 1.324
	if (State == 36) 	energyDemand_1 = 1.594 * energyDemand_1;	// NY	1x 2.076
	if (State == 42) 	energyDemand_1 = 1.058 * energyDemand_1;	// PA	1x 1.104
	if (State == 44) 	energyDemand_1 = 1.087 * energyDemand_1;	// RI	1x 1.423
	if (State == 7) 	energyDemand_1 = 1.259 * energyDemand_1;	// VA	1x 1.249
	if (State == 4) 	energyDemand_1 = 1.218 * energyDemand_1;	// VT	1x 1.330
	if (State == 11) 	energyDemand_1 = 1.000 * energyDemand_1;	// DC	1x 1.000
*/

/***************************************************/

  	if (energyDemand_1 > (NamePlate_1 * 24.0)) energyDemand_1 = NamePlate_1 * 24.0;				// added 122112


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
	if (State == 7) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_VAID,         itemID, 0.0);
	if (State == 4) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_VTID,         itemID, 0.0);
	if (State == 11) 	TempLimit  = MFVarGetFloat (_MDInTempLimit_DCID,         itemID, 0.0);

  	if (FuelType_1 == 1)   cond_h2o_1 = ((cond_h2o_bm    * 0.0037854) / 3600) * NamePlate_1;			// biomass, 0.0037854 converts gallons to m3. Units for cond_h2o_1 is m3/s
  	if (FuelType_1 == 2)   cond_h2o_1 = ((cond_h2o_coal  * 0.0037854) / 3600) * NamePlate_1;			// coal
  	if (FuelType_1 == 3)   cond_h2o_1 = ((cond_h2o_ng    * 0.0037854) / 3600) * NamePlate_1;			//
  	if (FuelType_1 == 4)   cond_h2o_1 = ((cond_h2o_nuc   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 5)   cond_h2o_1 = ((cond_h2o_oil   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 6)   cond_h2o_1 = ((cond_h2o_other * 0.0037854) / 3600) * NamePlate_1;			//

  	if (FuelType_1 == 1)   makeup_1 = ((makeup_bm    * 0.0037854) / 3600) * NamePlate_1;			// biomass, 0.0037854 converts gallons to m3. Units for cond_h2o_1 is m3/s
  	if (FuelType_1 == 2)   makeup_1 = ((makeup_coal  * 0.0037854) / 3600) * NamePlate_1;			// coal
  	if (FuelType_1 == 3)   makeup_1 = ((makeup_ng    * 0.0037854) / 3600) * NamePlate_1;			//
  	if (FuelType_1 == 4)   makeup_1 = ((makeup_nuc   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 5)   makeup_1 = ((makeup_oil   * 0.0037854) / 3600) * NamePlate_1;			//
	if (FuelType_1 == 6)   makeup_1 = ((makeup_other * 0.0037854) / 3600) * NamePlate_1;			//

  	if (FuelType_1 == 1)   op_consumption = ((op_cons_bm    * 0.0037854) / 3600) * NamePlate_1;			// 		120612	m3/s
  	if (FuelType_1 == 2)   op_consumption = ((op_cons_coal  * 0.0037854) / 3600) * NamePlate_1;			// 		120612
  	if (FuelType_1 == 3)   op_consumption = ((op_cons_ng    * 0.0037854) / 3600) * NamePlate_1;			//		120612
  	if (FuelType_1 == 4)   op_consumption = ((op_cons_nuc   * 0.0037854) / 3600) * NamePlate_1;			//		120612
	if (FuelType_1 == 5)   op_consumption = ((op_cons_oil   * 0.0037854) / 3600) * NamePlate_1;			//		120612
	if (FuelType_1 == 6)   op_consumption = ((op_cons_other * 0.0037854) / 3600) * NamePlate_1;			//		120612

	if (Technology_1 > 4.0 && Technology_1 < 4.15) op_consumption = ((100 * 0.0037854) / 3600) * NamePlate_1;	// 120612

	if (Technology_1 > 4.0 && Technology_1 < 4.15)	cond_h2o_1 = ((cond_h2o_ngccOT * 0.0037854) / 3600) * NamePlate_1;
	if (Technology_1 > 4.16 && Technology_1 < 4.25)	cond_h2o_1 = ((cond_h2o_ngccRC * 0.0037854) / 3600) * NamePlate_1;
	if (Technology_1 > 4.26 && Technology_1 < 4.35)	cond_h2o_1 = ((cond_h2o_ngccDC * 0.0037854) / 3600) * NamePlate_1;

	if (Technology_1 > 4.0 && Technology_1 < 4.15)    makeup_1 = ((makeup_ngccOT * 0.0037854) / 3600) * NamePlate_1;			//
  	if (Technology_1 > 4.16 && Technology_1 < 4.25)   makeup_1 = ((makeup_ngccRC * 0.0037854) / 3600) * NamePlate_1;			//
  	if (Technology_1 > 4.26 && Technology_1 < 4.35)   makeup_1 = ((makeup_ngccDC * 0.0037854) / 3600) * NamePlate_1;			//

	if (FuelType_1 == 1)   heat_sink_1 = other_heat_sink_bm;
	if (FuelType_1 == 2)   heat_sink_1 = other_heat_sink_coal;
	if (FuelType_1 == 3)   heat_sink_1 = other_heat_sink_ng;
	if (FuelType_1 == 4)   heat_sink_1 = other_heat_sink_nuc;
	if (FuelType_1 == 5)   heat_sink_1 = other_heat_sink_oil;
	if (FuelType_1 == 6)   heat_sink_1 = other_heat_sink_other;

	Q_in_1		= NamePlate_1 / Efficiency_1;
	heat_sink_1	= heat_sink_1 * Q_in_1;
	optDeltaT_1 = (Q_in_1 - NamePlate_1 - heat_sink_1) / (cond_h2o_1 * 4.18);		//MWh lost to other heat sinks
//	optDeltaT_1 = -(((NamePlate_1 * (1.0 -(1.0 / Efficiency_1))) + heat_sink_1) / (cond_h2o_1 * 4.18));				// optimal temperature increase of condenser water (cost effective)														// heat in
	opt_QO_1	= optDeltaT_1 * 4.18 * cond_h2o_1;

	energyDemand_1 	= fabs(energyDemand_1);	// RJS 120612

//	if (itemID == cell) printf("itemID = %d, %d %d %d, NamePlate_1 = %f, FuelType_1 = %f, opt_efficiency_1 = %f, LakeOcean = %f, energyDemand_1 = %f, Technology_1 = %f, airT = %f,\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1, FuelType_1, opt_efficiency_1, LakeOcean, energyDemand_1, Technology_1, airT);
//	if (itemID == cell) printf("*** optDeltaT_1 = %f, cond_h20_1 = %f\n", optDeltaT_1, cond_h2o_1);

	if (Q > 0.000001) {

		Q_WTemp				 = flux_QxT / (Q * 86400);			// degC RJS 013112
		pot_WTemp_1			 = NamePlate_1 > 0.0 ? 9999 : Q_WTemp;			// degC - this is the potential river temperature that power plant will produce based on its actions
//		Q_WTemp_mix			 = fluxmixing_QxT / (Q * 86400);		// degC RJS 013112
		bypass_Q			 = bypass_percent * Q;				// amount of flow that cannot be withdrawn
		alpha				 = 1.0 / 20.0;									// exponent of withdrawal

//		if (itemID == cell) {
//		printf("**** y = %d, m = %d, d = %d, Q = %f, NameP = %f, FuelT = %f, Tech = %f, Eff = %f, Demand = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q, NamePlate_1, FuelType_1, Technology_1, Efficiency_1, energyDemand_1);
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



			if (((Q - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_temp)) {		// enough water to satisfy optimal wdl, BUT water above 22
//				if (itemID == cell) printf("!!!!! Plenty of Water BUT Temperature TOO WARM (1)\n");
				calc_T_1     = Q_WTemp - riverThresh_temp;
				Eff_loss_1   = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
				Efficiency_1 = opt_efficiency_1 - Eff_loss_1;		// edited 123112

				Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
				dTemp_1      = optDeltaT_1;
				wdl_1        = (Qpp_1 -heat_sink_1)/ (dTemp_1 * 4.18);
				output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1       = (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1       = Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

					if (pot_WTemp_1 > TempLimit) {
						dTemp_1 = dTemp_1 - 0.5;
//						if (itemID == cell) printf("! dTemp DECREASING because of Limit (2)\n");
					}
					wdl_coeff_1 = pow((optDeltaT_1 / dTemp_1), alpha);
					wdl_1		= wdl_1 * wdl_coeff_1;
//					if (itemID == cell) printf(" wdl_coeff_1 = %f, dTemp_1 = %f, wdl_1 = %f, Qpp_1 = %f\n", wdl_coeff_1, dTemp_1, wdl_1, Qpp_1);
//					if (itemID == cell) printf("pot_WTemp_1 = %f, TempLimit = %f, heat_sink_1 = %f\n", pot_WTemp_1, TempLimit, heat_sink_1);
				}



				if (wdl_1 > (Q - bypass_Q)) {						// if wdl is greater than what is in river
//					if (itemID == cell) printf("!!!!! ADJUSTING for withdrawal (3)\n");
					wdl_1       = Q - bypass_Q;
					Qpp_1       = (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
					adj_dummy	= 1.0;
				}

				if (dTemp_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation

				if ((TempLimit == 0.0) && (adj_dummy != 1.0)) {				// RJS 123112
//				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT 2 (4)\n");
					calc_T_1     = Q_WTemp - riverThresh_temp;
					Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2)) + (0.1604 * calc_T_1)) / 100;
					Efficiency_1 = opt_efficiency_1 - Eff_loss_1;	// edit 123112

					Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
					dTemp_1      = optDeltaT_1;
					wdl_1        = (Qpp_1 - heat_sink_1) / (dTemp_1 * 4.18);
					output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

					Tpp_1		 = Q_WTemp + dTemp_1;
					pot_WTemp_1  = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "



			}			// this ends the "enough water for optimal wdl, but high temp" scenario ((Q_incoming - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_Temp))

			if ((Q_WTemp < riverThresh_temp) && (wdl_1 <= (Q - bypass_Q))) {
//				if (itemID == cell) printf("!!!!! PLENTY of water AND temperature is GOOD (5)\n");
				dTemp_1 	= optDeltaT_1;
				wdl_1		= cond_h2o_1;
				Qpp_1		= (1.0 - Efficiency_1) * Q_in_1;

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1  		= (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1 	= (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= dTemp_1 + Q_WTemp;
					pot_WTemp_1  = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

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
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "


			}			// this ends the "(Q_WTemp < riverThresh_Temp) && (wdl_1 <= (Q_incoming - bypass_Q))"



			if (((Q - bypass_Q) < wdl_1) && (adj_dummy != 1.0)) {								// less water available than optimal wdl
//				if (itemID == cell) printf("!!!!! Discharge is NOT enough for wdl (8)\n");
				wdl_1 = Q - bypass_Q;

				if (Q_WTemp >= riverThresh_temp) {
//					if (itemID == cell) printf("!! Temperature is TOO warm (9)\n");
						calc_T_1     = Q_WTemp - riverThresh_temp;
						Eff_loss_1   = ((0.0165 * pow(calc_T_1,2)) + (0.1604 * calc_T_1)) / 100;
						Efficiency_1 = opt_efficiency_1 - Eff_loss_1;		// edited 123112
				}

				Qpp_1 = (1.0 - Efficiency_1) * Q_in_1;			// heat out

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (wdl_1 > 0.0)){
						deltaT_1 = (Qpp_1 - heat_sink_1) / (wdl_1 * 4.18);
						if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
						else if ((Q_WTemp > 20.0) && (Q_WTemp <= 30.0)) dT_max_1 = 20.0;
						else dT_max_1 = optDeltaT_1;

						 if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
						 else dTemp_1 = deltaT_1;

						 Qpp_1  	 = (wdl_1 * dTemp_1 * 4.18)+heat_sink_1;
						 output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
						 Tpp_1	     = dTemp_1 + Q_WTemp;
						 pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

						 if (pot_WTemp_1 > TempLimit) {
							 wdl_1 = wdl_1 - 0.01;
//							 if (itemID == cell) printf("! Wdl DECREASING because of Limit (10)\n");
						 }

				}

				if (wdl_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation


				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT (11)\n");
						deltaT_1 = (Qpp_1 - heat_sink_1) / (wdl_1 * 4.18);
					if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
					else if ((Q_WTemp > 20.0) && (Q_WTemp <= 30.0)) dT_max_1 = 20.0;
					else dT_max_1 = optDeltaT_1;

					if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
					else dTemp_1 = deltaT_1;

					Qpp_1  		= (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1	    = dTemp_1 + Q_WTemp;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
				}

				wdl_1       = (wdl_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (wdl_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (wdl_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;	// "

			}				// this ends "less water available than optimal wdl" (Q_incoming - bypass_Q) < wdl_1)


			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;		// effluent from cooling towers
			OT_effluent_1 = wdl_1;		// return flow (or effluent) from power plant to river

//			if ((itemID == 47013) || (itemID == 47699) || (itemID == 47694)) {
//			if (itemID == cell) {
//			printf("y = %d, m = %d d = %d, NP = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1);
//			printf("Q = %f, bypass_Q = %f, wdl_1 = %f\n", Q, bypass_Q, wdl_1);
//			printf("dTemp_1 = %f, Qpp_1 = %f\n", dTemp_1, Qpp_1);
//			printf("Tpp_1 = %f, Eff_loss_1 = %f, Efficiency_1 = %f\n", Tpp_1, Eff_loss_1, Efficiency_1);
//			printf("riverThresh_temp = %f, dTemp_1 = %f, calc_T_1 = %f\n", riverThresh_temp, dTemp_1, calc_T_1);
//			printf("wdl_coeff_1 = %f, heat_sink_1 = %f\n", wdl_coeff_1, heat_sink_1);
//			printf("output_1 = %f, pot_WTemp_1 = %f\n", output_1, pot_WTemp_1);
//			}

		}				// this ends "once through"

		if (Technology_1 == 2.0) {				// re-circulating technology
			wdl_1 		  = makeup_1;
			circulating_1 = cond_h2o_1;
			Temp_In_1	  = wetBulbT + Approach;
			blowdown_1	  = (1.0 / concentration) * wdl_1;
			evaporation_1 = blowdown_1 * (concentration - 1.0);
			LH_fract	  = LH_vap * (evaporation_1 / (opt_QO_1));			// temporary attempt


	//			if (itemID == cell) printf("Entering Recirculating...\n");



			if (Temp_In_1 >= riverThresh_temp) {
	//			if (itemID == cell) printf("!!!!! Cooling Water Temperature HIGHER than Thresh\n");
				dTemp_1 	  = optDeltaT_1;
	//			calc_T_1      = Q_WTemp - riverThresh_temp;		// commented out 121212
				calc_T_1      = Temp_In_1 - riverThresh_temp;		// added 121212
				Eff_loss_1    = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
				Efficiency_1 = opt_efficiency_1 - Eff_loss_1;		// edited 123112
				Qpp_1		  = (1.0 - Efficiency_1) * Q_in_1;
				circulating_1 = (Qpp_1-heat_sink_1) / (dTemp_1 * 4.18);
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				makeup_1	  = makeup_1 + (circulating_1 - cond_h2o_1);
			}

			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 < riverThresh_temp)) {
	//			if (itemID == cell) printf("!!!!! Plenty of Water AND Temperature is good\n");
				dTemp_1		   = optDeltaT_1;
				wdl_1		   = makeup_1;
				blowdown_1	   = (1.0 / concentration) * wdl_1;
				evaporation_1  = blowdown_1 * (concentration - 1.0);
				Qpp_1		   = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1;
				circulating_1  = (Qpp_1 -heat_sink_1) / (4.18 * dTemp_1);
				output_1 	   = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		   = Temp_In_1;
				pot_WTemp_1    = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
			}



			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 >= riverThresh_temp)) {
	//			if (itemID == cell) printf("!!!!! More than enough Q, Cooling Temperature is too warm\n");
				dTemp_1		  = optDeltaT_1;
				evaporation_1 = (LH_fract / LH_vap) * (Qpp_1 - heat_sink_1);
				blowdown_1	  = (evaporation_1 / (concentration - 1.0));
				wdl_1		  = (blowdown_1 / (1.0 / concentration));

				if ((Q - bypass_Q) < wdl_1) {
//					if (itemID == cell) printf("!! AND Discharge minus bypass LESS than wdl\n");
					wdl_1 		  = Q - bypass_Q;
					dTemp_1		  = optDeltaT_1;
					blowdown_1	  = (1.0 / concentration) * wdl_1;
					evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
					Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap))+heat_sink_1;
					circulating_1 = (Qpp_1-heat_sink_1) / (4.18 * dTemp_1);
					output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		  = Temp_In_1;
					pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
				}

				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Q_WTemp;
				pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

			}

			if ((Q - bypass_Q) < wdl_1) {
	//			if (itemID == cell) printf("!!!!! Discharge minus bypass LESS than wdl\n");
				wdl_1 		  = Q - bypass_Q;
				dTemp_1		  = optDeltaT_1;
				blowdown_1	  = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
				Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap))+heat_sink_1;
				circulating_1 = (Qpp_1-heat_sink_1) / (4.18 * dTemp_1);
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Temp_In_1;
				pot_WTemp_1   = (((Q- wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

			}

			OT_effluent_1 = 0.0;
			LH_fract_post = LH_fract * (evaporation_1 / Qpp_1);			// temporary attempt

		}		// this ends re-circulating technology



//START OF DRY COOLING//

		if (Technology_1 == 3.0) {				// dry-cooling technology
			a = 0.0;
			Temp_In_1 = drybulbT + ITD;

//			if (Temp_In_1 <= 20.0)	{						// commented out 121212
			if (Temp_In_1 <= riverThresh_temp)	{			// added 121212
			output_1		= Efficiency_1 * Q_in_1;
			pot_WTemp_1		= Q_WTemp;
			wdl_1			= 0.0;
			evaporation_1	= 0.0;
			}

//			if (Temp_In_1 > 20.0)	{						// commented out 121212
			if (Temp_In_1 > riverThresh_temp)	{			// added 121212
//			calc_T_1	   = Temp_In_1 - 20.0;				// commented out 121212
			calc_T_1	   = Temp_In_1 - riverThresh_temp;	// added 121212
			Eff_loss_1     = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
			Efficiency_1 = opt_efficiency_1 - Eff_loss_1;		// edited 123112
			output_1	   = Efficiency_1 * Q_in_1;
			pot_WTemp_1	   = Q_WTemp;
			wdl_1		   = 0.0;
			evaporation_1  = 0.0;
			}

			DC_cond_heat_loss = Q_in_1 - (output_1 + heat_sink_1);

//			if (itemID == cell) printf("airT = %f, calc_T_1 = %f, Efficiency_1 = %f, output_1 = %f, Q_WTemp = %f, pot_WTemp_1 = %f\n", airT, calc_T_1, Efficiency_1, output_1, Q_WTemp, pot_WTemp_1);

		}

//END OF DRY COOLING//

//START OF COMBINED CYCLE COOLING//

		if (Technology_1 > 4.0 && Technology_1 < 4.15) {				// COMBINED CYCLE OT technology

//			if (itemID == cell) printf("Entering once-through ...\n");
			wdl_1 = cond_h2o_1;



			if (((Q - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_temp)) {		// enough water to satisfy optimal wdl, BUT water above 22
//				if (itemID == cell) printf("!!!!! Plenty of Water BUT Temperature TOO WARM (1)\n");
				eff_steam	 = Efficiency_1 * 0.526315789;
				eff_gas		 = (Efficiency_1 - eff_steam) / (1.0 - eff_steam);
				calc_T_1     = Q_WTemp - riverThresh_temp;
				Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
				eff_steam 	 = eff_steam - Eff_loss_1;								// edited 122612
				if (airT > 20.0) Eff_loss_2 = (0.5845 / 100.0) * (airT - 20.0);		// added 121912
				if (airT <= 20.0) Eff_loss_2 = 0.0;									// added 121912
				eff_gas      = eff_gas * (1.0 - Eff_loss_2);						// edited 122612
				Efficiency_1 = eff_steam + eff_gas - (eff_steam * eff_gas);


				Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
				dTemp_1      = optDeltaT_1;
				wdl_1        = (Qpp_1 - heat_sink_1) / (dTemp_1 * 4.18);
				output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1       = (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1       = Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

					if (pot_WTemp_1 > TempLimit) {
						dTemp_1 = dTemp_1 - 0.5;
//						if (itemID == cell) printf("! dTemp DECREASING because of Limit (2)\n");
					}
					wdl_coeff_1 = pow((optDeltaT_1 / dTemp_1),alpha);
					wdl_1		= wdl_1 * wdl_coeff_1;
//					if (itemID == cell) printf(" wdl_coeff_1 = %f, dTemp_1 = %f, wdl_1 = %f, Qpp_1 = %f\n", wdl_coeff_1, dTemp_1, wdl_1, Qpp_1);
//					if (itemID == cell) printf("pot_WTemp_1 = %f, TempLimit = %f\n", pot_WTemp_1, TempLimit);
				}



				if (wdl_1 > (Q - bypass_Q)) {						// if wdl is greater than what is in river
//					if (itemID == cell) printf("!!!!! ADJUSTING for withdrawal (3)\n");
					wdl_1       = Q - bypass_Q;
					Qpp_1       = (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= Q_WTemp + dTemp_1;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
					adj_dummy	= 1.0;
				}

				if (dTemp_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation

				if ((TempLimit == 0.0) && (adj_dummy != 1.0)) {			//RJS 123112
//				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT 2 (4)\n");

					eff_steam	 = Efficiency_1 * 0.526315789;
					eff_gas		 = (Efficiency_1 - eff_steam) / (1.0 - eff_steam);
					calc_T_1     = Q_WTemp - riverThresh_temp;
					Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
					eff_steam 	 = eff_steam - Eff_loss_1;				// edited 122612
					if (airT > 20.0) Eff_loss_2 = (0.5845 / 100.0) * (airT - 20.0);		// added 121912
					if (airT <= 20.0) Eff_loss_2 = 0.0;									// added 121912
					eff_gas      = eff_gas * (1.0 - Eff_loss_2);					// edited  122612
					Efficiency_1 = eff_steam + eff_gas - (eff_steam * eff_gas);


					Qpp_1        = (1.0 - Efficiency_1) * Q_in_1;
					dTemp_1      = optDeltaT_1;
					wdl_1        = (Qpp_1 - heat_sink_1)/ (dTemp_1 * 4.18);
					output_1     = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));

					Tpp_1		 = Q_WTemp + dTemp_1;
					pot_WTemp_1  = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "



			}			// this ends the "enough water for optimal wdl, but high temp" scenario ((Q_incoming - bypass_Q) >= wdl_1) && (Q_WTemp >= riverThresh_Temp))

			if ((Q_WTemp < riverThresh_temp) && (wdl_1 <= (Q - bypass_Q))) {
//				if (itemID == cell) printf("!!!!! PLENTY of water AND temperature is GOOD (5)\n");
				dTemp_1 	= optDeltaT_1;
				wdl_1		= cond_h2o_1;
				Qpp_1		= (1.0 - Efficiency_1) * Q_in_1;

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (dTemp_1 > 0.0)) {
					Qpp_1  		= (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1 	= (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		= dTemp_1 + Q_WTemp;
					pot_WTemp_1  = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

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
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

				}

				wdl_1       = (dTemp_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (dTemp_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (dTemp_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;		// "


			}			// this ends the "(Q_WTemp < riverThresh_Temp) && (wdl_1 <= (Q_incoming - bypass_Q))"



			if (((Q - bypass_Q) < wdl_1) && (adj_dummy != 1.0)) {								// less water available than optimal wdl
//				if (itemID == cell) printf("!!!!! Discharge is NOT enough for wdl (8)\n");
				wdl_1 = Q - bypass_Q;

				if (Q_WTemp >= riverThresh_temp) {
//					if (itemID == cell) printf("!! Temperature is TOO warm (9)\n");
					eff_steam	 = Efficiency_1 * 0.526315789;
					eff_gas		 = (Efficiency_1 - eff_steam) / (1.0 - eff_steam);
					calc_T_1     = Q_WTemp - riverThresh_temp;
					Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
					eff_steam 	 = eff_steam - Eff_loss_1;								// edited 122612
					if (airT > 20.0) Eff_loss_2 = (0.5845 / 100.0) * (airT - 20.0);		// added 121912
					if (airT <= 20.0) Eff_loss_2 = 0.0;									// added 121912
					eff_gas      = eff_gas * (1.0 - Eff_loss_2);						// added 122612
					Efficiency_1 = eff_steam + eff_gas - (eff_steam * eff_gas);
				}



				Qpp_1 = (1.0 - Efficiency_1) * Q_in_1;			// total heat out (waste)

				while ((pot_WTemp_1 > TempLimit) && (TempLimit > 0.0) && (wdl_1 > 0.0)){
						deltaT_1 = (Qpp_1 - heat_sink_1) / (wdl_1 * 4.18);
						if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
						else if ((Q_WTemp > 20.0) && (Q_WTemp <= 30.0)) dT_max_1 = 20.0;
						else dT_max_1 = optDeltaT_1;

						 if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
						 else dTemp_1 = deltaT_1;

						Qpp_1  		 = (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
						 output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
						 Tpp_1	     = dTemp_1 + Q_WTemp;
						 pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;

						 if (pot_WTemp_1 > TempLimit) {
							 wdl_1 = wdl_1 - 0.01;
//							 if (itemID == cell) printf("! Wdl DECREASING because of Limit (10)\n");
						 }

				}

				if (wdl_1 <= 0.0) a = 1.0;		// when dTemp_1 <= 0 no Operation


				if (TempLimit == 0.0) {
//					if (itemID == cell) printf("!!!!! NO TEMPERATURE LIMIT (11)\n");
						deltaT_1 = (Qpp_1 - heat_sink_1) / (wdl_1 * 4.18);
					if (Q_WTemp <= 20.0) dT_max_1 = 30.0;
					else if ((Q_WTemp > 20.0) && (Q_WTemp <= 30.0)) dT_max_1 = 20.0;
					else dT_max_1 = optDeltaT_1;

					if (deltaT_1 > dT_max_1) dTemp_1 = dT_max_1;
					else dTemp_1 = deltaT_1;

					Qpp_1  		= (wdl_1 * dTemp_1 * 4.18) + heat_sink_1;
					output_1    = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1	    = dTemp_1 + Q_WTemp;
					pot_WTemp_1 = (((Q - wdl_1) * Q_WTemp) + (wdl_1 * Tpp_1)) / Q;
				}

				wdl_1       = (wdl_1 <= 0.0) ? 0.0 : wdl_1;				// if wdl_1 is less than 0, then wdl_1 = 0, otherwise keep wdl_1
				output_1    = (wdl_1 <= 0.0) ? 0.0 : output_1;			// "
				pot_WTemp_1 = (wdl_1 <= 0.0) ? Q_WTemp : pot_WTemp_1;	// "

			}				// this ends "less water available than optimal wdl" (Q_incoming - bypass_Q) < wdl_1)


			evaporation_1 = 0.0;
			blowdown_1	  = 0.0;		// effluent from cooling towers
			OT_effluent_1 = wdl_1;		// return flow (or effluent) from power plant to river

//			if (itemID == cell) {
//			printf("itemID = %d, %d %d %d, NamePlate_1 = %f, FuelType_1 = %f, opt_efficiency_1 = %f, LakeOcean = %f \n energyDemand_1 = %f, Technology_1 = %f, airT = %f, wetbulbT = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1, FuelType_1, opt_efficiency_1, LakeOcean, energyDemand_1, Technology_1, airT, wetBulbT);
//			printf("output_1 = %f, pot_WTemp_1 = %f, Q_WTemp = %f, eff_gas = %f, eff_steam = %f, calc_T_1 = %f, Efficiency_1 = %f\n", output_1, pot_WTemp_1, Q_WTemp, eff_gas, eff_steam, calc_T_1, Efficiency_1);
//			printf("Qpp_1 = %f, heat_sink_1 = %f, circulating_1 = %f, dTemp_1 = %f, Q_in_1 = %f\n", Qpp_1, heat_sink_1, circulating_1, dTemp_1, Q_in_1);
//			printf("Q = %f, bypass_Q = %f, wdl_1 = %f, eff_steam_loss1 = %f, makeup_1 = %f\n", Q, bypass_Q, wdl_1, eff_steam_loss1, makeup_1);
//			}

		}				// this ends COMBINED CYCLE ONCE THROUGH


		if (Technology_1 > 4.16 && Technology_1 < 4.25) {				// COMBINED CYCLE re-circulating technology
			wdl_1 		  = makeup_1;
			circulating_1 = cond_h2o_1;
			Temp_In_1	  = wetBulbT + Approach;
			blowdown_1	  = (1.0 / concentration) * wdl_1;
			evaporation_1 = blowdown_1 * (concentration - 1.0);
			LH_fract	  = LH_vap * (evaporation_1 / (opt_QO_1));			// temporary attempt


//				if (itemID == cell) printf("Entering Recirculating...\n");



			if (Temp_In_1 >= riverThresh_temp) {
//				if (itemID == cell) printf("!!!!! Cooling Water Temperature HIGHER than Thresh\n");
				dTemp_1 	 = optDeltaT_1;
				eff_steam	 = Efficiency_1 * 0.526315789;
				eff_gas		 = (Efficiency_1 - eff_steam) / (1.0 - eff_steam);
//				calc_T_1      = Q_WTemp - riverThresh_temp;			// commented out 121212
				calc_T_1      = Temp_In_1 - riverThresh_temp;		// added 121212
				Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
				eff_steam 	 = eff_steam - Eff_loss_1;								// added 122612
				if (airT > 20.0) Eff_loss_2 = (0.5845 / 100.0) * (airT - 20.0);		// added 121912
				if (airT <= 20.0) Eff_loss_2 = 0.0;									// added 121912
				eff_gas      = eff_gas * (1.0 - Eff_loss_2);								// added 122612
				Efficiency_1 = eff_steam + eff_gas - (eff_steam * eff_gas);


				Qpp_1		  = (1.0 - Efficiency_1) * Q_in_1;
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				circulating_1 = (Q_in_1 - output_1 - heat_sink_1) / (dTemp_1 * 4.18);
				makeup_1	  = makeup_1 + (circulating_1 - cond_h2o_1);

//				if (itemID == cell)	printf("Qpp_1 = %f, heat_sink_1 = %f, circulating_1 = %f, dTemp_1 = %f, Q_in_1 = %f\n", Qpp_1, heat_sink_1, circulating_1, dTemp_1, Q_in_1);
			}

			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 < riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! Plenty of Water AND Temperature is good\n");
				dTemp_1		   = optDeltaT_1;
				wdl_1		   = makeup_1;
				blowdown_1	   = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);
				Qpp_1		   = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1;
				circulating_1  = (Qpp_1 - heat_sink_1) / (4.18 * dTemp_1);
				output_1 	   = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		   = Temp_In_1;
				pot_WTemp_1    = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
			}



			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 >= riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! More than enough Q, Cooling Temperature is too warm\n");
				dTemp_1		  = optDeltaT_1;
				evaporation_1 = (LH_fract / LH_vap) * (Qpp_1 - heat_sink_1);
				blowdown_1	  = (evaporation_1 / (concentration - 1.0));
				wdl_1		  = (blowdown_1 / (1.0 / concentration));

				if ((Q - bypass_Q) < wdl_1) {
//					if (itemID == cell) printf("!! AND Discharge minus bypass LESS than wdl\n");
					wdl_1 		  = Q - bypass_Q;
					dTemp_1		  = optDeltaT_1;
					blowdown_1	  = (1.0 / concentration) * wdl_1;
					evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
					Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1;
					circulating_1 = (Qpp_1 - heat_sink_1) / (4.18 * dTemp_1);
					output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		  = Temp_In_1;
					pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
				}

				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Temp_In_1;
				pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

//			if (itemID == cell)	printf("Qpp_1 = %f, heat_sink_1 = %f, circulating_1 = %f, dTemp_1 = %f, Q_in_1 = %f\n", Qpp_1, heat_sink_1, circulating_1, dTemp_1, Q_in_1);
			}

			if ((Q - bypass_Q) < wdl_1) {
//				if (itemID == cell) printf("!!!!! Discharge minus bypass LESS than wdl\n");
				wdl_1 		  = Q - bypass_Q;
				dTemp_1		  = optDeltaT_1;
				blowdown_1	  = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
				Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1;
				circulating_1 = (Qpp_1 - heat_sink_1) / (4.18 * dTemp_1);
				output_1 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Temp_In_1;
				pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

			}

			OT_effluent_1 = 0.0;
			LH_fract_post = LH_fract * (evaporation_1 / Qpp_1);			// temporary attempt

//			if (itemID == cell) {
//			printf("itemID = %d, %d %d %d, NamePlate_1 = %f, FuelType_1 = %f, opt_efficiency_1 = %f, LakeOcean = %f \n energyDemand_1 = %f, Technology_1 = %f, airT = %f, wetbulbT = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1, FuelType_1, opt_efficiency_1, LakeOcean, energyDemand_1, Technology_1, airT, wetBulbT);
//			printf("output_1 = %f, pot_WTemp_1 = %f, Q_WTemp = %f, eff_gas = %f, eff_steam = %f, calc_T_1 = %f, Efficiency_1 = %f\n", output_1, pot_WTemp_1, Q_WTemp, eff_gas, eff_steam, calc_T_1, Efficiency_1);
//			printf("Qpp_1 = %f, heat_sink_1 = %f, circulating_1 = %f, dTemp_1 = %f, Q_in_1 = %f\n", Qpp_1, heat_sink_1, circulating_1, dTemp_1, Q_in_1);
//			printf("Q = %f, bypass_Q = %f, wdl_1 = %f, eff_steam_loss1 = %f, makeup_1 = %f\n", Q, bypass_Q, wdl_1, eff_steam_loss1, makeup_1);
//			}

		}		// this ends COMBINED CYCLE re-circulating technology


//START OF COMBINED CYCLE DRY COOLING//

		if (Technology_1 > 4.26 && Technology_1 < 4.35) {				// COMBINED CYCLE dry-cooling technology
			a = 0.0;
			Temp_In_1 = drybulbT + ITD;

//			if (Temp_In_1 <= 20.0)	{					// commented out 121212
			if (Temp_In_1 <= riverThresh_temp)	{		// added 	121212
			output_1	  = Efficiency_1 * Q_in_1;
			pot_WTemp_1	  = Q_WTemp;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			}

//			if (Temp_In_1 > 20.0)	{					// commented out 121212
			if (Temp_In_1 > riverThresh_temp)	{		// added 121212


				eff_steam	 = Efficiency_1 * 0.526315789;
				eff_gas		 = (Efficiency_1 - eff_steam) / (1.0 - eff_steam);
//				calc_T_1      = Q_WTemp - riverThresh_temp;			// commented out 121212
				calc_T_1      = Temp_In_1 - riverThresh_temp;		// added 121212
				Eff_loss_1   = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
				eff_steam 	 = eff_steam - Eff_loss_1;								// added 122612
				if (airT > 20.0) Eff_loss_2 = (0.5845 / 100.0) * (airT - 20.0);		// added 121912
				if (airT <= 20.0) Eff_loss_2 = 0.0;									// added 121912
				eff_gas      = eff_gas * (1.0 - Eff_loss_2);						// added 122612
				Efficiency_1 = eff_steam + eff_gas - (eff_steam * eff_gas);


			output_1	  = Efficiency_1 * Q_in_1;
			pot_WTemp_1	  = Q_WTemp;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			}

			DC_cond_heat_loss = Q_in_1 - (output_1 + heat_sink_1);

		}

//END OF COMBINED CYCLE DRY COOLING//


//START OF HYBRID COOLING//

		if (Technology_1 == 5.0) {				// HYBRID technology -- 1/3 DRY, 2/3 COOLING TOWER

			if (airT <= 20.0) {			// start of new if statement, added 121912
						a = 0.0;
						Temp_In_1 = drybulbT + ITD;
			//			if (Temp_In_1 <= 20.0)	{						// commented out 121212
						if (Temp_In_1 <= riverThresh_temp)	{			// added 121212
						output_1		= Efficiency_1 * Q_in_1;
						pot_WTemp_1		= Q_WTemp;
						wdl_1			= 0.0;
						evaporation_1	= 0.0;
						}

			//			if (Temp_In_1 > 20.0)	{						// commented out 121212
						if (Temp_In_1 > riverThresh_temp)	{			// added 121212
			//			calc_T_1	   = Temp_In_1 - 20.0;				// commented out 121212
						calc_T_1	   = Temp_In_1 - riverThresh_temp;	// added 121212
						Eff_loss_1     = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
						Efficiency_1   = Efficiency_1 - Eff_loss_1;				// edited 122612
						output_1	   = Efficiency_1 * Q_in_1;
						pot_WTemp_1	   = Q_WTemp;
						wdl_1		   = 0.0;
						evaporation_1  = 0.0;
						}

						DC_cond_heat_loss = Q_in_1 - (output_1 + heat_sink_1);
			}		// end of new if statement added 121912

			if (airT > 20.0) {					// added 121912
//DRY COOLING PART
			Temp_In_1 = drybulbT + ITD;

//			if (Temp_In_1 <= 20.0)	{						//commented out 121212
			if (Temp_In_1 <= riverThresh_temp)	{			// added 121212
			output_1	  = Efficiency_1 * Q_in_1;
			pot_WTemp_1	  = Q_WTemp;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			}

//			if (Temp_In_1 > 20.0)	{						// commented out 121212
			if (Temp_In_1 > riverThresh_temp)	{			// added 121212
//			calc_T_1      = Temp_In_1 - 20.0;				// commented out 121212
			calc_T_1      = Temp_In_1 - riverThresh_temp;	// added 121212
			Eff_loss_1    = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
			Efficiency_1  = Efficiency_1 - Eff_loss_1;		// edited 122612
			output_1	  = Efficiency_1 * (Q_in_1 / 3);
			pot_WTemp_1	  = Q_WTemp;
			wdl_1		  = 0.0;
			evaporation_1 = 0.0;
			}

//WET COOLING PART

			wdl_1 		  = makeup_1;
			circulating_1 = cond_h2o_1;
			Temp_In_1	  = wetBulbT + Approach;
			blowdown_1	  = (1.0 / concentration) * wdl_1;
			evaporation_1 = blowdown_1 * (concentration - 1.0);
			LH_fract	  = LH_vap * (evaporation_1 / (opt_QO_1));			// temporary attempt


//				if (itemID == cell) printf("Entering Recirculating...\n");



			if (Temp_In_1 >= riverThresh_temp) {
//				if (itemID == cell) printf("!!!!! Cooling Water Temperature HIGHER than Thresh\n");
				dTemp_1 	  = optDeltaT_1;
//				calc_T_1      = Q_WTemp - riverThresh_temp;			// commented out 121212
				calc_T_1      = Temp_In_1 - riverThresh_temp;		// added 121212
				Eff_loss_1    = ((0.0165 * pow(calc_T_1, 2.0)) + (0.1604 * calc_T_1)) / 100.0;
				Efficiency_1  = Efficiency_1 - Eff_loss_1;					// edited 122612
				Qpp_1		  = (1.0 - Efficiency_1) * ((2 * Q_in_1) / 3.0);
				circulating_1 = (Qpp_1-heat_sink_1) / (dTemp_1 * 4.18);
				output_2 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				makeup_1	  = makeup_1 + (circulating_1-cond_h2o_1);
			}

			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 < riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! Plenty of Water AND Temperature is good\n");
				dTemp_1		   = optDeltaT_1;
				wdl_1		   = makeup_1;
				blowdown_1	   = (1.0 / concentration) * wdl_1;
				evaporation_1  = blowdown_1 * (concentration - 1.0);
//				LH_fract	   = LH_vap * (evaporation_1 / (1.0 - opt_efficiency_1) * Q_in_1);			// temporary attempt
				Qpp_1		   = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1 + output_1;
				circulating_1  = (Qpp_1 -heat_sink_1)/ (4.18 * dTemp_1);
				output_2 	   = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		   = Temp_In_1;
				pot_WTemp_1    = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
			}



			if (((Q - bypass_Q) >= wdl_1) && (Temp_In_1 >= riverThresh_temp)) {
//				if (itemID == cell) printf("!!!!! More than enough Q, Cooling Temperature is too warm\n");
				dTemp_1		  = optDeltaT_1;
				evaporation_1 = (LH_fract / LH_vap) * (Qpp_1 - heat_sink_1);
				blowdown_1	  = (evaporation_1 / (concentration - 1.0));
				wdl_1		  = (blowdown_1 / (1.0 / concentration));

				if ((Q - bypass_Q) < wdl_1) {
//					if (itemID == cell) printf("!! AND Discharge minus bypass LESS than wdl\n");
					wdl_1 		  = Q- bypass_Q;
					dTemp_1		  = optDeltaT_1;
					blowdown_1	  = (1.0 / concentration) * wdl_1;
					evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
					Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1 + output_1;
					circulating_1 = (Qpp_1-heat_sink_1) / (4.186 * dTemp_1);
					output_2 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
					Tpp_1		  = Temp_In_1;
					pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);
				}

				output_2 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Temp_In_1;
				pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

			}

			if ((Q - bypass_Q) < wdl_1) {
//				if (itemID == cell) printf("!!!!! Discharge minus bypass LESS than wdl\n");
				wdl_1 		  = Q - bypass_Q;
				dTemp_1		  = optDeltaT_1;
				blowdown_1	  = (1.0 / concentration) * wdl_1;
				evaporation_1 = blowdown_1 * (concentration - 1.0);		// m3s
				Qpp_1		  = (evaporation_1 / (LH_fract / LH_vap)) + heat_sink_1 + output_1;
				circulating_1 = (Qpp_1-heat_sink_1) / (4.186 * dTemp_1);
				output_2 	  = (-Qpp_1) / (1.0 - (1.0 / Efficiency_1));
				Tpp_1		  = Temp_In_1;
				pot_WTemp_1   = (((Q - wdl_1) * Q_WTemp) + (blowdown_1 * Temp_In_1)) / (Q - evaporation_1);

			}

			OT_effluent_1 = 0.0;
			LH_fract_post = LH_fract * (evaporation_1 / Qpp_1);			// temporary attempt
			output_1      = output_1 +output_2;

			} // ends new if statement, added 121912
		}		// this ends HYBRID technology







//END COOLING TECHS//


	if (b == 1.0) {
			pot_WTemp_1   = Q_WTemp;
			output_1	  = NamePlate_1;
//			wdl_1		  = wdl_1;
//			evaporation_1 = evaporation_1;
//			blowdown_1	  = blowdown_1;
//			OT_effluent_1 = OT_effluent_1;
			dTemp_1		  = optDeltaT_1;

	}





	if ((Technology_1 == 1.0) || (Technology_1 > 4.0 && Technology_1 < 4.15)) {					// RJS 120912
		if (op_consumption < ((Q - wdl_1) / 10.0)) evaporation_2 = op_consumption;	// RJS 120912
		else evaporation_2 = 0.0;																// RJS 120912
	}
	else op_consumption = evaporation_2 = 0.0;																	// RJS 120912 (m3/s due to once-thru consumption)

	output_1 = output_1 > energy_Pen ? output_1 - energy_Pen : 0.0;		//RJS 123112

	if (Efficiency_1 < 0.0) a = 1.0;

	condenser_inlet			  = ((Technology_1 == 1.0) || (Technology_1 == 4.1)) ? Q_WTemp : Temp_In_1;							// added 122112
	totalDaily_output_1 	  = (output_1 * 24)	> energyDemand_1 ? energyDemand_1 : (output_1 * 24);		// total MWhrs per day
	totalDaily_deficit_1	  = energyDemand_1 - totalDaily_output_1;		// energy deficit (MWhrs)
	totalDaily_percent_1	  = energyDemand_1 > 0.0 ? totalDaily_output_1 / energyDemand_1 : 0.0; 		// percent of demand fulfilled
//	totalHours_run_1		  = totalDaily_output_1 /output_1;			// hrs of operation run
	totalHours_run_1		  = output_1 == 0.0 ? 0.0 : totalDaily_output_1 / output_1;			// hrs of operation run
	totalDaily_wdl_1		  = wdl_1 * totalHours_run_1 * 3600;			// total wdl in m3 per day
	totalDaily_target_wdl_1	  = wdl_1 * (energyDemand_1 / NamePlate_1) * 3600;	// total target withdrawal to meet demand based on standard wdl per MW
	avgDaily_eff_dTemp_1	  = dTemp_1 / optDeltaT_1;							// average daily effluent temperature rise divided by optDeltaT_1 (positive is larger than optDeltaT)
	avgDaily_efficiency_1	  = Efficiency_1 / opt_efficiency_1;				// percentage of optimal efficiency fulfilled
	totalDaily_evap_1		  = evaporation_1 * totalHours_run_1 * 3600;	// total evaporation in m3 per day
	totalDaily_evap_2		  = evaporation_2 * totalHours_run_1 * 3600;	// total consumption by once-thru from river (m3/d)	RJS 120912
	totalDaily_cons_2		  = op_consumption * totalHours_run_1 * 3600;	// total consumption needed (either from river or municipal) (m3/d)	RJS 120912
	totalDaily_external_2	  = totalDaily_cons_2 - totalDaily_evap_2;		// total external water use needed from municipal (m3/d)
	totalDaily_blowdown_1	  = blowdown_1 * totalHours_run_1 * 3600;		// total blowdown (or discharge from power plant to river) in m3 per day
	totalDaily_OTeffluent_1   = OT_effluent_1 * totalHours_run_1 * 3600;		// total return flow from once through cooling to river (effluent)
	totalDaily_returnflow_1	  = totalDaily_blowdown_1 + totalDaily_OTeffluent_1;	// total (effluent + blowdown) from both "once through" and "recirc" back to river (m3)
	totalDaily_demand_1		  = energyDemand_1;
	totalDaily_heatToElec     = output_1;		// MJ per sec (MW)

	totalDaily_heatToRiv	  = 0.0;				// MJ per sec (MW)
	totalDaily_heatToSink	  = heat_sink_1;		// MJ per sec (MW)
	totalDaily_heatToEng	  = (wdl_1 * dTemp_1 * 4.18) * 3600 * totalHours_run_1; // MJ during operating hours to Eng

//	Q_outgoing_1			  = (b == 1.0) ? Q : ((Q * 86400) - totalDaily_evap_1) / 86400;	// commented out 120912
	Q_outgoing_1			  = (b == 1.0) ? Q : ((Q * 86400) - totalDaily_evap_1 - totalDaily_evap_2) / 86400;	//RJS 120912


	if (Technology_1 == 1.0 || (Technology_1 > 4.0 && Technology_1 < 4.15))  {
	totalDaily_heatToRiv = totalDaily_heatToEng;
	totalDaily_heatToEng = 0.0;
	}

	if (Technology_1 == 2.0 || (Technology_1 > 4.16 && Technology_1 < 4.25) || Technology_1 == 5.0)  {
		if (Temp_In_1 < 0.0) Temp_In_1 = 2.0;		// 120612
	totalDaily_heatToRiv = ((totalDaily_returnflow_1 / (3600 * totalHours_run_1)) * (Temp_In_1 - Q_WTemp) * 4.18) * 3600 * totalHours_run_1;
	totalDaily_heatToEng = (circulating_1 * dTemp_1 * 4.18) * 3600 * totalHours_run_1; 		// MJ during operating hours to Eng
		if (totalHours_run_1 == 0.0) totalDaily_heatToRiv = 0.0;							// late night add (to prevent nans)
	}


	flux_GJ_old_prist         = Q_WTemp * ((Q) * 3600 * totalHours_run_1) ;

	flux_GJ_old_prist2		  = (Q_WTemp * Q * 3600 * (24 - totalHours_run_1)) + (flux_GJ_old_prist);

	if ((Technology_1 == 2.0) || (Technology_1 > 4.16 && Technology_1 < 4.25) || (Technology_1 == 5.0)) Tpp_1 = Temp_In_1;		// added "or Technology_1 == 5

//	flux_GJ_new	  			  = ((Q_WTemp * (Q - wdl_1 + evaporation_1)) + (Tpp_1 * (totalDaily_returnflow_1 / (3600 * totalHours_run_1)))) * 3600 * totalHours_run_1;		// commented out RJS 120112
//	flux_GJ_new	  			  = ((Q_WTemp * (Q - wdl_1)) + (Tpp_1 * (totalDaily_returnflow_1 / (3600 * totalHours_run_1)))) * 3600 * totalHours_run_1;						// added RJS 120112, commented out 120912
	flux_GJ_new	  			  = ((Q_WTemp * (Q - wdl_1 - evaporation_2)) + (Tpp_1 * (totalDaily_returnflow_1 / (3600 * totalHours_run_1)))) * 3600 * totalHours_run_1;						// added RJS 120912


	if (totalHours_run_1 == 0.0) flux_GJ_new = 0.0;	//RJS late new (gets rid of nans)

//	evaporated_heat		  = Q_WTemp * (evaporation_1 * 3600 * totalHours_run_1);			// RJS added 120112
	evaporated_heat		  = (Q_WTemp * (evaporation_1 * 3600 * totalHours_run_1)) + (Q_WTemp * (evaporation_2 * 3600 * totalHours_run_1));			// RJS added 120912


	non_operatingHrs_heat = (24.0 - totalHours_run_1) * Q_WTemp * Q * 3600;
	operatingHrs_heat     = flux_GJ_new;
	flux_QxT_new		  = non_operatingHrs_heat + operatingHrs_heat;
//	Q_outgoing_WTemp_1	  = flux_QxT_new / (((Q - evaporation_1) * totalHours_run_1 * 3600) + (Q * (24.0 - totalHours_run_1) * 3600));
//	Q_outgoing_WTemp_1	  = ((pot_WTemp_1 * totalHours_run_1) + (Q_WTemp * (24.0 - totalHours_run_1))) / 24.0;			//comment out RJS 120112
	Q_outgoing_WTemp_1	  = (b == 1.0) ? Q_WTemp : flux_QxT_new / (Q_outgoing_1 * 86400);				// added RJS 120112


	if (b == 1.0) {

		flux_QxT_new		   = flux_QxT;
		Q_outgoing_WTemp_1	   = Q_WTemp;
		totalDaily_heatToOcean = totalDaily_heatToRiv;		//RJS 010412
		totalDaily_heatToRiv   = 0.0;
		totalDaily_heatToEng   = 0.0;
		totalDaily_heatToElec  = 0.0;
		totalDaily_heatToSink  = 0.0;
		evaporated_heat		   = 0.0;				// RJS 120912, does this change heat balance?
		Q_outgoing_1		   = Q;

	}


	if (a == 1.0 ) {						// CWA is on, BUT Q_WTemp is TOO high - so NO WITHDRAWALS / NO OPERATION
			pot_WTemp_1   = Q_WTemp;
			output_1	  = 0.0;
			wdl_1		  = 0.0;
			dTemp_1		  = 0.0;
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
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			totalDaily_heatToElec	  = 0.0;
			Q_outgoing_1			  = Q;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
			flux_GJ_old_prist         = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;
			flux_GJ_old_prist2        = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;
			flux_GJ_new				  = flux_GJ_old_prist;
			non_operatingHrs_heat     = flux_GJ_old_prist;
			operatingHrs_heat     	  = 0.0;
			evaporated_heat			  = 0.0;
			totalDaily_evap_2		  = 0.0;
			totalDaily_external_2     = 0.0;
			totalDaily_cons_2		  = 0.0;
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
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			Q_outgoing_1			  = Q;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
			flux_GJ_old_prist         = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;
			flux_GJ_new				  = flux_GJ_old_prist;
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			totalDaily_heatToElec	  = 0.0;
			evaporated_heat			  = 0.0;
	}

	}		// this ends Q> 0.000001

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
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			Q_outgoing_1			  = Q;
			Q_outgoing_WTemp_1		  = Q_WTemp;
			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
//			flux_QxT_new   	   		  = Q_outgoing_WTemp_1 * Q_outgoing_1 * 86400.0;		// late-night discharge test
			totalDaily_heatToRiv	  = 0.0;
			totalDaily_heatToEng	  = 0.0;
			totalDaily_heatToSink	  = 0.0;
			totalDaily_heatToElec	  = 0.0;
			evaporated_heat			  = 0.0;
	}


	totalGJ_heatToEng	 = totalDaily_heatToEng / 1000;								// from MJ (no unit of time) to GJ
	totalGJ_heatToSink	 = totalDaily_heatToSink * 3.6 * totalHours_run_1;			// from MJ/s to GJ
	totalGJ_heatToRiv	 = totalDaily_heatToRiv / 1000;								// from MJ (no unit of time) to GJ
	totalGJ_heatToElec	 = totalDaily_heatToElec * 3.6 * totalHours_run_1;			// from MJ/s to GJ
//	totalGJ_heatToEvap   = (Q_WTemp + 273.15) * (evaporation_1 * 3600 * totalHours_run_1) * 4.18 * 0.001;						// commented out 120912
	totalGJ_heatToEvap   = (b == 1.0) ? 0.0 : (Q_WTemp + 273.15) * ((evaporation_1 + evaporation_2) * 3600 * totalHours_run_1) * 4.18 * 0.001;		//RJS 120912, added the check for lake/ocean _: does this effect mass balance?

	totalGJ_fluxOLD		 = (Q_WTemp + 273.15) * Q * 86400 * 4.18 * 0.001;
	totalGJ_fluxNEW 	 = (Q_outgoing_WTemp_1 + 273.15) * Q_outgoing_1 * 86400 * 4.18 * 0.001;

	waterT_heatToRiv	 = (totalGJ_fluxNEW + totalGJ_heatToEvap) - totalGJ_fluxOLD;
	percent_wTheatToRiv	 = (waterT_heatToRiv - totalGJ_heatToRiv) / totalGJ_heatToRiv * 100;

	totalGJ_heatToRiv    = waterT_heatToRiv;



//	if (((NamePlate_1 > 356.1) && (NamePlate_1 < 356.3)) || ((NamePlate_1 > 659.6) && (NamePlate_1 < 659.8)) || ((NamePlate_1 > 321.9) && (NamePlate_1 < 322.1))) {
//		printf("y = %d, m = %d, d = %d, nameplate = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1);
//		printf("output_1 = %f, demand_1 = %f, Efficiency_1 = %f, Q_WTemp = %f, Q_Outgoing_WTemp_1 = %f, Q_incoming = %f, withdrawal_1 = %f\n", output_1, energyDemand_1, Efficiency_1, Q_WTemp, Q_outgoing_WTemp_1, Q, wdl_1);
//		printf("opt_efficiency_1 = %f, cond_h20_1 = %f\n", opt_efficiency_1, cond_h2o_1);
//		printf("Efficiency_loss = %f\n", Eff_loss_1);
//	}

	if (totalDaily_output_1 < 0.0) {
		printf("!!!!!! NEG OUTPUT: y = %d, m = %d, d = %d, TotalDaily_output_1 = %f, namplate = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), totalDaily_output_1, NamePlate_1);
	}

	if (wdl_1 > Q && Q > 0.0) {
		printf("!!!!!! WDL > Q: y = %d, m = %d, d = %d, wdl_1 = %f, Q = %f, namplate = %f\n", MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), wdl_1, Q, NamePlate_1);
	}

//	if ((Q_outgoing_1 < 0.000001) && (Q > 0.000001)) {
//	if ((itemID == 880) || (itemID == 389) || (itemID == 334) || (itemID == 233)) {
//	if ((itemID == 38) || (itemID == 37)) {
//	if ((itemID == cell) || (itemID == 482) || (itemID == 881)) {

//	if ((totalGJ_heatToEng < 0.0) || (totalGJ_heatToEvap < 0.0) || (energyDemand_1 < 0.0) || (totalHours_run_1 < 0.0) || (Q_outgoing_WTemp_1 < 0.0)) {
//	if ((NamePlate_1 > 59.9) && (NamePlate_1 < 60.1)) {
/*	if (itemID == 33 || itemID == 32) {
	printf("***** NP = %f, totalDaily_demand = %f\n", NamePlate_1, totalDaily_demand_1);
    printf("---- PrePP: itemID = %d, y = %d, m = %d, d = %d, Q_WTemp = %f, Q_incoming = %f, flux_QxT = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), Q_WTemp, Q, flux_QxT);
    printf("----PostPP: Q_outgoing_WTemp_1 = %f, Q_outgoing_1 = %f, flux_QxT_new = %f, totalDaily_evap_1(m3s) = %f\n", Q_outgoing_WTemp_1, Q_outgoing_1, flux_QxT_new, totalDaily_evap_1 / 86400);
	printf("---- pot_WTemp_1 = %f, diff = %f\n", pot_WTemp_1, Q_outgoing_WTemp_1 - pot_WTemp_1);
	printf("---- Tech = %f, FT = %f,hrs = %f,dTemp_1 = %f, Tpp_1 = %f, Qpp_1 = %f, wdl_1 = %f, evaporation_1 = %f, blowdown_1 = %f\n", Technology_1, FuelType_1, totalHours_run_1, dTemp_1, Tpp_1, Qpp_1, wdl_1, evaporation_1, blowdown_1);
	printf("---- Q = %f, bypass_Q = %f, wdl_1 = %f\n", Q, bypass_Q, wdl_1);
	printf("---- toRiv = %f, toEng = %f, toElec = %f, toSink = %f\n", totalGJ_heatToRiv, totalGJ_heatToEng, totalGJ_heatToElec, totalGJ_heatToSink);
	printf("---- GJ_fluxOLD = %f, GJ_fluxNEW = %f, GJ_evap = %f\n", totalGJ_fluxOLD, totalGJ_fluxNEW, totalGJ_heatToEvap);
    printf("---- waterT_heatToRiv = %f, percent_wTheatToRiv = %f\n", waterT_heatToRiv, percent_wTheatToRiv);
	printf("%f\n", waterT_heatToRiv - totalGJ_heatToRiv);
	printf("----demand = %f, totalDaily_output = %f\n", energyDemand_1, totalDaily_output_1);
	printf("dTemp_1 = %f, blowdown_1 = %f, evaporation_1 = %f, Qpp_1 = %f\n", dTemp_1, blowdown_1, evaporation_1, Qpp_1);
	printf("circulating_1 = %f, Tpp_1 = %f, Eff_loss_1 = %f, Efficiency_1 = %f\n", circulating_1, Tpp_1, Eff_loss_1, Efficiency_1);
	printf("Temp_In_1 = %f, riverThresh_temp = %f, dTemp_1 = %f, calc_T_1 = %f\n", Temp_In_1, riverThresh_temp, dTemp_1, calc_T_1);
	printf("LH_fract_1 = %f, opt_QO_1 = %f\n", LH_fract, opt_QO_1);
	printf("output_1 = %f, pot_WTemp_1 = %f\n", output_1, pot_WTemp_1);
	}
*/



//	heatBalance3 = fabs((flux_QxT_new) - (Q_WTemp * Q  * 86400)) - fabs(totalDaily_heatToRiv / 4.18);	//commented out RJS 120112
//	heatBalance3 = (fabs(flux_QxT_new) + fabs(evaporated_heat)) - fabs(Q_WTemp * Q  * 86400) - fabs(totalDaily_heatToRiv / 4.18);
	heatBalance3 = ((Q_WTemp * Q  * 86400) + (totalDaily_heatToRiv / 4.18)) - flux_QxT_new - evaporated_heat;
	pHB3 = fabs(heatBalance3) / (fabs(flux_QxT_new) + fabs(evaporated_heat));
//	heatBalance2 = fabs(flux_QxT_new - flux_GJ_old_prist2) - fabs(totalDaily_heatToRiv / 4.18);
//	pHB2 = fabs(heatBalance2) / fabs(flux_GJ_new);

	if (b == 1) heatBalance3 = fabs(flux_QxT_new) - fabs(flux_QxT);
	if (b == 1) pHB3 = fabs(heatBalance3) / fabs(flux_QxT_new);
//	if (b == 1) heatBalance2 = fabs(flux_QxT_new) - fabs(flux_QxT);
//	if (b == 1) pHB2 = fabs(heatBalance2) / fabs(flux_QxT_new);

//	if (itemID == cell) {
//		if ((pHB3 > 0.001) && (NamePlate_1 > 0.0)) {

//		printf("!!!!!!! Heat Balance 3 !!!!!!! itemID = %d, y = %d m = %d d = %d\n,",itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay());
//		printf("hB3 = %f, heatBalance3 = %f, flux_QxT_new = %f, evaporated_heat = %f, flux_QxT_old = %f, toRiv = %f\n", pHB3, heatBalance3, flux_QxT_new, evaporated_heat, Q_WTemp * Q * 86400, totalDaily_heatToRiv);
//		printf("evaporation_1 = %f, Q = %f, Q_in_1_Joule = %f, flux_GJ_old_prist = %f, totalHours_run_1 = %f\n", evaporation_1, Q, (Q_in_1 * totalHours_run_1 * 3.6), flux_GJ_old_prist, totalHours_run_1);
//		printf("Tech = %f, fuel = %f, a = %f, b = %f, Q_WTemp = %f, Temp_In_1 = %f, totalDaily_returnflow_1 = %f\n", Technology_1, FuelType_1, a, b, Q_WTemp, Temp_In_1, totalDaily_returnflow_1);
//		printf("blowdown_1 = %f, wdl_1 = %f, Tpp_1  = %f, dTemp_1 = %f, Qpp_1 = %f, heatsink_1 = %f, output_1 = %f\n", blowdown_1, wdl_1, Tpp_1, dTemp_1, Qpp_1, heat_sink_1, output_1);
//		printf("operatingHrs_heat = %f, non_operatingHrs_heat = %f, flux_QxT_new = %f, flux_GJ_old_prist2 = %f\n", operatingHrs_heat, non_operatingHrs_heat, flux_QxT_new, flux_GJ_old_prist2);
//		printf("hb3 = %f, percent_hb3 = %f\n", heatBalance3, pHB3);
//		printf("(2222) Heat = %f, Elec = %f, toSink = %f, toEng = %f, toRiv = %f\n", totalDaily_heat, totalDaily_elec, totalDaily_heatToSink, totalDaily_heatToEng, totalDaily_heatToRiv);
//		printf("(2222) itemID = %d, %d %d %d, NP = %f, hB2 = %f, percentError = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), NamePlate_1, heatBalance2, pHB2);
//		printf("(2222) flux_new = %f, flux_old_evap = %f, toRiv = %f\n", flux_GJ_new, flux_GJ_old_wevap, totalDaily_heatToRiv);
//		}

//		if (itemID == 83) printf("**Thermal3** itemID = %d, day = %d, Q = %f, Q = %f, Q_outgoing_1 = %f\n", itemID, MFDateGetCurrentDay(), Q, Q, Q_outgoing_1);

		if (Q_outgoing_WTemp_1 < 0.0) printf("!!!!! negative Temp, itemID = %d, y = %d, m = %d, d = %d\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay());

		//	}

	 MFVarSetFloat(_MDInDischargeID,         itemID, Q_outgoing_1);			//late-night discharge test
//	 MFVarSetFloat(_MDInDischargeIncomingID,  itemID, Q_outgoing_1);			//		113012 added
	 MFVarSetFloat(_MDWTemp_QxTID,            itemID, Q_outgoing_WTemp_1);
	 MFVarSetFloat(_MDFlux_QxTID,             itemID, flux_QxT_new);
//	 MFVarSetFloat(_MDFluxMixing_QxTID,       itemID, fluxmixing_QxT_new);
	 MFVarSetFloat(_MDOutTotalThermalWdlsID,  itemID, totalDaily_wdl_1);
	 MFVarSetFloat(_MDOutTotalEvaporationID,  itemID, totalDaily_evap_1 + totalDaily_evap_2);	//RJS 120912 added totalDaily_evap_2
	 MFVarSetFloat(_MDOutTotalExternalWaterID, itemID, totalDaily_external_2);		// RJS 120912, total external water use.  Sum of this and Evaporation is the TOTAL CONSUMPTION
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
	 MFVarSetFloat(_MDOutTotalHeatToRivID,    itemID, totalGJ_heatToRiv);
	 MFVarSetFloat(_MDOutTotalHeatToSinkID,   itemID, totalGJ_heatToSink);
	 MFVarSetFloat(_MDOutTotalHeatToEngID,    itemID, totalGJ_heatToEng);
	 MFVarSetFloat(_MDOutTotalHeatToElecID,   itemID, totalGJ_heatToElec);
	 MFVarSetFloat(_MDOutTotalHeatToEvapID,   itemID, totalGJ_heatToEvap);
	 MFVarSetFloat(_MDOutCondenserInletID,    itemID, condenser_inlet);			// added 122112
	 MFVarSetFloat(_MDOutSimEfficiencyID,     itemID, avgDaily_efficiency_1);	// added 122112
	 MFVarSetFloat(_MDOutTotalHoursRunID,     itemID, totalHours_run_1);		// added 030212

}

// end of code



enum { MDnone, MDinput };

int MDThermalInputs3Def () {
	int optID = MFUnset;
	const char *optStr, *optName = MDOptThermalInputs3;
	const char *options [] = { MDNoneStr, MDInputStr, (char *) NULL };

	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);

	MFDefEntering ("Thermal Inputs 3");
	switch (optID) {
		case MDinput:
			if (((_MDPlaceHolderID          = MDWTempRiverRouteDef ()) == CMfailed) ||
				((_MDInDischargeID          = MDDischargeDef ()) == CMfailed) ||
//				((_MDOutDischargeID         = MFVarGetID (MDVarDischarge,        "m3/s", MFOutput, MFState, MFInitial)) == CMfailed) ||		//RJS 113012
//				((_MDInEnergyDemand1ID       = MDEnergyDemandDef ()) == CMfailed) ||
				((_MDInEnergyDemand1ID      = MFVarGetID (MDVarEnergyDemand,   "MW", MFInput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDInDischargeIncomingID  = MFVarGetID (MDVarDischarge0,          "m3/s", MFInput,  MFState,  MFInitial)) == CMfailed) ||		// changed from flux to state, and boundary to initial 113012,
				((_MDFluxMixing_QxTID       = MFVarGetID (MDVarFluxMixing_QxT, "m3*degC/d", MFInput,  MFFlux,  MFBoundary)) == CMfailed)   ||
				((_MDFlux_QxTID             = MFVarGetID (MDVarFlux_QxT,       "m3*degC/d", MFInput,  MFFlux,  MFBoundary)) == CMfailed)   ||
		//		((_MDWTempMixing_QxTID      = MFVarGetID (MDVarWTempMixing_QxT,   "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDWTemp_QxTID            = MFVarGetID (MDVarWTemp_QxT,         "degC",  MFOutput, MFState, MFBoundary)) == CMfailed) ||	//RJS 013112
				((_MDInAirTemperatureID     = MFVarGetID (MDVarAirTemperature,     "degC",  MFInput,  MFState, MFBoundary)) == CMfailed) ||
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
				((_MDInTempLimit_DCID		= MFVarGetID (MDVarTempLimitDC,         "degC", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInNamePlate1ID         = MFVarGetID (MDVarNamePlate1,          "MW", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInFuelType1ID          = MFVarGetID (MDVarFuelType1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInTechnology1ID        = MFVarGetID (MDVarTechnology1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInEfficiency1ID        = MFVarGetID (MDVarEfficiency1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
				((_MDInLakeOcean1ID         = MFVarGetID (MDVarLakeOcean1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||
//				((_MDInPlantCode1ID         = MFVarGetID (MDVarPlantCode1,          "-", MFInput, MFState, MFBoundary)) == CMfailed) ||

//				((_MDOutDischargeID    	      = MFVarGetID (MDVarDischarge,  "m3/s", MFRoute, MFState, MFBoundary)) == CMfailed) ||		// late night discharge test 113012 -
				((_MDOutAvgEfficiencyID       = MFVarGetID (MDVarAvgEfficiency,  "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||
				((_MDOutAvgDeltaTempID        = MFVarGetID (MDVarAvgDeltaTemp,  "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||
			    ((_MDOutTotalEvaporationID    = MFVarGetID (MDVarTotalEvaporation,  "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalExternalWaterID  = MFVarGetID (MDVarTotalExternalWater, "m3", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
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
				((_MDOutTotalHeatToRivID	  = MFVarGetID (MDVarHeatToRiv,           "GJ", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalHeatToSinkID	  = MFVarGetID (MDVarHeatToSink,          "GJ", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalHeatToEngID      = MFVarGetID (MDVarHeatToEng,           "GJ", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalHeatToElecID     = MFVarGetID (MDVarHeatToElec,           "GJ", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutTotalHeatToEvapID     = MFVarGetID (MDVarHeatToEvap,           "GJ", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
				((_MDOutCondenserInletID      = MFVarGetID (MDVarCondenserInlet,       "degC", MFOutput, MFState, MFBoundary)) == CMfailed) ||		// added 122112
				((_MDOutSimEfficiencyID       = MFVarGetID (MDVarSimEfficiency,        "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||			// added 122112
				((_MDOutTotalHoursRunID       = MFVarGetID (MDVarTotalHoursRun,        "-", MFOutput, MFState, MFBoundary)) == CMfailed) ||			// added 030213
				(MFModelAddFunction (_MDThermalInputs3) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving ("Thermal Inputs 3");
	return (_MDWTemp_QxTID);
}









