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
    }
  }
    
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::Analyze() 
{

  for (unsigned int d = 0; d < m_Directories.size(); ++d) {
    
    m_DBA = 0;
    m_DBB = 0;
    if (LoadDataBase(m_DataBaseNameA, m_DBA) == false) continue;
    if (LoadDataBase(m_DataBaseNameB, m_DBB) == false) continue;
    cout<<"DB:"<<long(m_DBA)<<":"<<long(m_DBB)<<endl;
    
    NPhaFile PhaA;
    if (m_LookAtModule.Contains("a")) {
      if (PhaA.Load(m_PhaA[d]) == false) return false;
    }
    NPhaFile PhaB;
    if (m_LookAtModule.Contains("b")) {
      if (PhaB.Load(m_PhaB[d]) == false) return false;
    }
 
    if (Load(m_Directories[d]) == false) continue;
 
    if (m_LookAtModule.Contains("a")) Show(m_FilteredEventsA01, m_HousekeepingA, m_Orbits, m_Engineering, PhaA, m_DBA, m_DetPosXA[d], m_DetPosYA[d], m_DetSizeA[d]);
    if (m_LookAtModule.Contains("b")) Show(m_FilteredEventsB01, m_HousekeepingB, m_Orbits, m_Engineering, PhaB, m_DBB, m_DetPosXB[d], m_DetPosYB[d], m_DetSizeB[d]);
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::LoadDataBase(TString FileName, TH3D* DB)
{
  if (DB != 0) delete DB;
  
  TFile* F = new TFile(FileName);
  TIter I(F->GetListOfKeys());
  TKey* Key;
  
  while ((Key = (TKey*) I()) != 0) {
    TClass *Class = gROOT->GetClass(Key->GetClassName());
    if (Class->InheritsFrom("TH1")) {
      DB = (TH3D*) Key->ReadObj();
      break;
    } else if (Class->InheritsFrom("TCanvas")) {
      TCanvas* C = (TCanvas*) Key->ReadObj();
      TIter CanvasContent(C->GetListOfPrimitives());
      TObject* Object;
      while ((Object = CanvasContent()) != 0) {
        if (Object->InheritsFrom("TH1")) {
          DB = (TH3D*) Object;
          break;
        }
      }
    }
  }

  if (DB == 0) {
    cout<<"Background model file not found in file: "<<FileName<<endl;
    return false;
  }
  TCanvas* BackgroundModelCanvas = new TCanvas();
  BackgroundModelCanvas->cd();
  DB->Draw();
  BackgroundModelCanvas->Update();

  cout<<"I:"<<long(DB)<<endl;
  if (m_DBA == 0) {
    m_DBA = DB;
  } else {
    m_DBB = DB;
  }
  
  return true;
}

  
////////////////////////////////////////////////////////////////////////////////


bool NBackgroundMode4::Show(NFilteredEvents& FE, NHousekeeping& H, NOrbits& O, NEngineering& E, 
                       NPhaFile& P, TH3D* DB, int SourcePosX, int SourcePosY, double DistanceCutOff)
{
  cout<<"Creating a background using mode 4..."<<long(DB)<<endl;
    
  TString iID = "_ID_"; 
  iID += FE.m_ID;
  iID += "_M_";
  iID += ((FE.m_Module == 0) ? "A" : "B");
  iID += "_L_";
  iID += int(FE.m_LiveTime);
  iID += "_C_";
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

  TH1D* SpectrumOnSourceAll = new TH1D(TString("SpectrumOnSourceAll") + iID, 
                                       TString("Spectrum all") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumOnSourceAll->SetXTitle("Energy [keV]");
  SpectrumOnSourceAll->SetYTitle("cts");
  SpectrumOnSourceAll->SetLineColor(kBlue);

  TH1D* SpectrumSource = new TH1D(TString("SpectrumSource") + iID, 
                                       TString("Spectrum source") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumSource->SetXTitle("Energy [keV]");
  SpectrumSource->SetYTitle("cts");
  SpectrumSource->SetLineColor(kBlue);

  TH1D* SpectrumScaled = new TH1D(TString("SpectrumScaled") + iID, 
                                       TString("Spectrum source") + ID, 
                                       SpectrumBins, SpectrumMin, SpectrumMax);
  SpectrumScaled->SetXTitle("Energy [keV]");
  SpectrumScaled->SetYTitle("cts");
  SpectrumScaled->SetLineColor(kBlue);
  
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
  double LiveTime = 0;
  
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
    
    if (FE.IsGTI(FE.m_Time[e], true) == true && 
        FE.m_Energy[e] > SpectrumMin && FE.m_Energy[e] < SpectrumMax) {
      SpectrumOnSourceAll->Fill(FE.m_Energy[e]);
      if (Distance < DistanceCutOff) {
        SpectrumSource->Fill(FE.m_Energy[e]);
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
    for (unsigned int RawX = 0; RawX < 32; ++RawX) {
      for (unsigned int RawY = 0; RawY < 32; ++RawY) {
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
        if (Distance > DistanceCutOff) {
          NBackgroundPixels++;
        } else {
          NSourcePixels++;
          int Pos =  32*32*DetectorID + 32*RawY + RawX;

          for (int g = 1; g <= DB->GetNbinsX(); ++g) {
            for (int s = 1; s <= DB->GetNbinsY(); ++s) {
              SpectrumScaled->SetBinContent(s, SpectrumScaled->GetBinContent(s) + DB->GetBinContent(g, s, Pos)*GeomagneticCutOffLifetime->GetBinContent(g));
            }
          }
        }
      }
    }
  }
  cout<<"Source pixels: "<<NSourcePixels<<" background pixels: "<<NBackgroundPixels<<endl;
  
    
  TCanvas* SpectrumScaledCanvas = new TCanvas();
  SpectrumScaledCanvas->cd();
  SpectrumScaledCanvas->SetGridx();
  SpectrumScaledCanvas->SetGridy();
  SpectrumScaledCanvas->SetLogy();
  SpectrumScaled->Draw();
  SpectrumScaledCanvas->Update();

  double ScalingMin = m_MinFitRange;
  double ScalingMax = m_MaxFitRange;
  cout<<"Using fit range: "<<m_MinFitRange<<" "<<m_MaxFitRange<<endl;
  cout<<"Spectrum on source flux above before lifetime [cts/sec/cm2]: "<<SpectrumOnSourceAll->Integral(SpectrumOnSourceAll->FindBin(ScalingMin), SpectrumOnSourceAll->FindBin(ScalingMax))/FE.m_DetectorArea<<endl;
  
  // (2) Normalize
  TH1D* SrcPha = P.m_Spectrum;
  
  double PhaSourceHighEnergyIntensity = SrcPha->Integral(SrcPha->FindBin(ScalingMin), SrcPha->FindBin(ScalingMax));
  double PhaBackgroundHighEnergyIntensity = SpectrumScaled->Integral(SpectrumScaled->FindBin(ScalingMin), SpectrumScaled->FindBin(ScalingMax));
  double PhaScalerHighEnergy = PhaSourceHighEnergyIntensity/PhaBackgroundHighEnergyIntensity;
  cout<<"Pha scaler (high-energy): "<<PhaScalerHighEnergy<<":"<<PhaSourceHighEnergyIntensity<<":"<<PhaBackgroundHighEnergyIntensity<<endl;
  P.SaveBackground(SpectrumScaled, PhaScalerHighEnergy, "mode4");

     
  SrcPha->Rebin(4);
  SpectrumScaled->Scale(PhaScalerHighEnergy);
  SpectrumScaled->Rebin(4);
    
  TCanvas* PhaSpectrum = new TCanvas();
  PhaSpectrum->cd();
  PhaSpectrum->SetGridx();
  PhaSpectrum->SetGridy();
  PhaSpectrum->SetLogy();
  SrcPha->Draw();
  SpectrumScaled->Draw("SAME");
  PhaSpectrum->Update();
    
  
  TCanvas* SpectrumOnSourceAllCanvas = new TCanvas();
  SpectrumOnSourceAllCanvas->cd();
  SpectrumOnSourceAllCanvas->SetGridx();
  SpectrumOnSourceAllCanvas->SetGridy();
  SpectrumOnSourceAllCanvas->SetLogy();
  SpectrumOnSourceAll->Draw();
  SpectrumOnSourceAllCanvas->Update();
     
  TCanvas* PositionsSourceCanvas = new TCanvas(TString("PositionsSourceCanvas") + iID, TString("PositionsBackgroundCanvas") + ID, 600, 600);
  PositionsSourceCanvas->cd();
  PositionsSource->Draw("colz");
  PositionsSourceCanvas->Update();
 
  
  return true;
}


// NBackgroundMode4.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
