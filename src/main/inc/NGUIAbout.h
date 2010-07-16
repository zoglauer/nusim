/*
 * NGUIAbout.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __NGUIAbout__
#define __NGUIAbout__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:
#include "TObjArray.h"
#include "TGIcon.h"
#include "TGLabel.h"

// MEGAlib libs:
#include "NGlobal.h"
#include "MGUIDialog.h"

// Forward declarations:
class MGUIEText;
class MGUIButton;

////////////////////////////////////////////////////////////////////////////////


//! \brief The About GUI
//! The class represents the About GUI dialog window

class NGUIAbout : public MGUIDialog
{
  // Public Interface:
 public:
  NGUIAbout(const TGWindow* Parent, const TGWindow* Main);
  virtual ~NGUIAbout();

  void SetProgramName(TString Name) { m_ProgramName = Name; }
  void SetIconPath(TString IconPath) { m_IconPath = IconPath; }
  void SetLeadProgrammer(TString Lead) { m_LeadProgrammer = Lead; }
  void SetProgrammers(TString Programmers) { m_Programmers = Programmers; }
  void SetAdditionalProgrammers(TString Add) { m_AdditionalProgrammers = Add; }
  void SetUpdates(TString Updates) { m_Updates = Updates; } 
  void SetCopyright(TString Copyright) { m_Copyright = Copyright; } 
  void SetMasterReference(TString Reference) { m_MasterReference = Reference; } 
  void SetReference(TString Topic, TString Reference) { 
    m_References.push_back(Reference); 
    m_Topics.push_back(Topic);
  } 

  virtual void Create();

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  TString m_ProgramName; 
  TString m_IconPath; 
  TString m_LeadProgrammer;
  TString m_Programmers;
  TString m_AdditionalProgrammers;
  TString m_Email; 
  TString m_Updates; 
  TString m_Copyright;
  TString m_MasterReference;
  vector<TString> m_References;
  vector<TString> m_Topics;


#ifdef ___CINT___
 public:
  ClassDef(NGUIAbout, 0) // GUI window for unkown purpose ...
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
