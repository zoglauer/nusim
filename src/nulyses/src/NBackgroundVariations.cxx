/*
 * NBackgroundVariations.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundVariations
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundVariations.h"

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
ClassImp(NBackgroundVariations)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundVariations::NBackgroundVariations()
{
  // Construct an instance of NBackgroundVariations
  
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundVariations::~NBackgroundVariations()
{
  // Delete this instance of NBackgroundVariations
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundVariations::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  string Option;
  
  // Now parse the command line options:
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
       
    // Then fulfill the options:
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundVariations::Analyze() 
{
  double SpectrumMin = 2.0-0.02;
  double SpectrumMax = 150.0-0.02;  
  int SpectrumBins = (int(SpectrumMax-SpectrumMin)/0.04)/16;  
  
  m_SpectrumWafer0 = new TH1D(TString("m_SpectrumWafer0"), TString("Spectrum - background wafer 0"), SpectrumBins, SpectrumMin, SpectrumMax);
  m_SpectrumWafer0->SetXTitle("Energy [keV]");
  m_SpectrumWafer0->SetYTitle("norm. cts");
  m_SpectrumWafer0->SetLineColor(kRed);

  m_SpectrumWafer1 = new TH1D(TString("m_SpectrumWafer1"), TString("Spectrum - background wafer 1"), SpectrumBins, SpectrumMin, SpectrumMax);
  m_SpectrumWafer1->SetXTitle("Energy [keV]");
  m_SpectrumWafer1->SetYTitle("norm. cts");
  m_SpectrumWafer1->SetLineColor(kGreen+3);
  
  m_SpectrumWafer2 = new TH1D(TString("m_SpectrumWafer2"), TString("Spectrum - background wafer 2"), SpectrumBins, SpectrumMin, SpectrumMax);
  m_SpectrumWafer2->SetXTitle("Energy [keV]");
  m_SpectrumWafer2->SetYTitle("norm. cts");
  m_SpectrumWafer2->SetLineColor(kBlue);
  
  m_SpectrumWafer3 = new TH1D(TString("m_SpectrumWafer3"), TString("Spectrum - background wafer 3"), SpectrumBins, SpectrumMin, SpectrumMax);
  m_SpectrumWafer3->SetXTitle("Energy [keV]");
  m_SpectrumWafer3->SetYTitle("norm. cts");
  m_SpectrumWafer3->SetLineColor(kViolet);

  m_LifeTime = 0;
  
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  
  for (int b = 1; b <= m_SpectrumWafer0->GetNbinsX(); ++b) {
    m_SpectrumWafer0->SetBinError(b, sqrt(m_SpectrumWafer0->GetBinContent(b)));
    m_SpectrumWafer1->SetBinError(b, sqrt(m_SpectrumWafer1->GetBinContent(b)));
    m_SpectrumWafer2->SetBinError(b, sqrt(m_SpectrumWafer2->GetBinContent(b)));
    m_SpectrumWafer3->SetBinError(b, sqrt(m_SpectrumWafer3->GetBinContent(b)));
  }
  
  // Normalize
  double ScalingMin = 22;
  double ScalingMax = 32;
  
  double Wafer0 = m_SpectrumWafer0->Integral(m_SpectrumWafer0->FindBin(ScalingMin), m_SpectrumWafer0->FindBin(ScalingMax));
  double Wafer1 = m_SpectrumWafer1->Integral(m_SpectrumWafer1->FindBin(ScalingMin), m_SpectrumWafer1->FindBin(ScalingMax));
  double Wafer2 = m_SpectrumWafer2->Integral(m_SpectrumWafer2->FindBin(ScalingMin), m_SpectrumWafer2->FindBin(ScalingMax));
  double Wafer3 = m_SpectrumWafer3->Integral(m_SpectrumWafer3->FindBin(ScalingMin), m_SpectrumWafer3->FindBin(ScalingMax));
  
  cout<<"Scaler for 1: "<<Wafer0/Wafer1<<endl;
  cout<<"Scaler for 2: "<<Wafer0/Wafer2<<endl;
  cout<<"Scaler for 3: "<<Wafer0/Wafer3<<endl;
  
  m_SpectrumWafer1->Scale(Wafer0/Wafer1);
  m_SpectrumWafer2->Scale(Wafer0/Wafer2);
  m_SpectrumWafer3->Scale(Wafer0/Wafer3);
  
  TCanvas* SpectrumWafersCanvas = new TCanvas();
  SpectrumWafersCanvas->cd();
  m_SpectrumWafer0->SetTitle("Normalized spectrum per wafer (Red: 0, Green: 1, Blue: 2, Violet: 3)");
  m_SpectrumWafer0->Draw();
  m_SpectrumWafer1->Draw("SAME");
  m_SpectrumWafer2->Draw("SAME");
  m_SpectrumWafer3->Draw("SAME");
  SpectrumWafersCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumWafersCanvas->SaveAs(m_SpectrumWafer0->GetName() + m_FileType);
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundVariations::Show(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Quick view of the data..."<<endl;
  
  // Section A: Create all histograms:

  
  // Section B: Fill (and normalize) histograms
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {

    if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(F.m_Time[e]) == true) continue;

    
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
    
    if (F.IsGTI(F.m_Time[e], true) == true) {
      if (Distance > DistanceCutOff) {
        if (RawX == 0 || RawX == 31 || RawY == 0 || RawY == 31) continue;
        if (DetectorID == 0) {
          m_SpectrumWafer0->Fill(F.m_Energy[e]);
        } else if (DetectorID == 1) {
          m_SpectrumWafer1->Fill(F.m_Energy[e]);
        } else if (DetectorID == 2) {
          m_SpectrumWafer2->Fill(F.m_Energy[e]);
        } else if (DetectorID == 3) {
          m_SpectrumWafer3->Fill(F.m_Energy[e]);
        }
      }
    }
  }
  
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    if (F.IsGTI(H.m_Time[h], true) == false) continue;

    m_LifeTime += H.m_LiveTime[h];
  }
 
  
  return true;
}

// NBackgroundVariations.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
