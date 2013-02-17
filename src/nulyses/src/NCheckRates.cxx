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
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA, m_UnfilteredEventsA, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB, m_UnfilteredEventsB, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NCheckRates::Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Checking rates..."<<endl;
  
  // Section A: Create all histograms:

  double SpectrumMin = 3;
  double SpectrumMax = 120;  
    
  TString iID = "_CheckRates_id"; 
  iID += FE.m_ID;
  iID += "_m";
  iID += ((FE.m_Module == 0) ? "A" : "B");
  iID += "_cl";
  iID += FE.m_CleanLevel;

  TString ID = "  (ID: ";
  ID += FE.m_ID;
  ID += ", C: L0";
  ID += FE.m_CleanLevel;
  ID += ", Mod: ";
  ID += ((FE.m_Module == 0) ? "A" : "B");
  ID += ")";
      
  
  int NPositionBins = 64;
  TH2D* PositionsOnSource = new TH2D(TString("PositionsOnSource") + iID, TString("Counts in used detector regions") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsOnSource->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsOnSource->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsOnSource->SetZTitle("cts");

  
  
  int RatesBins = (H.GetMaxTime() - H.GetMinTime()+1)/120;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;

  TH1D* Rates = new TH1D(TString("Rates") + iID, TString("Rates all") + ID, RatesBins, MinTime, MaxTime);
  Rates->SetXTitle("Time [s]");
  Rates->SetYTitle("cts/sec");

  TH1D* SAARegion = new TH1D(TString("SAARegion") + iID, TString("SAARegion") + ID, RatesBins, MinTime, MaxTime);
  SAARegion->SetXTitle("Time [s]");
  SAARegion->SetYTitle("cts/sec");
  SAARegion->SetLineColor(kYellow);
  SAARegion->SetFillColor(kYellow);
  
  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimes->SetXTitle("Time [s]");
  LifeTimes->SetYTitle("cts/sec");

  TH1D* RatesSAANoTentacleNo = new TH1D(TString("RatesSAANoTentacleNo") + iID, TString("Rates - SAA:No, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleNo->SetXTitle("Time [s]");
  RatesSAANoTentacleNo->SetYTitle("cts/sec");

  TH1D* RatesSAAStrictTentacleNo = new TH1D(TString("RatesSAAStrictTentacleNo") + iID, TString("Rates - SAA: Strict, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictTentacleNo->SetXTitle("Time [s]");
  RatesSAAStrictTentacleNo->SetYTitle("cts/sec");

  TH1D* RatesSAAOptimizedTentacleNo = new TH1D(TString("RatesSAAOptimizedTentacleNo") + iID, TString("Rates - SAA: Optimized, Tentacle: No") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedTentacleNo->SetXTitle("Time [s]");
  RatesSAAOptimizedTentacleNo->SetYTitle("cts/sec");

  TH1D* RatesSAANoTentacleYes = new TH1D(TString("RatesSAANoTentacleYes") + iID, TString("Rates - SAA: No, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAANoTentacleYes->SetXTitle("Time [s]");
  RatesSAANoTentacleYes->SetYTitle("cts/sec");

  TH1D* RatesSAAStrictTentacleYes = new TH1D(TString("RatesSAAStrictTentacleYes") + iID, TString("Rates - SAA: Strict, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAStrictTentacleYes->SetXTitle("Time [s]");
  RatesSAAStrictTentacleYes->SetYTitle("cts/sec");

  TH1D* RatesSAAOptimizedTentacleYes = new TH1D(TString("RatesSAAOptimizedTentacleYes") + iID, TString("Rates - SAA: Optimized, Tentacle: Yes") + ID, RatesBins, MinTime, MaxTime);
  RatesSAAOptimizedTentacleYes->SetXTitle("Time [s]");
  RatesSAAOptimizedTentacleYes->SetYTitle("cts/sec");
  
  
  int LongitudeBins = 180;
  int LatitudeBins = 90;
  double MinLatitude = -6.2;
  double MaxLatitude = +6.2;
  
  TH2D* OrbitNormalizerDetectorSAANoTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAANoTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAANoTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAANoTentacleNoByOrbit = new TH2D(TString("RatesSAANoTentacleNoByOrbit") + iID, 
                                               TString("Rates in orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAANoTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAANoTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAANoTentacleNoByOrbit->SetZTitle("cts/sec");
  
  TH2D* OrbitNormalizerDetectorSAAStrictTentacleNo = new TH2D(TString("OrbitNormalizerDetectorSAAStrictTentacleNo") + iID, 
                                   TString("OrbitNormalizerDetectorSAAStrictTentacleNo") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesSAAStrictTentacleNoByOrbit = new TH2D(TString("RatesSAAStrictTentacleNoByOrbit") + iID, 
                                               TString("Rates in orbit - SAA: Strict, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesSAAStrictTentacleNoByOrbit->SetXTitle("Longitude [deg]");
  RatesSAAStrictTentacleNoByOrbit->SetYTitle("Latutude [deg]");
  RatesSAAStrictTentacleNoByOrbit->SetZTitle("cts/sec");
  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < U.m_Time.size(); ++e) {

    if (WithinSpecialGTI(U.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(U.m_Time[e]) == true) continue;
    if (U.m_Energy[e] < SpectrumMin || U.m_Energy[e] > SpectrumMax) continue;

    
    int HKIndex = H.FindClosestIndex(U.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found for time "<<U.m_Time[e]<<"..."<<endl;
      continue;      
    }
    
    if (IsGoodEventByExternalDetectorEffectsFilter(U.m_Energy[e], U.m_Grade[e], U.m_ShieldVeto[e], U.m_Status[e]) == false) continue;
    if (IsGoodEventByExternalDepthFilter(U.m_Status[e]) == false) continue;
 
    int OrbitIndex = O.FindClosestIndex(U.m_Time[e]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<U.m_Time[e]<<"..."<<endl;
      continue;      
    }

    
    int DetectorID = U.m_DetectorID[e];
    int RawX = U.m_RawX[e];
    int RawY = U.m_RawY[e];
    
    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);

    double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    
    if (O.m_SafelyOnSource[OrbitIndex] == true && 
        FE.IsGTI(U.m_Time[e], true) == true && 
        U.m_Energy[e] > SpectrumMin && U.m_Energy[e] < SpectrumMax) {
      if (Distance > DistanceCutOff) {
        if (RawX >= 0 && RawX <= 31 && RawY >= 0 && RawY <= 31) {
          PositionsOnSource->Fill(PosX, PosY);
          
          RatesSAANoTentacleNo->Fill(U.m_Time[e]);
          if (H.m_SoftTentacled[HKIndex] == false) {
            RatesSAANoTentacleYes->Fill(U.m_Time[e]);
          }
          
          if (H.m_SoftSAAStrict[HKIndex] == false) {
            RatesSAAStrictTentacleNo->Fill(U.m_Time[e]);
          }
          if (H.m_SoftSAAStrict[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
            RatesSAAStrictTentacleYes->Fill(U.m_Time[e]);
          }

          if (H.m_SoftSAAOptimized[HKIndex] == false) {
            RatesSAAOptimizedTentacleNo->Fill(U.m_Time[e]);
          }
          if (H.m_SoftSAAOptimized[HKIndex] == false && H.m_SoftTentacled[HKIndex] == false) {
            RatesSAAOptimizedTentacleYes->Fill(U.m_Time[e]);
          }
        }
        RatesSAANoTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
        if (H.m_SoftSAAStrict[HKIndex] == false) {
          RatesSAAStrictTentacleNoByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]);    
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
    if (FE.IsGTI(H.m_Time[h], true) == false) continue;

    OrbitNormalizerDetectorSAANoTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);
    if (H.m_SoftSAAStrict[h] == false) {
      OrbitNormalizerDetectorSAAStrictTentacleNo->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);
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
  
  cout<<"Min: "<<Minimum<<endl;
     
  if (m_WriteGTI == true) {
    CreateGTIFile(GTIStart, GTIStop, H);
  }  
  
  /*
  TString GTIFileName = "GTI_"; 
  GTIFileName += U.m_ID;
  GTIFileName += ((U.m_Module == 0) ? "A" : "B");
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
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAANoTentacleNo->SetBinContent(b, RatesSAANoTentacleNo->GetBinContent(b)/RatesSAANoTentacleNo->GetBinWidth(b)/LifeTimes->GetBinContent(b));
    if (Max < RatesSAANoTentacleNo->GetBinContent(b)) {
      Max = RatesSAANoTentacleNo->GetBinContent(b);
    }
  }
  SAARegion->Scale(Max);
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAAOptimizedTentacleNo->SetBinContent(b, RatesSAAOptimizedTentacleNo->GetBinContent(b)/RatesSAAOptimizedTentacleNo->GetBinWidth(b)/LifeTimes->GetBinContent(b));
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAAStrictTentacleNo->SetBinContent(b, RatesSAAStrictTentacleNo->GetBinContent(b)/RatesSAAStrictTentacleNo->GetBinWidth(b)/LifeTimes->GetBinContent(b));
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAANoTentacleYes->SetBinContent(b, RatesSAANoTentacleYes->GetBinContent(b)/RatesSAANoTentacleYes->GetBinWidth(b)/LifeTimes->GetBinContent(b));
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAAOptimizedTentacleYes->SetBinContent(b, RatesSAAOptimizedTentacleYes->GetBinContent(b)/RatesSAAOptimizedTentacleYes->GetBinWidth(b)/LifeTimes->GetBinContent(b));
  }
  for (int b = 1; b <= RatesSAANoTentacleNo->GetNbinsX(); ++b) {
    if (LifeTimes->GetBinContent(b) == 0) continue;
    RatesSAAStrictTentacleYes->SetBinContent(b, RatesSAAStrictTentacleYes->GetBinContent(b)/RatesSAAStrictTentacleYes->GetBinWidth(b)/LifeTimes->GetBinContent(b));
  }
  
  
  TCanvas* PositionsOnSourceCanvas = new TCanvas(TString("PositionsOnSourceCanvas") + iID, TString("PositionsOnSourceCanvas") + ID, 600, 600);
  PositionsOnSourceCanvas->cd();
  PositionsOnSource->Draw("colz");
  PositionsOnSourceCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsOnSourceCanvas->SaveAs(PositionsOnSource->GetName() + m_FileType);
  
  TCanvas* RatesSAANoTentacleNoCanvas = new TCanvas();
  RatesSAANoTentacleNoCanvas->cd();
  RatesSAANoTentacleNo->Draw();
  SAARegion->Draw("SAME");
  RatesSAANoTentacleNo->Draw("SAME");
  RatesSAANoTentacleNoCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAANoTentacleNoCanvas->SaveAs(RatesSAANoTentacleNo->GetName() + m_FileType);
  
  TCanvas* RatesSAAOptimizedTentacleNoCanvas = new TCanvas();
  RatesSAAOptimizedTentacleNoCanvas->cd();
  RatesSAAOptimizedTentacleNo->Draw();
  SAARegion->Draw("SAME");
  RatesSAAOptimizedTentacleNo->Draw("SAME");
  RatesSAAOptimizedTentacleNoCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAAOptimizedTentacleNoCanvas->SaveAs(RatesSAAOptimizedTentacleNo->GetName() + m_FileType);
  
  TCanvas* RatesSAAStrictTentacleNoCanvas = new TCanvas();
  RatesSAAStrictTentacleNoCanvas->cd();
  RatesSAAStrictTentacleNo->Draw();
  SAARegion->Draw("SAME");
  RatesSAAStrictTentacleNo->Draw("SAME");
  RatesSAAStrictTentacleNoCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAAStrictTentacleNoCanvas->SaveAs(RatesSAAStrictTentacleNo->GetName() + m_FileType);
  
  TCanvas* RatesSAANoTentacleYesCanvas = new TCanvas();
  RatesSAANoTentacleYesCanvas->cd();
  RatesSAANoTentacleYes->Draw();
  SAARegion->Draw("SAME");
  RatesSAANoTentacleYes->Draw("SAME");
  RatesSAANoTentacleYesCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAANoTentacleYesCanvas->SaveAs(RatesSAANoTentacleYes->GetName() + m_FileType);
  
  TCanvas* RatesSAAOptimizedTentacleYesCanvas = new TCanvas();
  RatesSAAOptimizedTentacleYesCanvas->cd();
  RatesSAAOptimizedTentacleYes->Draw();
  SAARegion->Draw("SAME");
  RatesSAAOptimizedTentacleYes->Draw("SAME");
  RatesSAAOptimizedTentacleYesCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAAOptimizedTentacleYesCanvas->SaveAs(RatesSAAOptimizedTentacleYes->GetName() + m_FileType);
  
  TCanvas* RatesSAAStrictTentacleYesCanvas = new TCanvas();
  RatesSAAStrictTentacleYesCanvas->cd();
  RatesSAAStrictTentacleYes->Draw();
  SAARegion->Draw("SAME");
  RatesSAAStrictTentacleYes->Draw("SAME");
  RatesSAAStrictTentacleYesCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAAStrictTentacleYesCanvas->SaveAs(RatesSAAStrictTentacleYes->GetName() + m_FileType);
  
  TCanvas* RatesSAANoTentacleNoByOrbitCanvas = new TCanvas(TString("RatesSAANoTentacleNoByOrbitCanvas") + iID, TString("RatesSAANoTentacleNoByOrbitCanvas") + ID, 1200, 400);
  RatesSAANoTentacleNoByOrbitCanvas->cd();
  RatesSAANoTentacleNoByOrbitCanvas->SetLogz();
  RatesSAANoTentacleNoByOrbit->Draw("colz");
  RatesSAANoTentacleNoByOrbitCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAANoTentacleNoByOrbitCanvas->SaveAs(RatesSAANoTentacleNoByOrbit->GetName() + m_FileType);
  
  /*
  TCanvas* RatesSAAStrictTentacleNoByOrbitCanvas = new TCanvas(TString("RatesSAAStrictTentacleNoByOrbitCanvas") + iID, TString("RatesSAAStrictTentacleNoByOrbitCanvas") + ID, 1200, 400);
  RatesSAAStrictTentacleNoByOrbitCanvas->cd();
  RatesSAAStrictTentacleNoByOrbitCanvas->SetLogz();
  RatesSAAStrictTentacleNoByOrbit->Draw("colz");
  RatesSAAStrictTentacleNoByOrbitCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesSAAStrictTentacleNoByOrbitCanvas->SaveAs(RatesSAAStrictTentacleNoByOrbit->GetName() + m_FileType);
  */
  
  return true;
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
