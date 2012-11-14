/*
 * NQuickView.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NQuickView
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NQuickView.h"

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
ClassImp(NQuickView)
#endif


////////////////////////////////////////////////////////////////////////////////


NQuickView::NQuickView()
{
  // Construct an instance of NQuickView
  
  m_MergedSpectrum = 0;
  m_MergedSpectrumLifetime = 0;
}


////////////////////////////////////////////////////////////////////////////////


NQuickView::~NQuickView()
{
  // Delete this instance of NQuickView
}


////////////////////////////////////////////////////////////////////////////////


bool NQuickView::ParseCommandLine(int argc, char** argv)
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


bool NQuickView::Analyze() 
{
  delete m_MergedSpectrum;
  m_MergedSpectrum = new TH1D(TString("MergedSpectrum"), TString("Spectrum - everything merged"), 
                   m_SpectrumBins, m_SpectrumMin, m_SpectrumMax);
  m_MergedSpectrum->SetXTitle("Energy [keV]");
  m_MergedSpectrum->SetYTitle("cts/sec");
  m_MergedSpectrum->SetLineColor(kBlue);
  m_MergedSpectrumLifetime = 0;
  
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
  
  
  for (int b = 1; b <= m_SpectrumBins; ++b) {
    m_MergedSpectrum->SetBinError(b, sqrt(m_MergedSpectrum->GetBinContent(b)));
  }
   
  TCanvas* SpectrumCanvas = new TCanvas();
  SpectrumCanvas->cd();
  m_MergedSpectrum->Scale(1.0/m_MergedSpectrumLifetime);
  m_MergedSpectrum->Draw();
  SpectrumCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumCanvas->SaveAs(m_MergedSpectrum->GetName() + m_FileType);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NQuickView::Show(NFilteredEvents& FE, NUnfilteredEvents& U, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Quick view of the data..."<<endl;
  
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

  /*  
  double SpectrumMin = 20-0.02;
  double SpectrumMax = 150-0.02;  
  int SpectrumBins = (int(SpectrumMax-SpectrumMin)/0.04)/8;  
  */

  double SpectrumMin = m_SpectrumMin;
  double SpectrumMax = m_SpectrumMax;  
  int SpectrumBins = m_SpectrumBins;  
  
  TH1D* Spectrum = new TH1D(TString("Spectrum") + iID, TString("Spectrum - everything") + ID, 
                            SpectrumBins, SpectrumMin, SpectrumMax);
  Spectrum->SetXTitle("Energy [keV]");
  Spectrum->SetYTitle("cts");
  Spectrum->SetLineColor(kBlue);
  
  TH1D* SpectrumSource = new TH1D(TString("SpectrumSource") + iID, TString("Spectrum - source") + ID, SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumSource->SetXTitle("Energy [keV]");
  SpectrumSource->SetYTitle("cts");
  SpectrumSource->SetLineColor(kBlue);
  
  TH1D* SpectrumBackground = new TH1D(TString("SpectrumBackground") + iID, TString("Spectrum - background") + ID, SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumBackground->SetXTitle("Energy [keV]");
  SpectrumBackground->SetYTitle("cts");
  SpectrumBackground->SetLineColor(kBlue);

  
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
  
  TH1D* LifeTimes = new TH1D(TString("LifeTimes") + iID, TString("LifeTimes all") + ID, RatesBins, MinTime, MaxTime);
  LifeTimes->SetXTitle("Time [s]");
  LifeTimes->SetYTitle("cts/sec");

  
  int LongitudeBins = 180;
  int LatitudeBins = 90;
  double MinLatitude = -6.2;
  double MaxLatitude = +6.2;
  
  TH2D* OrbitNormalizerDetector = new TH2D(TString("OrbitNormalizerDetector") + iID, 
                                   TString("OrbitNormalizerDetector") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  TH2D* RatesByOrbit = new TH2D(TString("RatesSAANoTentacleNoByOrbit") + iID, 
                                               TString("Rates in orbit - SAA: No, Tentacle: No") + ID, LongitudeBins, 0, 360, LatitudeBins, MinLatitude, MaxLatitude);
  RatesByOrbit->SetXTitle("Longitude [deg]");
  RatesByOrbit->SetYTitle("Latutude [deg]");
  RatesByOrbit->SetZTitle("cts/sec");
  
   
  TH2D* SpectrumVsDepth = new TH2D(TString("SpectrumVsDepth") + iID, 
                                   TString("Background spectrum vs. depth") + ID, 
                                   SpectrumBins, SpectrumMin, SpectrumMax, 100, -60, 10);
  SpectrumVsDepth->SetXTitle("Energy [keV]");
  SpectrumVsDepth->SetYTitle("Surr energy [keV]");
  SpectrumVsDepth->SetLineColor(7);

  
  
  // Section B: Fill (and normalize) histograms
  double LiveTime = 0;
  
  // Fill histograms which require filling by event
  int DepthCutTimes = 0;
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
    /*
    if (IsGoodEventByExternalDepthFilter(U.m_Status[e]) == false) {
      DepthCutTimes++;
      continue;
    }
    */
    
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
      Spectrum->Fill(U.m_Energy[e]);
      if (Distance > DistanceCutOff) {
        SpectrumBackground->Fill(U.m_Energy[e]);
        Rates->Fill(U.m_Time[e]);
        RatesByOrbit->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex]); 
        PositionsOnSource->Fill(PosX, PosY);
        SpectrumVsDepth->Fill(U.m_Energy[e], U.m_SurrEnergy[e]);
      } else {
        SpectrumSource->Fill(U.m_Energy[e]);        
      }
    }
  }
  
  cout<<"# depth cuts: "<<DepthCutTimes<<endl;
  
  double TotalTime = 0;
  LiveTime = 0;
  for (unsigned int h = 0; h < H.m_Time.size(); ++h) {
    LifeTimes->Fill(H.m_Time[h], H.m_LiveTime[h]);
    
    int OrbitIndex = O.FindClosestIndex(H.m_Time[h]);
    if (OrbitIndex == -1) {
      cout<<"Orbit: Index not found for time "<<H.m_Time[h]<<"..."<<endl;
      continue;      
    }

    if (WithinSpecialGTI(H.m_Time[h]) == false) continue;
    if (WithinSpecialBTI(H.m_Time[h]) == true) continue;
    if (FE.IsGTI(H.m_Time[h], true) == false) continue;

    OrbitNormalizerDetector->Fill(O.m_Longitude[OrbitIndex], O.m_Latitude[OrbitIndex], H.m_LiveTime[h]);

    if (O.m_SafelyOnSource[OrbitIndex] == true) {
      TotalTime += 1.0;
      LiveTime += H.m_LiveTime[h];
    }
  }
 
  // (2) Normalize  
  TCanvas* Positions = new TCanvas(TString("Positions") + iID, TString("Positions") + ID, 600, 600);
  Positions->cd();
  PositionsOnSource->Draw("colz");
  Positions->Update();
  if (m_ShowHistograms.Contains("f")) Positions->SaveAs(PositionsOnSource->GetName() + m_FileType);

  TCanvas* RatesCanvas = new TCanvas();
  RatesCanvas->cd();
  double RatesMax = Rates->GetMaximum();
  Rates->SetMaximum(1.12*RatesMax);
  Rates->Draw();
  RatesCanvas->Update();
    
  if (O.m_OccultationSteps.size() > 0) {
    int NBins = O.m_OccultationSteps.size();
    double* Bins = new double[NBins];
    for (int i = 0; i < NBins; ++i) {
      Bins[i] = O.m_OccultationSteps[i];
    }
    TH1D* Occultation = new TH1D(TString("Occultation") + iID, TString("Occultation") + ID, NBins-1, Bins);
    for (unsigned int b = 0; b < O.m_OccultationSteps.size(); ++b) {
      Occultation->SetBinContent(b+1, 1.025*RatesMax*O.m_OccultationValues[b]);
    }
    Occultation->SetLineColor(kGreen-9);
    Occultation->Draw("SAME");
  }
  
  if (O.m_SAASteps.size() > 0) {
    int NBins = O.m_SAASteps.size();
    double* Bins = new double[NBins];
    for (int i = 0; i < NBins; ++i) {
      Bins[i] = O.m_SAASteps[i];
    }
    TH1D* SAA = new TH1D(TString("SAA") + iID, TString("SAA") + ID, NBins-1, Bins);
    for (unsigned int b = 0; b < O.m_SAASteps.size(); ++b) {
      SAA->SetBinContent(b+1, 1.05*RatesMax*O.m_SAAValues[b]);
    }
    SAA->SetLineColor(kRed-9);
    SAA->Draw("SAME");
  }
  
  if (O.m_DaySteps.size() > 0) {
    int NBins = O.m_DaySteps.size();
    double* Bins = new double[NBins];
    for (int i = 0; i < NBins; ++i) {
      Bins[i] = O.m_DaySteps[i];
    }
    TH1D* SAA = new TH1D(TString("Day") + iID, TString("Day") + ID, NBins-1, Bins);
    for (unsigned int b = 0; b < O.m_DaySteps.size(); ++b) {
      SAA->SetBinContent(b+1, 1.075*RatesMax*O.m_DayValues[b]);
    }
    SAA->SetLineColor(kMagenta-9);
    SAA->Draw("SAME");
  }
  RatesCanvas->Update();
  
  if (O.m_SlewSteps.size() > 0) {
    int NBins = O.m_SlewSteps.size();
    double* Bins = new double[NBins];
    for (int i = 0; i < NBins; ++i) {
      Bins[i] = O.m_SlewSteps[i];
    }
    TH1D* SAA = new TH1D(TString("Slew") + iID, TString("Slew") + ID, NBins-1, Bins);
    for (unsigned int b = 0; b < O.m_SlewSteps.size(); ++b) {
      SAA->SetBinContent(b+1, 1.1*RatesMax*O.m_SlewValues[b]);
    }
    SAA->SetLineColor(kYellow);
    SAA->Draw("SAME");
  }
  RatesCanvas->Update();
  if (m_ShowHistograms.Contains("f")) RatesCanvas->SaveAs(Rates->GetName() + m_FileType);

  TCanvas* SpectrumCanvas = new TCanvas();
  SpectrumCanvas->cd();
  Spectrum->Draw();
  SpectrumCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumCanvas->SaveAs(Spectrum->GetName() + m_FileType);
  
  TCanvas* SpectrumSourceCanvas = new TCanvas();
  SpectrumSourceCanvas->cd();
  SpectrumSource->Draw();
  SpectrumSourceCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumSourceCanvas->SaveAs(SpectrumSource->GetName() + m_FileType);
  
  TCanvas* SpectrumBackgroundCanvas = new TCanvas();
  SpectrumBackgroundCanvas->cd();
  SpectrumBackground->Draw();
  SpectrumBackgroundCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumBackgroundCanvas->SaveAs(SpectrumBackground->GetName() + m_FileType);
  
  
  m_MergedSpectrum->Add(SpectrumBackground);
  m_MergedSpectrumLifetime += LiveTime;
  
  TCanvas* SpectrumVsDepthCanvas = new TCanvas();
  SpectrumVsDepthCanvas->cd();
  SpectrumVsDepth->Draw("colz");
  SpectrumVsDepthCanvas->Update();  
  if (m_ShowHistograms.Contains("f")) SpectrumVsDepthCanvas->SaveAs(SpectrumVsDepth->GetName() + m_FileType);

  return true;
}

bool NQuickView::CreateGTIFile(vector<double>& GTIStart, vector<double>& GTIStop, NHousekeeping& H)
{
  // Copy template
  TString FileName = "nu";
  FileName += H.m_ID;
  FileName += ((H.m_Module == 0) ? "A" : "B");
  FileName += "_user_gti.fits";
  
  ifstream src("Template_gti.fits");
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


// NQuickView.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
