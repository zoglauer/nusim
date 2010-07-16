/*
 * NGUIOptionsDetectorEffectsEnginePHE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsDetectorEffectsEnginePHE
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsDetectorEffectsEnginePHE.h"

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
#include "NModuleDetectorEffectsEnginePHE.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsDetectorEffectsEnginePHE)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorEffectsEnginePHE::NGUIOptionsDetectorEffectsEnginePHE(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorEffectsEnginePHE::~NGUIOptionsDetectorEffectsEnginePHE()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsDetectorEffectsEnginePHE::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_PHEFile = new MGUIEFileSelector(this, "Choose a PHE fits file:", 
                                           dynamic_cast<NModuleDetectorEffectsEnginePHE*>(m_Module)->GetPHEFileName());
  m_PHEFile->SetFileType("PHE fits file", "*.fits");

	AddFrame(m_PHEFile, FileLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsDetectorEffectsEnginePHE::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsDetectorEffectsEnginePHE::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleDetectorEffectsEnginePHE*>(m_Module)->SetPHEFileName(m_PHEFile->GetFileName());

	return true;
}


// NGUIOptionsDetectorEffectsEnginePHE: the end...
////////////////////////////////////////////////////////////////////////////////
