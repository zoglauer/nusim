/*
 * NBackgroundMode4.cxx
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NBackgroundMode4
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NBackgroundMode4.h"

// Standard libs:
#include <fstream>
#include <limits>
#include <sstream>
#include <iomanip>
#include <iostream>
using namespace std;
#include "fitsio.h"

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
ClassImp(NBackgroundMode4)
#endif


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode4::NBackgroundMode4()
{
  // Construct an instance of NBackgroundMode4
  
  m_DBA = 0;
  m_DBB = 0;
    
  m_SpectrumMax = 200;
  m_ReadUnfiltered = false;
  m_ReadFiltered02 = false;
  m_DumpOneSpectrumPerPixel = false;
}


////////////////////////////////////////////////////////////////////////////////


NBackgroundMode4::~NBackgroundMode4()
{
  // Delete this instance of NBackgroundMode4
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::ParseCommandLine(int argc, char** argv)
{
  if (NBackgroundModes::ParseCommandLine(argc, argv) == false) return false;
  
  // Now parse the command line options:
  string Option;
  for (int i = 1; i < argc; i++) {
    Option = argv[i];
    
    // First check if each option has sufficient arguments:
    // Single argument
    if (Option == "--names") {
      if (!((argc > i+1) && 
        (argv[i+1][0] != '-' || isalpha(argv[i+1][1]) == 0))){
        cout<<"Error: Option "<<argv[i][1]<<" needs one argument!"<<endl;
        return false;
      }
    }
    
    // Then fulfill the options:
    if (Option == "--names") {
      m_DataBaseNameA = argv[++i];
      m_DataBaseNameB = argv[++i];
      cout<<"Accepting name for the data bases: "<<m_DataBaseNameA<<" & "<<m_DataBaseNameB<<endl;
    } else if (Option == "--dump-one-spectrum-per-pixel") {
      m_DumpOneSpectrumPerPixel = true;
      cout<<"Accepting to dump one spectrum per pixel."<<endl;
    }
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::Analyze() 
{
  cout<<"Starting to generate background in mode 4"<<endl;

  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    
    m_DBA = 0;
    m_DBB = 0;
    
    NPhaFile PhaA;
    if (m_LookAtModule.Contains("a")) {
      if (m_PhaA[d] == "") {
        cout<<"Need a PHA file name..."<<endl;
        return false;
      }
      m_DBA = LoadDataBase(m_DataBaseNameA);
      if (m_DBA == 0) {
        cout<<"Unable to load data base: "<<m_DataBaseNameA<<endl;
        return false;
      }
      /*
      double Sum = 0;
      for (int p = 1; p <= m_DBA->GetNbinsZ(); ++p) {
        for (int g = 1; g <= m_DBA->GetNbinsX(); ++g) {
          for (int s = 1; s <= m_DBA->GetNbinsY(); ++s) {
            Sum += m_DBA->GetBinContent(g, s, p);
          }
        }
      }
      cout<<"Sum of content: "<<Sum<<endl;
      */
      if (PhaA.Load(m_PhaA[d]) == false) return false;
    }
    NPhaFile PhaB;
    if (m_LookAtModule.Contains("b")) {
      if (m_PhaB[d] == "") {
        cout<<"Need a PHA file name..."<<endl;
        return false;
      }
      m_DBB = LoadDataBase(m_DataBaseNameB);
      if (m_DBB == 0) {
        cout<<"Unable to load data base: "<<m_DataBaseNameA<<endl;
        return false;
      }
      if (PhaB.Load(m_PhaB[d]) == false) return false;
    }
 
    if (Load(m_Directories[d]) == false) continue;
 
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, PhaA, m_DBA, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, PhaB, m_DBB, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


TH3F* NBackgroundMode4::LoadDataBase(TString FileName)
{
  TH3F* DB = 0;
  
  TFile* F = new TFile(FileName);
  TIter I(F->GetListOfKeys());
  TKey* Key;
  
  while ((Key = (TKey*) I()) != 0) {
    TClass *Class = gROOT->GetClass(Key->GetClassName());
    if (Class->InheritsFrom("TH1")) {
      DB = (TH3F*) Key->ReadObj();
      break;
    } else if (Class->InheritsFrom("TCanvas")) {
      TCanvas* C = (TCanvas*) Key->ReadObj();
      TIter CanvasContent(C->GetListOfPrimitives());
      TObject* Object;
      while ((Object = CanvasContent()) != 0) {
        if (Object->InheritsFrom("TH1")) {
          DB = (TH3F*) Object;
          break;
        }
      }
    }
  }

  if (DB == 0) {
    cout<<"Background model file not found in file: "<<FileName<<endl;
    return 0;
  }
  TCanvas* BackgroundModelCanvas = new TCanvas();
  BackgroundModelCanvas->cd();
  DB->Draw();
  BackgroundModelCanvas->Update();

  return DB;
}

  
////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       NPhaFile& P, TH3F* DB, int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Creating a background using mode 4..."<<long(DB)<<endl;
    
  TString iID = "_BackgroundMode4_id"; 
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
  double SpectrumMin = DB->GetYaxis()->GetXmin();
  double SpectrumMax = DB->GetYaxis()->GetXmax();  
  int SpectrumBins = DB->GetNbinsY();  

  TH1D* SpectrumFromWholeDetector = new TH1D(TString("SpectrumFromWholeDetector") + iID, 
                                       TString("Spectrum from whole detector") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumFromWholeDetector->SetXTitle("Energy [keV]");
  SpectrumFromWholeDetector->SetYTitle("cts");
  SpectrumFromWholeDetector->SetLineColor(kBlue);

  TH1D* SpectrumFromSourceRegion = new TH1D(TString("SpectrumFromSourceRegion") + iID, 
                                       TString("Spectrum from source region") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumFromSourceRegion->SetXTitle("Energy [keV]");
  SpectrumFromSourceRegion->SetYTitle("cts");
  SpectrumFromSourceRegion->SetLineColor(kBlue);

  TH1D* ScaledSpectrumFromDB = new TH1D(TString("ScaledSpectrumFromDB") + iID, 
                                       TString("Scaled spectrum from DB") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  ScaledSpectrumFromDB->SetXTitle("Energy [keV]");
  ScaledSpectrumFromDB->SetYTitle("cts");
  ScaledSpectrumFromDB->SetLineColor(kGreen);
  
  // Spectra:

  
  int BinsGeoCutOff = DB->GetNbinsX();
  double MinGeoCutOff = DB->GetXaxis()->GetXmin();
  double MaxGeoCutOff = DB->GetXaxis()->GetXmax();
  TH1D* GeomagneticCutOffLifetime = new TH1D(TString("GeomagneticCutOffVsSpectrumLifetime") + iID, 
                                             TString("Filtered event rate by geomagnetic cut off") + ID, 
                                             BinsGeoCutOff, MinGeoCutOff, MaxGeoCutOff);


  int NPositionBins = 64;

  TH2D* PositionsSource = new TH2D(TString("PositionsSource") + iID, 
                                   TString("PositionsSource") + ID, 
                                   NPositionBins, -32, 32, NPositionBins, -32, 32);
  PositionsSource->SetXTitle("1, 2 <-- Raw X [pixel] --> 3, 0");
  PositionsSource->SetYTitle("2, 3 <-- Raw Y [pixel] --> 1, 0");
  PositionsSource->SetZTitle("cts");
  
  
  // Section B: Fill (and normalize) histograms
  
  // Fill histograms which require filling by event
  for (unsigned int e = 0; e < FE.m_Time.size(); ++e) {

    if (WithinSpecialGTI(FE.m_Time[e]) == false) continue;
    if (WithinSpecialBTI(FE.m_Time[e]) == true) continue;
    if (FE.m_Energy[e] < SpectrumMin || FE.m_Energy[e] > SpectrumMax) continue;

    
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
    ConvertRawPos(RawX, RawY, DetectorID, PosX, PosY);

    double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
    
    if (FE.IsGTI(FE.m_Time[e], true) == true && 
        FE.m_Energy[e] > SpectrumMin && FE.m_Energy[e] < SpectrumMax) {
      SpectrumFromWholeDetector->Fill(FE.m_Energy[e]);
      if (Distance < DistanceCutOff) {
        SpectrumFromSourceRegion->Fill(FE.m_Energy[e]);
        PositionsSource->Fill(PosX, PosY);
      }
    }
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
    GeomagneticCutOffLifetime->Fill(O.m_GeomagneticCutOff[OrbitsIndex], H.m_LiveTime[h]);
  }
  cout<<"Lifetime: "<<Lifetime<<" sec"<<endl;

  
  // Create spectrum from DB:
  int NSourcePixels = 0;
  int NBackgroundPixels = 0;
  
  for (unsigned int DetectorID = 0; DetectorID < 4; ++DetectorID) {
    for (int RawX = 0; RawX < 32; ++RawX) {
      for (int RawY = 0; RawY < 32; ++RawY) {
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
        
        if (m_DumpOneSpectrumPerPixel == false) {
          //cout<<"Source pos: "<<SourcePosX<<", "<<SourcePosY<<" - "<<PosX<<", "<<PosY<<endl;
          double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
          if (Distance > DistanceCutOff) {
            NBackgroundPixels++;
          } else {
            NSourcePixels++;
            int Pos =  32*32*DetectorID + 32*RawY + RawX;

            for (int g = 1; g <= DB->GetNbinsX(); ++g) {
              for (int s = 1; s <= DB->GetNbinsY(); ++s) {
                //if (DB->GetBinContent(g, s, Pos) > 0) {
                //  cout<<"Adding: "<<DB->GetBinContent(g, s, Pos)<<endl;
                //}
                ScaledSpectrumFromDB->SetBinContent(s, ScaledSpectrumFromDB->GetBinContent(s) + 1000*DB->GetBinContent(g, s, Pos)*GeomagneticCutOffLifetime->GetBinContent(g));
              }
            }
          }
        } else {
          for (int s = 1; s <= DB->GetNbinsY(); ++s) ScaledSpectrumFromDB->SetBinContent(s, 0);
          int Pos =  32*32*DetectorID + 32*RawY + RawX;
          for (int g = 1; g <= DB->GetNbinsX(); ++g) {
            for (int s = 1; s <= DB->GetNbinsY(); ++s) {
              //if (DB->GetBinContent(g, s, Pos) > 0) {
                //cout<<DetectorID<<":"<<RawY<<":"<<RawX<<": s:"<<s<<" g:"<<g<<": Adding: "<<DB->GetBinContent(g, s, Pos)<<":"<<GeomagneticCutOffLifetime->GetBinContent(g)<<endl;
              //}
              ScaledSpectrumFromDB->SetBinContent(s, ScaledSpectrumFromDB->GetBinContent(s) + DB->GetBinContent(g, s, Pos)*GeomagneticCutOffLifetime->GetBinContent(g));
            }
          }
          SaveAsFits(ScaledSpectrumFromDB, ((FE.m_Module == 0) ? 'A' : 'B'), DetectorID, RawX, RawY);
        }
      }
    }
  }
  cout<<"Source pixels: "<<NSourcePixels<<" background pixels: "<<NBackgroundPixels<<endl;
  
  if (m_DumpOneSpectrumPerPixel == true) {
    return true;
  }
    
  TCanvas* ScaledSpectrumFromDBCanvas = new TCanvas();
  ScaledSpectrumFromDBCanvas->cd();
  ScaledSpectrumFromDBCanvas->SetGridx();
  ScaledSpectrumFromDBCanvas->SetGridy();
  ScaledSpectrumFromDBCanvas->SetLogy();
  ScaledSpectrumFromDB->Draw();
  ScaledSpectrumFromDBCanvas->Update();
  if (m_ShowHistograms.Contains("f")) ScaledSpectrumFromDBCanvas->SaveAs(ScaledSpectrumFromDB->GetName() + m_FileType);

  double ScalingMin = m_MinFitRange;
  double ScalingMax = m_MaxFitRange;
  cout<<"Using fit range: "<<m_MinFitRange<<" "<<m_MaxFitRange<<endl;
  cout<<"Spectrum on source flux above before lifetime [cts/sec/cm2]: "<<SpectrumFromWholeDetector->Integral(SpectrumFromWholeDetector->FindBin(ScalingMin), SpectrumFromWholeDetector->FindBin(ScalingMax))/FE.m_DetectorArea<<endl;
  
  // (2) Normalize
  TH1D* SrcPha = P.m_Spectrum;
  
  double PhaSourceHighEnergyIntensity = SrcPha->Integral(SrcPha->FindBin(ScalingMin), SrcPha->FindBin(ScalingMax));
  if (PhaSourceHighEnergyIntensity < 1000) {
    cout<<endl<<"!!! WARNING !!!"<<endl;
    cout<<"You have a very low amount of source counts in your chosen energy range ("" - "" keV): "<<PhaSourceHighEnergyIntensity<<endl;
    cout<<"This background scaling method won't work very well if we don't have enough counts!"<<endl<<endl;
  }
  
  double PhaBackgroundHighEnergyIntensity = ScaledSpectrumFromDB->Integral(ScaledSpectrumFromDB->FindBin(ScalingMin), ScaledSpectrumFromDB->FindBin(ScalingMax));
  if (PhaBackgroundHighEnergyIntensity < 10000) {
    cout<<endl<<"!!! WARNING !!!"<<endl;
    cout<<"You have a very low amount of background counts in your chosen energy range ("" - "" keV): "<<PhaBackgroundHighEnergyIntensity<<endl;
    cout<<"This background scaling method won't work very well if we don't have enough counts!"<<endl<<endl;
  }
  if (PhaBackgroundHighEnergyIntensity == 0) {
    cout<<endl<<"!!! ERROR !!!"<<endl;
    cout<<"Something failed, we don't have any background counts in your chosen energy range ("" - "" keV): "<<PhaBackgroundHighEnergyIntensity<<endl;
    return false;
  }
  
  double PhaScalerHighEnergy = PhaSourceHighEnergyIntensity/PhaBackgroundHighEnergyIntensity;
  cout<<"Pha scaler (high-energy): "<<PhaScalerHighEnergy<<" with source counts: "<<PhaSourceHighEnergyIntensity<<" and background counts: "<<PhaBackgroundHighEnergyIntensity<<endl;
  P.SaveBackground(ScaledSpectrumFromDB, PhaScalerHighEnergy, "mode4");

     
  SrcPha->Rebin(4);
  ScaledSpectrumFromDB->Scale(PhaScalerHighEnergy);
  ScaledSpectrumFromDB->Rebin(4);
    
  TCanvas* PhaSpectrum = new TCanvas();
  PhaSpectrum->cd();
  PhaSpectrum->SetGridx();
  PhaSpectrum->SetGridy();
  PhaSpectrum->SetLogy();
  SrcPha->Draw();
  ScaledSpectrumFromDB->Draw("SAME");
  PhaSpectrum->Update();
  if (m_ShowHistograms.Contains("f")) PhaSpectrum->SaveAs(TString("Pha") + iID + m_FileType);
    
  
  TCanvas* SpectrumFromWholeDetectorCanvas = new TCanvas();
  SpectrumFromWholeDetectorCanvas->cd();
  SpectrumFromWholeDetectorCanvas->SetGridx();
  SpectrumFromWholeDetectorCanvas->SetGridy();
  SpectrumFromWholeDetectorCanvas->SetLogy();
  SpectrumFromWholeDetector->Draw();
  SpectrumFromWholeDetectorCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SpectrumFromWholeDetectorCanvas->SaveAs(SpectrumFromWholeDetector->GetName() + m_FileType);
  
  TCanvas* SpectrumFromSourceRegionCanvas = new TCanvas();
  SpectrumFromSourceRegionCanvas->cd();
  SpectrumFromSourceRegionCanvas->SetGridx();
  SpectrumFromSourceRegionCanvas->SetGridy();
  SpectrumFromSourceRegionCanvas->SetLogy();
  SpectrumFromSourceRegion->Draw();
  SpectrumFromSourceRegionCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SpectrumFromSourceRegionCanvas->SaveAs(SpectrumFromSourceRegion->GetName() + m_FileType);
  
    
  TCanvas* PositionsSourceCanvas = new TCanvas(TString("PositionsSourceCanvas") + iID, TString("PositionsBackgroundCanvas") + ID, 600, 600);
  PositionsSourceCanvas->cd();
  PositionsSource->Draw("colz");
  PositionsSourceCanvas->Update();
  if (m_ShowHistograms.Contains("f")) PositionsSourceCanvas->SaveAs(PositionsSource->GetName() + m_FileType);
 
  
  return true;
}

  
////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::SaveAsFits(TH1D* Spectrum, char Module, int DetectorID, int RawX, int RawY) 
{
  // 
  ostringstream out;
  out<<"!PixSpec_"<<Module<<"_"<<setfill('0')<<setw(2)<<DetectorID<<"_"<<setw(2)<<RawX<<"_"<<setw(2)<<RawY<<".fits";
    
  TString NewFileName(out.str().c_str());
  
  fitsfile* File = 0;
  int Status = 0;
  char errorcode[1000];
 
  
  fits_create_file(&File, NewFileName, &Status);
  if (Status != 0) {
    cout<<"Error in creating file: "<<endl;
    cout<<NewFileName<<endl;
    fits_get_errstatus(Status, errorcode);
    cout<<"Error code: "<<errorcode<<endl;
    return false;
  }

  //! create binary table extension
  char ExtensionName[] = "Spectra";
  long nrow = 4096;
  int tfield = 2;
  char* ttype[] = { "CHANNEL", "NORMCOUNTS" };
  char* tform[] = { "1J", "1E" };
  char* tunit[] = { "channel", "norm. cts" };
   
  fits_create_tbl(File, BINARY_TBL, nrow, tfield, ttype, tform, tunit, ExtensionName, &Status); 
  if (Status != 0) {
    cout<<"Error in creating extension: "<<ExtensionName<<endl;
    cout<<NewFileName<<endl;
    File = 0;
    return false;
  }
  
  for (int s = 1; s <= Spectrum->GetNbinsX(); ++s) {
    double Value = Spectrum->GetBinContent(s);
    fits_write_col_int(File, 1, s, 1, 1, &s, &Status);
    fits_write_col_dbl(File, 2, s, 1, 1, &Value, &Status);
    if (Status != 0) {
      cerr << "Error: Writing failed" << endl;
      fits_close_file(File, &Status);
      File = 0;
      return false;
    }
  }
   
  fits_close_file(File, &Status);
  
  return true;
}


// NBackgroundMode4.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
