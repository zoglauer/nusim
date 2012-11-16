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


////////////////////////////////////////////////////////////////////////////////


TH2D* NApertureTester::s_Aperture = 0;
double NApertureTester::s_Scaler0 = 0;
double NApertureTester::s_Scaler1 = 0;
double NApertureTester::s_Scaler2 = 0;
double NApertureTester::s_Scaler3 = 0;

Double_t ApertureFit(Double_t *x, Double_t *par) 
{
  double ApValue = NApertureTester::s_Aperture->GetBinContent(NApertureTester::s_Aperture->FindBin(x[0]), NApertureTester::s_Aperture->FindBin(x[1]));
  if (ApValue == 0) return 0;
  
  double Value = 0;
  
  if (x[0] > 185 && x[1] > 185) {
    Value = par[0] + par[4]*NApertureTester::s_Scaler0*ApValue;
  } else if (x[0] < 185 && x[1] > 185) {
    Value = par[1] + par[4]*NApertureTester::s_Scaler1*ApValue;
  } else if (x[0] < 185 && x[1] < 185) {
    Value = par[2] + par[4]*NApertureTester::s_Scaler2*ApValue;
  } else if (x[0] > 185 && x[1] < 185) {
    Value = par[3] + par[4]*NApertureTester::s_Scaler3*ApValue;
  }
  
  // Value = par[0] + par[4]*ApValue;
    
  return Value;
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


  double PosMin = 35;
  double PosMax = 335;
  double ExcludeMin = 175;
  double ExcludeMax = 195;
  int NPositionBins = (int(PosMax - PosMin) + 1)/10;
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

  TH2D* SubtractedBackground = new TH2D(TString("SubtractedBackground") + iID, TString("SubtractedBackground") + ID, 
                                       NPositionDataBins, PosMin, PosMax, NPositionDataBins, PosMin, PosMax);
  SubtractedBackground->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  SubtractedBackground->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  SubtractedBackground->SetZTitle("cts");

  NApertureModel M;
  if (M.Initialize() == false) {
    cout<<"Unable to initialize aperture model"<<endl;
    return false;
  }
  for (int bx = 0; bx <= Aperture->GetNbinsX(); ++bx) {
    for (int by = 0; by <= Aperture->GetNbinsX(); ++by) {
      double PosX = Aperture->GetXaxis()->GetBinCenter(bx);
      double PosY = Aperture->GetYaxis()->GetBinCenter(by);
      double Distance = sqrt((SourcePosX - PosX)*(SourcePosX - PosX) + (SourcePosY - PosY)*(SourcePosY - PosY));
      if (Distance > DistanceCutOff) {
        if ((PosX > ExcludeMin && PosX < ExcludeMax) || (PosY > ExcludeMin && PosY < ExcludeMax)) continue;
        Aperture->SetBinContent(bx, by, M.GetApertureShape(PosX, PosY, FE.m_Module));
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
    
  double SumBackground = 0;
  for (int bx = 0; bx <= PositionsBackground->GetNbinsX(); ++bx) {
    for (int by = 0; by <= PositionsBackground->GetNbinsX(); ++by) {
      double PosX = PositionsBackground->GetXaxis()->GetBinCenter(bx);
      double PosY = PositionsBackground->GetYaxis()->GetBinCenter(by);
      if ((PosX > ExcludeMin && PosX < ExcludeMax) || (PosY > ExcludeMin && PosY < ExcludeMax)) {
        PositionsBackground->SetBinContent(bx, by, 0);
      }
      PositionsBackground->SetBinError(bx, by, sqrt(PositionsBackground->GetBinContent(bx, by)));
      SumBackground += PositionsBackground->GetBinContent(bx, by);
    }
  }
  cout<<"Sum background: "<<SumBackground<<endl;

  
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
    
  // Set everything up for fitting
  s_Aperture = Aperture;
  if (FE.m_Module == 0) {
    for (int b = 1; b <= M.m_SpectrumA0.m_Spectrum->GetNbinsX(); ++b) {
      double Energy = M.m_SpectrumA0.m_Spectrum->GetBinCenter(b);
      if (Energy >= m_SpectrumMin && Energy <= m_SpectrumMax) {
        s_Scaler0 += M.m_SpectrumA0.m_Spectrum->GetBinContent(b);
        s_Scaler1 += M.m_SpectrumA1.m_Spectrum->GetBinContent(b);
        s_Scaler2 += M.m_SpectrumA2.m_Spectrum->GetBinContent(b);
        s_Scaler3 += M.m_SpectrumA3.m_Spectrum->GetBinContent(b);
      }
    }
  } else {
    for (int b = 1; b <= M.m_SpectrumB0.m_Spectrum->GetNbinsX(); ++b) {
      double Energy = M.m_SpectrumB0.m_Spectrum->GetBinCenter(b);
      if (Energy >= m_SpectrumMin && Energy <= m_SpectrumMax) {
        s_Scaler0 += M.m_SpectrumB0.m_Spectrum->GetBinContent(b);
        s_Scaler1 += M.m_SpectrumB1.m_Spectrum->GetBinContent(b);
        s_Scaler2 += M.m_SpectrumB2.m_Spectrum->GetBinContent(b);
        s_Scaler3 += M.m_SpectrumB3.m_Spectrum->GetBinContent(b);
      }
    }
  }
  s_Scaler3 /= s_Scaler0;
  s_Scaler2 /= s_Scaler0;
  s_Scaler1 /= s_Scaler0;
  s_Scaler0 /= s_Scaler0;
  
  TF2* ApFit = new TF2("ApFit", ApertureFit, PosMin, PosMax, PosMin, PosMax , 5);
  PositionsBackground->Fit(ApFit, "ILM N");
  //PositionsBackground->Draw("colz");
  //ApFit->Draw("cont1 same");
  
  cout<<"constant: "<<ApFit->GetParameter(0)<<" - scaler: "<<ApFit->GetParameter(4)<<endl;  
  //g_Aperture = 0;
  
  double SumModel = 0;
  for (int bx = 0; bx <= BackgroundModel->GetNbinsX(); ++bx) {
    for (int by = 0; by <= BackgroundModel->GetNbinsX(); ++by) {
      double PosX = BackgroundModel->GetXaxis()->GetBinCenter(bx);
      double PosY = BackgroundModel->GetYaxis()->GetBinCenter(by);
      if ((PosX > ExcludeMin && PosX < ExcludeMax) || (PosY > ExcludeMin && PosY < ExcludeMax)) continue;
      
      double Add = 0;
      if (PosX > 185 && PosY > 185) {
        Add = ApFit->GetParameter(0) + ApFit->GetParameter(4)*s_Scaler0*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < 185 && PosY > 185) {
        Add = ApFit->GetParameter(1) + ApFit->GetParameter(4)*s_Scaler1*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < 185 && PosY < 185) {
        Add = ApFit->GetParameter(2) + ApFit->GetParameter(4)*s_Scaler2*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX > 185 && PosY < 185) {
        Add = ApFit->GetParameter(3) + ApFit->GetParameter(4)*s_Scaler3*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      }

      // = ApFit->GetParameter(0) + ApFit->GetParameter(4)*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      
      BackgroundModel->SetBinContent(bx, by, Add);
      SumModel += Add;
    }
  }
  cout<<"Sum model: "<<SumModel<<endl;
    
  TCanvas* BackgroundModelCanvas = new TCanvas(TString("BackgroundModelCanvas") + iID, TString("BackgroundModelCanvas") + ID, 600, 600);
  BackgroundModelCanvas->cd();
  BackgroundModel->Draw("colz");
  BackgroundModelCanvas->Update();
  if (m_ShowHistograms.Contains("f")) BackgroundModelCanvas->SaveAs(BackgroundModel->GetName() + m_FileType);
  
  
  double SumSubtracted = 0;
  int BinsSubtracted = 0;
  
  for (int bx = 0; bx <= SubtractedBackground->GetNbinsX(); ++bx) {
    for (int by = 0; by <= SubtractedBackground->GetNbinsX(); ++by) {
      double PosX = SubtractedBackground->GetXaxis()->GetBinCenter(bx);
      double PosY = SubtractedBackground->GetYaxis()->GetBinCenter(by);
      if ((PosX > ExcludeMin && PosX < ExcludeMax) || (PosY > ExcludeMin && PosY < ExcludeMax)) continue;

      double ToSubtract = 0;
      if (PosX > 185 && PosY > 185) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler0*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < 185 && PosY > 185) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler1*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < 185 && PosY < 185) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler2*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX > 185 && PosY < 185) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler3*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      }
      //ToSubtract = ApFit->GetParameter(4)*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
     
      SubtractedBackground->SetBinContent(bx, by, PositionsBackground->GetBinContent(bx, by) - ToSubtract);
      if (SubtractedBackground->GetBinContent(bx, by) > 0) {
        SumSubtracted += SubtractedBackground->GetBinContent(bx, by);
        BinsSubtracted++;
      }
    }
  }
  if (BinsSubtracted > 0) {
    cout<<"Average subtracted content: "<<SumSubtracted/BinsSubtracted<<endl;
  }
  
  TCanvas* SubtractedBackgroundCanvas = new TCanvas(TString("SubtractedBackgroundCanvas") + iID, TString("SubtractedBackgroundCanvas") + ID, 600, 600);
  SubtractedBackgroundCanvas->cd();
  SubtractedBackground->Draw("colz");
  SubtractedBackgroundCanvas->Update();
  if (m_ShowHistograms.Contains("f")) SubtractedBackgroundCanvas->SaveAs(SubtractedBackground->GetName() + m_FileType);
 
  return true;
}


// NApertureTester.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
