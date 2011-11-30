/*
 * NInteraction.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NInteraction
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NInteraction.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NInteraction)
#endif


////////////////////////////////////////////////////////////////////////////////


NInteraction::NInteraction()
{
  // Construct an instance of NInteraction

  Clear();
}


////////////////////////////////////////////////////////////////////////////////


NInteraction::~NInteraction()
{
  // Delete this instance of NInteraction
}


////////////////////////////////////////////////////////////////////////////////


void NInteraction::Clear()
{
  // Reset all data

  m_IsDetector = true;

  m_Telescope = g_UnsignedIntNotDefined;
  m_Detector = g_UnsignedIntNotDefined;

  m_Position.Set(g_VectorNotDefined);
  m_Energy = g_DoubleNotDefined;

  m_Empty = true;
}


////////////////////////////////////////////////////////////////////////////////


bool NInteraction::Stream(ofstream& S, int Version)
{
  //! Stream the content to an ASCII file 

  S<<"IA ";
  if (m_Empty == true) {
    S<<"-"<<endl;
  } else {
    if (m_IsDetector == true) {
      S<<"d "<<m_Telescope<<" "<<m_Detector<<" "<<m_Position[0]<<" "<<m_Position[1]<<" "<<m_Position[2]<<" "<<m_Energy<<endl;
    } else {
      S<<"s "<<m_Telescope<<" "<<m_Energy<<endl;
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NInteraction::Parse(TString& Line, int Version)
{
  //! Stream the content from a line of an ASCII file  

  if (Line == "IA -") {
    Clear();
    return true;
  } else if (Line.BeginsWith("IA d") == true) {
    m_IsDetector = true;
    double x, y, z;
    if (sscanf(Line.Data(), "IA d %d %d %lf %lf %lf %lf", 
               &m_Telescope, &m_Detector,
               &x, &y, &z,
               &m_Energy) != 6) {
      return false;
    }
    m_Position.SetXYZ(x, y, z);
  } else if (Line.BeginsWith("IA s") == true) {
    m_IsDetector = false;
    if (sscanf(Line.Data(), "IA s %d %lf", 
               &m_Telescope, &m_Energy) != 2) {
      return false;
    }
  
  } else {
    return false;
  }

  m_Empty = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString NInteraction::ToString() const
{
  //! Dump the content to a string

  ostringstream out;

  if (m_Empty == false) {
    if (m_IsDetector == true) {
      out<<"Interaction in telescope "<<m_Telescope<<" in detector "<<m_Detector<<" at position "<<m_Position<<" mm depositing "<<m_Energy<<" keV energy."; 
    } else {
      out<<"Interaction in telescope "<<m_Telescope<<" in the shield depositing "<<m_Energy<<" keV energy."; 
    }
  } else {
    out<<"Non-initialized interaction object."<<endl;
  }
  
  return out.str().c_str();
}


// NInteraction.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
