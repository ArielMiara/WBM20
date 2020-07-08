/******************************************************************************

 GHAAS Water Balance/Transport Model V3.0
 Global Hydrologic Archive and Analysis System
 Copyright 1994-2020, UNH - ASRC/CUNY

 MD.h

 balazs.fekete@unh.edu

 *******************************************************************************/

#ifndef MD_H_INCLUDED
#define MD_H_INCLUDED

#if defined(__cplusplus)
extern "C" {
#endif

#define MDNoneStr      "none"
#define MDInputStr     "input"
#define MDLookupStr    "lookup"
#define MDCalculateStr "calculate"

// Options
#define MDOptDINInputs						 "DINInputs"		// RJS 042513
#define MDOptDO2							 "DO2"				// RJS 111612
#define MDOptDischarge                       "Discharge"
#define MDOptSedimentFlux                    "SedimentFlux" //new !!!!!! (S.C)

#define MDOptIrrigation                      "Irrigation"
#define MDOptModel                           "Model"
#define MDOptReservoirs                      "Reservoirs"
#define MDOptRiverbed                        "Riverbed"
#define MDOptRunoff							 "RunoffSwitch"		//RJS 111710
#define MDOptSurfRunoffPool					 "SurfRunoffPool"	//RJS 042612
#define MDOptGrdWaterTemp					 "GrdWaterTemp"		//RJS 061312
#define MDOptRouting                         "Routing"
#define MDOptMuskingum                       "Muskingum"
#define MDOptIrrReferenceET                  "IrrReferenceETP"
#define MDOptIrrIntensity                    "DoubleCropping"
#define MDOptIrrExcessWater                  "ExcessWater"
#define MDOptSoilMoisture					 "SoilMoisture"
#define MDOptSoilTemperature				 "SoilTemperature"
#define MDOptAllWithdrawals					 "AllWithdrawals"


#define MDOptIrrigatedAreaMap                "IrrigatedAreaMap"
#define MDOptSoilAvailableWaterCapacity		 "SoilWaterCapacity"
#define MDOptThermalInputs					 "ThermalInputs"
#define MDOptThermalInputs2					 "ThermalInputs2"		// RJS 062012
#define MDOptThermalInputs3					 "ThermalInputs3"		// RJS 112712

#define MDVarAccBalance                      "AccumBalance"
#define MDVarAccPrecipitation                "AccumPrecipitation"
#define MDVarAccEvapotranspiration           "AccumEvapotranspiration"
#define MDVarAccSoilMoistChange              "AccumSoilMoistureChange"
#define MDVarAccGroundWaterChange            "AccumGroundWaterChange"
#define MDVarAccRunoff                       "AccumRunoff"
#define MDVarAccBalance                      "AccumBalance"

#define MDVarApproach						 "Approach"					// RJS 062012

#define MDVarAirTemperature                  "AirTemperature"
#define MDVarAirTempMinimum                  "AirTempMinimum"
#define MDVarAirTempMaximum                  "AirTempMaximum"
#define MDVarAirTemperatureAcc		"AirTempAcc"
#define MDVarAirTemperatureAcc_space		"AirTempAcc_space"
#define MDVarAirTemperatureAcc_time		"AirTempAcc_time"
#define MDVarNewAirTemperatureAcc_time		"NewAirTempAcc_time"
#define MDVarAvgNSteps                       "AverageNSteps"
#define MDVarContributingArea                       "ContributingArea"
#define MDVarContributingAreaAcc                       "ContributingAreaAcc"

#define MDVarAvgEfficiency					 "AvgEfficiency"		// RJS 062012
#define MDVarAvgDeltaTemp					 "AvgDeltaTemp"			// RJS 062012

#define MDVarBaseFlow                        "BaseFlow"
#define MDVarBGCNonPoint_NO3                 "NinputsNO3_nonpoint"
#define MDVarBGCPointSources_NO3             "NinputsNO3_point" //TODO unused!
#define MDVarBGCFlux_NO3                     "BgcFlux_NO3"
#define MDVarBGCFlux_NH4                     "BgcFlux_NH4"
#define MDVarBGCFlux_DON                     "BgcFlux_DON"
#define MDVarBGCFlux_PON                     "BgcFlux_PON"

#define MDVarBGCStoreWater_NO3               "BgcStoreWater_NO3"
#define MDVarBgcLocalLoad_NO3                "BgcLocalLoad_NO3"

#define MDVarBGCStoreWater_NH4               "BgcStoreWater_NH4"
#define MDVarBGCLocalLoad_NH4                "BgcLocalLoad_NH4"

#define MDVarBGCStoreWater_DON               "BgcStoreWater_DON"
#define MDVarBGCLocalLoad_DON                "BgcLocalLoad_DON"

#define MDVarBGCStoreWater_PON               "BgcStoreWater_PON"
#define MDVarBGCLocalLoad_PON                "BgcLocalLoad_PON"
#define MDVarBGCStoreSeds_PON                "BgcStoreSeds_PONkgm2"

#define MDVarBGCNonPoint_NH4                 "NinputsNH4_nonpoint"
#define MDVarBGCPointSources_NH4             "NinputsNH4_point"
#define MDVarBGCNonPoint_DON                 "NinputsDON_nonpoint"
#define MDVarBGCNonPoint_PON                 "NinputsPON_nonpoint"
#define MDVarBGCNonPoint_DIN                 "NinputsDIN_nonpoint"
#define MDVarBGCPointSources_DIN             "NinputsDIN_point"

#define MDVarBypassPercent					 "BypassPercent"			// RJS 062012

#define MDVarCParamAlbedo                    "Albedo"
#define MDVarCParamCHeight                   "CanopyHeight"
#define MDVarCParamLWidth                    "LeafWidth"
#define MDVarCParamRSS                       "CParamRSS"
#define MDVarCParamR5                        "CParamR5"
#define MDVarCParamCD                        "CParamCD"
#define MDVarCParamCR                        "CParamCR"
#define MDVarCParamGLMax                     "CParamGLMax"
#define MDVarCParamLPMax                     "CParamLPMax"
#define MDVarCParamZ0g                       "CParamZ0g"
#define MDVarCloudCover                      "CloudCover"

#define MDVarDeltaT							 "DeltaT"				// RJS 030613
#define MDVarDischarge                       "Discharge"
#define MDVarDischarge0                      "Discharge0"
#define MDVarDischarge1                      "Discharge1"
#define MDVarDischObserved                   "DischargeObserved"
#define MDVarDischMean                       "DischargeMean"
#define MDVarDischargeAcc	"DischargeAcc"
#define MDVarNewDischargeAcc	"NewDischargeAcc"
#define MDVarOverBankQ	"OverBankQ"

#define MDVarElevationMax              "MaxElevation"
#define MDVarElevation              "Elevation"

#define MDVarDingmanOnOff					 "DingmanOnOff"			// RJS 073012

#define MDVarEfficiency1					 "Efficiency1"			// RJS 062012
#define MDVarEnergyDemand					 "EnergyDemand"			// RJS 062012

#define MDVarDischargeMin                    "DischargeMin"
#define MDVarDischargeMax                    "DischargeMax"
#define MDVarDischarge_t_1                   "Dischage_t_1"
#define MDVarDischarge_t_2                   "Dischage_t_2"

#define MDVarEvapotranspiration              "Evapotranspiration"
#define MDVarFuelType1						 "FuelType1"			//RJS 013012
#define MDVarFuelType2						 "FuelType2"			//RJS 013012
#define MDVarFuelType3						 "FuelType3"			//RJS 013012
#define MDVarFuelType4						 "FuelType4"			//RJS 013012
#define MDVarFuelType5						 "FuelType5"			//RJS 013012
#define MDVarFuelType6						 "FuelType6"			//RJS 013012
#define MDVarFuelType7						 "FuelType7"			//RJS 013012
#define MDVarFuelType8						 "FuelType8"			//RJS 013012
#define MDVarGrossRadiance                   "GrossRadiance"
#define MDVarGroundWater                     "GroundWater"
#define MDVarGroundWaterChange               "GroundWaterChange"
#define MDVarGroundWaterRecharge             "GroundWaterRecharge"
#define MDVarGroundWaterUptake               "GroundWaterUptake"
#define MDVarLeafAreaIndex                   "LeafAreaIndex"

#define MDVarHCIA							 "HCIA"					// RJS 082812

#define MDVarHeatToRiv						 "HeatToRiv"			// RJS 112712
#define MDVarHeatToSink						 "HeatToSink"			// RJS 112712
#define MDVarHeatToEng						 "HeatToEng"			// RJS 112712
#define MDVarHeatToElec						 "HeatToElec"			// RJS 112912
#define MDVarHeatToEvap						 "HeatToEvap"			// RJS 120212

#define MDVarImpFracSpatial					 "ImpFracSpatial"		//RJS 082812

#define MDVarIceCover			"IceCover"
	
#define MDVarIrrAreaFraction                 "IrrigatedAreaFraction"

#define MDVarIrrAreaFractionSeason1          "IrrigatedAreaFractionSeason1"
#define MDVarIrrAreaFractionSeason2          "IrrigatedAreaFractionSeason2"

#define MDVarIrrEfficiency                   "IrrigationEfficiency"
#define MDVarIrrEvapotranspiration           "IrrEvapotranspiration"
#define MDVarIrrGrossDemand                  "IrrGrossDemand"
#define MDVarIrrGrowingSeason1Start          "GrowingSeason1_Start"
#define MDVarIrrGrowingSeason2Start          "GrowingSeason2_Start"

#define MDVarIrrIntensity                    "IrrigationIntensity"
#define MDVarIrrNetWaterDemand               "IrrNetWaterDemand"
#define MDVarIrrRefEvapotrans                "IrrReferenceETP"
#define MDVarIrrReturnFlow                   "IrrReturnFlow"
#define MDVarIrrSoilMoisture                 "IrrSoilMoisture"
#define MDVarIrrSoilMoistChange              "IrrSoilMoistChange"
#define MDVarIrrUptakeBalance                "IrrUptakeBalance"
#define MDVarIrrUptakeExternal               "IrrUptakeExternal"
#define MDVarIrrUptakeExcess                 "IrrUptakeExcess"
#define MDVarIrrUptakeGrdWater               "IrrUptakeGroundWater"
#define MDVarIrrUptakeRiver                  "IrrUptakeRiver"       
#define MDVarIrrWaterBalance                 "IrrWaterBalance"
#define MDNonIrrigatedFraction               "NonIrrigatedFraction"	
#define MDVarIrrDailyRicePerolationRate      "RicePercolationRate"	
#define MDVarIrrRicePondingDepth			 "RicePondingDepth"

#define MDVarLakeOcean1						 "LakeOcean1"		// RJS 062012
#define MDVarLHFract						 "LHFract"			// RJS 071112
#define MDVarLHFractPost					 "LHFractPost"		// RJS 071112

#define	MDVarMeanElevation                   "Elevation"
#define MDVarMuskingumC0                     "MuskingumC0"
#define MDVarMuskingumC1                     "MuskingumC1"
#define MDVarMuskingumC2                     "MuskingumC2"

#define MDVarNamePlate1						 "NamePlate1"		//RJS 013012
#define MDVarNamePlate2						 "NamePlate2"		//RJS 013012
#define MDVarNamePlate3						 "NamePlate3"		//RJS 013012
#define MDVarNamePlate4						 "NamePlate4"		//RJS 013012
#define MDVarNamePlate5						 "NamePlate5"		//RJS 013012
#define MDVarNamePlate6						 "NamePlate6"		//RJS 013012
#define MDVarNamePlate7						 "NamePlate7"		//RJS 013012
#define MDVarNamePlate8						 "NamePlate8"		//RJS 013012

#define MDVarOptDeltaT						 "OptDeltaT"		//RJS 013012

#define MDVarPlantCode1						 "PlantCode1"		// RJS 062012

#define MDVarPowerOutput1					 "PowerOutput1"		//RJS 013112
#define MDVarPowerOutput2					 "PowerOutput2"		//RJS 013112
#define MDVarPowerOutput3					 "PowerOutput3"		//RJS 013112
#define MDVarPowerOutput4					 "PowerOutput4"		//RJS 013112
#define MDVarPowerOutput5					 "PowerOutput5"		//RJS 013112
#define MDVarPowerOutput6					 "PowerOutput6"		//RJS 013112
#define MDVarPowerOutput7					 "PowerOutput7"		//RJS 013112
#define MDVarPowerOutput8					 "PowerOutput8"		//RJS 013112

#define MDVarPowerDeficit1					 "PowerDeficit1"		//RJS 013112
#define MDVarPowerDeficit2					 "PowerDeficit2"		//RJS 013112
#define MDVarPowerDeficit3					 "PowerDeficit3"		//RJS 013112
#define MDVarPowerDeficit4					 "PowerDeficit4"		//RJS 013112
#define MDVarPowerDeficit5					 "PowerDeficit5"		//RJS 013112
#define MDVarPowerDeficit6					 "PowerDeficit6"		//RJS 013112
#define MDVarPowerDeficit7					 "PowerDeficit7"		//RJS 013112
#define MDVarPowerDeficit8					 "PowerDeficit8"		//RJS 013112

#define MDVarPowerPercent1					 "PowerPercent1"		//RJS 013112
#define MDVarPowerPercent2					 "PowerPercent2"		//RJS 013112
#define MDVarPowerPercent3					 "PowerPercent3"		//RJS 013112
#define MDVarPowerPercent4					 "PowerPercent4"		//RJS 013112
#define MDVarPowerPercent5					 "PowerPercent5"		//RJS 013112
#define MDVarPowerPercent6					 "PowerPercent6"		//RJS 013112
#define MDVarPowerPercent7					 "PowerPercent7"		//RJS 013112
#define MDVarPowerPercent8					 "PowerPercent8"		//RJS 013112

#define MDVarPowerOutputTotal				 "PowerOutputTotal"		//RJS 013112
#define MDVarPowerDeficitTotal				 "PowerDeficitTotal"	//RJS 013112
#define MDVarPowerPercentTotal				 "PowerPercentTotal"	//RJS 013112

#define MDVarPrecipitation                   "Precipitation"
#define MDVarPrecipFraction                  "PrecipitationFraction"
#define MDVarPrecipMonthly                   "MonthlyPrecipitation"

#define MDVarPrecipPerv						 "PrecipPerv"			// RJS 082812

#define MDVarLakeOcean						 "LakeOcean"			// RJS 062012

#define MDVarWetlandEvapotranspiration		 "WetlandEvapotranspiration"
#define MDVarWetlandAreaFraction			 "WetlandAreaFraction"

#define MDVarQpp1							 "Qpp1"			//RJS 071112
#define MDVarOptQO1							 "OptQO1"		//RJS 071112

#define MDVarRainEvapotranspiration          "RainEvapotranspiration"
#define	MDVarRainETnotScaled                 "RainETnotScaled"
#define MDVarRainInfiltration                "RainInfiltration"
#define MDVarRainInterception                "RainInterception"
#define MDVarRainPotEvapotrans               "RainPET"
#define MDVarRainSoilMoisture                "RainSoilMoisture"
#define MDVarRainSoilMoistureCell            "RainSoilMoistureCell"
#define MDVarRainSoilMoistChange             "RainSoilMoistureChange"
#define MDVarRainSurfRunoff                  "RainSurfRunoff"
#define MDVarRainTranspiration               "RainTranspiration"
#define MDVarRainWaterSurplus                "RainWaterSurplus"

#define MDVarReservoirCapacity               "ReservoirCapacity"
#define MDVarReservoirRelease                "ReservoirRelease"
#define MDVarReservoirStorage                "ReservoirStorage" 
#define MDVarReservoirStorageChange          "ReservoirStorageChange"
#define MDVarRelief                          "ReliefMax"

#define MDVarReservoirRelease_t_1            "ReservoirReslease_t_1"

#define MDVarRelHumidity                     "RelativeHumidity"
#define MDVarRelSoilMoisture                 "RelativeSoilMoisture"	
#define MDVarRiverbedAvgDepthMean            "RiverbedAvgDepthMean"
#define MDVarRiverbedSlope                   "RiverbedSlope"
#define MDVarRiverbedShapeExponent           "RiverbedShapeExponent"
#define MDVarRiverbedWidthMean               "RiverbedWidthMean"

#define MDVarRiverbedVelocityMean            "RiverbedVelocityMean"
#define MDVarRiverbedReachLength	     "RiverbedReachLength"
#define MDVarRiverDepth                      "RiverDepth"

// DIN Variables //
#define MDVarRiverOrder						 "RiverOrder"				// RJS 042413
#define MDVarFluxDIN						 "FluxDIN"
#define MDVarStoreWaterDIN					 "StoreWaterDIN"
#define MDVarTnQ10							 "TnQ10"
#define MDVarPostConcDIN					 "PostConcDIN"
#define MDVarPreConcDIN						 "PreConcDIN"
#define MDVarTotalMassRemovedTSDIN			 "TotalMassRemovedTSDIN"
#define MDVarTotalMassRemovedDZDIN			 "TotalMassRemovedDZDIN"
#define MDVarTotalMassRemovedHZDIN			 "TotalMassRemovedHZDIN"
#define MDVarTotalMassRemovedMCDIN			 "TotalMassRemovedMCDIN"
#define MDVarTotalMassRemovedDIN			 "TotalMassRemovedDIN"
#define MDVarTimeOfStorageDZ				 "TimeOfStorageDZ"
#define MDVarTimeOfStorageHZ				 "TimeOfStorageHZ"
#define MDVarTimeOfStorageMC				 "TimeOfStorageMC"
#define MDVarTransferDZ						 "TransferDZ"
#define MDVarTransferHZ						 "TransferHZ"
#define MDVarPreFluxDIN						 "PreFluxDIN"
#define MDVarWaterDZ						 "WaterDZ"
#define MDVarWaterHZ						 "WaterHZ"
#define MDVarUptakeVf						 "UptakeVf"
#define MDVarFluxMixingDIN					 "FluxMixingDIN"
#define MDVarStoreWaterMixingDIN			 "StoreWaterMixingDIN"
#define MDVarTotalMassPreDIN				 "TotalMassPreDIN"
#define MDVarConcMixing						 "ConcMixing"
#define MDVarMassBalanceDIN					 "MassBalanceDIN"
#define MDVarMassBalanceMixingDIN			 "MassBalanceMixingDIN"
#define MDVarRemovalDZ						 "RemovalDZ"
#define MDVarRemovalHZ						 "RemovalHZ"
#define MDVarRemovalMC						 "RemovalMC"
#define MDVarRemovalTotal					 "RemovalTotal"
#define MDVarAsDZ							 "AsDZ"
#define MDVarAsHZ							 "AsHZ"
#define MDVarAa								 "Aa"
#define MDVarVelocity						 "Velocity"
#define MDVarDINVf							 "DINVf"
#define MDVarDINKt							 "DINKt"
#define MDVarDeltaStoreWaterDIN				 "DeltaStoreWaterDIN"
#define MDVarDeltaStoreWaterMixingDIN		 "DeltaStoreWaterMixingDIN"
#define MDVarFlowPathRemovalDIN				 "FlowPathRemovalDIN"
#define MDVarFlowPathRemovalMixingDIN		 "FlowPathRemovalMixingDIN"
#define MDVfAdjust							 "VfAdjust"

#define MDVarLandUseSpatial					 "LandUseSpatial"
#define MDVarLawnFraction					 "LawnFraction"
#define MDVarLoadAdjust						 "LoadAdjust"
#define MDVarLocalLoadDIN					 "LocalLoadDIN"
#define MDVarLocalLoadDINnew				 "LocalLoadDINnew"
#define MDVarGroundWaterMassRel				 "GroundWaterMassRel"
#define MDVarRunoffPoolMassRel				 "RunoffPoolMassRel"
#define MDVarDINLoadConc					 "DINLoadConc"

#define MDVarLocalLoadSubDIN				 "LocalLoadSubDIN"
#define MDVarLocalLoadAgDIN					 "LocalLoadAgDIN"
#define MDVarDINSubLoadConc					 "DINSubLoadConc"
#define MDVarDINAgLoadConc					 "DINAgLoadConc"
#define MDVarLandUseSpatialSub				 "LandUseSpatialSub"
#define MDVarLandUseSpatialAg				 "LandUseSpatialAg"

// DO2 Variables //
#define MDVarRiverMassDO2					 "RiverMassDO2"				// RJS 111612
#define MDVarRiverConcDO2					 "RiverConcDO2"				// RJS 111612
#define MDVarRiverStorMassDO2				 "RiverStorMassDO2"			// RJS 111512
#define MDVarRiverStorMassChgDO2			 "RiverStorMassChgDO2"		// RJS 111612
#define MDVarRiverMixingMassDO2				 "RiverMixingMassDO2"  		// RJS 111612
#define MDVarRiverMixingConcDO2				 "RiverMixingConcDO2"		// RJS 111612
#define MDVarRiverMixingStorMassDO2			 "RiverMixingStorMassDO2"	// RJS 111612
#define MDVarRiverMixingStorMassChgDO2		 "RiverMixingStorMassChgDO2" // RJS 111612

#define MDVarRiverStorage                    "RiverStorage"
#define MDVarRiverStorageChg                 "RiverStorageChange"
#define MDVarRiverWidth                      "RiverbedWidth"


#define MDVarRiverThreshT					 "RiverThreshT"				// RJS 062012

#define MDVarRunoff                          "Runoff"
#define MDVarRunoffMean                      "RunoffMean"
#define MDVarRunoffCorretion                 "RunoffCorrection"
#define MDVarRunoffVolume                    "RunoffVolume"

#define MDVarRunoffPool						 "RunoffPool"			//RJS 042012
#define MDVarRunoffPoolChg					 "RunoffPoolChg"		//RJS 042012
#define MDVarRunoffPoolRecharge				 "RunoffPoolRecharge"	//RJS 042012
#define MDVarRunoffPoolRelease				 "RunoffPoolRelease"	//RJS 042012

#define MDVarRunofftoPerv					 "RunoffToPerv"	//RJS 042010

#define MDVarSedimentFlux		     "SedimentFlux"
#define MDVarBedloadFlux		     "BedloadFlux"

#define MDVarSedimentAcc		"SedimentAcc"	
#define MDVarNewSedimentAcc		"NewSedimentAcc"	
#define MDVarBQART_A			     "BQART_A"
#define MDVarBQART_B	     	     "BQART_B"
#define MDVarQbar	     	     "Qbar"
#define MDVarBQART_Qbar_km3y	"BQART_Qbar_km3y"
#define MDVarBQART_Qbar_m3s	"BQART_Qbar_m3s"
#define MDVarBQART_R	     	     "BQART_R"
#define MDVarBQART_T	     	     "BQART_T"
#define MDVarBQART_Lithology	     "BQART_Lithology"
#define MDVarBQART_GNP	     	     "BQART_GNP"
#define MDVarPopulation	     	     "Population"
#define MDVarLithologyAreaAcc	"LithologyAreaAcc"
#define MDVarLithologyMean	"LithologyMean"
#define MDVarPopulationAcc	"PopulationAcc"
#define MDVarPopulationDensity  "PopulationDensity"
#define MDVarNumPixels	 	"NumPixels"
#define MDVarGNPAreaAcc		"GNPAreaAcc"
#define MDVarMeanGNP		"MeanGNP"
#define MDVarBQART_Eh		"BQART_Eh"
#define MDVarBQART_Te		"BQART_Te"
#define MDVarTeAacc		"TeAacc"
#define MDVarResStorageAcc	"ResStorageAcc"
#define MDVarBQART_Te		"BQART_Te"
#define MDVarQs_bar		"Qs_bar"
#define MDVarrnseed		"rnseed"
#define MDVarMinSlope		"Slope-Min"
#define MDVarPsi		"Psi"
#define MDVarC			"C"
#define MDVarUpStreamQb		"UpStreamQb"
#define MDDeltaBedload		"DeltaBedload"
#define MDVarUpStreamQs		"UpStreamQs"
#define MDDeltaQs		"DeltaQs"

#define MDVarBankfullQ		"BankfullQ"	
#define MDVarBankfullQ2		"BankfullQ2"
#define MDVarBankfullQ5		"BankfullQ5"
#define MDVarBankfullQ10	"BankfullQ10"
#define MDVarBankfullQ25		"BankfullQ25"
#define MDVarBankfullQ50		"BankfullQ50"
#define MDVarBankfullQ100		"BankfullQ100"
#define MDVarBankfullQ200		"BankfullQ200"
#define MDVarBankfull_Qn		"Bankfull_Qn"
#define MDVarFlowCoefficient		"FlowCoefficient"


#define MDVarSAGEVegCover                    "SAGEVegCover"
#define MDVarSmallReservoirStorageFrac       "SmallReservoirStorageFraction"	
#define MDVarSmallResCapacity                "SmallReservoirCapacity"
#define MDVarSmallResRelease                 "SmallReservoirRelease"
#define MDVarSmallResStorage                 "SmallReservoirStorage" 
#define MDVarSmallResStorageChange           "SmallReservoirStorageChange"	
#define MDVarSmallResUptake                  "SmallReservoirUptake"
#define MDVarSmallResEvaporation             "SmallReservoirEvaporation"
#define MDVarSnowFall                        "SnowFall"
#define MDVarSnowMelt                        "SnowMelt"
#define MDVarSnowPack                        "SnowPack"
#define MDVarSnowPackChange                  "SnowPackChange"
#define MDVarSoilAvailWaterCap               "AvailableWaterCapacity"
#define MDVarSoilFieldCapacity               "FieldCapacity"
#define MDVarSoilRootingDepth                "RootingDepth"
#define MDVarSoilMoisture                    "SoilMoisture"
#define MDVarSoilMoistChange                 "SoilMoistureChange"
#define MDVarSoilWiltingPoint                "WiltingPoint"
#define MDVarSoilBulkDensity                 "SoilBulkDensity"

#define MDVarSoilTotalVol					 "SoilTotalVol"			// RJS 082812

#define MDVarState							 "State"				// RJS 062012
#define MDVarStormRunoffImp					 "StormRunoffImp"		// RJS 082812
#define MDVarStormRunoffTotal				 "StormRunoffTotal"		// RJS 082812

#define MDVarTechnology1						 "Technology1"		// RJS 062012
#define MDVarTempLimit						 "TempLimit"		//RJS 013012

#define MDVarTempLimitCT						 "TempLimitCT"		//RJS 062012
#define MDVarTempLimitDE						 "TempLimitDE"		//RJS 062012
#define MDVarTempLimitMA						 "TempLimitMA"		//RJS 062012
#define MDVarTempLimitMD						 "TempLimitMD"		//RJS 062012
#define MDVarTempLimitME						 "TempLimitME"		//RJS 062012
#define MDVarTempLimitNH						 "TempLimitNH"		//RJS 062012
#define MDVarTempLimitNJ						 "TempLimitNJ"		//RJS 062012
#define MDVarTempLimitNY						 "TempLimitNY"		//RJS 062012
#define MDVarTempLimitPA						 "TempLimitPA"		//RJS 062012
#define MDVarTempLimitRI						 "TempLimitRI"		//RJS 062012
#define MDVarTempLimitVA						 "TempLimitVA"		//RJS 062012
#define MDVarTempLimitVT						 "TempLimitVT"		//RJS 062012
#define MDVarTempLimitDC						 "TempLimitDC"		//RJS 112712

#define MDVarTempIn1							 "TempIn1"			//RJS 111812
#define MDVarTpp1								 "Tpp1"				//RJS 111812

#define MDVarTotalHoursRun						 "TotalHoursRun"	// RJS 030213

#define MDVarTotal2008						 "Total2008"			//RJS 062012
#define MDVarTotalThermalWdls				 "TotalThermalWdls"		//RJS 013112
#define MDVarTotalOptThermalWdls			 "TotalOptThermalWdls" 	//RJS 020212
#define MDVarTotalEvaporation				 "TotalEvaporation"		// RJS 062012
#define MDVarTotalEnergyDemand				 "TotalEnergyDemand"	// RJS 062012
#define MDVarTotalReturnFlow				 "TotalReturnFlow"		// RJS 062012

#define MDVarTotalExternalWater				 "TotalExternalWater"	// RJS 120912

#define	MDVarTransferEffRip					 "TransferEffRip"	//RJS 061511
#define MDVarTransferEffSep					 "TransferEffSep"	//RJS 061511

#define MDVarThermalWdl						 "ThermalWdl"		//RJS 081711

#define MDVarTotalSurfRunoff					 "TotalSurfRunoff"	// RJS 082812

#define MDVarTimeSteps			"TimeSteps"
#define MDVarNewTimeSteps		"NewTimeSteps"

#define MDVarLakeArea                        "LakeArea" // TODO I think this is Wil's addition but not used yet
#define MDVarReservoirArea                   "ReservoirArea" // TODO I think this is Wil's addition but not used yet
#define MDVarWetlandProp                     "WetlandProp"
#define MDVarKoppen                          "Koppen"

#define MDVarSolarRadiation                  "SolarRadiation"
#define MDVarSRadDayLength                   "DayLength"
#define MDVarSRadI0HDay                      "I0HDay"
#define MDVarStemAreaIndex                   "StemAreaIndex"
#define MDVarSunShine                        "SunShine"
#define MDVarSurfRunoff                      "SurfaceRunoff"
#define MDVarVaporPressure                   "VaporPressure"
#define MDVarTEMVegCover                     "TEMVegCover"
#define MDVarWarmingTemp					 "WarmingTemp"
#define MDVarWBMCover                        "WBMCover"
#define MDVarWaterBalance                    "WaterBalance"
#define MDVarWetDays                         "WetDays"
#define MDVarWetDaysAlpha                    "WetDaysAlpha"
#define MDVarWetDaysBeta                     "WetDaysBeta"
#define MDVarWindSpeed                       "WindSpeed"

#define	MDVarSaturationExcessflow			"SaturationExcessFlow"

#define MDVarWetlandAreaFraction            "WetlandAreaFraction"
#define MDVarWetlandEvapotranspiration	    "WetlandEvapotranspiration"
#define MDVarWetlandSurfROUptake            "WetlandSurfROUptake"
#define MDVarWetlandRunoff                  "WetlandRunoff"
#define MDVarWetlandStorageChange           "WetlandStorageChange"
#define MDVarWetlandCatchmentAreaFactor		"WetlandCatchmentAreaFactor"
#define MDVarWetlandStorage					"WetlandStorage"	
#define MDVarWetlandWaterLevelDepth			"WetlandWaterLevelDepth"

#define MDVarBeaverPondArea						"BeaverPondArea"			//RJS 120211

#define MDVarBenthicAlgaeCHL                "BenthicAlgaeCHL"
#define MDVarBenthicAlgaeC                  "BenthicAlgaeC"
#define MDVarBenthicGPP                     "BenthicGPP"
#define MDVarBenthicRa                      "BenthicRa"
#define MDVarBenthicNPP                     "BenthicNPP"
#define MDVarBenthicMortality               "BenthicMortality_REACH"
#define MDVarBenthicAlgaeC_REACH            "BenthicAlgaeC_REACH"
#define MDVarBenthicGPP_REACH               "BenthicGPP_REACH"
#define MDVarBenthicRa_REACH                "BenthicRa_REACH"
#define MDVarBenthicNPP_REACH               "BenthicNPP_REACH"
#define MDVarBenthicMortality_REACH         "BenthicMortality_REACH"

#define MDVarCondenserInlet					"CondenserInlet"			//RJS 122112
#define MDVarSimEfficiency					"SimEfficiency"				//RJS 122112

// Water Temperature Variables
// Runoff Temperature
#define MDVarWTempGrdWater                   "GroundWaterTemperature"
#define MDVarWTempRiver                      "RiverTemperature"
#define MDVarWTempSurfRunoff                 "SurfaceROTemperature"
#define	MDVarWTempSurfRunoffPool			 "SurfaceROPoolTemperature"
// Routing Temperature
#define MDVarBgcLocalIn_QxT                  "QxT_LocalIn"
#define MDVarRemoval_QxT                     "QxT_Removal"
#define MDVarFlux_QxT                        "QxT_Flux"
#define MDVarStorage_QxT                     "QxT_Storage"
#define MDVarDeltaStorage_QxT                "QxT_DeltaStorage"

#define MDVarWetBulbTemp					 "WetBulbTemp"			// RJS 062012

#define MDVarWTemp_QxT                       "QxT_WaterTemp"
#define MDVarWTempDeltaT_QxT                 "QxT_deltaT"

#define MDVarWithdrawal_QxT					 "Withdrawal_QxT"	//081311

#define MDVarFluxMixing_QxT                  "QxT_FluxMixing"
#define MDVarStorageMixing_QxT               "QxT_StorageMixing"
#define MDVarDeltaStorageMixing_QxT          "QxT_DeltaStorageMixing"
#define MDVarWTempMixing_QxT                 "QxT_WaterTempMixing"

#define MDVarPAR2Bottom                      "PAR2Bottom"
#define MDVarPAR2Reach                       "PAR2Reach"

#define	MDVarDINConcentration                "DIN_Concentration"
#define	MDVarDINLocalIn                      "DIN_LocalIn"
#define	MDVarDINRemoval                      "DIN_Removal"
#define	MDVarDINDenit                        "DIN_Denit"
#define	MDVarDINAssim                        "DIN_Assim"
#define	MDVarDINRemin                        "DIN_Remin"
#define	MDVarDINBiomass                      "DIN_Biomass"
#define	MDVarDINDeltaBiomass                 "DIN_DeltaBiomass"
#define	MDVarDINFlux                         "DIN_Flux"
#define	MDVarDINStorage                      "DIN_Storage"
#define	MDVarDINDeltaStorage                 "DIN_DeltaStorage"
#define	MDVarDINConcentration_Mixing         "DIN_ConcentrationMixing"
#define	MDVarDINFlux_Mixing                  "DIN_FluxMixing"
#define	MDVarDINStorage_Mixing               "DIN_StorageMixing"
#define	MDVarDINDeltaStorage_Mixing          "DIN_DeltaStorageMixing"
#define MDVarDINDrying                       "DIN_Drying"
#define MDVarDINDrying_Mixing                "DIN_DryingMixing"
#define MDVarPointScenario					 "PointScenario"

#define	MDVarDOCConcentration                "DOC_Concentration"
#define	MDVarDOCLocalIn                      "DOC_LocalIn"
#define	MDVarDOCRemoval                      "DOC_Removal"
#define	MDVarDOCFlux                         "DOC_Flux"
#define	MDVarDOCStorage                      "DOC_Storage"
#define	MDVarDOCDeltaStorage                 "DOC_DeltaStorage"
#define	MDVarDOCConcentration_Mixing         "DOC_ConcentrationMixing"
#define	MDVarDOCFlux_Mixing                  "DOC_FluxMixing"
#define	MDVarDOCStorage_Mixing               "DOC_StorageMixing"
#define	MDVarDOCDeltaStorage_Mixing          "DOC_DeltaStorageMixing"

#define MDVarPoint_Fecal                     "FecalPointSources" // TODO Not used!
#define	MDVarFecalConcentration              "FecalConcentration"
#define	MDVarFecalLocalIn                    "FecalLocalIn"
#define	MDVarFecalRemoval                    "FecalRemoval"
#define	MDVarFecalFlux                       "FecalFlux"
#define	MDVarFecalStorage                    "FecalStorage"
#define	MDVarFecalDeltaStorage               "FecalDeltaStorage"
#define	MDVarFecalConcentration_Mixing       "FecalConcentrationMixing"
#define	MDVarFecalFlux_Mixing                "FecalFluxMixing"
#define	MDVarFecalStorage_Mixing             "FecalStorageMixing"
#define	MDVarFecalDeltaStorage_Mixing        "FecalDeltaStorageMixing"
#define MDParFecalK                          "FecalK"

#define MDParGrossRadTAU                     "GrossRadTAU"
#define MDParGroundWatBETA                   "GroundWaterBETA"
#define MDParSurfWaterBETA2					 "SurfWaterBETA2"			//RJS 042612
#define MDParSoilMoistALPHA                  "SoilMoistureALPHA"
#define MDParInfiltrationFrac                "InfiltrationFraction"
#define MDParSurfRunoffPoolBETA			 	 "SurfacePoolBETA"			//RJS 042012
#define MDVarOutSoilDebug					 "DebugOutput"
#define MDVarInitalSoilTemperature		     "InitialSoilTemperature"
#define MDVarLiquidSoilMoisture			     "LiquidSoilMoisture"
#define MDVarActiveLayerDepth                "ActiveLayerDepth"
#define MDVarThawingDepth                    "ThawingDepth"
#define MDVarSnowDepth                       "SnowDepth"
#define MDVarSnowDensity                     "SnowDensity"
#define	MDVarIsInitial                       "IsInitial"
#define MDVarSoilMoistureIsInitial           "SoilMoistureIsInitial"
#define MDVarWinterOnsetDoy                  "WinterOnsetDoY"
#define MDVarWaterTableDepth                 "WaterTableDepth"
#define MDVarSoilOrganicLayer                "SoilOrganicLayer"
#define MDParInfiltrationFracSpatial         "InfiltrationFractionSpatial"
#define MDParSnowMeltThreshold               "SnowMeltThreshold"	
#define	MDParFallThreshold				     "SnowFallThreshold"

#define MDParAlphaSTS						 "AlphaSTS"
#define MDParAlphaHTS						 "AlphaHTS"
#define MDParASTSA							 "ASTSA"
#define MDParAHTSA							 "AHTSA"
#define MDParUptakeKtMC						 "UptakeKtMC"
#define MDParUptakeKtSTS					 "UptakeKtSTS"
#define MDParUptakeKtHTS					 "UptakeKtHTS"

#define MDConst_m3PerSecTOm3PerDay    86400.0
#define MDConst_m3PerSecTOmmKm2PerDay 86400000.0 // TODO Not Used!
#define MDConst_mmKm2PerDayTOm3PerSec (1.0 / 86400000.0) // TODO Not Used!
#define MDConstInterceptCI  0.3	   // Interception LAI+SAI constant
#define MDConstInterceptCH 10.0    // Interception canopy height constant 
#define MDConstInterceptD   0.2    // Interception rain fraction of the day
#define MDConstLPC          4.0    // minimum projected LAI for "closed" canopy
#define MDConstC1           0.25   // intercept in actual/potential solar radiation to sunshine duration
#define MDConstC2           0.5    // slope in actual/potential solar radiation to sunshine duration
#define MDConstC3           0.2    // longwave cloud correction coefficient
#define MDConstPTALPHA      1.26   // Priestley-Taylor coefficient
#define MDConstCZS          0.13   // Ratio of roughness to height for smooth closed canopies
#define MDConstCZR          0.05   // ratio of roughness to height for rough closed canopies
#define MDConstHR          10.0    // height above which CZR applies, m
#define MDConstHS           1.0    // height below which CZS applies, m
#define MDConstZMINH        2.0    // reference height above the canopy height m
#define MDConstRSS        500.0    // soil surface resistance, s/m TODO Not Used!
#define MDConstWNDRAT       0.3    // ratio of nighttime to daytime wind speed
#define MDConstN            2.5    // wind/diffusivity extinction coefficient
#define MDConstCS           0.035  // ratio of projected stem area to height for closed canopy
#define MDConstGLMIN        0.0003 // cuticular leaf surface conductance for all sides of leaf, m/s
#define MDConstRM        1000.0    // maximum solar radiation, at which FR = 1,  W/m2
#define MDConstRHOTP        2.0    // ratio of total leaf surface area to projected leaf area
#define MDConstCPRHO     1240.0    // volumetric heat capacity of air, J/(K m3)
#define MDConstPSGAMMA      0.067  // psychrometric constant, kPa/K
#define MDConstIGRATE       0.0864 // integrates W/m2 over 1 d to MJ/m2
#define MDConstEtoM         0.4085 // converts MJ/m2 to mm of water
#define MDConstSIGMA      5.67E-08 // Stefan-Boltzmann constant, W/(m2/K4)
#define MDConstK            0.4    // von Karman constant
#define MDMinimum(a,b) (((a) < (b)) ? (a) : (b))
#define MDMaximum(a,b) (((a) > (b)) ? (a) : (b))

int MDAccumPrecipDef();
int MDAccumEvapDef();
int MDAccumSMoistChgDef();
int MDAccumGrdWatChgDef();
int MDAccumRunoffDef();
int MDAccumBalanceDef();
int MDAvgNStepsDef();
int MDBaseFlowDef();

int MDBgcRoutingDef();
int MDBgcDOCRoutingDef();
int MDBgcDINRoutingDef();
int MDBgcDINPlusBiomassRoutingDef();
int MDBgcRiverLightDef();
int MDBgcRiverGPPDef(); // TODO Not Used!
int MDBQARTinputsDef(); //NEW !!!!!  (S.C)
int MDBQARTpreprocessDef(); //NEW !!!!!  (S.C)
int MDBedloadFluxDef();//NEW !!!!!  (S.C)
int MDBankfullQcalcDef();//NEW !!!!!  (S.C)

int MDCParamAlbedoDef();
int MDCParamCHeightDef();
int MDCParamLWidthDef();
int MDCParamRSSDef();
int MDCParamR5Def();
int MDCParamCDDef();
int MDCParamCRDef();
int MDCParamGLMaxDef();
int MDCParamLPMaxDef();
int MDCParamZ0gDef();
int MDDINDef();					// RJS 042513
int MDNitrogenInputsDef();		// RJS 042513
int MDDischargeDef();
int MDDischargeBFDef();
int MDDischLevel1Def();
int MDDischLevel2Def();
int MDDischLevel3Def();
int MDDischLevel3AccumulateDef();
int MDDischLevel3CascadeDef();
int MDDischLevel3MuskingumDef();
int MDDischLevel3MuskingumCoeffDef();
int MDDischMeanDef();
int MDDO2Def();					// RJS 2012-06-18
int MDEvapotranspirationDef();
int MDGrossRadDef();
int MDIrrigatedAreaDef();
int MDIrrGrossDemandDef();
int MDIrrRefEvapotransDef();
int MDIrrRefEvapotransFAODef();
int MDIrrRefEvapotransHamonDef();
int MDIrrReturnFlowDef();
int MDIrrSoilMoistureDef();
int MDIrrSoilMoistChgDef();
int MDIrrUptakeRiverDef();
int MDIrrUptakeGrdWaterDef();
int MDReservoirDef();
int MDLandCoverDef();
int MDLeafAreaIndexDef();
int MDRainInfiltrationDef();
int MDRainInterceptDef();
int MDRainPotETDef();
int MDRainPotETHamonDef();
int MDRainPotETJensenDef();
int MDRainPotETPstdDef();
int MDRainPotETPsTaylorDef();
int MDRainPotETPMdayDef();
int MDRainPotETPMdnDef();
int MDRainPotETSWGdayDef();
int MDRainPotETSWGdnDef();
int MDRainPotETTurcDef();
int MDRainSMoistChgDef();
int MDRainSurfRunoffDef();
int MDRainWaterSurplusDef();
int MDWetlandRunoffDef();
int MDRelHumidityDef(); // TODO Not Used!
int MDRiverbedShapeExponentDef();
int MDRiverWidthDef();

int MDRunoffDef();
int MDRunoffMeanDef();
int MDRunoffVolumeDef();
int MDSedimentFluxDef(); //NEW !!!!!  (S.C)
int MDSPackChgDef();
int MDSPackMeltDef();
int MDSmallReservoirReleaseDef();
int MDSmallReservoirCapacityDef();
int MDSolarRadDef();
int MDSRadDayLengthDef();
int MDSRadI0HDayDef();
int MDSoilAvailWaterCapDef();
int MDSoilMoistChgDef();
int MDSurfRunoffDef();
int MDSurfRunoffPoolDef();		// RJS 042012
int MDStemAreaIndexDef();
int MDStormRunoffDef();			// RJS 082812
int MDWaterBalanceDef();
int MDPrecipitationDef();
int MDWetDaysDef();
int MDWetlandAreaDef();
int MDPermafrostDef();
int MDWTempGrdWaterDef();
int MDWTempRiverDef();
int MDWTempRiverRouteDef();
int MDThermalInputsDef();
int MDThermalInputs2Def();		// RJS 062012
int MDThermalInputs3Def();		// RJS 112712
int MDEnergyDemandDef();		// RJS 062012

int MDWTempSurfRunoffDef();
int MDWTempSurfRunoffPoolDef();		//RJS 060512

int MDRainWaterSurplusDef();
int MDRainSMoistChgLayeredSoilDef();
/* PET & Related Functions */

float MDSRadNETLong(float, float, float, float);

float MDPETlibVPressSat(float);
float MDPETlibVPressDelta(float);

float MDWindAdjustment(float, float, float, float, float, float);

float MDPETlibLeafAreaIndex(float, float);
float MDPETlibSteamAreaIndex(float, float);
float MDPETlibRoughnessClosed(float, float);
float MDPETlibRoughness(float, float, float, float, float);
float MDPETlibZPDisplacement(float, float, float, float);
float MDPETlibCanopySurfResistance(float, float, float, float, float, float,
		float, float, float);
float MDPETlibBoundaryResistance(float, float, float, float, float, float,
		float);
float MDPETlibLeafResistance(float, float, float, float, float, float, float,
		float);
float MDPETlibGroundResistance(float, float, float, float, float, float, float);

float MDPETlibPenmanMontieth(float, float, float, float, float);
float MDPETlibShuttleworthWallace(float, float, float, float, float, float,
		float, float, float);
float hydroran2(long *idum);

#if defined(__cplusplus)
}
#endif

#endif /* MD_H_INCLUDED */
