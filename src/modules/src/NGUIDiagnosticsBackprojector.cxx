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

// NuSTAR libs:
#include "NModule.h"
#include "NModuleBackprojector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsBackprojector)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsBackprojector::NGUIDiagnosticsBackprojector() : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "Backprojections";

  // Add all histograms and canvases below
  m_InitialDistribution = new TH2D("Initial distribution", "Initial distribution", 161, -8, 8, 161, -8, 8);
  m_InitialDistribution->SetXTitle("[arcmin]");
  m_InitialDistribution->SetYTitle("[arcmin]");

  m_Backprojection = new TH2D("Backprojection", "Backprojection", 321, -8, 8, 321, -8, 8);
  m_Backprojection->SetXTitle("[arcmin]");
  m_Backprojection->SetYTitle("[arcmin]");

  m_Energy = new TH1D("Spectrum", "Spectrum", 148, 6, 80);
  m_Energy->SetXTitle("keV");
  m_Energy->SetYTitle("counts");

  m_NormalizedEnergy = new TH1D("Spectrum", "Spectrum", 148, 6, 80);
  m_NormalizedEnergy->SetXTitle("keV");
  m_NormalizedEnergy->SetYTitle("counts/keV/s/cm^{2}");
  m_NormalizedEnergy->SetMinimum(0);

  m_EnergyWidth = m_NormalizedEnergy->GetXaxis()->GetXmax() - m_NormalizedEnergy->GetXaxis()->GetXmin();

  m_InitialCanvas = 0;
  m_BackprojectionCanvas = 0;
  m_EnergyCanvas = 0;

  m_Area = 0;
  m_Time = 0;

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsBackprojector::~NGUIDiagnosticsBackprojector()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::SetDetectorParameters(const MVector& Center, const MVector& HalfSize, const int NPixelsX, const int NPixelsY)
{
  //! Set the detector parameters for improved display

  // two telescopes, 4 detectors, ...
  m_Area = 2 * 4 * 2*HalfSize.X() * 2*HalfSize.Y();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::AddInitialDirection(MVector Dir)
{
  //! Add data to the initial direction histogram 

  m_InitialDistribution->Fill(Dir.Theta()*cos(Dir.Phi())*c_Deg*60, Dir.Theta()*sin(Dir.Phi())*c_Deg*60);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::AddBackprojection(MVector Dir)
{
  //! Add data to the backprojection histogram

  m_Backprojection->Fill(Dir.Theta()*cos(Dir.Phi())*c_Deg*60, Dir.Theta()*sin(Dir.Phi())*c_Deg*60);
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

  
  m_InitialCanvas = new TRootEmbeddedCanvas("Initial", this, 100, 100);
  AddFrame(m_InitialCanvas, CanvasLayout);

  m_InitialCanvas->GetCanvas()->cd();
  m_InitialDistribution->Draw("colz");
  m_InitialCanvas->GetCanvas()->Update();

  m_BackprojectionCanvas = new TRootEmbeddedCanvas("Final", this, 100, 100);
  AddFrame(m_BackprojectionCanvas, CanvasLayout);

  m_BackprojectionCanvas->GetCanvas()->cd();
  m_Backprojection->Draw("colz");
  m_BackprojectionCanvas->GetCanvas()->Update();

  m_EnergyCanvas = new TRootEmbeddedCanvas("Energy", this, 100, 100);
  AddFrame(m_EnergyCanvas, CanvasLayout);

  m_EnergyCanvas->GetCanvas()->cd();
  m_EnergyCanvas->GetCanvas()->SetGridy();
  m_NormalizedEnergy->Draw();
  m_EnergyCanvas->GetCanvas()->Update();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsBackprojector::Update()
{
  //! Update the frame

  if (m_InitialCanvas != 0) {
    m_InitialCanvas->GetCanvas()->Modified();
    m_InitialCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }

  if (m_BackprojectionCanvas != 0) {
    m_BackprojectionCanvas->GetCanvas()->Modified();
    m_BackprojectionCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }

  if (m_EnergyCanvas != 0) {
    if (m_EnergyWidth != 0.0 && m_Area != 0.0 && m_Time.GetSeconds() != 0.0) {
      int counts = 0;

      for (int b = 1; b <= m_NormalizedEnergy->GetNbinsX(); ++b) {
        counts += m_Energy->GetBinContent(b);
        m_NormalizedEnergy->SetBinContent(b, m_Energy->GetBinContent(b)/m_Energy->GetBinWidth(b)/(m_Area/100)/m_Time.GetSeconds());
      }

//       cout<<"Counts: "<<counts<<endl; 
//       cout<<"Counts/sec: "<<counts/m_Time.GetSeconds()<<" --> "<<m_Time.GetSeconds()<<" s"<<endl; 
//       cout<<"Counts/sec/cm2: "<<counts/m_Time.GetSeconds()/(m_Area/100)<<" --> "<<m_Area/100<<" cm2"<<endl; 
//       cout<<"Counts/sec/cm2/keV: "<<counts/m_Time.GetSeconds()/(m_Area/100)/m_EnergyWidth<<" --> "<<m_EnergyWidth<<" keV"<<endl; 
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
