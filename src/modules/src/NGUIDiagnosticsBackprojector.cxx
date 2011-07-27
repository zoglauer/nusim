/*
 * NGUIDiagnosticsBackprojector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsBackprojector.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TGResourcePool.h>
#include <TCanvas.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MTime.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModuleBackprojector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsBackprojector)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsBackprojector::NGUIDiagnosticsBackprojector(double PixelSize) : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "Results";

  m_FixedSize = false;
  m_NeedsResize = false;
  
  m_MaxBins = 4096;

  m_MinDec = -10;
  m_MaxDec = 10;
  m_BinSizeDec = PixelSize/60;
  
  m_MinRa = -10;
  m_MaxRa = 10;
  m_BinSizeRa = PixelSize/60;
  
  m_Area = 0;
  m_Time.Set(0.0);

  // Add all histograms and canvases below
  m_Backprojection = new TH2D("Backprojection", TString("Backprojected hits after T_{eff} = ") + m_Time.GetString(3), 
                              int(m_BinSizeRa*(m_MaxRa-m_MinRa)), m_MinRa, m_MaxRa, 
                              int(m_BinSizeDec*(m_MaxDec-m_MinDec)), m_MinDec, m_MaxDec);
  m_Backprojection->SetXTitle("RA [deg]");
  m_Backprojection->SetYTitle("DEC [deg]");
  m_Backprojection->SetZTitle("counts");

  m_Energy = new TH1D("Spectrum", "Spectrum", 194, 3, 100);
  m_Energy->SetXTitle("keV");
  m_Energy->SetYTitle("counts");

  m_NormalizedEnergy = new TH1D("Spectrum", "Spectrum", 194, 3, 100);
  m_NormalizedEnergy->SetXTitle("keV");
  m_NormalizedEnergy->SetYTitle("counts/keV/s/cm^{2}");
  m_NormalizedEnergy->SetMinimum(0);

  m_EnergyWidth = m_NormalizedEnergy->GetXaxis()->GetXmax() - m_NormalizedEnergy->GetXaxis()->GetXmin();

  m_BackprojectionCanvas = 0;
  m_EnergyCanvas = 0;
  
  m_InitialRa = 0;
  m_InitialDec = 0;

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsBackprojector::~NGUIDiagnosticsBackprojector()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::SetInitialPointing(double Ra, double Dec) 
{ 
  //! Set the initial pointing 

  Ra /= 60.0;
  Dec /= 60.0;

  m_MinDec = Dec - 8.0/60.0;
  m_MaxDec = Dec + 8.0/60.0;

  double Scaler = 1.0/cos(Dec*c_Rad);
  
  if (cos(Dec*c_Rad) == 0) { 
    m_MinRa = 0.0;
    m_MaxRa = 360.0;
  } else {
    Scaler = 1.0/cos(Dec*c_Rad);
    m_MinRa = Ra - Scaler*8.0/60.0;
    m_MaxRa = Ra + Scaler*8.0/60.0;
  }



  //cout<<"Initial Min/Max:"<<endl;
  //cout<<m_MinRa<<":"<<Ra<<":"<<m_MaxRa<<endl;
  //cout<<m_MinDec<<":"<<Dec<<":"<<m_MaxDec<<endl;

  int BinsDec = ceil((m_MaxDec - m_MinDec)/m_BinSizeDec);
  int BinsRa = ceil((m_MaxRa - m_MinRa)/m_BinSizeDec/Scaler);
      
  if (m_MinRa < 0.0) m_MinRa = 0.0;
  if (m_MaxRa > 360.0) m_MaxRa = 360.0;
  if (m_MinDec < -90) m_MinDec = -90;
  if (m_MaxDec > 90) m_MaxDec = 90;

  // Limit the number of bins, or we spend for ever updating the histogram...
  if (BinsDec > m_MaxBins) {
    mout<<"Max. number of DEC bins reached: "<<m_MaxBins<<endl;
    BinsDec = m_MaxBins;
  }
  if (BinsRa > m_MaxBins) {
    mout<<"Max. number of RA bins reached: "<<m_MaxBins<<endl;
    BinsRa = m_MaxBins;
  }

  if (m_MinRa >= m_MaxRa) {
    merr<<"Ra-axis problem: Min (="<<m_MinRa<<") >= Max (="<<m_MaxRa<<")"<<show;
  }
  if (m_MinRa >= m_MaxRa) {
    merr<<"Dec-axis problem: Min (="<<m_MinDec<<") >= Max (="<<m_MaxDec<<")"<<show;
  }

  m_Backprojection->Reset();
  m_Backprojection->SetBins(BinsRa, m_MinRa, m_MaxRa, BinsDec, m_MinDec, m_MaxDec);

  m_InitialRa = Ra; 
  m_InitialDec = Dec; 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::SetDetectorParameters(const MVector& Center, const MVector& HalfSize, const int NPixelsX, const int NPixelsY)
{
  //! Set the detector parameters for improved display

  // two telescopes, 4 detectors, ...
  m_Area = 2 * 4 * 2*HalfSize.X() * 2*HalfSize.Y();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::AddBackprojection(double Ra, double Dec)
{
  //! Add data to the backprojection histogram

  // If this is the first event, and the initial pointing is more than 0.5 deg off
  // then we use the parameters of the first hit as initial pointing
  if (m_Ra.size() == 0) {
    if (fabs(m_InitialDec - Dec) > 0.5*60 || (cos(Dec*c_Rad) != 0 && fabs(m_InitialRa - Ra)/cos(Dec*c_Rad) > 0.5*60)) {
      SetInitialPointing(Ra, Dec);
    }
  }


  Ra /= 60;
  Dec /= 60;
  
  m_Ra.push_back(Ra);
  if (Ra < m_MinRa) {
    m_MinRa = Ra;
    m_NeedsResize = true;
    if (m_MinRa < 0.0) m_MinRa = 0.0;
    //cout<<"New Ra min: "<<Ra<<endl;
  }
  if (Ra > m_MaxRa) {
    m_MaxRa = Ra;
    m_NeedsResize = true;
    if (m_MaxRa > 360.0) m_MaxRa = 360.0;
    //cout<<"New Ra max: "<<Ra<<endl;
  }
  m_Dec.push_back(Dec);
  if (Dec < m_MinDec) {
    m_MinDec = Dec;
    m_NeedsResize = true;
    if (m_MinDec < -90) m_MinDec = -90;
    //cout<<"New Dec min: "<<Dec<<endl;
  }
  if (Dec > m_MaxDec) {
    m_MaxDec = Dec;
    m_NeedsResize = true;
    if (m_MaxDec > 90) m_MaxDec = 90;
    //cout<<"New Dec max: "<<Dec<<endl;
  }
  
  //cout<<Ra<<":"<<Dec<<endl;
  m_Backprojection->Fill(Ra, Dec);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::AddEnergy(double Energy)
{
  //! Add data to the backprojection histogram

  m_Energy->Fill(Energy);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::Create()
{
  // Add the GUI options here

  TGLayoutHints* CanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
                                                  2, 2, 2, 2);

  TGHorizontalFrame* HFrame = new TGHorizontalFrame(this);
  AddFrame(HFrame, CanvasLayout);

                                                  
  m_BackprojectionCanvas = new TRootEmbeddedCanvas("Final", HFrame, 100, 100);
  HFrame->AddFrame(m_BackprojectionCanvas, CanvasLayout);

  m_BackprojectionCanvas->GetCanvas()->cd();
  m_Backprojection->Draw("colz");
  m_BackprojectionCanvas->GetCanvas()->Update();

  m_EnergyCanvas = new TRootEmbeddedCanvas("Energy", HFrame, 100, 100);
  HFrame->AddFrame(m_EnergyCanvas, CanvasLayout);

  m_EnergyCanvas->GetCanvas()->cd();
  m_EnergyCanvas->GetCanvas()->SetGridy();
  m_NormalizedEnergy->Draw();
  m_EnergyCanvas->GetCanvas()->Update();
  
  Update();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::Update()
{
  //! Update the frame

  if (m_BackprojectionCanvas != 0) {
    if (m_NeedsResize == true && m_FixedSize == false) {
      
      //cout<<"Before:"<<endl;
      //cout<<m_MinRa<<":"<<m_MaxRa<<endl;
      //cout<<m_MinDec<<":"<<m_MaxDec<<endl;

      
      // First make sure the dimensions are identical
      double DimDec = m_MaxDec - m_MinDec;
      double DimRa = m_MaxRa - m_MinRa;
      double Scaler = 1.0/cos(0.5*(m_MaxDec+m_MinDec)*c_Rad);
      double DimDiff = DimDec - DimRa/Scaler;
      if (DimDiff > 0.0) {
        // Increase RA
        m_MaxRa += 0.5*DimDiff*Scaler;
        m_MinRa -= 0.5*DimDiff*Scaler;
        if (m_MinRa < 0.0) m_MinRa = 0.0;
        if (m_MaxRa > 360.0) m_MaxRa = 360.0;
      } else {
        // Increase DEC
        m_MaxDec += 0.5*fabs(DimDiff/Scaler);
        m_MinDec -= 0.5*fabs(DimDiff/Scaler);
        if (m_MaxDec > 90) m_MaxDec = 90;
        if (m_MinDec < -90) m_MinDec = -90;
        Scaler = 1.0/cos(0.5*(m_MaxDec+m_MinDec)*c_Rad);
      }
      
      //cout<<"After:"<<endl;
      //cout<<m_MinRa<<":"<<m_MaxRa<<endl;
      //cout<<m_MinDec<<":"<<m_MaxDec<<endl;

      
      int BinsDec = ceil((m_MaxDec - m_MinDec)/m_BinSizeDec);
      int BinsRa = ceil((m_MaxRa - m_MinRa)/m_BinSizeDec/Scaler);
  
      if (m_MinRa >= m_MaxRa) {
        merr<<"Ra-axis problem: Min (="<<m_MinRa<<") >= Max (="<<m_MaxRa<<")"<<show;
      }
      if (m_MinRa >= m_MaxRa) {
        merr<<"Dec-axis problem: Min (="<<m_MinDec<<") >= Max (="<<m_MaxDec<<")"<<show;
      }
      
      // Limit the number of bins, or we spend for ever updating the histogram...
      if (BinsDec > m_MaxBins) {
        mout<<"Max. number of DEC bins reached: "<<m_MaxBins<<endl;
        BinsDec = m_MaxBins;
      }
      if (BinsRa > m_MaxBins) {
        mout<<"Max. number of RA bins reached: "<<m_MaxBins<<endl;
        BinsRa = m_MaxBins;
      }

      
      m_Backprojection->Reset();
      m_Backprojection->SetBins(BinsRa, m_MinRa, m_MaxRa, BinsDec, m_MinDec, m_MaxDec);
      
      for (unsigned int i = 0; i < m_Ra.size(); ++i) {
        m_Backprojection->Fill(m_Ra[i], m_Dec[i]);
      }
    }
    // Add a miniscule value to empty bins to ensure a non-white background color
    for (int x = 1; x <= m_Backprojection->GetXaxis()->GetNbins(); ++x) {
      for (int y = 1; y <= m_Backprojection->GetYaxis()->GetNbins(); ++y) {
        if (m_Backprojection->GetBinContent(x, y) == 0) {
          m_Backprojection->SetBinContent(x, y, 1E-10);
        }
      }
    }
    m_Backprojection->SetContour(50);
    
    m_Backprojection->SetTitle(TString("Backprojected hits after T_{eff} = ") + m_Time.GetString(3));
    m_BackprojectionCanvas->GetCanvas()->Modified();
    m_BackprojectionCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }

  if (m_EnergyCanvas != 0) {
    if (m_EnergyWidth != 0.0 && m_Area != 0.0 && m_Time.GetAsSeconds() != 0.0) {
      int counts = 0;

      for (int b = 1; b <= m_NormalizedEnergy->GetNbinsX(); ++b) {
        counts += m_Energy->GetBinContent(b);
        m_NormalizedEnergy->SetBinContent(b, m_Energy->GetBinContent(b)/m_Energy->GetBinWidth(b)/(m_Area/100)/m_Time.GetAsSeconds());
      }

      /*
      // Dump to file:
      ofstream out;
      out.open("BackgroundDump.txt");
      out<<"# Name                 NuSTAR background file - XXX case"<<endl;    
      out<<""<<endl;
      MTime Now;   
      out<<"# Date                 "<<Now.GetYears()<<"-"<<Now.GetMonths()<<"-"<<Now.GetDays()<<endl;              
      out<<"# Creator              NuSIM - XXX thread"<<endl;            
      out<<"# Revision             "<<g_SVNRevision<<endl;             
      out<<" "<<endl;              
      out<<"# First column         Energy bin center in keV"<<endl;               
      out<<"# Second column        Rate in cts/s/keV/cm2"<<endl;         
      out<<"# Bin width            "<<m_NormalizedEnergy->GetBinWidth(1)<<" keV"<<endl; 
      out<<" "<<endl;               
      out<<"# Description          Best guess assumptions for input background fluxes simulated with MGGPOD"<<endl;         
      out<<" "<<endl;  
      out<<"# Attention:"<<endl;            
      out<<"# This background excludes the aperture flux"<<endl;            
      out<<"# This background excludes any depth cuts"<<endl;   
      out<<" "<<endl; 
      for (int b = 1; b <= m_NormalizedEnergy->GetNbinsX(); ++b) {
        out<<m_NormalizedEnergy->GetBinCenter(b)<<"  "<<m_NormalizedEnergy->GetBinContent(b)<<endl;
      }
      out.close();
      */
      
//       cout<<"Counts: "<<counts<<endl; 
//       cout<<"Counts/sec: "<<counts/m_Time.GetAsSeconds()<<" --> "<<m_Time.GetAsSeconds()<<" s"<<endl; 
//       cout<<"Counts/sec/cm2: "<<counts/m_Time.GetAsSeconds()/(m_Area/100)<<" --> "<<m_Area/100<<" cm2"<<endl; 
//       cout<<"Counts/sec/cm2/keV: "<<counts/m_Time.GetAsSeconds()/(m_Area/100)/m_EnergyWidth<<" --> "<<m_EnergyWidth<<" keV"<<endl; 
//     } else {
//       cout<<"Cannot show normalized energy histogram. "<<endl;
    }


    m_EnergyCanvas->GetCanvas()->Modified();
    m_EnergyCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
}


// NGUIDiagnosticsBackprojector: the end...
////////////////////////////////////////////////////////////////////////////////
