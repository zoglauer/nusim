/*
 * NDetectorHealth.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NDetectorHealth
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NDetectorHealth.h"

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
ClassImp(NDetectorHealth)
#endif


////////////////////////////////////////////////////////////////////////////////


NDetectorHealth::NDetectorHealth()
{
  // Construct an instance of NDetectorHealth
  
  m_SpectrumMin = 1.6-0.02;
  m_SpectrumMax = 150.0-0.22;
  m_SpectrumBins = (int(m_SpectrumMax-m_SpectrumMin)/0.04)/4;
}


////////////////////////////////////////////////////////////////////////////////


NDetectorHealth::~NDetectorHealth()
{
  // Delete this instance of NDetectorHealth
}


////////////////////////////////////////////////////////////////////////////////


bool NDetectorHealth::ParseCommandLine(int argc, char** argv)
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


bool NDetectorHealth::Analyze() 
{  
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (m_LookAtModule.Contains("a")) {
      if (Load(m_Directories[d], "a") == false) continue;
      Show(m_FilteredEventsA, m_UnfilteredEventsA, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    }
    if (m_LookAtModule.Contains("b")) {
      if (Load(m_Directories[d], "b") == false) continue;
      Show(m_FilteredEventsB, m_UnfilteredEventsB, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
    }
  }
   
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NDetectorHealth::Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Showing unfiltered events..."<<endl;
  
  bool xIsLog = true;
  int xNBins = 500;
  double* xBins = new double[xNBins+1];
  double xDist = 0.0;
  double xMin = 0.0000001;
  double xMax = 5;
  
  if (xIsLog == true) {
    xMin = log(xMin);
    xMax = log(xMax);
    xDist = (xMax-xMin)/(xNBins);
    for (int i = 0; i < xNBins+1; ++i) {
      xBins[i] = exp(xMin+i*xDist);
    }  
  } else {
    xDist = (xMax-xMin)/(xNBins);
    for (int i = 0; i < xNBins+1; ++i) {
      xBins[i] = xMin+i*xDist;
    }  
  }
  
  bool tIsLog = false;
  int tNBins = 500;
  double* tBins = new double[tNBins+1];
  double tDist = 0.0;
  double tMin = 0.00001;
  double tMax = 0.0012;
  
  if (tIsLog == true) {
    tMin = log(tMin);
    tMax = log(tMax);
    tDist = (tMax-tMin)/(tNBins);
    for (int i = 0; i < tNBins+1; ++i) {
      tBins[i] = exp(tMin+i*tDist);
    }  
  } else {
    tDist = (tMax-tMin)/(tNBins);
    for (int i = 0; i < tNBins+1; ++i) {
      tBins[i] = tMin+i*tDist;
    }  
  }
  
  bool yIsLog = false;
  int yNBins = 194;
  double* yBins = new double[yNBins+1];
  double yDist = 0.0;
  double yMin = 3;
  double yMax = 100;
  
  if (yIsLog == true) {
    yMin = log(yMin);
    yMax = log(yMax);
    yDist = (yMax-yMin)/(yNBins);
    for (int i = 0; i < yNBins+1; ++i) {
      yBins[i] = exp(yMin+i*yDist);
    }  
  } else {
    yDist = (yMax-yMin)/(yNBins);
    for (int i = 0; i < yNBins+1; ++i) {
      yBins[i] = yMin+i*yDist;
    }  
  }
  
    
  TString iID = "_DetectorHealth_id"; 
  iID += U.m_ID;
  iID += "_m";
  iID += ((U.m_Module == 0) ? "A" : "B");

  TString ID = "  (ID: ";
  ID += U.m_ID;
  ID += ", Mod: ";
  ID += ((U.m_Module == 0) ? "A" : "B");
  ID += ")";
  
  TH1D* Coincidence = new TH1D(TString("Prior") + iID, TString("Prior") + ID, xNBins, xBins);
  Coincidence->SetXTitle("Time [sec]");
  Coincidence->SetYTitle("Counts [cts]");
  
  TH2D* PriorVsEnergy = new TH2D(TString("PriorVsEnergy") + iID, TString("Prior vs. energy") + ID, xNBins, xBins, yNBins, yBins);
  PriorVsEnergy->SetXTitle("Time [sec]");
  PriorVsEnergy->SetYTitle("Energy [keV]");
  PriorVsEnergy->SetZTitle("Counts [cts]");
  
  TH2D* ResetVsEnergy = new TH2D(TString("ResetVsEnergy") + iID, TString("Reset vs. energy") + ID, tNBins, tBins, yNBins, yBins);
  ResetVsEnergy->SetXTitle("Time [sec]");
  ResetVsEnergy->SetYTitle("Energy [keV]");
  ResetVsEnergy->SetZTitle("Counts [cts]");
  
  TH2D* BaselineVsEnergy = new TH2D(TString("BaselineVsEnergy") + iID, TString("Baseline vs. energy") + ID, 500, 0, 25000, 500, 0, 500);
  BaselineVsEnergy->SetXTitle("Baseline [a.u.]");
  BaselineVsEnergy->SetYTitle("Energy [keV]");
  BaselineVsEnergy->SetZTitle("Counts [cts]");
  
  TH1D* Baseline = new TH1D(TString("Baseline") + iID, TString("Baseline") + ID, 500, 0, 25000);
  Baseline->SetXTitle("Baseline [a.u.]");
  Baseline->SetYTitle("Energy [keV]");
  Baseline->SetZTitle("Counts [cts]");
  
  int RatesBins = U.GetMaxTime() - U.GetMinTime()+1;
  double MinTime = U.GetMinTime()-0.5;
  double MaxTime = U.GetMaxTime()+0.5;
  
  TH1D* Rates = new TH1D(TString("RatesNotVetoed") + iID, TString("Rates") + ID, RatesBins, MinTime, MaxTime);
  Rates->SetXTitle("Time [s]");
  Rates->SetYTitle("cts/sec");

  for (unsigned int i = 0; i < U.m_Time.size(); ++i) {
    // CP-mode cuts

    if (IsGoodEventByExternalDetectorEffectsFilter(U.m_Energy[i], U.m_Grade[i], U.m_ShieldVeto[i], U.m_Status[i]) == false) continue;
    if (IsGoodEventByExternalDepthFilter(U.m_Status[i]) == false) continue;
    
    Coincidence->Fill(U.m_Prior[i]);
    PriorVsEnergy->Fill(U.m_Prior[i], U.m_Energy[i]);
    ResetVsEnergy->Fill(U.m_Reset[i], U.m_Energy[i]);
    BaselineVsEnergy->Fill(U.m_BaseLine[i], U.m_Energy[i]);
    Baseline->Fill(U.m_BaseLine[i]);  
    Rates->Fill(U.m_Time[i]);
  }
   
  TCanvas* RatesCanvas = new TCanvas();
  RatesCanvas->cd();
  Rates->Draw();
  RatesCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesCanvas->SaveAs(Rates->GetName() + m_FileType);
  
    
  TCanvas* CoincidenceCanvas = new TCanvas();
  CoincidenceCanvas->cd();
  CoincidenceCanvas->SetLogx();
  CoincidenceCanvas->SetLogy();
  CoincidenceCanvas->SetGridx();
  Coincidence->Draw();
  CoincidenceCanvas->Update();
  if (m_ShowHistograms.Contains("f")) CoincidenceCanvas->SaveAs(Coincidence->GetName() + m_FileType);

    
  TCanvas* PriorVsEnergyCanvas = new TCanvas();
  PriorVsEnergyCanvas->cd();
  PriorVsEnergyCanvas->SetLogx();
  PriorVsEnergyCanvas->SetLogz();
  PriorVsEnergyCanvas->SetGridx();
  PriorVsEnergy->Draw("colz");
  PriorVsEnergyCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PriorVsEnergyCanvas->SaveAs(PriorVsEnergy->GetName() + m_FileType);
    
  TCanvas* ResetVsEnergyCanvas = new TCanvas();
  ResetVsEnergyCanvas->cd();
  if (tIsLog == true) {
    ResetVsEnergyCanvas->SetLogx();
    ResetVsEnergyCanvas->SetLogz();
  }
  ResetVsEnergyCanvas->SetGridx();
  ResetVsEnergy->Draw("colz");
  ResetVsEnergyCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ResetVsEnergyCanvas->SaveAs(ResetVsEnergy->GetName() + m_FileType);
    
  TCanvas* BaselineVsEnergyCanvas = new TCanvas();
  BaselineVsEnergyCanvas->cd();
  BaselineVsEnergyCanvas->SetGridx();
  BaselineVsEnergy->Draw("colz");
  BaselineVsEnergyCanvas->Update();
  if (m_ShowHistograms.Contains("f")) BaselineVsEnergyCanvas->SaveAs(BaselineVsEnergy->GetName() + m_FileType);
    
  TCanvas* BaseLineCanvas = new TCanvas();
  BaseLineCanvas->cd();
  BaseLineCanvas->SetGridx();
  Baseline->Draw();
  BaseLineCanvas->Update();
  if (m_ShowHistograms.Contains("f")) BaseLineCanvas->SaveAs(Baseline->GetName() + m_FileType);

  return true;
}


// NDetectorHealth.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
