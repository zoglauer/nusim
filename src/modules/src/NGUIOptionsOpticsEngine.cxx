/*
 * NGUIOptionsOpticsEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsOpticsEngine.h"

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
#include "NModuleOpticsEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsOpticsEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOpticsEngine::NGUIOptionsOpticsEngine(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOpticsEngine::~NGUIOptionsOpticsEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsOpticsEngine::Create()
{
  PreCreate();

  // Modify here
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 40, 20, 10, 10);


  m_EnableScattering = new TGCheckButton(this, "Enable scattering");
  m_EnableScattering->Associate(this);
  if (dynamic_cast<NModuleOpticsEngine*>(m_Module)->GetUseScattering() == true) {
    m_EnableScattering->SetState(kButtonDown);
  } else {
    m_EnableScattering->SetState(kButtonUp);    
  }
  AddFrame(m_EnableScattering, LabelLayout);

  m_EnableGhostRays = new TGCheckButton(this, "Enable ghost rays");
  m_EnableGhostRays->Associate(this);
  if (dynamic_cast<NModuleOpticsEngine*>(m_Module)->GetUseGhostRays() == true) {
    m_EnableGhostRays->SetState(kButtonDown);
  } else {
    m_EnableGhostRays->SetState(kButtonUp);    
  }
  AddFrame(m_EnableGhostRays, LabelLayout);

  m_EnableIdealOptics = new TGCheckButton(this, "Enable ideal optics");
  m_EnableIdealOptics->Associate(this);
  if (dynamic_cast<NModuleOpticsEngine*>(m_Module)->GetUseIdealOptics() == true) {
    m_EnableIdealOptics->SetState(kButtonDown);
  } else {
    m_EnableIdealOptics->SetState(kButtonUp);    
  }
  AddFrame(m_EnableIdealOptics, LabelLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsOpticsEngine::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsOpticsEngine::OnApply()
{
	// Modify this to store the data in the module!

  if (m_EnableScattering->GetState() == kButtonDown) {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseScattering(true);
  } else {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseScattering(false);    
  }

  if (m_EnableGhostRays->GetState() == kButtonDown) {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseGhostRays(true);
  } else {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseGhostRays(false);    
  }

  if (m_EnableIdealOptics->GetState() == kButtonDown) {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseIdealOptics(true);
  } else {
    dynamic_cast<NModuleOpticsEngine*>(m_Module)->SetUseIdealOptics(false);    
  }
  
	return true;
}


// NGUIOptionsOpticsEngine: the end...
////////////////////////////////////////////////////////////////////////////////
