/*
 * NGUIOptionsSinglePointing.cxx
 *
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NGUIOptionsSinglePointing
//
//
// This class is an elementary GUI-widget:
//
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NGUIOptionsSinglePointing.h"

// Standard libs:

// ROOT libs:
#include <TGFont.h>

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NGUIOptionsSinglePointing)
#endif


////////////////////////////////////////////////////////////////////////////////
  
const int NGUIOptionsSinglePointing::c_Delete   = 995;
const int NGUIOptionsSinglePointing::c_AddAfter = 996;
const int NGUIOptionsSinglePointing::c_Up       = 997;
const int NGUIOptionsSinglePointing::c_Down     = 998;

  
////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSinglePointing::NGUIOptionsSinglePointing(const TGWindow* Parent, NPointing* Pointing, unsigned long ID) :
  TGCompositeFrame(Parent, 100, 100, kChildFrame), m_Pointing(Pointing), m_ID(ID)
{
  // Standard constructor

  SetCleanup(kNoCleanup);
}


////////////////////////////////////////////////////////////////////////////////


NGUIOptionsSinglePointing::~NGUIOptionsSinglePointing()
{
  // Destruct this instance of NGUIOptionsSinglePointing
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSinglePointing::Create()
{
  // Create the label and the input-field.

  TGLayoutHints* StandardLayout =  new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);


  TGCompositeFrame* HFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame);
  AddFrame(HFrame, StandardLayout);
  
  TGLayoutHints* LabelLayout =  new TGLayoutHints(kLHintsTop | kLHintsLeft, 10, 10, 0, 0);
  TGLayoutHints* ButtonLayout =  new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 0, 0);
  
  TString Label("(");
  Label += m_ID;
  Label += "):  DEC [deg]:";
  m_Dec = new MGUIEEntry(HFrame, Label, false, m_Pointing->GetDec()/60.0, true, -90.0, 90.0);
  HFrame->AddFrame(m_Dec, LabelLayout);
  
  m_Ra = new MGUIEEntry(HFrame, "RA [deg]:", false, m_Pointing->GetRa()/60.0, true, 0.0, 360.0);
  HFrame->AddFrame(m_Ra, LabelLayout);
  
  m_Yaw = new MGUIEEntry(HFrame, "Yaw [deg]:", false, m_Pointing->GetYaw()/60.0, true, 0.0, 360.0);
  HFrame->AddFrame(m_Yaw, LabelLayout);
  
  m_Time = new MGUIEEntry(HFrame, "Time [sec or a.u.]:", false, m_Pointing->GetTime().GetAsSeconds(), true, 0.0);
  HFrame->AddFrame(m_Time, LabelLayout);
  
  m_AddAfterButton = new TGTextButton(HFrame, "Add new after", c_AddAfter);
  m_AddAfterButton->Associate(this);
  m_AddAfterButton->SetUserData((void*) m_ID);
  HFrame->AddFrame(m_AddAfterButton, ButtonLayout);
  
  m_DeleteButton = new TGTextButton(HFrame, "Delete", c_Delete);
  m_DeleteButton->Associate(this);
  m_DeleteButton->SetUserData((void*) m_ID);
  HFrame->AddFrame(m_DeleteButton, ButtonLayout);
  
  m_UpButton = new TGTextButton(HFrame, "Up", c_Up);
  m_UpButton->Associate(this);
  m_UpButton->SetUserData((void*) m_ID);
  HFrame->AddFrame(m_UpButton, ButtonLayout);
  
  m_DownButton = new TGTextButton(HFrame, "Down", c_Down);
  m_DownButton->Associate(this);
  m_DownButton->SetUserData((void*) m_ID);
  HFrame->AddFrame(m_DownButton, ButtonLayout);
  
  
  return;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSinglePointing::Update()
{
  //! update the data
  
  m_Pointing->SetRaDecYaw(m_Ra->GetAsDouble()*60, m_Dec->GetAsDouble()*60, m_Yaw->GetAsDouble()*60);
  m_Pointing->SetTime(NTime(m_Time->GetAsDouble()));
}
  

////////////////////////////////////////////////////////////////////////////////


bool NGUIOptionsSinglePointing::ProcessMessage(long Message, long Parameter1, 
                                long Parameter2)
{
  // Process the messages for this application, mainly the scollbar moves:

  switch (GET_MSG(Message)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(Message)) {
    case kCM_CHECKBUTTON:
      break;
    case kCM_COMBOBOX:
      break;
    case kCM_BUTTON:
    case kCM_MENU:
      switch (Parameter1) {
      default:
        break;
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }

  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NGUIOptionsSinglePointing::Associate(TGWindow* Associate)
{
  m_DeleteButton->Associate(Associate);
  m_AddAfterButton->Associate(Associate);
  m_UpButton->Associate(Associate);
  m_DownButton->Associate(Associate);
}


// NGUIOptionsSinglePointing.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
