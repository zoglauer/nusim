/*
 * NMetrologyDBEntry.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NMetrologyDBEntry__
#define __NMetrologyDBEntry__


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


class NMetrologyDBEntry
{
  // public interface:
 public:
  //! Standard constructor
  NMetrologyDBEntry();
  //! Copy constructor
  NMetrologyDBEntry(const NMetrologyDBEntry& MetrologyDBEntry) { (*this) = MetrologyDBEntry; }
  //! Default destructor
  virtual ~NMetrologyDBEntry();

  //! Assignment operator
  const NMetrologyDBEntry& operator=(const NMetrologyDBEntry& MetrologyDBEntry);

  //! Resets all data to default values
  virtual void Clear();
  
  //! Set from an interpolation between the two values
  //! Fraction needs to be between ]0..1[
  void SetInterpolated(const NMetrologyDBEntry& A, const NMetrologyDBEntry& B, double Fraction);
  
  //! Get the pointing error of the star tracker
  virtual double GetPointingErrorStarTracker(int StarTrackerID);
 
  //! Get the pointing direction of the metrology laser (independent of time)
  virtual MVector GetPointingMetrologyLaserRelML(int MetrologyID);
 
  //! Get the origin direction of the metrology laser (independent of time)
  virtual MVector GetOriginMetrologyLaserRelML(int MetrologyID);

  //! Get the centroiding error of the metrology detector
  virtual double GetCentroidingErrorMetrologyDetector(int MetrologyID);
  
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

   
  //! The pointing knowledge error of the star tracker
  double m_PointingErrorStarTracker4;
  
  //! The origin of metrology laser 1
  MVector m_OriginMetrologyLaser1RelML1;
  //! The pointing of metrology laser 1
  MVector m_PointingMetrologyLaser1RelML1;
  //! The origin of metrology laser 2
  MVector m_OriginMetrologyLaser2RelML2;
  //! The pointing of metrology laser 2
  MVector m_PointingMetrologyLaser2RelML2;

  //! The centroiding error of the metrology detector 1
  double m_CentroidingErrorMetrologyDetector1;
  //! The centroiding error of the metrology detector 2
  double m_CentroidingErrorMetrologyDetector2;



#ifdef ___CINT___
 public:
  ClassDef(NMetrologyDBEntry, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
