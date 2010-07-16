/*
 * NGUIOptionsLoader.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsLoader
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsLoader.h"

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
ClassImp(NGUIOptionsLoader)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsLoader::NGUIOptionsLoader(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsLoader::~NGUIOptionsLoader()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsLoader::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_FileName = new MGUIEFileSelector(this, "Choose a file name to load the events from:", 
                                           dynamic_cast<NModuleInterfaceIO*>(m_Module)->GetFileName());
  m_FileName->SetFileType("Orientations data base", "*.dat");

	AddFrame(m_FileName, FileLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsLoader::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsLoader::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleInterfaceIO*>(m_Module)->SetFileName(m_FileName->GetFileName());

	return true;
}


// NGUIOptionsLoader: the end...
////////////////////////////////////////////////////////////////////////////////
