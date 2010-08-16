/*
 * NGUIOptionsSource.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __NGUIOptionsSource__
#define __NGUIOptionsSource__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include <TROOT.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TString.h>
#include <TGButton.h>
#include <TGComboBox.h>

// MEGAlib libs:
#include "MGUIEEntry.h"
#include "MGUIEFileSelector.h"

// NuSIM libs:
#include "NGlobal.h"
#include "NSource.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NGUIOptionsSource : public TGCompositeFrame
{
  // public interface:
 public:
  //! Standard constructor
  NGUIOptionsSource(NSource* Source, const TGWindow* Parent); 
  //! Default destructor
  virtual ~NGUIOptionsSource();

  //! Create the GUI
  void Create();
  //! Process or redirect all messages
  bool ProcessMessage(long Message, long Parameter1, long Parameter2);

  //! Update the source data
  void UpdateSource();

  // private methods:
 private:
  //! Update the options frames
  void UpdateOptions();

  // protected members:
 protected:
  //! The Source itself
  NSource* m_Source;

  TGComboBox* m_BeamTypes;
  TGGroupFrame* m_BeamOptionsFrame;
  TGCompositeFrame* m_BeamOptionsSubFrame;

  TGComboBox* m_SpectralTypes;
  TGGroupFrame* m_SpectralOptionsFrame;
  TGCompositeFrame* m_SpectralOptionsSubFrame;

  MGUIEEntry* m_Name;
  MGUIEEntry* m_Flux;
  MGUIEEntry* m_P1;
  MGUIEEntry* m_P2;
  MGUIEEntry* m_P3;
  MGUIEEntry* m_P4;
  MGUIEEntry* m_P5;
  MGUIEEntry* m_P6;
  MGUIEEntry* m_P7;
  MGUIEEntry* m_E1;
  MGUIEEntry* m_E2;
  MGUIEEntry* m_E3;
  MGUIEEntry* m_E4;
  MGUIEEntry* m_E5;
  MGUIEFileSelector* m_EF;

  int c_BeamComboBox;
  int c_SpectralComboBox;

#ifdef ___CINT___
 public:
  ClassDef(NGUIOptionsSource, 0) // GUI window for unkown purpose ...
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
