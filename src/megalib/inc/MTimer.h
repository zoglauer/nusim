/*
 * MTimer.h
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MTimer__
#define __MTimer__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MTimer
{
  // public interface:
 public:
  MTimer();
  MTimer(double TimeOutSeconds);
  virtual ~MTimer();

	void SetTimeOut(double TimeOutSeconds = -1);

  //! Start the timer - counting resarts from zero
  void Start();
  //! Reset the timer
  void Reset();

  //! Pause the timer
  void Pause();
  //! Continue the timer
  void Continue();

  //! 
  double GetElapsed() { return ElapsedTime(); }
  double ElapsedTime();
  bool HasTimedOut(double Seconds = -1);
	void SetHasTimedOut();
  double GetTimeOut();

  // protected methods:
 protected:
  double GetRelativeTime();

  // protected members:
 protected:


  // private members:
 private:
  //! Time since last start or reset
  double m_Time;
  //! Total elapsed time since last start or reset
  double m_ElapsedTime;
  //! Time which needs to be elapsed until a timeout will be triggered
	double m_TimeOut;

  //! Flag indicating that this timer is paused
	double m_IsPaused;

  //! Flag indicating a time out, even when the real time has not yet elapsed 
	bool m_HasTimedOut;



#ifdef ___CINT___
 public:
  ClassDef(MTimer, 0) // no description
#endif
};

#endif


////////////////////////////////////////////////////////////////////////////////
