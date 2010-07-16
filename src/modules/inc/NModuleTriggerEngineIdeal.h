/*
 * NModuleTriggerEngineIdeal.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleTriggerEngineIdeal__
#define __NModuleTriggerEngineIdeal__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleTriggerEngineIdeal : public NModule, public NModuleInterfaceEvent
{
  // public interface:
 public:
  //! Default constructor
  NModuleTriggerEngineIdeal(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleTriggerEngineIdeal();

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
  ClassDef(NModuleTriggerEngineIdeal, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
