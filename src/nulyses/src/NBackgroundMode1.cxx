/*
 * NBackgroundMode1.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundMode1
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundMode1.h"

// Standard libs:
#include <fstream>
#include <limits>
using namespace std;

// ROOT libs:
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include <TFile.h>
#include <TKey.h>
#include <TIterator.h>
#include <TList.h>
#include <TClass.h>
#include <TROOT.h>

// MEGAlib libs:

// NuSTAR libs:
#include "NPhaFile.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NBackgroundMode1)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode1::NBackgroundMode1()
{
  // Construct an instance of NBackgroundMode1
  
  m_InclusionMode = false;
  
  m_SpectrumMax = 200;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode1::~NBackgroundMode1()
{
  // Delete this instance of NBackgroundMode1
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode1::ParseCommandLine(int argc, char** argv)
{
  if (NBackgroundModes::ParseCommandLine(argc, argv) == false) return false;
  
  // Now parse the command line options:
  string Option;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // Then fulfill the options:
    if (Option == "--inclusion-mode") {
      m_InclusionMode = true;
      cout<<"Accepting Inclusion-Mode (instead of default exclusion mode)"<<endl;
    }
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode1::Analyze() 
{
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    
    NPhaFile PhaA;
    if (m_LookAtModule.Contains("a")) {
      if (PhaA.Load(m_PhaA[d]) == false) return false;
    }
    NPhaFile PhaB;
    if (m_LookAtModule.Contains("b")) {
      if (PhaB.Load(m_PhaB[d]) == false) return false;
    }
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, PhaA, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, PhaB, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode1::Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       NPhaFile& P, int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Creating a background using mode 1..."<<endl;
    
  TString iID = "_BackgroundMode1_id"; 
  iID += FE.m_ID;
  iID += "_m";
  iID += ((FE.m_Module == 0) ? "A" : "B");
  iID += "_cl";
  iID += FE.m_CleanLevel;

  TString ID = " (id";
  ID += FE.m_ID;
  ID += "-cl0";
  ID += FE.m_CleanLevel;
  ID += "-m";
  ID += ((FE.m_Module == 0) ? "A" : "B");
  ID += ")";
  
  // Spectra:
  TH1D* SpectrumOnSourceAll = new TH1D(TString("SpectrumOnSourceAll") + iID, 
                                       TString("Spectrum all") + ID, 
                             P.m_Spectrum->GetNbinsX(), P.m_Spectrum->GetXaxis()->GetXmin(), P.m_Spectrum->GetXaxis()->GetXmax());
  SpectrumOnSourceAll->SetXTitle("Energy [keV]");
  SpectrumOnSourceAll->SetYTitle("cts");
  SpectrumOnSourceAll->SetLineColor(kBlue);
  
  // Spectra:
  TH1D* SpectrumHighRes = 0;
  TH1D* SpectrumOccHighRes = 0;
  TH1D* SpectrumNightHighRes = 0;
  SpectrumHighRes = new TH1D(TString("SpectrumHighRes") + iID, 
                             TString("Background spectrum - high res") + ID, 
                             P.m_Spectrum->GetNbinsX(), P.m_Spectrum->GetXaxis()->GetXmin(), P.m_Spectrum->GetXaxis()->GetXmax());
  SpectrumHighRes->SetXTitle("Energy [keV]");
  SpectrumHighRes->SetYTitle("cts");
  SpectrumHighRes->SetLineColor(kRed);

  SpectrumOccHighRes = new TH1D(TString("SpectrumOccHighRes") + iID, 
             TString("Background spectrum occ- high res") + ID, 
             P.m_Spectrum->GetNbinsX(), P.m_Spectrum->GetXaxis()->GetXmin(), P.m_Spectrum->GetXaxis()->GetXmax());
  SpectrumOccHighRes->SetXTitle("Energy [keV]");
  SpectrumOccHighRes->SetYTitle("cts");
  SpectrumOccHighRes->SetLineColor(kRed);

  SpectrumNightHighRes = new TH1D(TString("SpectrumNightHighRes") + iID, 
             TString("Background spectrum night - high res") + ID, 
             P.m_Spectrum->GetNbinsX(), P.m_Spectrum->GetXaxis()->GetXmin(), P.m_Spectrum->GetXaxis()->GetXmax());
  SpectrumNightHighRes->SetXTitle("Energy [keV]");
  SpectrumNightHighRes->SetYTitle("cts");
  SpectrumNightHighRes->SetLineColor(kRed);


  int NPositionBins = 64;

  TH2D* PositionsBackground = new TH2D(TString("PositionsBackground") + iID, TString("PositionsBackground") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsBackground->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsBackground->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsBackground->SetZTitle("cts");

  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < FE.m_Time.size(); ++e) {

    if (WithinSpecialGTI(FE.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(FE.m_Time[e]) == true) continue;
    if (FE.m_Energy[e] < m_SpectrumMin || FE.m_Energy[e] > m_SpectrumMax) continue;

    
    int HKIndex = H.FindClosestIndex(FE.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found..."<<endl;
      continue;      
    }
 
    int OrbitsIndex = O.FindClosestIndex(FE.m_Time[e]);
    if (OrbitsIndex == -1) {
      cout<<"Orbits: Index not found..."<<endl;
      continue;      
    }

    
    int DetectorID = FE.m_DetectorID[e];
    int RawX = FE.m_RawX[e];
    int RawY = FE.m_RawY[e];
    
    double PosX = 0;
    double PosY = 0;
    if (DetectorID == 0) {
      PosX = +RawX+0.5; 
      PosY = +RawY+0.5;
    } else if (DetectorID == 1) {
      PosX = +RawY+0.5; 
      PosY = -RawX-0.5;
    } else if (DetectorID == 2) {
      PosX = -RawX-0.5;
      PosY = -RawY-0.5;
    } else if (DetectorID == 3) {
      PosX = -RawY-0.5;
      PosY = +RawX+0.5;
    }
    double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    
    SpectrumOnSourceAll->Fill(FE.m_Energy[e]);
    if (m_InclusionMode == true) {
      if (Distance < DistanceCutOff) {
        if (RawX > 0 && RawX < 31 && RawY > 0 && RawY < 31) {
          SpectrumHighRes->Fill(FE.m_Energy[e]);
          PositionsBackground->Fill(PosX, PosY);
        }
      }        
    } else {
      if (Distance > DistanceCutOff) {
        if (RawX > 0 && RawX < 31 && RawY > 0 && RawY < 31) {
          SpectrumHighRes->Fill(FE.m_Energy[e]);
          PositionsBackground->Fill(PosX, PosY);
        }
      }
    }    
  }
  
  int NSourcePixels = 0;
  int NBackgroundPixels = 0;
  for (int bx = 1; bx <= PositionsBackground->GetNbinsX(); ++bx) {
    for (int by = 1; by <= PositionsBackground->GetNbinsY(); ++by) {
      double PosX = PositionsBackground->GetXaxis()->GetBinCenter(bx);
      double PosY = PositionsBackground->GetYaxis()->GetBinCenter(by);
      double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
      if (Distance > DistanceCutOff) {
        if (fabs(PosX) > 0 && fabs(PosX) < 31 && fabs(PosY) > 0 && fabs(PosY) < 31) {
          NBackgroundPixels++;
        }
      } else {
        NSourcePixels++;
      }
    }
  }
  cout<<"Source pixels: "<<NSourcePixels<<" background pixels: "<<NBackgroundPixels<<endl;
    
  
  double TotalTime = 0;
  LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;

    //if (FE.IsGTI(H.m_Time[h]) == true) {
    if (FE.IsGTI(H.m_Time[h], true) == true && H.m_SoftSAA[h] == false && H.m_SoftTentacled[h] == false) {

      int OrbitsIndex = O.FindClosestIndex(H.m_Time[h]);
      if (OrbitsIndex == -1) {
        cout<<"Orbits: Index not found..."<<endl;
        continue;      
      }

      if (O.m_SafelyOnSource[OrbitsIndex] == true) {
        TotalTime += 1.0;
        LiveTime += H.m_LiveTime[h];
      }
    }
  }
    

  
  cout<<"LiveTime on file: "<<FE.m_LiveTime<<endl;
  cout<<"Live time after GTI and SAA: "<<LiveTime<<" sec ("<<LiveTime/TotalTime*100.0<<"%)"<<endl;
     

  
  double ScalingMin = m_MinFitRange;
  double ScalingMax = m_MaxFitRange;
  cout<<"Using fit range: "<<m_MinFitRange<<" "<<m_MaxFitRange<<endl;
  cout<<"Spectrum on source flux above before lifetime [cts/sec/cm2]: "<<SpectrumOnSourceAll->Integral(SpectrumOnSourceAll->FindBin(ScalingMin), SpectrumOnSourceAll->FindBin(ScalingMax))/FE.m_DetectorArea<<endl;
  
  // (2) Normalize
  TH1D* SrcPha = P.m_Spectrum;
  
  double PhaSourceHighEnergyIntensity = SrcPha->Integral(SrcPha->FindBin(ScalingMin), SrcPha->FindBin(ScalingMax));
  double PhaBackgroundHighEnergyIntensity = SpectrumHighRes->Integral(SpectrumHighRes->FindBin(ScalingMin), SpectrumHighRes->FindBin(ScalingMax));
  double PhaScalerHighEnergy = PhaSourceHighEnergyIntensity/PhaBackgroundHighEnergyIntensity;
  cout<<"Pha scaler (high-energy): "<<PhaScalerHighEnergy<<endl;
  P.SaveBackground(SpectrumHighRes, PhaScalerHighEnergy, "mode1");

  SrcPha->Rebin(4);
  SpectrumHighRes->Scale(PhaScalerHighEnergy);
  SpectrumHighRes->Rebin(4);
    
  TCanvas* PhaSpectrum = new TCanvas();
  PhaSpectrum->cd();
  PhaSpectrum->SetGridx();
  PhaSpectrum->SetGridy();
  PhaSpectrum->SetLogy();
  SrcPha->Draw();
  SpectrumHighRes->Draw("SAME");
  PhaSpectrum->Update();
  if (m_ShowHistograms.Contains("f")) PhaSpectrum->SaveAs(TString("PhaMode1") + iID + m_FileType);
  
  TCanvas* SpectrumOnSourceAllCanvas = new TCanvas();
  SpectrumOnSourceAllCanvas->cd();
  SpectrumOnSourceAllCanvas->SetGridx();
  SpectrumOnSourceAllCanvas->SetGridy();
  SpectrumOnSourceAllCanvas->SetLogy();
  SpectrumOnSourceAll->Draw();
  SpectrumOnSourceAllCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SpectrumOnSourceAllCanvas->SaveAs(SpectrumOnSourceAll->GetName() + m_FileType);
    
  TCanvas* SpectrumHighResCanvas = new TCanvas();
  SpectrumHighResCanvas->cd();
  SpectrumHighResCanvas->SetGridx();
  SpectrumHighResCanvas->SetGridy();
  SpectrumHighResCanvas->SetLogy();
  SpectrumHighRes->Draw();
  SpectrumHighResCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SpectrumHighResCanvas->SaveAs(SpectrumHighRes->GetName() + m_FileType);
    
  TCanvas* PositionsBackgroundCanvas = new TCanvas(TString("PositionsBackgroundCanvas") + iID, TString("PositionsBackgroundCanvas") + ID, 600, 600);
  PositionsBackgroundCanvas->cd();
  PositionsBackground->Draw("colz");
  PositionsBackgroundCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsBackgroundCanvas->SaveAs(PositionsBackground->GetName() + m_FileType);
    
  return true;
}


// NBackgroundMode1.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
