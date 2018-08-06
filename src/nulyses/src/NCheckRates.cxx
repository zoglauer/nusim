/*
 * NCheckRates.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NCheckRates
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NCheckRates.h"

// Standard libs:
#include <fstream>
#include <limits>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <cstdlib>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TText.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TColor.h"
#include "TStyle.h"
#include "TPaletteAxis.h"

// MEGAlib libs:

// NuSTAR libs:
#include "MFile.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NCheckRates)
#endif


////////////////////////////////////////////////////////////////////////////////


NCheckRates::NCheckRates()
{
  // Construct an instance of NCheckRates
  
  m_WriteGTI = false;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;

  m_SpectrumMin = 3;
  m_SpectrumMax = 120;  

  
  // Better ROOT palette for this task:
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}


////////////////////////////////////////////////////////////////////////////////


NCheckRates::~NCheckRates()
{
  // Delete this instance of NCheckRates
}


////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  string Option;
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
       
    // Then fulfill the options:
    if (Option == "--write-gti") {
      m_WriteGTI = true;
      cout<<"Writing GTI"<<endl;
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::Analyze() 
{
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::Show(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Checking rates..."<<endl;
  
  // Section A: Create all histograms:
  double SAARegionMin = 250;
  double SAARegionMax = 350; 
  
  double SpectrumMin = 3;
  double SpectrumMax = 120;  
   
  int LongitudeBins = 180;
  int LatitudeBins = 90;
  double MinLatitude = -6.2;
  double MaxLatitude = +6.2;
    
  TString iID = "_CheckRates_id"; 
  iID += F.m_ID;
  iID += "_m";
  iID += ((F.m_Module == 0) ? "A" : "B");
  iID += "_cl";
  iID += F.m_CleanLevel;

  TString ID = " \n (ID: ";
  ID += F.m_ID;
  ID += ", Mod: ";
  ID += ((F.m_Module == 0) ? "A" : "B");
  ID += ")";
      
  
  int NPositionBins = 64;
  TH2D* PositionsOnSource = new TH2D(TString("PositionsOnSource") + iID, TString("Counts in used detector regions") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsOnSource->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsOnSource->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsOnSource->SetZTitle("cts");

  
  
  int RatesBins = (H.GetMaxTime() - H.GetMinTime()+1)/60;
  double StartTime = H.GetMinTime()-0.5;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;

  // Convert to ks since start of abservation
  MaxTime -= MinTime;
  MinTime = 0;
  MaxTime /= 1000.0;
  
  TH1D* Rates = new TH1D(TString("Rates") + iID, TString("Rates all") + ID, RatesBins, MinTime, MaxTime);
  Rates->SetXTitle("Time since start of observation [ks]");
  Rates->SetYTitle("cts/sec");

  TH1D* ShieldRatesHigh = new TH1D(TString("ShieldRatesHigh") + iID, TString("ShieldRatesHigh") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesHigh->SetXTitle("Time since start of observation [ks]");
  ShieldRatesHigh->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerShieldRatesHighByOrbit = new TH2D(TString("OrbitNormalizerShieldRatesHighByOrbit") + iID, 
                                                         TString("OrbitNormalizerShieldRatesHighByOrbit") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* ShieldRatesHighByOrbit = new TH2D(TString("ShieldRatesHighByOrbit") + iID, 
                                          TString("Shield rates high by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  ShieldRatesHighByOrbit->SetXTitle("Longitude [deg]");
  ShieldRatesHighByOrbit->SetYTitle("Latitude [deg]");
  ShieldRatesHighByOrbit->SetZTitle("cts/sec");  
  
  
  TH1D* ShieldRatesLow = new TH1D(TString("ShieldRatesLow") + iID, TString("ShieldRatesLow") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesLow->SetXTitle("Time since start of observation [ks]");
  ShieldRatesLow->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerShieldRatesLowByOrbit = new TH2D(TString("OrbitNormalizerShieldRatesLowByOrbit") + iID, 
                                                        TString("OrbitNormalizerShieldRatesLowByOrbit") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* ShieldRatesLowByOrbit = new TH2D(TString("ShieldRatesLowByOrbit") + iID, 
                                          TString("Shield rates low by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  ShieldRatesLowByOrbit->SetXTitle("Longitude [deg]");
  ShieldRatesLowByOrbit->SetYTitle("Latitude [deg]");
  ShieldRatesLowByOrbit->SetZTitle("cts/sec");  

  TH1D* ShieldRatesEntries = new TH1D(TString("ShieldRatesEntries") + iID, TString("ShieldRatesEntries") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesEntries->SetXTitle("Time since start of observation [ks]");
  ShieldRatesEntries->SetYTitle("cts");

  TH1D* SAARegion = new TH1D(TString("SAARegion") + iID, TString("SAARegion") + ID, RatesBins, MinTime, MaxTime);
  SAARegion->SetXTitle("Time since start of observation [ks]");
  SAARegion->SetYTitle("cts/sec");
  SAARegion->SetLineColor(kYellow);
  SAARegion->SetFillColor(kYellow);
  
  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimes->SetXTitle("Time since start of observation [ks]");
  LifeTimes->SetYTitle("cts/sec");

  
  
  // Base: 1A-1, 1B-1, 2A-1, 2B-1, 3A-1, 3B-1
  TH1D* RatesSAANoTentacleNo = new TH1D(TString("RatesSAANoTentacleNo") + iID, TString("Rates - SAA: No, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAANoTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAANoTentacleNo = new TH1D(TString("LifeTimesSAANoTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAANoTentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAANoTentacleNo->SetYTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAANoTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleNoByOrbit = new TH2D(TString("RatesSAANoTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAANoTentacleNoByOrbit->SetZTitle("cts/sec");

  double TimeSAANoTentacleNo = 0.0; 

  
  // 1A-2
  TH1D* RatesSAAStrictHSRTentacleNo = new TH1D(TString("RatesSAAStrictHSRTentacleNo") + iID, TString("Rates - SAA: StrictHSR, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictHSRTentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAAStrictHSRTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictHSRTentacleNo = new TH1D(TString("LifeTimesSAAStrictHSRTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictHSRTentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAStrictHSRTentacleNo->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAStrictHSRTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictHSRTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictHSRTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictHSRTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictHSRTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: StrictHSR, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictHSRTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictHSRTentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAStrictHSRTentacleNoByOrbit->SetZTitle("cts/sec");
  
  double TimeSAAStrictHSRTentacleNo = 0.0; 


  
  // 1A-3
  TH1D* RatesSAAStrictHSRTentacleFoM = new TH1D(TString("RatesSAAStrictHSRTentacleFoM") + iID, TString("Rates - SAA: StrictHSR, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictHSRTentacleFoM->SetXTitle("Time since start of observation [ks]");
  RatesSAAStrictHSRTentacleFoM->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictHSRTentacleFoM = new TH1D(TString("LifeTimesSAAStrictHSRTentacleFoM") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictHSRTentacleFoM->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAStrictHSRTentacleFoM->SetYTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictHSRTentacleFoM = new TH2D(TString("OrbitNormalizerDetectorSAAStrictHSRTentacleFoM") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictHSRTentacleFoM") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictHSRTentacleFoMByOrbit = new TH2D(TString("RatesSAAStrictHSRTentacleFoMByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: StrictHSR, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictHSRTentacleFoMByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictHSRTentacleFoMByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAStrictHSRTentacleFoMByOrbit->SetZTitle("cts/sec");

  double TimeSAAStrictHSRTentacleFoM = 0.0; 


  
  // 1B-2
  TH1D* RatesSAAOptimizedHSRFoMTentacleNo = new TH1D(TString("RatesSAAOptimizedHSRFoMTentacleNo") + iID, TString("Rates - SAA: Optimized, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedHSRFoMTentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedHSRFoMTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedHSRFoMTentacleNo = new TH1D(TString("LifeTimesSAAOptimizedHSRFoMTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedHSRFoMTentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedHSRFoMTentacleNo->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedHSRFoMTentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedHSRFoMTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetZTitle("cts/sec");  
  
  double TimeSAAOptimizedHSRFoMTentacleNo = 0.0; 


  
  // 1B-3
  TH1D* RatesSAAOptimizedHSRFoMTentacleFoM = new TH1D(TString("RatesSAAOptimizedHSRFoMTentacleFoM") + iID, TString("Rates - SAA: Optimized, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedHSRFoMTentacleFoM->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedHSRFoMTentacleFoM->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedHSRFoMTentacleFoM = new TH1D(TString("LifeTimesSAAOptimizedHSRFoMTentacleFoM") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedHSRFoMTentacleFoM->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedHSRFoMTentacleFoM->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedHSRFoMTentacleFoMByOrbit = new TH2D(TString("RatesSAAOptimizedHSRFoMTentacleFoMByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetZTitle("cts/sec");

  double TimeSAAOptimizedHSRFoMTentacleFoM = 0.0; 
  
  
  
  // 2A-2 = 3A-2
  TH1D* RatesSAAStrictLSRTentacleNo = new TH1D(TString("RatesSAAStrictLSRTentacleNo") + iID, TString("Rates - SAA: Super strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictLSRTentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAAStrictLSRTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictLSRTentacleNo = new TH1D(TString("LifeTimesSAAStrictLSRTentacleNo") + iID, TString("LifeTimesSAAStrictLSRTentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictLSRTentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAStrictLSRTentacleNo->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAStrictLSRTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictLSRTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictLSRTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictLSRTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictLSRTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Super strict, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictLSRTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictLSRTentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAStrictLSRTentacleNoByOrbit->SetZTitle("cts/sec");
  
  double TimeSAAStrictLSRTentacleNo = 0.0; 

  
  
  // 2A-3
  TH1D* RatesSAAStrictLSRTentacleRMS1 = new TH1D(TString("RatesSAAStrictLSRTentacleRMS1") + iID, TString("Rates - SAA: Super strict, Tentacle: RMS v1") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictLSRTentacleRMS1->SetXTitle("Time since start of observation [ks]");
  RatesSAAStrictLSRTentacleRMS1->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictLSRTentacleRMS1 = new TH1D(TString("LifeTimesSAAStrictLSRTentacleRMS1") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictLSRTentacleRMS1->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAStrictLSRTentacleRMS1->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAStrictLSRTentacleRMS1 = new TH2D(TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS1") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS1") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictLSRTentacleRMS1ByOrbit = new TH2D(TString("RatesSAAStrictLSRTentacleRMS1ByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Super strict, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictLSRTentacleRMS1ByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictLSRTentacleRMS1ByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAStrictLSRTentacleRMS1ByOrbit->SetZTitle("cts/sec");

  double TimeSAAStrictLSRTentacleRMS1 = 0.0; 
    


  // 2B-2
  TH1D* RatesSAAOptimizedLSRRMS1TentacleNo = new TH1D(TString("RatesSAAOptimizedLSRRMS1TentacleNo") + iID, TString("Rates - SAA: Optimized super strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMS1TentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedLSRRMS1TentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMS1TentacleNo = new TH1D(TString("LifeTimesSAAOptimizedLSRRMS1TentacleNo") + iID, TString("LifeTimesSAAOptimizedLSRRMS1TentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMS1TentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedLSRRMS1TentacleNo->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMS1TentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMS1TentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized RMS v1, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMS1TentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMS1TentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedLSRRMS1TentacleNoByOrbit->SetZTitle("cts/sec");

  double TimeSAAOptimizedLSRRMS1TentacleNo = 0.0; 

  
  
  // 2B-3
  TH1D* RatesSAAOptimizedLSRRMS1TentacleRMS1 = new TH1D(TString("RatesSAAOptimizedLSRRMS1TentacleRMS1") + iID, TString("Rates - SAA: Optimized RMS v1, Tentacle: RMS v1") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMS1TentacleRMS1->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedLSRRMS1TentacleRMS1->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMS1TentacleRMS1 = new TH1D(TString("LifeTimesSAAOptimizedLSRRMS1TentacleRMS1") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->SetYTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleRMS1 = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleRMS1") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleRMS1") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized RMS v1, Tentacle: RMS v1") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit->SetZTitle("cts/sec");
  
  double TimeSAAOptimizedLSRRMS1TentacleRMS1 = 0.0; 

  
  // 3A-2 == 2A-2
  
  // 3A-3
  TH1D* RatesSAAStrictLSRTentacleRMS2 = new TH1D(TString("RatesSAAStrictLSRTentacleRMS2") + iID, TString("Rates - SAA: Super strict, Tentacle: RMS v1") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictLSRTentacleRMS2->SetXTitle("Time since start of observation [ks]");
  RatesSAAStrictLSRTentacleRMS2->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictLSRTentacleRMS2 = new TH1D(TString("LifeTimesSAAStrictLSRTentacleRMS2") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictLSRTentacleRMS2->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAStrictLSRTentacleRMS2->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAStrictLSRTentacleRMS2 = new TH2D(TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS2") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS2") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictLSRTentacleRMS2ByOrbit = new TH2D(TString("RatesSAAStrictLSRTentacleRMS2ByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Super strict, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictLSRTentacleRMS2ByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictLSRTentacleRMS2ByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAStrictLSRTentacleRMS2ByOrbit->SetZTitle("cts/sec");  

  double TimeSAAStrictLSRTentacleRMS2 = 0.0; 

  
  
  // 3B-2
  TH1D* RatesSAAOptimizedLSRRMS2TentacleNo = new TH1D(TString("RatesSAAOptimizedLSRRMS2TentacleNo") + iID, TString("Rates - SAA: Optimized super strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMS2TentacleNo->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedLSRRMS2TentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMS2TentacleNo = new TH1D(TString("LifeTimesSAAOptimizedLSRRMS2TentacleNo") + iID, TString("LifeTimesSAAOptimizedLSRRMS2TentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMS2TentacleNo->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedLSRRMS2TentacleNo->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMS2TentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMS2TentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized RMS v2, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMS2TentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMS2TentacleNoByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedLSRRMS2TentacleNoByOrbit->SetZTitle("cts/sec");

  double TimeSAAOptimizedLSRRMS2TentacleNo = 0.0; 

  
  
  // 3B-3
  TH1D* RatesSAAOptimizedLSRRMS2TentacleRMS2 = new TH1D(TString("RatesSAAOptimizedLSRRMS2TentacleRMS2") + iID, TString("Rates - SAA: Optimized RMS v2, Tentacle: RMS v2") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMS2TentacleRMS2->SetXTitle("Time since start of observation [ks]");
  RatesSAAOptimizedLSRRMS2TentacleRMS2->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMS2TentacleRMS2 = new TH1D(TString("LifeTimesSAAOptimizedLSRRMS2TentacleRMS2") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->SetXTitle("Time since start of observation [ks]");
  LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->SetYTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleRMS2 = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleRMS2") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleRMS2") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized RMS v2, Tentacle: RMS v2") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit->SetYTitle("Latitude [deg]");
  RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit->SetZTitle("cts/sec");  

  double TimeSAAOptimizedLSRRMS2TentacleRMS2 = 0.0;   



  
  // Create a list of pixels with high source count for elimination
  vector<int> ExcludedDetRawXY;
  if (m_TentacleCutRMSSourceElimination == true || m_SAACutRMSSourceElimination == true) { 
    ExcludedDetRawXY = MostlyEliminateSource(F, O, false);
  }
  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {
    
    //if (WithinSpecialGTI((F.m_Time[e] - StartTime)/1000) == false) continue;
    //if (WithinSpecialBTI((F.m_Time[e] - StartTime)/1000) == true) continue;
    if (F.m_Energy[e] < SpectrumMin || F.m_Energy[e] > SpectrumMax) continue;
    
    if (F.IsGTI(F.m_Time[e]) == false) continue;
    
    int HKIndex = H.FindClosestIndex(F.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found for time "<<F.m_Time[e]<<"..."<<endl;
      continue;      
    }
    double LifeTime = H.m_LiveTime[HKIndex];
    double Count = 1.0/LifeTime;
    
    int OrbitIndex = O.FindClosestIndex(F.m_Time[e]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<F.m_Time[e]<<"..."<<endl;
      continue;      
    }
    
    
    int DetectorID = F.m_DetectorID[e];
    int RawX = F.m_RawX[e];
    int RawY = F.m_RawY[e];
    int ID = 10000*DetectorID + 100*RawX + RawY;
    vector<int>::iterator I = lower_bound(ExcludedDetRawXY.begin(), ExcludedDetRawXY.end(), 10000*DetectorID + 100*RawX + RawY);
    if (I != ExcludedDetRawXY.end() && (*I) == ID) continue;
    
    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);
    
    if (RawX >= 0 && RawX <= 31 && RawY >= 0 && RawY <= 31) {
      PositionsOnSource->Fill(PosX, PosY);
    }
    
    // Base
    if (H.m_HardSAA[HKIndex] == false) {
      RatesSAANoTentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAANoTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);
    }
    // 1A-2
    if (H.m_SoftSAAStrictHSR[HKIndex] == false) {
      RatesSAAStrictHSRTentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAStrictHSRTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 1A-3
    if (H.m_SoftSAAStrictHSR[HKIndex] == false && H.m_SoftTentacledFoM[HKIndex] == false) {
      RatesSAAStrictHSRTentacleFoM->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAStrictHSRTentacleFoMByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 1B-2
    if (H.m_SoftSAAOptimizedHSRFoM[HKIndex] == false) {
      RatesSAAOptimizedHSRFoMTentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedHSRFoMTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 1B-3
    if (H.m_SoftSAAOptimizedHSRFoM[HKIndex] == false && H.m_SoftTentacledFoM[HKIndex] == false) {
      RatesSAAOptimizedHSRFoMTentacleFoM->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 2A-2
    if (H.m_SoftSAAStrictLSR[HKIndex] == false) {
      RatesSAAStrictLSRTentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAStrictLSRTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 2A-3
    if (H.m_SoftSAAStrictLSR[HKIndex] == false && H.m_SoftTentacledRMS_v1[HKIndex] == false) {
      RatesSAAStrictLSRTentacleRMS1->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAStrictLSRTentacleRMS1ByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 2B-2
    if (H.m_SoftSAAOptimizedLSRRMS_v1[HKIndex] == false) {
      RatesSAAOptimizedLSRRMS1TentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedLSRRMS1TentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 2B-3
    if (H.m_SoftSAAOptimizedLSRRMS_v1[HKIndex] == false && H.m_SoftTentacledRMS_v1[HKIndex] == false) {
      RatesSAAOptimizedLSRRMS1TentacleRMS1->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 3A-3
    if (H.m_SoftSAAStrictLSR[HKIndex] == false && H.m_SoftTentacledRMS_v2[HKIndex] == false) {
      RatesSAAStrictLSRTentacleRMS2->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAStrictLSRTentacleRMS2ByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 3A-2
    if (H.m_SoftSAAOptimizedLSRRMS_v2[HKIndex] == false) {
      RatesSAAOptimizedLSRRMS2TentacleNo->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedLSRRMS2TentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    // 3A-3
    if (H.m_SoftSAAOptimizedLSRRMS_v2[HKIndex] == false && H.m_SoftTentacledRMS_v2[HKIndex] == false) {
      RatesSAAOptimizedLSRRMS2TentacleRMS2->Fill((F.m_Time[e] - StartTime)/1000, Count);
      RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Count);    
    }
    
  }

  
  double TotalTime = 0;
  LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    double Life = 1.0; //H.m_LiveTime[h];

    LifeTimes->Fill((H.m_Time[h] - StartTime)/1000, Life);
    
    int OrbitIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<H.m_Time[h]<<"..."<<endl;
      continue;      
    }

    if (O.m_Longitude[OrbitIndex] > SAARegionMin && O.m_Longitude[OrbitIndex] < SAARegionMax) {
      SAARegion->SetBinContent(SAARegion->FindBin((H.m_Time[h] - StartTime)/1000), 1); 
    }

    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    if (F.IsGTI(H.m_Time[h]) == false) continue;

    ShieldRatesHigh->Fill((H.m_Time[h] - StartTime)/1000, H.m_ShieldRateHigh[h]);  
    OrbitNormalizerShieldRatesHighByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
    ShieldRatesHighByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_ShieldRateHigh[h]);  
    ShieldRatesLow->Fill((H.m_Time[h] - StartTime)/1000, H.m_ShieldRateLow[h]);  
    OrbitNormalizerShieldRatesLowByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
    ShieldRatesLowByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_ShieldRateLow[h]);  
    ShieldRatesEntries->Fill((H.m_Time[h] - StartTime)/1000, 1);  
 
    //if (H.m_LiveTime[h] == 0) continue;
    
    // Base
    if (H.m_HardSAA[h] == false) {
      OrbitNormalizerDetectorSAANoTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
      LifeTimesSAANoTentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAANoTentacleNo += 1;
    }
    // 1A-2      
    if (H.m_SoftSAAStrictHSR[h] == false) {
      OrbitNormalizerDetectorSAAStrictHSRTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictHSRTentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAStrictHSRTentacleNo += 1;
    }
    // 1A-3
    if (H.m_SoftSAAStrictHSR[h] == false && H.m_SoftTentacledFoM[h] == false) {
      OrbitNormalizerDetectorSAAStrictHSRTentacleFoM->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictHSRTentacleFoM->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAStrictHSRTentacleFoM += 1;
    }
    // 1B-2
    if (H.m_SoftSAAOptimizedHSRFoM[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedHSRFoMTentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedHSRFoMTentacleNo += 1;
    }
    // 1B-3
    if (H.m_SoftSAAOptimizedHSRFoM[h] == false && H.m_SoftTentacledFoM[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedHSRFoMTentacleFoM->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedHSRFoMTentacleFoM += 1;
    }
    // 2A-2
    if (H.m_SoftSAAStrictLSR[h] == false) {
      OrbitNormalizerDetectorSAAStrictLSRTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictLSRTentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAStrictLSRTentacleNo += 1;
    }
    // 2A-3
    if (H.m_SoftSAAStrictLSR[h] == false && H.m_SoftTentacledRMS_v1[h] == false) {
      OrbitNormalizerDetectorSAAStrictLSRTentacleRMS1->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictLSRTentacleRMS1->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAStrictLSRTentacleRMS1 += 1;
    }
    // 2B-2
    if (H.m_SoftSAAOptimizedLSRRMS_v1[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMS1TentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedLSRRMS1TentacleNo += 1;
    }
    // 2B-3
    if (H.m_SoftSAAOptimizedLSRRMS_v1[h] == false && H.m_SoftTentacledRMS_v1[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleRMS1->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedLSRRMS1TentacleRMS1 += 1;
    }
    // 3A-3
    if (H.m_SoftSAAStrictLSR[h] == false && H.m_SoftTentacledRMS_v2[h] == false) {
      OrbitNormalizerDetectorSAAStrictLSRTentacleRMS2->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictLSRTentacleRMS2->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAStrictLSRTentacleRMS2 += 1;
    }
    // 3B-2
    if (H.m_SoftSAAOptimizedLSRRMS_v2[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMS2TentacleNo->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedLSRRMS2TentacleNo += 1;
    }
    // 3B-3
    if (H.m_SoftSAAOptimizedLSRRMS_v2[h] == false && H.m_SoftTentacledRMS_v2[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleRMS2->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->Fill((H.m_Time[h] - StartTime)/1000, Life);
      TimeSAAOptimizedLSRRMS2TentacleRMS2 += 1;
    }
   
    
        
    if (O.m_SafelyOnSource[OrbitIndex] == true) {
      TotalTime += 1.0;
      LiveTime += H.m_LiveTime[h];
    }
  }
    
  
  // Normalize
  
  // Substep 1: Calculate the median time we spend in a bin with content:

    
  // Normalize & set the minimum not to zero, but to the smallest no-zero entry for nicer plots:  
  int ShiftedMedian = 10;

  NormalizeOrbit(RatesSAANoTentacleNoByOrbit, OrbitNormalizerDetectorSAANoTentacleNo, ShiftedMedian);
  
  NormalizeOrbit(RatesSAAStrictHSRTentacleNoByOrbit, OrbitNormalizerDetectorSAAStrictHSRTentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAStrictHSRTentacleFoMByOrbit, OrbitNormalizerDetectorSAAStrictHSRTentacleFoM, ShiftedMedian);
  
  NormalizeOrbit(RatesSAAOptimizedHSRFoMTentacleNoByOrbit, OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedHSRFoMTentacleFoMByOrbit, OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM, ShiftedMedian);
  
  NormalizeOrbit(RatesSAAStrictLSRTentacleNoByOrbit, OrbitNormalizerDetectorSAAStrictLSRTentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAStrictLSRTentacleRMS1ByOrbit, OrbitNormalizerDetectorSAAStrictLSRTentacleRMS1, ShiftedMedian);
  
  NormalizeOrbit(RatesSAAOptimizedLSRRMS1TentacleNoByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMS1TentacleRMS1, ShiftedMedian);
  
  NormalizeOrbit(RatesSAAStrictLSRTentacleRMS2ByOrbit, OrbitNormalizerDetectorSAAStrictLSRTentacleRMS2, ShiftedMedian);

  NormalizeOrbit(RatesSAAOptimizedLSRRMS2TentacleNoByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMS2TentacleRMS2, ShiftedMedian);
  
  NormalizeOrbit(ShieldRatesHighByOrbit, OrbitNormalizerShieldRatesHighByOrbit, ShiftedMedian);
  NormalizeOrbit(ShieldRatesLowByOrbit, OrbitNormalizerShieldRatesLowByOrbit, ShiftedMedian);

 
  // (2) Normalize
  double Max = 0;
  double CutOffLifeTimeFraction = 0.5; // In some bins we will only be partially on - skip those where we are below CutOffLifeTimeFraction on
  
  // Base
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAANoTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAANoTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAANoTentacleNo->SetBinContent(b, 0); 
    } else {
      RatesSAANoTentacleNo->SetBinContent(b, RatesSAANoTentacleNo->GetBinContent(b)/RatesSAANoTentacleNo->GetBinWidth(b)/1000.0); ///LifeTimesSAANoTentacleNo->GetBinContent(b));
    }
    if (Max < RatesSAANoTentacleNo->GetBinContent(b)) {
      Max = RatesSAANoTentacleNo->GetBinContent(b);
    }
  }
  SAARegion->Scale(Max);
  
  // 1A-2
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictHSRTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAStrictHSRTentacleNo->SetBinContent(b, RatesSAAStrictHSRTentacleNo->GetBinContent(b)/RatesSAAStrictHSRTentacleNo->GetBinWidth(b)/1000.0); //LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b));
    }
  }
  // 1A-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictHSRTentacleFoM->SetBinContent(b, 0);
    } else {
      RatesSAAStrictHSRTentacleFoM->SetBinContent(b, RatesSAAStrictHSRTentacleFoM->GetBinContent(b)/RatesSAAStrictHSRTentacleFoM->GetBinWidth(b)/1000.0); //LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b));
    }
  } 
  
  // 1B-2
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedHSRFoMTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedHSRFoMTentacleNo->SetBinContent(b, RatesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b)/RatesSAAOptimizedHSRFoMTentacleNo->GetBinWidth(b)/1000.0); ///LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b));
    }
  }
  // 1B-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedHSRFoMTentacleFoM->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedHSRFoMTentacleFoM->SetBinContent(b, RatesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b)/RatesSAAOptimizedHSRFoMTentacleFoM->GetBinWidth(b)/1000.0); //LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b));
    }
  }
  
  // 2A-2
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictLSRTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAStrictLSRTentacleNo->SetBinContent(b, RatesSAAStrictLSRTentacleNo->GetBinContent(b)/RatesSAAStrictLSRTentacleNo->GetBinWidth(b)/1000.0); //LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b));
    }
  }
  // 2A-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictLSRTentacleRMS1->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictLSRTentacleRMS1->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictLSRTentacleRMS1->SetBinContent(b, 0);
    } else {
      RatesSAAStrictLSRTentacleRMS1->SetBinContent(b, RatesSAAStrictLSRTentacleRMS1->GetBinContent(b)/RatesSAAStrictLSRTentacleRMS1->GetBinWidth(b)/1000.0); //LifeTimesSAAStrictLSRTentacleRMS1->GetBinContent(b));
    }
  }
  
  // 2B-2
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMS1TentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMS1TentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMS1TentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMS1TentacleNo->SetBinContent(b, RatesSAAOptimizedLSRRMS1TentacleNo->GetBinContent(b)/RatesSAAOptimizedLSRRMS1TentacleNo->GetBinWidth(b)/1000.0); //LifeTimesSAAOptimizedLSRRMS1TentacleNo->GetBinContent(b));
    }
  }
  // 2B-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMS1TentacleRMS1->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMS1TentacleRMS1->SetBinContent(b, RatesSAAOptimizedLSRRMS1TentacleRMS1->GetBinContent(b)/RatesSAAOptimizedLSRRMS1TentacleRMS1->GetBinWidth(b)/1000.0); //LifeTimesSAAOptimizedLSRRMS1TentacleRMS1->GetBinContent(b));
    }
  }
  
  // 3A-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictLSRTentacleRMS2->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictLSRTentacleRMS2->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictLSRTentacleRMS2->SetBinContent(b, 0);
    } else {
      RatesSAAStrictLSRTentacleRMS2->SetBinContent(b, RatesSAAStrictLSRTentacleRMS2->GetBinContent(b)/RatesSAAStrictLSRTentacleRMS2->GetBinWidth(b)/1000.0); //LifeTimesSAAStrictLSRTentacleRMS2->GetBinContent(b));
    }
  }
  
  // 3B-2
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMS2TentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMS2TentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMS2TentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMS2TentacleNo->SetBinContent(b, RatesSAAOptimizedLSRRMS2TentacleNo->GetBinContent(b)/RatesSAAOptimizedLSRRMS2TentacleNo->GetBinWidth(b)/1000.0); //LifeTimesSAAOptimizedLSRRMS2TentacleNo->GetBinContent(b));
    }
  }
  // 3B-3
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMS2TentacleRMS2->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMS2TentacleRMS2->SetBinContent(b, RatesSAAOptimizedLSRRMS2TentacleRMS2->GetBinContent(b)/RatesSAAOptimizedLSRRMS2TentacleRMS2->GetBinWidth(b)/1000.0); //LifeTimesSAAOptimizedLSRRMS2TentacleRMS2->GetBinContent(b));
    }
  }
  
  
  
  for (int b = 1; b <= ShieldRatesEntries->GetNbinsX(); ++b) {
    if (ShieldRatesEntries->GetBinContent(b) == 0) continue;
    ShieldRatesHigh->SetBinContent(b, ShieldRatesHigh->GetBinContent(b)/ShieldRatesEntries->GetBinContent(b)/1000.0);
    ShieldRatesLow->SetBinContent(b, ShieldRatesLow->GetBinContent(b)/ShieldRatesEntries->GetBinContent(b)/1000.0);
  }
  
  
  // Show histograms:
    
  TCanvas* ShieldRatesHighCanvas = new TCanvas();
  ShieldRatesHighCanvas->cd();
  ShieldRatesHigh->Draw("HIST");
  ShieldRatesHighCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ShieldRatesHighCanvas->SaveAs(ShieldRatesHigh->GetName() + m_FileType);
  ShowRates(ShieldRatesHigh, SAARegion, 1);
  ShowRates(ShieldRatesHighByOrbit, 1);
      
  TCanvas* ShieldRatesLowCanvas = new TCanvas();
  ShieldRatesLowCanvas->cd();
  ShieldRatesLow->Draw("HIST");
  ShieldRatesLowCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ShieldRatesLowCanvas->SaveAs(ShieldRatesLow->GetName() + m_FileType);
  ShowRates(ShieldRatesLow, SAARegion, 1);
  ShowRates(ShieldRatesLowByOrbit, 1);

  //TCanvas* PositionsOnSourceCanvas = new TCanvas(TString("PositionsOnSourceCanvas") + iID, TString("PositionsOnSourceCanvas") + ID, 600, 600);
  //PositionsOnSourceCanvas->cd();
  //PositionsOnSource->Draw("colz");
  //ositionsOnSourceCanvas->Update();
  //if (m_ShowHistograms.Contains("f")) PositionsOnSourceCanvas->SaveAs(PositionsOnSource->GetName() + m_FileType);

  // 1A
  CreateReport(F.m_ID, 1, 1, 
               "Filter options: SAA algorithm 1 (high-shield rate based using a figure-of-merit as evaluation metric), strict SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAStrictHSRTentacleNo, RatesSAAStrictHSRTentacleNoByOrbit, TimeSAAStrictHSRTentacleNo,
               RatesSAAStrictHSRTentacleFoM, RatesSAAStrictHSRTentacleFoMByOrbit, TimeSAAStrictHSRTentacleFoM,
               SAARegion);
  // 1B
  CreateReport(F.m_ID, 1, 2,
               "Filter options: SAA algorithm 1 (high-shield rate based using a figure-of-merit as evaluation metric), optimized SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAOptimizedHSRFoMTentacleNo, RatesSAAOptimizedHSRFoMTentacleNoByOrbit, TimeSAAOptimizedHSRFoMTentacleNo,
               RatesSAAOptimizedHSRFoMTentacleFoM, RatesSAAOptimizedHSRFoMTentacleFoMByOrbit, TimeSAAOptimizedHSRFoMTentacleFoM,
               SAARegion);
  
  // 2A
  CreateReport(F.m_ID, 2, 1, 
               "Filter options: SAA algorithm 2 (low-shield rate based using rms as evaluation metric), strict SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAStrictLSRTentacleNo, RatesSAAStrictLSRTentacleNoByOrbit, TimeSAAStrictLSRTentacleNo,
               RatesSAAStrictLSRTentacleRMS1, RatesSAAStrictLSRTentacleRMS1ByOrbit, TimeSAAStrictLSRTentacleRMS1,
               SAARegion);
  // 2B
  CreateReport(F.m_ID, 2, 2, 
               "Filter options: SAA algorithm 2 (low-shield rate based using rms as evaluation metric), optimized SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAOptimizedLSRRMS1TentacleNo, RatesSAAOptimizedLSRRMS1TentacleNoByOrbit, TimeSAAOptimizedLSRRMS1TentacleNo,
               RatesSAAOptimizedLSRRMS1TentacleRMS1, RatesSAAOptimizedLSRRMS1TentacleRMS1ByOrbit, TimeSAAOptimizedLSRRMS1TentacleRMS1,
               SAARegion);
  
  // 3A
  CreateReport(F.m_ID, 3, 1, 
               "Filter options: SAA algorithm 3 (low-shield rate based using rms as evaluation metric), strict SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAStrictLSRTentacleNo, RatesSAAStrictLSRTentacleNoByOrbit, TimeSAAStrictLSRTentacleNo,
               RatesSAAStrictLSRTentacleRMS2, RatesSAAStrictLSRTentacleRMS2ByOrbit, TimeSAAStrictLSRTentacleRMS2,
               SAARegion);
  
  // 3B
  CreateReport(F.m_ID, 3, 2, 
               "Filter options: SAA algorithm 3 (low-shield rate based using rms as evaluation metric), optimized SAA cut",
               ((F.m_Module == 0) ? "A" : "B"),
               F.m_StartTime, F.m_ObservationDate, F.m_Object,
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAOptimizedLSRRMS2TentacleNo, RatesSAAOptimizedLSRRMS2TentacleNoByOrbit, TimeSAAOptimizedLSRRMS2TentacleNo,
               RatesSAAOptimizedLSRRMS2TentacleRMS2, RatesSAAOptimizedLSRRMS2TentacleRMS2ByOrbit, TimeSAAOptimizedLSRRMS2TentacleRMS2,
               SAARegion);
  
  CreateGTIs(H);
  
  // And finally write all settings
  ofstream settings;
  
  TString Report = "Settings_id";
  Report += F.m_ID;
  Report += "_m";
  Report += ((F.m_Module == 0) ? "A" : "B");
  Report += ".txt";
  Report.ReplaceAll(" ", "");
  Report.ReplaceAll(")", "");
  Report.ReplaceAll("(", "");
  Report.ReplaceAll(":", "");
  Report.ReplaceAll("-", "");
  Report.ReplaceAll(",", "");
  
  settings.open(Report);
  
  settings<<"nulyses checkrates settings:"<<endl;
  settings<<endl;
  settings<<"Spectral range:                      "<<m_SpectrumMin<<" - "<<m_SpectrumMax<<endl;
  settings<<"SAA cut RMS threshold:               "<<m_SAACutRMSThreshold<<endl;
  settings<<"SAAC cut RMS source elimination:     "<<m_SAACutRMSSourceElimination<<endl;
  settings<<"SAA Cut RMS sanity checks:           "<<m_SAACutRMSSanityChecks<<endl;
  settings<<"Tentacle cut RMS threshold:          "<<m_TentacleCutRMSThreshold<<endl;
  settings<<"Tentacle cut RMS source elimination: "<<m_TentacleCutRMSSourceElimination<<endl;
  settings<<"Tentacle cut RMS region restriction: "<<m_TentacleCutRMSRegionRestriction<<endl;
  settings<<"Tentacle cut RMS sanity checks:      "<<m_TentacleCutRMSSanityChecks<<endl;
  settings<<"Source cut RMS threshold:            "<<m_SourceCutRMSThreshold<<endl;
  
  settings.close();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::CreateGTIs(NHousekeeping& H) 
{
  bool On = true;
  bool Off = false;
  
  bool GTIStatus_SAAOptimizedLSRRMS1TentacleNo = Off;
  vector<double> GTIStart_SAAOptimizedLSRRMS1TentacleNo;
  vector<double> GTIStop_SAAOptimizedLSRRMS1TentacleNo;
  
  bool GTIStatus_SAAOptimizedLSRRMS1TentacleRMS1 = Off;
  vector<double> GTIStart_SAAOptimizedLSRRMS1TentacleRMS1;
  vector<double> GTIStop_SAAOptimizedLSRRMS1TentacleRMS1;
  
  bool GTIStatus_SAAOptimizedLSRRMS2TentacleNo = Off;
  vector<double> GTIStart_SAAOptimizedLSRRMS2TentacleNo;
  vector<double> GTIStop_SAAOptimizedLSRRMS2TentacleNo;
  
  bool GTIStatus_SAAOptimizedLSRRMS2TentacleRMS2 = Off;
  vector<double> GTIStart_SAAOptimizedLSRRMS2TentacleRMS2;
  vector<double> GTIStop_SAAOptimizedLSRRMS2TentacleRMS2;

  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
              
    // Case SAAOptimizedLSRRMS1TentacleNo
    if (GTIStatus_SAAOptimizedLSRRMS1TentacleNo != H.m_SoftSAAOptimizedLSRRMS_v1[h]) {
      if (H.m_SoftSAAOptimizedLSRRMS_v1[h] == On) {
        GTIStart_SAAOptimizedLSRRMS1TentacleNo.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAAOptimizedLSRRMS1TentacleNo.size() < GTIStart_SAAOptimizedLSRRMS1TentacleNo.size()) {
          GTIStop_SAAOptimizedLSRRMS1TentacleNo.push_back(H.m_Time[h]);
        }
      }
      GTIStatus_SAAOptimizedLSRRMS1TentacleNo = H.m_SoftSAAOptimizedLSRRMS_v1[h];
    }
    if (h == H.m_Time.size()-1 && GTIStatus_SAAOptimizedLSRRMS1TentacleNo == On && 
        GTIStop_SAAOptimizedLSRRMS1TentacleNo.size() < GTIStart_SAAOptimizedLSRRMS1TentacleNo.size()) {
      GTIStop_SAAOptimizedLSRRMS1TentacleNo.push_back(H.m_Time[h]+1);
    }
      
    // Case SAAOptimizedLSRRMS1TentacleRMS1
    if (GTIStatus_SAAOptimizedLSRRMS1TentacleRMS1 != (H.m_SoftSAAOptimizedLSRRMS_v1[h] == On || H.m_SoftTentacledRMS_v1[h] == On)) {
      if ((H.m_SoftSAAOptimizedLSRRMS_v1[h] == On || H.m_SoftTentacledRMS_v1[h] == On) == true) {
        GTIStart_SAAOptimizedLSRRMS1TentacleRMS1.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAAOptimizedLSRRMS1TentacleRMS1.size() < GTIStart_SAAOptimizedLSRRMS1TentacleRMS1.size()) {
          GTIStop_SAAOptimizedLSRRMS1TentacleRMS1.push_back(H.m_Time[h]);
        }
      }
      GTIStatus_SAAOptimizedLSRRMS1TentacleRMS1 = (H.m_SoftSAAOptimizedLSRRMS_v1[h] == On || H.m_SoftTentacledRMS_v1[h] == On);
    }
    if (h == H.m_Time.size()-1 && GTIStatus_SAAOptimizedLSRRMS1TentacleRMS1 == On && 
        GTIStop_SAAOptimizedLSRRMS1TentacleRMS1.size() < GTIStart_SAAOptimizedLSRRMS1TentacleRMS1.size()) {
      GTIStop_SAAOptimizedLSRRMS1TentacleRMS1.push_back(H.m_Time[h]+1);
    }

    
    // Case SAAOptimizedLSRRMS2TentacleNo
    if (GTIStatus_SAAOptimizedLSRRMS2TentacleNo != H.m_SoftSAAOptimizedLSRRMS_v2[h]) {
      if (H.m_SoftSAAOptimizedLSRRMS_v2[h] == On) {
        GTIStart_SAAOptimizedLSRRMS2TentacleNo.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAAOptimizedLSRRMS2TentacleNo.size() < GTIStart_SAAOptimizedLSRRMS2TentacleNo.size()) {
          GTIStop_SAAOptimizedLSRRMS2TentacleNo.push_back(H.m_Time[h]);
        }
      }
      GTIStatus_SAAOptimizedLSRRMS2TentacleNo = H.m_SoftSAAOptimizedLSRRMS_v2[h];
    }
    if (h == H.m_Time.size()-1 && GTIStatus_SAAOptimizedLSRRMS2TentacleNo == On && 
        GTIStop_SAAOptimizedLSRRMS2TentacleNo.size() < GTIStart_SAAOptimizedLSRRMS2TentacleNo.size()) {
      GTIStop_SAAOptimizedLSRRMS2TentacleNo.push_back(H.m_Time[h]+1);
    }
      
    // Case SAAOptimizedLSRRMS2TentacleRMS2
    if (GTIStatus_SAAOptimizedLSRRMS2TentacleRMS2 != (H.m_SoftSAAOptimizedLSRRMS_v2[h] == On || H.m_SoftTentacledRMS_v2[h] == On)) {
      if ((H.m_SoftSAAOptimizedLSRRMS_v2[h] == On || H.m_SoftTentacledRMS_v2[h] == On) == true) {
        GTIStart_SAAOptimizedLSRRMS2TentacleRMS2.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAAOptimizedLSRRMS2TentacleRMS2.size() < GTIStart_SAAOptimizedLSRRMS2TentacleRMS2.size()) {
          GTIStop_SAAOptimizedLSRRMS2TentacleRMS2.push_back(H.m_Time[h]);
        }
      }
      GTIStatus_SAAOptimizedLSRRMS2TentacleRMS2 = (H.m_SoftSAAOptimizedLSRRMS_v2[h] == On || H.m_SoftTentacledRMS_v2[h] == On);
    }
    if (h == H.m_Time.size()-1 && GTIStatus_SAAOptimizedLSRRMS2TentacleRMS2 == On && 
        GTIStop_SAAOptimizedLSRRMS2TentacleRMS2.size() < GTIStart_SAAOptimizedLSRRMS2TentacleRMS2.size()) {
      GTIStop_SAAOptimizedLSRRMS2TentacleRMS2.push_back(H.m_Time[h]+1);
    }
  }

  /*
  cout<<"Sizes: "<<GTIStart_SAAOptimizedLSRRMS1TentacleRMS1.size()<<" vs. "<<GTIStop_SAAOptimizedLSRRMS1TentacleRMS1.size()<<endl;
  for (unsigned int i = 0; i < GTIStart_SAAOptimizedLSRRMS1TentacleRMS1.size(); ++i) {
    cout<<GTIStart_SAAOptimizedLSRRMS1TentacleRMS1[i]<<":"<<GTIStop_SAAOptimizedLSRRMS1TentacleRMS1[i]<<endl; 
  }
  */
  
  CreateGTIFile(GTIStart_SAAOptimizedLSRRMS1TentacleNo, GTIStop_SAAOptimizedLSRRMS1TentacleNo, H, "SAAOptimizedLSRRMS1TentacleNo");
  CreateGTIFile(GTIStart_SAAOptimizedLSRRMS1TentacleRMS1, GTIStop_SAAOptimizedLSRRMS1TentacleRMS1, H, "SAAOptimizedLSRRMS1TentacleRMS1");
  CreateGTIFile(GTIStart_SAAOptimizedLSRRMS2TentacleNo, GTIStop_SAAOptimizedLSRRMS2TentacleNo, H, "SAAOptimizedLSRRMS2TentacleNo");
  CreateGTIFile(GTIStart_SAAOptimizedLSRRMS2TentacleRMS2, GTIStop_SAAOptimizedLSRRMS2TentacleRMS2, H, "SAAOptimizedLSRRMS2TentacleRMS2");
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::NormalizeOrbit(TH2D* Rates, TH2D* Normalizer, int ShiftedMedian) 
{
  if (ShiftedMedian == 0) {
     cout<<"NCheckRates::NormalizeOrbit: ShiftedMedian cannot be zero"<<endl;
    return;   
  }
  
  vector<double> RatesVector;
  
  for (int lo = 1; lo <= Normalizer->GetNbinsX(); ++lo) {
    for (int la = 1; la <= Normalizer->GetNbinsY(); ++la) {
      if (Normalizer->GetBinContent(lo, la) > 0 ) {
        Rates->SetBinContent(lo, la, Rates->GetBinContent(lo, la)/Normalizer->GetBinContent(lo, la));
        
        if (Rates->GetBinContent(lo, la) > 0) {
          RatesVector.push_back(Rates->GetBinContent(lo, la)); 
        }
      } else {
        Rates->SetBinContent(lo, la, 0);
      }
    }
  }
  sort(RatesVector.begin(), RatesVector.end());
  
  if (RatesVector.size() == 0) {
    cout<<"NCheckRates::NormalizeOrbit: No usable data"<<endl;
    return;
  }
  
  double Minimum = RatesVector[RatesVector.size()/ShiftedMedian];
  Rates->SetMinimum(Minimum);
  for (int b = 0; b < Rates->GetSize(); ++b) {
    if (Rates->GetAt(b) > 0 && Rates->GetAt(b) < Minimum) {
      Rates->SetAt(Minimum, b);
    }
  }
  double Maximum = RatesVector[(unsigned int) (0.99*RatesVector.size())];
  Rates->SetMaximum(Maximum);
}


////////////////////////////////////////////////////////////////////////////////


time_t NCheckRates::GetUTCTime(struct tm* T)
{
  // From: https://stackoverflow.com/questions/283166/easy-way-to-convert-a-struct-tm-expressed-in-utc-to-time-t-type
  
  char* tz = getenv("TZ");
  setenv("TZ", "", 1);
  tzset();
  
  time_t Time = mktime(T);
  
  if (tz) {
    setenv("TZ", tz, 1);
  } else {
    unsetenv("TZ");
  }
  tzset();
  
  return Time;
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::CreateReport(TString Observation, int Algorithm, int Mode,
                               TString ModeText, TString Module, 
                               double StartTime, TString ObservationDate, TString Object,
                               TH1D* RatesNoNo, TH2D* OrbitNoNo, double ObsTimeNoNo,
                               TH1D* RatesYesNo, TH2D* OrbitYesNo, double ObsTimeYesNo,
                               TH1D* RatesYesYes, TH2D* OrbitYesYes, double ObsTimeYesYes,
                               TH1D* SAARegion)
{
  // Each rates plot has lines indicating day changes, calculate the position here:
  ObservationDate = ObservationDate.ReplaceAll("T", " ");
  
  struct tm BaseTime;
  strptime(ObservationDate.Data(), "%Y-%m-%d %H:%M:%S", &BaseTime);
  time_t Base = mktime(&BaseTime); 
  
  struct tm NextDay = BaseTime;
  NextDay.tm_sec = 0;
  NextDay.tm_min = 0;
  NextDay.tm_hour = 0;
  
  vector<double> Days;
  do {
    NextDay.tm_mday += 1;
    time_t New = mktime(&NextDay); // This automatically takes care of overflows!
    if ((New - Base) / 1000.0 > RatesNoNo->GetXaxis()->GetXmax()) break;
    Days.push_back((New - Base) / 1000.0);
  } while (true);
    
  TString AlgorithmString = "Unknown";
  if (Algorithm == 1) AlgorithmString = "Figure-of-merit";
  else if (Algorithm == 2) AlgorithmString = "RMS-based";
  
  TString ModeString = "unknown";
  if (Mode == 1) ModeString = "strict";
  else if (Mode == 2) ModeString = "optimized";
    
  // Define the Canvas
  double Scaler = 150;
  TString CanvasTitle = TString("Canvas_") + ModeText + "_" + Module;
  TCanvas* Canvas = new TCanvas(CanvasTitle, "Canvas", 11*Scaler, 8.5*Scaler);
  
  double TitleSize = 0.03499;
  double TitleFontSize = 0.8;
  double SubTitleGap = 0.01;
  double SubTitleSize = 0.030;
  double SubTitleFontSize = 0.8;
  double GapSize = 0.025; 
  
  double HeaderSize = 0.025;
  double HeaderFontSize = 0.8;
  double BodySize = 0.25;
  double RatesWidth = 0.3;
  
  double Top = 0.0;
  double Bottom = 0.0;
  
  double TopMargin = 0.07;
  
  // Upper plot will be in pad1
  Canvas->cd();
  Top = 1.0;
  Bottom = Top - TitleSize;
  TPad* Title = new TPad("Title", "Title", 0, Bottom, 1, Top);
  Title->Draw();
  //Title->SetFillColor(2);
  Title->cd();
  TString TitleString;
  TitleString += "Target: ";
  TitleString += Object;
  TitleString += "      ID: ";
  TitleString += Observation;
  TitleString += "      Start: ";
  TitleString += ObservationDate;
  TitleString += " UTC";
  TitleString += "      Module: ";
  TitleString += Module;
  TText* T = new TText(.5, .5, TitleString);
  T->SetTextSize(TitleFontSize);
  T->SetTextAlign(22);
  T->Draw();
  
  Canvas->cd();   
  Top = Bottom - SubTitleGap;
  Bottom = Top - SubTitleSize;
  TPad* SubTitle = new TPad("SubTitle", "SubTitle", 0, Bottom, 1, Top);
  SubTitle->Draw();
  //SubTitle->SetFillColor(3);
  SubTitle->cd();
  TString SubTitleString(ModeText);
  TText* ST = new TText(.5, .5, SubTitleString);
  ST->SetTextSize(SubTitleFontSize);
  ST->SetTextAlign(22);
  ST->Draw();
  
  
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleNoNo = new TPad("PadTitleNoNo", "PadTitleNoNo", 0, Bottom, 1, Top);
  //PadTitleNoNo->SetFillColor(3);
  PadTitleNoNo->Draw();
  PadTitleNoNo->cd();
  TString NoNoMode("nucalcsaa options: nucalcsaa task not used!");
  TText* TextNoNo = new TText(.02, .5, NoNoMode);
  TextNoNo->SetTextSize(HeaderFontSize);
  TextNoNo->SetTextAlign(12);
  TextNoNo->Draw();
  ostringstream NoNoOut;
  NoNoOut<<"Effective exposure time: "<<ObsTimeNoNo<<" sec  (100.0%)";
  TText* TextTimesNoNo = new TText(.98, .5, NoNoOut.str().c_str());
  TextTimesNoNo->SetTextSize(HeaderFontSize);
  TextTimesNoNo->SetTextAlign(32);
  TextTimesNoNo->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesNoNo = new TPad("PadRatesNoNo", "PadRatesNoNo", 0, Bottom, RatesWidth, Top);
  //PadRatesNoNo->SetFillColor(4);
  PadRatesNoNo->SetLeftMargin(0.20);
  PadRatesNoNo->SetRightMargin(0.05);
  PadRatesNoNo->SetTopMargin(TopMargin);
  PadRatesNoNo->SetBottomMargin(0.15);
  PadRatesNoNo->Draw();
  PadRatesNoNo->cd();
  RatesNoNo->SetTitle("");
  RatesNoNo->GetXaxis()->CenterTitle(true);
  RatesNoNo->GetXaxis()->SetNdivisions(505);
  RatesNoNo->GetXaxis()->SetLabelSize(0.06);
  RatesNoNo->GetXaxis()->SetTitleSize(0.07);
  RatesNoNo->GetXaxis()->SetTitleOffset(0.9);
  RatesNoNo->GetYaxis()->CenterTitle(true);
  RatesNoNo->GetYaxis()->SetNdivisions(505);
  RatesNoNo->GetYaxis()->SetLabelSize(0.06);
  RatesNoNo->GetYaxis()->SetTitleSize(0.07);
  RatesNoNo->GetYaxis()->SetTitleOffset(1.3);
  RatesNoNo->SetMaximum(1.02*RatesNoNo->GetMaximum());
  RatesNoNo->Draw("HIST");
  // Draw some lines
  for (unsigned int i = 0; i < Days.size(); ++i) {
    TLine* L = new TLine(Days[i], 0, Days[i], RatesNoNo->GetMaximum());
    L->SetLineStyle(2);
    L->SetLineColor(15);
    L->Draw("SAME");
  }
  RatesNoNo->Draw("HIST SAME");
  
  //SAARegion->Scale(RatesNoNo->GetMaximum()/SAARegion->GetMaximum());
  //SAARegion->DrawCopy("SAME");
  //RatesNoNo->Draw("SAME");
  
  Canvas->cd();         
  TPad* PadOrbitNoNo = new TPad("PadOrbitNoNo", "PadOrbitNoNo", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitNoNo->SetFillColor(5);
  PadOrbitNoNo->SetLeftMargin(0.08);
  PadOrbitNoNo->SetRightMargin(0.13);
  PadOrbitNoNo->SetTopMargin(TopMargin);
  PadOrbitNoNo->SetBottomMargin(0.15);
  PadOrbitNoNo->SetGridx();
  PadOrbitNoNo->SetGridy();
  PadOrbitNoNo->Draw();
  PadOrbitNoNo->cd(); 
  PadOrbitNoNo->SetLogz();
  if (OrbitNoNo->GetMinimum() < 0.2) OrbitNoNo->SetMinimum(0.2);
  OrbitNoNo->SetTitle("");
  OrbitNoNo->GetXaxis()->CenterTitle(true);
  OrbitNoNo->GetXaxis()->SetLabelSize(0.06);
  OrbitNoNo->GetXaxis()->SetTitleSize(0.07);
  OrbitNoNo->GetXaxis()->SetTitleOffset(0.9);
  OrbitNoNo->GetXaxis()->SetNdivisions(612, false);
  OrbitNoNo->GetYaxis()->CenterTitle(true);
  OrbitNoNo->GetYaxis()->SetLabelSize(0.06);
  OrbitNoNo->GetYaxis()->SetTitleSize(0.07);
  OrbitNoNo->GetYaxis()->SetTitleOffset(0.35);
  OrbitNoNo->GetYaxis()->SetTickLength(0.015);
  OrbitNoNo->GetZaxis()->SetLabelSize(0.05);
  OrbitNoNo->GetZaxis()->CenterTitle(true);
  OrbitNoNo->GetZaxis()->SetLabelSize(0.06);
  OrbitNoNo->GetZaxis()->SetLabelOffset(0.0);
  OrbitNoNo->GetZaxis()->SetTitleSize(0.07);
  OrbitNoNo->GetZaxis()->SetTitleOffset(0.6);
  OrbitNoNo->GetZaxis()->SetMoreLogLabels(true);
  OrbitNoNo->Draw("colz");
  /*
  TPaletteAxis* OrbitNoNoPalette = (TPaletteAxis*) OrbitNoNo->GetListOfFunctions()->FindObject("palette");
  if (OrbitNoNoPalette != 0) {
    OrbitNoNoPalette->SetX1NDC(0.9);
    OrbitNoNoPalette->SetX2NDC(0.93);
    OrbitNoNoPalette->SetY1NDC(0.15);
    OrbitNoNoPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitNoNo->Update();
  }
  */
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleYesNo = new TPad("PadTitleYesNo", "PadTitleYesNo", 0, Bottom, 1, Top);
  PadTitleYesNo->Draw();
  PadTitleYesNo->cd(); 
  TString YesNoMode("nucalcsaa options: --saacalc=");
  YesNoMode += Algorithm;
  YesNoMode += " --saamode=";
  YesNoMode += ModeString;
  YesNoMode += " --tentacle=no";
  TText* TextYesNo = new TText(.02, .5, YesNoMode);
  TextYesNo->SetTextSize(HeaderFontSize);
  TextYesNo->SetTextAlign(12);
  TextYesNo->Draw();
  ostringstream YesNoOut;
  YesNoOut<<"Effective exposure time: "<<ObsTimeYesNo<<" sec  ("<<setprecision(4)<<100.0*ObsTimeYesNo/ObsTimeNoNo<<"%)";
  TText* TextTimesYesNo = new TText(.98, .5, YesNoOut.str().c_str());
  TextTimesYesNo->SetTextSize(HeaderFontSize);
  TextTimesYesNo->SetTextAlign(32);
  TextTimesYesNo->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesYesNo = new TPad("PadRatesYesNo", "PadRatesYesNo", 0, Bottom, RatesWidth, Top);
  //PadRatesYesNo->SetFillColor(4);
  PadRatesYesNo->SetLeftMargin(0.20);
  PadRatesYesNo->SetRightMargin(0.05);
  PadRatesYesNo->SetTopMargin(TopMargin);
  PadRatesYesNo->SetBottomMargin(0.15);
  PadRatesYesNo->Draw();
  PadRatesYesNo->cd();
  RatesYesNo->SetTitle("");
  RatesYesNo->GetXaxis()->CenterTitle(true);
  RatesYesNo->GetXaxis()->SetNdivisions(505);
  RatesYesNo->GetXaxis()->SetLabelSize(0.06);
  RatesYesNo->GetXaxis()->SetTitleSize(0.07);
  RatesYesNo->GetXaxis()->SetTitleOffset(0.9);
  RatesYesNo->GetYaxis()->CenterTitle(true);
  RatesYesNo->GetYaxis()->SetNdivisions(505);
  RatesYesNo->GetYaxis()->SetLabelSize(0.06);
  RatesYesNo->GetYaxis()->SetTitleSize(0.07);
  RatesYesNo->GetYaxis()->SetTitleOffset(1.3);
  RatesYesNo->SetMaximum(1.02*RatesYesNo->GetMaximum());
  RatesYesNo->Draw("HIST");
  // Draw some lines
  for (unsigned int i = 0; i < Days.size(); ++i) {
    TLine* L = new TLine(Days[i], 0, Days[i], RatesYesNo->GetMaximum());
    L->SetLineStyle(2);
    L->SetLineColor(15);
    L->Draw("SAME");
  }
  RatesYesNo->Draw("HIST SAME");
  //SAARegion->Scale(RatesYesNo->GetMaximum()/SAARegion->GetMaximum());
  //SAARegion->DrawCopy("SAME");
  //RatesYesNo->Draw("SAME");
  
  Canvas->cd();         
  TPad* PadOrbitYesNo = new TPad("PadOrbitYesNo", "PadOrbitYesNo", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitYesNo->SetFillColor(5);
  PadOrbitYesNo->SetLeftMargin(0.08);
  PadOrbitYesNo->SetRightMargin(0.13);
  PadOrbitYesNo->SetTopMargin(TopMargin);
  PadOrbitYesNo->SetBottomMargin(0.15);
  PadOrbitYesNo->SetGridx();
  PadOrbitYesNo->SetGridy();
  PadOrbitYesNo->Draw();
  PadOrbitYesNo->cd(); 
  PadOrbitYesNo->SetLogz();
  if (OrbitYesNo->GetMinimum() < 0.2) OrbitYesNo->SetMinimum(0.2);
  OrbitYesNo->SetTitle("");
  OrbitYesNo->GetXaxis()->CenterTitle(true);
  OrbitYesNo->GetXaxis()->SetLabelSize(0.06);
  OrbitYesNo->GetXaxis()->SetTitleSize(0.07);
  OrbitYesNo->GetXaxis()->SetTitleOffset(0.9);
  OrbitYesNo->GetXaxis()->SetNdivisions(612, false);
  OrbitYesNo->GetYaxis()->CenterTitle(true);
  OrbitYesNo->GetYaxis()->SetLabelSize(0.06);
  OrbitYesNo->GetYaxis()->SetTitleSize(0.07);
  OrbitYesNo->GetYaxis()->SetTitleOffset(0.35);
  OrbitYesNo->GetYaxis()->SetTickLength(0.015);
  OrbitYesNo->GetZaxis()->SetLabelSize(0.05);
  OrbitYesNo->GetZaxis()->CenterTitle(true);
  OrbitYesNo->GetZaxis()->SetLabelSize(0.06);
  OrbitYesNo->GetZaxis()->SetLabelOffset(0.0);
  OrbitYesNo->GetZaxis()->SetTitleSize(0.07);
  OrbitYesNo->GetZaxis()->SetTitleOffset(0.6);
  OrbitYesNo->GetZaxis()->SetMoreLogLabels(true);
  OrbitYesNo->Draw("colz");
  /*
  TPaletteAxis* OrbitYesNoPalette = (TPaletteAxis*) OrbitYesNo->GetListOfFunctions()->FindObject("palette");
  if (OrbitYesNoPalette != 0) {
    OrbitYesNoPalette->SetX1NDC(0.9);
    OrbitYesNoPalette->SetX2NDC(0.93);
    OrbitYesNoPalette->SetY1NDC(0.15);
    OrbitYesNoPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitYesNo->Update();
  }
  */
  
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleYesYes = new TPad("PadTitleYesYes", "PadTitleYesYes", 0, Bottom, 1, Top);
  PadTitleYesYes->Draw();
  PadTitleYesYes->cd(); 
  TString YesYesMode("nucalcsaa options: --saacalc=");
  YesYesMode += Algorithm;
  YesYesMode += " --saamode=";
  YesYesMode += ModeString;
  YesYesMode += " --tentacle=yes";
  TText* TextYesYes = new TText(.02, .5, YesYesMode);
  TextYesYes->SetTextSize(HeaderFontSize);
  TextYesYes->SetTextAlign(12);
  TextYesYes->Draw();
  ostringstream YesYesOut;
  YesYesOut<<"Effective exposure time: "<<ObsTimeYesYes<<" sec  ("<<setprecision(4)<<100.0*ObsTimeYesYes/ObsTimeNoNo<<"%)";
  TText* TextTimesYesYes = new TText(.98, .5, YesYesOut.str().c_str());
  TextTimesYesYes->SetTextSize(HeaderFontSize);
  TextTimesYesYes->SetTextAlign(32);
  TextTimesYesYes->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesYesYes = new TPad("PadRatesYesYes", "PadRatesYesYes", 0, Bottom, RatesWidth, Top);
  //PadRatesYesYes->SetFillColor(4);
  PadRatesYesYes->SetLeftMargin(0.20);
  PadRatesYesYes->SetRightMargin(0.05);
  PadRatesYesYes->SetTopMargin(TopMargin);
  PadRatesYesYes->SetBottomMargin(0.15);
  PadRatesYesYes->Draw();
  PadRatesYesYes->cd(); 
  RatesYesYes->SetTitle("");
  RatesYesYes->GetXaxis()->CenterTitle(true);
  RatesYesYes->GetXaxis()->SetNdivisions(505);
  RatesYesYes->GetXaxis()->SetLabelSize(0.06);
  RatesYesYes->GetXaxis()->SetTitleSize(0.07);
  RatesYesYes->GetXaxis()->SetTitleOffset(0.9);
  RatesYesYes->GetYaxis()->CenterTitle(true);
  RatesYesYes->GetYaxis()->SetNdivisions(505);
  RatesYesYes->GetYaxis()->SetLabelSize(0.06);
  RatesYesYes->GetYaxis()->SetTitleSize(0.07);
  RatesYesYes->GetYaxis()->SetTitleOffset(1.3);
  RatesYesYes->SetMaximum(1.02*RatesYesYes->GetMaximum());
  RatesYesYes->Draw("HIST");
  // Draw some lines
  for (unsigned int i = 0; i < Days.size(); ++i) {
    TLine* L = new TLine(Days[i], 0, Days[i], RatesYesYes->GetMaximum());
    L->SetLineStyle(2);
    L->SetLineColor(15);
    L->Draw("SAME");
  }
  RatesYesYes->Draw("HIST SAME");
  //SAARegion->Scale(RatesYesYes->GetMaximum()/SAARegion->GetMaximum());
  //SAARegion->DrawCopy("SAME");
  //RatesYesYes->Draw("SAME");
  
  Canvas->cd();         
  TPad* PadOrbitYesYes = new TPad("PadOrbitYesYes", "PadOrbitYesYes", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitYesYes->SetFillColor(5);
  PadOrbitYesYes->SetLeftMargin(0.08);
  PadOrbitYesYes->SetRightMargin(0.13);
  PadOrbitYesYes->SetTopMargin(TopMargin);
  PadOrbitYesYes->SetBottomMargin(0.15);
  PadOrbitYesYes->SetGridx();
  PadOrbitYesYes->SetGridy();
  PadOrbitYesYes->Draw();
  PadOrbitYesYes->cd(); 
  PadOrbitYesYes->SetLogz();
  if (OrbitYesYes->GetMinimum() < 0.2) OrbitYesYes->SetMinimum(0.2);
  OrbitYesYes->SetTitle("");
  OrbitYesYes->GetXaxis()->CenterTitle(true);
  OrbitYesYes->GetXaxis()->SetLabelSize(0.06);
  OrbitYesYes->GetXaxis()->SetTitleSize(0.07);
  OrbitYesYes->GetXaxis()->SetTitleOffset(0.9);
  OrbitYesYes->GetXaxis()->SetNdivisions(612, false);
  OrbitYesYes->GetYaxis()->CenterTitle(true);
  OrbitYesYes->GetYaxis()->SetLabelSize(0.06);
  OrbitYesYes->GetYaxis()->SetTitleSize(0.07);
  OrbitYesYes->GetYaxis()->SetTitleOffset(0.35);
  OrbitYesYes->GetYaxis()->SetTickLength(0.015);
  OrbitYesYes->GetZaxis()->SetLabelSize(0.05);
  OrbitYesYes->GetZaxis()->CenterTitle(true);
  OrbitYesYes->GetZaxis()->SetLabelSize(0.06);
  OrbitYesYes->GetZaxis()->SetLabelOffset(0.0);
  OrbitYesYes->GetZaxis()->SetTitleSize(0.07);
  OrbitYesYes->GetZaxis()->SetTitleOffset(0.6);
  OrbitYesYes->GetZaxis()->SetMoreLogLabels(true);
  OrbitYesYes->Draw("colz");
  /*
  TPaletteAxis* OrbitYesYesPalette = (TPaletteAxis*) OrbitYesYes->GetListOfFunctions()->FindObject("palette");
  if (OrbitYesYesPalette != 0) {
    OrbitYesYesPalette->SetX1NDC(0.9);
    OrbitYesYesPalette->SetX2NDC(0.93);
    OrbitYesYesPalette->SetY1NDC(0.15);
    OrbitYesYesPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitYesYes->Update();
  }
  */
  // Mark highest pixel:
  //cout<<"Final bottom: "<<Bottom<<endl;
  
  
  TString Report = TString("Report") + "_id" + Observation + "_m" + Module;
  Report += "_saaalgorithm";
  Report += Algorithm;
  Report += TString("_saacut") + ModeString + m_FileType;
  Report.ReplaceAll(" ", "");
  Report.ReplaceAll(")", "");
  Report.ReplaceAll("(", "");
  Report.ReplaceAll(":", "");
  Report.ReplaceAll("-", "");
  Report.ReplaceAll(",", "");
  
  if (m_ShowHistograms.Contains("f")) Canvas->SaveAs(Report);  
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::ShowRates(TH1D* Rates, TH1D* SAARegion, double LifeTime)
{
  TString Title = Rates->GetTitle();
  
  ostringstream out;
  out<<"#splitline{"<<Rates->GetTitle()<<"}{with life time relative to no cuts: "<<setprecision(4)<<LifeTime*100<<"%}";
  Rates->SetTitle(out.str().c_str());
  
  TCanvas* Canvas = new TCanvas();
  Canvas->SetTitle(Title);
  Canvas->cd();
  Rates->Draw("HIST");
  SAARegion->Scale(Rates->GetMaximum()/SAARegion->GetMaximum());
  SAARegion->DrawCopy("HIST SAME");
  Rates->Draw("HIST SAME");
  Canvas->Update();
  if (m_ShowHistograms.Contains("f")) Canvas->SaveAs(Rates->GetName() + m_FileType);
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::ShowRates(TH2D* Orbit, double LifeTime)
{
  ostringstream out;
  out<<"#splitline{"<<Orbit->GetTitle()<<"}{with life time relative to no cuts: "<<setprecision(4)<<LifeTime*100<<"%}";
  Orbit->SetTitle(out.str().c_str());
  
  TCanvas* Canvas = new TCanvas(TString(Orbit->GetName()) + "Canvas", TString(Orbit->GetName()) + "Canvas", 1200, 400);
  Canvas->cd();
  Canvas->SetLogz();
  if (Orbit->GetMinimum() < 0.2) Orbit->SetMinimum(0.2);
  Orbit->Draw("colz");
  Canvas->Update();
  if (m_ShowHistograms.Contains("f")) Canvas->SaveAs(Orbit->GetName() + m_FileType);
}

  
////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H, TString Name)
{
  // Copy template
  TString FileName = "nu";
  FileName += H.m_ID;
  FileName += ((H.m_Module == 0) ? "A" : "B");
  FileName += "_";
  FileName += Name;
  FileName += "_saa_cut.fits";

  TString Template = "$(NUSIM)/resource/nulyses/Template_gti.fits";
  MFile::ExpandFileName(Template);

  ifstream src(Template);
  ofstream dst(FileName);
  dst << src.rdbuf();
  src.close();
  dst.close();
  
  // Section 1:
  // initialize the files
  int status = 0;
  char value[1000];
  char templt[100];

  fitsfile* File = 0;
  if (fits_open_file(&File, FileName, READWRITE, &status)) {
    fits_get_errstatus(status, value);
    cout<<"Unable to open GTI file \""<<FileName<<"\": "<<status<<":"<<value<<endl;
    return false;
  }
  //cout<<"Opened "<<FileName<<endl;

  // Move to the second hdu
  fits_movabs_hdu(File, 2, NULL, &status);

  // Get the number of columns
  int NColumns = 0;
  map<TString, int> Columns;
  fits_get_num_cols(File, &NColumns, &status);
  for (int c = 1; c <= NColumns; ++c) {
    int col;
    snprintf(templt, sizeof(templt), "%d", c);
    fits_get_colname(File, CASEINSEN, templt, value, &col, &status);
    Columns[value] = c;
    //cout<<value<<endl;
  }

  for (unsigned int s = 1; s <= GTIStart.size(); ++s) {
    fits_write_col_dbl(File, Columns["START"], s, 1, 1, &GTIStart[s-1], &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing GTI column START failed (" << value << ")" << endl;
      fits_close_file(File, &status);
      File = 0;
      return false;
    }
  }

  for (unsigned int s = 1; s <= GTIStop.size(); ++s) {
    fits_write_col_dbl(File, Columns["STOP"], s, 1, 1, &GTIStop[s-1], &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing GTI column STOPfailed (" << value << ")" << endl;
      fits_close_file(File, &status);
      File = 0;
      return false;
    }
  }
  
  fits_close_file(File, &status);
  
  return true;
}


// NCheckRates.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
