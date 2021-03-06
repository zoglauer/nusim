/*
 * NOpticsDBEntry.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NOpticsDBEntry__
#define __NOpticsDBEntry__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NOpticsDBEntry
{
  // public interface:
 public:
  //! Standard constructor
  NOpticsDBEntry();
  //! Default destructor
  virtual ~NOpticsDBEntry();

  //! Resets all data to default values
  virtual void Clear();
  
  //! Set from an interpolation between the two values
  //! Fraction needs to be between ]0..1[
  void SetInterpolated(const NOpticsDBEntry& A, const NOpticsDBEntry& B, double Fraction);
  
  //! The bore sight of an optics module 
  MVector GetBoreSightRelOM(int ModuleID);

    
  //! Get the time to which these orientations belong
  virtual NTime GetTime() const { return m_Time; }
  //! Set the time to which these orientations belong
  virtual void SetTime(NTime Time) { m_Time = Time; }


  //! Return the content as text
  virtual TString ToString() const;
  
  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);

  //! Stream the content from the data base 
  bool ParseDB(TString Line);

  
  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The time of the orientations
  NTime m_Time;

  //! The bore sight of optics module 1
  MVector m_BoreSight1RelOM;
  //! The bore sight of optics module 2
  MVector m_BoreSight2RelOM;



#ifdef ___CINT___
 public:
  ClassDef(NOpticsDBEntry, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
