/*
 * NBackgroundTester.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundTester
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundTester.h"

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
ClassImp(NBackgroundTester)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundTester::NBackgroundTester()
{
  // Construct an instance of NBackgroundTester
  
  m_SpectrumMin = 3;
  m_SpectrumMax = 78;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundTester::~NBackgroundTester()
{
  // Delete this instance of NBackgroundTester
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundTester::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
  
  // Now parse the command line options:
  string Option;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "--srcbkg") {
      if (!((argc > i+8) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) &&
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) &&
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) &&
        (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) &&
        (argv[i+5][0] != '-' || isalpha(argv[i+5][1]) == 0) &&
        (argv[i+6][0] != '-' || isalpha(argv[i+6][1]) == 0) &&
        (argv[i+7][0] != '-' || isalpha(argv[i+7][1]) == 0) &&
        (argv[i+8][0] != '-' || isalpha(argv[i+8][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs 9 arguments!"<<endl;
        return false;
      }
    } else if (Option == "--srcdb") {
      if (!((argc > i+6) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0) &&
        (argv[i+2][0] != '-' || isalpha(argv[i+2][1]) == 0) &&
        (argv[i+3][0] != '-' || isalpha(argv[i+3][1]) == 0) &&
        (argv[i+4][0] != '-' || isalpha(argv[i+4][1]) == 0) &&
        (argv[i+5][0] != '-' || isalpha(argv[i+6][1]) == 0) &&
        (argv[i+6][0] != '-' || isalpha(argv[i+7][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs 6 arguments!"<<endl;
        return false;
      }
    }
    
    // Then fulfill the options:
    if (Option == "--srcbkg") {
      m_SBDirectory.push_back(argv[++i]);
      m_SBModule.push_back(argv[++i]);
      m_SBSourcePosX.push_back(atoi(argv[++i]));
      m_SBSourcePosX.push_back(atoi(argv[++i]));
      m_SBSourceSize.push_back(atoi(argv[++i]));
      m_SBBkgPosX.push_back(atoi(argv[++i]));
      m_SBBkgPosY.push_back(atoi(argv[++i]));
      m_SBBkgSize.push_back(atoi(argv[++i]));
      cout<<"Accepting source-bkg test: "<<m_SBDirectory.back()<<" - "<<m_SBModule.back()
        <<" - "<<m_SBSourcePosX.back()<<" - "<<m_SBSourcePosX.back()<<" - "<<m_SBSourceSize.back()<<" - "
        <<m_SBBkgPosX.back()<<" - "<<m_SBBkgPosY.back()<<" - "<<m_SBBkgSize.back()<<endl;
    } else if (Option == "--srcdb") {
      m_SDDirectory.push_back(argv[++i]);
      m_SDSourceModule.push_back(argv[++i]);
      m_SDSourcePosX.push_back(atoi(argv[++i]));
      m_SDSourcePosY.push_back(atoi(argv[++i]));
      m_SDSourceSize.push_back(atoi(argv[++i]));
      m_SDDBName.push_back(argv[++i]);
      cout<<"Accepting source-db test: "<<m_SDDirectory.back()<<" - "<<m_SDSourceModule.back()
        <<" - "<<m_SDSourcePosX.back()<<" - "<<m_SDSourcePosX.back()<<" - "<<m_SDSourceSize.back()<<" - "
        <<m_SDDBName.back()<<endl;
    }
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundTester::Analyze() 
{
  // First do src-bkg tests:
  
  for (unsigned int d = 0; d < m_SBDirectory.size(); ++d) {
    if (d > 0 && m_SBDirectory[d] == m_SBDirectory[d-1]) {
      cout<<"Directory already loaded :)"<<endl;
    } else {
      if (Load(m_SBDirectory[d]) == false) continue;
      // Calculate up time based on 01 data
      m_BadPixelA.CalculateUpTimes(m_FilteredEventsA01);
      m_BadPixelB.CalculateUpTimes(m_FilteredEventsB01);
    }
  
    if (m_SBModule[d] == "a") {
      Compare(m_FilteredEventsA01, m_HousekeepingA, m_BadPixelA, m_SBSourcePosX[d], m_SBSourcePosX[d], m_SBSourceSize[d], m_SBBkgPosX[d], m_SBBkgPosX[d], m_SBBkgSize[d]);
    } else if (m_SBModule[d] == "b") { 
      Compare(m_FilteredEventsB01, m_HousekeepingB, m_BadPixelB, m_SBSourcePosX[d], m_SBSourcePosX[d], m_SBSourceSize[d], m_SBBkgPosX[d], m_SBBkgPosX[d], m_SBBkgSize[d]);
    } else {
      cout<<"Unknown module: "<<m_SBModule[d]<<endl; 
    }
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundTester::Compare(NFilteredEvents& FE, NHousekeeping& H, NBadPixel& BP,
                               int SourcePosX, int SourcePosY, double SourceCutOff,
                               int BkgPosX, int BkgPosY, double BkgCutOff)
{
  cout<<"Creating a background using Tester 1..."<<endl;
    
  TString iID = "_BackgroundTester_id"; 
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
  TH1D* SpectrumSource = new TH1D(TString("SpectrumSource") + iID, 
                                  TString("SpectrumSource") + ID, int(m_SpectrumMax-m_SpectrumMin), m_SpectrumMin, m_SpectrumMax);
  SpectrumSource->SetXTitle("Energy [keV]");
  SpectrumSource->SetYTitle("cts");
  SpectrumSource->SetLineColor(kBlue);

  TH1D* SpectrumBackground = new TH1D(TString("SpectrumBackground") + iID, 
                                  TString("SpectrumBackground") + ID, int(m_SpectrumMax-m_SpectrumMin), m_SpectrumMin, m_SpectrumMax);
  SpectrumBackground->SetXTitle("Energy [keV]");
  SpectrumBackground->SetYTitle("cts");
  SpectrumBackground->SetLineColor(kRed);


  int NPositionBins = 64;

  TH2D* PositionsUpTime = new TH2D(TString("PositionsUpTime") + iID, TString("PositionsUpTime") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsUpTime->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsUpTime->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsUpTime->SetZTitle("cts");

  TH2D* PositionsSource = new TH2D(TString("PositionsSource") + iID, TString("PositionsSource") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsSource->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsSource->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsSource->SetZTitle("cts");

  TH2D* PositionsBackground = new TH2D(TString("PositionsBackground") + iID, TString("PositionsBackground") + ID, NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsBackground->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsBackground->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsBackground->SetZTitle("cts");

  for (unsigned int d = 0; d < 4; ++d) {
    for (unsigned int x = 0; x < 32; ++x) {
      for (unsigned int y = 0; y < 32; ++y) {
        double PosX = 0;
        double PosY = 0;
        ConvertRawPos(x, y, d, PosX, PosY);
        PositionsUpTime->Fill(PosX, PosY, BP.GetUpTime(d, x, y));
      }
    }
  }
  
  // Section B: Fill (and normalize) histograms

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
 
    /*
    int OrbitsIndex = O.FindClosestIndex(FE.m_Time[e]);
    if (OrbitsIndex == -1) {
      cout<<"Orbits: Index not found..."<<endl;
      continue;      
    }
    */
    
    int DetectorID = FE.m_DetectorID[e];
    int RawX = FE.m_RawX[e];
    int RawY = FE.m_RawY[e];
    
    double PosX = 0;
    double PosY = 0;
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);
    
    double SourceDistance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    double BkgDistance = sqrt((BkgPosX - PosX)*(BkgPosX - PosX) + (BkgPosY - PosY)*(BkgPosY - PosY));
    
    if (SourceDistance < SourceCutOff) {
      SpectrumSource->Fill(FE.m_Energy[e]);
      PositionsSource->Fill(PosX, PosY);
    }        
    if (BkgDistance < BkgCutOff) {
      SpectrumBackground->Fill(FE.m_Energy[e]);
      PositionsBackground->Fill(PosX, PosY);
    }        
  }
    
  TCanvas* PositionsUpTimeCanvas = new TCanvas(TString("PositionsUpTimeCanvas") + iID, TString("PositionsUpTimeCanvas") + ID, 600, 600);
  PositionsUpTimeCanvas->cd();
  PositionsUpTime->Draw("colz");
  PositionsUpTimeCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsUpTimeCanvas->SaveAs(PositionsUpTime->GetName() + m_FileType);
    
  TCanvas* PositionsSourceCanvas = new TCanvas(TString("PositionsSourceCanvas") + iID, TString("PositionsSourceCanvas") + ID, 600, 600);
  PositionsSourceCanvas->cd();
  PositionsSource->Draw("colz");
  PositionsSourceCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsSourceCanvas->SaveAs(PositionsSource->GetName() + m_FileType);
    
  TCanvas* PositionsBackgroundCanvas = new TCanvas(TString("PositionsBackgroundCanvas") + iID, TString("PositionsBackgroundCanvas") + ID, 600, 600);
  PositionsBackgroundCanvas->cd();
  PositionsBackground->Draw("colz");
  PositionsBackgroundCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsBackgroundCanvas->SaveAs(PositionsBackground->GetName() + m_FileType);
    
  return true;
}


// NBackgroundTester.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
