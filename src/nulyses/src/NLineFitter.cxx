/*
 * NLineFitter.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NLineFitter
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NLineFitter.h"

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

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NLineFitter)
#endif


////////////////////////////////////////////////////////////////////////////////


NLineFitter::NLineFitter()
{
  // Construct an instance of NLineFitter
  
  m_SpectrumMin = 1.6-0.02;
  m_SpectrumMax = 150.0-0.22;
  m_SpectrumBins = (int(m_SpectrumMax-m_SpectrumMin)/0.04)/4;
  
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
}


////////////////////////////////////////////////////////////////////////////////


NLineFitter::~NLineFitter()
{
  // Delete this instance of NLineFitter
}


////////////////////////////////////////////////////////////////////////////////


bool NLineFitter::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  string Option;
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
       
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NLineFitter::Analyze() 
{  
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (m_LookAtModule.Contains("a")) {
      if (Load(m_Directories[d], "a") == false) continue;
      Show(m_FilteredEventsA01, m_UnfilteredEventsA, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    }
    if (m_LookAtModule.Contains("b")) {
      if (Load(m_Directories[d], "b") == false) continue;
      Show(m_FilteredEventsB01, m_UnfilteredEventsB, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
    }
  }
   
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NLineFitter::Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Fit all lines..."<<endl;
  
  // Section A: Create all histograms:
    
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
  
  TH1D* Spectrum = new TH1D(TString("Spectrum") + iID, 
                            TString("Spectrum - everything") + ID, 
                            m_SpectrumBins, m_SpectrumMin, m_SpectrumMax);
  Spectrum->SetXTitle("Energy [keV]");
  Spectrum->SetYTitle("cts/sec");
  Spectrum->SetLineColor(kBlue);
  
  double LineMin = 92-1;
  double LineMax = 92+1;
  
  int RatesBins = (H.GetMaxTime() - H.GetMinTime()+1)/(60*90);
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;
  
  TH1D* SpectrumLine = new TH1D(TString("SpectrumLine") + iID, 
                            TString("SpectrumLine - everything") + ID, 
                            RatesBins, MinTime, MaxTime);
  SpectrumLine->SetXTitle("Energy [keV]");
  SpectrumLine->SetYTitle("cts/sec");
  SpectrumLine->SetLineColor(kBlue);
  
  double BeforeLineMin = 97-3;
  double BeforeLineMax = 97+3;
  
  TH1D* SpectrumBeforeLine = new TH1D(TString("SpectrumBeforeLine") + iID, 
                            TString("SpectrumBeforeLine - everything") + ID, 
                            RatesBins, MinTime, MaxTime);
  SpectrumBeforeLine->SetXTitle("Energy [keV]");
  SpectrumBeforeLine->SetYTitle("cts/sec");
  SpectrumBeforeLine->SetLineColor(kBlue);
  
  TH1D* SpectrumRatio = new TH1D(TString("SpectrumRatio") + iID, 
                            TString("SpectrumRatio - everything") + ID, 
                            RatesBins, MinTime, MaxTime);
  SpectrumRatio->SetXTitle("Energy [keV]");
  SpectrumRatio->SetYTitle("cts/sec");
  SpectrumRatio->SetLineColor(kBlue);
    
  // Section B: Fill (and normalize) histograms
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < FE.m_Time.size(); ++e) {

    if (FE.IsGTI(FE.m_Time[e], true) == false) continue;
    if (WithinSpecialGTI(FE.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(FE.m_Time[e]) == true) continue;
    if (FE.m_Energy[e] < m_SpectrumMin || FE.m_Energy[e] > m_SpectrumMax) continue;

    
    int HKIndex = H.FindClosestIndex(FE.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found for time "<<FE.m_Time[e]<<"..."<<endl;
      continue;      
    }
    
    //if (IsGoodEventByExternalDetectorEffectsFilter(FE.m_Energy[e], FE.m_Grade[e], FE.m_ShieldVeto[e], FE.m_Status[e]) == false) continue;
    
    int OrbitIndex = O.FindClosestIndex(FE.m_Time[e]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<FE.m_Time[e]<<"..."<<endl;
      continue;      
    }

    if (FE.m_Energy[e] > m_SpectrumMin && FE.m_Energy[e] < m_SpectrumMax) {
      Spectrum->Fill(FE.m_Energy[e]);
    }
    if (FE.m_Energy[e] > LineMin && FE.m_Energy[e] < LineMax) {
      SpectrumLine->Fill(FE.m_Time[e]);
    }
    if (FE.m_Energy[e] > BeforeLineMin && FE.m_Energy[e] < BeforeLineMax) {
      SpectrumBeforeLine->Fill(FE.m_Time[e]);
    }
  }
  
  
  double TotalTime = 0;
  double LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (FE.IsGTI(H.m_Time[h], true) == false) continue;
    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;

    TotalTime += 1.0;
    LiveTime += H.m_LiveTime[h];
  }
 
  // (2) Normalize & Draw
  Spectrum->Scale(1.0/LiveTime);
  
  TCanvas* SpectrumCanvas = new TCanvas();
  SpectrumCanvas->cd();
  SpectrumCanvas->SetGridx();
  SpectrumCanvas->SetGridy();
  SpectrumCanvas->SetLogy();
  Spectrum->Draw();
  SpectrumCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumCanvas->SaveAs(Spectrum->GetName() + m_FileType);

  for (int b = 1; b <= RatesBins; ++b) {
    if (SpectrumBeforeLine->GetBinContent(b) > 0 && SpectrumLine->GetBinContent(b) > 0) {
      SpectrumRatio->SetBinContent(b, SpectrumLine->GetBinContent(b) / SpectrumBeforeLine->GetBinContent(b));
      SpectrumRatio->SetBinError(b, sqrt(1.0/SpectrumLine->GetBinContent(b) + 1.0/SpectrumBeforeLine->GetBinContent(b)));
    }
  }
  
  TCanvas* SpectrumRatioCanvas = new TCanvas();
  SpectrumRatioCanvas->cd();
  SpectrumRatioCanvas->SetGridx();
  SpectrumRatioCanvas->SetGridy();
  SpectrumRatioCanvas->SetLogy();
  SpectrumRatio->Draw();
  SpectrumRatioCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumRatioCanvas->SaveAs(SpectrumRatio->GetName() + m_FileType);
  
  return true;
}


// NLineFitter.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
