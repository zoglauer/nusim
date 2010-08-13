/*
 * NGUIDiagnosticsSourceDistribution.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsSourceDistribution.h"

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
#include "NModuleSourceDistribution.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsSourceDistribution)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsSourceDistribution::NGUIDiagnosticsSourceDistribution() : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "Source";

  // Add all histograms and canvases below
  m_Origin = new TH2D("Origin distribution", "Origin distribution", 161, -8, 8, 161, -8, 8);
  m_Origin->SetXTitle("RA - relative to initial pointing [arcmin]");
  m_Origin->SetYTitle("DEC - relative to initial pointing [arcmin]");

  m_Energy = new TH1D("Spectrum", "Spectrum", 148, 6, 80);
  m_Energy->SetXTitle("keV");
  m_Energy->SetYTitle("counts");

  m_OriginCanvas = 0;
  m_EnergyCanvas = 0;
  
  m_InitialRa = 0;
  m_InitialDec = 0;

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsSourceDistribution::~NGUIDiagnosticsSourceDistribution()
{
  // kDeepCleanup is activated 
}



////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsSourceDistribution::AddOrigin(double Ra, double Dec)
{
  //! Add data to the initial direction histogram 

  m_Origin->Fill(Ra - m_InitialRa, Dec - m_InitialDec);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsSourceDistribution::AddEnergy(double Energy)
{
  //! Add data to the backprojection histogram

  m_Energy->Fill(Energy);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsSourceDistribution::Create()
{
  // Add the GUI options here

  TGLayoutHints* CanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
                                                  2, 2, 2, 2);

  
  m_OriginCanvas = new TRootEmbeddedCanvas("Origin", this, 100, 100);
  AddFrame(m_OriginCanvas, CanvasLayout);

  m_OriginCanvas->GetCanvas()->cd();
  m_Origin->Draw("colz");
  m_OriginCanvas->GetCanvas()->Update();

  
  m_EnergyCanvas = new TRootEmbeddedCanvas("Energy", this, 100, 100);
  AddFrame(m_EnergyCanvas, CanvasLayout);

  m_EnergyCanvas->GetCanvas()->cd();
  m_EnergyCanvas->GetCanvas()->SetGridy();
  m_Energy->Draw();
  m_EnergyCanvas->GetCanvas()->Update();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsSourceDistribution::Update()
{
  //! Update the frame

  if (m_OriginCanvas != 0) {
    m_OriginCanvas->GetCanvas()->Modified();
    m_OriginCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }

  if (m_EnergyCanvas != 0) {
    m_EnergyCanvas->GetCanvas()->Modified();
    m_EnergyCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
}


// NGUIDiagnosticsSourceDistribution: the end...
////////////////////////////////////////////////////////////////////////////////
