/*
 * NModuleObservatoryMerger.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleObservatoryMerger__
#define __NModuleObservatoryMerger__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <list>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceMerger.h"
#include "NModuleInterfaceObservatory.h"
#include "NObservatoryData.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleObservatoryMerger : public NModule, public NModuleInterfaceEvent, public NModuleInterfaceMerger, public NModuleInterfaceObservatory
{
  // public interface:
 public:
  //! Default constructor
  NModuleObservatoryMerger(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleObservatoryMerger();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the observatory data
  virtual bool AnalyzeObservatoryData(NObservatoryData& Data);

  //! Main data analysis routine, which updates the event to a new level 
  virtual bool AnalyzeEvent(NEvent& Event);

  //! Verify that enough data is available
  virtual bool HasEnoughData(NTime Time);

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
  //! That's a temporary store of star tracker data, from which the current pointing is interpolated
  list<NObservatoryData> m_ObservatoryData;



#ifdef ___CINT___
 public:
  ClassDef(NModuleObservatoryMerger, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
