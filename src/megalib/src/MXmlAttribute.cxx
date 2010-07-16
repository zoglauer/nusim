/*
 * MXmlAttribute.cxx
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
// MXmlAttribute
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MXmlAttribute.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"
#include "MXmlNode.h"

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MXmlAttribute)
#endif


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute() : MXmlData()
{
  // Construct an instance of MXmlAttribute
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name) : MXmlData(Name)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name, TString Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name, int Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name, unsigned int Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name, double Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::MXmlAttribute(MXmlNode* MotherNode, TString Name, bool Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddAttribute(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute::~MXmlAttribute()
{
  // Delete this instance of MXmlAttribute
}


////////////////////////////////////////////////////////////////////////////////


TString MXmlAttribute::ToString()
{
  //! Returns the XML text

  TString Xml;
  Xml += m_Name;
  Xml += "=\"";
  Xml += m_Value;
  Xml += "\"";

  return Xml;
}


// MXmlAttribute.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
