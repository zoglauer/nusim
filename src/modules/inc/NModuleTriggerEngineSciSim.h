/*
 * NModuleTriggerEngineSciSim.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleTriggerEngineSciSim__
#define __NModuleTriggerEngineSciSim__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceDeadTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleTriggerEngineSciSim : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceDeadTime
{
  // public interface:
 public:
  //! Default constructor
  NModuleTriggerEngineSciSim(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleTriggerEngineSciSim();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Finalize the module
  virtual bool Finalize();

  //! Show the options GUI
  virtual void ShowOptionsGUI();

  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();

  //! Set the low trigger
  void   SetLowTrigger(double LowTrigger) { m_LowTrigger = LowTrigger; }
  //! Set the high trigger
  void   SetHighTrigger(double HighTrigger) { m_HighTrigger = HighTrigger; }
  //! Get the low trigger
  double GetLowTrigger() const { return m_LowTrigger; }
  //! Get the high trigger
  double GetHighTrigger() const { return m_HighTrigger; }

  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:

  //! Low veto trigger energy threshold
  double m_LowTrigger;
  //! High veto trigger energy threshold
  double m_HighTrigger;

  //! The pixel trigger threshold
  double m_TriggerThreshold;

  //! The pixel trigger pattern
  static const int m_TriggerPattern[33];

  //! Life time counter of telescope 1
  NTime m_LifeTime1;
  //! Life time counter of telescope 2
  NTime m_LifeTime2;
  

#ifdef ___CINT___
 public:
  ClassDef(NModuleTriggerEngineSciSim, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
