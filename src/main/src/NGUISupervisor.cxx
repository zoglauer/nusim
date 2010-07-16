/*
 * NGUISupervisor.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUISupervisor.h"

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
#include "NSupervisor.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUISupervisor)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUISupervisor::NGUISupervisor(NSupervisor* Supervisor) 
   : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 320, 240), m_Supervisor(Supervisor)
{
  // standard constructor
  SetCleanup(kDeepCleanup);

  Create();
}


////////////////////////////////////////////////////////////////////////////////


NGUISupervisor::~NGUISupervisor()
{
  // kDeepCleanup is activated
}


////////////////////////////////////////////////////////////////////////////////


void NGUISupervisor::Create()
{
  
  // We start with a name and an icon...
  SetWindowName("Supervisor options");  

  // Main label
  const TGFont* lFont = gClient->GetFont("-*-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  if (!lFont) lFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t LargeFont = lFont->GetFontStruct();

  TGLabel* MainLabel = new TGLabel(this, "Supervisor options:");
  MainLabel->SetTextFont(LargeFont);
  TGLayoutHints* MainLabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 10, 10, 10, 10);
	AddFrame(MainLabel, MainLabelLayout);



  TGLayoutHints* EntryLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 20, 20, 2, 2);

  m_ObservationTime = new MGUIEEntry(this, "Observation time [s]:", false, m_Supervisor->GetObservationTime().GetSeconds());
  AddFrame(m_ObservationTime, EntryLayout);

  m_UpdateInterval = new MGUIEEntry(this, "Update interval diagnostics window [events]:", false, m_Supervisor->GetUpdateInterval());
  AddFrame(m_UpdateInterval, EntryLayout);



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


void NGUISupervisor::CloseWindow()
{
  // When the x is pressed, this function is called.

  DeleteWindow();
}



////////////////////////////////////////////////////////////////////////////////


bool NGUISupervisor::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUISupervisor::OnOk()
{
	// The OK button has been pressed

	if (OnApply() == true) {
    CloseWindow();
		return true;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUISupervisor::OnCancel()
{
	// The Cancel button has been pressed

  CloseWindow();

	return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUISupervisor::OnApply()
{
	// The Apply button has been pressed

  m_Supervisor->SetObservationTime(NTime(m_ObservationTime->GetAsDouble()));
  m_Supervisor->SetUpdateInterval(m_UpdateInterval->GetAsInt());

	return true;
}


// NGUISupervisor: the end...
////////////////////////////////////////////////////////////////////////////////
