/*
 * NMetrologyDBEntry.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMetrologyDBEntry
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMetrologyDBEntry.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMetrologyDBEntry)
#endif


////////////////////////////////////////////////////////////////////////////////


NMetrologyDBEntry::NMetrologyDBEntry()
{
  // Construct an instance of NMetrologyDBEntry

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NMetrologyDBEntry::~NMetrologyDBEntry()
{
  // Delete this instance of NMetrologyDBEntry
}


////////////////////////////////////////////////////////////////////////////////


const NMetrologyDBEntry& NMetrologyDBEntry::operator=(const NMetrologyDBEntry& M)
{
  //! Assignment operator

  m_Empty = M.m_Empty;

  m_Time = M.m_Time;

  m_PointingErrorStarTracker4 = M.m_PointingErrorStarTracker4;
  
  m_OriginMetrologyLaser1RelML1 = M.m_OriginMetrologyLaser1RelML1;
  m_PointingMetrologyLaser1RelML1 = M.m_PointingMetrologyLaser1RelML1;
  m_OriginMetrologyLaser2RelML2 = M.m_OriginMetrologyLaser2RelML2;
  m_PointingMetrologyLaser2RelML2 = M.m_PointingMetrologyLaser2RelML2;

  m_CentroidingErrorMetrologyDetector1 = M.m_CentroidingErrorMetrologyDetector1;
  m_CentroidingErrorMetrologyDetector2 = M.m_CentroidingErrorMetrologyDetector2;
  
  return (*this);
}
  
  
////////////////////////////////////////////////////////////////////////////////


void NMetrologyDBEntry::Clear()
{
  //! Resets all data to default values
 
  m_PointingErrorStarTracker4 = g_DoubleNotDefined;
  
  m_OriginMetrologyLaser1RelML1 = g_VectorNotDefined;
  m_OriginMetrologyLaser2RelML2 = g_VectorNotDefined;
  m_PointingMetrologyLaser1RelML1 = g_VectorNotDefined;
  m_PointingMetrologyLaser2RelML2 = g_VectorNotDefined;

  m_CentroidingErrorMetrologyDetector1 = g_DoubleNotDefined;
  m_CentroidingErrorMetrologyDetector2 = g_DoubleNotDefined;

  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NMetrologyDBEntry::SetInterpolated(const NMetrologyDBEntry& A, const NMetrologyDBEntry& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;
  m_PointingErrorStarTracker4 = A.m_PointingErrorStarTracker4 + Fraction*(B.m_PointingErrorStarTracker4 - A.m_PointingErrorStarTracker4);
   
  m_OriginMetrologyLaser1RelML1 = A.m_OriginMetrologyLaser1RelML1 + Fraction*(B.m_OriginMetrologyLaser1RelML1 - A.m_OriginMetrologyLaser1RelML1);
  m_OriginMetrologyLaser2RelML2 = A.m_OriginMetrologyLaser2RelML2 + Fraction*(B.m_OriginMetrologyLaser2RelML2 - A.m_OriginMetrologyLaser2RelML2);
   
  m_PointingMetrologyLaser1RelML1 = A.m_PointingMetrologyLaser1RelML1 + Fraction*(B.m_PointingMetrologyLaser1RelML1 - A.m_PointingMetrologyLaser1RelML1);
  m_PointingMetrologyLaser2RelML2 = A.m_PointingMetrologyLaser2RelML2 + Fraction*(B.m_PointingMetrologyLaser2RelML2 - A.m_PointingMetrologyLaser2RelML2);
  
  m_CentroidingErrorMetrologyDetector1 = A.m_CentroidingErrorMetrologyDetector1 + Fraction*(B.m_CentroidingErrorMetrologyDetector1 - A.m_CentroidingErrorMetrologyDetector1);
  m_CentroidingErrorMetrologyDetector2 = A.m_CentroidingErrorMetrologyDetector2 + Fraction*(B.m_CentroidingErrorMetrologyDetector2 - A.m_CentroidingErrorMetrologyDetector2);

  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


MVector NMetrologyDBEntry::GetOriginMetrologyLaserRelML(int MetrologyID)
{
  //! Get the origin of the metrology laser

  if (MetrologyID == 1) {
    return m_OriginMetrologyLaser1RelML1; 
  } else if (MetrologyID == 2) {
    return m_OriginMetrologyLaser2RelML2; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return MVector();
  }
}


////////////////////////////////////////////////////////////////////////////////


MVector NMetrologyDBEntry::GetPointingMetrologyLaserRelML(int MetrologyID)
{
  //! Get the pointing direction of the metrology laser

  if (MetrologyID == 1) {
    return m_PointingMetrologyLaser1RelML1; 
  } else if (MetrologyID == 2) {
    return m_PointingMetrologyLaser2RelML2; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return MVector();
  }
}


////////////////////////////////////////////////////////////////////////////////


double NMetrologyDBEntry::GetCentroidingErrorMetrologyDetector(int MetrologyID)
{
  //! Get the pointing error of the star tracker

  if (MetrologyID == 1) {
    return m_CentroidingErrorMetrologyDetector1; 
  } else if (MetrologyID == 2) {
    return m_CentroidingErrorMetrologyDetector2; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


////////////////////////////////////////////////////////////////////////////////


double NMetrologyDBEntry::GetPointingErrorStarTracker(int StarTrackerID)
{
  //! Get the pointing error of the star tracker

  if (StarTrackerID == 4) {
    return m_PointingErrorStarTracker4;
  } else {
    cout<<"SEVERE ERROR: Star tracker ID = "<<StarTrackerID<<" not allowed. Must be 4."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return 0.0;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NMetrologyDBEntry::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  // We require a higher precision here...
  streamsize p = S.precision();
  S.precision(12);

  S<<"MU ";
  S<<m_Time.GetAsSeconds()<<" ";
  S<<m_PointingErrorStarTracker4<<" ";
  S<<m_OriginMetrologyLaser1RelML1[0]<<" "<<m_OriginMetrologyLaser1RelML1[1]<<" "<<m_OriginMetrologyLaser1RelML1[2]<<" ";
  S<<m_PointingMetrologyLaser1RelML1[0]<<" "<<m_PointingMetrologyLaser1RelML1[1]<<" "<<m_PointingMetrologyLaser1RelML1[2]<<" ";
  S<<m_CentroidingErrorMetrologyDetector1<<" ";
  S<<m_OriginMetrologyLaser2RelML2[0]<<" "<<m_OriginMetrologyLaser2RelML2[1]<<" "<<m_OriginMetrologyLaser2RelML2[2]<<" ";
  S<<m_PointingMetrologyLaser2RelML2[0]<<" "<<m_PointingMetrologyLaser2RelML2[1]<<" "<<m_PointingMetrologyLaser2RelML2[2]<<" ";
  S<<m_CentroidingErrorMetrologyDetector2<<" ";
  S<<endl;

  S.precision(p);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

TString NMetrologyDBEntry::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "MU: ";
 Text += "Time: ";
 Text += m_Time.GetAsSeconds();
 Text += ", Pointing error: ";
 Text += m_PointingErrorStarTracker4;
 Text += ", Origin ML1 REL ML1: ";
 Text += m_OriginMetrologyLaser1RelML1.ToString();
 Text += ", Pointing ML1 REL ML1: ";
 Text += m_PointingMetrologyLaser1RelML1.ToString();
 Text += ", Centroiding error MD1: ";
 Text += m_CentroidingErrorMetrologyDetector1;
 Text += ", Origin ML2 REL ML2: ";
 Text += m_OriginMetrologyLaser2RelML2.ToString();
 Text += ", Pointing ML2 REL ML2: ";
 Text += m_PointingMetrologyLaser2RelML2.ToString();
 Text += ", Centroiding error MD2: ";
 Text += m_CentroidingErrorMetrologyDetector2;
 
 return Text;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NMetrologyDBEntry::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  
  
  MTokenizer T;
  T.Analyze(Line);
  
  if (T.GetNTokens() != 16) {
    merr<<"Not enough tokens in string for parsing!"<<show;
  }
  
  m_Time.Set(T.GetTokenAtAsDouble(0));
  m_PointingErrorStarTracker4 = T.GetTokenAtAsDouble(1);
  m_OriginMetrologyLaser1RelML1.SetXYZ(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4));
  m_PointingMetrologyLaser1RelML1.SetXYZ(T.GetTokenAtAsDouble(5), T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7));
  m_CentroidingErrorMetrologyDetector1 = T.GetTokenAtAsDouble(8);
  m_OriginMetrologyLaser2RelML2.SetXYZ(T.GetTokenAtAsDouble(9), T.GetTokenAtAsDouble(10), T.GetTokenAtAsDouble(11));
  m_PointingMetrologyLaser2RelML2.SetXYZ(T.GetTokenAtAsDouble(12), T.GetTokenAtAsDouble(13), T.GetTokenAtAsDouble(14));
  m_CentroidingErrorMetrologyDetector2 = T.GetTokenAtAsDouble(15);

  m_Empty = false;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NMetrologyDBEntry::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator(',');
  T.Analyze(Line, false);
      
  if (T.GetNTokens() != 20) {
    mgui<<"Not enough tokens in string for metrology DB entry ("<<T.GetNTokens()<<" vs. "<<20<<")"<<show;
    return false;
  }

  m_Time.Set(T.GetTokenAtAsDouble(0));
  
  m_PointingErrorStarTracker4 = T.GetTokenAtAsDouble(2);
  
  m_OriginMetrologyLaser1RelML1[0] = T.GetTokenAtAsDouble(6);
  m_OriginMetrologyLaser1RelML1[1] = T.GetTokenAtAsDouble(7);
  m_OriginMetrologyLaser1RelML1[2] = T.GetTokenAtAsDouble(8);
  m_PointingMetrologyLaser1RelML1[0] = T.GetTokenAtAsDouble(9);
  m_PointingMetrologyLaser1RelML1[1] = T.GetTokenAtAsDouble(10);
  m_PointingMetrologyLaser1RelML1[2] = T.GetTokenAtAsDouble(11);
  m_CentroidingErrorMetrologyDetector1 = T.GetTokenAtAsDouble(12);
  
  m_OriginMetrologyLaser2RelML2[0] = T.GetTokenAtAsDouble(13);
  m_OriginMetrologyLaser2RelML2[1] = T.GetTokenAtAsDouble(14);
  m_OriginMetrologyLaser2RelML2[2] = T.GetTokenAtAsDouble(15);
  m_PointingMetrologyLaser2RelML2[0] = T.GetTokenAtAsDouble(16);
  m_PointingMetrologyLaser2RelML2[1] = T.GetTokenAtAsDouble(17);
  m_PointingMetrologyLaser2RelML2[2] = T.GetTokenAtAsDouble(18);
  m_CentroidingErrorMetrologyDetector2 = T.GetTokenAtAsDouble(19);
  
  m_Empty = false;
  
  return true;
}


// NMetrologyDBEntry.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
