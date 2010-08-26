/*
 * NGUIDiagnosticsDetectorEffectsEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsDetectorEffectsEngine.h"

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
#include "NModuleDetectorEffectsEngineIdeal.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsDetectorEffectsEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsDetectorEffectsEngine::NGUIDiagnosticsDetectorEffectsEngine() : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "DEE 1";

  // Add all histograms and canvases below
  m_PositionsBefore = new TH2D("PositionsBeforeDEE", "Positions before", 200, -25, 25, 200, -25, 25);
  m_PositionsBefore->SetXTitle("[mm]");
  m_PositionsBefore->SetYTitle("[mm]");
  m_PositionsBefore->SetZTitle("counts");

  m_PositionsAfter = new TH2D("PositionsAfterDEE", "Positions after", 200, -25, 25, 200, -25, 25);
  m_PositionsAfter->SetXTitle("[mm]");
  m_PositionsAfter->SetYTitle("[mm]");
  m_PositionsAfter->SetZTitle("counts");

  m_PositionsBeforeCanvas = 0;
  m_PositionsAfterCanvas = 0;

  m_DepthBefore = new TH1D("DepthsBeforeDEE", "Depth before (per interaction)", 100, -1.0, 1.0);
  m_DepthBefore->SetXTitle("[mm]");
  m_DepthBefore->SetYTitle("counts");
  m_DepthBefore->SetMinimum(0);

  m_DepthAfter = new TH1D("DepthsAfterDEE", "Depth after (per pixel)", 100, -1.0, 1.0);
  m_DepthAfter->SetXTitle("[mm]");
  m_DepthAfter->SetYTitle("counts");
  m_DepthAfter->SetMinimum(0);

  m_DepthBeforeCanvas = 0;
  m_DepthAfterCanvas = 0;

  m_EnergyBefore = new TH1D("EnergyBeforeDEE", "Energy before (per interaction)", 300, 0, 150);
  m_EnergyBefore->SetXTitle("[keV]");
  m_EnergyBefore->SetYTitle("counts");
  m_EnergyBefore->SetMinimum(0);

  m_EnergyAfter = new TH1D("EnergyAfterDEE", "Energy after (per pixel)", 300, 0, 150);
  m_EnergyAfter->SetXTitle("[keV]");
  m_EnergyAfter->SetYTitle("counts");
  m_EnergyAfter->SetMinimum(0);

  m_EnergyBeforeCanvas = 0;
  m_EnergyAfterCanvas = 0;

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsDetectorEffectsEngine::~NGUIDiagnosticsDetectorEffectsEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsDetectorEffectsEngine::SetDetectorParameters(const MVector& Center, const MVector& Size, const int NPixelsX, const int NPixelsY)
{
  //! Set the detector parameters for improved display
  
  cout<<Center<<":"<<Size<<":"<<NPixelsX<<":"<<NPixelsY<<endl;
  
  double xCenter = fabs(Center.X());
  double xPixelSize = Size.X()/(0.5*NPixelsX);
  vector<double> xAxis;
  xAxis.push_back(-2*xCenter);
  xAxis.push_back(-xCenter - Size.X());
  for (int i = 0; i < NPixelsX; ++i) {
    xAxis.push_back(xAxis.back() + xPixelSize);
  }
  xAxis.push_back(0);
  xAxis.push_back(xCenter - Size.X());
  for (int i = 0; i < NPixelsX; ++i) {
    xAxis.push_back(xAxis.back() + xPixelSize);
  }
  xAxis.push_back(2*xCenter);
	
  double yCenter = fabs(Center.Y());
  double yPixelSize = Size.Y()/(0.5*NPixelsY);
  vector<double> yAxis;
  yAxis.push_back(-2*yCenter);
  yAxis.push_back(-yCenter - Size.Y());
  for (int i = 0; i < NPixelsY; ++i) {
    yAxis.push_back(yAxis.back() + yPixelSize);
  }
  yAxis.push_back(0);
  yAxis.push_back(yCenter - Size.Y());
  for (int i = 0; i < NPixelsY; ++i) {
    yAxis.push_back(yAxis.back() + yPixelSize);
  }
  yAxis.push_back(2*yCenter);
 
  delete m_PositionsAfter;
  m_PositionsAfter = new TH2D("PositionsAfterDEE", "Positions after (1 bin = 1 pixel)", xAxis.size()-1, &xAxis[0], yAxis.size()-1, &yAxis[0]);
  m_PositionsAfter->SetXTitle("[mm]");
  m_PositionsAfter->SetYTitle("[mm]");
  m_PositionsAfter->SetZTitle("counts");
  
  
//   int Scaler = 3;
//   xPixelSize /= Scaler;
//   xAxis.clear();
//   xAxis.push_back(-2*xCenter);
//   xAxis.push_back(-xCenter - Size.X());
//   for (int i = 0; i < Scaler*NPixelsX; ++i) {
//     xAxis.push_back(xAxis.back() + xPixelSize);
//   }
//   xAxis.push_back(0);
//   xAxis.push_back(xCenter - Size.X());
//   for (int i = 0; i < Scaler*NPixelsX; ++i) {
//     xAxis.push_back(xAxis.back() + xPixelSize);
//   }
//   xAxis.push_back(2*xCenter);

//   yPixelSize /= Scaler;
//   yAxis.clear();
//   yAxis.push_back(-2*yCenter);
//   yAxis.push_back(-yCenter - Size.Y());
//   for (int i = 0; i < Scaler*NPixelsY; ++i) {
//     yAxis.push_back(yAxis.back() + yPixelSize);
//   }
//   yAxis.push_back(0);
//   yAxis.push_back(yCenter - Size.Y());
//   for (int i = 0; i < Scaler*NPixelsY; ++i) {
//     yAxis.push_back(yAxis.back() + yPixelSize);
//   }
//   yAxis.push_back(2*yCenter);
  
//   delete m_PositionsBefore;
//   m_PositionsBefore = new TH2D("PositionsBeforeDEE", "Positions before", xAxis.size()-1, &xAxis[0], yAxis.size()-1, &yAxis[0]);
//   m_PositionsBefore->SetXTitle("[mm]");
//   m_PositionsBefore->SetYTitle("[mm]");
//   m_PositionsBefore->SetZTitle("counts");
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsDetectorEffectsEngine::AddBefore(MVector Dir, double Energy)
{
  //! Add data to the initial direction histogram 

  m_PositionsBefore->Fill(Dir.X(), Dir.Y());
  m_DepthBefore->Fill(Dir.Z());
  m_EnergyBefore->Fill(Energy);
}

////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsDetectorEffectsEngine::AddAfter(MVector Dir, double Energy)
{
  //! Add data to the backprojection histogram

  m_PositionsAfter->Fill(Dir.X(), Dir.Y());
  m_DepthAfter->Fill(Dir.Z());
  m_EnergyAfter->Fill(Energy);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsDetectorEffectsEngine::Create()
{
  // Add the GUI options here

  TGLayoutHints* CanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
                                                  2, 2, 2, 2);


  TGHorizontalFrame* BeforeFrame = new TGHorizontalFrame(this);
  AddFrame(BeforeFrame, CanvasLayout);
  
  TGHorizontalFrame* AfterFrame = new TGHorizontalFrame(this);
  AddFrame(AfterFrame, CanvasLayout);
  
  m_PositionsBeforeCanvas = new TRootEmbeddedCanvas("PositionsBefore", BeforeFrame, 100, 100);
  BeforeFrame->AddFrame(m_PositionsBeforeCanvas, CanvasLayout);
  m_PositionsBeforeCanvas->GetCanvas()->cd();
  m_PositionsBefore->Draw("colz");
  m_PositionsBeforeCanvas->GetCanvas()->Update();

  m_DepthBeforeCanvas = new TRootEmbeddedCanvas("DepthBefore", BeforeFrame, 100, 100);
  BeforeFrame->AddFrame(m_DepthBeforeCanvas, CanvasLayout);
  m_DepthBeforeCanvas->GetCanvas()->cd();
  m_DepthBefore->Draw();
  m_DepthBeforeCanvas->GetCanvas()->Update();

  m_EnergyBeforeCanvas = new TRootEmbeddedCanvas("EnergyBefore", BeforeFrame, 100, 100);
  BeforeFrame->AddFrame(m_EnergyBeforeCanvas, CanvasLayout);
  m_EnergyBeforeCanvas->GetCanvas()->cd();
  m_EnergyBefore->Draw();
  m_EnergyBeforeCanvas->GetCanvas()->Update();
  

  m_PositionsAfterCanvas = new TRootEmbeddedCanvas("PositionsAfter", AfterFrame, 100, 100);
  AfterFrame->AddFrame(m_PositionsAfterCanvas, CanvasLayout);
  m_PositionsAfterCanvas->GetCanvas()->cd();
  m_PositionsAfter->Draw("colz");
  m_PositionsAfterCanvas->GetCanvas()->Update();

  m_DepthAfterCanvas = new TRootEmbeddedCanvas("DepthAfter", AfterFrame, 100, 100);
  AfterFrame->AddFrame(m_DepthAfterCanvas, CanvasLayout);
  m_DepthAfterCanvas->GetCanvas()->cd();
  m_DepthAfter->Draw();
  m_DepthAfterCanvas->GetCanvas()->Update();

  m_EnergyAfterCanvas = new TRootEmbeddedCanvas("EnergyAfter", AfterFrame, 100, 100);
  AfterFrame->AddFrame(m_EnergyAfterCanvas, CanvasLayout);
  m_EnergyAfterCanvas->GetCanvas()->cd();
  m_EnergyAfter->Draw();
  m_EnergyAfterCanvas->GetCanvas()->Update();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsDetectorEffectsEngine::Update()
{
  //! Update the frame

  if (m_PositionsBeforeCanvas != 0) {
    m_PositionsBeforeCanvas->GetCanvas()->Modified();
    m_PositionsBeforeCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
  if (m_DepthBeforeCanvas != 0) {
    m_DepthBeforeCanvas->GetCanvas()->Modified();
    m_DepthBeforeCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
  if (m_EnergyBeforeCanvas != 0) {
    m_EnergyBeforeCanvas->GetCanvas()->Modified();
    m_EnergyBeforeCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }

  if (m_PositionsAfterCanvas != 0) {
    m_PositionsAfterCanvas->GetCanvas()->Modified();
    m_PositionsAfterCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
  if (m_DepthAfterCanvas != 0) {
    m_DepthAfterCanvas->GetCanvas()->Modified();
    m_DepthAfterCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
  if (m_EnergyAfterCanvas != 0) {
    m_EnergyAfterCanvas->GetCanvas()->Modified();
    m_EnergyAfterCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
}


// NGUIDiagnosticsDetectorEffectsEngine: the end...
////////////////////////////////////////////////////////////////////////////////
