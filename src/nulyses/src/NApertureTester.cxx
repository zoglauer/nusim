/*
 * NApertureTester.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NApertureTester
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NApertureTester.h"

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
#include <TF2.h>

// MEGAlib libs:

// NuSTAR libs:
#include "NPhaFile.h"
#include "NApertureModel.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NApertureTester)
#endif


////////////////////////////////////////////////////////////////////////////////


NApertureTester::NApertureTester()
{
  // Construct an instance of NApertureTester
  
  m_InclusionMode = false;
  
  m_SpectrumMax = 200;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
}


////////////////////////////////////////////////////////////////////////////////


NApertureTester::~NApertureTester()
{
  // Delete this instance of NApertureTester
}


////////////////////////////////////////////////////////////////////////////////


bool NApertureTester::ParseCommandLine(int argc, char** argv)
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


bool NApertureTester::Analyze() 
{
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}

TH2D* g_Aperture;
Double_t ApertureFit(Double_t *x, Double_t *par) 
{
  double ApValue = g_Aperture->GetBinContent(g_Aperture->FindBin(x[0]), g_Aperture->FindBin(x[1]));
  if (ApValue == 0) return 0;
  return par[0] + par[1]*ApValue;
}   



////////////////////////////////////////////////////////////////////////////////


bool NApertureTester::Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                           int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Testing the aperture flux..."<<endl;
    
  SourcePosX = 225;
  SourcePosY = 242;
  DistanceCutOff = 0;
  
  m_SpectrumMin = 3.0-0.02;
  m_SpectrumMax = 10-0.02;  
  m_SpectrumBins = int((m_SpectrumMax - m_SpectrumMin)/0.04)/8;  
  
  TString iID = "_ApertureTester_id"; 
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
                                       m_SpectrumBins, m_SpectrumMin, m_SpectrumMax);
  SpectrumOnSourceAll->SetXTitle("Energy [keV]");
  SpectrumOnSourceAll->SetYTitle("cts");
  SpectrumOnSourceAll->SetLineColor(kBlue);


  double PosMin = 30.5;
  double PosMax = 340.5;
  int NPositionBins = (int(PosMax - PosMin) + 1);
  int NPositionDataBins = (int(PosMax - PosMin) + 1)/10;

  TH2D* PositionsBackground = new TH2D(TString("PositionsBackground") + iID, TString("PositionsBackground") + ID, 
                                       NPositionDataBins, PosMin, PosMax, NPositionDataBins, PosMin, PosMax);
  PositionsBackground->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  PositionsBackground->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  PositionsBackground->SetZTitle("cts");


  TH2D* BackgroundModel = new TH2D(TString("BackgroundModel") + iID, TString("BackgroundModel") + ID, 
                                       NPositionDataBins, PosMin, PosMax, NPositionDataBins, PosMin, PosMax);
  BackgroundModel->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  BackgroundModel->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  BackgroundModel->SetZTitle("cts");

  TH2D* Aperture = new TH2D(TString("Aperture") + iID, TString("Aperture") + ID, 
                                       NPositionBins, PosMin, PosMax, NPositionBins, PosMin, PosMax);
  Aperture->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  Aperture->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  Aperture->SetZTitle("cts");

  NApertureModel M;
  for (int bx = 0; bx <= Aperture->GetNbinsX(); ++bx) {
    for (int by = 0; by <= Aperture->GetNbinsX(); ++by) {
      double PosX = Aperture->GetBinCenter(bx);
      double PosY = Aperture->GetBinCenter(by);
      double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
      if (Distance > DistanceCutOff) {
        Aperture->SetBinContent(bx, by, M.GetApertureShape(bx, by, FE.m_Module));
      }
    }
  }

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
    int DetX = FE.m_Det1X[e];
    int DetY = FE.m_Det1Y[e];
    
    double PosX = DetX;
    double PosY = DetY;

    double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    
    SpectrumOnSourceAll->Fill(FE.m_Energy[e]);
    if (m_InclusionMode == true) {
      if (Distance < DistanceCutOff) {
        PositionsBackground->Fill(PosX, PosY);
      }        
    } else {
      if (Distance > DistanceCutOff) {
        PositionsBackground->Fill(PosX, PosY);
      }
    }    
  }
    
  
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
     
  
  TCanvas* SpectrumOnSourceAllCanvas = new TCanvas();
  SpectrumOnSourceAllCanvas->cd();
  SpectrumOnSourceAllCanvas->SetGridx();
  SpectrumOnSourceAllCanvas->SetGridy();
  SpectrumOnSourceAllCanvas->SetLogy();
  SpectrumOnSourceAll->Draw();
  SpectrumOnSourceAllCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SpectrumOnSourceAllCanvas->SaveAs(SpectrumOnSourceAll->GetName() + m_FileType);
    
  TCanvas* ApertureCanvas = new TCanvas(TString("ApertureCanvas") + iID, TString("ApertureCanvas") + ID, 600, 600);
  ApertureCanvas->cd();
  Aperture->Draw("colz");
  ApertureCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ApertureCanvas->SaveAs(Aperture->GetName() + m_FileType);
    
  TCanvas* PositionsBackgroundCanvas = new TCanvas(TString("PositionsBackgroundCanvas") + iID, TString("PositionsBackgroundCanvas") + ID, 600, 600);
  PositionsBackgroundCanvas->cd();
  PositionsBackground->Draw("colz");
  PositionsBackgroundCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsBackgroundCanvas->SaveAs(PositionsBackground->GetName() + m_FileType);
    
  g_Aperture = Aperture;
  
  TF2* ApFit = new TF2("ApFit", ApertureFit, PosMin, PosMax, PosMin, PosMax , 2);
  PositionsBackground->Fit(ApFit);
  PositionsBackground->Draw("colz");
  ApFit->Draw("cont1 same");
  
  cout<<"constant: "<<ApFit->GetParameter(0)<<" - scaler: "<<ApFit->GetParameter(1)<<endl;  
  //g_Aperture = 0;
  
  for (int bx = 0; bx <= BackgroundModel->GetNbinsX(); ++bx) {
    for (int by = 0; by <= BackgroundModel->GetNbinsX(); ++by) {
      BackgroundModel->SetBinContent(bx, by, ApFit->GetParameter(0) + ApFit->GetParameter(1)*
      Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(BackgroundModel->GetBinCenter(bx)), Aperture->GetYaxis()->FindBin(BackgroundModel->GetBinCenter(by))));
    }
  }
    
  TCanvas* BackgroundModelCanvas = new TCanvas(TString("BackgroundModelCanvas") + iID, TString("BackgroundModelCanvas") + ID, 600, 600);
  BackgroundModelCanvas->cd();
  BackgroundModel->Draw("colz");
  BackgroundModelCanvas->Update();
  if (m_ShowHistograms.Contains("f")) BackgroundModelCanvas->SaveAs(BackgroundModel->GetName() + m_FileType);
  
  
  return true;
}


// NApertureTester.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
