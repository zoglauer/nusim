/*
 * NGUIOptionsOrientationsDatabase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsOrientationsDatabase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsOrientationsDatabase.h"

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
#include "NModuleOrientationsDatabase.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsOrientationsDatabase)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrientationsDatabase::NGUIOptionsOrientationsDatabase(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsOrientationsDatabase::~NGUIOptionsOrientationsDatabase()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsOrientationsDatabase::Create()
{
  PreCreate();

  TGLayoutHints* FileLayoutTop = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 15, 10);
  TGLayoutHints* FileLayoutBottom = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 10, 15);

  m_CalibratedMetrologyDB = new MGUIEFileSelector(this, "Pre-launch alignment metrology system database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetCalibratedMetrologyDBFileName());
  m_CalibratedMetrologyDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_CalibratedMetrologyDB, FileLayoutTop);

  m_PerturbedMetrologyDB = new MGUIEFileSelector(this, "Perturbed OR pre-launch metrology system database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetPerturbedMetrologyDBFileName());
  m_PerturbedMetrologyDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_PerturbedMetrologyDB, FileLayoutBottom);

  
  m_CalibratedOpticsDB = new MGUIEFileSelector(this, "Pre-launch alignment optics system database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetCalibratedOpticsDBFileName());
  m_CalibratedOpticsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_CalibratedOpticsDB, FileLayoutTop);
  
  m_PerturbedOpticsDB = new MGUIEFileSelector(this, "Perturbed OR pre-launch optics system database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetPerturbedOpticsDBFileName());
  m_PerturbedOpticsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_PerturbedOpticsDB, FileLayoutBottom);

  
  m_CalibratedAlignmentsDB = new MGUIEFileSelector(this, "Pre-launch alignments database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetCalibratedAlignmentsDBFileName());
  m_CalibratedAlignmentsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_CalibratedAlignmentsDB, FileLayoutTop);

  m_PerturbedAlignmentsDB = new MGUIEFileSelector(this, "Perturbed OR pre-launch alignments database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetPerturbedAlignmentsDBFileName());
  m_PerturbedAlignmentsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_PerturbedAlignmentsDB, FileLayoutBottom);



  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsOrientationsDatabase::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsOrientationsDatabase::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetCalibratedMetrologyDBFileName(m_CalibratedMetrologyDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetPerturbedMetrologyDBFileName(m_PerturbedMetrologyDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetCalibratedOpticsDBFileName(m_CalibratedOpticsDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetPerturbedOpticsDBFileName(m_PerturbedOpticsDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetCalibratedAlignmentsDBFileName(m_CalibratedAlignmentsDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetPerturbedAlignmentsDBFileName(m_PerturbedAlignmentsDB->GetFileName());

	return true;
}


// NGUIOptionsOrientationsDatabase: the end...
////////////////////////////////////////////////////////////////////////////////
