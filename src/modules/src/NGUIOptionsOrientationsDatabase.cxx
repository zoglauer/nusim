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

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);

  m_MetrologyDB = new MGUIEFileSelector(this, "Metrology system errors database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetMetrologyDBFileName());
  m_MetrologyDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_MetrologyDB, FileLayout);

  m_OpticsDB = new MGUIEFileSelector(this, "Optics system errors database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetOpticsDBFileName());
  m_OpticsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_OpticsDB, FileLayout);

  m_CalibratedAlignmentsDB = new MGUIEFileSelector(this, "Calibrated alignments database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetCalibratedAlignmentsDBFileName());
  m_CalibratedAlignmentsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_CalibratedAlignmentsDB, FileLayout);

  m_PerturbedAlignmentsDB = new MGUIEFileSelector(this, "Choose either a perturbed or an ideal alignments database:", 
                                           dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->GetPerturbedAlignmentsDBFileName());
  m_PerturbedAlignmentsDB->SetFileType("CSV data base", "*.csv");
  AddFrame(m_PerturbedAlignmentsDB, FileLayout);



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

  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetMetrologyDBFileName(m_MetrologyDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetOpticsDBFileName(m_OpticsDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetCalibratedAlignmentsDBFileName(m_CalibratedAlignmentsDB->GetFileName());
  dynamic_cast<NModuleOrientationsDatabase*>(m_Module)->SetPerturbedAlignmentsDBFileName(m_PerturbedAlignmentsDB->GetFileName());

	return true;
}


// NGUIOptionsOrientationsDatabase: the end...
////////////////////////////////////////////////////////////////////////////////
