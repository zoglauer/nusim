/*
 * NGUIOptionsMetrologyEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsMetrologyEngine.h"

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
#include "NModuleMetrologyEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsMetrologyEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsMetrologyEngine::NGUIOptionsMetrologyEngine(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsMetrologyEngine::~NGUIOptionsMetrologyEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsMetrologyEngine::Create()
{
  PreCreate();

  // Modify here
  m_UpdateInterval = new MGUIEEntry(this, "Update interval of the metrology data [s]:", false, dynamic_cast<NModuleMetrologyEngine*>(m_Module)->GetUpdateInterval());
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 10, 10);
	AddFrame(m_UpdateInterval, LabelLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsMetrologyEngine::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsMetrologyEngine::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleMetrologyEngine*>(m_Module)->SetUpdateInterval(m_UpdateInterval->GetAsDouble());

	return true;
}


// NGUIOptionsMetrologyEngine: the end...
////////////////////////////////////////////////////////////////////////////////
