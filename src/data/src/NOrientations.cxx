/*
 * NOrientations.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NOrientations
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NOrientations.h"

// Standard libs:
#include <cstdlib>
#include <sstream>
#include <iomanip>
using namespace std;

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NOrientations)
#endif


////////////////////////////////////////////////////////////////////////////////


NOrientations::NOrientations() : m_Empty(true)
{
  // Construct an instance of NOrientations
}


////////////////////////////////////////////////////////////////////////////////


NOrientations::~NOrientations()
{
  // Delete this instance of NOrientations
}


////////////////////////////////////////////////////////////////////////////////


void NOrientations::Clear()
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

  m_Time.Clear();
  
  m_Empty = true;
}
  

////////////////////////////////////////////////////////////////////////////////


NOrientation NOrientations::GetOrientationSpaceCraftRelInertial() 
{ 
  //! Get the space craft orientation relativ eto the inertial system

  return m_SpaceCraftRelInertial; 
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NOrientations::GetOrientationFocalPlaneRelSC() 
{ 
  //! Get the focal plane orientation relative to the space craft

  return m_FocalPlaneRelSC; 
}

////////////////////////////////////////////////////////////////////////////////


NOrientation NOrientations::GetOrientationOpticalBench() 
{ 
  // Get the optic bench orientation at time t                      

  return m_OpticalBench; 
}


////////////////////////////////////////////////////////////////////////////////


NOrientation NOrientations::GetOrientationOptics(int OpticsID) 
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


NOrientation NOrientations::GetOrientationOpticsRelOpticalBench(int OpticsID) 
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


NOrientation NOrientations::GetOrientationAperture(int ApertureID) 
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


NOrientation NOrientations::GetOrientationMetrologyLaser(int MetrologyID) 
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


NOrientation NOrientations::GetOrientationMetrologyLaserRelOpticalBench(int MetrologyID) 
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


NOrientation NOrientations::GetOrientationMetrologyDetector(int MetrologyID) 
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


NOrientation NOrientations::GetOrientationStarTracker(int StarTrackerID) 
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


NOrientation NOrientations::GetOrientationStarTrackerRelOpticalBench(int StarTrackerID) 
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


NOrientation NOrientations::GetOrientationFocalPlaneModule(int ModuleID) 
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


NOrientation NOrientations::GetOrientationDetectorRelFocalPlaneModule(int ModuleID, int DetectorID) 
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


bool NOrientations::Stream(ofstream& S)
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
  

bool NOrientations::Parse(TString& Line)
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
  


bool NOrientations::ParseDB(TString Positions, TString Rotations)
{
  // Parse some input from file
  double Time;
  double xSC = 0, ySC = 0, zSC = 0, tSC = 0;
  double xFP = 0, yFP = 0, zFP = 0, tFP = 0;
  double xFPM1 = 0, yFPM1 = 0, zFPM1 = 0, tFPM1 = 0;
  double xFPM2 = 0, yFPM2 = 0, zFPM2 = 0, tFPM2 = 0;
  double xMD1 = 0, yMD1 = 0, zMD1 = 0, tMD1 = 0;
  double xMD2 = 0, yMD2 = 0, zMD2 = 0, tMD2 = 0;
  double xA1 = 0, yA1 = 0, zA1 = 0, tA1 = 0;
  double xA2 = 0, yA2 = 0, zA2 = 0, tA2 = 0;
  double xOB = 0, yOB = 0, zOB = 0, tOB = 0;
  double xO1 = 0, yO1 = 0, zO1 = 0, tO1 = 0;
  double xO2 = 0, yO2 = 0, zO2 = 0, tO2 = 0;
  double xML1 = 0, yML1 = 0, zML1 = 0, tML1 = 0;
  double xML2 = 0, yML2 = 0, zML2 = 0, tML2 = 0;
  double xST4 = 0, yST4 = 0, zST4 = 0, tST4 = 0;
 
  int E = sscanf(Positions.Data(), "%lf,v,"
                 "%lf,%lf,%lf,," // m_SpaceCraftRelInertial
                 "%lf,%lf,%lf,," // m_FocalPlaneRelSC
                 "%lf,%lf,%lf,," // m_FocalPlaneModule1
                 "%lf,%lf,%lf,," // m_FocalPlaneModule2
                 "%lf,%lf,%lf,," // m_MetrologyDetector1
                 "%lf,%lf,%lf,," // m_MetrologyDetector2
                 "%lf,%lf,%lf,," // m_Aperture1
                 "%lf,%lf,%lf,," // m_Aperture2
                 "%lf,%lf,%lf,," // m_OpticalBench
                 "%lf,%lf,%lf,," // m_Optics1RelOB
                 "%lf,%lf,%lf,," // m_Optics2RelOB
                 "%lf,%lf,%lf,," // m_MetrologyLaser1RelOB
                 "%lf,%lf,%lf,," // m_MetrologyLaser2RelOB
                 "%lf,%lf,%lf,"  // m_StarTracker4RelOB
                 ,
                 &Time, 
								 &xSC, &ySC, &zSC,
								 &xFP, &yFP, &zFP,
                 &xFPM1, &yFPM1, &zFPM1,
                 &xFPM2, &yFPM2, &zFPM2,
                 &xMD1, &yMD1, &zMD1, 
                 &xMD2, &yMD2, &zMD2, 
                 &xA1, &yA1, &zA1, 
                 &xA2, &yA2, &zA2, 
                 &xOB, &yOB, &zOB, 
                 &xO1, &yO1, &zO1, 
                 &xO2, &yO2, &zO2, 
                 &xML1, &yML1, &zML1, 
                 &xML2, &yML2, &zML2, 
                 &xST4, &yST4, &zST4);
  if (E != 43) {
    cerr<<"Unable to scan orientation position. Scanned entries "<<E<<" != 43"<<endl;
    cerr<<Positions.Data()<<endl;
    //return false;
  }
  
  m_SpaceCraftRelInertial.SetTranslation(xSC, ySC, zSC);
  m_FocalPlaneRelSC.SetTranslation(xFP, yFP, zFP);
  m_FocalPlaneModule1.SetTranslation(xFPM1, yFPM1, zFPM1);
  m_FocalPlaneModule2.SetTranslation(xFPM2, yFPM2, zFPM2);
  m_MetrologyDetector1.SetTranslation(xMD1, yMD1, zMD1);
  m_MetrologyDetector2.SetTranslation(xMD2, yMD2, zMD2);
  m_Aperture1.SetTranslation(xA1, yA1, zA1);
  m_Aperture2.SetTranslation(xA2, yA2, zA2);
  m_OpticalBench.SetTranslation(xOB, yOB, zOB);
  m_Optics1RelOB.SetTranslation(xO1, yO1, zO1);
  m_Optics2RelOB.SetTranslation(xO2, yO2, zO2);
  m_MetrologyLaser1RelOB.SetTranslation(xML1, yML1, zML1);
  m_MetrologyLaser2RelOB.SetTranslation(xML2, yML2, zML2);
  m_StarTracker4RelOB.SetTranslation(xST4, yST4, zST4);


  E = sscanf(Rotations.Data(), "%lf,q,"
             "%lf,%lf,%lf,%lf," // m_SpaceCraftRelInertial
             "%lf,%lf,%lf,%lf," // m_FocalPlaneRelSC
             "%lf,%lf,%lf,%lf," // m_FocalPlaneModule1
             "%lf,%lf,%lf,%lf," // m_FocalPlaneModule2
             "%lf,%lf,%lf,%lf," // m_MetrologyDetector1
             "%lf,%lf,%lf,%lf," // m_MetrologyDetector2
             "%lf,%lf,%lf,%lf," // m_Aperture1
             "%lf,%lf,%lf,%lf," // m_Aperture2
             "%lf,%lf,%lf,%lf," // m_OpticalBench
             "%lf,%lf,%lf,%lf," // m_Optics1RelOB
             "%lf,%lf,%lf,%lf," // m_Optics2RelOB
             "%lf,%lf,%lf,%lf," // m_MetrologyLaser1RelOB
             "%lf,%lf,%lf,%lf," // m_MetrologyLaser2RelOB
             "%lf,%lf,%lf,%lf"  // m_StarTracker4RelOB
             , 
             &Time,
						 &xSC, &ySC, &zSC, &tSC,
						 &xFP, &yFP, &zFP, & tFP,
             &xFPM1, &yFPM1, &zFPM1, &tFPM1,
             &xFPM2, &yFPM2, &zFPM2, &tFPM2,
             &xMD1, &yMD1, &zMD1, &tMD1, 
             &xMD2, &yMD2, &zMD2, &tMD2,  
             &xA1, &yA1, &zA1, &tA1, 
             &xA2, &yA2, &zA2, &tA2,
             &xOB, &yOB, &zOB, &tOB, 
             &xO1, &yO1, &zO1, &tO1, 
             &xO2, &yO2, &zO2, &tO2, 
             &xML1, &yML1, &zML1, &tML1, 
             &xML2, &yML2, &zML2, &tML2, 
             &xST4, &yST4, &zST4, &tST4);
  if (E != 57) {
    cerr<<"Unable to scan orientation position. Scanned entries "<<E<<" != 57"<<endl;
    cerr<<Positions.Data()<<endl;
    //return false;
  }
           
  m_SpaceCraftRelInertial.SetRotation(xSC, ySC, zSC, tSC);
  m_FocalPlaneRelSC.SetRotation(xFP, yFP, zFP, tFP);
  m_FocalPlaneModule1.SetRotation(xFPM1, yFPM1, zFPM1, tFPM1);
  m_FocalPlaneModule2.SetRotation(xFPM2, yFPM2, zFPM2, tFPM2);
  m_MetrologyDetector1.SetRotation(xMD1, yMD1, zMD1, tMD1);
  m_MetrologyDetector2.SetRotation(xMD2, yMD2, zMD2, tMD2);
  m_Aperture1.SetRotation(xA1, yA1, zA1, tA1);
  m_Aperture2.SetRotation(xA2, yA2, zA2, tA2);
  m_OpticalBench.SetRotation(xOB, yOB, zOB, tOB);
  m_Optics1RelOB.SetRotation(xO1, yO1, zO1, tO1);
  m_Optics2RelOB.SetRotation(xO2, yO2, zO2, tO2);
  m_MetrologyLaser1RelOB.SetRotation(xML1, yML1, zML1, tML1);
  m_MetrologyLaser2RelOB.SetRotation(xML2, yML2, zML2, tML2);
  m_StarTracker4RelOB.SetRotation(xST4, yST4, zST4, tST4);

  m_Time.SetSeconds(Time);
  
  //! Those are currently !NOT! in the alignments DB  
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


TString NOrientations::ToString() const
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

// NOrientations.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
