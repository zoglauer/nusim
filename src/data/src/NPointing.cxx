/*
 * NPointing.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPointing
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPointing.h"

// Standard libs:

// ROOT libs:
#include "NQuaternion.h"
#include "TVector3.h"
// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPointing)
#endif


////////////////////////////////////////////////////////////////////////////////


NPointing::NPointing()
{
  // Construct an instance of NPointing

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPointing::~NPointing()
{
  // Delete this instance of NPointing
}


////////////////////////////////////////////////////////////////////////////////


void NPointing::Clear() 
{
  // Reset the pointing data to default values 

  m_Ra = 0.0;
  m_Dec = 0.0;
  RaDecToQuaternion();

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


void NPointing::QuaternionToRaDec()
{
  // Euler angles in radians. Move in Ra first then Dec.
 
  m_Ra = 60 * c_Deg * atan(m_Q.m_V[2]/m_Q.m_R)*2.;
  m_Dec = 60 * c_Deg * atan(m_Q.m_V[1]/m_Q.m_R)*2.; 
 }


////////////////////////////////////////////////////////////////////////////////


void NPointing::RaDecToQuaternion()
{
   // Ra, Dec are in radians. Euler angles.

   double cr = cos(m_Ra/60*c_Rad/2.);
   double cd = cos((c_Pi/2.-m_Dec/60*c_Rad)/2.);
   double sr = sin(m_Ra/60*c_Rad/2.);
   double sd = sin((c_Pi/2.-m_Dec/60*c_Rad)/2.);

   m_Q.m_R = cr*cd;
   m_Q.m_V[0] = -sr*sd;
   m_Q.m_V[1] = cr*sd; 
   m_Q.m_V[2] = sr*cd;
   
}


////////////////////////////////////////////////////////////////////////////////


bool NPointing::Stream(ofstream& S, TString Keyword)
{
  //! Stream the content to an ASCII file 

  S<<Keyword<<" ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Ra<<" "<<m_Dec<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPointing::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line.Length() == 4 && Line[4] == '-') {
    Clear();
    return true;
  }

  char Key[2];
  if (sscanf(Line.Data(), "%s %lf %lf", Key, &m_Ra, &m_Dec) != 3) {
    Clear();
    return false;
  }
  
  RaDecToQuaternion();

  m_Empty = false;

  return true;
}



// NPointing.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
