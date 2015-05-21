/*
 * NGUIOptions.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptions.h"

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
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptions)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptions::NGUIOptions(NModule* Module) 
  : TGTransientFrame(gClient->GetRoot(), gClient->GetRoot(), 320, 240), m_Module(Module)
{
  // standard constructor

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptions::~NGUIOptions()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptions::PreCreate()
{
  // Create the GUI options

  // We start with a name and an icon...
  SetWindowName("Module options");  

  // Main label
  const TGFont* lFont = gClient->GetFont("-*-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1");
  if (!lFont) lFont = gClient->GetResourcePool()->GetDefaultFont();
  FontStruct_t LargeFont = lFont->GetFontStruct();

  TGLabel* MainLabel = new TGLabel(this, TString("Options for module \"") + m_Module->GetFullName() + TString("\":"));
  MainLabel->SetTextFont(LargeFont);
  TGLayoutHints* MainLabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 10, 25, 10, 10);
	AddFrame(MainLabel, MainLabelLayout);
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptions::Create()
{
  //! The creation part which gets overwritten

  PreCreate();

  // Add your code between the "--"
  // -------------------

  TGLayoutHints* Layout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 0, 0, 10, 10);
  TGLabel* Label = new TGLabel(this, "This module has currently no GUI options.");
  AddFrame(Label, Layout);

  // -------------------

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptions::PostCreate()
{
  // OK and cancel buttons
	// Frame around the buttons:
	TGHorizontalFrame* ButtonFrame = new TGHorizontalFrame(this, 150, 25);
	TGLayoutHints* ButtonFrameLayout =	new TGLayoutHints(kLHintsBottom | kLHintsExpandX | kLHintsCenterX, 10, 10, 20, 10);
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


void NGUIOptions::CloseWindow()
{
  // When the x is pressed, this function is called.

  UnmapWindow();
  DeleteWindow();
}



////////////////////////////////////////////////////////////////////////////////


bool NGUIOptions::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptions::OnOk()
{
	// The Ok button has been pressed

	if (OnApply() == true) {
		CloseWindow();
		return true;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptions::OnCancel()
{
	// The Cancel button has been pressed

	CloseWindow();

	return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptions::OnApply()
{
	// The Apply button has been pressed

	return true;
}


// NGUIOptions: the end...
////////////////////////////////////////////////////////////////////////////////
