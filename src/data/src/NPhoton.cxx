/*
 * NPhoton.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NPhoton
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NPhoton.h"

// Standard libs:
#include <cstdio>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NPhoton)
#endif


////////////////////////////////////////////////////////////////////////////////


NPhoton::NPhoton()
{
  // Construct an instance of NPhoton

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NPhoton::~NPhoton()
{
  // Delete this instance of NPhoton
}


////////////////////////////////////////////////////////////////////////////////


void NPhoton::Clear() 
{
  // Reset the photon data to "NotDefined" values (polarization is zero)

  m_Position.Set(g_VectorNotDefined);
  m_Direction.Set(g_VectorNotDefined);
  m_Polarization.Clear();
  m_Energy = g_DoubleNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPhoton::Stream(ofstream& S, TString Keyword)
{
  //! Stream the content to an ASCII file 

  S<<Keyword<<" ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
   S<<m_Position[0]<<" "<<m_Position[1]<<" "<<m_Position[2]<<" "
    <<m_Direction[0]<<" "<<m_Direction[1]<<" "<<m_Direction[2]<<" "
    <<m_Polarization[0]<<" "<<m_Polarization[1]<<" "<<m_Polarization[2]<<" "
    <<m_Energy<<endl;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NPhoton::Parse(const TString& Line)
{
  //! Stream the content from a line of an ASCII file  

  if (Line.Length() == 4 && Line[4] == '-') {
    Clear();
    return true;
  }

  char Key[2];
  double xP, yP, zP, xD, yD, zD, xO, yO, zO;
  if (sscanf(Line.Data(), 
             "%s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
             Key, &xP, &yP, &zP, &xD, &yD, &zD, &xO, &yO, &zO,
             &m_Energy) != 11) {
    return false;
  }
  m_Position.SetXYZ(xP, yP, zP);
  m_Direction.SetXYZ(xD, yD, zD);
  m_Polarization.SetXYZ(xO, yO, zO);

  m_Empty = false;

  return true;
}

// NPhoton.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
