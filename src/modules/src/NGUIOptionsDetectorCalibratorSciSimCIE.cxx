/*
 * NGUIOptionsDetectorCalibratorSciSimCIE.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsDetectorCalibratorSciSimCIE.h"

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
#include "NModuleDetectorCalibratorSciSimCIE.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsDetectorCalibratorSciSimCIE)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorCalibratorSciSimCIE::NGUIOptionsDetectorCalibratorSciSimCIE(NModule* Module)
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsDetectorCalibratorSciSimCIE::~NGUIOptionsDetectorCalibratorSciSimCIE()
{
  // kDeepCleanup is activated
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsDetectorCalibratorSciSimCIE::Create()
{
  PreCreate();

  // Modify here
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 40, 20, 10, 10);

  m_GainSingleTrigger = new MGUIEEntry(this, "Gain for single-trigger events:", false,
				       dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetGainSingleTrigger());
  AddFrame(m_GainSingleTrigger, LabelLayout);

  m_OffsetSingleTrigger = new MGUIEEntry(this, "Offset for single-trigger events:", false,
					 dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetOffsetSingleTrigger());
  AddFrame(m_OffsetSingleTrigger, LabelLayout);

  m_GainDoubleTrigger = new MGUIEEntry(this, "Gain for double-trigger events:", false,
				       dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetGainDoubleTrigger());
  AddFrame(m_GainDoubleTrigger, LabelLayout);

  m_OffsetDoubleTrigger = new MGUIEEntry(this, "Offset for double-trigger events:", false,
					 dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetOffsetDoubleTrigger());
  AddFrame(m_OffsetDoubleTrigger, LabelLayout);

  m_GainTripleTrigger = new MGUIEEntry(this, "Gain for triple-trigger events:", false,
				       dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetGainTripleTrigger());
  AddFrame(m_GainTripleTrigger, LabelLayout);

  m_OffsetTripleTrigger = new MGUIEEntry(this, "Offset for triple-trigger events:", false,
					 dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetOffsetTripleTrigger());
  AddFrame(m_OffsetTripleTrigger, LabelLayout);

  m_GainQuadrupleTrigger = new MGUIEEntry(this, "Gain for quadruple-trigger events:", false,
				       dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetGainQuadrupleTrigger());
  AddFrame(m_GainQuadrupleTrigger, LabelLayout);

  m_OffsetQuadrupleTrigger = new MGUIEEntry(this, "Offset for quadruple-trigger events:", false,
					 dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->GetOffsetQuadrupleTrigger());
  AddFrame(m_OffsetQuadrupleTrigger, LabelLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsDetectorCalibratorSciSimCIE::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsDetectorCalibratorSciSimCIE::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetGainSingleTrigger(m_GainSingleTrigger->GetAsDouble());
  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetOffsetSingleTrigger(m_OffsetSingleTrigger->GetAsDouble());

  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetGainDoubleTrigger(m_GainDoubleTrigger->GetAsDouble());
  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetOffsetDoubleTrigger(m_OffsetDoubleTrigger->GetAsDouble());

  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetGainTripleTrigger(m_GainTripleTrigger->GetAsDouble());
  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetOffsetTripleTrigger(m_OffsetTripleTrigger->GetAsDouble());

  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetGainQuadrupleTrigger(m_GainQuadrupleTrigger->GetAsDouble());
  dynamic_cast<NModuleDetectorCalibratorSciSimCIE*>(m_Module)->SetOffsetQuadrupleTrigger(m_OffsetQuadrupleTrigger->GetAsDouble());

  return true;
}


// NGUIOptionsDetectorCalibratorSciSimCIE: the end...
////////////////////////////////////////////////////////////////////////////////
