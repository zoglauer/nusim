/*
 * NGUIDiagnosticsMain.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsMain.h"

// Standard libs:

// ROOT libs:
#include <KeySymbols.h>
#include <TApplication.h>
#include <TGPicture.h>
#include <TStyle.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TString.h>
#include <TGClient.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MFile.h"
#include "MGUIEFileSelector.h"

// NuSTAR libs:
#include "NModule.h"
#include "NGUIModuleSelector.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsMain)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsMain::NGUIDiagnosticsMain() : MGUIDialog(gClient->GetRoot(), gClient->GetRoot(), 400, 700)
{
  gStyle->SetPalette(1, 0);

  // No deep clean-up allowed in this function!
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsMain::~NGUIDiagnosticsMain()
{
  // Deep Cleanup automatically deletes all used GUI elements

  // ROOT insists to delete the subframes, but cannot do this since they are the
  // property of the module...
  // Thus for the time being we have a tiny memory leak...
  //delete m_MainTab;
  //delete m_MainTabLayout;

  // The diagnostics tabs are only borrowed from the modules!
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsMain::Create()
{
  // Create the main window

  // We start with a name and an icon...
  SetWindowName("Diagnostics");  

   
  // The subtitle
  AddSubTitle(TString("Press the tab-bar to switch between diagnostics modules"));

  m_MainTab = new TGTab(this, 400, 700);
  m_MainTabLayout = 
    new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 10, 10, 0, 0);
  AddFrame(m_MainTab, m_MainTabLayout);

  for (unsigned int d = 0; d < m_DiagnosticTabs.size(); ++d) {
    m_DiagnosticTabs[d]->ReparentWindow(m_MainTab);
    m_DiagnosticTabs[d]->Create();
    m_MainTab->AddTab(m_DiagnosticTabs[d]->GetTabTitle(), m_DiagnosticTabs[d]);
  }

  // The buttons
  AddButtons(c_Ok | c_Apply, true);

  m_ApplyButton->SetText("Update");

  // Give this element the default size of its content:
  Resize(400, 700); 

  MapSubwindows();
  MapWindow();  
  Layout();

  return;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIDiagnosticsMain::ProcessMessage(long Message, long Parameter1, 
                                         long Parameter2)
{
  // Process the messages for this application

  bool Status = true;

  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      switch (Parameter1) {
      case e_Ok:
        Status = OnCancel();
      case e_Apply:
        Status = OnApply();
        break;
      default:
        break;
      }

      break;
    default:
      break;
    }
  default:
    break;
  }
  
  return Status;
}
 

////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsMain::Update()
{
  //! Update all tabs

  for (unsigned int d = 0; d < m_DiagnosticTabs.size(); ++d) {
    m_DiagnosticTabs[d]->Update();
  }
}



////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsMain::CloseWindow()
{
  // When the x is pressed, this function is called.
  // We do not delete it!

  UnmapWindow();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIDiagnosticsMain::OnApply()
{
	// The Apply button has been pressed

	Update();

	return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIDiagnosticsMain::OnCancel()
{
	// The Apply button has been pressed

	CloseWindow();

	return true;
}


// NGUIDiagnosticsMain: the end...
////////////////////////////////////////////////////////////////////////////////
