/*
 * NGUIDiagnostics.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnostics.h"

// Standard libs:
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

// ROOT libs:
#include <KeySymbols.h>
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"
#include "NModule.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnostics)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnostics::NGUIDiagnostics() 
  : TGCompositeFrame(gClient->GetRoot(), 320, 240)
{
  // standard constructor

  // use hierarchical cleaning
  SetCleanup(kDeepCleanup);

  m_TabTitle = "No title";
  m_NeedsUpdate = false;
}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnostics::~NGUIDiagnostics()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnostics::CloseWindow()
{
  // When the x is pressed, this function is called.

  DeleteWindow();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIDiagnostics::ProcessMessage(long Message, long Parameter1, long Parameter2)
{
  // Process the messages for this window

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
  
  return Status;
}


// NGUIDiagnostics: the end...
////////////////////////////////////////////////////////////////////////////////
