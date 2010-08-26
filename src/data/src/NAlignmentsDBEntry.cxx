/*
 * NAlignmentsDBEntry.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NAlignmentsDBEntry
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NAlignmentsDBEntry.h"

// Standard libs:
#include <cstdlib>
#include <sstream>
#include <iomanip>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MTokenizer.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NAlignmentsDBEntry)
#endif


////////////////////////////////////////////////////////////////////////////////


NAlignmentsDBEntry::NAlignmentsDBEntry() : m_Empty(true)
{
  // Construct an instance of NAlignmentsDBEntry
}


////////////////////////////////////////////////////////////////////////////////


NAlignmentsDBEntry::~NAlignmentsDBEntry()
{
  // Delete this instance of NAlignmentsDBEntry
}


////////////////////////////////////////////////////////////////////////////////


void NAlignmentsDBEntry::Clear()
{
  //! Resets all data to default values
 
  m_SpaceCraftRelInertial.Clear();
  m_FocalPlaneRelSC.Clear();
  m_FocalPlaneModule1.Clear();
  m_FocalPlaneModule2.Clear();
  m_MetrologyDetector1.Clear();
  m_MetrologyDetector2.Clear();
  m_Aperture1.Clear();
  m_Aperture2.Clear();
  m_OpticalBench.Clear();
  m_Optics1RelOB.Clear();
  m_Optics2RelOB.Clear();
  m_MetrologyLaser1RelOB.Clear();
  m_MetrologyLaser2RelOB.Clear();
  m_StarTracker4RelOB.Clear();

  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.Clear();
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.Clear();
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.Clear();
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.Clear();

  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.Clear();
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.Clear();
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.Clear();
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.Clear();

  m_Time.Clear();
  
  m_Empty = true;
}
    

////////////////////////////////////////////////////////////////////////////////


void NAlignmentsDBEntry::SetInterpolated(const NAlignmentsDBEntry& A, const NAlignmentsDBEntry& B, double Fraction)
{
  //! Set from a linear interpolation between the two values
  //! Fraction needs to be between ]0..1[  
  
  
  m_Time = A.m_Time + (B.m_Time - A.m_Time)*Fraction;

  m_SpaceCraftRelInertial.SetInterpolated(A.m_SpaceCraftRelInertial, B.m_SpaceCraftRelInertial, Fraction);
  m_FocalPlaneRelSC.SetInterpolated(A.m_FocalPlaneRelSC, B.m_FocalPlaneRelSC, Fraction);
  m_FocalPlaneModule1.SetInterpolated(A.m_FocalPlaneModule1, B.m_FocalPlaneModule1, Fraction);
  m_FocalPlaneModule2.SetInterpolated(A.m_FocalPlaneModule2, B.m_FocalPlaneModule2, Fraction);
  m_MetrologyDetector1.SetInterpolated(A.m_MetrologyDetector1, B.m_MetrologyDetector1, Fraction);
  m_MetrologyDetector2.SetInterpolated(A.m_MetrologyDetector2, B.m_MetrologyDetector2, Fraction);
  m_Aperture1.SetInterpolated(A.m_Aperture1, B.m_Aperture1, Fraction);
  m_Aperture2.SetInterpolated(A.m_Aperture2, B.m_Aperture2, Fraction);
  m_OpticalBench.SetInterpolated(A.m_OpticalBench, B.m_OpticalBench, Fraction);
  m_Optics1RelOB.SetInterpolated(A.m_Optics1RelOB, B.m_Optics1RelOB, Fraction);
  m_Optics2RelOB.SetInterpolated(A.m_Optics2RelOB, B.m_Optics2RelOB, Fraction);
  m_MetrologyLaser1RelOB.SetInterpolated(A.m_MetrologyLaser1RelOB, B.m_MetrologyLaser1RelOB, Fraction);
  m_MetrologyLaser2RelOB.SetInterpolated(A.m_MetrologyLaser2RelOB, B.m_MetrologyLaser2RelOB, Fraction);
  m_StarTracker4RelOB.SetInterpolated(A.m_StarTracker4RelOB, B.m_StarTracker4RelOB, Fraction);

  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.SetInterpolated(A.m_FocalPlaneModule1Detector1RelFocalPlaneModule1, B.m_FocalPlaneModule1Detector1RelFocalPlaneModule1, Fraction);
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.SetInterpolated(A.m_FocalPlaneModule1Detector2RelFocalPlaneModule1, B.m_FocalPlaneModule1Detector2RelFocalPlaneModule1, Fraction);
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.SetInterpolated(A.m_FocalPlaneModule1Detector3RelFocalPlaneModule1, B.m_FocalPlaneModule1Detector3RelFocalPlaneModule1, Fraction);
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.SetInterpolated(A.m_FocalPlaneModule1Detector4RelFocalPlaneModule1, B.m_FocalPlaneModule1Detector4RelFocalPlaneModule1, Fraction);

  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.SetInterpolated(A.m_FocalPlaneModule2Detector1RelFocalPlaneModule2, B.m_FocalPlaneModule2Detector1RelFocalPlaneModule2, Fraction);
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.SetInterpolated(A.m_FocalPlaneModule2Detector2RelFocalPlaneModule2, B.m_FocalPlaneModule2Detector2RelFocalPlaneModule2, Fraction);
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.SetInterpolated(A.m_FocalPlaneModule2Detector3RelFocalPlaneModule2, B.m_FocalPlaneModule2Detector3RelFocalPlaneModule2, Fraction);
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.SetInterpolated(A.m_FocalPlaneModule2Detector4RelFocalPlaneModule2, B.m_FocalPlaneModule2Detector4RelFocalPlaneModule2, Fraction);
   
  m_Empty = false;
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationSpaceCraftRelInertial() 
{ 
  //! Get the space craft orientation relativ eto the inertial system

  return m_SpaceCraftRelInertial; 
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationFocalPlaneRelSC() 
{ 
  //! Get the focal plane orientation relative to the space craft

  return m_FocalPlaneRelSC; 
}

////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationOpticalBench() 
{ 
  // Get the optic bench orientation at time t                      

  return m_OpticalBench; 
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationOptics(int OpticsID) 
{
  // Get the optics 1 orientation at time t                      
   
  if (OpticsID == 1) {
    return m_OpticalBench*m_Optics1RelOB;
  } else if (OpticsID == 2) {
    return m_OpticalBench*m_Optics2RelOB;
  } else {
    cout<<"SEVERE ERROR: Optics ID = "<<OpticsID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationOpticsRelOpticalBench(int OpticsID) 
{ 
  // Get the optics orientation realtive to the optical bench at time t                      
  
  if (OpticsID == 1) {
    return m_Optics1RelOB;
  } else if (OpticsID == 2) {
    return m_Optics2RelOB;
  } else {
    cout<<"SEVERE ERROR: Optics ID = "<<OpticsID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationAperture(int ApertureID) 
{ 
  // Get the aperture orientation at time t

  if (ApertureID == 1) {
    return m_Aperture1; 
  } else if (ApertureID == 2) {
    return m_Aperture2; 
  } else {
    cout<<"SEVERE ERROR: Aperture ID = "<<ApertureID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationMetrologyLaser(int MetrologyID) 
{ 
  // Get the metrology orientation at time t

  if (MetrologyID == 1) {
    return m_OpticalBench*m_MetrologyLaser1RelOB; 
  } else if (MetrologyID == 2) {
    return m_OpticalBench*m_MetrologyLaser2RelOB; 
  } else if (MetrologyID == 3) {
    return m_OpticalBench*m_MetrologyLaser3RelOB; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1, 2 or 3."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationMetrologyLaserRelOpticalBench(int MetrologyID) 
{ 
  // Get the metrology orientation realtive to the optical bench at time t

  if (MetrologyID == 1) {
    return m_MetrologyLaser1RelOB; 
  } else if (MetrologyID == 2) {
    return m_MetrologyLaser2RelOB; 
  } else if (MetrologyID == 3) {
    return m_MetrologyLaser3RelOB; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1, 2 or 3."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationMetrologyDetector(int MetrologyID) 
{ 
  // Get the metrology orientation at time t

  if (MetrologyID == 1) {
    return m_MetrologyDetector1; 
  } else if (MetrologyID == 2) {
    return m_MetrologyDetector2; 
  } else if (MetrologyID == 3) {
    return m_MetrologyDetector3; 
  } else {
    cout<<"SEVERE ERROR: Metrology ID = "<<MetrologyID<<" not allowed. Must be 1, 2 or 3."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationStarTracker(int StarTrackerID) 
{ 
  //! Get the star tracker orientation at time t

  if (StarTrackerID == 4) {
    return m_OpticalBench*m_StarTracker4RelOB;
  } else {
    cout<<"SEVERE ERROR: Star tracker ID = "<<StarTrackerID<<" not allowed. Must be 4."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationStarTrackerRelOpticalBench(int StarTrackerID) 
{ 
  //! Get the star tracker orientation realtive to the optical bench at time t

  if (StarTrackerID == 4) {
    return m_StarTracker4RelOB;
  } else {
    cout<<"SEVERE ERROR: Star tracker ID = "<<StarTrackerID<<" not allowed. Must be 4."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationFocalPlaneModule(int ModuleID) 
{ 
  //! Get the focal plane detector module  orientation at time t

  if (ModuleID == 1) {
    return m_FocalPlaneModule1;
  } else if (ModuleID == 2) {
    return m_FocalPlaneModule2;
  } else {
    cout<<"SEVERE ERROR: Module ID = "<<ModuleID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    abort();
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NAlignmentsDBEntry::GetOrientationDetectorRelFocalPlaneModule(int ModuleID, int DetectorID) 
{ 
  //! Get the focal plane detector module - detector orientation at time t

  if (ModuleID == 1) {
    if (DetectorID == 1) {
      return m_FocalPlaneModule1Detector1RelFocalPlaneModule1;
    } else if (DetectorID == 2) {
      return m_FocalPlaneModule1Detector2RelFocalPlaneModule1;
    } else if (DetectorID == 3) {
      return m_FocalPlaneModule1Detector3RelFocalPlaneModule1;
    } else if (DetectorID == 4) {
      return m_FocalPlaneModule1Detector4RelFocalPlaneModule1;
    } else {
      cout<<"SEVERE ERROR: Detector ID = "<<DetectorID<<" not allowed. Must be 1, 2, 3, or 4."<<endl;
      cout<<"The results for this event will be wrong!"<<endl;
      return NOrientation();
    }
  } else if (ModuleID == 2) {
    if (DetectorID == 1) {
      return m_FocalPlaneModule2Detector1RelFocalPlaneModule2;
    } else if (DetectorID == 2) {
      return m_FocalPlaneModule2Detector2RelFocalPlaneModule2;
    } else if (DetectorID == 3) {
      return m_FocalPlaneModule2Detector3RelFocalPlaneModule2;
    } else if (DetectorID == 4) {
      return m_FocalPlaneModule2Detector4RelFocalPlaneModule2;
    } else {
      cout<<"SEVERE ERROR: Detector ID = "<<DetectorID<<" not allowed. Must be 1, 2, 3, or 4."<<endl;
      cout<<"The results for this event will be wrong!"<<endl;
      return NOrientation();
    }
  } else {
    cout<<"SEVERE ERROR: Module ID = "<<ModuleID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    abort();
    return NOrientation();
  }
}


////////////////////////////////////////////////////////////////////////////////


bool NAlignmentsDBEntry::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  // We require a higher precision here...
  streamsize p = S.precision();
  S.precision(12);

  S<<"OR ";
  m_SpaceCraftRelInertial.Stream(S);
  m_FocalPlaneRelSC.Stream(S);
  m_FocalPlaneModule1.Stream(S);
  m_FocalPlaneModule2.Stream(S);
  m_MetrologyDetector1.Stream(S);
  m_MetrologyDetector2.Stream(S);
  m_Aperture1.Stream(S);
  m_Aperture2.Stream(S);
  m_OpticalBench.Stream(S);
  m_Optics1RelOB.Stream(S);
  m_Optics2RelOB.Stream(S);
  m_MetrologyLaser1RelOB.Stream(S);
  m_MetrologyLaser2RelOB.Stream(S);
  m_StarTracker4RelOB.Stream(S);
  S<<endl;

  S.precision(p);
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  

bool NAlignmentsDBEntry::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  
  
  int StartToken = 1;
  if (m_SpaceCraftRelInertial.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_FocalPlaneRelSC.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_FocalPlaneModule1.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_FocalPlaneModule2.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_MetrologyDetector1.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_MetrologyDetector2.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_Aperture1.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_Aperture2.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_OpticalBench.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_Optics1RelOB.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_Optics2RelOB.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_MetrologyLaser1RelOB.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_MetrologyLaser2RelOB.Parse(Line, StartToken) == false) return false;
  StartToken += 7;
  if (m_StarTracker4RelOB.Parse(Line, StartToken) == false) return false;
  
  m_Empty = false;
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////
  


bool NAlignmentsDBEntry::ParseDB(TString Positions, TString Rotations)
{
  
  MTokenizer T;
  T.AllowComposed(false);
  T.AllowEmpty(true);
  T.SetSeparator(',');
  T.Analyze(Positions, false);
        
  if (T.GetNTokens() != 58) {
    mgui<<"Not enough tokens in string for alignments DB entry ("<<T.GetNTokens()<<" vs. "<<58<<")"<<show;
    return false;
  }

  m_Time.SetSeconds(T.GetTokenAtAsDouble(0));
  
  m_SpaceCraftRelInertial.SetTranslation(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4));
  m_FocalPlaneRelSC.SetTranslation(T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7), T.GetTokenAtAsDouble(8));
  m_FocalPlaneModule1.SetTranslation(T.GetTokenAtAsDouble(10), T.GetTokenAtAsDouble(11), T.GetTokenAtAsDouble(12));
  m_FocalPlaneModule2.SetTranslation(T.GetTokenAtAsDouble(14), T.GetTokenAtAsDouble(15), T.GetTokenAtAsDouble(16));
  m_MetrologyDetector1.SetTranslation(T.GetTokenAtAsDouble(18), T.GetTokenAtAsDouble(19), T.GetTokenAtAsDouble(20));
  m_MetrologyDetector2.SetTranslation(T.GetTokenAtAsDouble(22), T.GetTokenAtAsDouble(23), T.GetTokenAtAsDouble(24));
  m_Aperture1.SetTranslation(T.GetTokenAtAsDouble(26), T.GetTokenAtAsDouble(27), T.GetTokenAtAsDouble(28));
  m_Aperture2.SetTranslation(T.GetTokenAtAsDouble(30), T.GetTokenAtAsDouble(31), T.GetTokenAtAsDouble(32));
  m_OpticalBench.SetTranslation(T.GetTokenAtAsDouble(34), T.GetTokenAtAsDouble(35), T.GetTokenAtAsDouble(36));
  m_Optics1RelOB.SetTranslation(T.GetTokenAtAsDouble(38), T.GetTokenAtAsDouble(39), T.GetTokenAtAsDouble(40));
  m_Optics2RelOB.SetTranslation(T.GetTokenAtAsDouble(42), T.GetTokenAtAsDouble(43), T.GetTokenAtAsDouble(44));
  m_MetrologyLaser1RelOB.SetTranslation(T.GetTokenAtAsDouble(46), T.GetTokenAtAsDouble(47), T.GetTokenAtAsDouble(48));
  m_MetrologyLaser2RelOB.SetTranslation(T.GetTokenAtAsDouble(50), T.GetTokenAtAsDouble(51), T.GetTokenAtAsDouble(52));
  m_StarTracker4RelOB.SetTranslation(T.GetTokenAtAsDouble(54), T.GetTokenAtAsDouble(55), T.GetTokenAtAsDouble(56));

  
  T.Analyze(Rotations, false);
  
  if (T.GetNTokens() != 58) {
    mgui<<"Not enough tokens in string for alignments DB entry ("<<T.GetNTokens()<<" vs. "<<58<<")"<<show;
    return false;
  }
  
  m_SpaceCraftRelInertial.SetRotation(T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3), T.GetTokenAtAsDouble(4), T.GetTokenAtAsDouble(5));
  m_FocalPlaneRelSC.SetRotation(T.GetTokenAtAsDouble(6), T.GetTokenAtAsDouble(7), T.GetTokenAtAsDouble(8), T.GetTokenAtAsDouble(9));
  m_FocalPlaneModule1.SetRotation(T.GetTokenAtAsDouble(10), T.GetTokenAtAsDouble(11), T.GetTokenAtAsDouble(12), T.GetTokenAtAsDouble(13));
  m_FocalPlaneModule2.SetRotation(T.GetTokenAtAsDouble(14), T.GetTokenAtAsDouble(15), T.GetTokenAtAsDouble(16), T.GetTokenAtAsDouble(17));
  m_MetrologyDetector1.SetRotation(T.GetTokenAtAsDouble(18), T.GetTokenAtAsDouble(19), T.GetTokenAtAsDouble(20), T.GetTokenAtAsDouble(21));
  m_MetrologyDetector2.SetRotation(T.GetTokenAtAsDouble(22), T.GetTokenAtAsDouble(23), T.GetTokenAtAsDouble(24), T.GetTokenAtAsDouble(25));
  m_Aperture1.SetRotation(T.GetTokenAtAsDouble(26), T.GetTokenAtAsDouble(27), T.GetTokenAtAsDouble(28), T.GetTokenAtAsDouble(29));
  m_Aperture2.SetRotation(T.GetTokenAtAsDouble(30), T.GetTokenAtAsDouble(31), T.GetTokenAtAsDouble(32), T.GetTokenAtAsDouble(33));
  m_OpticalBench.SetRotation(T.GetTokenAtAsDouble(34), T.GetTokenAtAsDouble(35), T.GetTokenAtAsDouble(36), T.GetTokenAtAsDouble(37));
  m_Optics1RelOB.SetRotation(T.GetTokenAtAsDouble(38), T.GetTokenAtAsDouble(39), T.GetTokenAtAsDouble(40), T.GetTokenAtAsDouble(41));
  m_Optics2RelOB.SetRotation(T.GetTokenAtAsDouble(42), T.GetTokenAtAsDouble(43), T.GetTokenAtAsDouble(44), T.GetTokenAtAsDouble(45));
  m_MetrologyLaser1RelOB.SetRotation(T.GetTokenAtAsDouble(46), T.GetTokenAtAsDouble(47), T.GetTokenAtAsDouble(48), T.GetTokenAtAsDouble(49));
  m_MetrologyLaser2RelOB.SetRotation(T.GetTokenAtAsDouble(50), T.GetTokenAtAsDouble(51), T.GetTokenAtAsDouble(52), T.GetTokenAtAsDouble(53));
  m_StarTracker4RelOB.SetRotation(T.GetTokenAtAsDouble(54), T.GetTokenAtAsDouble(55), T.GetTokenAtAsDouble(56), T.GetTokenAtAsDouble(57));

  
  //! Those are missing in the DB:
  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.SetTranslation(-10.25, -10.25, 0.0);
  m_FocalPlaneModule1Detector1RelFocalPlaneModule1.SetRotation(0.0, 0.0, 90.0*c_Rad);
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.SetTranslation(-10.25, +10.25, 0.0);
  m_FocalPlaneModule1Detector2RelFocalPlaneModule1.SetRotation(0.0, 0.0, 180.0*c_Rad);
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.SetTranslation(+10.25, -10.25, 0.0);
  m_FocalPlaneModule1Detector3RelFocalPlaneModule1.SetRotation(0.0, 0.0, 270.0*c_Rad);
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.SetTranslation(+10.25, +10.25, 0.0);
  m_FocalPlaneModule1Detector4RelFocalPlaneModule1.SetRotation(0.0, 0.0, 0.0);

  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.SetTranslation(-10.25, -10.25, 0.0);
  m_FocalPlaneModule2Detector1RelFocalPlaneModule2.SetRotation(0.0, 0.0, 90.0*c_Rad);
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.SetTranslation(-10.25, +10.25, 0.0);
  m_FocalPlaneModule2Detector2RelFocalPlaneModule2.SetRotation(0.0, 0.0, 180.0*c_Rad);
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.SetTranslation(+10.25, -10.25, 0.0);
  m_FocalPlaneModule2Detector3RelFocalPlaneModule2.SetRotation(0.0, 0.0, 270.0*c_Rad);
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.SetTranslation(+10.25, +10.25, 0.0);
  m_FocalPlaneModule2Detector4RelFocalPlaneModule2.SetRotation(0.0, 0.0, 0.0);

  m_Empty = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString NAlignmentsDBEntry::ToString() const
{
  //! Return the content as text
  
  ostringstream out;
  
  out<<"Orientations at t="<<m_Time.GetSeconds()<<" sec:"<<endl;
  out<<"Focal plane module 1:"<<endl;
  out<<m_FocalPlaneModule1.ToString()<<endl;
  out<<"Focal plane module 2:"<<endl;
  out<<m_FocalPlaneModule2.ToString()<<endl;
  out<<"Metrology detector 1:"<<endl;
  out<<m_MetrologyDetector1.ToString()<<endl;
  out<<"Metrology detector 2:"<<endl;
  out<<m_MetrologyDetector2.ToString()<<endl;
  out<<"Aperture 1:"<<endl;
  out<<m_Aperture1.ToString()<<endl;
  out<<"Aperture 2:"<<endl;
  out<<m_Aperture2.ToString()<<endl;
  out<<"Optical bench:"<<endl;
  out<<m_OpticalBench.ToString()<<endl;
  out<<"Optics 1 rel OB:"<<endl;
  out<<m_Optics1RelOB.ToString()<<endl;
  out<<"Optics 2 rel OB:"<<endl;
  out<<m_Optics2RelOB.ToString()<<endl;
  out<<"Metrology laser 1 rel OB:"<<endl;
  out<<m_MetrologyLaser1RelOB.ToString()<<endl;
  out<<"Metrology laser 2 rel OB:"<<endl;
  out<<m_MetrologyLaser2RelOB.ToString()<<endl;
  out<<"Star tracker 4 rel OB:"<<endl;
  out<<m_StarTracker4RelOB.ToString()<<endl;

  return out.str().c_str();
}

// NAlignmentsDBEntry.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
