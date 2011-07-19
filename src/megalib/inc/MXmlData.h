/*
 * MXmlData.h
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MXmlData__
#define __MXmlData__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cstdlib>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MXmlData
{
  // public interface:
 public:
  //! Default constructor
  MXmlData();
  //! Constructor -- no values
  MXmlData(TString Name);
  //! Constructor -- content is string
  MXmlData(TString Name, TString Value);
  //! Constructor -- content is integer
  MXmlData(TString Name, int Value);
  //! Constructor -- content is integer
  MXmlData(TString Name, long Value);
  //! Constructor -- content is unsigned integer
  MXmlData(TString Name, unsigned int Value);
  //! Constructor -- content is double
  MXmlData(TString Name, double Value);
  //! Constructor -- content is a boolean
  MXmlData(TString Name, bool Value);
  //! Default destructor
  virtual ~MXmlData();

  //! Reset the node
  virtual void Clear();

  //! Set the name of the node
  void SetName(TString Name) { m_Name = Name; }
  //! Get the name of the node
  TString GetName() const { return m_Name; }

  //! Set the value of the node
  void SetValue(TString Value) { m_Value = Value; }
  //! Return the value of the node
  TString GetValue() const { return m_Value; }

  //! Return the value of the node
  TString GetValueAsString() const { return m_Value; }
  //! Return the value of the node as int
  int GetValueAsInt() const { return atoi(m_Value); }
  //! Return the value of the node as long
  long GetValueAsLong() const { return atol(m_Value); }
  //! Return the value of the node as unsigned int
  unsigned int GetValueAsUnsignedInt() const { return atoi(m_Value); }
  //! Return the value of the node as double
  double GetValueAsDouble() const { return atof(m_Value); }
  //! Return the value of the node as boolean
  bool GetValueAsBoolean() const;


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! The name of the node
  TString m_Name;
  //! The value of the node (if any)
  TString m_Value;


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(MXmlData, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
