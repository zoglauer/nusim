/*
 * MGUIEEntry.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MGUIEEntry__
#define __MGUIEEntry__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TString.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGNumberEntry.h>

// Standard libs::
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <ctype.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MGUIElement.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


// Actually it would be better to store all those global Ids in one place...
enum MGUIEEntryIds {
  kC_ENTRY = 2000,
  kET_CHANGED = 1
};


////////////////////////////////////////////////////////////////////////////////


class MGUIEEntry : public MGUIElement
{
  // public interface:
 public:
  MGUIEEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
             double Value = 0.0, bool Limits = true, 
             double Min = -DBL_MAX, double Max = DBL_MAX); 
  MGUIEEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
             int Value = 0, bool Limits = true, 
             int Min = -INT_MAX, int Max = INT_MAX); 
  MGUIEEntry(const TGWindow* Parent, TString Label, bool Emphasize, TString Value = " "); 
  virtual ~MGUIEEntry();

  void Init();

  void Associate(TGCompositeFrame* w, int Id);
  bool ProcessMessage(long Message, long Parameter1, long Parameter2);

  void SetLabel(TString Label) { m_Label = Label; if (m_TextLabel != 0) m_TextLabel->SetText(m_Label); }

  void SetEntryFieldSize(int Size = 70);
	virtual void SetEnabled(bool flag = true);

  bool IsInt(int Min = -INT_MAX, int Max = INT_MAX);
  bool IsDouble(double Min = -DBL_MAX, double Max = DBL_MAX);

  TString GetAsString();
  int GetAsInt();
  double GetAsDouble();

  void SetValue(double Value);
  void SetValue(int Value);
  void SetValue(unsigned int Value);
  void SetValue(TString Value);

  // private methods:
 private:
  void Create();

  TString MakeSmartString(double Number);
  bool CheckForNumber(TString Number);
  double ToNumber(TString Number);
  bool CheckRange(double Value, double Min, double Max);

  // private members:
 private:
  TGCompositeFrame* m_MessageWindow;
  int m_Id;

  TString m_Label;

  TString m_Value;

  double m_ValueAsDouble;
  double m_Min;
  double m_Max;

  int m_Type;
  bool m_Limits;
  int m_Size;

  TGLabel* m_TextLabel;
  TGLayoutHints* m_TextLabelLayout;

  TGLayoutHints* m_InputLayout;

  TGTextBuffer* m_InputBuffer;
  TGTextEntry* m_Input;
  TGNumberEntry* m_NumberInput;

  enum Type { e_String, e_Integer, e_Double };


#ifdef ___CINT___
 public:
  ClassDef(MGUIEEntry, 0) // GUI window for unkown purpose ...
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
