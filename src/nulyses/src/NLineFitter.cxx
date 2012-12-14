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
#include "TF1.h"

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
  m_SpectrumMax = 150.0-0.2;
  m_SpectrumBins = (int(m_SpectrumMax-m_SpectrumMin)/0.04)/4;
  
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
  
  m_ApplySAAFilters = true; 
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

Double_t VariationFit(Double_t *x, Double_t *par) 
{    
  return par[0];
}   


bool NLineFitter::Analyze() 
{
  if (m_Directories.size() != 1) {
    cout<<"This tool works only for exactly one observation!"<<endl;
    return false;
  }

  if (Load(m_Directories[0], "a") == false) return false;

    
  TString iID = "_LineFitter_id"; 
  iID += m_FilteredEventsA01.m_ID;
  iID += "_m";
  iID += ((m_FilteredEventsA01.m_Module == 0) ? "A" : "B");
  iID += "_cl";
  iID += m_FilteredEventsA01.m_CleanLevel;

  TString ID = "  (ID: ";
  ID += m_FilteredEventsA01.m_ID;
  ID += ", C: L0";
  ID += m_FilteredEventsA01.m_CleanLevel;
  ID += ", Mod: ";
  ID += ((m_FilteredEventsA01.m_Module == 0) ? "A" : "B");
  ID += ")";
  
  int RatesBins = (m_HousekeepingA.GetMaxTime() - m_HousekeepingA.GetMinTime()+1)/(60*90);
  double MinTime = m_HousekeepingA.GetMinTime()-0.5;
  double MaxTime = m_HousekeepingA.GetMaxTime()+0.5;

  // Geo-cutoff:
  int BinsGeoCutOff = 8;
  double MinGeoCutOff = m_Orbits.GetMinGeomagneticCutOff();
  double MaxGeoCutOff = m_Orbits.GetMaxGeomagneticCutOff();

  TH1D* LifeTime = new TH1D(TString("LifeTime") + iID, 
                            TString("LifeTime") + ID, 
                            RatesBins, MinTime, MaxTime);

  TH1D* LowShieldRate = new TH1D(TString("LowShieldRate") + iID, 
                            TString("LowShieldRate") + ID, 
                            RatesBins, MinTime, MaxTime);
  
  TH2D* Spectrum = new TH2D(TString("SpectrumVsRate") + iID, 
                            TString("Spectrum vs. rate") + ID, 
                            m_SpectrumBins, m_SpectrumMin, m_SpectrumMax,
                            RatesBins, MinTime, MaxTime);
  Spectrum->SetXTitle("Energy [keV]");
  Spectrum->SetYTitle("Time [sec]");
  Spectrum->SetZTitle("cts");
  Spectrum->SetLineColor(kBlue);
  
  TH1D * GeomagneticCutOff = new TH1D(TString("GeomagneticCutOff") + iID, 
                                      TString("Filtered event rate by geomagnetic cut off") + ID, 
                                      BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff);
  GeomagneticCutOff->SetXTitle("Geomagnetic cutoff [GV]");
  GeomagneticCutOff->SetYTitle("cts");
  TH1D* GeomagneticCutOffLifetime = new TH1D(TString("GeomagneticCutOffLifetime") + iID, 
                                                 TString("Filtered event rate by geomagnetic cut off") + ID, 
                                                 BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff);

  
  
  Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, m_DetPosXA[0], m_DetPosYA[0], m_DetSizeA[0], LifeTime, Spectrum, GeomagneticCutOff, GeomagneticCutOffLifetime);
  
  if (Load(m_Directories[0], "b") == false) return false;
  Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, m_DetPosXB[0], m_DetPosYB[0], m_DetSizeB[0], LifeTime, Spectrum, GeomagneticCutOff, GeomagneticCutOffLifetime);
  

  for (int g = 1; g <= BinsGeoCutOff; ++g) {
    if (GeomagneticCutOffLifetime->GetBinContent(g) > 0) {  
      GeomagneticCutOff->SetBinContent(g, GeomagneticCutOff->GetBinContent(g)/GeomagneticCutOffLifetime->GetBinContent(g));
    }
  }

  
  TCanvas* GeomagneticCutOffCanvas = new TCanvas(TString("GeomagneticCutOffCanvas") + iID, "GeomagneticCutOffCanvas");
  GeomagneticCutOffCanvas->cd();
  GeomagneticCutOff->Draw();
  GeomagneticCutOffCanvas->Update();
  if (m_ShowHistograms.Contains("f")) GeomagneticCutOffCanvas->SaveAs(GeomagneticCutOff->GetName() + m_FileType);
    
  TCanvas* LifeTimeCanvas = new TCanvas(TString("LifeTimeCanvas") + iID, "LifeTimeCanvas");
  LifeTimeCanvas->cd();
  LifeTime->Draw();
  LifeTimeCanvas->Update();
  if (m_ShowHistograms.Contains("f")) LifeTimeCanvas->SaveAs(LifeTime->GetName() + m_FileType);
    
  
  TCanvas* SpectrumCanvas = new TCanvas();
  SpectrumCanvas->cd();
  SpectrumCanvas->SetGridx();
  SpectrumCanvas->SetGridy();
  SpectrumCanvas->SetLogz();
  Spectrum->Draw("colz");
  SpectrumCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumCanvas->SaveAs(Spectrum->GetName() + m_FileType);

  vector<TString> Names;
  vector<double> LineMin;
  vector<double> LineMax;
  vector<double> BackMin;
  vector<double> BackMax;
  
  Names.push_back("Unknown");
  LineMin.push_back(20.8);
  LineMax.push_back(23);  
  BackMin.push_back(33);
  BackMax.push_back(37);
  
  Names.push_back("{105}^Cd, {106}^Ag");
  LineMin.push_back(23.7);
  LineMax.push_back(25.3);  
  BackMin.push_back(33);
  BackMax.push_back(37);
  
  Names.push_back("Cs Fl.");
  LineMin.push_back(27.8);
  LineMax.push_back(28.8);  
  BackMin.push_back(33);
  BackMax.push_back(37);
  
  Names.push_back("I Fl.");
  LineMin.push_back(29.7);
  LineMax.push_back(31.3);  
  BackMin.push_back(33);
  BackMax.push_back(37);
  
  
  Names.push_back("Unknown");
  LineMin.push_back(20.8);
  LineMax.push_back(23);  
  BackMin.push_back(41);
  BackMax.push_back(45);
  
  Names.push_back("{105}^Cd, {106}^Ag");
  LineMin.push_back(23.7);
  LineMax.push_back(25.3);  
  BackMin.push_back(41);
  BackMax.push_back(45);
  
  Names.push_back("Cs Fl.");
  LineMin.push_back(27.8);
  LineMax.push_back(28.8);  
  BackMin.push_back(41);
  BackMax.push_back(45);
  
  Names.push_back("I Fl.");
  LineMin.push_back(29.7);
  LineMax.push_back(31.3);  
  BackMin.push_back(41);
  BackMax.push_back(45);
  
  Names.push_back("{125}^Cs");
  LineMin.push_back(38);
  LineMax.push_back(40);  
  BackMin.push_back(41);
  BackMax.push_back(45);
  
  Names.push_back("{101}^Pd");
  LineMin.push_back(46);
  LineMax.push_back(48);  
  BackMin.push_back(42);
  BackMax.push_back(45);
  
  Names.push_back("{125}^I");
  LineMin.push_back(65.5);
  LineMax.push_back(67.5);  
  BackMin.push_back(70);
  BackMax.push_back(74);
  
  Names.push_back("{119}^Sb");
  LineMin.push_back(50.8);
  LineMax.push_back(53.3);  
  BackMin.push_back(55);
  BackMax.push_back(59);
  
  Names.push_back("{119}^Sb");
  LineMin.push_back(50.8);
  LineMax.push_back(53.3);  
  BackMin.push_back(55);
  BackMax.push_back(59);
  
  Names.push_back("{109}^Cd");
  LineMin.push_back(86.8);
  LineMax.push_back(88.2);  
  BackMin.push_back(89);
  BackMax.push_back(91);
  
  Names.push_back("{107}^Cd");
  LineMin.push_back(91);
  LineMax.push_back(93);  
  BackMin.push_back(95);
  BackMax.push_back(99);
  
  Names.push_back("Reference");
  LineMin.push_back(53);
  LineMax.push_back(57);  
  BackMin.push_back(57);
  BackMax.push_back(63);
  
  Names.push_back("Reference");
  LineMin.push_back(100);
  LineMax.push_back(110);  
  BackMin.push_back(110);
  BackMax.push_back(120);
  
  Names.push_back("Reference");
  LineMin.push_back(100);
  LineMax.push_back(120);  
  BackMin.push_back(120);
  BackMax.push_back(140);
  
  Names.push_back("Reference");
  LineMin.push_back(100);
  LineMax.push_back(160);  
  BackMin.push_back(120);
  BackMax.push_back(140);
  
  for (unsigned int l = 0; l < LineMin.size(); ++l) {
    TString Name = Names[l];
    Name += ": Line from ";
    Name += LineMin[l];
    Name += " keV to ";
    Name += LineMax[l];
    Name += " keV with Bkg from ";
    Name += BackMin[l];
    Name += " keV to ";
    Name += BackMax[l];
    Name += " keV!";
    TString Title("LineFrom");
    Title += LineMin[l];
    Title += "keVto";
    Title += LineMax[l];
    Title += "keVwithBkgfrom";
    Title += BackMin[l];
    Title += "keVto";
    Title += BackMax[l];
    Title += "keV";
    TH1D* Line = new TH1D(Title, Name, RatesBins, MinTime, MaxTime);
    Line->SetXTitle("Time [sec]");
    Line->SetYTitle("#frac{N_{Line}-N_{Bkg}}{N_{Bkg}}");
    
    
    for (int t = 1; t <= Spectrum->GetNbinsY(); ++t) {
      double LineCounts = 0;
      double BackgroundCounts = 0;
      for (int e = 1; e <= Spectrum->GetNbinsX(); ++e) {
        if (Spectrum->GetXaxis()->GetBinCenter(e) >= LineMin[l] && Spectrum->GetXaxis()->GetBinCenter(e) <= LineMax[l]) {
          LineCounts += Spectrum->GetBinContent(e, t);  
        }
        if (Spectrum->GetXaxis()->GetBinCenter(e) >= BackMin[l] && Spectrum->GetXaxis()->GetBinCenter(e) <= BackMax[l]) {
          BackgroundCounts += Spectrum->GetBinContent(e, t);
        }
      }
      //cout<<LineCounts<<":"<<BackgroundCounts<<endl;
      if (BackgroundCounts == 0 || LineCounts == 0) continue;
      //double Value = LineCounts/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t);
      //Line->SetBinContent(t, Value);
      //Line->SetBinError(t, sqrt(LineCounts)/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t));
 
      //double Value = (LineCounts/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t)) / (BackgroundCounts/(BackMax[l]-BackMin[l])/LifeTime->GetBinContent(t));
      //Line->SetBinContent(t, Value);
      //Line->SetBinError(t, Value*sqrt(1.0/BackgroundCounts + 1.0/LineCounts));
 
      double Value = (LineCounts/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t) - BackgroundCounts/(BackMax[l]-BackMin[l])/LifeTime->GetBinContent(t)) / (BackgroundCounts/(BackMax[l]-BackMin[l])/LifeTime->GetBinContent(t));
      double Value2 = (LineCounts/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t)) / (BackgroundCounts/(BackMax[l]-BackMin[l])/LifeTime->GetBinContent(t));
      Line->SetBinContent(t, Value);
      Line->SetBinError(t, Value2*sqrt(1.0/BackgroundCounts + 1.0/LineCounts));
      
      //Line->SetBinContent(t, LineCounts/(LineMax[l] - LineMin[l])/LifeTime->GetBinContent(t) - BackgroundCounts/(BackMax[l]-BackMin[l])/LifeTime->GetBinContent(t));
      //Line->SetBinError(t, sqrt(LineCounts/pow((LineMax[l] - LineMin[l])*LifeTime->GetBinContent(t), 2) + BackgroundCounts/pow((BackMax[l]-BackMin[l])*LifeTime->GetBinContent(t), 2)));
    }
    
    TCanvas* LineCanvas = new TCanvas();
    LineCanvas->cd();
    Line->Draw();
    LineCanvas->Update();
    
    TF1* LineFitter = new TF1(Name, VariationFit, MinTime, MaxTime, 1);
    Line->Fit(LineFitter);
    
    cout<<Names[l]<<": Chi-Square: "<<LineFitter->GetChisquare()<<" NDFs: "<<LineFitter->GetNDF()<<endl;
    
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NLineFitter::Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff, TH1D* LifeTime, TH2D* Spectrum,
                      TH1D* GeomagneticCutOff, TH1D* GeomagneticCutOffLifetime)
{
  cout<<"Fit all lines..."<<endl;
  
  // Section A: Create all histograms:
      
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

    Spectrum->Fill(FE.m_Energy[e], FE.m_Time[e]);
    GeomagneticCutOff->Fill(O.m_GeomagneticCutOff[OrbitIndex]);
  }
  
  
  double TotalLifeTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (FE.IsGTI(H.m_Time[h], true) == false) continue;
    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    
    int OrbitsIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitsIndex == -1) {
      cout<<"Orbits: Index not found..."<<endl;
      continue;      
    }

    TotalLifeTime += H.m_LiveTime[h];
    LifeTime->Fill(H.m_Time[h], H.m_LiveTime[h]);
    GeomagneticCutOffLifetime->Fill(O.m_GeomagneticCutOff[OrbitsIndex], H.m_LiveTime[h]);
  }
   
  return true;
}


// NLineFitter.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
