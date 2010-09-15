/*
 * NModuleInterfaceDeadTime.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleInterfaceDeadTime__
#define __NModuleInterfaceDeadTime__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:
#include "NTime.h"
#include "NEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! The class is an DeadTime decorator for a module.
//! It provides a function to check if the stop criterion is fullfilled

class NModuleInterfaceDeadTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleInterfaceDeadTime();
  //! Default destructor
  virtual ~NModuleInterfaceDeadTime();

  //! Initialize
  virtual bool Initialize();
  
  //! Finalize
  virtual bool Finalize();
  
  //! Set if dead time is to be applied
  void SetApplyDeadTime(bool Apply) { m_ApplyDeadTime = Apply; }
  //! Check if dead time is to be applied
  bool GetApplyDeadTime() const { return m_ApplyDeadTime; }
  
  //! Return the time, when the dead time is over
  NTime GetDeadTimeEndTelescope(int TelescopeID) const;

  //! Set the time when the dead time ends
  //! The time you set is the CURRENT time of the event, not the dead time
  void SetDeadTimeDetectorHit(NTime CurrentTime, int Telescope);

  //! Check if the event is lost in dead time and if yes set the appropriate flags
  bool IsLostInDeadTime(NEvent& Event);

  
  // protected methods:
 protected:



  // private methods:
 private:



  // protected members:
 protected:
  //! True if dead time should be applied
  bool m_ApplyDeadTime;
  //! The time when the dead time is over for telescope 1
  NTime m_DeadTimeEndTelescope1;
  //! The time when the dead time is over for telescope 2
  NTime m_DeadTimeEndTelescope2;
  
  //! The dead time due to a detector hit
  NTime m_DetectorDeadTime;
  //! The dead time counter for detector 1
  unsigned int m_DeadTimeCouter1;
  //! The dead time counter for detector 2
  unsigned int m_DeadTimeCouter2;
  //! The number of events lost in dead time of detector 1
  unsigned int m_NEventsLostInDeadTime1;
  //! The number of events lost in dead time of detector 2
  unsigned int m_NEventsLostInDeadTime2;
  //! The number of events NOT lost in dead time of detector 1
  unsigned int m_NEventsNotLostInDeadTime1;
  //! The number of events NOT lost in dead time of detector 2
  unsigned int m_NEventsNotLostInDeadTime2;

  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(NModuleInterfaceDeadTime, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
