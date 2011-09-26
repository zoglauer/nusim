/*
 * NGUIDiagnosticsOrbitEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsOrbitEngine.h"

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


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsOrbitEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsOrbitEngine::NGUIDiagnosticsOrbitEngine() : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "Orbit";

  m_OccultationTime = new TH1D("Occultation time", "Can the object be observed?", 1, -0, 1.0);
  m_OccultationTime->SetXTitle("Time since observation start [sec]");
  m_OccultationTime->SetYTitle("Is NOT occulted");
  m_OccultationTime->SetMinimum(0);

  m_NightTime = new TH1D("DepthsAfterDEE", "Is the satellite on the day side of Earth?", 1, -0, 1.0);
  m_NightTime->SetXTitle("Time since observation start [sec]");
  m_NightTime->SetYTitle("Is Day");
  m_NightTime->SetMinimum(0);

  m_OccultationTimeCanvas = 0;
  m_NightTimeCanvas = 0;

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsOrbitEngine::~NGUIDiagnosticsOrbitEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsOrbitEngine::AddOrbitStatus(const NTime& Time, bool IsOcculted, bool IsNight)
{
  //! Add data to the initial direction histogram 

  if (m_Time.size() >= 50000) {
    m_Time.pop_front();
    m_IsOcculted.pop_front();
    m_IsNight.pop_front();
  }

  m_Time.push_back(Time);
  m_IsOcculted.push_back(IsOcculted);
  m_IsNight.push_back(IsNight);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsOrbitEngine::Create()
{
  // Add the GUI options here

  Update();

  TGLayoutHints* CanvasLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY,
                                                  2, 2, 2, 2);

  m_OccultationTimeCanvas = new TRootEmbeddedCanvas("PositionsBefore", this, 100, 100);
  this->AddFrame(m_OccultationTimeCanvas, CanvasLayout);
  m_OccultationTimeCanvas->GetCanvas()->cd();
  m_OccultationTime->Draw();
  m_OccultationTimeCanvas->GetCanvas()->Update();

  m_NightTimeCanvas = new TRootEmbeddedCanvas("DepthBefore", this, 100, 100);
  this->AddFrame(m_NightTimeCanvas, CanvasLayout);
  m_NightTimeCanvas->GetCanvas()->cd();
  m_NightTime->Draw();
  m_NightTimeCanvas->GetCanvas()->Update();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsOrbitEngine::Update()
{
  //! Update the frame

  int Bins = int((m_Time.back()-m_Time.front()).GetAsSeconds())/10;

  m_OccultationTime->SetBins(Bins, m_Time.front().GetAsSeconds(), m_Time.back().GetAsSeconds());
  m_NightTime->SetBins(Bins, m_Time.front().GetAsSeconds(), m_Time.back().GetAsSeconds());
  for (int B = 1; B <= m_OccultationTime->GetXaxis()->GetNbins(); ++B) {
    m_OccultationTime->SetBinContent(B, 0);
  }
  for (int B = 1; B <= m_NightTime->GetXaxis()->GetNbins(); ++B) {
    m_NightTime->SetBinContent(B, 0);
  }
  
  list<NTime>::iterator T;
  list<bool>::iterator O;
  list<bool>::iterator N;
  for (T = m_Time.begin(), O = m_IsOcculted.begin(), N = m_IsNight.begin(); T != m_Time.end(); ++T, ++O, ++N) {
    m_OccultationTime->Fill((*T).GetAsSeconds(), ((*O) == true ? 0 : 1));
    m_NightTime->Fill((*T).GetAsSeconds(), ((*N) == true ? 0 : 1));
  }
  for (int B = 1; B <= m_OccultationTime->GetXaxis()->GetNbins(); ++B) {
    if (m_OccultationTime->GetBinContent(B) > 1) m_OccultationTime->SetBinContent(B, 1);
  }
  for (int B = 1; B <= m_NightTime->GetXaxis()->GetNbins(); ++B) {
    if (m_NightTime->GetBinContent(B) > 1) m_NightTime->SetBinContent(B, 1);
  }

  if (m_OccultationTimeCanvas != 0) {
    m_OccultationTimeCanvas->GetCanvas()->Modified();
    m_OccultationTimeCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
  if (m_NightTimeCanvas != 0) {
    m_NightTimeCanvas->GetCanvas()->Modified();
    m_NightTimeCanvas->GetCanvas()->Update();
    gSystem->ProcessEvents();
  }
}


// NGUIDiagnosticsOrbitEngine: the end...
////////////////////////////////////////////////////////////////////////////////
