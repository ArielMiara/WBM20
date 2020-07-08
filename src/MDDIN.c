
/******************************************************************************

GHAAS Water Balance/Transport Model V3.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDDIN.c  - transfer and processing of nutrients (transient storage and lakes)

rob.stewart@unh.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>
#include <math.h>

// input
static int _MDInDischarge0ID            	= MFUnset;		// RJS010609
static int _MDInDischargeID			= MFUnset;
static int _MDInRiverStorageID		    	= MFUnset;
static int _MDInRiverStorageChgID		= MFUnset;		// RJS 010609
static int _MDInRiverDepthID			= MFUnset;
static int _MDInRiverWidthID			= MFUnset;
static int _MDInLocalLoad_DINID			= MFUnset;		// RJS 091108
static int _MDFlux_DINID			= MFUnset;		// RJS 091108		changed from MDInFlux_DINID 091408
static int _MDStoreWater_DINID		    	= MFUnset;		// RJS 091108
static int _MDFluxMixing_DINID	 	    	= MFUnset;		// RJS 091108			changed from MDInFluxMixing_DINID 091408
static int _MDStoreWaterMixing_DINID		= MFUnset;		// RJS 091108
static int _MDInRunoffVolID			= MFUnset;		// RJS 091108
static int _MDInTnQ10ID				= MFUnset;      // RJS 102410
static int _MDInRiverOrderID			= MFUnset;		// RJS 112211
static int _MDWTemp_QxTID				=MFUnset;
static int _MDInPlaceHolderID			= MFUnset;		//RJS 042913
static int _MDInVfAdjustID				= MFUnset;		//RJS 050213

// output

static int _MDOutTotalMassRemovedTS_DINID   = MFUnset;
static int _MDOutPostConc_DINID		    = MFUnset;
static int _MDOutTotalMassRemovedDZ_DINID   = MFUnset;
static int _MDOutTotalMassRemovedHZ_DINID   = MFUnset;
static int _MDOutTotalMassRemovedMC_DINID   = MFUnset;
static int _MDOutTimeOfStorageMCID	    = MFUnset;	// RJS 120608
static int _MDOutTimeOfStorageDZID	    = MFUnset;
static int _MDOutTimeOfStorageHZID	    = MFUnset;
static int _MDOutTransferDZID		    = MFUnset;
static int _MDOutTransferHZID		    = MFUnset;
static int _MDOutWaterDZID		    = MFUnset;
static int _MDOutWaterHZID		    = MFUnset;
static int _MDOutUptakeVfID		    = MFUnset;
static int _MDOutTotalMassPre_DINID	    = MFUnset;	// RJS 091408
static int _MDOutConcMixing_DINID	    = MFUnset;	// RJS 091408
static int _MDOutMassBalance_DINID          = MFUnset;	// RJS 110308
static int _MDOutMassBalanceMixing_DINID    = MFUnset;	// RJS 112008
static int _MDOutRemovalDZID		    = MFUnset;	// RJS 110708
static int _MDOutRemovalHZID		    = MFUnset;	// RJS 110708
static int _MDOutRemovalMCID		    = MFUnset;	// RJS 110708
static int _MDOutRemovalTotalID		    = MFUnset;	// RJS 03-01-09
static int _MDOutAaID			    = MFUnset;	// RJS 110708
static int _MDOutAsDZID			    = MFUnset;	// RJS 110708
static int _MDOutAsHZID			    = MFUnset;	// RJS 110708
static int _MDDeltaStoreWater_DINID	    = MFUnset;  // RJS 112008
static int _MDDeltaStoreWaterMixing_DINID   = MFUnset;  // RJS 112008
static int _MDFlowPathRemoval_DINID	    = MFUnset;	// RJS 112008
static int _MDFlowPathRemovalMixing_DINID   = MFUnset;	// RJS 112008
static int _MDOutVelocityID		    = MFUnset;	// RJS 112108
static int _MDOutPreConc_DINID              = MFUnset;	// RJS 120408
static int _MDOutTotalMassRemoved_DINID	    = MFUnset;	// RJS 032509

static int _MDOutDINVfID		     = MFUnset;	// RJS 112210
static int _MDOutDINKtID		     = MFUnset; // RJS 112210
static int _MDOutPreFlux_DINID		     = MFUnset; // RJS 050911

static float _MDUptakeKtMC   = 0.60;		// RJS 033009
static float _MDUptakeKtSTS  = 0.60;		// RJS 033009
static float _MDUptakeKtHTS  = 0.60;		// RJS 033009

static float _MDAlphaSTS     = 0.0;		// RJS 032509
static float _MDAlphaHTS     = 0.0;		// RJS 032509
static float _MDASTSA	     = 0.0;		// RJS 032509
static float _MDAHTSA	     = 0.0;		// RJS 032509


static void _MDNProcessing (int itemID) {

float localLoad_DIN          	= 0.0;		// mass of N from local non-point sources (kg/day) RJS 091108
float preFlux_DIN		= 0.0;		// mass of N coming from upstream (kg/day) RJS 091108
float preFluxMixing_DIN		= 0.0;		// mass of N coming from upstream MIXING ONLY (kg/day) RJS 091108
float storeWaterMixing_DIN	= 0.0;		// store water DIN MIXING ONLY (kg/day)
float preConcMixingDIN		= 0.0;		// concentration MIXING ONLY (mg/L)
float runoffVol			= 0.0;		// m3/sec
float waterTotalVolume		= 0.0;   	// water total volume (m3)
float postFluxDIN		= 0.0;		// post flux of DIN (kg/day)
float postFluxDINMixing		= 0.0;		// post flux of DIN mixing (kg/day)
float storeWater_DIN		= 0.0;	  	// store water DIN (kg) ?
float postStoreWater_DIN	= 0.0;  	// store water DIN new time step (kg)		// RJS 110308
float postStoreWaterMixing_DIN	= 0.0;		// store water DIN new time step (kg)
float DINDeltaStorage		= 0.0;	  	// change in water DIN (kg)				// RJS 110308
float DINDeltaStorageMixing	= 0.0; 		// change in mixed water DIN (kg)		// RJS 110508
float DINTotalIn		= 0.0;		// mass DIN in (kg)				// RJS 110308
float DINTotalInMixing		= 0.0;		// mass DIN in Mixing (kg)		// RJS 112008
float waterStorage		= 0.0;		// water storage (m3/day)
float waterStorageChg		= 0.0;		// water storage change (m3/day) 					// RJS 01-06-09
float waterStoragePrev		= 0.0;		// water storage previous timestep (m3/day)			// RJS 01-06-09
float totalMassRemovedTS_DIN 	= 0.0;	  	// mass DIN removed in Transient Storage (kg/day)
float preConcDIN		= 0.0;		// pre concentration of DIN kg/m3/day
float postConcDIN		= 0.0;		// post concentration of DIN kg/m3/day
float postConcDINMixing		= 0.0;		// post concentration of DIN mixing (kg/m3/day)
float dischargePre		= 0.0;		// pre routing discharge that includes local cell runoff (m3/s)	// 010609
float discharge			= 0.0;		// instantaneous discharge (m3/s)
float dL			= 0.0;		// cell length (m)
float transferDZ		= 0.0;		// probability of water transfer from MC to DZ
float transferHZ		= 0.0;		// probability of water transfer from MC to HZ
float uptakeDZ			= 0.0;		// probability of N uptake in DZ
float uptakeHZ			= 0.0;		// probability of N uptake in HZ
float removalTotal		= 0.0;		// Total removal (HZ + DZ) in grid cell reach
float alphaDZ			= 0.0;		// exchange rate of water between MC and DZ
float alphaHZ			= 0.0;		// exchange rate of water between MC and HZ
float depth			= 0.0;		// river depth (m)
float width			= 0.0;		// river width (m)
float aA			= 0.0;		// main channel area (m2)
float asDZ			= 0.0;		// DZ storage area (m2)
float asHZ			= 0.0;		// HZ storage area (m2)
float tStorDZ			= 0.0;		// time of storage or residence time in DZ (days)
float tStorHZ			= 0.0;		// time of storage or residence time in HZ (days)
float tStorMC			= 0.0;		// time of storage or residence time in MC (days)
float vf			= 0.0;		// areal uptake rate (m / sec)
float hydLoad			= 0.0;		// hydraulic load or discharge / benthic surface area (m / day)
float uptakeMC			= 0.0;		// probability of N uptake in MC
float removalDZ			= 0.0;		// probability that a molecule in channel removed in DZ
float removalHZ			= 0.0;		// probability that a molecule in channel removed in HZ
float removalMC			= 0.0;		// probability that a molecule in channel removed in MC
float riverOrder		= 0.0;		// river order
float R				= 0.0;		// proportional removal
float totalMassRemovedDZ_DIN	= 0.0;		// total mass removed in DZ (kg/day)
float totalMassRemovedHZ_DIN 	= 0.0;		// total mass removed in HZ (kg/day)
float totalMassRemovedMC_DIN 	= 0.0;		// total mass removed in MC (kg/day)
float totalMassRemoved_DIN	= 0.0;		// total mass removed (LK, DZ, MC, HZ)	032509
float waterDZ			= 0.0;  	// water entering DZ
float waterHZ			= 0.0;		// water entering HZ
float massBalance_DIN;  			// RJS 110308
float massBalanceMixing_DIN;			// RJS 112008
float flowPathRemoval 		= 0.0;		// kg/day RJS 110508  ---  if order<3, discharge = 0, preflux is removed via flowPathRemoval
float flowPathRemovalMixing     = 0.0;		// kg/day RJS 112008
float velocity			= 0.0;
float runoffConc		= 0.0;		// RJS 120208
float ktMC			= 0.0;		// MC volumetric uptake rate (1 / day)
float ktSTS			= 0.0;		// STS volumetric uptake rate (1 / day)
float ktHTS			= 0.0;		// HTS volumetric uptake rate (1 / day)
float tnQ10   			= 2.0;					//RJS 102410
float denit_int_vf		= -2.975;				//RJS 051011	calculated from avg vf = 0.137 m/day (LINX2), and average [] of 529 ug/L in LINX experiments, log(vf) = -m * log(C in ug/L) + e.
float denit_slope 		= -0.493;				//RJS 110110
float denit_int_kt		= 0.2319;				//MMM March 2013 New value calculated RJS 110110	calculated from kt = 0.64, and average [] of 529 ug/L in LINX experiments, log(kt) = -m * log(C in ug/L) + e
float DIN_Vf			= 0.0;					//RJS 110110
float DIN_Vf_ref		= 0.0;					//RJS 110110
float DIN_Kt			= 0.0;					//RJS 110110
float DIN_Kt_ref		= 0.0;					//RJS 110110
float waterT			= 0.0;					//RJS 051211
float tnTref			= 20.0;
float placeHolder		= 0.0;					//RJS 042913
float VfAdjust			= 1.0;					//RJS 050213

float cell_1      		= 1293;  //255,138
float cell_2			= 999999;

	riverOrder       = MFVarGetFloat (_MDInRiverOrderID,      itemID, 0.0);	//RJS 112211
	localLoad_DIN  	     = MFVarGetFloat (_MDInLocalLoad_DINID,      itemID, 0.0);	 // kg/day RJS 091108
	preFlux_DIN	     = MFVarGetFloat (_MDFlux_DINID,             itemID, 0.0);	 // kg/day RJS 091108
	storeWater_DIN	     = MFVarGetFloat (_MDStoreWater_DINID,       itemID, 0.0);	 // kg/day RJS 091108
	preFluxMixing_DIN    = MFVarGetFloat (_MDFluxMixing_DINID,       itemID, 0.0);	 // kg/day RJS 091108 changed from MDInFluxMixing 091408
	storeWaterMixing_DIN = MFVarGetFloat (_MDStoreWaterMixing_DINID, itemID, 0.0);	 // kg/day RJS 091108
	runoffVol            = MFVarGetFloat (_MDInRunoffVolID,          itemID, 0.0); 	 // m3/sec
	waterStorage	     = MFVarGetFloat (_MDInRiverStorageID,       itemID, 0.0);	 // m3/day	RJS 01-06-09
	waterStorageChg	     = MFVarGetFloat (_MDInRiverStorageChgID,    itemID, 0.0);	 // m3/day  RJS 01-06-09
	waterStoragePrev     = waterStorage - waterStorageChg;				 // m3/day	RJS 01-06-09
	discharge            = MFVarGetFloat (_MDInDischargeID,          itemID, 0.0);	 // m3/sec
	dischargePre	     = MFVarGetFloat (_MDInDischarge0ID,         itemID, 0.0);	 // m3/sec
	dL                   = MFModelGetLength (itemID);			 // km converted to m
//	placeHolder			 = MFVarGetFloat (_MDInPlaceHolderID,        itemID, 0.0);		// run ThermalInputs

	tnQ10                = MFVarGetFloat (_MDInTnQ10ID,              itemID, 0.0);	 // RJS 102410
	waterT			 = MFVarGetFloat (_MDWTemp_QxTID,			 itemID, 0.0);

	DINTotalIn           = localLoad_DIN + preFlux_DIN + storeWater_DIN; 				 // kg/day		RJS 110308
	DINTotalInMixing     = localLoad_DIN + preFluxMixing_DIN + storeWaterMixing_DIN; 		 // kg/day

	ktMC		     = _MDUptakeKtMC;			 	// RJS 033009
	ktSTS		     = _MDUptakeKtSTS;				// RJS 033009
	ktHTS		     = _MDUptakeKtHTS;				// RJS 033009

	runoffConc	     = runoffVol > 0.0 ? (localLoad_DIN * 1000000) / (runoffVol * 86400 * 1000) : 0.0;

	depth		      = MFVarGetFloat (_MDInRiverDepthID, 			itemID, 0.0);	// m			// moved here 031209
	width	          = MFVarGetFloat (_MDInRiverWidthID,    		itemID, 0.0);	// m			// moved here 031209
	aA		      	  = width * depth;	// m2														// moved here 031209
//	waterTotalVolume  = (waterStoragePrev) + (dischargePre * MDConst_m3PerSecTOm3PerDay);	// DOES NOT INCLUDE WDLS m3/day RJS 01-06-09: using water storage from previous time step
	waterTotalVolume  = (waterStoragePrev) + (discharge * MDConst_m3PerSecTOm3PerDay);	// DOES NOT INCLUDE WDLS m3/day RJS 01-06-09: using water storage from previous time step

	VfAdjust		   = MFVarGetFloat (_MDInVfAdjustID,            itemID, 1.0);			// RJS 112211, adjusts loads, keep at 1 if nodata


	//waterT = 0.8 + ((26.2 - 0.8) / (1 + exp(0.18 * (13.3 - airT))));


// processing code


	if (discharge > 0.000001) {			//

		preConcDIN	     = DINTotalIn / (waterTotalVolume) * 1000;     	// mg/L
		preConcMixingDIN     = DINTotalInMixing / (waterTotalVolume) * 1000; 	// mg/L
		velocity	     = discharge / (depth * width); 			// m/s


		if (aA > 0.0) {			// if aA < 0.0, then no TS should be executed.  RJS 03-14-09		KYLE

				asDZ = aA * 0.204;			// single value MEAN Scenario 061609    - ln transformed mean
				asHZ = aA * 0.348;			// single value MEAN Scenario 061609	- ln transformed mean

		//		alphaDZ = 0.00013;			// single value MEAN Scenario 061609 - ln transformed mean
		//		alphaHZ = 0.00000953;			// single value MEAN Scenario 061609 - ln transformed mean

				alphaDZ = 0.0;			// transient storage is off
				alphaHZ = 0.0;			// transient storage is off

				ktMC  = _MDUptakeKtMC;   		// single value MEAN Scenario 061609 - ln transformed mean
		 		ktSTS = _MDUptakeKtSTS;   		// single value MEAN Scenario 061609 - ln transformed mean
				ktHTS = _MDUptakeKtHTS;   		// single value MEAN Scenario 061609 - ln transformed mean

				tStorDZ    = (asDZ / (alphaDZ * aA)) / 86400; // days
				if (alphaDZ == 0.0) tStorDZ = 0.0;
												// RJS 111710
				tStorHZ    = (asHZ / (alphaHZ * aA)) / 86400; // days
				if (alphaHZ == 0.0) tStorHZ = 0.0;												// RJS 111710

				transferDZ = alphaDZ * aA * dL / discharge;		//RJS 072909
				transferHZ = alphaHZ * aA * dL / discharge;		//RJS 072909


				if (preConcDIN > 0.0) {

//					DIN_Vf_ref   = pow(10,(0.4328 + (log10(preConcDIN * 1000) * -0.479)));	// based on Wollheim 2008
//					DIN_Vf	     = DIN_Vf_ref * pow(tnQ10, (((waterT) - tnTref) / 10)); 			//m/d	RJS 051211
//					DIN_Kt_ref   = pow(10,(denit_int_kt + (log10(preConcDIN * 1000) * denit_slope)));	//ConcPre must be in ug/L
//					DIN_Kt	     = DIN_Kt_ref * pow(tnQ10, (((waterT) - tnTref) / 10));			//m/d	RJS 051211

					DIN_Vf_ref   = pow(10,(denit_int_vf + (log10(preConcDIN * 1000) * denit_slope)))*86400/100;	//no conversion to m/d neccesary - already in m/d
					DIN_Vf_ref	 = DIN_Vf_ref * VfAdjust;				// m/d RJS 050213
					DIN_Vf	     = DIN_Vf_ref * pow(tnQ10, (((waterT) - tnTref) / 10)); 			//m/d	RJS 051211
					DIN_Kt_ref   = pow(10,(denit_int_kt + (log10(preConcDIN * 1000) * denit_slope)))*86400;	//ConcPre must be in ug/L
					DIN_Kt	     = DIN_Kt_ref * pow(tnQ10, (((waterT) - tnTref) / 10));			//m/d	RJS 051211
				}

				else {
					DIN_Vf		     = 0.0;		// m/d
					DIN_Kt		     = 0.0;		// 1/d
				}

				waterHZ    = discharge * transferHZ; 		// m3/s
				waterDZ    = discharge * transferDZ; 		// m3/s

				hydLoad    = discharge / (width * dL) * 86400;	// m/day,
				vf	   = _MDUptakeKtMC * 0.131;	        // m/day... 0.131 is average depth (m) in Linx2 experiments		RJS 033009
				tStorMC    = dL / velocity / 86400;             // days,

				uptakeMC  = 1.0 - pow(2.718281828, -1 * DIN_Vf / hydLoad);	// RJS 102410
				uptakeDZ  = 1.0 - pow(2.718281828, -1 * DIN_Kt * tStorDZ);	// RJS 102410
				uptakeHZ  = 1.0 - pow(2.718281828, -1 * DIN_Kt * tStorHZ);	// RJS 102410

				removalDZ = transferDZ * uptakeDZ;  // reach scale proportional nutrient removal
				removalHZ = transferHZ * uptakeHZ;  // reach scale proportional nutrient removal
				removalMC = uptakeMC;				// reach scale proportional nutrient removal  // **** RJS 032309

				removalTotal = removalDZ + removalHZ + removalMC;

				totalMassRemovedDZ_DIN = (removalDZ) * DINTotalIn; 						// kg/day RJS 110308
				totalMassRemovedHZ_DIN = (removalHZ) * DINTotalIn; 						// kg/day RJS 110308
				totalMassRemovedMC_DIN = (removalMC) * DINTotalIn; 						// kg/day RJS 110308

				totalMassRemovedTS_DIN   = totalMassRemovedDZ_DIN + totalMassRemovedHZ_DIN; // kg/dayf

		}
		}
	else {

		flowPathRemoval       = DINTotalIn;
		flowPathRemovalMixing = DINTotalInMixing;

	}


	if (discharge > 0.000001) {		//
	postConcDIN 	  		= (DINTotalIn - totalMassRemovedTS_DIN - totalMassRemovedMC_DIN - flowPathRemoval) / waterTotalVolume * 1000;  // mg/L
	postConcDINMixing 		= (DINTotalInMixing - flowPathRemovalMixing) / waterTotalVolume * 1000;					    // mg/L

	}

	else {

	postConcDIN 	  = 0.0;	// mg/L
	postConcDINMixing = 0.0;	// mg/L

	}

postFluxDIN 	  		 = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcDIN / 1000;  		// kg/day
postFluxDINMixing 		 = (discharge * MDConst_m3PerSecTOm3PerDay) * postConcDINMixing / 1000;		// kg/day

postStoreWater_DIN 		 =  (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcDIN / 1000;					// kg/day
postStoreWaterMixing_DIN =  (waterStorage * MDConst_m3PerSecTOm3PerDay) * postConcDINMixing / 1000;					// kg/day

DINDeltaStorage       		 = postStoreWater_DIN - storeWater_DIN;						// kg/day
DINDeltaStorageMixing 		 = postStoreWaterMixing_DIN - storeWaterMixing_DIN;				// kg/day


if (discharge > 0.000001 && preConcDIN > 0.0) {		//
	R  = 1.0 - (postConcDIN / preConcDIN);		// RJS 032409
	vf = -1.0 * hydLoad * log(1.0 - R);		// RJS 032409

	if (R == 1.0) vf = -9999;
}

else {
	R  = 0.0;
	vf = 0.0;

}

massBalance_DIN 	  = ((localLoad_DIN + preFlux_DIN + storeWater_DIN) - (totalMassRemovedTS_DIN + totalMassRemovedMC_DIN + postFluxDIN + postStoreWater_DIN + flowPathRemoval)) / (localLoad_DIN + storeWater_DIN + preFlux_DIN);		// RJS 111411
massBalanceMixing_DIN	  = ((localLoad_DIN + preFluxMixing_DIN) - (DINDeltaStorageMixing + flowPathRemovalMixing + postFluxDINMixing)) / (localLoad_DIN + storeWaterMixing_DIN + preFluxMixing_DIN);

if (massBalance_DIN > 0.0003 && localLoad_DIN + storeWater_DIN + preFlux_DIN > 0.000001) printf("******\nmassBalance_DIN = %f, itemID = %d, postConcDIN = %f, waterStorage = %f, localLoad_DIN = %f, preFlux_DIN = %f, storeWater_DIN = %f\n totalMassRemovedTS_DIN = %f, totalMassRemovedMC_DIN = %f, postFluxDIN = %f, postStoreWater_DIN = %f, flowPathRemoval = %f", massBalance_DIN, itemID, postConcDIN, waterStorage, localLoad_DIN, preFlux_DIN, storeWater_DIN, totalMassRemovedTS_DIN, totalMassRemovedMC_DIN, postFluxDIN, postStoreWater_DIN, flowPathRemoval);

//if (itemID==809) printf ("***** itemID=%d, month=%d, day=%d, year=%d, Q=%f, hydLoad=%f, waterT=%f, localLoad_DIN = %f \n",itemID,MFDateGetCurrentMonth(),MFDateGetCurrentDay(),MFDateGetCurrentYear(),discharge,hydLoad,waterT,localLoad_DIN);
//if (itemID==809) printf ("preConcDIN=%f, postConcDIN=%f, preConcMixingDIN=%f, postConcMixingDIN=%f \n",preConcDIN,postConcDIN,preConcMixingDIN,postConcDINMixing);
//if (itemID==809) printf ("DIN_Vf_ref=%f, DIN_Vf=%f, uptakeMC=%f, removalTotal=%f,totalMassRemovedMC_DIN=%f \n",DIN_Vf_ref,DIN_Vf,uptakeMC,removalTotal, totalMassRemovedMC_DIN);

//if (itemID == cell_1 || itemID == cell_2) printf("**** itemID = %d, y = %d, m = %d, d = %d, discharge = %f, dischargePre = %f, waterStoragePrev = %f, waterTotalVolume = %f\n", itemID, MFDateGetCurrentYear(), MFDateGetCurrentMonth(), MFDateGetCurrentDay(), discharge, dischargePre, waterStoragePrev, waterTotalVolume);
//if (itemID == cell_1 || itemID == cell_2) printf("ktMC = %f, DINTotalIn = %f,localLoad_DIN = %f, preFlux_DIN = %f, storeWater_DIN = %f, preConcDIN = %f\n", ktMC, DINTotalIn, localLoad_DIN, preFlux_DIN, storeWater_DIN, preConcDIN);
//if (itemID == cell_1 || itemID == cell_2) printf("aA = %f, width = %f, depth = %f, dL = %f, asDZ = %f, asHZ = %f, alphaDZ = %f, alphaHZ = %f, tstorDZ = %f, tstorHZ = %f, transferDZ = %f, transferHZ = %f\n", aA, width, depth, dL, asDZ, asHZ, alphaDZ, alphaHZ, tStorDZ, tStorHZ, transferDZ, transferHZ);
//if (itemID == cell_1 || itemID == cell_2) printf("DIN_Vf = %f, DIN_Kt = %f, hydLoad = %f, vf = %f, uptakeMC = %f, uptakeDZ = %f, uptakeHZ = %f, removalMC = %f, removalDZ = %f, removalHZ = %f\n", DIN_Vf, DIN_Kt, hydLoad, vf, uptakeMC, uptakeDZ, uptakeHZ, removalMC, removalDZ, removalHZ);
//if (itemID == cell_1 || itemID == cell_2) printf("totalMassRemovedDZ_DIN = %f, totalMassRemovedHZ_DIN = %f, totalMassRemovedMC_DIN = %f, postConcDIN = %f, postFluxDIN = %f\n", totalMassRemovedDZ_DIN, totalMassRemovedHZ_DIN, totalMassRemovedMC_DIN, postConcDIN, postFluxDIN);
//if (itemID == cell_1 || itemID == cell_2) printf("waterT = %f, postConcDINMixing = %f, tnTref = %f, DIN_Vf_ref = %f, DIN_Kt_ref = %f\n", waterT, postConcDINMixing, tnTref, DIN_Vf_ref, DIN_Kt_ref);

MFVarSetFloat (_MDOutPreFlux_DINID,              itemID, preFlux_DIN);				// RJS 050911
MFVarSetFloat (_MDOutTotalMassRemoved_DINID,     itemID, totalMassRemoved_DIN);		// RJS 032509
MFVarSetFloat (_MDStoreWater_DINID,              itemID, postStoreWater_DIN);
MFVarSetFloat (_MDFlux_DINID,           	 itemID, postFluxDIN);		// changed from MDOutFlux_DINID 091408
MFVarSetFloat (_MDOutTotalMassRemovedTS_DINID,   itemID, totalMassRemovedTS_DIN);
MFVarSetFloat (_MDOutPostConc_DINID,		 itemID, postConcDIN);
MFVarSetFloat (_MDOutPreConc_DINID,              itemID, preConcDIN);		// RJS 120408
MFVarSetFloat (_MDOutTotalMassRemovedDZ_DINID,   itemID, totalMassRemovedDZ_DIN);
MFVarSetFloat (_MDOutTotalMassRemovedHZ_DINID,   itemID, totalMassRemovedHZ_DIN);
MFVarSetFloat (_MDOutTotalMassRemovedMC_DINID,   itemID, totalMassRemovedMC_DIN);
MFVarSetFloat (_MDOutTimeOfStorageDZID, 	 itemID, tStorDZ);
MFVarSetFloat (_MDOutTimeOfStorageHZID, 	 itemID, tStorHZ);
MFVarSetFloat (_MDOutTimeOfStorageMCID,          itemID, tStorMC);		// RJS 120608
MFVarSetFloat (_MDOutTransferDZID,               itemID, transferDZ);
MFVarSetFloat (_MDOutTransferHZID,        	 itemID, transferHZ);
MFVarSetFloat (_MDOutWaterDZID, 		 itemID, waterDZ);
MFVarSetFloat (_MDOutWaterHZID, 		 itemID, waterHZ);
MFVarSetFloat (_MDOutUptakeVfID, 		 itemID, DIN_Vf);
MFVarSetFloat (_MDFluxMixing_DINID, 		 itemID, postFluxDINMixing);		// changed from MDOutFluxMixing_DINID 091408
MFVarSetFloat (_MDStoreWaterMixing_DINID,        itemID, postStoreWaterMixing_DIN);
MFVarSetFloat (_MDOutTotalMassPre_DINID, 	 itemID, DINTotalIn);		// RJS 091408
MFVarSetFloat (_MDOutConcMixing_DINID,           itemID, postConcDINMixing);			// RJS 091408
MFVarSetFloat (_MDOutMassBalance_DINID, 	 itemID, massBalance_DIN);		// RJS 110308
MFVarSetFloat (_MDOutMassBalanceMixing_DINID,    itemID, massBalanceMixing_DIN);	// RJS 112008
MFVarSetFloat (_MDOutRemovalDZID,                itemID, removalDZ);		//RJS 110708
MFVarSetFloat (_MDOutRemovalHZID, 		 itemID, removalHZ);		//RJS 110708
MFVarSetFloat (_MDOutRemovalMCID, 		 itemID, removalMC);
MFVarSetFloat (_MDOutRemovalTotalID,             itemID, removalTotal);		// RJS 03-01-09
MFVarSetFloat (_MDOutAaID,                       itemID, aA);		//RJS 110808
MFVarSetFloat (_MDOutAsDZID,                     itemID, asDZ);		//RJS 110808
MFVarSetFloat (_MDOutAsHZID,                     itemID, asHZ);		//RJS 110808
MFVarSetFloat (_MDDeltaStoreWater_DINID,         itemID, DINDeltaStorage);
MFVarSetFloat (_MDDeltaStoreWaterMixing_DINID,   itemID, DINDeltaStorageMixing);
MFVarSetFloat (_MDFlowPathRemoval_DINID,         itemID, flowPathRemoval);
MFVarSetFloat (_MDFlowPathRemovalMixing_DINID,   itemID, flowPathRemovalMixing);

}

int MDDINDef () {


	float par1;		//RJS 032509
	float par2;		//RJS 032509
	float par3;		//RJS 032509
	float par4;		//RJS 032509
	float par5;		//RJS 032509
	float par6;		//RJS 033009
	float par7;		//RJS 033009

	const char *optStr1, *optStr2, *optStr3, *optStr4, *optStr5, *optStr6, *optStr7;		//RJS 032509

	if (((optStr1 = MFOptionGet (MDParAlphaSTS))  != (char *) NULL) && (sscanf (optStr1,"%f",&par1) == 1)) _MDAlphaSTS = par1;  // RJS 032509
	if (((optStr2 = MFOptionGet (MDParAlphaHTS))  != (char *) NULL) && (sscanf (optStr2,"%f",&par2) == 1)) _MDAlphaHTS = par2;  // RJS 032509
	if (((optStr3 = MFOptionGet (MDParASTSA))  != (char *) NULL) && (sscanf (optStr3,"%f",&par3) == 1)) _MDASTSA = par3;  // RJS 032509
	if (((optStr4 = MFOptionGet (MDParAHTSA))  != (char *) NULL) && (sscanf (optStr4,"%f",&par4) == 1)) _MDAHTSA = par4;  // RJS 032509
	if (((optStr5 = MFOptionGet (MDParUptakeKtMC))     != (char *) NULL) && (sscanf (optStr5,"%f",&par5) == 1)) _MDUptakeKtMC = par5; // RJS 033009
	if (((optStr6 = MFOptionGet (MDParUptakeKtSTS))     != (char *) NULL) && (sscanf (optStr6,"%f",&par6) == 1)) _MDUptakeKtSTS = par6; // RJS 033009
	if (((optStr7 = MFOptionGet (MDParUptakeKtHTS))     != (char *) NULL) && (sscanf (optStr7,"%f",&par7) == 1)) _MDUptakeKtHTS = par7; // RJS 033009


	MFDefEntering ("N Processing");



   // Input
	if (
		((_MDWTemp_QxTID 			= MDThermalInputs3Def ()) == CMfailed) ||		// comment out for no plants
//		((_MDWTemp_QxTID 		    = MDWTempRiverRouteDef()) == CMfailed) ||
		((_MDInDischarge0ID         = MFVarGetID (MDVarDischarge0,    "m3/s",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||					// RJS 01-06-09     includes local cells runoff
	    ((_MDInDischargeID          = MFVarGetID (MDVarDischarge,     "m3/s",  MFRoute,  MFState, MFBoundary)) == CMfailed) ||
	    ((_MDInRiverStorageID       = MFVarGetID (MDVarRiverStorage,           "m3/day",   MFInput,  MFState,  MFInitial)) == CMfailed)  ||		// RJS 01-06-09		m3/day instead of m3/s
	    ((_MDInRiverStorageChgID    = MFVarGetID (MDVarRiverStorageChg,        "m3/day",   MFInput,  MFState,  MFBoundary)) == CMfailed)  ||     // RJS 01-06-09
	    ((_MDInRiverOrderID         = MFVarGetID (MDVarRiverOrder,               "-",   MFInput,  MFState, MFBoundary)) == CMfailed)  ||			//RJS 112211
	    ((_MDInRiverWidthID         = MDRiverWidthDef ())   == CMfailed) ||
	    ((_MDInRiverDepthID		    = MFVarGetID (MDVarRiverDepth,   	   	    "m",   MFInput,  MFState, MFBoundary)) == CMfailed)  ||
	    ((_MDInLocalLoad_DINID	    = MDNitrogenInputsDef()) == CMfailed) ||	// RJS 091108
	    ((_MDInVfAdjustID           = MFVarGetID (MDVfAdjust,   "-",   MFInput, MFState, MFBoundary)) == CMfailed) ||
	    ((_MDFlux_DINID             = MFVarGetID (MDVarFluxDIN,                "kg/day",   MFRoute,  MFFlux,  MFBoundary)) == CMfailed)  ||		// RJS 091408
	    ((_MDStoreWater_DINID       = MFVarGetID (MDVarStoreWaterDIN,          "kg/day",   MFOutput,  MFState,  MFInitial)) == CMfailed)  ||		// RJS 091108
	    ((_MDInRunoffVolID          = MFVarGetID (MDVarRunoffVolume, 		 "m3/s",   MFInput, MFState,  MFBoundary)) == CMfailed)  ||		// RJS 091108
	    ((_MDInTnQ10ID		        = MFVarGetID (MDVarTnQ10,               "-",     MFInput, MFState,  MFBoundary)) == CMfailed)  || 	//RJS 102410


   // Output
	    ((_MDOutPostConc_DINID		= MFVarGetID (MDVarPostConcDIN,	       "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutPreConc_DINID		= MFVarGetID (MDVarPreConcDIN,	       "mg/L",   MFOutput,  MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedTS_DINID   	= MFVarGetID (MDVarTotalMassRemovedTSDIN,    "kg/day", MFOutput, MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedDZ_DINID 	= MFVarGetID (MDVarTotalMassRemovedDZDIN,  "kg/day",   MFOutput, MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedHZ_DINID 	= MFVarGetID (MDVarTotalMassRemovedHZDIN,  "kg/day",   MFOutput, MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemovedMC_DINID 	= MFVarGetID (MDVarTotalMassRemovedMCDIN,  "kg/day",   MFOutput, MFFlux, MFBoundary))   == CMfailed) ||
	    ((_MDOutTotalMassRemoved_DINID      = MFVarGetID (MDVarTotalMassRemovedDIN,    "kg/day",   MFOutput, MFFlux, MFBoundary))   == CMfailed) ||		// RJS 032509
	    ((_MDOutTimeOfStorageDZID		= MFVarGetID (MDVarTimeOfStorageDZ,          "days",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTimeOfStorageHZID		= MFVarGetID (MDVarTimeOfStorageHZ,          "days",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTimeOfStorageMCID		= MFVarGetID (MDVarTimeOfStorageMC,          "days",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 120608
	    ((_MDOutTransferDZID		= MFVarGetID (MDVarTransferDZ,               "m3/s",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutTransferHZID		= MFVarGetID (MDVarTransferHZ,               "m3/s",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutPreFlux_DINID               = MFVarGetID (MDVarPreFluxDIN,               "kg/day", MFOutput, MFState, MFBoundary))   == CMfailed) || 	//RJS 050911
	    ((_MDOutWaterDZID			= MFVarGetID (MDVarWaterDZ, 				    "m3",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutWaterHZID			= MFVarGetID (MDVarWaterHZ, 				    "m3",   MFOutput, MFState, MFBoundary))   == CMfailed) ||
	    ((_MDOutUptakeVfID               	= MFVarGetID (MDVarUptakeVf,                 "m/day",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 090508
	    ((_MDFluxMixing_DINID		= MFVarGetID (MDVarFluxMixingDIN,  		"	kg/day",    MFRoute,  MFFlux, MFBoundary))   == CMfailed) ||	// RJS 091108
	    ((_MDStoreWaterMixing_DINID      	= MFVarGetID (MDVarStoreWaterMixingDIN,    "kg/day",   MFOutput, MFState, MFInitial))   == CMfailed) ||	// RJS 091108
	    ((_MDOutTotalMassPre_DINID       	= MFVarGetID (MDVarTotalMassPreDIN,        "kg/day",   MFOutput, MFFlux, MFBoundary))   == CMfailed) ||	// RJS 091408
	    ((_MDOutConcMixing_DINID         	= MFVarGetID (MDVarConcMixing,    		      "mg/L",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 091108
	    ((_MDOutMassBalance_DINID		= MFVarGetID (MDVarMassBalanceDIN,          "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 110308
	    ((_MDOutMassBalanceMixing_DINID	= MFVarGetID (MDVarMassBalanceMixingDIN,    "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 110308
	    ((_MDOutRemovalDZID              	= MFVarGetID (MDVarRemovalDZ,               "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalHZID              	= MFVarGetID (MDVarRemovalHZ,               "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalMCID              	= MFVarGetID (MDVarRemovalMC,               "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 110708
	    ((_MDOutRemovalTotalID              = MFVarGetID (MDVarRemovalTotal,            "kg/kg",   MFOutput, MFState, MFBoundary))   == CMfailed) || 	// RJS 030109
	    ((_MDOutAsDZID                   	= MFVarGetID (MDVarAsDZ,                       "m2",   MFOutput, MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutAsHZID                   	= MFVarGetID (MDVarAsHZ,                       "m2",   MFOutput, MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutAaID                     	= MFVarGetID (MDVarAa,                         "m2",   MFOutput, MFState, MFBoundary))   == CMfailed) ||   // RJS 110808
	    ((_MDOutVelocityID                  = MFVarGetID (MDVarVelocity,                  "m/s",   MFOutput, MFState, MFBoundary))   == CMfailed) ||   // RJS 112108
	    ((_MDOutDINVfID                     = MFVarGetID (MDVarDINVf,                       "-",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
	    ((_MDOutDINKtID                     = MFVarGetID (MDVarDINKt,                       "-",   MFOutput, MFState, MFBoundary))   == CMfailed) ||	// RJS 112210
  	    ((_MDDeltaStoreWater_DINID       	= MFVarGetID (MDVarDeltaStoreWaterDIN,    	 "kg/day", MFOutput, MFState, MFBoundary)) == CMfailed) ||   // RJS 112008
	    ((_MDDeltaStoreWaterMixing_DINID    = MFVarGetID (MDVarDeltaStoreWaterMixingDIN,  "kg/day", MFOutput, MFState, MFBoundary)) == CMfailed) ||   // RJS 112008
	    ((_MDFlowPathRemoval_DINID       	= MFVarGetID (MDVarFlowPathRemovalDIN,      	"kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||   // RJS 112008
	    ((_MDFlowPathRemovalMixing_DINID    = MFVarGetID (MDVarFlowPathRemovalMixingDIN,  "kg/day", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||   // RJS 112008


	(MFModelAddFunction (_MDNProcessing) == CMfailed)) return (CMfailed);

	MFDefLeaving ("N Processing");

	return (_MDFlux_DINID);		// RJS 091408
}




