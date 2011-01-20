/*
 * NMetrologyDetectorShift.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMetrologyDetectorShift
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMetrologyDetectorShift.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMetrologyDetectorShift)
#endif


////////////////////////////////////////////////////////////////////////////////


NMetrologyDetectorShift::NMetrologyDetectorShift()
{
  // Construct an instance of NMetrologyDetectorShift

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NMetrologyDetectorShift::~NMetrologyDetectorShift()
{
  // Delete this instance of NMetrologyDetectorShift
}


////////////////////////////////////////////////////////////////////////////////


const NMetrologyDetectorShift& NMetrologyDetectorShift::operator=(const NMetrologyDetectorShift& M)
{
  //! Assignment operator

  m_Empty = M.m_Empty;

  m_Position = M.m_Position;
  m_Shift = M.m_Shift;
  
  return (*this);
}
  
  
////////////////////////////////////////////////////////////////////////////////


void NMetrologyDetectorShift::Clear()
{
  //! Resets all data to default values
 
  m_Position = g_VectorNotDefined;
  m_Shift = g_VectorNotDefined;
  
  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////
  

TString NMetrologyDetectorShift::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "MD-shift: X=";
 Text += m_Position[0];
 Text += ", Y=";
 Text += m_Position[1];
 Text += ", dx=";
 Text += m_Shift[0];
 Text += ", dz=";
 Text += m_Shift[1];
 
 return Text;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NMetrologyDetectorShift::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator(',');
  T.Analyze(Line, false);
      
  if (T.GetNTokens() != 4) {
    mgui<<"Not enough tokens in string for metrology detector shift entry ("<<T.GetNTokens()<<" vs. "<<4<<")"<<show;
    return false;
  }

  m_Position.SetXYZ(T.GetTokenAtAsDouble(0), T.GetTokenAtAsDouble(1), 0.0);
  m_Shift.SetXYZ(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), 0.0);
  
  m_Empty = false;
  
  return true;
}


// NMetrologyDetectorShift.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
