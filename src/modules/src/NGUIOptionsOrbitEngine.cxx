/*
 * NGUIOptionsOrbitEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsOrbitEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsOrbitEngine.h"

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
#include "NModuleOrbitEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsOrbitEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrbitEngine::NGUIOptionsOrbitEngine(NModule* Module)
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrbitEngine::~NGUIOptionsOrbitEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsOrbitEngine::Create()
{
  PreCreate();

  TGLayoutHints* DefaultLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 5);
  
  m_OrbitDuration = new MGUIEEntry(this, "Duration of one orbit (minutes):", false, dynamic_cast<NModuleOrbitEngine*>(m_Module)->GetOrbitDuration().GetAsSeconds()/minute);
	AddFrame(m_OrbitDuration, DefaultLayout);

  m_BlackoutDuration = new MGUIEEntry(this, "Duration of the blackout due to occultation (minutes):", false, dynamic_cast<NModuleOrbitEngine*>(m_Module)->GetBlackoutDuration().GetAsSeconds()/minute);
	AddFrame(m_BlackoutDuration, DefaultLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsOrbitEngine::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsOrbitEngine::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleOrbitEngine*>(m_Module)->SetOrbitDuration(m_OrbitDuration->GetAsDouble()*minute);
  dynamic_cast<NModuleOrbitEngine*>(m_Module)->SetBlackoutDuration(m_BlackoutDuration->GetAsDouble()*minute);

	return true;
}


// NGUIOptionsOrbitEngine: the end...
////////////////////////////////////////////////////////////////////////////////
