/*
 * NGUIOptionsBackgroundSimulator.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsBackgroundSimulator
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsBackgroundSimulator.h"

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
#include "NModuleBackgroundSimulator.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsBackgroundSimulator)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsBackgroundSimulator::NGUIOptionsBackgroundSimulator(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsBackgroundSimulator::~NGUIOptionsBackgroundSimulator()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsBackgroundSimulator::Create()
{
  PreCreate();

  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsLeft, 20, 20, 20, 2);
  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 50, 20, 5, 10);

  TGLabel* ApertureLabel = new TGLabel(this, "Aperture (sampled from distributions):");
  AddFrame(ApertureLabel, LabelLayout);

  m_ApertureSpectrumFile = new MGUIEFileSelector(this, "Choose the aperture spectrum file:", 
                                       dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->GetApertureSpectrum());
  m_ApertureSpectrumFile->SetFileType("Aperture spectrum", "*.dat");
	AddFrame(m_ApertureSpectrumFile, FileLayout);

  m_ApertureFieldOfViewFile = new MGUIEFileSelector(this, "Choose the aperture file containing the field-of-view on the detector:", 
                                                    dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->GetApertureFieldOfView());
  m_ApertureFieldOfViewFile->SetFileType("Aperture field-of-view", "*.dat");
	AddFrame(m_ApertureFieldOfViewFile, FileLayout);


  TGLabel* DetectorLabel = new TGLabel(this, "Detectors hits (extracted from data base):");
  AddFrame(DetectorLabel, LabelLayout);

  m_DetectorDB = new MGUIEFileSelector(this, "Choose the background data base:", 
                                       dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->GetDetectorDataBase());
  m_DetectorDB->SetFileType("Detector data base", "*.dat");

	AddFrame(m_DetectorDB, FileLayout);


  TGLabel* ShieldLabel = new TGLabel(this, "Shield hits without detector hits (can be left empty to significantly speed up simulations):");
  AddFrame(ShieldLabel, LabelLayout);

  m_ShieldFile = new MGUIEFileSelector(this, "Choose the shield distribution:", 
                                       dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->GetShieldHitSpectrum());
  m_ShieldFile->SetFileType("Shield distribution", "*.dat");
	AddFrame(m_ShieldFile, FileLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsBackgroundSimulator::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsBackgroundSimulator::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->SetApertureSpectrum(m_ApertureSpectrumFile->GetFileName());
  dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->SetApertureFieldOfView(m_ApertureFieldOfViewFile->GetFileName());
  dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->SetDetectorDataBase(m_DetectorDB->GetFileName());
  dynamic_cast<NModuleBackgroundSimulator*>(m_Module)->SetShieldHitSpectrum(m_ShieldFile->GetFileName());

	return true;
}


// NGUIOptionsBackgroundSimulator: the end...
////////////////////////////////////////////////////////////////////////////////
