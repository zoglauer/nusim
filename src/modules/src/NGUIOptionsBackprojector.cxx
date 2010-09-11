/*
 * NGUIOptionsBackprojector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsBackprojector.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TFormula.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModuleBackprojector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsBackprojector)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsBackprojector::NGUIOptionsBackprojector(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsBackprojector::~NGUIOptionsBackprojector()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsBackprojector::Create()
{
  PreCreate();

  TGLayoutHints* MinMaxLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 10, 0);
  
  m_PixelSize = new MGUIEEntry(this, "Pixel size [arcsec]", false,
                               dynamic_cast<NModuleBackprojector*>(m_Module)->GetPixelSize()*60,
                               true, 3.0, 48.0);
  AddFrame(m_PixelSize, MinMaxLayout);
  
  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsBackprojector::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsBackprojector::OnApply()
{
	// Modify this to store the data in the module!
    
  dynamic_cast<NModuleBackprojector*>(m_Module)->SetPixelSize(m_PixelSize->GetAsDouble()/60);
  
	return true;
}


// NGUIOptionsBackprojector: the end...
////////////////////////////////////////////////////////////////////////////////
