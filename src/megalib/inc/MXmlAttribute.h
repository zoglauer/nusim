/*
 * MXmlAttribute.h
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MXmlAttribute__
#define __MXmlAttribute__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cstdlib>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"
#include "MXmlData.h"

// Forward declarations:
class MXmlNode;


////////////////////////////////////////////////////////////////////////////////


class MXmlAttribute : public MXmlData
{
  // public interface:
 public:
  //! Default constructor
  MXmlAttribute();
  //! Constructor -- no values
  MXmlAttribute(MXmlNode* MotherNode, TString Name);
  //! Constructor -- content is string
  MXmlAttribute(MXmlNode* MotherNode, TString Name, TString Value);
  //! Constructor -- content is integer
  MXmlAttribute(MXmlNode* MotherNode, TString Name, int Value);
  //! Constructor -- content is unsigned integer
  MXmlAttribute(MXmlNode* MotherNode, TString Name, unsigned int Value);
  //! Constructor -- content is double
  MXmlAttribute(MXmlNode* MotherNode, TString Name, double Value);
  //! Constructor -- content is a boolean
  MXmlAttribute(MXmlNode* MotherNode, TString Name, bool Value);
  //! Default destructor
  virtual ~MXmlAttribute();

  //! Returns the XML text
  virtual TString ToString();


  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(MXmlAttribute, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
