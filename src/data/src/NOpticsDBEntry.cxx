/*
 * NOpticsDBEntry.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NOpticsDBEntry
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NOpticsDBEntry.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NOpticsDBEntry)
#endif


////////////////////////////////////////////////////////////////////////////////


NOpticsDBEntry::NOpticsDBEntry()
{
  // Construct an instance of NOpticsDBEntry

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NOpticsDBEntry::~NOpticsDBEntry()
{
  // Delete this instance of NOpticsDBEntry
}


////////////////////////////////////////////////////////////////////////////////


void NOpticsDBEntry::Clear()
{
  //! Resets all data to default values
 
  m_BoreSight1RelOM = g_DoubleNotDefined;
  m_BoreSight2RelOM = g_DoubleNotDefined;

  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NOpticsDBEntry::SetInterpolated(const NOpticsDBEntry& A, const NOpticsDBEntry& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;
   
  m_BoreSight1RelOM = A.m_BoreSight1RelOM + Fraction*(B.m_BoreSight1RelOM - A.m_BoreSight1RelOM);
  m_BoreSight2RelOM = A.m_BoreSight2RelOM + Fraction*(B.m_BoreSight2RelOM - A.m_BoreSight2RelOM);

  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


MVector NOpticsDBEntry::GetBoreSightRelOM(int OpticsID)
{
  //! Get the pointing error of the star tracker

  if (OpticsID == 1) {
    return m_BoreSight1RelOM; 
  } else if (OpticsID == 2) {
    return m_BoreSight2RelOM; 
  } else {
    cout<<"SEVERE ERROR: Optics ID = "<<OpticsID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NOpticsDBEntry::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  // We require a higher precision here...
  streamsize p = S.precision();
  S.precision(12);

  S<<"MU ";
  S<<m_Time.GetAsSeconds()<<" ";
  S<<m_BoreSight1RelOM[0]<<" "<<m_BoreSight1RelOM[1]<<" "<<m_BoreSight1RelOM[2]<<" ";
  S<<m_BoreSight2RelOM[0]<<" "<<m_BoreSight2RelOM[1]<<" "<<m_BoreSight2RelOM[2]<<" ";
  S<<endl;

  S.precision(p);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////
  

TString NOpticsDBEntry::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "MU: ";
 Text += "Time: ";
 Text += m_Time.GetAsSeconds();
 Text += ", bore sight optics 1: ";
 Text += m_BoreSight1RelOM.ToString();
 Text += ", bore sight optics 2: ";
 Text += m_BoreSight2RelOM.ToString();
 
 return Text;
}

////////////////////////////////////////////////////////////////////////////////
  

bool NOpticsDBEntry::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  
  
  MTokenizer T;
  T.Analyze(Line);
  
  if (T.GetNTokens() != 7) {
    merr<<"Not enough tokens in string for parsing ("<<T.GetNTokens()<<" vs. "<<4<<")"<<show;
  }
  
  m_Time.Set(T.GetTokenAtAsDouble(1));
  m_BoreSight1RelOM.SetXYZ(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4));
  m_BoreSight2RelOM.SetXYZ(T.GetTokenAtAsDouble(5), T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7));
 
  m_Empty = false;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NOpticsDBEntry::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator(',');
  T.Analyze(Line, false);
    
  if (T.GetNTokens() != 8) {
    mgui<<"Not enough tokens in string for optics DB entry!"<<show;
    return false;
  }

  m_Time.Set(T.GetTokenAtAsDouble(0));
  
  m_BoreSight1RelOM[0] = T.GetTokenAtAsDouble(2);
  m_BoreSight1RelOM[1] = T.GetTokenAtAsDouble(3);
  m_BoreSight1RelOM[2] = T.GetTokenAtAsDouble(4);
  
  m_BoreSight2RelOM[0] = T.GetTokenAtAsDouble(5);
  m_BoreSight2RelOM[1] = T.GetTokenAtAsDouble(6);
  m_BoreSight2RelOM[2] = T.GetTokenAtAsDouble(7);

  
  m_Empty = false;

  return true;
}


// NOpticsDBEntry.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
