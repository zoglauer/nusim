/*
 * NPointingJitterDBEntry.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPointingJitterDBEntry
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPointingJitterDBEntry.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPointingJitterDBEntry)
#endif


////////////////////////////////////////////////////////////////////////////////


NPointingJitterDBEntry::NPointingJitterDBEntry()
{
  // Construct an instance of NPointingJitterDBEntry

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPointingJitterDBEntry::~NPointingJitterDBEntry()
{
  // Delete this instance of NPointingJitterDBEntry
}


////////////////////////////////////////////////////////////////////////////////


const NPointingJitterDBEntry& NPointingJitterDBEntry::operator=(const NPointingJitterDBEntry& P)
{
  //! Assignment operator

  m_Empty = P.m_Empty;

  m_Time = P.m_Time;

  m_Quaternion = P.m_Quaternion;

  return (*this);
}

////////////////////////////////////////////////////////////////////////////////


void NPointingJitterDBEntry::Clear()
{
  //! Resets all data to default values
 
  m_Quaternion = g_DoubleNotDefined;
  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NPointingJitterDBEntry::SetInterpolated(const NPointingJitterDBEntry& A, const NPointingJitterDBEntry& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;
  m_Quaternion.SetInterpolated(A.m_Quaternion, B.m_Quaternion, Fraction);

  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////
  

TString NPointingJitterDBEntry::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "PJ: ";
 Text += "Time: ";
 Text += m_Time.GetSeconds();
 Text += ", Quaternoin: r=";
 Text += m_Quaternion.m_R;
 Text += ", x/y/z=";
 Text += m_Quaternion.m_V.ToString();
 
 return Text;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NPointingJitterDBEntry::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator(',');
  T.Analyze(Line, false);
    
  if (T.GetNTokens() != 5) {
    mgui<<"Not enough tokens in string for PointingJitter DB entry!"<<show;
    return false;
  }

  m_Time.SetSeconds(T.GetTokenAtAsDouble(0));
  
  m_Quaternion.SetRXYZ(T.GetTokenAtAsDouble(4), T.GetTokenAtAsDouble(1), T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3));

  m_Empty = false;

  return true;
}


// NPointingJitterDBEntry.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
