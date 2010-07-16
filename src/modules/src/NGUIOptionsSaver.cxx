/*
 * NGUIOptionsSaver.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsSaver
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsSaver.h"

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
#include "NModuleInterfaceIO.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsSaver)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSaver::NGUIOptionsSaver(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSaver::~NGUIOptionsSaver()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSaver::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_FileName = new MGUIEFileSelector(this, "Choose a file name to save the events to:", 
                                     dynamic_cast<NModuleInterfaceIO*>(m_Module)->GetFileName());
  m_FileName->SetFileType("Orientations data base", "*.dat");

	AddFrame(m_FileName, FileLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSaver::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsSaver::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleInterfaceIO*>(m_Module)->SetFileName(m_FileName->GetFileName());

	return true;
}


// NGUIOptionsSaver: the end...
////////////////////////////////////////////////////////////////////////////////
