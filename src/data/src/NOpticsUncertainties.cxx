/*
 * NOpticsUncertainties.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NOpticsUncertainties
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NOpticsUncertainties.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NOpticsUncertainties)
#endif


////////////////////////////////////////////////////////////////////////////////


NOpticsUncertainties::NOpticsUncertainties()
{
  // Construct an instance of NOpticsUncertainties

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NOpticsUncertainties::~NOpticsUncertainties()
{
  // Delete this instance of NOpticsUncertainties
}


////////////////////////////////////////////////////////////////////////////////


void NOpticsUncertainties::Clear()
{
  //! Resets all data to default values
 
  m_BoreSightOptics1 = g_DoubleNotDefined;
  m_BoreSightOptics2 = g_DoubleNotDefined;

  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NOpticsUncertainties::SetInterpolated(const NOpticsUncertainties& A, const NOpticsUncertainties& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;
   
  m_BoreSightOptics1 = A.m_BoreSightOptics1 + Fraction*(B.m_BoreSightOptics1 - A.m_BoreSightOptics1);
  m_BoreSightOptics2 = A.m_BoreSightOptics2 + Fraction*(B.m_BoreSightOptics2 - A.m_BoreSightOptics2);

  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


MVector NOpticsUncertainties::GetBoreSightOpticsModule(int OpticsID)
{
  //! Get the pointing error of the star tracker

  if (OpticsID == 1) {
    return m_BoreSightOptics1; 
  } else if (OpticsID == 2) {
    return m_BoreSightOptics2; 
  } else {
    cout<<"SEVERE ERROR: Optics ID = "<<OpticsID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NOpticsUncertainties::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  // We require a higher precision here...
  streamsize p = S.precision();
  S.precision(12);

  S<<"MU ";
  S<<m_Time.GetSeconds()<<" ";
  S<<m_BoreSightOptics1[0]<<" "<<m_BoreSightOptics1[1]<<" "<<m_BoreSightOptics1[2]<<" ";
  S<<m_BoreSightOptics2[0]<<" "<<m_BoreSightOptics2[1]<<" "<<m_BoreSightOptics2[2]<<" ";
  S<<endl;

  S.precision(p);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////
  

TString NOpticsUncertainties::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "MU: ";
 Text += "Time: ";
 Text += m_Time.GetSeconds();
 Text += ", bore sight optics 1: ";
 Text += m_BoreSightOptics1.ToString();
 Text += ", bore sight optics 2: ";
 Text += m_BoreSightOptics2.ToString();
 
 return Text;
}

////////////////////////////////////////////////////////////////////////////////
  

bool NOpticsUncertainties::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  
  
  MTokenizer T;
  T.Analyze(Line);
  
  if (T.GetNTokens() != 7) {
    merr<<"Not enough tokens in string for parsing!"<<show;
  }
  
  m_Time.SetSeconds(T.GetTokenAtAsDouble(1));
  m_BoreSightOptics1.SetXYZ(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4));
  m_BoreSightOptics2.SetXYZ(T.GetTokenAtAsDouble(5), T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7));
 
  m_Empty = false;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NOpticsUncertainties::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  double Time;
 
  int E = sscanf(Line.Data(), 
                 "%lf,," // m_Time
                 "%lf,%lf,%lf," // m_BoreSightOptics1
                 "%lf,%lf,%lf" // m_BoreSightOptics2
                 ,
                 &Time, 
                 &m_BoreSightOptics1[0], &m_BoreSightOptics1[1], &m_BoreSightOptics1[2],
                 &m_BoreSightOptics2[0], &m_BoreSightOptics2[1], &m_BoreSightOptics2[2]);
  if (E != 7) {
    cerr<<"Unable to scan optics uncertainties. Scanned entries "<<E<<" != 7"<<endl;
    cerr<<"Content of line: "<<endl;
    cerr<<Line.Data()<<endl;
    Clear();
    return false;
  }
  
  m_Time.SetSeconds(Time);
  
  m_Empty = false;

  return true;
}


// NOpticsUncertainties.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
