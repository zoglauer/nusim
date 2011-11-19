/*
 * NPointingJitterDBEntry.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPointingJitterDBEntry__
#define __NPointingJitterDBEntry__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"
#include "NQuaternion.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NPointingJitterDBEntry
{
  // public interface:
 public:
  //! Standard constructor
  NPointingJitterDBEntry();
  //! Copy constructor
  NPointingJitterDBEntry(const NPointingJitterDBEntry& PointingJitterDBEntry) { (*this) = PointingJitterDBEntry; }
  //! Default destructor
  virtual ~NPointingJitterDBEntry();

  //! Assignment operator
  const NPointingJitterDBEntry& operator=(const NPointingJitterDBEntry& PointingJitterDBEntry);

  //! Resets all data to default values
  virtual void Clear();
  
  //! Set from an interpolation between the two values
  //! Fraction needs to be between ]0..1[
  void SetInterpolated(const NPointingJitterDBEntry& A, const NPointingJitterDBEntry& B, double Fraction);
  
  //! The bore sight of an PointingJitter module 
  NQuaternion GetQuaternion() const { return m_Quaternion; }

    
  //! Get the time to which these orientations belong
  virtual NTime GetTime() const { return m_Time; }
  //! Set the time to which these orientations belong
  virtual void SetTime(NTime Time) { m_Time = Time; }


  //! Return the content as text
  virtual TString ToString() const;

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

  //! The quaternion
  NQuaternion m_Quaternion;



#ifdef ___CINT___
 public:
  ClassDef(NPointingJitterDBEntry, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
