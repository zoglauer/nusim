/*
 * NGUIOptionsGeometryAndDetectorProperties.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsGeometryAndDetectorProperties.h"

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
#include "NModuleGeometryAndDetectorProperties.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsGeometryAndDetectorProperties)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsGeometryAndDetectorProperties::NGUIOptionsGeometryAndDetectorProperties(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsGeometryAndDetectorProperties::~NGUIOptionsGeometryAndDetectorProperties()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsGeometryAndDetectorProperties::Create()
{
  PreCreate();

  // Modify here
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 40, 20, 10, 10);


  m_EnableDetectorGaps = new TGCheckButton(this, "Use a detector with gaps between wafers (should always be on!)");
  m_EnableDetectorGaps->Associate(this);
  if (dynamic_cast<NModuleGeometryAndDetectorProperties*>(m_Module)->GetUseDetectorGaps() == true) {
    m_EnableDetectorGaps->SetState(kButtonDown);
  } else {
    m_EnableDetectorGaps->SetState(kButtonUp);    
  }
  AddFrame(m_EnableDetectorGaps, LabelLayout);
  

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsGeometryAndDetectorProperties::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsGeometryAndDetectorProperties::OnApply()
{
	// Modify this to store the data in the module!

  if (m_EnableDetectorGaps->GetState() == kButtonDown) {
    dynamic_cast<NModuleGeometryAndDetectorProperties*>(m_Module)->SetUseDetectorGaps(true);
  } else {
    dynamic_cast<NModuleGeometryAndDetectorProperties*>(m_Module)->SetUseDetectorGaps(false);    
  }

  
	return true;
}


// NGUIOptionsGeometryAndDetectorProperties: the end...
////////////////////////////////////////////////////////////////////////////////
