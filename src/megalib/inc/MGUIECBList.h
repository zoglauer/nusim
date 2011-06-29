/*
 * MGUIECBList.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MGUIECBList__
#define __MGUIECBList__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TString.h>
#include <TGButton.h>
#include <TObjArray.h>
#include <TArrayI.h>
#include <TGCanvas.h>

// MEGAlib libs:
#include "MGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MGUIECBList : public TGCanvas
{
  // public interface:
 public:
  MGUIECBList(const TGWindow *Parent, TString Label = "", bool Multiple = true);
  virtual ~MGUIECBList();

	void SetMultiple(bool IsMultiple);
	void SetEnabled(bool flag = true);
  bool IsEnabled() { return m_IsEnabled; }

  //! Set the wrap length of all text
	virtual void SetWrapLength(int WrapLength);

  bool ProcessMessage(long Message, long Parameter1, long Parameter2);
  void Associate(TGWindow* Associate);

  TArrayI GetSelected();
  int GetSelected(int i);
  bool IsSelected(int i);
  TString GetName(int i);
  void SetSelectedBitEncoded(int Selected);
  void SetSelected(int i, bool Selected);
  int GetSelectedBitEncoded();
  int GetNEntries() { return m_CBList->GetSize(); }
  void Add(TString CBLabel, bool Selected = false);
  void Create();

  void SelectAll();
  void SelectNone();

  // protected methods:
 protected:


  // private methods:
 private:


  // protected members:
 protected:


  // private members:
 private:
  TGWindow *m_Parent;                       // parent frame

	bool m_IsMultiple;                      // true, if multiple selections are allowed
	bool m_IsEnabled;                       // true, if it is enabled

  TGLabel *m_TextLabel;                     // the label of the GUI element
  TGLayoutHints *m_TextLabelLayout;         // its layout

  TString m_Label;                          // description label
  TObjArray *m_CBList;                      // array containing the buttons
  TObjArray *m_NamesList;                   // array containing the names of the buttons
  TGLayoutHints *m_CBLayout;                // the layout of the checknuttons

  TGWindow* m_Associate;

  //! The wrap length of text labels:
  int m_WrapLength;

#ifdef ___CINT___
 public:
  ClassDef(MGUIECBList, 0) // Basic GUI element: a check button list
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
