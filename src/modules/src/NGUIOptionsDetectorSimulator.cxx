/*
 * NGUIOptionsDetectorSimulator.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsDetectorSimulator.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModuleDetectorSimulatorDetailed.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsDetectorSimulator)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorSimulator::NGUIOptionsDetectorSimulator(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorSimulator::~NGUIOptionsDetectorSimulator()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsDetectorSimulator::Create()
{
  PreCreate();

  // Modify here
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 40, 20, 10, 10);


  m_EnableBerylliumWindow = new TGCheckButton(this, "Enable Beryllium window (should always be on)");
  m_EnableBerylliumWindow->Associate(this);
  if (dynamic_cast<NModuleDetectorSimulatorDetailed*>(m_Module)->GetUseBerylliumWindow() == true) {
    m_EnableBerylliumWindow->SetState(kButtonDown);
  } else {
    m_EnableBerylliumWindow->SetState(kButtonUp);    
  }
  AddFrame(m_EnableBerylliumWindow, LabelLayout);
  

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsDetectorSimulator::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Modify here if you have more buttons

	bool Status = true;
	
  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_BUTTON:
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
  
  if (Status == false) {
    return false;
  }

  // Call also base class
  return NGUIOptions::ProcessMessage(Message, Parameter1, Parameter2);
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsDetectorSimulator::OnApply()
{
	// Modify this to store the data in the module!

  if (m_EnableBerylliumWindow->GetState() == kButtonDown) {
    dynamic_cast<NModuleDetectorSimulatorDetailed*>(m_Module)->SetUseBerylliumWindow(true);
  } else {
    dynamic_cast<NModuleDetectorSimulatorDetailed*>(m_Module)->SetUseBerylliumWindow(false);    
  }

  
	return true;
}


// NGUIOptionsDetectorSimulator: the end...
////////////////////////////////////////////////////////////////////////////////
