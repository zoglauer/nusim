/*
 * NModuleInterfaceTime.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceTime__
#define __NModuleInterfaceTime__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief The class is a the interface class for the time modules
//! This interface class provides the functions for retrieving all different times.


class NModuleInterfaceTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceTime();
  //! Default destructor
  virtual ~NModuleInterfaceTime();

  //! Set the NuSIM time --- only the supervisor should touch this function
  virtual void SetTime(const NTime& Time) { m_Time = Time; }
  //! Advance the ideal time --- only the supervisor should touch this function
  virtual void AdvanceTime(const NTime& Time) { m_Time += Time; }
  //! Get the NuSIM time
  virtual NTime GetTime() { return m_Time; }
  

  //! Get the NuSIM time as time since Epoch
  virtual NTime GetTimeSinceEpoch() { return GetAbsoluteTime() - m_Epoch; }
  
  
  //! Get the absolute time
  virtual NTime GetAbsoluteTime() { return m_Time + m_AbsoluteObservationStartTime; }


  //! Set the current effective observation time --- only the supervisor should touch this function
  virtual void SetEffectiveObservationTime(const NTime& Time) { m_EffectiveObservationTime = Time; }
  //! Get the current observation time
  virtual NTime GetEffectiveObservationTime() { return m_EffectiveObservationTime; }


  //! Set the absolute observation start time
  virtual void SetAbsoluteObservationStartTime(const NTime& Time) { m_AbsoluteObservationStartTime = Time; }
  //! Get the absolute observation start time
  virtual NTime GetAbsoluteObservationStartTime() { return m_AbsoluteObservationStartTime; }

  //! Set the absolute observation end time
  virtual void SetAbsoluteObservationEndTime(const NTime& Time) { m_AbsoluteObservationEndTime = Time; }
  //! Get the absolute observation end time
  virtual NTime GetAbsoluteObservationEndTime() { return m_AbsoluteObservationEndTime; }

  //! Convert from NuSIM time to time since Epoch
  virtual NTime ConvertToTimeSinceEpoch(const NTime& Time) { return Time + m_AbsoluteObservationStartTime - m_Epoch; }
  //! Convert to NuSIM time from time since Epoch
  virtual NTime ConvertFromTimeSinceEpoch(const NTime& Time) { return Time - m_AbsoluteObservationStartTime + m_Epoch; }
  //! Convert from NuSIM time to absolute time
  virtual NTime ConvertToAbsoluteTime(const NTime& Time) { return Time + m_AbsoluteObservationStartTime; }
  //! Convert to NuSIM time from absolute time
  virtual NTime ConvertFromAbsoluteTime(const NTime& Time) { return Time - m_AbsoluteObservationStartTime; }
  //! Convert from NuSIM time to effective observation time ---- only the orbit module can to that!!
  // virtual NTime ConvertToEffectiveObservationTime(const NTime& Time) { }

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:
  //! The epoch time 1/1/2010
  NTime m_Epoch;

  //! The NuSIM time - time since observation start
  NTime m_Time;
    
  //! The effective observation time;
  NTime m_EffectiveObservationTime;

  //! The start time of the observations
  NTime m_AbsoluteObservationStartTime;
  //! The end time of the observations
  NTime m_AbsoluteObservationEndTime;

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceTime, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
