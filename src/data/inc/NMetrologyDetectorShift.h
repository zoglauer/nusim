/*
 * NMetrologyDetectorShift.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NMetrologyDetectorShift__
#define __NMetrologyDetectorShift__


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


class NMetrologyDetectorShift
{
  // public interface:
 public:
  //! Standard constructor
  NMetrologyDetectorShift();
  //! Copy constructor
  NMetrologyDetectorShift(const NMetrologyDetectorShift& MetrologyDetectorShift) { (*this) = MetrologyDetectorShift; }
  //! Default destructor
  virtual ~NMetrologyDetectorShift();

  //! Assignment operator
  const NMetrologyDetectorShift& operator=(const NMetrologyDetectorShift& MetrologyDetectorShift);

  //! Resets all data to default values
  virtual void Clear();
  
  //! Return the Position (z position is always zero)
  MVector GetPosition() { return m_Position; }
  //! Return the shift (z position is always zero)
  MVector GetShift() { return m_Shift; }

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

  //! The X-Y-Position (z is set to zero)
  MVector m_Position;

  //! The shift at the above X-Y-Position (z is set to zero)
  MVector m_Shift;



#ifdef ___CINT___
 public:
  ClassDef(NMetrologyDetectorShift, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
