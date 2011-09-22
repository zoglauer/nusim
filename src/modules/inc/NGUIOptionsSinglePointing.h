/*
 * NGUIOptionsSinglePointing.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the SinglePointing-file for the copyright-notice.
 *
 */


#ifndef __NGUIOptionsSinglePointing__
#define __NGUIOptionsSinglePointing__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include <TROOT.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TString.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGNumberEntry.h>

// MEGAlib libs:
#include "MGUIEEntry.h"
#include "MGUIEFileSelector.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NPointing.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsSinglePointing : public TGCompositeFrame
{
  // public interface:
 public:
  //! Standard constructor
  NGUIOptionsSinglePointing(const TGWindow* Parent, NPointing* Pointing, unsigned int ID); 
  //! Default destructor
  virtual ~NGUIOptionsSinglePointing();

  //! Create the GUI
  void Create();
  //! Process or redirect all messages
  bool ProcessMessage(long Message, long Parameter1, long Parameter2);
  //! Associate all messages to this window
  void Associate(TGWindow* Associate);
  //! Update the data in the pointing
  void Update();
  
  static const int c_Delete;
  static const int c_AddAfter;
  static const int c_Up;
  static const int c_Down;
  
  
  // private methods:
 private:


  // protected members:
 protected:
  //! The SinglePointing itself
  NPointing* m_Pointing;
  //! ID of this pointing
  unsigned int m_ID;
  
  MGUIEEntry* m_Dec;
  MGUIEEntry* m_Ra;
  MGUIEEntry* m_Yaw;
  MGUIEEntry* m_Time;
  
  TGTextButton* m_DeleteButton;
  TGTextButton* m_AddAfterButton;
  TGTextButton* m_UpButton;
  TGTextButton* m_DownButton;

#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsSinglePointing, 0) // GUI window for unkown purpose ...
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
