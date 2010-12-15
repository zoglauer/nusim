/*
 * NGUIOptionsPointingPattern.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsPointingPattern
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsPointingPattern.h"

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
#include "NModuleSourceDistribution.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsPointingPattern)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPattern::NGUIOptionsPointingPattern(NModule* Module) 
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsPointingPattern::~NGUIOptionsPointingPattern()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsPointingPattern::Create()
{
  PreCreate();

  TGLabel* Intro = new TGLabel(this, "This samples the initial photon directions, and generates a pointing pattern, which covers all the photons, steered by the below parameters. Start the sampling by pressing OK. The following process can not be interupted. Afterwards load the generated pattern file into the pointing module");
  Intro->SetWrapLength(530);
  TGLayoutHints* IntroLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 30, 5);
  AddFrame(Intro, IntroLayout);

  m_NTestPhotons = new MGUIEEntry(this, "Number of photons to simulate (e.g. 1,000,000):", false, dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetNTestPhotons());
  TGLayoutHints* FirstNumberEntryLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 30, 5);
	AddFrame(m_NTestPhotons, FirstNumberEntryLayout);

  m_DistanceBetweenPointings = new MGUIEEntry(this, "Distance between pointings in arcmin (e.g. 7.3 arcmin):", false, dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetDistanceBetweenPointings());
  TGLayoutHints* NumberEntryLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 5);
	AddFrame(m_DistanceBetweenPointings, NumberEntryLayout);

  m_MaxAllowedDistanceForAcceptance = new MGUIEEntry(this, "Maximum radial distance pointing <-> sim photon in arcmin (e.g. 2.0 arcmin):", false, dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetMaxAllowedDistanceForAcceptance());
	AddFrame(m_MaxAllowedDistanceForAcceptance, NumberEntryLayout);

  TGLayoutHints* FileLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 10);
  m_FileName = new MGUIEFileSelector(this, "Choose a file name to save the pointing pattern to:", 
                                     dynamic_cast<NModuleSourceDistribution*>(m_Module)->GetPointingPatternFileName());
  m_FileName->SetFileType("Pointing pattern file name", "*.dat");

	AddFrame(m_FileName, FileLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsPointingPattern::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsPointingPattern::OnApply()
{
	// Modify this to store the data in the module!

  dynamic_cast<NModuleSourceDistribution*>(m_Module)->SetPointingPatternFileName(m_FileName->GetFileName());
  dynamic_cast<NModuleSourceDistribution*>(m_Module)->SetNTestPhotons(m_NTestPhotons->GetAsInt());
  dynamic_cast<NModuleSourceDistribution*>(m_Module)->SetDistanceBetweenPointings(m_DistanceBetweenPointings->GetAsDouble());
  dynamic_cast<NModuleSourceDistribution*>(m_Module)->SetMaxAllowedDistanceForAcceptance(m_MaxAllowedDistanceForAcceptance->GetAsDouble());

  dynamic_cast<NModuleSourceDistribution*>(m_Module)->GeneratePointingPattern();

	return true;
}


// NGUIOptionsPointingPattern: the end...
////////////////////////////////////////////////////////////////////////////////
