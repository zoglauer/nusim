/*
 * NGUIOptionsEventSelector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsEventSelector.h"

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
#include "NModuleEventSelector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsEventSelector)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsEventSelector::NGUIOptionsEventSelector(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsEventSelector::~NGUIOptionsEventSelector()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsEventSelector::Create()
{
  PreCreate();

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 20, 20, 20, 5);

  m_FileOptions = new MGUIECBList(this, "Choose how to save the output file (you can choose none or multiple):", true);
  m_FileOptions->Add("Events in FITS format (*.events.fits)", dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsFits());
  m_FileOptions->Add("Events in ASCII format (*.events.dat)", dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsDat());
  m_FileOptions->Add("Events in ROOT format (*.events.root)", dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsROOT());
  m_FileOptions->Add("Spectral response in ROOT format (*.energyresponse.root)", dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEnergyResponseAsROOT());
  m_FileOptions->Create();
  m_FileOptions->Associate(this);
  AddFrame(m_FileOptions, FileLayout);

  TGLayoutHints* SaveBeforeSelectionsLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 15);
  m_SaveBeforeSelections = new TGCheckButton(this, "Save events before selections (otherwise after)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveBeforeSelections() == true) {
    m_SaveBeforeSelections->SetState(kButtonDown);
  } else {
    m_SaveBeforeSelections->SetState(kButtonUp);    
  }
  AddFrame(m_SaveBeforeSelections, SaveBeforeSelectionsLayout);

  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 15, 10);
  TGLabel* SelectionsLabel = new TGLabel(this, "Event selections:");
  AddFrame(SelectionsLabel, LabelLayout);
  
  
  TGLayoutHints* MinMaxLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 10, 20);
  
  m_Energies = new MGUIEMinMaxEntry(this, "Energy window [keV]", false,
                                    dynamic_cast<NModuleEventSelector*>(m_Module)->GetEnergyMin(),
                                    dynamic_cast<NModuleEventSelector*>(m_Module)->GetEnergyMax()); // , true, 3.0, 80.0);
  AddFrame(m_Energies, MinMaxLayout);

  m_DepthMax = new MGUIEEntry(this, "Maximum depth measured from top of detector [mm]", false,
                              dynamic_cast<NModuleEventSelector*>(m_Module)->GetDepthMax(), true, 0.0);
  AddFrame(m_DepthMax, MinMaxLayout);
  
  m_SelectByBadDepthCal = new TGCheckButton(this, "Enable event selection by bad depth calibration flag");
  m_SelectByBadDepthCal->Associate(this);
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSelectByBadDepthCal() == true) {
    m_SelectByBadDepthCal->SetState(kButtonDown);
  } else {
    m_SelectByBadDepthCal->SetState(kButtonUp);
  }
  AddFrame(m_SelectByBadDepthCal, LabelLayout);

  m_SelectByDepthCut = new TGCheckButton(this, "Enable event selection by depth cut");
  m_SelectByDepthCut->Associate(this);
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSelectByDepthCut() == true) {
    m_SelectByDepthCut->SetState(kButtonDown);
  } else {
    m_SelectByDepthCut->SetState(kButtonUp);
  }
  AddFrame(m_SelectByDepthCut, LabelLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsEventSelector::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsEventSelector::OnApply()
{
  // Modify this to store the data in the module!

  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsFits(m_FileOptions->IsSelected(0));
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsDat(m_FileOptions->IsSelected(1));
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsROOT(m_FileOptions->IsSelected(2));
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEnergyResponseAsROOT(m_FileOptions->IsSelected(3));

  if (m_SaveBeforeSelections->GetState() == kButtonDown) {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveBeforeSelections(true);
  } else {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveBeforeSelections(false);
  }

  if (m_Energies->GetMaxValueDouble() <= m_Energies->GetMinValueDouble()) {
    mgui<<"The minimum energy must be smaller than the maximum energy!"<<show;
    return false;
  }
  
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetEnergyMin(m_Energies->GetMinValueDouble());
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetEnergyMax(m_Energies->GetMaxValueDouble());
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetDepthMax(m_DepthMax->GetAsDouble());


  if (m_SelectByBadDepthCal->GetState() == kButtonDown) {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSelectByBadDepthCal(true);
  } else {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSelectByBadDepthCal(false);
  }

  if (m_SelectByDepthCut->GetState() == kButtonDown) {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSelectByDepthCut(true);
  } else {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSelectByDepthCut(false);
  }

  
  return true;
}


// NGUIOptionsEventSelector: the end...
////////////////////////////////////////////////////////////////////////////////
