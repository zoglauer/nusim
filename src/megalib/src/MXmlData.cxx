/*
 * MXmlData.cxx
 *
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// MXmlData
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MXmlData.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MXmlData)
#endif


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData()
{
  // Construct an instance of MXmlData

  m_Name = "";
  m_Value = "";
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name)
{
  //! Constructor

  m_Name = Name;
  m_Value = "";
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name, TString Value)
{
  //! Constructor

  m_Name = Name;
  m_Value = Value;
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name, int Value)
{
  //! Constructor

  m_Name = Name;
  m_Value += TString::Format("%i", Value).Strip(TString::kBoth);
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name, unsigned int Value)
{
  //! Constructor

  m_Name = Name;
  m_Value += TString::Format("%u", Value).Strip(TString::kBoth);
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name, double Value)
{
  //! Constructor

  m_Name = Name;
  m_Value += TString::Format("%.8E", Value).Strip(TString::kBoth);
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::MXmlData(TString Name, bool Value)
{
  //! Constructor

  m_Name = Name;
  m_Value = ((Value == true) ? "true" : "false");
}


////////////////////////////////////////////////////////////////////////////////


MXmlData::~MXmlData()
{
}


////////////////////////////////////////////////////////////////////////////////


void MXmlData::Clear()
{
  // Reset the node

  m_Name = "";
  m_Value = "";
}


////////////////////////////////////////////////////////////////////////////////


bool MXmlData::GetValueAsBoolean() const
{
  //! Return the value of the node as boolean
  
  if (m_Value == "true" || m_Value == "TRUE") {
    return true;
  }

  return false;
}


// MXmlData.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
