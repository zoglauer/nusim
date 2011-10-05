/*
 * NGUIOptionsOrbitEngineTLE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsOrbitEngineTLE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsOrbitEngineTLE.h"

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
#include "NModuleOrbitEngineTLE.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsOrbitEngineTLE)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrbitEngineTLE::NGUIOptionsOrbitEngineTLE(NModule* Module)
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrbitEngineTLE::~NGUIOptionsOrbitEngineTLE()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsOrbitEngineTLE::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 50, 20, 5, 10);

  m_TLEFile = new MGUIEFileSelector(this, "Choose a TLE orbit propagation file:", 
                                       dynamic_cast<NModuleOrbitEngineTLE*>(m_Module)->GetTLEFileName());
  m_TLEFile->SetFileType("TLE file", "*.tle");
  AddFrame(m_TLEFile, FileLayout);


  m_Save = new TGCheckButton(this, "Save occultation data");
  m_Save->Associate(this);
  if (dynamic_cast<NModuleOrbitEngineTLE*>(m_Module)->GetSave() == true) {
    m_Save->SetState(kButtonDown);
  } else {
    m_Save->SetState(kButtonUp);
  }
  AddFrame(m_Save, FileLayout);
  
  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsOrbitEngineTLE::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsOrbitEngineTLE::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleOrbitEngineTLE*>(m_Module)->SetTLEFileName(m_TLEFile->GetFileName());

  if (m_Save->GetState() == kButtonDown) {
    dynamic_cast<NModuleOrbitEngineTLE*>(m_Module)->SetSave(true);
  } else {
    dynamic_cast<NModuleOrbitEngineTLE*>(m_Module)->SetSave(false);
  }
  
	return true;
}


// NGUIOptionsOrbitEngineTLE: the end...
////////////////////////////////////////////////////////////////////////////////
