/*
 * NGUIOptionsTriggerEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsTriggerEngine.h"

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
#include "NModuleTriggerEngineSciSim.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsTriggerEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsTriggerEngine::NGUIOptionsTriggerEngine(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsTriggerEngine::~NGUIOptionsTriggerEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsTriggerEngine::Create()
{
  PreCreate();

  // Modify here
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 40, 20, 10, 10);


  m_EnableDeadTime = new TGCheckButton(this, "Enable dead time");
  m_EnableDeadTime->Associate(this);
  if (dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->GetApplyDeadTime() == true) {
    m_EnableDeadTime->SetState(kButtonDown);
  } else {
    m_EnableDeadTime->SetState(kButtonUp);    
  }
  AddFrame(m_EnableDeadTime, LabelLayout);

  m_PixelTrigger = new MGUIEEntry(this, "Pixel trigger (standard: 3 keV) [keV]:", false, dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->GetPixelTrigger());
  AddFrame(m_PixelTrigger, LabelLayout);

  m_LowTrigger = new MGUIEEntry(this, "Shield lower trigger [keV]:", false, dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->GetLowTrigger());
  AddFrame(m_LowTrigger, LabelLayout);

  m_HighTrigger = new MGUIEEntry(this, "Shield higher trigger [keV]:", false, dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->GetHighTrigger());
  AddFrame(m_HighTrigger, LabelLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsTriggerEngine::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsTriggerEngine::OnApply()
{
	// Modify this to store the data in the module!

  if (m_EnableDeadTime->GetState() == kButtonDown) {
    dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->SetApplyDeadTime(true);
  } else {
    dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->SetApplyDeadTime(false);    
  }

  dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->SetPixelTrigger(m_PixelTrigger->GetAsDouble());
  dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->SetLowTrigger(m_LowTrigger->GetAsDouble());
  dynamic_cast<NModuleTriggerEngineSciSim*>(m_Module)->SetHighTrigger(m_HighTrigger->GetAsDouble());

  return true;
}


// NGUIOptionsTriggerEngine: the end...
////////////////////////////////////////////////////////////////////////////////
