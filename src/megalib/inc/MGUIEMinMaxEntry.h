/*
 * MGUIEMinMaxEntry.h
 *
 * Copyright (C) 1998-2010 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MGUIEMinMaxEntry__
#define __MGUIEMinMaxEntry__


////////////////////////////////////////////////////////////////////////////////


// Standard libs::
#include <limits>
using namespace std;

// ROOT libs:
#include <TROOT.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TString.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TGGC.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MGUIElement.h"
#include "MGUIEEntry.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MGUIEMinMaxEntry : public MGUIElement
{
  // public interface:
 public:
  MGUIEMinMaxEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
									 double MinValue, double MaxValue,
									 bool Limits = true, 
                   double Min = -numeric_limits<double>::max(), 
                   double Max = numeric_limits<double>::max());
  MGUIEMinMaxEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
									 TString MinLabel, TString MaxLabel, 
                   double MinValue, double MaxValue,
									 bool Limits = true, 
                   double Min = -numeric_limits<double>::max(), 
                   double Max = numeric_limits<double>::max());
  MGUIEMinMaxEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
									 int MinValue, int MaxValue,
									 bool Limits = true, 
                   int Min = numeric_limits<int>::min(), 
                   int Max = numeric_limits<int>::max());
  MGUIEMinMaxEntry(const TGWindow* Parent, TString Label, bool Emphasize, 
									 TString MinLabel, TString MaxLabel, 
                   int MinValue, int MaxValue,
									 bool Limits = true, 
                   int Min = numeric_limits<int>::min(), 
                   int Max = numeric_limits<int>::max());
  virtual ~MGUIEMinMaxEntry();

  bool CheckRange(double MinMin, double MinMax, 
                  double MaxMin, double MaxMax, 
                  bool MinLMax = true);

  bool CheckRange(int MinMin, int MinMax, 
                  int MaxMin, int MaxMax, 
                  bool MinLMax = true);

  double GetMinValue() { return GetMinValueDouble(); } 
  double GetMaxValue() { return GetMaxValueDouble();};
  double GetMinValueDouble();
  double GetMaxValueDouble();
  int GetMinValueInt();
  int GetMaxValueInt();

  void SetEntryFieldSize(int Size = 70);
	void SetEnabled(bool flag = true);

  //! Return true if the content has been modified 
  bool IsModified();


  // protected methods:
 protected:
  void Init();

  // private methods:
 private:
  void Create(int Mode);
  TString MakeSmartString(double Number);
  bool CheckForNumber(TString Number);
  double ToNumber(TString Number);


  // protected members:
 protected:


  // private members:
 private:
  TString m_Label;

  TString m_MinLabel;
  TString m_MaxLabel;

  double m_MinValue;
  double m_MaxValue;
	
  int m_Type;
	bool m_Limits;
	double m_Min;
	double m_Max;
  int m_Size;

  TGLabel* m_TextLabel;
  TGLayoutHints* m_TextLabelLayout;

  TGLayoutHints* m_EntryLayout;
	MGUIEEntry* m_MinEntry;
	MGUIEEntry* m_MaxEntry;

  enum Type { e_Integer, e_Double };


#ifdef ___CINT___
 public:
  ClassDef(MGUIEMinMaxEntry, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
