/*
 * MFileResponse.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MFileResponse__
#define __MFileResponse__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TString.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MParser.h"
#include "MTokenizer.h"

// Standard libs:
#include <vector>
#include <sstream>
using namespace std;

// Forward declarations:
class MResponseMatrix;

////////////////////////////////////////////////////////////////////////////////


class MFileResponse : public MParser
{
  // public interface:
 public:
  explicit MFileResponse();
  virtual ~MFileResponse();

  MResponseMatrix* Read(TString FileName);

  //! Open the file name and read the header
  virtual bool Open(TString FileName, unsigned int Way = 1);
  //! Return the name - available after a call to Open
  TString GetName() const { return m_Name; }
  //! Are the values centered
  bool AreValuesCentered() const { return m_ValuesCentered; }
  //! Are the values centered
  unsigned long GetHash() const { return m_Hash; }

  // protected methods:
 protected:

  // private methods:
 private:

  // protected members:
 protected:
  //! Name of the response matrix
  TString m_Name;
  //! Are the values stored cenetered
  bool m_ValuesCentered;
  //! The hash value
  unsigned long m_Hash;

#ifdef ___CINT___
 public:
  ClassDef(MFileResponse, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
