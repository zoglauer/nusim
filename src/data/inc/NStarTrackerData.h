/*
 * NStarTrackerData.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NStarTrackerData__
#define __NStarTrackerData__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
#include <cmath>
using namespace std;

// ROOT libs:
#include "NQuaternion.h"

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NStarTrackerDataSet.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing the all star tracker data.
//! This class contains all the measured star tracker data sets
//! Therefore currently only the data set from the star tracker on the optics bench 

class NStarTrackerData
{
  // public interface:
 public:
  //! Standard constructor
  NStarTrackerData();
  //! Default destructor
  virtual ~NStarTrackerData();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return (m_Empty && m_StarTracker4.IsEmpty()); }

  //! \todo Verify that the star tracker data sets are all read out at the same time:

  //! Get the measurement time --- I currently assume all star tracker data sets are read out at the same time:
  NTime GetTime() const { return m_StarTracker4.GetTime(); }

  //! Set the original pointing
  void SetStarTrackerDataSet4(const NStarTrackerDataSet& S) { m_Empty = false; m_StarTracker4 = S; }
  //! Return a copy of the data of star tracker CHU
  NStarTrackerDataSet GetStarTrackerDataSet4() const { return m_StarTracker4; }
  //! Return a REFERENCE TO the data of star tracker CHU
  NStarTrackerDataSet& GetStarTrackerDataSet4ByRef() { return m_StarTracker4; }

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);
  
  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! The data set of star tracker number 4 (the one on the optical bench)
  NStarTrackerDataSet m_StarTracker4;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NStarTrackerData, 0) // no description
#endif

};


////////////////////////////////////////////////////////////////////////////////


//! \brief Simple structure allowing to compare NStarTrackerData objects by their measurement time.
//! This structure enables comparing (and ultimately sorting) NStarTrackerData objects by
//! their measurement time.
struct NStarTrackerDataTimeComparator
{
  bool operator() (const NStarTrackerData& D, NTime Time) const
  {
    return D.GetTime() < Time;
  }

  bool operator() (NTime Time, const NStarTrackerData& D) const
  {
    return Time < D.GetTime();
  }
};


#endif


////////////////////////////////////////////////////////////////////////////////
