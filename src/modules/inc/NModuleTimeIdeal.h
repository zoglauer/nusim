/*
 * NModuleTimeIdeal.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleTimeIdeal__
#define __NModuleTimeIdeal__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR:
#include "NTime.h"
#include "NModule.h"
#include "NModuleInterfaceTime.h"
#include "NOrbit.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Satellite module providing all time information.
//! This module is part of the satellite super-module and provides all relevant times,
//! the ideal time, the time the satellite bus measures, the time detector 1 and 2 measure 

class NModuleTimeIdeal : public NModule, public NModuleInterfaceTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleTimeIdeal(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleTimeIdeal();

  //! Initialize the module
  virtual bool Initialize();

  //! Set the ideal time
  virtual void SetTimeIdeal(const NTime& Ideal) { m_Ideal = Ideal; }
  //! Advance the ideal time
  virtual void AdvanceTimeIdeal(const NTime& Ideal) { m_Ideal += Ideal; }
  //! Get the ideal time
  virtual NTime GetTimeIdeal() { return m_Ideal; }


  //! Set the effective observation time
  virtual void SetEffectiveObservationTime(const NTime& Time) { m_EffectiveObservationTime = Time; }
  //! Get the effective observation time
  virtual NTime GetEffectiveObservationTime() { return m_EffectiveObservationTime; }

  //! Set the absolute observation start time
  virtual void SetAbsoluteObservationStartTime(const NTime& Time) { m_AbsoluteObservationStartTime = Time; }
  //! Get the absolute observation start time
  virtual NTime GetAbsoluteObservationStartTime() { return m_AbsoluteObservationStartTime; }

  //! Set the absolute observation end time
  virtual void SetAbsoluteObservationEndTime(const NTime& Time) { m_AbsoluteObservationEndTime = Time; }
  //! Get the absolute observation end time
  virtual NTime GetAbsoluteObservationEndTime() { return m_AbsoluteObservationEndTime; }


  //! Get the time as measured by the instrument
  virtual NTime GetTimeSatelliteBus();

  //! Get the time as measured by detector 1
  virtual NTime GetTimeDetector1();

  //! Get the time as measured by detector 2
  virtual NTime GetTimeDetector2();
  

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The start time of the observations
  NTime m_AbsoluteObservationStartTime;
  //! The end time of the observations
  NTime m_AbsoluteObservationEndTime;
  
  //! The absolute time
  NTime m_Ideal;
  //! The effective observation time;
  NTime m_EffectiveObservationTime;

  
#ifdef ___CINT___
 public:
  ClassDef(NModuleTimeIdealPosition, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
