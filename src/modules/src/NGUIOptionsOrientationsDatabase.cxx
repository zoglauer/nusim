/*
 * NGUIOptionsOrientationsDatabase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsOrientationsDatabase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsOrientationsDatabase.h"

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
#include "NModuleOrientationsDatabase.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsOrientationsDatabase)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrientationsDatabase::NGUIOptionsOrientationsDatabase(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrientationsDatabase::~NGUIOptionsOrientationsDatabase()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsOrientationsDatabase::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_OrientationsDB = new MGUIEFileSelector(this, "Choose an orientations database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetFileName());
  m_OrientationsDB->SetFileType("Orientations data base", "*.csv");

	AddFrame(m_OrientationsDB, FileLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsOrientationsDatabase::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsOrientationsDatabase::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetFileName(m_OrientationsDB->GetFileName());

	return true;
}


// NGUIOptionsOrientationsDatabase: the end...
////////////////////////////////////////////////////////////////////////////////
