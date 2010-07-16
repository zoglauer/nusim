/*
 * NStarTrackerDataSet.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NStarTrackerDataSet__
#define __NStarTrackerDataSet__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cmath>
#include <fstream>
using namespace std;

// ROOT libs:
#include "NQuaternion.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"
#include "NPointing.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing the measured star tracker data.
//! This class contains the measured star tracker data (the tranformation between celestial and its internal coordinate system), 
//! the measurement time. the reconstructed pointing, 
//! as well as the original pointing of the star tracker for diagnostics reasons.

class NStarTrackerDataSet
{
  // public interface:
 public:
  //! Standard constructor
  NStarTrackerDataSet();
  //! Default destructor
  virtual ~NStarTrackerDataSet();

  //! Resets all data to default values
  virtual void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the original pointing
  void SetOriginalPointing(const NPointing& P) { m_Empty = false; m_OriginalPointing = P; }
  //! Return the original pointing
  NPointing GetOriginalPointing() const { return m_OriginalPointing; }

  //! Set the measurement time
  void SetTime(const NTime& Time) { m_Empty = false; m_Time = Time; }
  //! Get the measurement time
  NTime GetTime() const { return m_Time; }

  //! Set the measured star tracker transformation
  void SetMeasuredTransformation(const NQuaternion& Q) { m_Transformation = Q; }
  //! Return the measured star tracker transformation
  NQuaternion GetMeasuredTransformation() const { return m_Transformation; }

  //! Set the reconstructed pointing
  void SetReconstructedPointing(const NPointing& P) { m_Empty = false; m_ReconstructedPointing = P; }
  //! Return the reconstructed pointing
  NPointing GetReconstructedPointing() const { return m_ReconstructedPointing; }
  
  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, TString Keyword);

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

  //! Time the star tracker data was taken
  NTime m_Time;

  //! The original pointing of the satellite 
  NPointing m_OriginalPointing;

  //! The measured quaternion (rotation) returned by the star tracker 
  NQuaternion m_Transformation;

  //! The reconstrcuted pointing of the satellite
  NPointing m_ReconstructedPointing;


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NStarTrackerDataSet, 0) // no description
#endif

};


////////////////////////////////////////////////////////////////////////////////


//! \brief Simple structure allowing to compare NStarTrackerDataSet objects by their measurement time.
//! This structure enables comparing (and ultimately sorting) NStarTrackerDataSet objects by
//! their measurement time.
struct NStarTrackerDataSetTimeComparator
{
  bool operator() (const NStarTrackerDataSet& D, NTime Time) const
  {
    return D.GetTime() < Time;
  }

  bool operator() (NTime Time, const NStarTrackerDataSet& D) const
  {
    return Time < D.GetTime();
  }
};


#endif


////////////////////////////////////////////////////////////////////////////////
