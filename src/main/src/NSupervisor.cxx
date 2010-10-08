/*
 * NSupervisor.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NSupervisor
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NSupervisor.h"

// Standard libs:
#include <algorithm>
#include <iomanip>
using namespace std;

// ROOT libs:
#include "TSystem.h"

// MEGAlib libs:
#include "MStreams.h"
#include "MTimer.h"
#include "MXmlDocument.h"
#include "MXmlNode.h"


// NuSTAR libs:
#include "NModule.h"
#include "NModuleInterfaceEvent.h"
#include "NModuleInterfaceEntry.h"
#include "NModuleInterfaceMerger.h"
#include "NModuleInterfaceStarTracker.h"
#include "NModuleInterfaceMetrology.h"
#include "NModuleInterfaceOrbit.h"
#include "NModuleInterfacePointing.h"
#include "NModuleInterfaceOrientations.h"
#include "NModuleInterfaceTime.h"
#include "NModuleInterfaceGeometryAndDetectorProperties.h"

#include "NModuleSourceDistribution.h"

#include "NModuleOpticsEngine.h"
#include "NModuleOpticsEngineNone.h"
#include "NModuleOpticsEngineTrivial.h"

#include "NModuleApertureEngineNone.h"
#include "NModuleApertureEngineTrivial.h"

#include "NModuleDetectorSimulatorIdeal.h"
#include "NModuleDetectorSimulatorDetailed.h"
#include "NModuleDetectorSimulatorGeant4.h"
#include "NModuleDetectorSimulatorPropagatorOnly.h"

#include "NModuleBackgroundSimulatorTrivial.h"
#include "NModuleBackgroundSimulator.h"

#include "NModuleDetectorEffectsEngineSciSim.h"
#include "NModuleDetectorEffectsEnginePHE.h"
#include "NModuleDetectorEffectsEngineIdeal.h"
#include "NModuleDetectorEffectsEngineBackgroundSims.h"

#include "NModuleTriggerEngineIdeal.h"
#include "NModuleTriggerEnginePHE.h"
#include "NModuleTriggerEngineSciSim.h"
#include "NModuleTriggerEngineBackgroundSims.h"

#include "NModuleDetectorCalibratorSciSim.h"
#include "NModuleDetectorCalibratorIdeal.h"
#include "NModuleDetectorCalibratorBackgroundSims.h"

#include "NModuleEventSelector.h"
#include "NModuleEventSelectorBackgroundSims.h"

#include "NModuleBackprojector.h"
#include "NModuleImageDeconvolver.h"

#include "NModuleMetrologyEngine.h"
#include "NModuleMetrologyEngineTrivial.h"
#include "NModuleMetrologyCalibratorTrivial.h"
#include "NModuleMetrologyLoader.h"
#include "NModuleMetrologySaver.h"

#include "NModuleStarTrackerEngineTrivial.h"
#include "NModuleStarTrackerCalibratorTrivial.h"
#include "NModuleStarTrackerLoader.h"
#include "NModuleStarTrackerSaver.h"

#include "NModuleObservatoryReconstructor.h"
#include "NModuleObservatoryMerger.h"
#include "NModuleObservatoryLoader.h"
#include "NModuleObservatorySaver.h"

#include "NModuleEventLoader.h"
#include "NModuleEventSaver.h"
#include "NModuleEventSaverCompressedEventFormat.h"

#include "NModuleTimeIdeal.h"
#include "NModuleOrbitStationary.h"
#include "NModulePointingPredefined.h"
#include "NModulePointingDatabase.h"
#include "NModuleOrientationsDatabase.h"
#include "NModuleGeometryAndDetectorProperties.h"

#include "NGUIDiagnostics.h"
#include "NGUIDiagnosticsMain.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NSupervisor)
#endif


////////////////////////////////////////////////////////////////////////////////


NSupervisor::NSupervisor()
{
  // Construct an instance of NSupervisor

  Clear();

  // Add in this list all available modules:

  // Satellite modules
  m_AvailableModules.push_back(new NModuleTimeIdeal(m_Satellite));
  m_AvailableModules.push_back(new NModuleOrbitStationary(m_Satellite));
  m_AvailableModules.push_back(new NModulePointingPredefined(m_Satellite));
  m_AvailableModules.push_back(new NModulePointingDatabase(m_Satellite));
  m_AvailableModules.push_back(new NModuleOrientationsDatabase(m_Satellite));
  m_AvailableModules.push_back(new NModuleGeometryAndDetectorProperties(m_Satellite));
  
  // Pipeline modules
  m_AvailableModules.push_back(new NModuleSourceDistribution(m_Satellite));
  m_AvailableModules.push_back(new NModuleApertureEngineNone(m_Satellite));
  m_AvailableModules.push_back(new NModuleApertureEngineTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleOpticsEngine(m_Satellite));
  m_AvailableModules.push_back(new NModuleOpticsEngineNone(m_Satellite));
  m_AvailableModules.push_back(new NModuleOpticsEngineTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorSimulatorIdeal(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorSimulatorPropagatorOnly(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorSimulatorDetailed(m_Satellite));
  m_AvailableModules.push_back(new NModuleBackgroundSimulatorTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleBackgroundSimulator(m_Satellite));
  
  m_AvailableModules.push_back(new NModuleDetectorEffectsEngineSciSim(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorEffectsEnginePHE(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorEffectsEngineIdeal(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorEffectsEngineBackgroundSims(m_Satellite));
  
  m_AvailableModules.push_back(new NModuleTriggerEngineBackgroundSims(m_Satellite));
  m_AvailableModules.push_back(new NModuleTriggerEngineSciSim(m_Satellite));
  m_AvailableModules.push_back(new NModuleTriggerEnginePHE(m_Satellite));
  m_AvailableModules.push_back(new NModuleTriggerEngineIdeal(m_Satellite));
  
  m_AvailableModules.push_back(new NModuleDetectorCalibratorSciSim(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorCalibratorIdeal(m_Satellite));
  m_AvailableModules.push_back(new NModuleDetectorCalibratorBackgroundSims(m_Satellite));

  m_AvailableModules.push_back(new NModuleEventSelector(m_Satellite));
  m_AvailableModules.push_back(new NModuleEventSelectorBackgroundSims(m_Satellite));
  
  m_AvailableModules.push_back(new NModuleBackprojector(m_Satellite));

  m_AvailableModules.push_back(new NModuleEventLoader(m_Satellite));
  m_AvailableModules.push_back(new NModuleEventSaver(m_Satellite));
  m_AvailableModules.push_back(new NModuleEventSaverCompressedEventFormat(m_Satellite));

  m_AvailableModules.push_back(new NModuleStarTrackerEngineTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleStarTrackerCalibratorTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleStarTrackerLoader(m_Satellite));
  m_AvailableModules.push_back(new NModuleStarTrackerSaver(m_Satellite));

  m_AvailableModules.push_back(new NModuleMetrologyEngine(m_Satellite));
  m_AvailableModules.push_back(new NModuleMetrologyEngineTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleMetrologyCalibratorTrivial(m_Satellite));
  m_AvailableModules.push_back(new NModuleMetrologyLoader(m_Satellite));
  m_AvailableModules.push_back(new NModuleMetrologySaver(m_Satellite));
  
  m_AvailableModules.push_back(new NModuleObservatoryReconstructor(m_Satellite));
  //m_AvailableModules.push_back(new NModuleObservatoryMerger(m_Satellite));
  m_AvailableModules.push_back(new NModuleObservatoryLoader(m_Satellite));
  m_AvailableModules.push_back(new NModuleObservatorySaver(m_Satellite));
  
  m_SourceSequence.push_back(NModule::c_SourceGenerator);
  m_SourceSequence.push_back(NModule::c_OpticsEngine);
  m_SourceSequence.push_back(NModule::c_ApertureEngine);
  m_SourceSequence.push_back(NModule::c_DetectorEngine);
  m_SourceSequence.push_back(NModule::c_DetectorEffectsEngine);
  m_SourceSequence.push_back(NModule::c_TriggerEngine);
  m_SourceSequence.push_back(NModule::c_DetectorCalibrator);
  m_SourceSequence.push_back(NModule::c_ObservatoryReconstructor);
  m_SourceSequence.push_back(NModule::c_EventSelector);
  m_SourceSequence.push_back(NModule::c_ScienceAnalyzer);

  m_BackgroundSequence.push_back(NModule::c_BackgroundEngine);
  m_BackgroundSequence.push_back(NModule::c_DetectorEffectsEngine);
  m_BackgroundSequence.push_back(NModule::c_TriggerEngine);
  m_BackgroundSequence.push_back(NModule::c_DetectorCalibrator);
  m_BackgroundSequence.push_back(NModule::c_ObservatoryReconstructor);
  m_BackgroundSequence.push_back(NModule::c_EventSelector);
  m_BackgroundSequence.push_back(NModule::c_ScienceAnalyzer);

  m_MetrologySequence.push_back(NModule::c_MetrologyEngine);
  m_MetrologySequence.push_back(NModule::c_MetrologyCalibrator);
  m_MetrologySequence.push_back(NModule::c_ObservatoryReconstructor);

  m_StarTrackerSequence.push_back(NModule::c_StarTrackerEngine);
  m_StarTrackerSequence.push_back(NModule::c_StarTrackerCalibrator);
  m_StarTrackerSequence.push_back(NModule::c_ObservatoryReconstructor);

  m_ObservatorySequence.push_back(NModule::c_ObservatoryReconstructor);
  //m_ObservatorySequence.push_back(NModule::c_ObservatoryMerger);

  m_MergerSequence.push_back(NModule::c_ObservatoryReconstructor);
  //m_MergerSequence.push_back(NModule::c_ObservatoryMerger);


  //! Some GUI parameters
  m_ButtonSizeX = 210;
  m_ButtonSizeY = 40;
  m_ButtonMargins = 3;
  m_ButtonDistanceX = 25;
  m_ButtonDistanceY = 10;

  m_ToggleDiagnostics = false;
  m_DiagnosticsGUI = 0;
}


////////////////////////////////////////////////////////////////////////////////


NSupervisor::~NSupervisor()
{
  // Delete this instance of NSupervisor

  for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
    delete m_AvailableModules[m];
  }

  delete m_DiagnosticsGUI;
}


////////////////////////////////////////////////////////////////////////////////


void NSupervisor::Clear()
{
  //! Reset all data

  m_Modules.clear();
  m_SatelliteModules.clear();
  m_PipelineModules.clear();
  m_PipelineEntryModules.clear();
  m_BackgroundModules.clear();
  m_StarTrackerModules.clear();
  m_MetrologyModules.clear();
  m_ObservatoryModules.clear();
  m_MergerModules.clear();
  m_StopCriterionModules.clear();
  m_PointingModules.clear();
  
  m_Interrupt = false;

  m_DiagnosticsGUI = 0;

  m_ObservationTime = 1000.0; //s
  m_UpdateInterval = 10000;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::Run()
{
  // Analyze the data

  if (ValidateConsistency() == false) {
    return false;
  }

  map<int, NModule*>::iterator Iter;

  m_Interrupt = false;
  m_ToggleDiagnostics = false;

  // Initialize the satellite data

  // Pointing modules need the observation time:
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfacePointing*>((*Iter).second) != 0) {
      dynamic_cast<NModuleInterfacePointing*>((*Iter).second)->SetObservationTime(m_ObservationTime);
    }
  }


  // Initialize the satellite
  if (m_Satellite.Initialize() == false) {
    cout<<"Unable to initialize the satellite - aborting..."<<endl;
    return false;
  }
  // Initialize the module data:
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if ((*Iter).second->Initialize() == false) {
      cout<<"Unable to initialize module: "<<(*Iter).second->GetFullName()<<" - aborting..."<<endl;
      return false;
    }
  }

  // Set up the diagnostics GUI
  delete m_DiagnosticsGUI; // we do not delete but just unmap the GUI when pressing X or cancel
  m_DiagnosticsGUI = 0;

  int NDiagnostics = 0;
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if ((*Iter).second->HasDiagnosticsGUI() == true) {
      ++NDiagnostics;
    }
  }
  if (NDiagnostics > 0 && gROOT->IsBatch() == false) {
    m_DiagnosticsGUI = new NGUIDiagnosticsMain(m_Satellite);
    for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
      if ((*Iter).second->HasDiagnosticsGUI() == true) {
        m_DiagnosticsGUI->AddDiagnostics((*Iter).second->GetDiagnosticsGUI());
      }
    }
    m_DiagnosticsGUI->Create();
  }

  // If we have an exit module which is also an IO module we save the configuration
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfaceExit*>((*Iter).second) != 0 && 
      dynamic_cast<NModuleInterfaceIO*>((*Iter).second) != 0) {
      TString Name = dynamic_cast<NModuleInterfaceIO*>((*Iter).second)->GetFileName();
      Size_t Last = Name.Last('.');
      if (Last != 0) {
        Name = Name.Remove(Last, Name.Length() - Last);      
      }
      Name.Append(".cfg");
      Save(Name);
    }
  }

  // Reference to the pipeline (source or background) which has to be simulated next
  vector<NModuleInterfaceEvent*> NextPipeline = m_PipelineModules;

  NModuleInterfaceEntry* SourceStart = 0;
  if (m_PipelineModules.size() > 0) SourceStart = dynamic_cast<NModuleInterfaceEntry*>(m_PipelineModules[0]);
  NModuleInterfaceEntry* BackgroundStart = 0;
  if (m_BackgroundModules.size() > 0) BackgroundStart = dynamic_cast<NModuleInterfaceEntry*>(m_BackgroundModules[0]);
  NModuleInterfaceEntry* StarTrackerStart = 0;
  if (m_StarTrackerModules.size() > 0) StarTrackerStart = dynamic_cast<NModuleInterfaceEntry*>(m_StarTrackerModules[0]);
  NModuleInterfaceEntry* MetrologyStart = 0;
  if (m_MetrologyModules.size() > 0) MetrologyStart = dynamic_cast<NModuleInterfaceEntry*>(m_MetrologyModules[0]);
  
  NModuleInterfacePointing* Pointing = m_PointingModules[0];

  bool HasSourcePipe = false;
  if (m_PipelineModules.size() > 0 && SourceStart != 0) HasSourcePipe = true;
  bool HasBackgroundPipe = false;
  if (m_BackgroundModules.size() > 0 && BackgroundStart != 0) HasBackgroundPipe = true;
  bool HasStarTrackerPipe = false;
  if (m_StarTrackerModules.size() > 0 && StarTrackerStart != 0) HasStarTrackerPipe = true;
  bool HasMetrologyPipe = false;
  if (m_MetrologyModules.size() > 0 && MetrologyStart != 0) HasMetrologyPipe = true;
  bool HasObservatoryPipe = false;
  if (m_ObservatoryModules.size() > 0) HasObservatoryPipe = true;

  bool StarTrackerNext = false;
  bool MetrologyNext = false;


  // Do the pipeline
  NEvent Event;
  NStarTrackerData StarTrackerData;
  NMetrologyData MetrologyData;
  NObservatoryData ObservatoryData;

  NModuleObservatoryReconstructor* OR = 0;
  NModuleObservatoryMerger* OM = 0;
  for (unsigned int m = 0; m < m_MergerModules.size(); ++m) {
    if (dynamic_cast<NModuleObservatoryReconstructor*>(m_MergerModules[m]) != 0) {
      OR = dynamic_cast<NModuleObservatoryReconstructor*>(m_MergerModules[m]);
    } else if (dynamic_cast<NModuleObservatoryMerger*>(m_MergerModules[m]) != 0) {
      OM = dynamic_cast<NModuleObservatoryMerger*>(m_MergerModules[m]);
    }
  }


  MTimer Timer;
  int EventBasedUpdateInterval = 1000000;

  int EventID = -1;
  int NBlockedEvents = 0;
  int NLostInDeadTimeEvents = 0;
  int NVetoedEvents = 0;
  int NCutEvents = 0;
  int NPassedEvents = 0;
  
  m_Running = true;
  
  // Keep track of the star tracker time, since we might have to do intermediate times to take care of pointing slews
  NTime LastStarTrackerTime(-100);

  while (m_Satellite.GetTimeIdeal() < m_ObservationTime) {

    // Check if stop criteria are fullfilled:
    bool StopCriterionFullFilled = false;
    for (unsigned int m = 0; m < m_StopCriterionModules.size(); ++m) {
      if (m_StopCriterionModules[m]->StopCriterionFullFilled() == true) {
        mout<<"Supervisor: Module stop criterion fullfilled."<<endl;
        StopCriterionFullFilled = true;
        break;
      }
    }
    if (StopCriterionFullFilled == true) break;
    
    
    if (EventID == 10000) {
      EventBasedUpdateInterval = 0.5 * EventID/Timer.GetElapsed(); // at least update supervisor each half second
    }

    Event.Clear();

    // Check the source and the background pipeline, which one is next
    NTime TimeOfNextEvent;
    if (HasSourcePipe == true && HasBackgroundPipe == true) {
      if (SourceStart->GetTimeOfNextEvent() < BackgroundStart->GetTimeOfNextEvent()) {
        TimeOfNextEvent = SourceStart->GetTimeOfNextEvent();
        NextPipeline = m_PipelineModules;
      } else {
        TimeOfNextEvent = BackgroundStart->GetTimeOfNextEvent();
        NextPipeline = m_BackgroundModules;
      }
    } else if (HasSourcePipe == true && HasBackgroundPipe == false) {
      TimeOfNextEvent = SourceStart->GetTimeOfNextEvent();
      NextPipeline = m_PipelineModules;      
    } else if (HasSourcePipe == false && HasBackgroundPipe == true) {
      TimeOfNextEvent = BackgroundStart->GetTimeOfNextEvent();
      NextPipeline = m_BackgroundModules;
    } else {
      // If this is the case we might only have to generate satellite data or metrology data or star tracker data
      if (HasStarTrackerPipe == true && HasMetrologyPipe == true) {
        if (StarTrackerStart->GetTimeOfNextEvent() < MetrologyStart->GetTimeOfNextEvent()) {
          TimeOfNextEvent = StarTrackerStart->GetTimeOfNextEvent();
          StarTrackerNext = true;
          MetrologyNext = false;
        } else {
          TimeOfNextEvent = MetrologyStart->GetTimeOfNextEvent();
          StarTrackerNext = false;
          MetrologyNext = true;
        }
      } else if (HasStarTrackerPipe == true && HasMetrologyPipe == false) {
        TimeOfNextEvent = StarTrackerStart->GetTimeOfNextEvent();
        StarTrackerNext = true;
        MetrologyNext = false;
      } else if (HasStarTrackerPipe == false && HasMetrologyPipe == true) {
        TimeOfNextEvent = MetrologyStart->GetTimeOfNextEvent();
        StarTrackerNext = false;
        MetrologyNext = true;
      } else {
        // Nothing has to be done... perhaps some satellte data base handling in the future ...
        TimeOfNextEvent.Clear();
        mout<<"Supervisor: No start modules!"<<endl;
      }
    }

    // If time == numeric_limits<double>::max() the module might determine that it is Out of events - or have a function which checks this...
    if (TimeOfNextEvent.IsEmpty() == true) {
      mout<<"Supervisor: No start module has events left. Ending run."<<endl;
      break;
    }
    if (TimeOfNextEvent > m_ObservationTime) {
      m_Satellite.SetTimeIdeal(m_ObservationTime);
      mout<<"Supervisor: Observation time exceeded."<<endl;
      break;
    }
    
    // Set the current time:
    m_Satellite.SetTimeIdeal(TimeOfNextEvent);

    //cout<<"T: "<<m_Satellite.GetTimeIdeal()<<endl;

    if (HasSourcePipe == true || HasBackgroundPipe == true) {
      // Check if we have enough data in the star tracker & metrology stream

      if (OR != 0) {
        while (OR->HasEnoughData(TimeOfNextEvent) == false) {
          NTime iTimeOfNextEvent;
          
          // Make sure we get the star tracker time also at the boundaries of the pointing slews 
          NTime NextStarTrackerTime = StarTrackerStart->GetTimeOfNextEvent();
          unsigned int NSlews = Pointing->GetNPointingSlews(LastStarTrackerTime, NextStarTrackerTime);
          //cout<<"Slews: "<<NSlews<<" between "<<LastStarTrackerTime<<":"<<NextStarTrackerTime<<endl;
          if (NSlews > 0) {
            NTime SlewTime = Pointing->GetPointingSlewTime(LastStarTrackerTime, NextStarTrackerTime);
            SlewTime -= 0.00000001; // 10 ns before slew
            StarTrackerStart->ForceTimeOfNextEvent(SlewTime);
            //cout<<"New next event time: "<<StarTrackerStart->GetTimeOfNextEvent()<<endl;
          }
          
          // Check whether star tracker or metrology is next:
          if (HasStarTrackerPipe == true && HasMetrologyPipe == true) {
            if (StarTrackerStart->GetTimeOfNextEvent() < MetrologyStart->GetTimeOfNextEvent()) {
              iTimeOfNextEvent = StarTrackerStart->GetTimeOfNextEvent();
              StarTrackerNext = true;
              MetrologyNext = false;
            } else {
              iTimeOfNextEvent = MetrologyStart->GetTimeOfNextEvent();
              StarTrackerNext = false;
              MetrologyNext = true;
            }
          } else if (HasStarTrackerPipe == true && HasMetrologyPipe == false) {
            iTimeOfNextEvent = StarTrackerStart->GetTimeOfNextEvent();
            StarTrackerNext = true;
            MetrologyNext = false;
          } else if (HasStarTrackerPipe == false && HasMetrologyPipe == true) {
            iTimeOfNextEvent = MetrologyStart->GetTimeOfNextEvent();
            StarTrackerNext = false;
            MetrologyNext = true;
          } else {
            // Nothing has to be done... perhaps some satellte data base handling in the future ...
            mout<<"Supervisor: No start modules!"<<endl;
          }
          
          if (StarTrackerNext == false) {
            // Undo the change because it is not yet our time...
            StarTrackerStart->ForceTimeOfNextEvent(NextStarTrackerTime);
          }
          
          if (StarTrackerNext == true) {
            // The initial star tracker module has its own clock running on "star tracker heart beat"
            LastStarTrackerTime = StarTrackerStart->GetTimeOfNextEvent();
            cout<<"Star tracker time: "<<LastStarTrackerTime<<endl;
            StarTrackerData.Clear();
            for (unsigned int s = 0; s < m_StarTrackerModules.size(); ++s) {
              // Do the analysis
              if (m_StarTrackerModules[s]->AnalyzeStarTrackerData(StarTrackerData) == false) {
                mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_StarTrackerModules[s])->GetFullName()<<"\""<<endl;
                break;
              }
            }
          } else if (MetrologyNext == true) {
            // The initial metrology module has its own clock running on "metrology tracker heart beat"
            MetrologyData.Clear();
            for (unsigned int s = 0; s < m_MetrologyModules.size(); ++s) {
              // Do the analysis
              if (m_MetrologyModules[s]->AnalyzeMetrologyData(MetrologyData) == false) {
                mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_MetrologyModules[s])->GetFullName()<<"\""<<endl;
                break;
              }
            }
          } else {
            mout<<"Problem: neither a next star tracker nor a next metrology module."<<endl;
            break;
          }
          
          // Make sure we get the star tracker time also at the boundaries of the pointing slews 
          if (NSlews > 0 && StarTrackerNext == true) {
            NTime SlewTime = Pointing->GetPointingSlewTime(LastStarTrackerTime, NextStarTrackerTime);
            SlewTime += 0.000000005; // 5 ns after slew
            LastStarTrackerTime = SlewTime;
            SlewTime += 0.000000005; // 5 ns after slew
            StarTrackerStart->ForceTimeOfNextEvent(SlewTime);
          }
        }

        // Run the observatory pipelines
        // The initial metrology module has its own clock running on "star tracker heart beat"
        ObservatoryData.Clear();
        for (unsigned int s = 0; s < m_ObservatoryModules.size(); ++s) {
          // Do the analysis
          if (m_ObservatoryModules[s]->AnalyzeObservatoryData(ObservatoryData) == false) {
            mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_ObservatoryModules[s])->GetFullName()<<"\""<<endl;
            break;
          }
        }
      }
      

      EventID++;
      Event.SetID(EventID);
      
      // Loop over the modules
      for (unsigned int m = 0; m < NextPipeline.size(); ++m) {
        
        // Only analyze non-vetoed, triggered events
        if (Event.GetBlocked() == true) {
          NBlockedEvents++;
          break;
        }
        if (Event.GetLostInDeadTime() == true) {
          NLostInDeadTimeEvents++;
          break;
        }
        if (Event.GetVeto() == true) {
          NVetoedEvents++;
          break;
        }
        if (Event.GetEventCut() == true) {
          NCutEvents++;
          break;
        }
        if (Event.GetTrigger() == false) {
          break;
        }
        // Do the analysis
        //cout<<"Current pipe: "<<dynamic_cast<NModule*>(NextPipeline[m])->GetFullName()<<endl;
        if (NextPipeline[m]->AnalyzeEvent(Event) == false) {
          mout<<"Analysis failed for event "<<Event.GetID()<<" ("<<Event.GetOriginString()<<") in module \""<<dynamic_cast<NModule*>(NextPipeline[m])->GetFullName()<<"\""<<endl;
          break;
        }
        
        // Redisplay the GUI...
        gSystem->ProcessEvents();
        
      }
      if (Event.GetBlocked() == false && Event.GetLostInDeadTime() == false && Event.GetVeto() == false && Event.GetEventCut() == false && Event.GetTrigger() == true) {
        NPassedEvents++;
      }
    }
    // Just take care of star tracker or metrology alone
    else {
      if (HasStarTrackerPipe == true && StarTrackerNext == true) {
        // The initial star tracker module has its own clock running on "star tracker heart beat"
        LastStarTrackerTime = StarTrackerStart->GetTimeOfNextEvent();
        StarTrackerData.Clear();
        for (unsigned int s = 0; s < m_StarTrackerModules.size(); ++s) {
          // Do the analysis
          if (m_StarTrackerModules[s]->AnalyzeStarTrackerData(StarTrackerData) == false) {
            mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_StarTrackerModules[s])->GetFullName()<<"\""<<endl;
            break;
          }
        }
      }

      if (HasMetrologyPipe == true && MetrologyNext == true) {
        // The initial metrology module has its own clock running on "star tracker heart beat"
        MetrologyData.Clear();
        for (unsigned int s = 0; s < m_MetrologyModules.size(); ++s) {
          // Do the analysis
          if (m_MetrologyModules[s]->AnalyzeMetrologyData(MetrologyData) == false) {
            mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_MetrologyModules[s])->GetFullName()<<"\""<<endl;
            break;
          }
        }
      }

      if (HasObservatoryPipe == true && OR != 0 && OR->HasEnoughData(0.0) == true) {
        ObservatoryData.Clear();
        for (unsigned int s = 0; s < m_ObservatoryModules.size(); ++s) {
          // Do the analysis
          if (m_ObservatoryModules[s]->AnalyzeObservatoryData(ObservatoryData) == false) {
            mout<<"Analysis failed in module \""<<dynamic_cast<NModule*>(m_ObservatoryModules[s])->GetFullName()<<"\""<<endl;
            break;
          }
        }
      }

    }

    if (EventID % EventBasedUpdateInterval == 0) {
      gSystem->ProcessEvents();
    }


    if (EventID % m_UpdateInterval == 0) {
      gSystem->ProcessEvents();
      if (m_DiagnosticsGUI != 0 && m_DiagnosticsGUI->IsMapped()) {
        m_DiagnosticsGUI->Update();
      }
    }

    if (m_ToggleDiagnostics == true && m_DiagnosticsGUI != 0) {
      m_ToggleDiagnostics = false;
      if (m_DiagnosticsGUI->IsMapped() == true) {
        m_DiagnosticsGUI->UnmapWindow();
      } else {
        m_DiagnosticsGUI->MapWindow();
      }
    }

    if (m_Interrupt == true) break;
  }

  // Finalize the module data:
  cout<<endl;
  cout<<"  Summary"<<endl;
  cout<<"==========="<<endl;
  cout<<endl;
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if ((*Iter).second->Finalize() == false) {
      cout<<"Unable to finalize module: "<<(*Iter).second->GetFullName()<<" - but who cares..."<<endl;
    }
  }

  if (m_DiagnosticsGUI != 0 && m_DiagnosticsGUI->IsMapped()) {
    gSystem->ProcessEvents();
    m_DiagnosticsGUI->Update();
  }

  cout<<endl;
  cout<<"Supervisor summary:"<<endl;
  cout<<endl;
  cout<<"  CPU Timer:                  "<<Timer.GetElapsed()<<" sec"<<endl;
  cout<<"  Observation time:           "<<m_Satellite.GetTimeIdeal().GetSeconds()<<" sec"<<endl;
  cout<<endl;
  cout<<"  Started events:             "<<setw(9)<<EventID+1<<endl;
  cout<<"  Blocked events:             "<<setw(9)<<NBlockedEvents<<" "<<setw(9)<<NBlockedEvents/m_Satellite.GetTimeIdeal().GetSeconds()/2<<" cts/sec/mod"<<endl;
  cout<<"  Lost in dead time events:   "<<setw(9)<<NLostInDeadTimeEvents<<" "<<setw(9)<<NLostInDeadTimeEvents/m_Satellite.GetTimeIdeal().GetSeconds()/2<<" cts/sec/mod"<<endl;
  cout<<"  Vetoed events:              "<<setw(9)<<NVetoedEvents<<" "<<setw(9)<<NVetoedEvents/m_Satellite.GetTimeIdeal().GetSeconds()/2<<" cts/sec/mod"<<endl;
  cout<<"  Cut events:                 "<<setw(9)<<NCutEvents<<" "<<setw(9)<<NCutEvents/m_Satellite.GetTimeIdeal().GetSeconds()/2<<" cts/sec/mod"<<endl;
  cout<<"  Passed events:              "<<setw(9)<<NPassedEvents<<" "<<setw(9)<<NPassedEvents/m_Satellite.GetTimeIdeal().GetSeconds()/2<<" cts/sec/mod"<<endl;
  cout<<endl;

  m_Running = false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NSupervisor::ToggleDiagnosticsWindow()
{
  //! Toggle Diagnostics window

  m_ToggleDiagnostics = true;

  if (m_Running == false && m_DiagnosticsGUI != 0) {
    m_ToggleDiagnostics = false;
    if (m_DiagnosticsGUI->IsMapped() == true) {
      m_DiagnosticsGUI->UnmapWindow();
    } else {
      m_DiagnosticsGUI->MapWindow();
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


vector<NModule*> NSupervisor::GetModuleList(const int ModuleType)
{
  //! Return a list of the available modules of given type

  vector<NModule*> List;
  for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
    for (unsigned int n = 0; n < m_AvailableModules[m]->GetNModulePlacements(); ++n) {
      if (ModuleType == m_AvailableModules[m]->GetModulePlacement(n)) {
        List.push_back(m_AvailableModules[m]);
      }
    }
  }
  
  return List;
}


////////////////////////////////////////////////////////////////////////////////


void NSupervisor::SetActiveModule(NModule* ActiveModule, int ModuleType)
{
  //! Set the active volume for the given type

  if (ModuleType == NModule::c_Unknown) {
    ModuleType = ActiveModule->GetModuleType();
  }

  m_ActiveModules[ModuleType] = ActiveModule;

  GenerateAllLists();
  Validate(ModuleType);
}


////////////////////////////////////////////////////////////////////////////////


void NSupervisor::RemoveActiveModule(const int ModuleType)
{
  //! Set the active volume for the given type

  m_ActiveModules.erase(ModuleType);

  GenerateAllLists();
}


////////////////////////////////////////////////////////////////////////////////


NModule* NSupervisor::GetActiveModule(const int ModuleType)
{
  // Return the active module of type ModuleType
  // Return 0 if there is no such module

  map<int, NModule*>::iterator Iter;
  Iter = m_ActiveModules.find(ModuleType);
  if (Iter != m_ActiveModules.end()) {
    return (*Iter).second;
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////////


NModule* NSupervisor::GetModule(unsigned int i) 
{ 
  //! Return the module at position i of all USED modules --- no error checks are performed  

  if (i < m_Modules.size()) {
    return m_Modules[i];
  }

  return 0; 
}


////////////////////////////////////////////////////////////////////////////////


NModule* NSupervisor::GetAvailableModule(TString Name) 
{ 
  //! Return the modules at position i in the current sequence --- no error checks are performed  

  for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
    if (m_AvailableModules[m]->GetName() == Name) {
      return m_AvailableModules[m];
    }
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////////


NModule* NSupervisor::GetAvailableModuleByXmlTag(TString Name) 
{ 
  //! Return the modules at position i in the current sequence --- no error checks are performed  

  for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
    if (m_AvailableModules[m]->GetXmlTag() == Name) {
      return m_AvailableModules[m];
    }
  }

  return 0;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsSourceModule(int ModuleType)
{
  //! Return true if the given module type is a source module

  if (find(m_SourceSequence.begin(), m_SourceSequence.end(), ModuleType) != m_SourceSequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsBackgroundModule(int ModuleType)
{
  //! Return true if the given module type is a background module

  if (find(m_BackgroundSequence.begin(), m_BackgroundSequence.end(), ModuleType) != m_BackgroundSequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsMetrologyModule(int ModuleType)
{
  //! Return true if the given module type is a metrology module

  if (find(m_MetrologySequence.begin(), m_MetrologySequence.end(), ModuleType) != m_MetrologySequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsStarTrackerModule(int ModuleType)
{
  //! Return true if the given module type is a star tracker module

  if (find(m_StarTrackerSequence.begin(), m_StarTrackerSequence.end(), ModuleType) != m_StarTrackerSequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsObservatoryModule(int ModuleType)
{
  //! Return true if the given module type is a star tracker module

  if (find(m_ObservatorySequence.begin(), m_ObservatorySequence.end(), ModuleType) != m_ObservatorySequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsMergerModule(int ModuleType)
{
  //! Return true if the given module type is a star tracker module

  if (find(m_MergerSequence.begin(), m_MergerSequence.end(), ModuleType) != m_MergerSequence.end()) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::IsGeometryModule(int ModuleType)
{
  //! Return true if the given module type is a geometry module

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::Validate(int ModulePosition)
{
  NModule* Current = GetActiveModule(ModulePosition);
  if (Current == 0) return true;

  if (IsSourceModule(ModulePosition) == true) {
    ValidateSequence(ModulePosition, m_SourceSequence);
  }

  if (IsBackgroundModule(ModulePosition) == true) {
    ValidateSequence(ModulePosition, m_BackgroundSequence);
  }

  if (IsMetrologyModule(ModulePosition) == true) {
    ValidateSequence(ModulePosition, m_MetrologySequence);
  }

  if (IsStarTrackerModule(ModulePosition) == true) {
    ValidateSequence(ModulePosition, m_StarTrackerSequence);
  }

  if (IsObservatoryModule(ModulePosition) == true) {
    ValidateSequence(ModulePosition, m_ObservatorySequence);
  }


//   // Add missing satellite types
//   if (NOrbitSimulators == 0) {
//     for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
//       if (m_AvailableModules[m]->GetNModuleTypes() == 1 &&
//           m_AvailableModules[m]->GetModuleType(0) == NModule::c_OrbitEngine) {
//         m_SatelliteModules.push_back(dynamic_cast<NSatelliteModule*>(m_AvailableModules[m]));
//       }
//     }
//   }
//   if (NOpticsJitterSimulators == 0) {
//     for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
//       if (m_AvailableModules[m]->GetNModuleTypes() == 1 &&
//           m_AvailableModules[m]->GetModuleType(0) == NModule::c_OrientationsEngine) {
//         m_SatelliteModules.push_back(dynamic_cast<NSatelliteModule*>(m_AvailableModules[m]));
//       }
//     }
//   }
//   if (NPointingJitterSimulators == 0) {
//     for (unsigned int m = 0; m < m_AvailableModules.size(); ++m) {
//       if (m_AvailableModules[m]->GetNModuleTypes() == 1 &&
//           m_AvailableModules[m]->GetModuleType(0) == NModule::c_PointingEngine) {
//         m_SatelliteModules.push_back(dynamic_cast<NSatelliteModule*>(m_AvailableModules[m]));
//       }
//     }
//   }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::ValidateSequence(int ModulePosition, vector<int> Sequence)
{
  // ModulePosition has been changed
  // Thus make sure that everything above and below is valid

  // (1) No holes in sequence allowed
  //     If this is a hole - invalidate down until first entry module

  // (2) Only one entry module
  //     If this is an entry module, make sure none is above or below

  // (3) Only one exit module
  //     If this is an entry module, make sure none is above or below

  // (4) Nothing below a saving module

  
  NModule* M = 0;
  NModule* Current = GetActiveModule(ModulePosition);
  if (Current == 0) return true;

  vector<int>::reverse_iterator Up;
  vector<int>::iterator Down;

  if (dynamic_cast<NModuleInterfaceEntry*>(Current) != 0) {
    // (2) Only one entry module - up
    Up = find(Sequence.rbegin(), Sequence.rend(), ModulePosition);
    if (Up != Sequence.rend()) {
      while (++Up != Sequence.rend()) {
        RemoveActiveModule((*Up));
      } while (Up != Sequence.rend());
    }
    // (2) Only one entry module - down
    Down = find(Sequence.begin(), Sequence.end(), ModulePosition);
    while (++Down != Sequence.end()) {
      M = GetActiveModule((*Down));
      if (M == 0) continue;
      if (dynamic_cast<NModuleInterfaceEntry*>(M) != 0) {
        RemoveActiveModule((*Down));
      }
    }
  }
  
  if (dynamic_cast<NModuleInterfaceExit*>(Current) != 0) {
    // (3) Only one entry module - up
    Up = find(Sequence.rbegin(), Sequence.rend(), ModulePosition);
    if (Up != Sequence.rend()) {
      while (++Up != Sequence.rend()) {
        M = GetActiveModule((*Up));
        if (dynamic_cast<NModuleInterfaceExit*>(M) != 0) {
          RemoveActiveModule((*Up));
        }
      } 
    }
    // (3) Only one entry module - down
    Down = find(Sequence.begin(), Sequence.end(), ModulePosition);
    while (++Down != Sequence.end()) {
      RemoveActiveModule((*Down));
    }
  }

  GenerateAllLists();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::ValidateConsistency()
{
  //! Validate the sequence of possible modules for a specific sequence 
  
  // Validate proper entry modules
  if (m_PipelineModules.size() > 0 && dynamic_cast<NModuleInterfaceEntry*>(m_PipelineModules[0]) == 0) {
    mgui<<"Your source pipeline is missing an entry module (either source or event loader)"<<error;
    return false;
  }
  if (m_BackgroundModules.size() > 0 && dynamic_cast<NModuleInterfaceEntry*>(m_BackgroundModules[0]) == 0) {
    mgui<<"Your background pipeline is missing an entry module (either background or event loader)"<<error;
    return false;
  }
  if (m_MetrologyModules.size() > 0 && dynamic_cast<NModuleInterfaceEntry*>(m_MetrologyModules[0]) == 0) {
    mgui<<"Your metrology pipeline is missing an entry module (either a metrology engine or metrology data loader)"<<error;
    return false;
  }
  if (m_StarTrackerModules.size() > 0 && dynamic_cast<NModuleInterfaceEntry*>(m_StarTrackerModules[0]) == 0) {
    if (m_StarTrackerModules.size() == 1 && dynamic_cast<NModuleMetrologySaver*>(m_StarTrackerModules[0]) != 0) {
      // we just have metrology saver here, which is OK
    } else {
      mgui<<"Your star tracker pipeline is missing an entry module (either a star tracker engine or a star tracker data loader)"<<error;
      return false;
    }
  }

  // Check for holes:
  map<int, NModule*>::iterator Iter;
  bool Found = false;
  bool Hole = false;
  for (unsigned int i = 0; i < m_SourceSequence.size(); ++i) {
    if (Found == false) {
      if ((Iter = m_ActiveModules.find(m_SourceSequence[i])) != m_ActiveModules.end()) {
        if (Hole == true) {
          mgui<<"You have a hole in your source pipeline before: "<<(*Iter).second->GetFullName()<<error;
          return false;
        } else {
          Found = true;
        }
      }
    } else {
      if ((Iter = m_ActiveModules.find(m_SourceSequence[i])) == m_ActiveModules.end()) {
        if (IsMergerModule(m_SourceSequence[i]) == false) {
          Hole = true;
          Found = false;
        }
      }
    }    
  }
  Found = false;
  Hole = false;
  for (unsigned int i = 0; i < m_BackgroundSequence.size(); ++i) {
    if (Found == false) {
      if ((Iter = m_ActiveModules.find(m_BackgroundSequence[i])) != m_ActiveModules.end()) {
        if (Hole == true) {
          mgui<<"You have a hole in your background pipeline before: "<<(*Iter).second->GetFullName()<<error;
          return false;
        } else {
          Found = true;
        }
      }
    } else {
      if ((Iter = m_ActiveModules.find(m_BackgroundSequence[i])) == m_ActiveModules.end()) {
        if (IsMergerModule(m_BackgroundSequence[i]) == false) {
          Hole = true;
          Found = false;
        }
      }
    }    
  }
  Found = false;
  Hole = false;
  for (unsigned int i = 0; i < m_StarTrackerSequence.size(); ++i) {
    if (Found == false) {
      if ((Iter = m_ActiveModules.find(m_StarTrackerSequence[i])) != m_ActiveModules.end()) {
        if (Hole == true) {
          mgui<<"You have a hole in your star tracker pipeline before: "<<(*Iter).second->GetFullName()<<error;
          return false;
        } else {
          Found = true;
        }
      }
    } else {
      if ((Iter = m_ActiveModules.find(m_StarTrackerSequence[i])) == m_ActiveModules.end()) {
        Hole = true;
        Found = false;
      }
    }    
  }
  Found = false;
  Hole = false;
  for (unsigned int i = 0; i < m_MetrologySequence.size(); ++i) {
    if (Found == false) {
      if ((Iter = m_ActiveModules.find(m_MetrologySequence[i])) != m_ActiveModules.end()) {
        if (Hole == true) {
          mgui<<"You have a hole in your metrology pipeline before: "<<(*Iter).second->GetFullName()<<error;
          return false;
        } else {
          Found = true;
        }
      }
    } else {
      if ((Iter = m_ActiveModules.find(m_MetrologySequence[i])) == m_ActiveModules.end()) {
        Hole = true;
        Found = false;
      }
    }    
  }
  Found = false;
  Hole = false;
  for (unsigned int i = 0; i < m_ObservatorySequence.size(); ++i) {
    if (Found == false) {
      if ((Iter = m_ActiveModules.find(m_ObservatorySequence[i])) != m_ActiveModules.end()) {
        if (Hole == true) {
          mgui<<"You have a hole in your metrology pipeline before: "<<(*Iter).second->GetFullName()<<error;
          return false;
        } else {
          Found = true;
        }
      }
    } else {
      if ((Iter = m_ActiveModules.find(m_ObservatorySequence[i])) == m_ActiveModules.end()) {
        Hole = true;
        Found = false;
      }
    }    
  }

  // If you have a source and/or background module as well as a metrology and/or star tracker module 
  // you also need a complete observatory pipeline!  
  if (((m_MetrologyModules.size() > 0 || m_StarTrackerModules.size() > 0) &&
       (m_PipelineModules.size() > 0 || m_BackgroundModules.size() > 0)) &&
      m_ObservatoryModules.size() != 1) {
    mgui<<"If you have a source and/or background module as well as a metrology and/or star tracker module you also need a complete observatory pipeline!"<<error;
    return false;
  }
      


  if (m_Satellite.GetOrbitModule() == 0) {
    mgui<<"Your satellite needs an orbit module!"<<error;
    return false;
  }

  if (m_Satellite.GetOrientationsModule() == 0) {
    mgui<<"Your satellite needs an orientations module!"<<error;
    return false;
  }

  if (m_Satellite.GetTimeModule() == 0) {
    mgui<<"Your satellite needs a time module!"<<error;
    return false;
  }

  if (m_Satellite.GetPointingModule() == 0) {
    mgui<<"Your satellite needs a pointing module!"<<error;
    return false;
  }

  if (m_Satellite.GetPointingModule() == 0) {
    mgui<<"Your satellite needs a pointing module!"<<error;
    return false;
  }

  if (m_Satellite.GetGeometryAndDetectorPropertiesModule() == 0) {
    mgui<<"Your satellite needs a geometry and detector properties module!"<<error;
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::GenerateAllLists()
{
  //! This function redetermines the entry modules

  map<int, NModule*>::iterator Iter;

  // Extract source pipeline modules:
  m_PipelineModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_SourceGenerator))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_OpticsEngine))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_ApertureEngine))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_DetectorEngine))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  if (m_PipelineModules.size() != 0 || (Iter = m_ActiveModules.find(int(NModule::c_BackgroundEngine))) == m_ActiveModules.end()) { 
    if ((Iter = m_ActiveModules.find(int(NModule::c_DetectorEffectsEngine))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_TriggerEngine))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_DetectorCalibrator))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) {
      // At the position of the metrology merger a metrology saver might be without event interface
      if (dynamic_cast<NModuleInterfaceEvent*>((*Iter).second) != 0) {
        m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
      }
    }
    if ((Iter = m_ActiveModules.find(int(NModule::c_EventSelector))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_ScienceAnalyzer))) != m_ActiveModules.end()) m_PipelineModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  }

  // Extract background pipeline modules:
  m_BackgroundModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_BackgroundEngine))) != m_ActiveModules.end()) {
    if ((Iter = m_ActiveModules.find(int(NModule::c_BackgroundEngine))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_DetectorEffectsEngine))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_TriggerEngine))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_DetectorCalibrator))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) {
      // At the position of the metrology merger a metrology saver might be without event interface
      if (dynamic_cast<NModuleInterfaceEvent*>((*Iter).second) != 0) {
        m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
      }
    }
    if ((Iter = m_ActiveModules.find(int(NModule::c_EventSelector))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
    if ((Iter = m_ActiveModules.find(int(NModule::c_ScienceAnalyzer))) != m_ActiveModules.end()) m_BackgroundModules.push_back(dynamic_cast<NModuleInterfaceEvent*>((*Iter).second));
  }

  // Extract star tracker modules:
  m_StarTrackerModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_StarTrackerEngine))) != m_ActiveModules.end()) m_StarTrackerModules.push_back(dynamic_cast<NModuleInterfaceStarTracker*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_StarTrackerCalibrator))) != m_ActiveModules.end()) m_StarTrackerModules.push_back(dynamic_cast<NModuleInterfaceStarTracker*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) {
    if (dynamic_cast<NModuleInterfaceStarTracker*>((*Iter).second) != 0) {
      m_StarTrackerModules.push_back(dynamic_cast<NModuleInterfaceStarTracker*>((*Iter).second));
    }
  }

  // Extract metrology modules:
  m_MetrologyModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_MetrologyEngine))) != m_ActiveModules.end()) m_MetrologyModules.push_back(dynamic_cast<NModuleInterfaceMetrology*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_MetrologyCalibrator))) != m_ActiveModules.end()) m_MetrologyModules.push_back(dynamic_cast<NModuleInterfaceMetrology*>((*Iter).second));
  if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) {
    if (dynamic_cast<NModuleInterfaceMetrology*>((*Iter).second) != 0) {
      m_MetrologyModules.push_back(dynamic_cast<NModuleInterfaceMetrology*>((*Iter).second));
    }
  }
  // Extract observatory modules:
  m_ObservatoryModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) m_ObservatoryModules.push_back(dynamic_cast<NModuleInterfaceObservatory*>((*Iter).second));
  //if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryMerger))) != m_ActiveModules.end()) m_ObservatoryModules.push_back(dynamic_cast<NModuleInterfaceObservatory*>((*Iter).second));


  // Extract merger modules:
  m_MergerModules.clear();
  if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryReconstructor))) != m_ActiveModules.end()) {
    if (dynamic_cast<NModuleInterfaceMerger*>((*Iter).second) != 0) {
      m_MergerModules.push_back(dynamic_cast<NModuleInterfaceMerger*>((*Iter).second));
    }
  }
  //if ((Iter = m_ActiveModules.find(int(NModule::c_ObservatoryMerger))) != m_ActiveModules.end()) {
  //  if (dynamic_cast<NModuleInterfaceMerger*>((*Iter).second) != 0) {
  //    m_MergerModules.push_back(dynamic_cast<NModuleInterfaceMerger*>((*Iter).second));
  //  }
  //}

  m_PipelineEntryModules.clear();
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfaceEntry*>((*Iter).second) != 0) {
      m_PipelineEntryModules.push_back(dynamic_cast<NModuleInterfaceEntry*>((*Iter).second));
    }
  }

  m_StopCriterionModules.clear();
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfaceStopCriterion*>((*Iter).second) != 0) {
      m_StopCriterionModules.push_back(dynamic_cast<NModuleInterfaceStopCriterion*>((*Iter).second));
    }
  }

  m_PointingModules.clear();
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfacePointing*>((*Iter).second) != 0) {
      m_PointingModules.push_back(dynamic_cast<NModuleInterfacePointing*>((*Iter).second));
    }
  }

  m_Modules.clear();
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    m_Modules.push_back((*Iter).second);
  }

  if ((Iter = m_ActiveModules.find(int(NModule::c_OrbitEngine))) != m_ActiveModules.end()) {
    m_Satellite.SetOrbitModule(dynamic_cast<NModuleInterfaceOrbit*>((*Iter).second));
  }

  if ((Iter = m_ActiveModules.find(int(NModule::c_OrientationsEngine))) != m_ActiveModules.end()) {
    m_Satellite.SetOrientationsModule(dynamic_cast<NModuleInterfaceOrientations*>((*Iter).second));
  }

  if ((Iter = m_ActiveModules.find(int(NModule::c_TimeEngine))) != m_ActiveModules.end()) {
    m_Satellite.SetTimeModule(dynamic_cast<NModuleInterfaceTime*>((*Iter).second));
  }

  if ((Iter = m_ActiveModules.find(int(NModule::c_PointingEngine))) != m_ActiveModules.end()) {
    m_Satellite.SetPointingModule(dynamic_cast<NModuleInterfacePointing*>((*Iter).second));
  }

  if ((Iter = m_ActiveModules.find(int(NModule::c_GeometryAndDetectorProperties))) != m_ActiveModules.end()) {
    m_Satellite.SetGeometryAndDetectorPropertiesModule(dynamic_cast<NModuleInterfaceGeometryAndDetectorProperties*>((*Iter).second));
  }
  
  // Set the chosen position for IO modules:
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    if (dynamic_cast<NModuleInterfaceIO*>((*Iter).second) != 0) {
      dynamic_cast<NModuleInterfaceIO*>((*Iter).second)->SetChosenModuleType((*Iter).first);
    }
  }


  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::Load(TString FileName)
{
  // Load all data from a file

  Clear();

  // Create a XML document describing the data:
  MXmlDocument* Document = new MXmlDocument();
  Document->Load(FileName);

  int Version = 2;
  MXmlNode* Node = 0;
  MXmlNode* SubNode = 0;

  if ((Node = Document->GetNode("Version")) != 0) {
    Version = Node->GetValueAsInt();
  }

  if ((Node = Document->GetNode("ObservationTime")) != 0) {
    SetObservationTime(NTime(Node->GetValueAsDouble()));
  }

  if ((Node = Document->GetNode("UpdateInterval")) != 0) {
    SetUpdateInterval(Node->GetValueAsInt());
  }

  RemoveAllActiveModules();
  if ((Node = Document->GetNode("ActiveModuleSequence")) != 0) {
    for (unsigned int m = 0; m < Node->GetNNodes(); ++m) {
      MXmlNode* Item = Node->GetNode(m);
      if (Item->GetNode("Position") != 0 && Item->GetNode("Module") != 0) {
        if (NModule::IsUsedType(Item->GetNode("Position")->GetValueAsInt()) == false) {
          cout<<"Removing unused/unavailable module from sequence: "<<Item->GetNode("Module")->GetValue()<<endl;
          continue;
        }
        NModule* M = GetAvailableModuleByXmlTag(Item->GetNode("Module")->GetValue());
        if (M != 0) {
          SetActiveModule(M, Item->GetNode("Position")->GetValueAsInt());
        } else {
          mout<<"Error: Cannot find a module with name: "<<Node->GetNode(m)->GetValue()<<endl;
        }
      }
    }
  }

  if ((Node = Document->GetNode("ModuleOptions")) != 0) {
    for (unsigned int a = 0; a < m_AvailableModules.size(); ++a) {
      MXmlNode* ModuleNode = Node->GetNode(m_AvailableModules[a]->GetXmlTag());
      if (ModuleNode != 0) {
        m_AvailableModules[a]->ReadXmlConfiguration(ModuleNode);
      }
    }
  }

  if ((Node = Document->GetNode("GUIOptions")) != 0) {
    if ((SubNode = Node->GetNode("ButtonSizeX")) != 0) {
      m_ButtonSizeX = SubNode->GetValueAsInt();
    }
    if ((SubNode = Node->GetNode("ButtonSizeY")) != 0) {
      m_ButtonSizeY = SubNode->GetValueAsInt();
    }
    if ((SubNode = Node->GetNode("ButtonMargins")) != 0) {
      m_ButtonMargins = SubNode->GetValueAsInt();
    }
    if ((SubNode = Node->GetNode("ButtonDistanceX")) != 0) {
      m_ButtonDistanceX = SubNode->GetValueAsInt();
    }
    if ((SubNode = Node->GetNode("ButtonDistanceY")) != 0) {
      m_ButtonDistanceY = SubNode->GetValueAsInt();
    }
  }


  delete Document;

  GenerateAllLists();

  return true;
} 


////////////////////////////////////////////////////////////////////////////////


bool NSupervisor::Save(TString FileName)
{
  //! Save all data to a file

  // Create a XML document describing the data:
  MXmlDocument* Document = new MXmlDocument("NuSimData");

  // Version string: 1
  new MXmlNode(Document, "Version", 2);
  new MXmlNode(Document, "NuSIMRevision", g_SVNRevision);

  new MXmlNode(Document, "ObservationTime", m_ObservationTime.GetSeconds());
  new MXmlNode(Document, "UpdateInterval", m_UpdateInterval);

  MXmlNode* ActiveModuleSequence = new MXmlNode(Document, "ActiveModuleSequence");
  map<int, NModule*>::iterator Iter;
  for (Iter = m_ActiveModules.begin(); Iter != m_ActiveModules.end(); ++Iter) {
    MXmlNode* Item = new MXmlNode(ActiveModuleSequence, "ModuleSequenceItem");
    new MXmlNode(Item, "Position", (*Iter).first);
    new MXmlNode(Item, "Module", (*Iter).second->GetXmlTag());
  }

  MXmlNode* ModuleOptions = new MXmlNode(Document, "ModuleOptions");
  for (unsigned int a = 0; a < m_AvailableModules.size(); ++a) {
    MXmlNode* ModuleNode = m_AvailableModules[a]->CreateXmlConfiguration();
    ModuleOptions->AddNode(ModuleNode);
  }

  MXmlNode* GUIOptions = new MXmlNode(Document, "GUIOptions");
  new MXmlNode(GUIOptions, "ButtonSizeX", m_ButtonSizeX); 
  new MXmlNode(GUIOptions, "ButtonSizeY", m_ButtonSizeY); 
  new MXmlNode(GUIOptions, "ButtonMargins", m_ButtonMargins); 
  new MXmlNode(GUIOptions, "ButtonDistanceX", m_ButtonDistanceX); 
  new MXmlNode(GUIOptions, "ButtonDistanceY", m_ButtonDistanceY); 

  // Store the module content
  Document->Save(FileName);

  delete Document;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


int NSupervisor::GetHighestAnalysisLevel() const
{
  //! Get highest analysis level

//   bool FoundEventReconstruction = false;
//   bool FoundStripPairing = false;
//   bool FoundDepthCorrection = false;
//   bool FoundChargeSharingCorrection = false;
//   bool FoundEnergyCalibration = false;
//   bool FoundDetectorEffectsEngine = false;
//   bool FoundSimulator = false;

//   for (unsigned int m = 0; m < m_Modules.size(); ++m) {
//     for (unsigned int t = 0; t < m_Modules[m]->GetNModuleTypes(); ++t) {
//       if (m_Modules[m]->GetModuleType(t) == NModule::c_EventReconstruction) {
//         FoundEventReconstruction = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_StripPairing) {
//         FoundStripPairing = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_DepthCorrection) {
//         FoundDepthCorrection = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_ChargeSharingCorrection) {
//         FoundChargeSharingCorrection = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_EnergyCalibration) {
//         FoundEnergyCalibration = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_DetectorEffectsEngine) {
//         FoundDetectorEffectsEngine = true;
//       } else if (m_Modules[m]->GetModuleType(t) == NModule::c_Simulation) {
//         FoundSimulator = true;
//       } else {
//         merr<<"Not implemented module type: "<<m_Modules[m]->GetModuleType(t)<<endl;
//       }
//     }
//   }

//   if (FoundEventReconstruction == true) {
//     return c_DataReconstructed;
//   } else if (FoundEnergyCalibration == true &&
//              FoundChargeSharingCorrection == true &&
//              FoundDepthCorrection == true &&
//              FoundStripPairing == true) {
//     return c_DataCalibrated;
//   }

  return c_DataRaw;
}


// NSupervisor.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
