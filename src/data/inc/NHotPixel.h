/*
 * NHotPixel.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NHotPixel__
#define __NHotPixel__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MVector.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"
#include "NInteraction.h"
#include "NSatellite.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a hot pixel


class NHotPixel
{
  // public interface:
 public:
  //! Standard constructor
  NHotPixel();
  //! Copy constructor
  NHotPixel(const NHotPixel& HotPixel) { (*this) = HotPixel; }
  //! Default destructor
  virtual ~NHotPixel();

  //! Set detector boundaries
  void SetDetectorBoundaries(const MVector& Boundaries);
  
  //! Assignment operator
  const NHotPixel& operator=(const NHotPixel& HotPixel);

  //! Return true if the HotPixel is empty
  bool IsEmpty() const { return m_Empty; }

  //! Return the time of the next event
  NTime GetTimeOfNextEvent() { return m_Time; } 
  
  //! Perform a time jump:
  void PerformTimeJump(const NTime& TimeJump);

  //! Get the telescope
  unsigned int GetTelescope() { return m_Telescope; }
  
  //! Create an interaction
  NInteraction GetNextInteraction();

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);


  // protected methods:
 protected:
  //! Adance the internal data to the next hot pixel event
  void Advance();

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;
  
  //! The active detector boundaries
  MVector m_Boundaries;
  
  //! Time of the next event
  NTime m_Time;
  
  //! Telescope ID
  int m_Telescope;
  //! Detector ID
  int m_Detector;
  //! Position
  MVector m_Position;
  
  //! We have a random telescope, detector, position each time it starts again
  bool m_RandomPosition;
  
  //! Start time
  NTime m_Start;
  //! Stop time
  NTime m_Stop;
  
  //! On duration in sec (mean)
  double m_OnDuration;
  //! On duration uncertainty in sec (1 sigma)
  double m_OnDurationUncertainty;
  
  //! Off duration in sec
  double m_OffDuration;
  //! Off duration uncertainty in sec (1 sigma)
  double m_OffDurationUncertainty;
  
  //! Current fire frequency in Hz
  double m_CurrentFireFrequency;
  
  //! Fire frequency in Hz
  double m_FireFrequency;
  //! Fire frequency uncertainty in Hz
  double m_FireFrequencyUncertainty;

  //! Energy in keV
  double m_Energy;
  //! Energy resolution in keV (1 sigma)
  double m_EnergyUncertainty;


#ifdef ___CINT___
 public:
  ClassDef(NHotPixel, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
