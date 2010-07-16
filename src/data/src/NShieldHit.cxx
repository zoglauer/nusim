/*
 * NShieldHit.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *

 */


////////////////////////////////////////////////////////////////////////////////
//
// NShieldHit
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NShieldHit.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NShieldHit)
#endif


////////////////////////////////////////////////////////////////////////////////


NShieldHit::NShieldHit()
{
  // Construct an instance of NShieldHit

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NShieldHit::~NShieldHit()
{
  // Delete this instance of NShieldHit
}


////////////////////////////////////////////////////////////////////////////////


void NShieldHit::Clear()
{
  // Reset all data

  m_IdealEnergy = g_DoubleNotDefined;
  m_NoisedEnergy = g_DoubleNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NShieldHit::Stream(ofstream& S)
{
  //! Stream the content to an ASCII file 

  S<<"SH ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    S<<m_Telescope<<" "<<m_IdealEnergy<<" "<<m_NoisedEnergy<<endl;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NShieldHit::Parse(TString& Line)
{
  //! Stream the content from a line of an ASCII file

  if (Line == "SH -") {
    Clear();
    return true;
  }

  if (sscanf(Line.Data(), 
             "SH %d %lf %lf", 
             &m_Telescope, &m_IdealEnergy, & m_NoisedEnergy) != 3) {
    return false;
  }

  m_Empty = false;
  
  return true;
}



// NShieldHit.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
