/*
 * NModuleTriggerEngineBackgroundSims.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleTriggerEngineBackgroundSims__
#define __NModuleTriggerEngineBackgroundSims__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleTriggerEngineBackgroundSims : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleTriggerEngineBackgroundSims(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleTriggerEngineBackgroundSims();

  //! Initialize the module
  virtual bool Initialize();

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

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

  //! Low veto trigger energy threshold
  double m_LowTrigger;
  //! High veto trigger energy threshold
  double m_HighTrigger;

  //! The pixel trigger threshold
  double m_TriggerThreshold;




#ifdef ___CINT___
 public:
  ClassDef(NModuleTriggerEngineBackgroundSims, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
