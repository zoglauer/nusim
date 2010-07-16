/*
 * NGUIDiagnosticsTemplate.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIDiagnosticsTemplate.h"

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
#include "NModuleTemplate.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIDiagnosticsTemplate)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsTemplate::NGUIDiagnosticsTemplate() : NGUIDiagnostics()
{
  // standard constructor

  // Set the new title of the tab here:
  m_TabTitle = "Title Template";

  // Add all histograms and canvases below



}


////////////////////////////////////////////////////////////////////////////////


NGUIDiagnosticsTemplate::~NGUIDiagnosticsTemplate()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsTemplate::Create()
{
  // Add the GUI options here

}


////////////////////////////////////////////////////////////////////////////////


void NGUIDiagnosticsTemplate::Update()
{
  //! Update the frame

}



// NGUIDiagnosticsTemplate: the end...
////////////////////////////////////////////////////////////////////////////////
