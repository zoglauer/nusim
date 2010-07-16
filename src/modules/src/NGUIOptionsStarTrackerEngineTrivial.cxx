/*
 * NGUIOptionsStarTrackerEngineTrivial.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsStarTrackerEngineTrivial.h"

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
#include "NModuleStarTrackerEngineTrivial.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsStarTrackerEngineTrivial)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsStarTrackerEngineTrivial::NGUIOptionsStarTrackerEngineTrivial(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsStarTrackerEngineTrivial::~NGUIOptionsStarTrackerEngineTrivial()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsStarTrackerEngineTrivial::Create()
{
  PreCreate();

  // Modify here
  m_UpdateInterval = new MGUIEEntry(this, "Update interval of the star tracker data [s]:", false, dynamic_cast<NModuleStarTrackerEngineTrivial*>(m_Module)->GetUpdateInterval());
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 10, 10);
	AddFrame(m_UpdateInterval, LabelLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsStarTrackerEngineTrivial::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsStarTrackerEngineTrivial::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleStarTrackerEngineTrivial*>(m_Module)->SetUpdateInterval(m_UpdateInterval->GetAsDouble());

	return true;
}


// NGUIOptionsStarTrackerEngineTrivial: the end...
////////////////////////////////////////////////////////////////////////////////
