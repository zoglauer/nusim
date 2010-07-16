/*
 * MXmlDocument.h
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MXmlDocument__
#define __MXmlDocument__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"
#include "MXmlNode.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MXmlDocument : public MXmlNode
{
  // public interface:
 public:
  //! Default constructor
  MXmlDocument();
  //! Default constructor
  MXmlDocument(TString Name);
  //! Default destructor
  virtual ~MXmlDocument();

  //! Load an xml document
  bool Load(TString FileName);
  //! Save an Xml ldocument
  bool Save(TString FileName);

  // protected methods:
 protected:
  //MXmlDocument() {};
  //MXmlDocument(const MXmlDocument& XmlDocument) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(MXmlDocument, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
