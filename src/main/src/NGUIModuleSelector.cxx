/*
 * NGUIModuleSelector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIModuleSelector.h"

// Standard libs:
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

// ROOT libs:
#include <KeySymbols.h>
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NModule.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIModuleSelector)
#endif


////////////////////////////////////////////////////////////////////////////////


  NGUIModuleSelector::NGUIModuleSelector(vector<NModule*> Modules, NModule* PreSelected) 
    : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 320, 240), m_Modules(Modules), 
      m_PreSelected(PreSelected), m_OKPressed(false), m_SelectedModule(0), m_List(0)
{
  // standard constructor
  SetCleanup(kDeepCleanup);

  Create();
}


////////////////////////////////////////////////////////////////////////////////


NGUIModuleSelector::~NGUIModuleSelector()
{
  // kDeepCleanup is activated
}


////////////////////////////////////////////////////////////////////////////////


void NGUIModuleSelector::Create()
{
  
  // We start with a name and an icon...
  SetWindowName("Module selector");  

  // Main label
  const TGFont* lFont = gClient->GetFont("-*-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  if (!lFont) lFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t LargeFont = lFont->GetFontStruct();

  TGLabel* MainLabel = new TGLabel(this, "Choose a module:");
  MainLabel->SetTextFont(LargeFont);
  TGLayoutHints* MainLabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 10, 10, 10, 10);
	AddFrame(MainLabel, MainLabelLayout);


  //
  if (m_Modules.size() > 0) {
    m_List = new MGUIERBList(this, "The following modules are available: ");
    m_List->Add("No module");
    if (m_PreSelected == 0) {
      m_List->SetSelected(0);
    }
    for (unsigned int m = 0; m < m_Modules.size(); ++m) {
      m_List->Add(m_Modules[m]->GetFullName());
      if (m_PreSelected == m_Modules[m]) {
        m_List->SetSelected(m+1);
      }
    }
    m_List->Create();
    TGLayoutHints* ListLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 20, 20);
    AddFrame(m_List, ListLayout);
  } else {
    TGLabel* NoModulesLabel = new TGLabel(this, "Unfortunately there are no modules available for this spot in the pipeline...");
    TGLayoutHints* NoModulesLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 20, 20);
    AddFrame(NoModulesLabel, NoModulesLayout);
  }


  // OK and cancel buttons
	// Frame around the buttons:
	TGHorizontalFrame* ButtonFrame = new TGHorizontalFrame(this, 150, 25);
	TGLayoutHints* ButtonFrameLayout =	new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsCenterX, 10, 10, 10, 10);
	AddFrame(ButtonFrame, ButtonFrameLayout);
	
  // The buttons itself
	TGTextButton*	OKButton = new TGTextButton(ButtonFrame, "OK", e_Ok); 
  OKButton->Associate(this);
  TGLayoutHints* OKButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsRight | kLHintsExpandX, 20, 0, 0, 0);
	ButtonFrame->AddFrame(OKButton, OKButtonLayout);
	
	TGTextButton* CancelButton = new TGTextButton(ButtonFrame, "     Cancel     ", e_Cancel); 
  CancelButton->Associate(this);
  TGLayoutHints* CancelButtonLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);
  ButtonFrame->AddFrame(CancelButton, CancelButtonLayout);

  // Give this element the default size of its content:
  Resize(GetDefaultWidth(), GetDefaultHeight()); 

  MapSubwindows();
  MapWindow();  
  Layout();

  return;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIModuleSelector::CloseWindow()
{
  // When the x is pressed, this function is called.

  DeleteWindow();
}



////////////////////////////////////////////////////////////////////////////////


bool NGUIModuleSelector::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Process the messages for this window

	bool Status = true;
	
  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      switch (Parameter1) {
      case e_Ok:
				Status = OnOk();
        break;
        
      case e_Cancel:
        Status = OnCancel();
        break;

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
    break;
  default:
    break;
  }
  
  return Status;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIModuleSelector::OnOk()
{
	// The OK button has been pressed

	if (OnApply() == true) {
    m_OKPressed = true;
		UnmapWindow();
		return true;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIModuleSelector::OnCancel()
{
	// The Cancel button has been pressed

	UnmapWindow();

	return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIModuleSelector::OnApply()
{
	// The Apply button has been pressed

  if (m_List != 0 && m_List->GetNEntries() > 0) {
    if (m_List->GetSelected() < 0) {
      mgui<<"Please select a module or press cancel"<<show;
      return false;
    }
    
    if (m_List->GetSelected() == 0) {
      m_SelectedModule = 0;
    } else {
      m_SelectedModule = m_Modules[m_List->GetSelected()-1]; // First is "No module"
    }
  }

	return true;
}


// NGUIModuleSelector: the end...
////////////////////////////////////////////////////////////////////////////////
