/*
 * NBackgroundMode4DataBase.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundMode4DataBase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundMode4DataBase.h"

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
ClassImp(NBackgroundMode4DataBase)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode4DataBase::NBackgroundMode4DataBase()
{
  // Construct an instance of NBackgroundMode4DataBase
  
  m_DBInitialized = false;
    
  m_SpectrumMax = 200;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false; 
  
  m_DataBaseName = "WhyDoIHaveNoNameDB";
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode4DataBase::~NBackgroundMode4DataBase()
{
  // Delete this instance of NBackgroundMode4DataBase
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4DataBase::ParseCommandLine(int argc, char** argv)
{
  if (NBaseTool::ParseCommandLine(argc, argv) == false) return false;
   // Now parse the command line options:
  string Option;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "--name") {
      if (!((argc > i+1) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs one argument!"<<endl;
        return false;
      }
    }
    
    // Then fulfill the options:
    if (Option == "--name") {
      m_DataBaseName = argv[++i];
      cout<<"Accepting name for the data base: "<<m_DataBaseName<<endl;
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4DataBase::Analyze() 
{
  m_DBInitialized = false;
  
  if (m_LookAtModule.Contains("a") && m_LookAtModule.Contains("b")) {
    cout<<"Error: For the time being please run it once for a and once for b (Option: -m a or -m b)"<<endl;
    return false;
  }
  
  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    if (Load(m_Directories[d]) == false) continue;
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering);
  }
  
  
  for (int b = 1; b <= m_DBInternalSpectrum->GetNbinsX(); ++b) {
    m_DBInternalSpectrum->SetBinContent(b, m_DBInternalSpectrum->GetBinContent(b)/m_DBInternalSpectrumLifetime);
    for (int g = 1; g <= m_DBInternalGeomagneticCutOffVsSpectrum->GetNbinsY(); ++g) {
      if (m_DBInternalGeomagneticCutOffLifetime->GetBinContent(g) > 0) {
        m_DBInternalGeomagneticCutOffVsSpectrum->SetBinContent(g, b, m_DBInternalGeomagneticCutOffVsSpectrum->GetBinContent(g, b)/m_DBInternalGeomagneticCutOffLifetime->GetBinContent(g));
        for (int p = 1; p <= m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->GetNbinsZ(); ++p) {
          m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->SetBinContent(g, b, p, m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->GetBinContent(g, b, p)/m_DBInternalGeomagneticCutOffLifetime->GetBinContent(g));
        }
      }
    }
  }
  

  
  TCanvas* DBGeomagneticCutOffVsSpectrumVsPixelIDCanvas = new TCanvas(TString("DBGeomagneticCutOffVsSpectrumVsPixelIDCanvas"), "DBGeomagneticCutOffVsSpectrumVsPixelIDCanvas");
  DBGeomagneticCutOffVsSpectrumVsPixelIDCanvas->cd();
  m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->Draw();
  DBGeomagneticCutOffVsSpectrumVsPixelIDCanvas->Update();
  
  TFile f(m_DataBaseName, "recreate");
  m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->Write();
  f.Close();
  
  TCanvas* DBGeomagneticCutOffVsSpectrumCanvas = new TCanvas(TString("DBGeomagneticCutOffVsSpectrumCanvas"), "DBGeomagneticCutOffVsSpectrumCanvas");
  DBGeomagneticCutOffVsSpectrumCanvas->cd();
  m_DBInternalGeomagneticCutOffVsSpectrum->Draw("colz");
  DBGeomagneticCutOffVsSpectrumCanvas->Update();
  
  TCanvas* DBSpectrumCanvas = new TCanvas(TString("DBSpectrumCanvas"), "DBSpectrumCanvas");
  DBSpectrumCanvas->cd();
  m_DBInternalSpectrum->Draw();
  DBSpectrumCanvas->Update();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4DataBase::Show(NFilteredEvents& F, NHousekeeping& H, NOrbits& O, NEngineering& E)
{
  cout<<"Creating data base for background mode 4..."<<endl;
  
  // Section A: Create all histograms:

  TString iID = "_ID_"; 
  iID += F.m_ID;
  iID += "_M_";
  iID += ((F.m_Module == 0) ? "A" : "B");
  iID += "_L_";
  iID += int(F.m_LiveTime);
  iID += "_C_";
  iID += F.m_CleanLevel;
  TString ID = " (id";
  ID += F.m_ID;
  ID += "-cl0";
  ID += F.m_CleanLevel;
  ID += "-m";
  ID += ((F.m_Module == 0) ? "A" : "B");
  ID += ")";

  // Spectra:    
  double SpectrumMin = 1.6-0.02;
  double SpectrumMax = 121-0.02;  
  int SpectrumBins = int((SpectrumMax-SpectrumMin)/0.04);  

  // Rates:  
  int RatesBins = (H.GetMaxTime() - H.GetMinTime()+1)/1;
  double MinTime = H.GetMinTime()-0.5;
  double MaxTime = H.GetMaxTime()+0.5;

  // Geo-cutoff:
  int BinsGeoCutOff = 8;
  double MinGeoCutOff = O.GetMinGeomagneticCutOff();
  double MaxGeoCutOff = O.GetMaxGeomagneticCutOff();
  
  TH1D* Spectrum = new TH1D(TString("Spectrum") + iID, TString("Spectrum") + ID, SpectrumBins, SpectrumMin, SpectrumMax);
  Spectrum->SetXTitle("Energy [keV]");
  Spectrum->SetYTitle("cts");
  Spectrum->SetLineColor(kBlue);
  
  TH1D* Rates = new TH1D(TString("Rates") + iID, TString("Rates") + ID, RatesBins, MinTime, MaxTime);
  Rates->SetXTitle("Time [s]");
  Rates->SetYTitle("cts");
  Rates->SetLineColor(kBlue);
    
  TH1D* RatesLifetime = new TH1D(TString("RatesLifetime") + iID, TString("RatesLifetime") + ID, RatesBins, MinTime, MaxTime);
  RatesLifetime->SetXTitle("Time [s]");
  RatesLifetime->SetYTitle("cts");
  RatesLifetime->SetLineColor(kBlue);
    
  TH2D* GeomagneticCutOffVsSpectrum = new TH2D(TString("GeomagneticCutOffVsSpectrum") + iID, 
                                         TString("Filtered event rate by geomagnetic cut off") + ID, 
                                         BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff,
                                         SpectrumBins, SpectrumMin, SpectrumMax);
  GeomagneticCutOffVsSpectrum->SetXTitle("Geomagnetic cutoff [GV]");
  GeomagneticCutOffVsSpectrum->SetXTitle("Energy [keV]");
  GeomagneticCutOffVsSpectrum->SetXTitle("cts");
  TH1D* GeomagneticCutOffLifetime = new TH1D(TString("GeomagneticCutOffVsSpectrumLifetime") + iID, 
                                                 TString("Filtered event rate by geomagnetic cut off") + ID, 
                                                 BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff);
  TH3D* GeomagneticCutOffVsSpectrumVsPixelID = 
      new TH3D(TString("RatesByGeomagneticCutOffAndPixelID"), 
               TString("DB 3D"), 
               BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff,
               SpectrumBins, SpectrumMin, SpectrumMax,
               64*64, -0.5, 64*64+0.5);
  GeomagneticCutOffVsSpectrumVsPixelID->SetXTitle("Geomagnetic cutoff [GV]");
  GeomagneticCutOffVsSpectrumVsPixelID->SetXTitle("Energy [keV]");
  GeomagneticCutOffVsSpectrumVsPixelID->SetXTitle("Pixel ID (long-format)");
  
  if (m_DBInitialized == false) {
    m_DBInternalSpectrum = new TH1D(TString("SpectrumOnSource") + iID, TString("Background spectrum all") + ID, SpectrumBins, SpectrumMin, SpectrumMax);
    m_DBInternalSpectrum->SetXTitle("Energy [keV]");
    m_DBInternalSpectrum->SetYTitle("cts");
    //m_DBInternalSpectrum->SetLineColor(kBlue);
    m_DBInternalSpectrumLifetime = 0;
    
    m_DBInternalRates = new TH1D(TString("m_DBInternalRates") + iID, TString("m_DBInternalRates") + ID, RatesBins, MinTime, MaxTime);
    m_DBInternalRates->SetXTitle("Time [s]");
    m_DBInternalRates->SetYTitle("cts");
    //m_DBInternalRates->SetLineColor(kBlue);
    
    m_DBInternalRatesLifetime = new TH1D(TString("m_DBInternalRatesLifetime") + iID, TString("m_DBInternalRatesLifetime") + ID, RatesBins, MinTime, MaxTime);
    m_DBInternalRatesLifetime->SetXTitle("Time [s]");
    m_DBInternalRatesLifetime->SetYTitle("cts");
    //m_DBInternalRatesLifetime->SetLineColor(kBlue);
    
    m_DBInternalGeomagneticCutOffVsSpectrum = new TH2D(TString("m_DBInternalRatesByGeomagneticCutOff"), 
                                                       TString("DB 2D"), 
                                                       BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff,
                                                       SpectrumBins, SpectrumMin, SpectrumMax);
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("Geomagnetic cutoff [GV]");
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("Energy [keV]");
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("cts");
    m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID = 
      new TH3D(TString("m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID"), 
               TString("DB 3D (looks always empty...)"), 
               BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff,
               SpectrumBins, SpectrumMin, SpectrumMax,
               64*64, -0.5, 64*64+0.5);
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("Geomagnetic cutoff [GV]");
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("Energy [keV]");
    m_DBInternalGeomagneticCutOffVsSpectrum->SetXTitle("Pixel ID (long-format)");

    m_DBInternalGeomagneticCutOffLifetime = new TH1D(TString("m_DBInternalGeomagneticCutOffLifetime") + iID, 
                                                               TString("Filtered event rate by geomagnetic cut off") + ID, 
                                                               BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff);
    m_DBInitialized = true;
  }
 
  
  // Section B: Fill (and normalize) histograms
  
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < F.m_Time.size(); ++e) {
    if (WithinSpecialGTI(F.m_Time[e]) == false) continue;
    if (F.m_Energy[e] < SpectrumMin || F.m_Energy[e] > SpectrumMax) continue;

    
    int HKIndex = H.FindClosestIndex(F.m_Time[e]);
    if (HKIndex == -1) {
      cout<<"Housekeeping: Index not found..."<<endl;
      continue;      
    }
 
    int OrbitsIndex = O.FindClosestIndex(F.m_Time[e]);
    if (OrbitsIndex == -1) {
      cout<<"Orbits: Index not found..."<<endl;
      continue;      
    }
    
    int EngIndex = E.FindClosestIndex(F.m_Time[e]);
    if (EngIndex == -1) {
      cout<<"Engineering: Index not found..."<<endl;
      continue;      
    }
    
    int DetectorID = F.m_DetectorID[e];
    int RawX = F.m_RawX[e];
    int RawY = F.m_RawY[e];
    
    /*
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
    */
    Spectrum->Fill(F.m_Energy[e]);
    Rates->Fill(F.m_Time[e]);
    GeomagneticCutOffVsSpectrum->Fill(O.m_GeomagneticCutOff[OrbitsIndex], F.m_Energy[e]);
    GeomagneticCutOffVsSpectrumVsPixelID->Fill(O.m_GeomagneticCutOff[OrbitsIndex], F.m_Energy[e], 
                                               32*32*DetectorID + 32*RawY + RawX);
  }

  double Lifetime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;

    int OrbitsIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitsIndex == -1) {
      cout<<"Orbits: Index not found..."<<endl;
      continue;      
    }
    
    Lifetime += H.m_LiveTime[h];
    RatesLifetime->Fill(H.m_Time[h], H.m_LiveTime[h]);
    GeomagneticCutOffLifetime->Fill(O.m_GeomagneticCutOff[OrbitsIndex], H.m_LiveTime[h]);
  }
  cout<<"Lifetime: "<<Lifetime<<" sec"<<endl;

 
  // (2) Normalize
  /*
  for (int b = 1; b <= RatesBins; ++b) {
    if (RatesLifetime->GetBinContent(b) > 0) {
      Rates->SetBinContent(b, Rates->GetBinContent(b)/Rates->GetBinWidth(b)/RatesLifetime->GetBinContent(b));
    }
  }
  for (int b = 1; b <= SpectrumBins; ++b) {
    Spectrum->SetBinContent(b, Spectrum->GetBinContent(b)/Spectrum->GetBinWidth(b)/Lifetime/F.m_DetectorArea);
  }
  for (int b = 1; b <= SpectrumBins; ++b) {
    for (int g = 1; g <= BinsGeoCutOff; ++g) {
      if (GeomagneticCutOffLifetime->GetBinContent(g) > 0) {  
        GeomagneticCutOffVsSpectrum->SetBinContent(g, b, GeomagneticCutOffVsSpectrum->GetBinContent(g, b)/Spectrum->GetBinWidth(b)/GeomagneticCutOffLifetime->GetBinContent(g)/F.m_DetectorArea);
      }
    }
  }
  */
  
  // (3) To the screen
  TCanvas* SpectrumCanvas = new TCanvas(TString("SpectrumCanvas") + iID, "SpectrumCanvas");
  SpectrumCanvas->cd();
  Spectrum->Draw();
  SpectrumCanvas->Update();
 
  TCanvas* RatesCanvas = new TCanvas(TString("RatesCanvas") + iID, "RatesCanvas");
  RatesCanvas->cd();
  Rates->Draw();
  RatesCanvas->Update();
  
  TCanvas* GeomagneticCutOffVsSpectrumCanvas = new TCanvas(TString("GeomagneticCutOffVsSpectrumCanvas") + iID, "GeomagneticCutOffVsSpectrumCanvas");
  GeomagneticCutOffVsSpectrumCanvas->cd();
  GeomagneticCutOffVsSpectrum->Draw("colz");
  GeomagneticCutOffVsSpectrumCanvas->Update();
  
  
  // (4) Merge
  for (int b = 1; b <= SpectrumBins; ++b) {
    m_DBInternalSpectrum->SetBinContent(b, m_DBInternalSpectrum->GetBinContent(b) + Spectrum->GetBinContent(b));
    for (int g = 1; g <= BinsGeoCutOff; ++g) {
      m_DBInternalGeomagneticCutOffVsSpectrum->SetBinContent(g, b, m_DBInternalGeomagneticCutOffVsSpectrum->GetBinContent(g, b) + GeomagneticCutOffVsSpectrum->GetBinContent(g, b));
      for (int p = 1; p <= 64*64; ++p) {
        m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->SetBinContent(g, b, p, m_DBInternalGeomagneticCutOffVsSpectrumVsPixelID->GetBinContent(g, b, p) + GeomagneticCutOffVsSpectrumVsPixelID->GetBinContent(g, b, p));
      }
    }
  }

  m_DBInternalSpectrumLifetime += Lifetime;
  for (int g = 1; g <= BinsGeoCutOff; ++g) {
    m_DBInternalGeomagneticCutOffLifetime->SetBinContent(g, m_DBInternalGeomagneticCutOffLifetime->GetBinContent(g) + GeomagneticCutOffLifetime->GetBinContent(g));
  }
  
  delete GeomagneticCutOffVsSpectrumVsPixelID;
  
  return true;
}


// NBackgroundMode4DataBase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
