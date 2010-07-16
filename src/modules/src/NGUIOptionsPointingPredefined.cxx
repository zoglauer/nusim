/*
 * NGUIOptionsPointingPredefined.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsPointingPredefined.h"

// Standard libs:

// ROOT libs:
#include <TSystem.h>
#include <TString.h>
#include <TGLabel.h>
#include <TFormula.h>
#include <TGResourcePool.h>

// MEGAlib libs:
#include "MStreams.h"
#include "MGUIEEntryList.h"

// NuSTAR libs:
#include "NModule.h"
#include "NModulePointingPredefined.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsPointingPredefined)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPredefined::NGUIOptionsPointingPredefined(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPredefined::~NGUIOptionsPointingPredefined()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsPointingPredefined::Create()
{
  PreCreate();

  TGLayoutHints* StartLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 10, 0);
  TGLayoutHints* StandardLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 40, 20, 5, 0);
  TGLayoutHints* ComboFrameLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY | kLHintsExpandX, 20, 19, 10, 0);
  TGLayoutHints* ComboLabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY, 0, 40, 0, 0);
  TGLayoutHints* ComboLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX, 0, 0, 0, 0);

  TGLabel* PointingLabel = new TGLabel(this, "Original pointing:");
  AddFrame(PointingLabel, StartLayout);


  NPointing P = dynamic_cast<NModulePointingPredefined*>(m_Module)->GetPointingCenter();

  m_Ra = new MGUIEEntry(this, "Right ascension: ", false, P.GetRa());
  AddFrame(m_Ra, StandardLayout);
  m_Dec = new MGUIEEntry(this, "Declination: ", false, P.GetDec());
  AddFrame(m_Dec, StandardLayout);

  TGCompositeFrame* MotionPatternFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame);
  AddFrame(MotionPatternFrame, ComboFrameLayout);

  TGLabel* MotionPatternLabel = new TGLabel(MotionPatternFrame, "Motion pattern:");
  MotionPatternLabel->SetWidth(120);
  MotionPatternFrame->AddFrame(MotionPatternLabel, ComboLabelLayout);

  m_MotionPatterns = new TGComboBox(MotionPatternFrame);
  for (int b = NModulePointingPredefined::c_MotionPatternMin; 
       b <= NModulePointingPredefined::c_MotionPatternMax; ++b) {
    m_MotionPatterns->AddEntry(NModulePointingPredefined::GetMotionPatternName(b), b);
  }
  m_MotionPatterns->Associate(this);
  m_MotionPatterns->SetHeight(20);
  m_MotionPatterns->Select(dynamic_cast<NModulePointingPredefined*>(m_Module)->GetMotionPattern());
  MotionPatternFrame->AddFrame(m_MotionPatterns, ComboLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsPointingPredefined::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsPointingPredefined::OnApply()
{
	// Modify this to store the data in the module!
  
  NPointing P = dynamic_cast<NModulePointingPredefined*>(m_Module)->GetPointingCenter();
  P.SetRaDec(m_Ra->GetAsDouble(), m_Dec->GetAsDouble());
  dynamic_cast<NModulePointingPredefined*>(m_Module)->SetPointingCenter(P);
  dynamic_cast<NModulePointingPredefined*>(m_Module)->SetMotionPattern(m_MotionPatterns->GetSelected());

	return true;
}


// NGUIOptionsPointingPredefined: the end...
////////////////////////////////////////////////////////////////////////////////
