/*
 * NModuleBackgroundSimulatorTrivial.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleBackgroundSimulatorTrivial__
#define __NModuleBackgroundSimulatorTrivial__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceEvent.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleBackgroundSimulatorTrivial : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceEntry
{
  // public interface:
 public:
  //! Default constructor
  NModuleBackgroundSimulatorTrivial(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleBackgroundSimulatorTrivial();

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




#ifdef ___CINT___
 public:
  ClassDef(NModuleBackgroundSimulatorTrivial, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
