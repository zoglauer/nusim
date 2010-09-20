/*
 * NObservatoryData.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NObservatoryData
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NObservatoryData.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NObservatoryData)
#endif


////////////////////////////////////////////////////////////////////////////////


NObservatoryData::NObservatoryData()
{
  // Construct an instance of NObservatoryData

  m_Empty = false;
  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NObservatoryData::~NObservatoryData()
{
  // Delete this instance of NObservatoryData
}


////////////////////////////////////////////////////////////////////////////////


void NObservatoryData::Clear() 
{
  // Reset the data

  if (m_Empty == true) return;

  m_Time.Clear();

  m_DirectionOpticalAxisInIS.Set(g_VectorNotDefined);
  m_DirectionEventInIS.Set(g_VectorNotDefined);
  m_OrientationFocalPlaneToOB.Clear();
  m_OrientationOBToIS.Clear();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


double NObservatoryData::GetRa()
{
  //! Return the right ascension
  
  double Value = atan2(m_DirectionEventInIS[1], m_DirectionEventInIS[0])*c_Deg*60;
  if (Value < 0.0) Value += 360*60;
  return Value;
}


////////////////////////////////////////////////////////////////////////////////

double NObservatoryData::GetRaScaled()
{
  //! Return the right ascension with proper declination scaling
  
  double OARa = GetRaOA();
  double Dec = GetDec();
  
  double Value = atan2(m_DirectionEventInIS[1], m_DirectionEventInIS[0])*c_Deg*60;
  if (Value < 0.0) Value += 360*60;
  Value = OARa + (Value-OARa)*cos(Dec/60./c_Deg);
  return Value;
}


////////////////////////////////////////////////////////////////////////////////


double NObservatoryData::GetDecOA()
{
  //! Return the declination

  double Value = m_DirectionOpticalAxisInIS[0]*m_DirectionOpticalAxisInIS[0] + m_DirectionOpticalAxisInIS[1]*m_DirectionOpticalAxisInIS[1] + m_DirectionOpticalAxisInIS[2]*m_DirectionOpticalAxisInIS[2];
  if (Value <= 0) return 0.0;

  return asin(m_DirectionOpticalAxisInIS[2]/sqrt(Value))*c_Deg*60;
}


////////////////////////////////////////////////////////////////////////////////

double NObservatoryData::GetRaOA()
{
  //! Return the right ascension
  
  double Value = atan2(m_DirectionOpticalAxisInIS[1], m_DirectionOpticalAxisInIS[0])*c_Deg*60;
  if (Value < 0.0) Value += 360*60;
  
  return Value;
}


////////////////////////////////////////////////////////////////////////////////


double NObservatoryData::GetDec()
{
  //! Return the declination

  double Value = m_DirectionEventInIS[0]*m_DirectionEventInIS[0] + m_DirectionEventInIS[1]*m_DirectionEventInIS[1] + m_DirectionEventInIS[2]*m_DirectionEventInIS[2];
  if (Value <= 0) return 0.0;

  return asin(m_DirectionEventInIS[2]/sqrt(Value))*c_Deg*60;
}


////////////////////////////////////////////////////////////////////////////////

bool NObservatoryData::Stream(ofstream& S, bool Compact)
{
  //! Stream the content to an ASCII file 

  streamsize OriginalPrecision = S.precision();
  S.precision(9);
  
  if (Compact == false) {
    if (m_Empty == true) {
      S<<"OD -"<<endl;
    } else {
      S<<"OD "<<m_Time.GetSeconds()<<" "
       <<m_DirectionOpticalAxisInIS[0]<<" "<<m_DirectionOpticalAxisInIS[1]<<" "<<m_DirectionOpticalAxisInIS[2]<<" "
       <<m_DirectionEventInIS[0]<<" "<<m_DirectionEventInIS[1]<<" "<<m_DirectionEventInIS[2]<<" "
       <<m_OrientationFocalPlaneToOB.GetTranslation()[0]<<" "<<m_OrientationFocalPlaneToOB.GetTranslation()[1]<<" "<<m_OrientationFocalPlaneToOB.GetTranslation()[2]<<" "
       <<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[0]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[1]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[2]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[3]<<" "
       <<m_OrientationOBToIS.GetTranslation()[0]<<" "<<m_OrientationOBToIS.GetTranslation()[1]<<" "<<m_OrientationOBToIS.GetTranslation()[2]<<" "
       <<m_OrientationOBToIS.GetRotationQuaternion()[0]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[1]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[2]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[3]<<endl;
    }
  } else {
    if (m_Empty == true) {
      S<<"-"<<endl;
    } else {
      S<<m_Time.GetSeconds()<<" "
       <<m_DirectionOpticalAxisInIS[0]<<" "<<m_DirectionOpticalAxisInIS[1]<<" "<<m_DirectionOpticalAxisInIS[2]<<" "
       <<m_DirectionEventInIS[0]<<" "<<m_DirectionEventInIS[1]<<" "<<m_DirectionEventInIS[2]<<" "
       <<m_OrientationFocalPlaneToOB.GetTranslation()[0]<<" "<<m_OrientationFocalPlaneToOB.GetTranslation()[1]<<" "<<m_OrientationFocalPlaneToOB.GetTranslation()[2]<<" "
       <<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[0]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[1]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[2]<<" "<<m_OrientationFocalPlaneToOB.GetRotationQuaternion()[3]<<" "
       <<m_OrientationOBToIS.GetTranslation()[0]<<" "<<m_OrientationOBToIS.GetTranslation()[1]<<" "<<m_OrientationOBToIS.GetTranslation()[2]<<" "
       <<m_OrientationOBToIS.GetRotationQuaternion()[0]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[1]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[2]<<" "<<m_OrientationOBToIS.GetRotationQuaternion()[3]<<endl;
    }
  }

  S.precision(OriginalPrecision);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NObservatoryData::Parse(TString& Line, bool Compact)
{
  //! Stream the content from a line of an ASCII file  

  if ((Line.Length() == 4 && Line[4] == '-') || (Line.Length() == 1 && Line[1] == '-')) {
    Clear();
    return true;
  }

  double t, xO, yO, zO, xE, yE, zE, xF, yF, zF, rF, sF, tF, uF, xI, yI, zI, rI, sI, tI, uI;
  if (Compact == false) {
    if (sscanf(Line.Data(), 
               "OD %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
               &t, &xO, &yO, &zO, &xE, &yE, &zE, &xF, &yF, &zF, &rF, &sF, &tF, &uF, &xI, &yI, &zI, &rI, &sI, &tI, &uI) != 21) {
      merr<<"Unable to parse observatory data"<<show;
      return false;
    }
  } else {
    if (sscanf(Line.Data(), 
               "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
               &t, &xO, &yO, &zO, &xE, &yE, &zE, &xF, &yF, &zF, &rF, &sF, &tF, &uF, &xI, &yI, &zI, &rI, &sI, &tI, &uI) != 21) {
      merr<<"Unable to parse observatory data"<<show;
      return false;
    }    
  }
  
  m_Time.SetSeconds(t);
  m_DirectionOpticalAxisInIS.SetXYZ(xO, yO, zO);
  m_DirectionEventInIS.SetXYZ(xE, yE, zE);
  m_OrientationFocalPlaneToOB.SetTranslation(xF, yF, zF);
  m_OrientationFocalPlaneToOB.SetRotation(rF, sF, tF, uF);
  m_OrientationOBToIS.SetTranslation(xI, yI, zI);
  m_OrientationOBToIS.SetRotation(rI, sI, tI, uI);

  m_Empty = false;

  return true;
}


// NObservatoryData.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
