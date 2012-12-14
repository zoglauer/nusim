/*
* NApertureModel.cxx
*
* Copyright (C) by the NuSTAR tea
* All rights reserved.
*
*/


////////////////////////////////////////////////////////////////////////////////
//
// NApertureModel
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NApertureModel.h"

// Standard libs:

// ROOT libs:
#include "TCanvas.h"
#include "TF2.h"

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NApertureModel)
#endif


////////////////////////////////////////////////////////////////////////////////


NApertureModel::NApertureModel()
{
  // Construct an instance of NApertureModel
}


////////////////////////////////////////////////////////////////////////////////


NApertureModel::~NApertureModel()
{
  // Delete this instance of NApertureModel
  
  // ... too lazy ...
}


////////////////////////////////////////////////////////////////////////////////


bool NApertureModel::Initialize(TString ObsID, int Module)
{
  if (m_SpectrumA0.Load("$(NUSIM)/resource/nulyses/aperbgdA_det0.pha") == false) return false;
  if (m_SpectrumA1.Load("$(NUSIM)/resource/nulyses/aperbgdA_det1.pha") == false) return false;
  if (m_SpectrumA2.Load("$(NUSIM)/resource/nulyses/aperbgdA_det2.pha") == false) return false;
  if (m_SpectrumA3.Load("$(NUSIM)/resource/nulyses/aperbgdA_det3.pha") == false) return false;
  if (m_SpectrumB0.Load("$(NUSIM)/resource/nulyses/aperbgdB_det0.pha") == false) return false;
  if (m_SpectrumB1.Load("$(NUSIM)/resource/nulyses/aperbgdB_det1.pha") == false) return false;
  if (m_SpectrumB2.Load("$(NUSIM)/resource/nulyses/aperbgdB_det2.pha") == false) return false;
  if (m_SpectrumB3.Load("$(NUSIM)/resource/nulyses/aperbgdB_det3.pha") == false) return false;

  m_Module = Module;
  m_ObsID = ObsID;
  
  m_iID = "_ApertureTester_id"; 
  m_iID += m_ObsID;
  m_iID += "_m";
  m_iID += ((m_Module == 0) ? "A" : "B");

  m_ID = " (id";
  m_ID += m_ObsID;
  m_ID += "-m";
  m_ID += ((m_Module == 0) ? "A" : "B");
  m_ID += ")";
  
   
  m_PosMin = 35;
  m_PosMax = 335;
  m_ExcludeMin = 175;
  m_ExcludeMax = 195;
  m_Divider = (m_ExcludeMin + m_ExcludeMax)/2.0;
  m_NPositionBins = (int(m_PosMax - m_PosMin))/10;
  m_NPositionDataBins = (int(m_PosMax - m_PosMin))/10;

  m_SpectrumMin = 3.0-0.02;
  m_SpectrumMax = 150-0.02;  
  m_SpectrumBins = int((m_SpectrumMax - m_SpectrumMin)/0.04);  

  m_ApertureSpectrumMin = 3.0-0.02;
  m_ApertureSpectrumMax = 10-0.02;  
  m_ApertureSpectrumBins = int((m_SpectrumMax - m_SpectrumMin)/0.04)/8;  
  
  m_Positions = new TH2D(TString("PositionsBackground") + m_iID, TString("PositionsBackground") + m_ID, 
                         m_NPositionDataBins, m_PosMin, m_PosMax, m_NPositionDataBins, m_PosMin, m_PosMax);
  m_Positions->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  m_Positions->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  m_Positions->SetZTitle("cts");
  
  m_SubtractedPositions = new TH2D(TString("SubtractedPositionsBackground") + m_iID, TString("SubtractedPositionsBackground") + m_ID, 
                         m_NPositionDataBins, m_PosMin, m_PosMax, m_NPositionDataBins, m_PosMin, m_PosMax);
  m_SubtractedPositions->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  m_SubtractedPositions->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  m_SubtractedPositions->SetZTitle("cts");
  
  m_SpectrumAll = new TH1D(TString("SpectrumAll") + m_iID, TString("Spectrum all") + m_ID, 
                            m_SpectrumBins, m_SpectrumMin, m_SpectrumMax);
  m_SpectrumAll->SetXTitle("Energy [keV]");
  m_SpectrumAll->SetYTitle("cts");
  m_SpectrumAll->SetLineColor(kBlue);
  
  m_SubtractedSpectrumAll = new TH1D(TString("SubtractedSpectrumAll") + m_iID, TString("Subtracted spectrum all") + m_ID, 
                            m_SpectrumBins, m_SpectrumMin, m_SpectrumMax);
  m_SubtractedSpectrumAll->SetXTitle("Energy [keV]");
  m_SubtractedSpectrumAll->SetYTitle("cts");
  m_SubtractedSpectrumAll->SetLineColor(kRed);
  
  m_Spectrum0 = new TH1D(TString("Spectrum0") + m_iID, TString("Spectrum0") + m_ID, 
                         m_ApertureSpectrumBins, m_ApertureSpectrumMin, m_ApertureSpectrumMax);
  m_Spectrum0->SetXTitle("Energy [keV]");
  m_Spectrum0->SetYTitle("cts");
  m_Spectrum0->SetLineColor(kBlue);
  
  m_Spectrum1 = new TH1D(TString("Spectrum1") + m_iID, TString("Spectrum1") + m_ID, 
                         m_ApertureSpectrumBins, m_ApertureSpectrumMin, m_ApertureSpectrumMax);
  m_Spectrum1->SetXTitle("Energy [keV]");
  m_Spectrum1->SetYTitle("cts");
  m_Spectrum1->SetLineColor(kBlue);
  
  m_Spectrum2 = new TH1D(TString("Spectrum2") + m_iID, TString("Spectrum2") + m_ID, 
                         m_ApertureSpectrumBins, m_ApertureSpectrumMin, m_ApertureSpectrumMax);
  m_Spectrum2->SetXTitle("Energy [keV]");
  m_Spectrum2->SetYTitle("cts");
  m_Spectrum2->SetLineColor(kBlue);
  
  m_Spectrum3 = new TH1D(TString("Spectrum3") + m_iID, TString("Spectrum3") + m_ID, 
                         m_ApertureSpectrumBins, m_ApertureSpectrumMin, m_ApertureSpectrumMax);
  m_Spectrum3->SetXTitle("Energy [keV]");
  m_Spectrum3->SetYTitle("cts");
  m_Spectrum3->SetLineColor(kBlue);

  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NApertureModel::Add(double Det1X, double Det1Y, double Energy)
{
  if ((Det1X > m_ExcludeMin && Det1X < m_ExcludeMax) || (Det1Y > m_ExcludeMin && Det1Y < m_ExcludeMax)) return;
  m_SpectrumAll->Fill(Energy);

  if (Energy < m_ApertureSpectrumMin || Energy > m_ApertureSpectrumMax) return;
  
  m_Positions->Fill(Det1X, Det1Y);
  
  if (Det1X > m_Divider && Det1Y > m_Divider) {
    m_Spectrum0->Fill(Energy);
  } else if (Det1X < m_Divider && Det1Y > m_Divider) {
    m_Spectrum1->Fill(Energy);
  } else if (Det1X < m_Divider && Det1Y < m_Divider) {
    m_Spectrum2->Fill(Energy);
  } else if (Det1X > m_Divider && Det1Y < m_Divider) {
    m_Spectrum3->Fill(Energy);
  }
}


////////////////////////////////////////////////////////////////////////////////


TH2D* NApertureModel::s_Aperture = 0;
double NApertureModel::s_Scaler0 = 0;
double NApertureModel::s_Scaler1 = 0;
double NApertureModel::s_Scaler2 = 0;
double NApertureModel::s_Scaler3 = 0;
bool NApertureModel::s_CombinedFit = true;


////////////////////////////////////////////////////////////////////////////////


Double_t ApertureFit(Double_t *x, Double_t *par) 
{
  double ApValue = NApertureModel::s_Aperture->GetBinContent(NApertureModel::s_Aperture->FindBin(x[0]), NApertureModel::s_Aperture->FindBin(x[1]));
  if (ApValue == 0) return 0;
  
  double Value = 0;
  if (NApertureModel::s_CombinedFit == false) {
    if (x[0] > 185 && x[1] > 185) {
      Value = par[0] + par[4]*NApertureModel::s_Scaler0*ApValue;
    } else if (x[0] < 185 && x[1] > 185) {
      Value = par[1] + par[4]*NApertureModel::s_Scaler1*ApValue;
    } else if (x[0] < 185 && x[1] < 185) {
      Value = par[2] + par[4]*NApertureModel::s_Scaler2*ApValue;
    } else if (x[0] > 185 && x[1] < 185) {
      Value = par[3] + par[4]*NApertureModel::s_Scaler3*ApValue;
    }
  } else {
    if (x[0] > 185 && x[1] > 185) {
      Value = par[0] + par[4]*NApertureModel::s_Scaler0*ApValue;
    } else if (x[0] < 185 && x[1] > 185) {
      Value = par[0] + par[4]*NApertureModel::s_Scaler1*ApValue;
    } else if (x[0] < 185 && x[1] < 185) {
      Value = par[0] + par[4]*NApertureModel::s_Scaler2*ApValue;
    } else if (x[0] > 185 && x[1] < 185) {
      Value = par[0] + par[4]*NApertureModel::s_Scaler3*ApValue;
    }
  }
  // Value = par[0] + par[4]*ApValue;
    
  return Value;
}   


////////////////////////////////////////////////////////////////////////////////


bool NApertureModel::Analyze()
{
  // The input positions
  TCanvas* PositionsCanvas = new TCanvas(TString("PositionsCanvas") + m_iID, TString("PositionsCanvas") + m_ID, 600, 600);
  PositionsCanvas->cd();
  m_Positions->Draw("colz");
  PositionsCanvas->Update();

  double SumBkg = 0;
  for (int bx = 1; bx <= m_NPositionBins; ++bx) {
    for (int by = 1; by <= m_NPositionBins; ++by) {
      SumBkg += m_Positions->GetBinContent(bx, by);
    }
  }
  cout<<"Sum positions: "<<SumBkg<<endl;
  
  // Create aperture image
  TH2D* Aperture = new TH2D(TString("Aperture") + m_iID, TString("Aperture") + m_ID, 
                            m_NPositionDataBins, m_PosMin, m_PosMax, m_NPositionDataBins, m_PosMin, m_PosMax);
  Aperture->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  Aperture->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  Aperture->SetZTitle("cts");

  for (int bx = 1; bx <= m_NPositionBins; ++bx) {
    for (int by = 1; by <= m_NPositionBins; ++by) {
      double PosX = Aperture->GetXaxis()->GetBinCenter(bx);
      double PosY = Aperture->GetYaxis()->GetBinCenter(by);
      if ((PosX > m_ExcludeMin && PosX < m_ExcludeMax) || (PosY > m_ExcludeMin && PosY < m_ExcludeMax)) continue;
      if (m_Positions->GetBinContent(bx, by) == 0) continue;
      
      Aperture->SetBinContent(bx, by, GetApertureShape(PosX, PosY, m_Module));
    }
  }
    
  TCanvas* ApertureCanvas = new TCanvas(TString("ApertureCanvas") + m_iID, TString("ApertureCanvas") + m_ID, 600, 600);
  ApertureCanvas->cd();
  Aperture->Draw("colz");
  ApertureCanvas->Update();

  
  // Set everything up for fitting
  s_Aperture = Aperture;
  if (m_Module == 0) {
    for (int e = 1; e <= m_SpectrumBins; ++e) {
      double Energy = m_SpectrumAll->GetBinCenter(e);
      int Bin = m_SpectrumA0.m_Spectrum->FindBin(Energy);
      if (Energy >= m_ApertureSpectrumMin && Energy <= m_ApertureSpectrumMax) {
        s_Scaler0 += m_SpectrumA0.m_Spectrum->GetBinContent(Bin);
        s_Scaler1 += m_SpectrumA1.m_Spectrum->GetBinContent(Bin);
        s_Scaler2 += m_SpectrumA2.m_Spectrum->GetBinContent(Bin);
        s_Scaler3 += m_SpectrumA3.m_Spectrum->GetBinContent(Bin);
      }
    }
  } else {
    for (int e = 1; e <= m_SpectrumBins; ++e) {
      double Energy = m_SpectrumAll->GetBinCenter(e);
      int Bin = m_SpectrumA0.m_Spectrum->FindBin(Energy);
      if (Energy >= m_ApertureSpectrumMin && Energy <= m_ApertureSpectrumMax) {
        s_Scaler0 += m_SpectrumB0.m_Spectrum->GetBinContent(Bin);
        s_Scaler1 += m_SpectrumB1.m_Spectrum->GetBinContent(Bin);
        s_Scaler2 += m_SpectrumB2.m_Spectrum->GetBinContent(Bin);
        s_Scaler3 += m_SpectrumB3.m_Spectrum->GetBinContent(Bin);
      }
    }
  }
  /*
  s_Scaler3 /= s_Scaler0;
  s_Scaler2 /= s_Scaler0;
  s_Scaler1 /= s_Scaler0;
  s_Scaler0 /= s_Scaler0;
  */
  
  TF2* ApFit = new TF2("ApFit", ApertureFit, m_PosMin, m_PosMax, m_PosMin, m_PosMax , 5);
  m_Positions->Fit(ApFit, "ILMR N");
  //PositionsBackground->Draw("colz");
  //ApFit->Draw("cont1 same");
  
  cout<<"constant: "<<ApFit->GetParameter(0)<<" - scaler: "<<ApFit->GetParameter(4)<<endl;  
  //g_Aperture = 0;
  
  
  TH2D* BackgroundModel = new TH2D(TString("BackgroundModel") + m_iID, TString("BackgroundModel") + m_ID, 
                            m_NPositionDataBins, m_PosMin, m_PosMax, m_NPositionDataBins, m_PosMin, m_PosMax);
  BackgroundModel->SetXTitle("1, 2 <-- Det1 X [pixel] --> 3, 0");
  BackgroundModel->SetYTitle("2, 3 <-- Det1 Y [pixel] --> 1, 0");
  BackgroundModel->SetZTitle("cts");

  
  double SumModelAp = 0;
  double SumModelInt = 0;
  for (int bx = 0; bx <= BackgroundModel->GetNbinsX(); ++bx) {
    for (int by = 0; by <= BackgroundModel->GetNbinsX(); ++by) {
      double PosX = BackgroundModel->GetXaxis()->GetBinCenter(bx);
      double PosY = BackgroundModel->GetYaxis()->GetBinCenter(by);
      if ((PosX > m_ExcludeMin && PosX < m_ExcludeMax) || (PosY > m_ExcludeMin && PosY < m_ExcludeMax)) continue;
      
      double AddAp = 0;
      double AddInt = 0;
      if (s_CombinedFit == false) {
        if (PosX > m_Divider && PosY > m_Divider) {
          AddInt = ApFit->GetParameter(0);
          AddAp = ApFit->GetParameter(4)*s_Scaler0*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX < m_Divider && PosY > m_Divider) {
          AddInt = ApFit->GetParameter(1);
          AddAp = ApFit->GetParameter(4)*s_Scaler1*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX < m_Divider && PosY < m_Divider) {
          AddInt = ApFit->GetParameter(2);
          AddAp = ApFit->GetParameter(4)*s_Scaler2*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX > m_Divider && PosY < m_Divider) {
          AddInt = ApFit->GetParameter(3);
          AddAp = ApFit->GetParameter(4)*s_Scaler3*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        }
      } else {
        if (PosX > m_Divider && PosY > m_Divider) {
          AddInt = ApFit->GetParameter(0);
          AddAp = ApFit->GetParameter(4)*s_Scaler0*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX < m_Divider && PosY > m_Divider) {
          AddInt = ApFit->GetParameter(0);
          AddAp = ApFit->GetParameter(4)*s_Scaler1*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX < m_Divider && PosY < m_Divider) {
          AddInt = ApFit->GetParameter(0);
          AddAp = ApFit->GetParameter(4)*s_Scaler2*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        } else if (PosX > m_Divider && PosY < m_Divider) {
          AddInt = ApFit->GetParameter(0);
          AddAp = ApFit->GetParameter(4)*s_Scaler3*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
        }        
      }
      
      BackgroundModel->SetBinContent(bx, by, AddInt + AddAp);
      SumModelAp += AddAp;
      SumModelInt += AddInt;
    }
  }
  cout<<"Sum model: "<<SumModelAp + SumModelInt<<endl;
  
  
  TCanvas* BackgroundModelCanvas = new TCanvas(TString("BackgroundModelCanvas") + m_iID, TString("BackgroundModelCanvas") + m_ID, 600, 600);
  BackgroundModelCanvas->cd();
  BackgroundModel->Draw("colz");
  BackgroundModelCanvas->Update();
  
  // Prepare for subtraction
  double Scaler = SumBkg/(SumModelAp + SumModelInt)*SumModelAp/(s_Scaler0 + s_Scaler1 + s_Scaler2 + s_Scaler3);
  
  
  double SumSubtracted = 0;
  int BinsSubtracted = 0;
  
  for (int bx = 0; bx <= m_SubtractedPositions->GetNbinsX(); ++bx) {
    for (int by = 0; by <= m_SubtractedPositions->GetNbinsX(); ++by) {
      double PosX = m_SubtractedPositions->GetXaxis()->GetBinCenter(bx);
      double PosY = m_SubtractedPositions->GetYaxis()->GetBinCenter(by);
      if ((PosX > m_ExcludeMin && PosX < m_ExcludeMax) || (PosY > m_ExcludeMin && PosY < m_ExcludeMax)) continue;

      double ToSubtract = 0;
      if (PosX > m_Divider && PosY > m_Divider) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler0*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < m_Divider && PosY > m_Divider) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler1*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX < m_Divider && PosY < m_Divider) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler2*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      } else if (PosX > m_Divider && PosY < m_Divider) {
        ToSubtract = ApFit->GetParameter(4)*s_Scaler3*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
      }
      //ToSubtract = ApFit->GetParameter(4)*Aperture->GetBinContent(Aperture->GetXaxis()->FindBin(PosX), Aperture->GetYaxis()->FindBin(PosY));
     
      m_SubtractedPositions->SetBinContent(bx, by, m_Positions->GetBinContent(bx, by) - ToSubtract);
      if (m_SubtractedPositions->GetBinContent(bx, by) > 0) {
        SumSubtracted += m_SubtractedPositions->GetBinContent(bx, by);
        BinsSubtracted++;
      }
    }
  }
  if (BinsSubtracted > 0) {
    cout<<"Average subtracted content: "<<SumSubtracted/BinsSubtracted<<endl;
  }
  
  TCanvas* m_SubtractedPositionsCanvas = new TCanvas(TString("m_SubtractedPositionsCanvas") + m_iID, TString("m_SubtractedPositionsCanvas") + m_ID, 600, 600);
  m_SubtractedPositionsCanvas->cd();
  m_SubtractedPositions->Draw("colz");
  m_SubtractedPositionsCanvas->Update();

  cout<<Scaler<<":"<<ApFit->GetParameter(4)<<endl;
  Scaler = Scaler/ApFit->GetParameter(4);
  for (int e = 1; e <= m_SpectrumBins; ++e) {
    int Bin = m_SpectrumA0.m_Spectrum->FindBin(m_SpectrumAll->GetBinCenter(e));
    double Content = m_SpectrumAll->GetBinContent(e);
    Content -= Scaler*m_SpectrumA0.m_Spectrum->GetBinContent(Bin);
    Content -= Scaler*m_SpectrumA1.m_Spectrum->GetBinContent(Bin);
    Content -= Scaler*m_SpectrumA2.m_Spectrum->GetBinContent(Bin);
    Content -= Scaler*m_SpectrumA3.m_Spectrum->GetBinContent(Bin);
    
    //cout<<e<<": "<<m_SpectrumAll->GetBinContent(e)<<" --> "<<Content<<":"<<Scaler<<":"<<ApFit->GetParameter(4)<<endl;
    m_SubtractedSpectrumAll->SetBinContent(e, Content); 
  }
  cout<<"New scaler: "<<Scaler<<endl;

    
  TCanvas* SubtractedSpectrumAllCanvas = new TCanvas(TString("SubtractedSpectrumAllCanvas") + m_iID, TString("SubtractedSpectrumAllCanvas") + m_ID, 600, 600);
  SubtractedSpectrumAllCanvas->cd();  
  m_SpectrumAll->Draw();
  m_SubtractedSpectrumAll->Draw("SAME");
  SubtractedSpectrumAllCanvas->Update();
  
  
  return true;
}

  
////////////////////////////////////////////////////////////////////////////////


double NApertureModel::GetApertureShape(int detx, int dety, int Module)
{
  double p[] = { 1.0, 1.0, 0.0, 0.0, 1.28, -1.23, 4.55, 0.70 };
  //double p[] = { 1.8247618,1.8456988,6.5860895,6.1775471,-0.094825531,-0.26157041,26.617317,2.3922457 };

  // 70x70 mm polynomial fit, z-distance reduced to use optic backside
  double acoeff[6][6] = { 
    { 374459.,    -1396.94,     12.8261,    -2.60828,   0.0499416,-0.000233391 }, 
    {-968.233,     63.0611,    -31.1783,     1.33731,  -0.0210902, 0.000113852 }, 
    {-61.1686,    -19.9369,     2.79334,   -0.120849,  0.00194248,-1.03889e-05 }, 
    {1.84454,    0.559458,  -0.0719477,  0.00305963,-4.88927e-05, 2.58232e-07 },
    {-0.00628727, -0.00408784, 0.000572117,-2.29168e-05, 3.50316e-07,-1.74875e-09 },
    {-6.81786e-05,-1.16308e-07,-3.66437e-07, 4.21674e-11, 1.87926e-10,-2.18813e-12 }
  };


  double bcoeff[6][6] = {
    {     145375.,    -2845.75,    -203.809,     13.3728,   -0.202717, 0.000997269 }, 
    {    -6128.16,    -1170.23,     84.8699,    -2.24046,   0.0267836,-0.000120917 }, 
    {     112.675,     97.2683,    -8.47097,    0.255571, -0.00329352, 1.54470e-05 }, 
    {     6.11565,    -2.86178,    0.276812, -0.00896906, 0.000120047,-5.74844e-07 }, 
    {   -0.136972,   0.0356496, -0.00367424, 0.000123755,-1.68958e-06, 8.17812e-09 }, 
    { 0.000771634,-0.000159447, 1.72740e-05,-5.96966e-07, 8.25131e-09,-4.02023e-11 }
  };

  double coeff[6][6];
  double xp = 0;
  double yp = 0;
  double scl = 0;
  double off = 0;
  if (Module == 0) {
    for (int x = 0; x < 6; ++x) for (int y = 0; y < 6; ++y) coeff[x][y] = acoeff[x][y];
    xp=p[4];
    yp=p[5];
    scl=p[0];
    off=p[2];
  } else if (Module == 1) {
    for (int x = 0; x < 6; ++x) for (int y = 0; y < 6; ++y) coeff[x][y] = bcoeff[x][y];
    xp=p[6];
    yp=p[7];
    scl=p[1];
    off=p[3];   
  }
  
  double dx = 0.12096; // mm/(det1 pixel)
  double val = 0;
  for (int i= 0; i <= 5; ++i) { 
    for (int j = 0; j <= 5; ++j) {
      if (Module == 0) { 
        val += coeff[j][i]*pow(dety*dx+13+xp, i)*pow(detx*dx+13+yp, j);
      } else {
        val += coeff[j][i]*pow(dety*dx+13+xp, i)*pow(detx*dx+13+yp, j);
      }
    }
  }
      
  return (val*scl*3.1865e-5 + off)*0.01; // *dx*dx
}
 
  
// NApertureModel.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
  