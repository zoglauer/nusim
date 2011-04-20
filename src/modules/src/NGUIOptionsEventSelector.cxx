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

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 20, 5);

  m_FileName = new MGUIEFileSelector(this, "Save events before or after the event selection to the file:\nSuffix = *.fits: FITS, *.root: ROOT, *.dat: ASCII\nIf empty, don't save anything", 
                                     dynamic_cast<NModuleInterfaceIO*>(m_Module)->GetFileName());
  m_FileName->SetFileType("Event file", "*.dat");
  m_FileName->SetFileType("Event file", "*.fits");
  m_FileName->SetFileType("Event file", "*.root");
  AddFrame(m_FileName, FileLayout);

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

  TString Name = m_FileName->GetFileName();
  if (Name != "") {
    TString FileSuffix = Name(Name.Last('.'), Name.Length() - Name.Last('.'));
    if (FileSuffix.Contains("fits") == false && FileSuffix.Contains("root") == false && FileSuffix.Contains("dat") == false) {
      mgui<<"Unknown file suffix: \""<<Name<<"\". Please use either dat, root, or fits!"<<show;   
      return false;
    }
  }

  dynamic_cast<NModuleInterfaceIO*>(m_Module)->SetFileName(m_FileName->GetFileName());
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
  
	return true;
}


// NGUIOptionsEventSelector: the end...
////////////////////////////////////////////////////////////////////////////////
