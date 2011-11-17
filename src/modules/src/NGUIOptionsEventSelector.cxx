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

  TGLayoutHints* FileLabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 15, 10);
  TGLabel* FileLabel = new TGLabel(this, "Event saving options:");
  AddFrame(FileLabel, FileLabelLayout);

  TGLayoutHints* FileOptionsLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 20, 20, 10, 5);
  TGLabel* FileOptions = new TGLabel(this, "Choose how to save the output file (you can choose none or multiple):");
  AddFrame(FileOptions, FileOptionsLayout);

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX | kLHintsExpandY, 40, 20, 2, 2);

  m_SaveAsFits = new TGCheckButton(this, "Events in FITS format (*.events.fits)", c_SaveAsFitsID);
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsFits() == true) {
    m_SaveAsFits->SetState(kButtonDown);
  } else {
    m_SaveAsFits->SetState(kButtonUp);    
  }
  AddFrame(m_SaveAsFits, FileLayout);

  TGLayoutHints* PixelSizeLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 61, 20, 2, 2);
  m_PixelSize = new MGUIEEntry(this, "Pixel size for fits file [arcsec]", false,
                               dynamic_cast<NModuleEventSelector*>(m_Module)->GetPixelSize()/arcsec);
  AddFrame(m_PixelSize, PixelSizeLayout);
  
  m_SaveExposureMap = new TGCheckButton(this, "Save also (time-only) exposure map (*.exposure.fits)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetCreateExposureMap() == true) {
    m_SaveExposureMap->SetState(kButtonDown);
  } else {
    m_SaveExposureMap->SetState(kButtonUp);    
  }
  AddFrame(m_SaveExposureMap, PixelSizeLayout);

  /*
  if (m_SaveAsFits->GetState() == kButtonDown) {
    m_PixelSize->SetEnabled(true);
    m_SaveExposureMap->SetEnabled(true);
  } else if (m_SaveAsFits->GetState() == kButtonUp) {
    m_PixelSize->SetEnabled(false);
    m_SaveExposureMap->SetEnabled(false);
  }
  */
  
  m_SaveAsASCII = new TGCheckButton(this, "Events in ASCII format (*.events.dat)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsDat() == true) {
    m_SaveAsASCII->SetState(kButtonDown);
  } else {
    m_SaveAsASCII->SetState(kButtonUp);    
  }
  AddFrame(m_SaveAsASCII, FileLayout);

  m_SaveAsROOT = new TGCheckButton(this, "Events in ROOT format (*.events.root)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEventsAsROOT() == true) {
    m_SaveAsROOT->SetState(kButtonDown);
  } else {
    m_SaveAsROOT->SetState(kButtonUp);    
  }
  AddFrame(m_SaveAsROOT, FileLayout);

  m_SaveSpectralResponse = new TGCheckButton(this, "Spectral response in ROOT format (*.energyresponse.root)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveEnergyResponseAsROOT() == true) {
    m_SaveSpectralResponse->SetState(kButtonDown);
  } else {
    m_SaveSpectralResponse->SetState(kButtonUp);    
  }
  AddFrame(m_SaveSpectralResponse, FileLayout);
  
  
  TGLayoutHints* SaveBeforeSelectionsLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 15);
  m_SaveBeforeSelections = new TGCheckButton(this, "Save events before selections (otherwise after)");
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSaveBeforeSelections() == true) {
    m_SaveBeforeSelections->SetState(kButtonDown);
  } else {
    m_SaveBeforeSelections->SetState(kButtonUp);    
  }
  AddFrame(m_SaveBeforeSelections, SaveBeforeSelectionsLayout);

  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 15, 10);
  TGLabel* SelectionsLabel = new TGLabel(this, "Event selection options:");
  AddFrame(SelectionsLabel, LabelLayout);
  
  
  TGLayoutHints* MinMaxLayout = new TGLayoutHints(kLHintsLeft | kLHintsTop, 20, 20, 10, 5);
  
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
  AddFrame(m_SelectByBadDepthCal, MinMaxLayout);

  m_SelectByDepthCut = new TGCheckButton(this, "Enable event selection by depth cut");
  m_SelectByDepthCut->Associate(this);
  if (dynamic_cast<NModuleEventSelector*>(m_Module)->GetSelectByDepthCut() == true) {
    m_SelectByDepthCut->SetState(kButtonDown);
  } else {
    m_SelectByDepthCut->SetState(kButtonUp);
  }
  AddFrame(m_SelectByDepthCut, MinMaxLayout);

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
    case kCM_CHECKBUTTON:
      switch (Parameter1) {
      case c_SaveAsFitsID:
        /*
        if (m_SaveAsFits->GetState() == kButtonDown) {
          m_PixelSize->SetEnabled(true);
          m_SaveExposureMap->SetEnabled(true);
        } else {
          m_PixelSize->SetEnabled(false);
          m_SaveExposureMap->SetEnabled(false);
        }
        */

      default:
        break; 
      }
      break;

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

  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsFits((m_SaveAsFits->GetState() == kButtonDown) ? true : false);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetPixelSize(m_PixelSize->GetAsDouble()*arcsec);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetCreateExposureMap((m_SaveExposureMap->GetState() == kButtonDown) ? true : false);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsDat((m_SaveAsASCII->GetState() == kButtonDown) ? true : false);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEventsAsROOT((m_SaveAsROOT->GetState() == kButtonDown) ? true : false);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveEnergyResponseAsROOT((m_SaveSpectralResponse->GetState() == kButtonDown) ? true : false);

  if (m_SaveBeforeSelections->GetState() == kButtonDown) {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveBeforeSelections(true);
  } else {
    dynamic_cast<NModuleEventSelector*>(m_Module)->SetSaveBeforeSelections(false);
  }

  if (m_Energies->GetMaxValueDouble() <= m_Energies->GetMinValueDouble()) {
    mgui<<"The minimum energy must be smaller than the maximum energy!"<<show;
    return false;
  }
  
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetEnergyMin(m_Energies->GetMinValueDouble()*keV);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetEnergyMax(m_Energies->GetMaxValueDouble()*keV);
  dynamic_cast<NModuleEventSelector*>(m_Module)->SetDepthMax(m_DepthMax->GetAsDouble()*mm);


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
