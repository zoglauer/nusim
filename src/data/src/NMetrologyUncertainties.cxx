/*
 * NMetrologyUncertainties.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NMetrologyUncertainties
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NMetrologyUncertainties.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NMetrologyUncertainties)
#endif


////////////////////////////////////////////////////////////////////////////////


NMetrologyUncertainties::NMetrologyUncertainties()
{
  // Construct an instance of NMetrologyUncertainties

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NMetrologyUncertainties::~NMetrologyUncertainties()
{
  // Delete this instance of NMetrologyUncertainties
}


////////////////////////////////////////////////////////////////////////////////


void NMetrologyUncertainties::Clear()
{
  //! Resets all data to default values
 
  m_PointingErrorStarTracker4 = g_DoubleNotDefined;
  
  m_PointingMetrologyLaser1RelML1 = g_VectorNotDefined;
  m_PointingMetrologyLaser2RelML2 = g_VectorNotDefined;

  m_CentroidingErrorMetrologyDetector1 = g_DoubleNotDefined;
  m_CentroidingErrorMetrologyDetector2 = g_DoubleNotDefined;

  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


void NMetrologyUncertainties::SetInterpolated(const NMetrologyUncertainties& A, const NMetrologyUncertainties& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[

  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;
  m_PointingErrorStarTracker4 = A.m_PointingErrorStarTracker4 + Fraction*(B.m_PointingErrorStarTracker4 - A.m_PointingErrorStarTracker4);
   
  m_PointingMetrologyLaser1RelML1 = A.m_PointingMetrologyLaser1RelML1 + Fraction*(B.m_PointingMetrologyLaser1RelML1 - A.m_PointingMetrologyLaser1RelML1);
  m_PointingMetrologyLaser2RelML2 = A.m_PointingMetrologyLaser2RelML2 + Fraction*(B.m_PointingMetrologyLaser2RelML2 - A.m_PointingMetrologyLaser2RelML2);
  
  m_CentroidingErrorMetrologyDetector1 = A.m_CentroidingErrorMetrologyDetector1 + Fraction*(B.m_CentroidingErrorMetrologyDetector1 - A.m_CentroidingErrorMetrologyDetector1);
  m_CentroidingErrorMetrologyDetector2 = A.m_CentroidingErrorMetrologyDetector2 + Fraction*(B.m_CentroidingErrorMetrologyDetector2 - A.m_CentroidingErrorMetrologyDetector2);

  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


MVector NMetrologyUncertainties::GetPointingMetrologyLaserRelML(int MetrologyID)
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


double NMetrologyUncertainties::GetCentroidingErrorMetrologyDetector(int MetrologyID)
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


double NMetrologyUncertainties::GetPointingErrorStarTracker(int StarTrackerID)
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


bool NMetrologyUncertainties::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  // We require a higher precision here...
  streamsize p = S.precision();
  S.precision(12);

  S<<"MU ";
  S<<m_Time.GetSeconds()<<" ";
  S<<m_PointingErrorStarTracker4<<" ";
  S<<m_PointingMetrologyLaser1RelML1[0]<<" "<<m_PointingMetrologyLaser1RelML1[1]<<" "<<m_PointingMetrologyLaser1RelML1[2]<<" ";
  S<<m_PointingMetrologyLaser2RelML2[0]<<" "<<m_PointingMetrologyLaser2RelML2[1]<<" "<<m_PointingMetrologyLaser2RelML2[2]<<" ";
  S<<m_CentroidingErrorMetrologyDetector1<<" ";
  S<<m_CentroidingErrorMetrologyDetector2<<" ";
  S<<endl;

  S.precision(p);
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////
  

TString NMetrologyUncertainties::ToString() const
{
 //! Return the content as text
 
 TString Text;
 Text += "MU: ";
 Text += "Time: ";
 Text += m_Time.GetSeconds();
 Text += ", Pointing error: ";
 Text += m_PointingErrorStarTracker4;
 Text += ", Pointing ML1 REL ML1: ";
 Text += m_PointingMetrologyLaser1RelML1.ToString();
 Text += ", Centroiding error MD1: ";
 Text += m_CentroidingErrorMetrologyDetector1;
 Text += ", Pointing ML2 REL ML2: ";
 Text += m_PointingMetrologyLaser2RelML2.ToString();
 Text += ", Centroiding error MD2: ";
 Text += m_CentroidingErrorMetrologyDetector2;
 
 return Text;
}

////////////////////////////////////////////////////////////////////////////////
  

bool NMetrologyUncertainties::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  
  
  MTokenizer T;
  T.Analyze(Line);
  
  if (T.GetNTokens() != 11) {
    merr<<"Not enough tokens in string for parsing!"<<show;
  }
  
  m_Time.SetSeconds(T.GetTokenAtAsDouble(1));
  m_PointingErrorStarTracker4 = T.GetTokenAtAsDouble(2);
  m_PointingMetrologyLaser1RelML1.SetXYZ(T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4), T.GetTokenAtAsDouble(5));
  m_PointingMetrologyLaser2RelML2.SetXYZ(T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7), T.GetTokenAtAsDouble(8));
  m_CentroidingErrorMetrologyDetector1 = T.GetTokenAtAsDouble(9);
  m_CentroidingErrorMetrologyDetector2 = T.GetTokenAtAsDouble(10);

  m_Empty = false;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NMetrologyUncertainties::ParseDB(TString Line)
{
  // Parse some input from file - fast!

  double Time;
 
  int E = sscanf(Line.Data(), 
                 "%lf,," // m_Time
                 "%lf,,,," // m_PointingErrorStarTracker4
                 "%lf,%lf,%lf," // m_PointingMetrologyLaser1RelML1
                 "%lf," // m_CentroidingErrorMetrologyDetector1
                 "%lf,%lf,%lf," // m_PointingMetrologyLaser1RelML2
                 "%lf" // m_CentroidingErrorMetrologyDetector2
                 ,
                 &Time, 
                 &m_PointingErrorStarTracker4, 
                 &m_PointingMetrologyLaser1RelML1[0], &m_PointingMetrologyLaser1RelML1[1], &m_PointingMetrologyLaser1RelML1[2],
                 &m_CentroidingErrorMetrologyDetector1,
                 &m_PointingMetrologyLaser2RelML2[0], &m_PointingMetrologyLaser2RelML2[1], &m_PointingMetrologyLaser2RelML2[2],
                 &m_CentroidingErrorMetrologyDetector2);
  if (E != 10) {
    cerr<<"Unable to scan metrology uncertainties. Scanned entries "<<E<<" != 10"<<endl;
    cerr<<"Content of line: "<<endl;
    cerr<<Line.Data()<<endl;
    Clear();
    return false;
  }
  
  m_Time.SetSeconds(Time);
  
  m_Empty = false;

  return true;
}


// NMetrologyUncertainties.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
