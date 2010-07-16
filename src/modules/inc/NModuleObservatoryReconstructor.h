/*
 * NModuleObservatoryReconstructor.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NModuleObservatoryReconstructor__
#define __NModuleObservatoryReconstructor__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <list>
using namespace std;

// ROOT libs:

// MEGAlib libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "NModule.h"
#include "NModuleInterfaceMerger.h"
#include "NModuleInterfaceStarTracker.h"
#include "NModuleInterfaceMetrology.h"
#include "NModuleInterfaceObservatory.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfacePhotonPropagation.h"
#include "NStarTrackerData.h"
#include "NMetrologyData.h"
#include "NObservatoryData.h"
#include "NOrientation.h"
#include "NObservatoryReconstructor.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NModuleObservatoryReconstructor : public NModule, public NModuleInterfaceMerger, public NModuleInterfaceEvent, 
      public NModuleInterfaceStarTracker, public NModuleInterfaceMetrology, public NModuleInterfaceObservatory, public NModuleInterfacePhotonPropagation
{
  // public interface:
 public:
  //! Default constructor
  NModuleObservatoryReconstructor(NSatellite& Satellite);
  //! Default destructor
  virtual ~NModuleObservatoryReconstructor();

  //! Initialize the module
  virtual bool Initialize();

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeMetrologyData(NMetrologyData& Data);

  // Main data analysis routine, which updates the star tracker data
  virtual bool AnalyzeStarTrackerData(NStarTrackerData& Data);

  // Main data analysis routine, which updates the observatory data
  virtual bool AnalyzeObservatoryData(NObservatoryData& Data) { return true; };

  // Main data analysis routine, which update the event data
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
  //! That's a temporary store of star tracker data, from which the pointing at event time is interpolated
  list<NStarTrackerData> m_StarTrackerData;

  //! That's a temporary store of metrology data, from which the pointing at event time is interpolated
  list<NMetrologyData> m_MetrologyData;

  //! Time of last observatory data generation
  NTime m_Time;

  //! The observatory reconstructor
  NObservatoryReconstructor m_ObservatoryReconstructor;

#ifdef ___CINT___
 public:
  ClassDef(NModuleObservatoryReconstructor, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
