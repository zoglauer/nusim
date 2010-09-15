/*
 * NModuleInterfaceDeadTime.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleInterfaceDeadTime
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleInterfaceDeadTime.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleInterfaceDeadTime)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceDeadTime::NModuleInterfaceDeadTime()
{
  // Construct an instance of NModuleInterfaceDeadTime

  m_ApplyDeadTime = false;
}


////////////////////////////////////////////////////////////////////////////////


NModuleInterfaceDeadTime::~NModuleInterfaceDeadTime()
{
  // Delete this instance of NModuleInterfaceDeadTime
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceDeadTime::Initialize()
{
  //! Initialize

  m_DeadTimeEndTelescope1.SetSeconds(0.0);
  m_DeadTimeEndTelescope2.SetSeconds(0.0);
  m_DetectorDeadTime.SetSeconds(0.0025);

  m_DeadTimeCouter1 = 0;
  m_DeadTimeCouter2 = 0;
  m_NEventsLostInDeadTime1 = 0;
  m_NEventsLostInDeadTime2 = 0;
  m_NEventsNotLostInDeadTime1 = 0;
  m_NEventsNotLostInDeadTime2 = 0;

  return true;
}


////////////////////////////////////////////////////////////////////////////////



bool NModuleInterfaceDeadTime::Finalize()
{
  //! Finalize
  
  return true;
}
  

////////////////////////////////////////////////////////////////////////////////


bool NModuleInterfaceDeadTime::IsLostInDeadTime(NEvent& Event) 
{
  //! Return the time, when the dead time is over

  if (Event.GetTelescope() == 1) {
    if (Event.GetTime() <= m_DeadTimeEndTelescope1) {
      //cout<<"Lost in dead time 1: "<<Event.GetID()<<" "<<Event.GetTime()<<" --> "<<m_DeadTimeEndTelescope1<<endl;
      Event.SetLostInDeadTime(true);
      m_NEventsLostInDeadTime1++;
      return true;
    } else {
      m_NEventsNotLostInDeadTime1++;      
    }
  } else if (Event.GetTelescope() == 2) {
    if (Event.GetTime() <= m_DeadTimeEndTelescope2) {
      //cout<<"Lost in dead time 2: "<<Event.GetID()<<" "<<Event.GetTime()<<" --> "<<m_DeadTimeEndTelescope2<<endl;
      Event.SetLostInDeadTime(true);
      m_NEventsLostInDeadTime2++;
      return true;
    } else {
      m_NEventsNotLostInDeadTime2++;      
    }
  }
  
  return false;
}  

////////////////////////////////////////////////////////////////////////////////


NTime NModuleInterfaceDeadTime::GetDeadTimeEndTelescope(int TelescopeID) const
{
  //! Return the time, when the dead time is over

  if (TelescopeID == 1) {
    return m_DeadTimeEndTelescope1;
  } else if (TelescopeID == 2) {
    return m_DeadTimeEndTelescope2;
  } else {
    cout<<"SEVERE ERROR: Telescope ID = "<<TelescopeID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
    return m_DeadTimeEndTelescope1;
  }
}


////////////////////////////////////////////////////////////////////////////////


void NModuleInterfaceDeadTime::SetDeadTimeDetectorHit(NTime CurrentTime, int TelescopeID)
{
  //! Set the time when the dead time ends
  //! The time you set is the CURRENT time of the event, not the dead time
    
  if (TelescopeID == 1) {
    m_DeadTimeEndTelescope1 = CurrentTime + m_DetectorDeadTime;
    //cout<<"Setting dead time 1 until "<<m_DeadTimeEndTelescope1<<" (Now: "<<CurrentTime<<")"<<endl;
    m_DeadTimeCouter1++;
  } else if (TelescopeID == 2) {
    m_DeadTimeEndTelescope2 = CurrentTime + m_DetectorDeadTime;
    //cout<<"Setting dead time 2 until "<<m_DeadTimeEndTelescope2<<" (Now: "<<CurrentTime<<")"<<endl;
    m_DeadTimeCouter2++;
  } else {
    cout<<"SEVERE ERROR: Telescope ID = "<<TelescopeID<<" not allowed. Must be 1 or 2."<<endl;
    cout<<"The results for this event will be wrong!"<<endl;
  }
}


// NModuleInterfaceDeadTime.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
