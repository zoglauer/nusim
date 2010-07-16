/*
 * NGUIOptionsSourceDistributionFarFieldPointSource.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


// Include the header:
#include "NGUIOptionsSourceDistributionFarFieldPointSource.h"

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
#include "NModuleSourceDistributionFarFieldPointSource.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsSourceDistributionFarFieldPointSource)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSourceDistributionFarFieldPointSource::NGUIOptionsSourceDistributionFarFieldPointSource(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSourceDistributionFarFieldPointSource::~NGUIOptionsSourceDistributionFarFieldPointSource()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSourceDistributionFarFieldPointSource::Create()
{
  PreCreate();

  TGLayoutHints* StandardLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 10, 10);

  m_Start = new MGUIEEntryList(this, "Data of nuclear line source");
  //m_Start->Add("Right ascension-differnce from pointing direction [armin]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetRightAscension());
  //m_Start->Add("Declination-differnce from pointing direction [arcmin]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetDeclination());
  m_Start->Add("Theta angle relative to focal plane pointing [armin]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetRightAscension());
  m_Start->Add("Phi angle relative to focal plane pointing [arcmin]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetDeclination());
  m_Start->Add("Radius (0 means point source) [arcmin]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetRadiusExtent());
  m_Start->Add("Energy [keV]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetEnergy());
  m_Start->Add("Flux [ph/s/mm2]: ", dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->GetFlux());

  m_Start->Create();

  AddFrame(m_Start, StandardLayout);

  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSourceDistributionFarFieldPointSource::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsSourceDistributionFarFieldPointSource::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->SetRightAscension(m_Start->GetAsDouble(0));
  TString S = m_Start->GetAsString(1);
  TFormula Formula;
  if (Formula.Compile(S) != 0) {
    mgui<<"Second entry is not a number!"<<show;
    return false;
  } else {
    ostringstream out;
    out<<Formula.Eval(0.0);
    S = out.str().c_str();
  }
  dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->SetDeclination(atof(S));
  dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->SetRadiusExtent(m_Start->GetAsDouble(2));
  dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->SetEnergy(m_Start->GetAsDouble(3));
  dynamic_cast<NModuleSourceDistributionFarFieldPointSource*>(m_Module)->SetFlux(m_Start->GetAsDouble(4));

	return true;
}


// NGUIOptionsSourceDistributionFarFieldPointSource: the end...
////////////////////////////////////////////////////////////////////////////////
