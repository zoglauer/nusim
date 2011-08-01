/*
 * NGUIOptionsTimeEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsTimeEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsTimeEngine.h"

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
#include "NModuleTimeEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsTimeEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsTimeEngine::NGUIOptionsTimeEngine(NModule* Module)
  : NGUIOptions(Module)
{
  // standard constructor
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsTimeEngine::~NGUIOptionsTimeEngine()
{
  // kDeepCleanup is activated 
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsTimeEngine::Create()
{
  PreCreate();

  TGLayoutHints* DefaultLayout = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX, 20, 20, 5, 5);
  TGLayoutHints* LabelLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 20, 20, 5, 5);
  TGLayoutHints* EntryLayout = new TGLayoutHints(kLHintsTop | kLHintsRight, 20, 20, 5, 5);
  
  TGHorizontalFrame* DateFrame = new TGHorizontalFrame(this);
  AddFrame(DateFrame, DefaultLayout);
  
  TGLabel* DateLabel = new TGLabel(DateFrame, "Start date (DD/MM/YYYY): ");
  DateFrame->AddFrame(DateLabel, LabelLayout);

  NTime Time = dynamic_cast<NModuleTimeEngine*>(m_Module)->GetAbsoluteObservationStartTime();
  
  m_Date = new TGNumberEntry(DateFrame, 03022012, 8, -1, TGNumberEntry::kNESDayMYear);
  m_Date->SetDate(Time.GetYears(), Time.GetMonths(), Time.GetDays());
  DateFrame->AddFrame(m_Date, EntryLayout);
  
  
  TGHorizontalFrame* TimeFrame = new TGHorizontalFrame(this);
  AddFrame(TimeFrame, DefaultLayout);
  
  TGLabel* TimeLabel = new TGLabel(TimeFrame, "Start time (HH:MM:SS): ");
  TimeFrame->AddFrame(TimeLabel, LabelLayout);

  m_Time = new TGNumberEntry(TimeFrame, 03022012, 8, -1, TGNumberEntry::kNESHourMinSec);
  m_Time->SetTime(Time.GetHours(), Time.GetMinutes(), Time.GetSeconds());
  TimeFrame->AddFrame(m_Time, EntryLayout);


  PostCreate();
}


////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsTimeEngine::ProcessMessage(long Message, long Parameter1, long Parameter2)
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


bool NGUIOptionsTimeEngine::OnApply()
{
	// Modify this to store the data in the module!

  int Year, Month, Day;
  m_Date->GetDate(Year, Month, Day);
  int Hour, Minute, Second;
  m_Time->GetTime(Hour, Minute, Second);
  NTime Time(Year, Month, Day, Hour, Minute, Second);
  cout<<"H:"<<Hour<<":"<<Time.GetHours()<<endl;
  dynamic_cast<NModuleTimeEngine*>(m_Module)->SetAbsoluteObservationStartTime(Time);

	return true;
}


// NGUIOptionsTimeEngine: the end...
////////////////////////////////////////////////////////////////////////////////
