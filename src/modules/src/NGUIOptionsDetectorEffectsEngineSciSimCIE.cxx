/*
 * NGUIOptionsDetectorEffectsEngineSciSimCIE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsDetectorEffectsEngineSciSimCIE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsDetectorEffectsEngineSciSimCIE.h"

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
#include "NModuleDetectorEffectsEngineSciSimCIE.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsDetectorEffectsEngineSciSimCIE)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorEffectsEngineSciSimCIE::NGUIOptionsDetectorEffectsEngineSciSimCIE(NModule* Module)
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorEffectsEngineSciSimCIE::~NGUIOptionsDetectorEffectsEngineSciSimCIE()
{
  // kDeepCleanup is activated
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsDetectorEffectsEngineSciSimCIE::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_CIEFile = new MGUIEFileSelector(this, "Choose a CIE ROOT file:",
				    dynamic_cast<NModuleDetectorEffectsEngineSciSimCIE*>(m_Module)->GetCIEFileName());
  m_CIEFile->SetFileType("CIE ROOT file", "*.root");

  AddFrame(m_CIEFile, FileLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsDetectorEffectsEngineSciSimCIE::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsDetectorEffectsEngineSciSimCIE::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleDetectorEffectsEngineSciSimCIE*>(m_Module)->SetCIEFileName(m_CIEFile->GetFileName());

	return true;
}


// NGUIOptionsDetectorEffectsEngineSciSimCIE: the end...
////////////////////////////////////////////////////////////////////////////////
