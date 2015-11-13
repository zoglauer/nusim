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
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TText.h"
#include "TCanvas.h"
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
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;

  TH1D* Rates = new TH1D(TString("Rates") + iID, TString("Rates all") + ID, RatesBins, MinTime, MaxTime);
  Rates->SetXTitle("Time [s]");
  Rates->SetYTitle("cts/sec");

  TH1D* ShieldRatesHigh = new TH1D(TString("ShieldRatesHigh") + iID, TString("ShieldRatesHigh") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesHigh->SetXTitle("Time [s]");
  ShieldRatesHigh->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerShieldRatesHighByOrbit = new TH2D(TString("OrbitNormalizerShieldRatesHighByOrbit") + iID, 
                                                         TString("OrbitNormalizerShieldRatesHighByOrbit") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* ShieldRatesHighByOrbit = new TH2D(TString("ShieldRatesHighByOrbit") + iID, 
                                          TString("Shield rates high by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  ShieldRatesHighByOrbit->SetXTitle("Longitude [deg]");
  ShieldRatesHighByOrbit->SetYTitle("Latutude [deg]");
  ShieldRatesHighByOrbit->SetZTitle("cts/sec");  
  
  
  TH1D* ShieldRatesLow = new TH1D(TString("ShieldRatesLow") + iID, TString("ShieldRatesLow") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesLow->SetXTitle("Time [s]");
  ShieldRatesLow->SetYTitle("cts/sec");

  TH2D* OrbitNormalizerShieldRatesLowByOrbit = new TH2D(TString("OrbitNormalizerShieldRatesLowByOrbit") + iID, 
                                                        TString("OrbitNormalizerShieldRatesLowByOrbit") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* ShieldRatesLowByOrbit = new TH2D(TString("ShieldRatesLowByOrbit") + iID, 
                                          TString("Shield rates low by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  ShieldRatesLowByOrbit->SetXTitle("Longitude [deg]");
  ShieldRatesLowByOrbit->SetYTitle("Latutude [deg]");
  ShieldRatesLowByOrbit->SetZTitle("cts/sec");  

  TH1D* ShieldRatesEntries = new TH1D(TString("ShieldRatesEntries") + iID, TString("ShieldRatesEntries") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesEntries->SetXTitle("Time [s]");
  ShieldRatesEntries->SetYTitle("cts");

  TH1D* SAARegion = new TH1D(TString("SAARegion") + iID, TString("SAARegion") + ID, RatesBins, MinTime, MaxTime);
  SAARegion->SetXTitle("Time [s]");
  SAARegion->SetYTitle("cts/sec");
  SAARegion->SetLineColor(kYellow);
  SAARegion->SetFillColor(kYellow);
  
  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimes->SetXTitle("Time [s]");
  LifeTimes->SetYTitle("cts/sec");

  
  
  TH1D* RatesSAANoTentacleNo = new TH1D(TString("RatesSAANoTentacleNo") + iID, TString("Rates - SAA: No, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleNo->SetXTitle("Time [s]");
  RatesSAANoTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAANoTentacleNo = new TH1D(TString("LifeTimesSAANoTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAANoTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAANoTentacleNo->SetYTitle("cts/sec");
  
  
  TH1D* RatesSAAStrictHSRTentacleNo = new TH1D(TString("RatesSAAStrictHSRTentacleNo") + iID, TString("Rates - SAA: StrictHSR, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictHSRTentacleNo->SetXTitle("Time [s]");
  RatesSAAStrictHSRTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictHSRTentacleNo = new TH1D(TString("LifeTimesSAAStrictHSRTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictHSRTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAStrictHSRTentacleNo->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedHSRFoMTentacleNo = new TH1D(TString("RatesSAAOptimizedHSRFoMTentacleNo") + iID, TString("Rates - SAA: Optimized, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedHSRFoMTentacleNo->SetXTitle("Time [s]");
  RatesSAAOptimizedHSRFoMTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedHSRFoMTentacleNo = new TH1D(TString("LifeTimesSAAOptimizedHSRFoMTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedHSRFoMTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedHSRFoMTentacleNo->SetYTitle("cts/sec");

  
  TH1D* RatesSAAStrictLSRTentacleNo = new TH1D(TString("RatesSAAStrictLSRTentacleNo") + iID, TString("Rates - SAA: Super strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictLSRTentacleNo->SetXTitle("Time [s]");
  RatesSAAStrictLSRTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictLSRTentacleNo = new TH1D(TString("LifeTimesSAAStrictLSRTentacleNo") + iID, TString("LifeTimesSAAStrictLSRTentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictLSRTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAStrictLSRTentacleNo->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedLSRRMSTentacleNo = new TH1D(TString("RatesSAAOptimizedLSRRMSTentacleNo") + iID, TString("Rates - SAA: Optimized super strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMSTentacleNo->SetXTitle("Time [s]");
  RatesSAAOptimizedLSRRMSTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMSTentacleNo = new TH1D(TString("LifeTimesSAAOptimizedLSRRMSTentacleNo") + iID, TString("LifeTimesSAAOptimizedLSRRMSTentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMSTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedLSRRMSTentacleNo->SetYTitle("cts/sec");

  
  
  TH1D* RatesSAANoTentacleRMS = new TH1D(TString("RatesSAANoTentacleRMS") + iID, TString("Rates - SAA: No, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleRMS->SetXTitle("Time [s]");
  RatesSAANoTentacleRMS->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAANoTentacleRMS = new TH1D(TString("LifeTimesSAANoTentacleRMS") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAANoTentacleRMS->SetXTitle("Time [s]");
  LifeTimesSAANoTentacleRMS->SetYTitle("cts/sec");

  
  TH1D* RatesSAAStrictHSRTentacleFoM = new TH1D(TString("RatesSAAStrictHSRTentacleFoM") + iID, TString("Rates - SAA: StrictHSR, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictHSRTentacleFoM->SetXTitle("Time [s]");
  RatesSAAStrictHSRTentacleFoM->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictHSRTentacleFoM = new TH1D(TString("LifeTimesSAAStrictHSRTentacleFoM") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictHSRTentacleFoM->SetXTitle("Time [s]");
  LifeTimesSAAStrictHSRTentacleFoM->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedHSRFoMTentacleFoM = new TH1D(TString("RatesSAAOptimizedHSRFoMTentacleFoM") + iID, TString("Rates - SAA: Optimized, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedHSRFoMTentacleFoM->SetXTitle("Time [s]");
  RatesSAAOptimizedHSRFoMTentacleFoM->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedHSRFoMTentacleFoM = new TH1D(TString("LifeTimesSAAOptimizedHSRFoMTentacleFoM") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedHSRFoMTentacleFoM->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedHSRFoMTentacleFoM->SetYTitle("cts/sec");

  
  TH1D* RatesSAAStrictLSRTentacleRMS = new TH1D(TString("RatesSAAStrictLSRTentacleRMS") + iID, TString("Rates - SAA: Super strict, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictLSRTentacleRMS->SetXTitle("Time [s]");
  RatesSAAStrictLSRTentacleRMS->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictLSRTentacleRMS = new TH1D(TString("LifeTimesSAAStrictLSRTentacleRMS") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictLSRTentacleRMS->SetXTitle("Time [s]");
  LifeTimesSAAStrictLSRTentacleRMS->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedLSRRMSTentacleRMS = new TH1D(TString("RatesSAAOptimizedLSRRMSTentacleRMS") + iID, TString("Rates - SAA: Optimized super strict, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedLSRRMSTentacleRMS->SetXTitle("Time [s]");
  RatesSAAOptimizedLSRRMSTentacleRMS->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedLSRRMSTentacleRMS = new TH1D(TString("LifeTimesSAAOptimizedLSRRMSTentacleRMS") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedLSRRMSTentacleRMS->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedLSRRMSTentacleRMS->SetYTitle("cts/sec");


  
  TH2D* OrbitNormalizerDetectorSAANoTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleNoByOrbit = new TH2D(TString("RatesSAANoTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAANoTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedHSRFoMTentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedHSRFoMTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedHSRFoMTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictLSRTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictLSRTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictLSRTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictLSRTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictLSRTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Super strict, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictLSRTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictLSRTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictLSRTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMSTentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMSTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized super strict, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMSTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMSTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedLSRRMSTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictHSRTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictHSRTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictHSRTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictHSRTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictHSRTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: StrictHSR, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictHSRTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictHSRTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictHSRTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAANoTentacleRMS = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleRMS") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleRMS") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleRMSByOrbit = new TH2D(TString("RatesSAANoTentacleRMSByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: No, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleRMSByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleRMSByOrbit->SetYTitle("Latutude [deg]");
  RatesSAANoTentacleRMSByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedHSRFoMTentacleFoMByOrbit = new TH2D(TString("RatesSAAOptimizedHSRFoMTentacleFoMByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictLSRTentacleRMS = new TH2D(TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictLSRTentacleRMS") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictLSRTentacleRMSByOrbit = new TH2D(TString("RatesSAAStrictLSRTentacleRMSByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Super strict, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictLSRTentacleRMSByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictLSRTentacleRMSByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictLSRTentacleRMSByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleRMS = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleRMS") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleRMS") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedLSRRMSTentacleRMSByOrbit = new TH2D(TString("RatesSAAOptimizedLSRRMSTentacleRMSByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized super strict, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedLSRRMSTentacleRMSByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedLSRRMSTentacleRMSByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedLSRRMSTentacleRMSByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictHSRTentacleFoM = new TH2D(TString("OrbitNormalizerDetectorSAAStrictHSRTentacleFoM") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictHSRTentacleFoM") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictHSRTentacleFoMByOrbit = new TH2D(TString("RatesSAAStrictHSRTentacleFoMByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: StrictHSR, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictHSRTentacleFoMByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictHSRTentacleFoMByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictHSRTentacleFoMByOrbit->SetZTitle("cts/sec");
  
  
  // GTI's
  //vector<double> GTIStart;
  
  // Exposures:
  double TimeSAANoTentacleNo = 0.0; 
  double TimeSAANoTentacleRMS = 0.0; 

  double TimeSAAStrictHSRTentacleNo = 0.0; 
  double TimeSAAStrictHSRTentacleFoM = 0.0; 

  double TimeSAAOptimizedHSRFoMTentacleNo = 0.0; 
  double TimeSAAOptimizedHSRFoMTentacleFoM = 0.0; 

  double TimeSAAStrictLSRTentacleNo = 0.0; 
  double TimeSAAStrictLSRTentacleRMS = 0.0; 

  double TimeSAAOptimizedLSRRMSTentacleNo = 0.0; 
  double TimeSAAOptimizedLSRRMSTentacleRMS = 0.0; 
  
  // Create a list of pixels with high source count for elimination
  vector<int> ExcludedDetRawXY = MostlyEliminateSource(F, false);
  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {
    
    //if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    //if (WithinSpecialBTI(F.m_Time[e]) == true) continue;
    if (F.m_Energy[e] < SpectrumMin || F.m_Energy[e] > SpectrumMax) continue;
    
    if (F.IsGTI(F.m_Time[e]) == false) continue;
    
    int HKIndex = H.FindClosestIndex(F.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found for time "<<F.m_Time[e]<<"..."<<endl;
      continue;      
    }
    
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
    
    if (H.m_HardSAA[HKIndex] == false) {
      RatesSAANoTentacleNo->Fill(F.m_Time[e]);
      RatesSAANoTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);
    }
    
    if (H.m_SoftTentacledRMS[HKIndex] == false) {
      RatesSAANoTentacleRMS->Fill(F.m_Time[e]);
      RatesSAANoTentacleRMSByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    
    if (H.m_SoftSAAStrictHSR[HKIndex] == false) {
      RatesSAAStrictHSRTentacleNo->Fill(F.m_Time[e]);
      RatesSAAStrictHSRTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    if (H.m_SoftSAAStrictHSR[HKIndex] == false && H.m_SoftTentacledFoM[HKIndex] == false) {
      RatesSAAStrictHSRTentacleFoM->Fill(F.m_Time[e]);
      RatesSAAStrictHSRTentacleFoMByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    
    if (H.m_SoftSAAOptimizedHSRFoM[HKIndex] == false) {
      RatesSAAOptimizedHSRFoMTentacleNo->Fill(F.m_Time[e]);
      RatesSAAOptimizedHSRFoMTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    if (H.m_SoftSAAOptimizedHSRFoM[HKIndex] == false && H.m_SoftTentacledFoM[HKIndex] == false) {
      RatesSAAOptimizedHSRFoMTentacleFoM->Fill(F.m_Time[e]);
      RatesSAAOptimizedHSRFoMTentacleFoMByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    
    if (H.m_SoftSAAStrictLSR[HKIndex] == false) {
      RatesSAAStrictLSRTentacleNo->Fill(F.m_Time[e]);
      RatesSAAStrictLSRTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    if (H.m_SoftSAAStrictLSR[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
      RatesSAAStrictLSRTentacleRMS->Fill(F.m_Time[e]);
      RatesSAAStrictLSRTentacleRMSByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    
    if (H.m_SoftSAAOptimizedLSRRMS[HKIndex] == false) {
      RatesSAAOptimizedLSRRMSTentacleNo->Fill(F.m_Time[e]);
      RatesSAAOptimizedLSRRMSTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    if (H.m_SoftSAAOptimizedLSRRMS[HKIndex] == false && H.m_SoftTentacledRMS[HKIndex] == false) {
      RatesSAAOptimizedLSRRMSTentacleRMS->Fill(F.m_Time[e]);
      RatesSAAOptimizedLSRRMSTentacleRMSByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
    }
    
  }
  
  
  double TotalTime = 0;
  LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    double Life = 1.0; //H.m_LiveTime[h];

    LifeTimes->Fill(H.m_Time[h], Life);
    
    int OrbitIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<H.m_Time[h]<<"..."<<endl;
      continue;      
    }

    if (O.m_Longitude[OrbitIndex] > SAARegionMin && O.m_Longitude[OrbitIndex] < SAARegionMax) {
      SAARegion->SetBinContent(SAARegion->FindBin(H.m_Time[h]), 1); 
    }

    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    if (F.IsGTI(H.m_Time[h]) == false) continue;

    ShieldRatesHigh->Fill(H.m_Time[h], H.m_ShieldRateHigh[h]);  
    OrbitNormalizerShieldRatesHighByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
    ShieldRatesHighByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_ShieldRateHigh[h]);  
    ShieldRatesLow->Fill(H.m_Time[h], H.m_ShieldRateLow[h]);  
    OrbitNormalizerShieldRatesLowByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
    ShieldRatesLowByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_ShieldRateLow[h]);  
    ShieldRatesEntries->Fill(H.m_Time[h], 1);  
 
    //if (H.m_LiveTime[h] == 0) continue;
    
    
    if (H.m_HardSAA[h] == false) {
      OrbitNormalizerDetectorSAANoTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);
      LifeTimesSAANoTentacleNo->Fill(H.m_Time[h], Life);
      TimeSAANoTentacleNo += 1;
    }
    
    if (H.m_SoftTentacledRMS[h] == false) {
      OrbitNormalizerDetectorSAANoTentacleRMS->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAANoTentacleRMS->Fill(H.m_Time[h], Life);
      TimeSAANoTentacleRMS += 1;
    }
          
    if (H.m_SoftSAAStrictHSR[h] == false) {
      OrbitNormalizerDetectorSAAStrictHSRTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictHSRTentacleNo->Fill(H.m_Time[h], Life);
      TimeSAAStrictHSRTentacleNo += 1;
    }
    if (H.m_SoftSAAStrictHSR[h] == false && H.m_SoftTentacledFoM[h] == false) {
      OrbitNormalizerDetectorSAAStrictHSRTentacleFoM->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictHSRTentacleFoM->Fill(H.m_Time[h], Life);
      TimeSAAStrictHSRTentacleFoM += 1;
    }

    if (H.m_SoftSAAOptimizedHSRFoM[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedHSRFoMTentacleNo->Fill(H.m_Time[h], Life);
      TimeSAAOptimizedHSRFoMTentacleNo += 1;
    }
    if (H.m_SoftSAAOptimizedHSRFoM[h] == false && H.m_SoftTentacledFoM[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedHSRFoMTentacleFoM->Fill(H.m_Time[h], Life);
      TimeSAAOptimizedHSRFoMTentacleFoM += 1;
    }

    if (H.m_SoftSAAStrictLSR[h] == false) {
      OrbitNormalizerDetectorSAAStrictLSRTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictLSRTentacleNo->Fill(H.m_Time[h], Life);
      TimeSAAStrictLSRTentacleNo += 1;
    }
    if (H.m_SoftSAAStrictLSR[h] == false && H.m_SoftTentacledRMS[h] == false) {
      OrbitNormalizerDetectorSAAStrictLSRTentacleRMS->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAStrictLSRTentacleRMS->Fill(H.m_Time[h], Life);
      TimeSAAStrictLSRTentacleRMS += 1;
    }

    if (H.m_SoftSAAOptimizedLSRRMS[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMSTentacleNo->Fill(H.m_Time[h], Life);
      TimeSAAOptimizedLSRRMSTentacleNo += 1;
    }
    if (H.m_SoftSAAOptimizedLSRRMS[h] == false && H.m_SoftTentacledRMS[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleRMS->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], Life);    
      LifeTimesSAAOptimizedLSRRMSTentacleRMS->Fill(H.m_Time[h], Life);
      TimeSAAOptimizedLSRRMSTentacleRMS += 1;
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
  NormalizeOrbit(RatesSAAOptimizedHSRFoMTentacleNoByOrbit, OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAStrictLSRTentacleNoByOrbit, OrbitNormalizerDetectorSAAStrictLSRTentacleNo, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedLSRRMSTentacleNoByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleNo, ShiftedMedian);
  
  NormalizeOrbit(RatesSAANoTentacleRMSByOrbit, OrbitNormalizerDetectorSAANoTentacleRMS, ShiftedMedian);
  NormalizeOrbit(RatesSAAStrictHSRTentacleFoMByOrbit, OrbitNormalizerDetectorSAAStrictHSRTentacleFoM, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedHSRFoMTentacleFoMByOrbit, OrbitNormalizerDetectorSAAOptimizedHSRFoMTentacleFoM, ShiftedMedian);
  NormalizeOrbit(RatesSAAStrictLSRTentacleRMSByOrbit, OrbitNormalizerDetectorSAAStrictLSRTentacleRMS, ShiftedMedian);
  NormalizeOrbit(RatesSAAOptimizedLSRRMSTentacleRMSByOrbit, OrbitNormalizerDetectorSAAOptimizedLSRRMSTentacleRMS, ShiftedMedian);
  
  NormalizeOrbit(ShieldRatesHighByOrbit, OrbitNormalizerShieldRatesHighByOrbit, ShiftedMedian);
  NormalizeOrbit(ShieldRatesLowByOrbit, OrbitNormalizerShieldRatesLowByOrbit, ShiftedMedian);

 
  // (2) Normalize
  double Max = 0;
  double CutOffLifeTimeFraction = 0.5; // In some bins we will only be partially on - skip those where we are below CutOffLifeTimeFraction on
  
  
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAANoTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAANoTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAANoTentacleNo->SetBinContent(b, 0); 
    } else {
      RatesSAANoTentacleNo->SetBinContent(b, RatesSAANoTentacleNo->GetBinContent(b)/RatesSAANoTentacleNo->GetBinWidth(b)/LifeTimesSAANoTentacleNo->GetBinContent(b));
    }
    if (Max < RatesSAANoTentacleNo->GetBinContent(b)) {
      Max = RatesSAANoTentacleNo->GetBinContent(b);
    }
  }
  SAARegion->Scale(Max);
  
  
  
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedHSRFoMTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedHSRFoMTentacleNo->SetBinContent(b, RatesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b)/RatesSAAOptimizedHSRFoMTentacleNo->GetBinWidth(b)/LifeTimesSAAOptimizedHSRFoMTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictLSRTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAStrictLSRTentacleNo->SetBinContent(b, RatesSAAStrictLSRTentacleNo->GetBinContent(b)/RatesSAAStrictLSRTentacleNo->GetBinWidth(b)/LifeTimesSAAStrictLSRTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMSTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMSTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMSTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMSTentacleNo->SetBinContent(b, RatesSAAOptimizedLSRRMSTentacleNo->GetBinContent(b)/RatesSAAOptimizedLSRRMSTentacleNo->GetBinWidth(b)/LifeTimesSAAOptimizedLSRRMSTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictHSRTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAStrictHSRTentacleNo->SetBinContent(b, RatesSAAStrictHSRTentacleNo->GetBinContent(b)/RatesSAAStrictHSRTentacleNo->GetBinWidth(b)/LifeTimesSAAStrictHSRTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAANoTentacleRMS->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAANoTentacleRMS->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAANoTentacleRMS->SetBinContent(b, 0);
    } else {
      RatesSAANoTentacleRMS->SetBinContent(b, RatesSAANoTentacleRMS->GetBinContent(b)/RatesSAANoTentacleRMS->GetBinWidth(b)/LifeTimesSAANoTentacleRMS->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedHSRFoMTentacleFoM->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedHSRFoMTentacleFoM->SetBinContent(b, RatesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b)/RatesSAAOptimizedHSRFoMTentacleFoM->GetBinWidth(b)/LifeTimesSAAOptimizedHSRFoMTentacleFoM->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictLSRTentacleRMS->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictLSRTentacleRMS->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictLSRTentacleRMS->SetBinContent(b, 0);
    } else {
      RatesSAAStrictLSRTentacleRMS->SetBinContent(b, RatesSAAStrictLSRTentacleRMS->GetBinContent(b)/RatesSAAStrictLSRTentacleRMS->GetBinWidth(b)/LifeTimesSAAStrictLSRTentacleRMS->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedLSRRMSTentacleRMS->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedLSRRMSTentacleRMS->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedLSRRMSTentacleRMS->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedLSRRMSTentacleRMS->SetBinContent(b, RatesSAAOptimizedLSRRMSTentacleRMS->GetBinContent(b)/RatesSAAOptimizedLSRRMSTentacleRMS->GetBinWidth(b)/LifeTimesSAAOptimizedLSRRMSTentacleRMS->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictHSRTentacleFoM->SetBinContent(b, 0);
    } else {
      RatesSAAStrictHSRTentacleFoM->SetBinContent(b, RatesSAAStrictHSRTentacleFoM->GetBinContent(b)/RatesSAAStrictHSRTentacleFoM->GetBinWidth(b)/LifeTimesSAAStrictHSRTentacleFoM->GetBinContent(b));
    }
  }
  
  
  for (int b = 1; b <= ShieldRatesEntries->GetNbinsX(); ++b) {
    if (ShieldRatesEntries->GetBinContent(b) == 0) continue;
    ShieldRatesHigh->SetBinContent(b, ShieldRatesHigh->GetBinContent(b)/ShieldRatesEntries->GetBinContent(b));
    ShieldRatesLow->SetBinContent(b, ShieldRatesLow->GetBinContent(b)/ShieldRatesEntries->GetBinContent(b));
  }
  
  
  // Show histograms:
    
  TCanvas* ShieldRatesHighCanvas = new TCanvas();
  ShieldRatesHighCanvas->cd();
  ShieldRatesHigh->Draw();
  ShieldRatesHighCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ShieldRatesHighCanvas->SaveAs(ShieldRatesHigh->GetName() + m_FileType);
  ShowRates(ShieldRatesHigh, SAARegion, 1);
  ShowRates(ShieldRatesHighByOrbit, 1);
      
  TCanvas* ShieldRatesLowCanvas = new TCanvas();
  ShieldRatesLowCanvas->cd();
  ShieldRatesLow->Draw();
  ShieldRatesLowCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ShieldRatesLowCanvas->SaveAs(ShieldRatesLow->GetName() + m_FileType);
  ShowRates(ShieldRatesLow, SAARegion, 1);
  ShowRates(ShieldRatesLowByOrbit, 1);

  //TCanvas* PositionsOnSourceCanvas = new TCanvas(TString("PositionsOnSourceCanvas") + iID, TString("PositionsOnSourceCanvas") + ID, 600, 600);
  //PositionsOnSourceCanvas->cd();
  //PositionsOnSource->Draw("colz");
  //ositionsOnSourceCanvas->Update();
  //if (m_ShowHistograms.Contains("f")) PositionsOnSourceCanvas->SaveAs(PositionsOnSource->GetName() + m_FileType);

  
  CreateReport(F.m_ID, "SAA: Strict (high-shield rate based), Tentacle: Figure-of-merit", ((F.m_Module == 0) ? "A" : "B"),
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAStrictHSRTentacleNo, RatesSAAStrictHSRTentacleNoByOrbit, TimeSAAStrictHSRTentacleNo,
               RatesSAAStrictHSRTentacleFoM, RatesSAAStrictHSRTentacleFoMByOrbit, TimeSAAStrictHSRTentacleFoM);
  
  CreateReport(F.m_ID, "SAA: Figure-of-merit (high-shield rate based), Tentacle: figure-of-merit", ((F.m_Module == 0) ? "A" : "B"),
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAOptimizedHSRFoMTentacleNo, RatesSAAOptimizedHSRFoMTentacleNoByOrbit, TimeSAAOptimizedHSRFoMTentacleNo,
               RatesSAAOptimizedHSRFoMTentacleFoM, RatesSAAOptimizedHSRFoMTentacleFoMByOrbit, TimeSAAOptimizedHSRFoMTentacleFoM);
  
  CreateReport(F.m_ID, "SAA: Strict (low-shield rate based), Tentacle: rms-based", ((F.m_Module == 0) ? "A" : "B"),
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAStrictLSRTentacleNo, RatesSAAStrictLSRTentacleNoByOrbit, TimeSAAStrictLSRTentacleNo,
               RatesSAAStrictLSRTentacleRMS, RatesSAAStrictLSRTentacleRMSByOrbit, TimeSAAStrictLSRTentacleRMS);
  
  CreateReport(F.m_ID, "SAA: Optimized rms-based (low-shield rate based), Tentacle: rms-based", ((F.m_Module == 0) ? "A" : "B"),
               RatesSAANoTentacleNo, RatesSAANoTentacleNoByOrbit, TimeSAANoTentacleNo,
               RatesSAAOptimizedLSRRMSTentacleNo, RatesSAAOptimizedLSRRMSTentacleNoByOrbit, TimeSAAOptimizedLSRRMSTentacleNo,
               RatesSAAOptimizedLSRRMSTentacleRMS, RatesSAAOptimizedLSRRMSTentacleRMSByOrbit, TimeSAAOptimizedLSRRMSTentacleRMS);
  
  CreateGTIs(H);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::CreateGTIs(NHousekeeping& H) 
{
  bool GTIOff_SAANoTentacleNo = false;
  vector<double> GTIStart_SAANoTentacleNo;
  vector<double> GTIStop_SAANoTentacleNo;
  
  bool GTIOff_SAAOptimizedLSRRMSTentacleNo = false;
  vector<double> GTIStart_SAAOptimizedLSRRMSTentacleNo;
  vector<double> GTIStop_SAAOptimizedLSRRMSTentacleNo;

  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (h == 0 && H.m_HardSAA[h] == true) {
      GTIStart_SAANoTentacleNo.push_back(H.m_Time[h]);
    }
    if (GTIOff_SAANoTentacleNo != H.m_HardSAA[h]) {
      if (GTIOff_SAANoTentacleNo == true) {
        GTIStart_SAANoTentacleNo.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAANoTentacleNo.size() < GTIStart_SAANoTentacleNo.size()) {
          GTIStop_SAANoTentacleNo.push_back(H.m_Time[h]);
        }
      }
      GTIOff_SAANoTentacleNo = !GTIOff_SAANoTentacleNo;
    }
    if (h == H.m_Time.size()-1 && GTIOff_SAANoTentacleNo == false && 
        GTIStop_SAANoTentacleNo.size() < GTIStart_SAANoTentacleNo.size()) {
      GTIStop_SAANoTentacleNo.push_back(H.m_Time[h]);
    }
      
    if (h == 0 && H.m_SoftSAAOptimizedLSRRMS[h] == true) {
      GTIStart_SAAOptimizedLSRRMSTentacleNo.push_back(H.m_Time[h]);
    }
    if (GTIOff_SAAOptimizedLSRRMSTentacleNo != H.m_SoftSAAOptimizedLSRRMS[h]) {
      if (GTIOff_SAAOptimizedLSRRMSTentacleNo == true) {
        GTIStart_SAAOptimizedLSRRMSTentacleNo.push_back(H.m_Time[h]);
      } else {
        if (GTIStop_SAAOptimizedLSRRMSTentacleNo.size() < GTIStart_SAAOptimizedLSRRMSTentacleNo.size()) {
          GTIStop_SAAOptimizedLSRRMSTentacleNo.push_back(H.m_Time[h]);
        }
      }
      GTIOff_SAAOptimizedLSRRMSTentacleNo = !GTIOff_SAAOptimizedLSRRMSTentacleNo;
    }
    if (h == H.m_Time.size()-1 && GTIOff_SAANoTentacleNo == false && 
        GTIStop_SAAOptimizedLSRRMSTentacleNo.size() < GTIStart_SAAOptimizedLSRRMSTentacleNo.size()) {
      GTIStop_SAAOptimizedLSRRMSTentacleNo.push_back(H.m_Time[h]);
    }
  }

  cout<<"Size start nono: "<<GTIStart_SAANoTentacleNo.size()<<endl;
  
  CreateGTIFile(GTIStart_SAANoTentacleNo, GTIStop_SAANoTentacleNo, H, "SAANoTentacleNo");
  CreateGTIFile(GTIStart_SAAOptimizedLSRRMSTentacleNo, GTIStop_SAAOptimizedLSRRMSTentacleNo, H, "SAAOptimizedLSRRMSTentacleNo");
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::NormalizeOrbit(TH2D* Rates, TH2D* Normalizer, int ShiftedMedian) 
{
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


void NCheckRates::CreateReport(TString Observation, TString Mode, TString Module, 
                              TH1D* RatesNoNo, TH2D* OrbitNoNo, double ObsTimeNoNo,
                              TH1D* RatesYesNo, TH2D* OrbitYesNo, double ObsTimeYesNo,
                              TH1D* RatesYesYes, TH2D* OrbitYesYes, double ObsTimeYesYes)
{
  // Define the Canvas
  double Scaler = 150;
  TString CanvasTitle = TString("Canvas_") + Mode + "_" + Module;
  TCanvas* Canvas = new TCanvas(CanvasTitle, "Canvas", 11*Scaler, 8.5*Scaler);
  
  double TitleSize = 0.05;
  double GapSize = 0.03; 
  
  double HeaderSize = 0.03;
  double BodySize = 0.25;
  double RatesWidth = 0.3;
  
  double Top = 0.0;
  double Bottom = 0.0;
  
  double TopMargin = 0.02;
  
  // Upper plot will be in pad1
  Canvas->cd();
  Top = 1.0;
  Bottom = Top - TitleSize;
  TPad* Title = new TPad("Title", "pTitlead1", 0, Bottom, 1, Top);
  Title->Draw();
  //Title->SetFillColor(2);
  Title->cd();
  TString TitleString;
  TitleString += "   Filter: ";
  TitleString += Mode;
  TitleString += "   Observation: ";
  TitleString += Observation;
  TitleString += "   Module: ";
  TitleString += Module;
  TText* T = new TText(.5, .5, TitleString);
  T->SetTextSize(0.45);
  T->SetTextAlign(22);
  T->Draw();
  
  
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleNoNo = new TPad("PadTitleNoNo", "PadTitleNoNo", 0, Bottom, 1, Top);
  //PadTitleNoNo->SetFillColor(3);
  PadTitleNoNo->Draw();
  PadTitleNoNo->cd(); 
  TText* TextNoNo = new TText(.02, .5, "Mode: SAA=OFF, Tentacle=OFF");
  TextNoNo->SetTextSize(0.6);
  TextNoNo->SetTextAlign(12);
  TextNoNo->Draw();
  ostringstream NoNoOut;
  NoNoOut<<"Effective exposure time: "<<ObsTimeNoNo<<" sec  (100.0%)";
  TText* TextTimesNoNo = new TText(.98, .5, NoNoOut.str().c_str());
  TextTimesNoNo->SetTextSize(0.6);
  TextTimesNoNo->SetTextAlign(32);
  TextTimesNoNo->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesNoNo = new TPad("PadRatesNoNo", "PadRatesNoNo", 0, Bottom, RatesWidth, Top);
  //PadRatesNoNo->SetFillColor(4);
  PadRatesNoNo->SetLeftMargin(0.15);
  PadRatesNoNo->SetTopMargin(TopMargin);
  PadRatesNoNo->SetBottomMargin(0.1);
  PadRatesNoNo->SetBottomMargin(0.15);
  PadRatesNoNo->Draw();
  PadRatesNoNo->cd();
  RatesNoNo->SetTitle("");
  RatesNoNo->GetXaxis()->SetNdivisions(505);
  RatesNoNo->GetXaxis()->SetLabelSize(0.05);
  RatesNoNo->GetXaxis()->SetTitleSize(0.05);
  RatesNoNo->GetXaxis()->SetTitleOffset(1.2);
  RatesNoNo->GetYaxis()->SetLabelSize(0.05);
  RatesNoNo->GetYaxis()->SetTitleSize(0.05);
  RatesNoNo->GetYaxis()->SetTitleOffset(1.2);
  RatesNoNo->Draw();
  
  Canvas->cd();         
  TPad* PadOrbitNoNo = new TPad("PadOrbitNoNo", "PadOrbitNoNo", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitNoNo->SetFillColor(5);
  PadOrbitNoNo->SetLeftMargin(0.08);
  PadOrbitNoNo->SetRightMargin(0.11);
  PadOrbitNoNo->SetTopMargin(TopMargin);
  PadOrbitNoNo->SetBottomMargin(0.15);
  PadOrbitNoNo->Draw();
  PadOrbitNoNo->cd(); 
  PadOrbitNoNo->SetLogz();
  if (OrbitNoNo->GetMinimum() < 0.2) OrbitNoNo->SetMinimum(0.2);
  OrbitNoNo->SetTitle("");
  OrbitNoNo->GetXaxis()->SetLabelSize(0.05);
  OrbitNoNo->GetXaxis()->SetTitleSize(0.05);
  OrbitNoNo->GetXaxis()->SetTitleOffset(1.2);
  OrbitNoNo->GetYaxis()->SetLabelSize(0.05);
  OrbitNoNo->GetYaxis()->SetTitleSize(0.05);
  OrbitNoNo->GetYaxis()->SetTitleOffset(0.5);
  OrbitNoNo->GetZaxis()->SetLabelSize(0.05);
  OrbitNoNo->GetZaxis()->SetLabelOffset(0.0);
  OrbitNoNo->GetZaxis()->SetTitleSize(0.05);
  OrbitNoNo->GetZaxis()->SetTitleOffset(0.5);
  OrbitNoNo->GetZaxis()->SetMoreLogLabels(true);
  OrbitNoNo->Draw("colz");
  TPaletteAxis* OrbitNoNoPalette = (TPaletteAxis*) OrbitNoNo->GetListOfFunctions()->FindObject("palette");
  if (OrbitNoNoPalette != 0) {
    OrbitNoNoPalette->SetX1NDC(0.9);
    OrbitNoNoPalette->SetX2NDC(0.93);
    OrbitNoNoPalette->SetY1NDC(0.15);
    OrbitNoNoPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitNoNo->Update();
  }
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleYesNo = new TPad("PadTitleYesNo", "PadTitleYesNo", 0, Bottom, 1, Top);
  PadTitleYesNo->Draw();
  PadTitleYesNo->cd(); 
  TText* TextYesNo = new TText(.02, .5, "Mode: SAA=ON, Tentacle=OFF");
  TextYesNo->SetTextSize(0.6);
  TextYesNo->SetTextAlign(12);
  TextYesNo->Draw();
  ostringstream YesNoOut;
  YesNoOut<<"Effective exposure time: "<<ObsTimeYesNo<<" sec  ("<<setprecision(4)<<100.0*ObsTimeYesNo/ObsTimeNoNo<<"%)";
  TText* TextTimesYesNo = new TText(.98, .5, YesNoOut.str().c_str());
  TextTimesYesNo->SetTextSize(0.6);
  TextTimesYesNo->SetTextAlign(32);
  TextTimesYesNo->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesYesNo = new TPad("PadRatesYesNo", "PadRatesYesNo", 0, Bottom, RatesWidth, Top);
  //PadRatesYesNo->SetFillColor(4);
  PadRatesYesNo->SetLeftMargin(0.15);
  PadRatesYesNo->SetTopMargin(TopMargin);
  PadRatesYesNo->SetBottomMargin(0.1);
  PadRatesYesNo->SetBottomMargin(0.15);
  PadRatesYesNo->Draw();
  PadRatesYesNo->cd();
  RatesYesNo->SetTitle("");
  RatesYesNo->GetXaxis()->SetNdivisions(505);
  RatesYesNo->GetXaxis()->SetLabelSize(0.05);
  RatesYesNo->GetXaxis()->SetTitleSize(0.05);
  RatesYesNo->GetXaxis()->SetTitleOffset(1.2);
  RatesYesNo->GetYaxis()->SetLabelSize(0.05);
  RatesYesNo->GetYaxis()->SetTitleSize(0.05);
  RatesYesNo->GetYaxis()->SetTitleOffset(1.2);
  RatesYesNo->Draw();
  
  Canvas->cd();         
  TPad* PadOrbitYesNo = new TPad("PadOrbitYesNo", "PadOrbitYesNo", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitYesNo->SetFillColor(5);
  PadOrbitYesNo->SetLeftMargin(0.08);
  PadOrbitYesNo->SetRightMargin(0.11);
  PadOrbitYesNo->SetTopMargin(TopMargin);
  PadOrbitYesNo->SetBottomMargin(0.15);
  PadOrbitYesNo->Draw();
  PadOrbitYesNo->cd(); 
  PadOrbitYesNo->SetLogz();
  if (OrbitYesNo->GetMinimum() < 0.2) OrbitYesNo->SetMinimum(0.2);
  OrbitYesNo->SetTitle("");
  OrbitYesNo->GetXaxis()->SetLabelSize(0.05);
  OrbitYesNo->GetXaxis()->SetTitleSize(0.05);
  OrbitYesNo->GetXaxis()->SetTitleOffset(1.2);
  OrbitYesNo->GetYaxis()->SetLabelSize(0.05);
  OrbitYesNo->GetYaxis()->SetTitleSize(0.05);
  OrbitYesNo->GetYaxis()->SetTitleOffset(0.5);
  OrbitYesNo->GetZaxis()->SetLabelSize(0.05);
  OrbitYesNo->GetZaxis()->SetLabelOffset(0.0);
  OrbitYesNo->GetZaxis()->SetTitleSize(0.05);
  OrbitYesNo->GetZaxis()->SetTitleOffset(0.5);
  OrbitYesNo->GetZaxis()->SetMoreLogLabels(true);
  OrbitYesNo->Draw("colz");
  TPaletteAxis* OrbitYesNoPalette = (TPaletteAxis*) OrbitYesNo->GetListOfFunctions()->FindObject("palette");
  if (OrbitYesNoPalette != 0) {
    OrbitYesNoPalette->SetX1NDC(0.9);
    OrbitYesNoPalette->SetX2NDC(0.93);
    OrbitYesNoPalette->SetY1NDC(0.15);
    OrbitYesNoPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitYesNo->Update();
  }
  
  
  Canvas->cd();   
  Top = Bottom - GapSize;
  Bottom = Top - HeaderSize;
  TPad* PadTitleYesYes = new TPad("PadTitleYesYes", "PadTitleYesYes", 0, Bottom, 1, Top);
  PadTitleYesYes->Draw();
  PadTitleYesYes->cd(); 
  TText* TextYesYes = new TText(.02, .5, "Mode: SAA=ON, Tentacle=ON");
  TextYesYes->SetTextSize(0.6);
  TextYesYes->SetTextAlign(12);
  TextYesYes->Draw();
  ostringstream YesYesOut;
  YesYesOut<<"Effective exposure time: "<<ObsTimeYesYes<<" sec  ("<<setprecision(4)<<100.0*ObsTimeYesYes/ObsTimeNoNo<<"%)";
  TText* TextTimesYesYes = new TText(.98, .5, YesYesOut.str().c_str());
  TextTimesYesYes->SetTextSize(0.6);
  TextTimesYesYes->SetTextAlign(32);
  TextTimesYesYes->Draw();
  
  Canvas->cd();         
  Top = Bottom;
  Bottom = Top - BodySize;
  TPad* PadRatesYesYes = new TPad("PadRatesYesYes", "PadRatesYesYes", 0, Bottom, RatesWidth, Top);
  //PadRatesYesYes->SetFillColor(4);
  PadRatesYesYes->SetLeftMargin(0.15);
  PadRatesYesYes->SetTopMargin(TopMargin);
  PadRatesYesYes->SetBottomMargin(0.1);
  PadRatesYesYes->SetBottomMargin(0.15);
  PadRatesYesYes->Draw();
  PadRatesYesYes->cd(); 
  RatesYesYes->SetTitle("");
  RatesYesYes->GetXaxis()->SetNdivisions(505);
  RatesYesYes->GetXaxis()->SetLabelSize(0.05);
  RatesYesYes->GetXaxis()->SetTitleSize(0.05);
  RatesYesYes->GetXaxis()->SetTitleOffset(1.2);
  RatesYesYes->GetYaxis()->SetLabelSize(0.05);
  RatesYesYes->GetYaxis()->SetTitleSize(0.05);
  RatesYesYes->GetYaxis()->SetTitleOffset(1.2);
  RatesYesYes->Draw();
  
  Canvas->cd();         
  TPad* PadOrbitYesYes = new TPad("PadOrbitYesYes", "PadOrbitYesYes", RatesWidth, Bottom, 1.0, Top);
  //PadOrbitYesYes->SetFillColor(5);
  PadOrbitYesYes->SetLeftMargin(0.08);
  PadOrbitYesYes->SetRightMargin(0.11);
  PadOrbitYesYes->SetTopMargin(TopMargin);
  PadOrbitYesYes->SetBottomMargin(0.15);
  PadOrbitYesYes->Draw();
  PadOrbitYesYes->cd(); 
  PadOrbitYesYes->SetLogz();
  if (OrbitYesYes->GetMinimum() < 0.2) OrbitYesYes->SetMinimum(0.2);
  OrbitYesYes->SetTitle("");
  OrbitYesYes->GetXaxis()->SetLabelSize(0.05);
  OrbitYesYes->GetXaxis()->SetTitleSize(0.05);
  OrbitYesYes->GetXaxis()->SetTitleOffset(1.2);
  OrbitYesYes->GetYaxis()->SetLabelSize(0.05);
  OrbitYesYes->GetYaxis()->SetTitleSize(0.05);
  OrbitYesYes->GetYaxis()->SetTitleOffset(0.5);
  OrbitYesYes->GetZaxis()->SetLabelSize(0.05);
  OrbitYesYes->GetZaxis()->SetLabelOffset(0.0);
  OrbitYesYes->GetZaxis()->SetTitleSize(0.05);
  OrbitYesYes->GetZaxis()->SetTitleOffset(0.5);
  OrbitYesYes->GetZaxis()->SetMoreLogLabels(true);
  OrbitYesYes->Draw("colz");
  TPaletteAxis* OrbitYesYesPalette = (TPaletteAxis*) OrbitYesYes->GetListOfFunctions()->FindObject("palette");
  if (OrbitYesYesPalette != 0) {
    OrbitYesYesPalette->SetX1NDC(0.9);
    OrbitYesYesPalette->SetX2NDC(0.93);
    OrbitYesYesPalette->SetY1NDC(0.15);
    OrbitYesYesPalette->SetY2NDC(1.0 - TopMargin);
    PadOrbitYesYes->Update();
  }
  // Mark highest pixel:
  
  TString Report = "Report_"  + Mode + "_id" + Observation + "_m" + Module + m_FileType;
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
  Rates->Draw();
  SAARegion->Scale(Rates->GetMaximum()/SAARegion->GetMaximum());
  SAARegion->DrawCopy("SAME");
  Rates->Draw("SAME");
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
  FileName += "_user_gti.fits";

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
    fits_write_col_dbl(File, Columns["START"], s, 1, 1, &GTIStart[s], &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing GTI failed (" << value << ")" << endl;
      fits_close_file(File, &status);
      File = 0;
      return false;
    }
  }

  for (unsigned int s = 1; s <= GTIStop.size(); ++s) {
    fits_write_col_dbl(File, Columns["STOP"], s, 1, 1, &GTIStop[s], &status);
    if (status != 0) {
      fits_get_errstatus(status, value);
      cerr << "Error: Writing GTI failed (" << value << ")" << endl;
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
