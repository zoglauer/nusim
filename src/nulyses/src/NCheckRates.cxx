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
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"

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

  double SpectrumMin = 3;
  double SpectrumMax = 120;  
    
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

  TH1D* ShieldRatesLow = new TH1D(TString("ShieldRatesLow") + iID, TString("ShieldRatesLow") + ID, RatesBins, MinTime, MaxTime);
  ShieldRatesLow->SetXTitle("Time [s]");
  ShieldRatesLow->SetYTitle("cts/sec");

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
  
  
  TH1D* RatesSAAStrictTentacleNo = new TH1D(TString("RatesSAAStrictTentacleNo") + iID, TString("Rates - SAA: Strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictTentacleNo->SetXTitle("Time [s]");
  RatesSAAStrictTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictTentacleNo = new TH1D(TString("LifeTimesSAAStrictTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAStrictTentacleNo->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedTentacleNo = new TH1D(TString("RatesSAAOptimizedTentacleNo") + iID, TString("Rates - SAA: Optimized, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedTentacleNo->SetXTitle("Time [s]");
  RatesSAAOptimizedTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedTentacleNo = new TH1D(TString("LifeTimesSAAOptimizedTentacleNo") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedTentacleNo->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedByLifeTimeTentacleNo = new TH1D(TString("RatesSAAOptimizedByLifeTimeTentacleNo") + iID, TString("Rates - SAA: Optimized (by life time), Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedByLifeTimeTentacleNo->SetXTitle("Time [s]");
  RatesSAAOptimizedByLifeTimeTentacleNo->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedByLifeTimeTentacleNo = new TH1D(TString("LifeTimesSAAOptimizedByLifeTimeTentacleNo") + iID, TString("LifeTimesSAAOptimizedByLifeTimeTentacleNo") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedByLifeTimeTentacleNo->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedByLifeTimeTentacleNo->SetYTitle("cts/sec");

  
  
  TH1D* RatesSAANoTentacleYes = new TH1D(TString("RatesSAANoTentacleYes") + iID, TString("Rates - SAA: No, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleYes->SetXTitle("Time [s]");
  RatesSAANoTentacleYes->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAANoTentacleYes = new TH1D(TString("LifeTimesSAANoTentacleYes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAANoTentacleYes->SetXTitle("Time [s]");
  LifeTimesSAANoTentacleYes->SetYTitle("cts/sec");

  
  TH1D* RatesSAAStrictTentacleYes = new TH1D(TString("RatesSAAStrictTentacleYes") + iID, TString("Rates - SAA: Strict, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictTentacleYes->SetXTitle("Time [s]");
  RatesSAAStrictTentacleYes->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAStrictTentacleYes = new TH1D(TString("LifeTimesSAAStrictTentacleYes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAStrictTentacleYes->SetXTitle("Time [s]");
  LifeTimesSAAStrictTentacleYes->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedTentacleYes = new TH1D(TString("RatesSAAOptimizedTentacleYes") + iID, TString("Rates - SAA: Optimized, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedTentacleYes->SetXTitle("Time [s]");
  RatesSAAOptimizedTentacleYes->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedTentacleYes = new TH1D(TString("LifeTimesSAAOptimizedTentacleYes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedTentacleYes->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedTentacleYes->SetYTitle("cts/sec");

  
  TH1D* RatesSAAOptimizedByLifeTimeTentacleYes = new TH1D(TString("RatesSAAOptimizedByLifeTimeTentacleYes") + iID, TString("Rates - SAA: Optimized (by life time), Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedByLifeTimeTentacleYes->SetXTitle("Time [s]");
  RatesSAAOptimizedByLifeTimeTentacleYes->SetYTitle("cts/sec");
  
  TH1D* LifeTimesSAAOptimizedByLifeTimeTentacleYes = new TH1D(TString("LifeTimesSAAOptimizedByLifeTimeTentacleYes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimesSAAOptimizedByLifeTimeTentacleYes->SetXTitle("Time [s]");
  LifeTimesSAAOptimizedByLifeTimeTentacleYes->SetYTitle("cts/sec");
  
  
  int LongitudeBins = 180;
  int LatitudeBins = 90;
  double MinLatitude = -6.2;
  double MaxLatitude = +6.2;
  
  TH2D* OrbitNormalizerDetectorSAANoTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleNoByOrbit = new TH2D(TString("RatesSAANoTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAANoTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedTentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedByLifeTimeTentacleNoByOrbit = new TH2D(TString("RatesSAAOptimizedByLifeTimeTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: OptimizedByLifeTime, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictTentacleNoByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Strict, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAANoTentacleYes = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleYes") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleYes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleYesByOrbit = new TH2D(TString("RatesSAANoTentacleYesByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: No, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleYesByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleYesByOrbit->SetYTitle("Latutude [deg]");
  RatesSAANoTentacleYesByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedTentacleYes = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedTentacleYes") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedTentacleYes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedTentacleYesByOrbit = new TH2D(TString("RatesSAAOptimizedTentacleYesByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Optimized, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedTentacleYesByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedTentacleYesByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedTentacleYesByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes = new TH2D(TString("OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes") + iID, 
                                   TString("OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAOptimizedByLifeTimeTentacleYesByOrbit = new TH2D(TString("RatesSAAOptimizedByLifeTimeTentacleYesByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: OptimizedByLifeTime, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictTentacleYes = new TH2D(TString("OrbitNormalizerDetectorSAAStrictTentacleYes") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictTentacleYes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictTentacleYesByOrbit = new TH2D(TString("RatesSAAStrictTentacleYesByOrbit") + iID, 
                                               TString("Rates by orbit - SAA: Strict, Tentacle: Yes") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictTentacleYesByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictTentacleYesByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictTentacleYesByOrbit->SetZTitle("cts/sec");
  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {

    if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(F.m_Time[e]) == true) continue;
    if (F.m_Energy[e] < SpectrumMin || F.m_Energy[e] > SpectrumMax) continue;

    
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
    
    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);

    double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    
    if (F.IsGTI(F.m_Time[e], true) == true && 
        F.m_Energy[e] > SpectrumMin && F.m_Energy[e] < SpectrumMax) {
      if (Distance > DistanceCutOff) {
        if (RawX >= 0 && RawX <= 31 && RawY >= 0 && RawY <= 31) {
          PositionsOnSource->Fill(PosX, PosY);
          
          RatesSAANoTentacleNo->Fill(F.m_Time[e]);
          RatesSAANoTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          if (H.m_SoftTentacled[HKIndex] == false) {
            RatesSAANoTentacleYes->Fill(F.m_Time[e]);
            RatesSAANoTentacleYesByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }
          
          if (H.m_SoftSAAStrict[HKIndex] == false) {
            RatesSAAStrictTentacleNo->Fill(F.m_Time[e]);
            RatesSAAStrictTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }
          if (H.m_SoftSAAStrict[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
            RatesSAAStrictTentacleYes->Fill(F.m_Time[e]);
            RatesSAAStrictTentacleYesByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }

          if (H.m_SoftSAAOptimized[HKIndex] == false) {
            RatesSAAOptimizedTentacleNo->Fill(F.m_Time[e]);
            RatesSAAOptimizedTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }
          if (H.m_SoftSAAOptimized[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
            RatesSAAOptimizedTentacleYes->Fill(F.m_Time[e]);
            RatesSAAOptimizedTentacleYesByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }

          if (H.m_SoftSAAOptimizedByLifeTime[HKIndex] == false) {
            RatesSAAOptimizedByLifeTimeTentacleNo->Fill(F.m_Time[e]);
            RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }
          if (H.m_SoftSAAOptimizedByLifeTime[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
            RatesSAAOptimizedByLifeTimeTentacleYes->Fill(F.m_Time[e]);
            RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
          }
        }
      }
    }
  }
  
    
  bool GTIOn = false;
  vector<double> GTIStart;
  vector<double> GTIStop;
  
  double TotalTime = 0;
  LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    LifeTimes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    
    int OrbitIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<H.m_Time[h]<<"..."<<endl;
      continue;      
    }

    if (O.m_Longitude[OrbitIndex] > 250 && O.m_Longitude[OrbitIndex] < 350) {
      SAARegion->SetBinContent(SAARegion->FindBin(H.m_Time[h]), 1); 
    }

    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    if (F.IsGTI(H.m_Time[h], true) == false) continue;

    ShieldRatesHigh->Fill(H.m_Time[h], H.m_ShieldRateHigh[h]);  
    ShieldRatesLow->Fill(H.m_Time[h], H.m_ShieldRateLow[h]);  
    ShieldRatesEntries->Fill(H.m_Time[h], 1);  
 
    OrbitNormalizerDetectorSAANoTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);
    LifeTimesSAANoTentacleNo->Fill(H.m_Time[h], H.m_LiveTime[h]);
    if (H.m_SoftTentacled[h] == false) {
      OrbitNormalizerDetectorSAANoTentacleYes->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAANoTentacleYes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }
          
    if (H.m_SoftSAAStrict[h] == false) {
      OrbitNormalizerDetectorSAAStrictTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAStrictTentacleNo->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }
    if (H.m_SoftSAAStrict[h] == false && H.m_SoftTentacled[h] == false) {
      OrbitNormalizerDetectorSAAStrictTentacleYes->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAStrictTentacleYes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }

    if (H.m_SoftSAAOptimized[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAOptimizedTentacleNo->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }
    if (H.m_SoftSAAOptimized[h] == false && H.m_SoftTentacled[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedTentacleYes->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAOptimizedTentacleYes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }

    if (H.m_SoftSAAOptimizedByLifeTime[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAOptimizedByLifeTimeTentacleNo->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }
    if (H.m_SoftSAAOptimizedByLifeTime[h] == false && H.m_SoftTentacled[h] == false) {
      OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);    
      LifeTimesSAAOptimizedByLifeTimeTentacleYes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    }
   
    
        
    if (O.m_SafelyOnSource[OrbitIndex] == true) {
      TotalTime += 1.0;
      LiveTime += H.m_LiveTime[h];
        
      if (GTIStop.size() > 0 && H.m_Time[h] - GTIStop.back() > 2.1) {
        GTIOn = false;
      }
      if (GTIOn == false) {
        GTIStart.push_back(H.m_Time[h]);
        GTIStop.push_back(H.m_Time[h]);
        GTIOn = true;
      } else {
        if (GTIStop.size() > 0) GTIStop[GTIStop.size() - 1] = H.m_Time[h]; 
      }
    }
  }
    
    
    
  // Normalize & set the minimum not to zero, but to the smallest no-zero entry for nicer plots:  
  double Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAANoTentacleNo->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAANoTentacleNo->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAANoTentacleNo->GetBinContent(lo, la) > 0 ) {
        RatesSAANoTentacleNoByOrbit->SetBinContent(lo, la, RatesSAANoTentacleNoByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAANoTentacleNo->GetBinContent(lo, la));
        if (RatesSAANoTentacleNoByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAANoTentacleNoByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAANoTentacleNoByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAANoTentacleNoByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAANoTentacleNoByOrbit->SetMinimum(Minimum);
    
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAOptimizedTentacleNo->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAOptimizedTentacleNo->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAOptimizedTentacleNo->GetBinContent(lo, la) > 0 ) {
        RatesSAAOptimizedTentacleNoByOrbit->SetBinContent(lo, la, RatesSAAOptimizedTentacleNoByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAOptimizedTentacleNo->GetBinContent(lo, la));
        if (RatesSAAOptimizedTentacleNoByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAOptimizedTentacleNoByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAOptimizedTentacleNoByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAOptimizedTentacleNoByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAOptimizedTentacleNoByOrbit->SetMinimum(Minimum);
    
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo->GetBinContent(lo, la) > 0 ) {
        RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetBinContent(lo, la, RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleNo->GetBinContent(lo, la));
        if (RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAOptimizedByLifeTimeTentacleNoByOrbit->SetMinimum(Minimum);
  
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAStrictTentacleNo->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAStrictTentacleNo->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAStrictTentacleNo->GetBinContent(lo, la) > 0 ) {
        RatesSAAStrictTentacleNoByOrbit->SetBinContent(lo, la, RatesSAAStrictTentacleNoByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAStrictTentacleNo->GetBinContent(lo, la));
        if (RatesSAAStrictTentacleNoByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAStrictTentacleNoByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAStrictTentacleNoByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAStrictTentacleNoByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAStrictTentacleNoByOrbit->SetMinimum(Minimum);
  
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAANoTentacleYes->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAANoTentacleYes->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAANoTentacleYes->GetBinContent(lo, la) > 0 ) {
        RatesSAANoTentacleYesByOrbit->SetBinContent(lo, la, RatesSAANoTentacleYesByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAANoTentacleYes->GetBinContent(lo, la));
        if (RatesSAANoTentacleYesByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAANoTentacleYesByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAANoTentacleYesByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAANoTentacleYesByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAANoTentacleYesByOrbit->SetMinimum(Minimum);
    
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAOptimizedTentacleYes->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAOptimizedTentacleYes->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAOptimizedTentacleYes->GetBinContent(lo, la) > 0 ) {
        RatesSAAOptimizedTentacleYesByOrbit->SetBinContent(lo, la, RatesSAAOptimizedTentacleYesByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAOptimizedTentacleYes->GetBinContent(lo, la));
        if (RatesSAAOptimizedTentacleYesByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAOptimizedTentacleYesByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAOptimizedTentacleYesByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAOptimizedTentacleYesByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAOptimizedTentacleYesByOrbit->SetMinimum(Minimum);
    
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes->GetBinContent(lo, la) > 0 ) {
        RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetBinContent(lo, la, RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAOptimizedByLifeTimeTentacleYes->GetBinContent(lo, la));
        if (RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAOptimizedByLifeTimeTentacleYesByOrbit->SetMinimum(Minimum);
  
  Minimum = numeric_limits<double>::max();
  for (int lo = 1; lo <= OrbitNormalizerDetectorSAAStrictTentacleYes->GetNbinsX(); ++lo) {
    for (int la = 1; la <= OrbitNormalizerDetectorSAAStrictTentacleYes->GetNbinsY(); ++la) {
      if (OrbitNormalizerDetectorSAAStrictTentacleYes->GetBinContent(lo, la) > 0 ) {
        RatesSAAStrictTentacleYesByOrbit->SetBinContent(lo, la, RatesSAAStrictTentacleYesByOrbit->GetBinContent(lo, la)/OrbitNormalizerDetectorSAAStrictTentacleYes->GetBinContent(lo, la));
        if (RatesSAAStrictTentacleYesByOrbit->GetBinContent(lo, la) < Minimum && 
            RatesSAAStrictTentacleYesByOrbit->GetBinContent(lo, la) > 0) {
          Minimum = RatesSAAStrictTentacleYesByOrbit->GetBinContent(lo, la);
        }
      } else {
        RatesSAAStrictTentacleYesByOrbit->SetBinContent(lo, la, 0);
      }
    }
  }
  RatesSAAStrictTentacleYesByOrbit->SetMinimum(Minimum);
  

     
  if (m_WriteGTI == true) {
    CreateGTIFile(GTIStart, GTIStop, H);
  }  
  
  /*
  TString GTIFileName = "GTI_"; 
  GTIFileName += F.m_ID;
  GTIFileName += ((F.m_Module == 0) ? "A" : "B");
  GTIFileName += ".txt";

  ofstream gti;
  gti.open(GTIFileName);
  gti.precision(10);
  for (unsigned int g = 0; g < GTIStart.size(); ++g) {
    gti<<GTIStart[g]<<" "<<GTIStop[g]<<endl;
  }
  gti.close();
  */
 
  // (2) Normalize
  double Max = 0;
  double CutOffLifeTimeFraction = 0.25; // In some bins we will only be partially on - skip those where we are below CutOffLifeTimeFraction on
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
    if (LifeTimesSAAOptimizedTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedTentacleNo->SetBinContent(b, RatesSAAOptimizedTentacleNo->GetBinContent(b)/RatesSAAOptimizedTentacleNo->GetBinWidth(b)/LifeTimesSAAOptimizedTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedByLifeTimeTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedByLifeTimeTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedByLifeTimeTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedByLifeTimeTentacleNo->SetBinContent(b, RatesSAAOptimizedByLifeTimeTentacleNo->GetBinContent(b)/RatesSAAOptimizedByLifeTimeTentacleNo->GetBinWidth(b)/LifeTimesSAAOptimizedByLifeTimeTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictTentacleNo->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictTentacleNo->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictTentacleNo->SetBinContent(b, 0);
    } else {
      RatesSAAStrictTentacleNo->SetBinContent(b, RatesSAAStrictTentacleNo->GetBinContent(b)/RatesSAAStrictTentacleNo->GetBinWidth(b)/LifeTimesSAAStrictTentacleNo->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAANoTentacleYes->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAANoTentacleYes->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAANoTentacleYes->SetBinContent(b, 0);
    } else {
      RatesSAANoTentacleYes->SetBinContent(b, RatesSAANoTentacleYes->GetBinContent(b)/RatesSAANoTentacleYes->GetBinWidth(b)/LifeTimesSAANoTentacleYes->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedTentacleYes->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedTentacleYes->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedTentacleYes->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedTentacleYes->SetBinContent(b, RatesSAAOptimizedTentacleYes->GetBinContent(b)/RatesSAAOptimizedTentacleYes->GetBinWidth(b)/LifeTimesSAAOptimizedTentacleYes->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAOptimizedByLifeTimeTentacleYes->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAOptimizedByLifeTimeTentacleYes->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAOptimizedByLifeTimeTentacleYes->SetBinContent(b, 0);
    } else {
      RatesSAAOptimizedByLifeTimeTentacleYes->SetBinContent(b, RatesSAAOptimizedByLifeTimeTentacleYes->GetBinContent(b)/RatesSAAOptimizedByLifeTimeTentacleYes->GetBinWidth(b)/LifeTimesSAAOptimizedByLifeTimeTentacleYes->GetBinContent(b));
    }
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimesSAAStrictTentacleYes->GetBinContent(b) == 0 || 
        LifeTimes->GetBinContent(b) == 0 || 
        LifeTimesSAAStrictTentacleYes->GetBinContent(b)/LifeTimes->GetBinContent(b) < CutOffLifeTimeFraction) {
      RatesSAAStrictTentacleYes->SetBinContent(b, 0);
    } else {
      RatesSAAStrictTentacleYes->SetBinContent(b, RatesSAAStrictTentacleYes->GetBinContent(b)/RatesSAAStrictTentacleYes->GetBinWidth(b)/LifeTimesSAAStrictTentacleYes->GetBinContent(b));
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
    
  TCanvas* ShieldRatesLowCanvas = new TCanvas();
  ShieldRatesLowCanvas->cd();
  ShieldRatesLow->Draw();
  ShieldRatesLowCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ShieldRatesLowCanvas->SaveAs(ShieldRatesLow->GetName() + m_FileType);

  TCanvas* PositionsOnSourceCanvas = new TCanvas(TString("PositionsOnSourceCanvas") + iID, TString("PositionsOnSourceCanvas") + ID, 600, 600);
  PositionsOnSourceCanvas->cd();
  PositionsOnSource->Draw("colz");
  PositionsOnSourceCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsOnSourceCanvas->SaveAs(PositionsOnSource->GetName() + m_FileType);

  TCanvas* LifeTimeCanvas = new TCanvas(TString("LifeTimeCanvas") + iID, TString("LifeTimeCanvas") + ID, 600, 600);
  LifeTimeCanvas->cd();
  LifeTimesSAANoTentacleNo->Draw();
  LifeTimeCanvas->Update();
  if (m_ShowHistograms.Contains("f")) LifeTimeCanvas->SaveAs(LifeTimesSAANoTentacleNo->GetName() + m_FileType);
  
  TString Title;
  double BaseLifeTime = LifeTimesSAANoTentacleNo->Integral();
  
  ShowRates(RatesSAANoTentacleNo, SAARegion, LifeTimesSAANoTentacleNo->Integral()/BaseLifeTime);
  ShowRates(RatesSAANoTentacleNoByOrbit, LifeTimesSAANoTentacleNo->Integral()/BaseLifeTime);
  
  ShowRates(RatesSAAOptimizedTentacleNo, SAARegion, LifeTimesSAAOptimizedTentacleNo->Integral()/BaseLifeTime);
  ShowRates(RatesSAAOptimizedTentacleNoByOrbit, LifeTimesSAAOptimizedTentacleNo->Integral()/BaseLifeTime);
  
  //ShowRates(RatesSAAOptimizedByLifeTimeTentacleNo, SAARegion, LifeTimesSAAOptimizedByLifeTimeTentacleNo->Integral()/BaseLifeTime);
  //ShowRates(RatesSAAOptimizedByLifeTimeTentacleNoByOrbit, LifeTimesSAAOptimizedByLifeTimeTentacleNo->Integral()/BaseLifeTime);
  
  ShowRates(RatesSAAStrictTentacleNo, SAARegion, LifeTimesSAAStrictTentacleNo->Integral()/BaseLifeTime);
  ShowRates(RatesSAAStrictTentacleNoByOrbit, LifeTimesSAAStrictTentacleNo->Integral()/BaseLifeTime);
  
  
  ShowRates(RatesSAANoTentacleYes, SAARegion, LifeTimesSAANoTentacleYes->Integral()/BaseLifeTime);
  ShowRates(RatesSAANoTentacleYesByOrbit, LifeTimesSAANoTentacleYes->Integral()/BaseLifeTime);
  
  ShowRates(RatesSAAOptimizedTentacleYes, SAARegion, LifeTimesSAAOptimizedTentacleYes->Integral()/BaseLifeTime);
  ShowRates(RatesSAAOptimizedTentacleYesByOrbit, LifeTimesSAAOptimizedTentacleYes->Integral()/BaseLifeTime);
  
  //ShowRates(RatesSAAOptimizedByLifeTimeTentacleYes, SAARegion, LifeTimesSAAOptimizedByLifeTimeTentacleYes->Integral()/BaseLifeTime);
  //ShowRates(RatesSAAOptimizedByLifeTimeTentacleYesByOrbit, LifeTimesSAAOptimizedByLifeTimeTentacleYes->Integral()/BaseLifeTime);
  
  ShowRates(RatesSAAStrictTentacleYes, SAARegion, LifeTimesSAAStrictTentacleYes->Integral()/BaseLifeTime);
  ShowRates(RatesSAAStrictTentacleYesByOrbit, LifeTimesSAAStrictTentacleYes->Integral()/BaseLifeTime);
  
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NCheckRates::ShowRates(TH1D* Rates, TH1D* SAARegion, double LifeTime)
{
  ostringstream out;
  out<<"#splitline{"<<Rates->GetTitle()<<"}{with life time relative to no cuts: "<<setprecision(4)<<LifeTime*100<<"%}";
  Rates->SetTitle(out.str().c_str());
  
  TCanvas* Canvas = new TCanvas();
  Canvas->cd();
  Rates->Draw();
  SAARegion->Draw("SAME");
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
  Orbit->Draw("colz");
  Canvas->Update();
  if (m_ShowHistograms.Contains("f")) Canvas->SaveAs(Orbit->GetName() + m_FileType);
}

  
////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H)
{
  // Copy template
  TString FileName = "nu";
  FileName += H.m_ID;
  FileName += ((H.m_Module == 0) ? "A" : "B");
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
