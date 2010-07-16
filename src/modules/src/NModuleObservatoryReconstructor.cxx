/*
 * NModuleObservatoryReconstructor.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleObservatoryReconstructor
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleObservatoryReconstructor.h"

// Standard libs:
#include <algorithm>
using namespace std;

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptions.h"
#include "NObservatoryReconstructor.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleObservatoryReconstructor)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleObservatoryReconstructor::NModuleObservatoryReconstructor(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceMerger(), NModuleInterfaceStarTracker(), NModuleInterfaceMetrology(), NModuleInterfaceObservatory()
{
  // Construct an instance of NModuleObservatoryReconstructor

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "NuSTAR default";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagObservatoryReconstructor";

  // Set the tool tip text
  m_ToolTip = "This is the standard reconstructor for observatory data.\n"
    "It reconstructs metrology and star tracker data, and calculates the aspect of the telescopes at event observation time.";

  // Set all types this modules handles
  m_ModuleType = c_ObservatoryReconstructor;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsStarTrackerMerger)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsStarTrackerMerger();
}


////////////////////////////////////////////////////////////////////////////////


NModuleObservatoryReconstructor::~NModuleObservatoryReconstructor()
{
  // Delete this instance of NModuleObservatoryReconstructor
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::Initialize()
{
  // Initialize the module 

  m_StarTrackerData.clear();
  m_MetrologyData.clear();

  // We probably will have to find an agreeable value here...
  m_Time = -0.00001;

  m_ObservatoryReconstructor.SetCalibratedOrientationStarTrackerRelOpticalBench(m_Satellite.GetCalibratedOrientationStarTrackerRelOpticalBench(4));
  m_ObservatoryReconstructor.SetCalibratedOrientationOpticalBench(m_Satellite.GetCalibratedOrientationOpticalBench());
  m_ObservatoryReconstructor.SetCalibratedOrientationOpticsRelOpticalBench(m_Satellite.GetCalibratedOrientationOpticsRelOpticalBench(1), m_Satellite.GetCalibratedOrientationOpticsRelOpticalBench(2));
  m_ObservatoryReconstructor.SetCalibratedPointingOpticalAxisRelOM(m_Satellite.GetCalibratedPointingOpticalAxisRelOM(1), m_Satellite.GetCalibratedPointingOpticalAxisRelOM(2));
  m_ObservatoryReconstructor.SetCalibratedOrientationMetrologyLaserRelOpticalBench(m_Satellite.GetCalibratedOrientationMetrologyLaserRelOpticalBench(1), m_Satellite.GetCalibratedOrientationMetrologyLaserRelOpticalBench(2));
  m_ObservatoryReconstructor.SetCalibratedPointingMetrologyLaserRelML(m_Satellite.GetCalibratedPointingMetrologyLaserRelML(1), m_Satellite.GetCalibratedPointingMetrologyLaserRelML(2));
  m_ObservatoryReconstructor.SetCalibratedOrientationMetrologyDetectors(m_Satellite.GetCalibratedOrientationMetrologyDetector(1), m_Satellite.GetCalibratedOrientationMetrologyDetector(2));
  m_ObservatoryReconstructor.SetCalibratedOrientationFocalPlaneModules(m_Satellite.GetCalibratedOrientationFocalPlaneModule(1), m_Satellite.GetCalibratedOrientationFocalPlaneModule(2));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::AnalyzeStarTrackerData(NStarTrackerData& Data) 
{
  // Simply add the data to the list

  m_StarTrackerData.push_back(Data);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::AnalyzeMetrologyData(NMetrologyData& Data) 
{
  // Simply add the data to the list

  m_MetrologyData.push_back(Data);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::AnalyzeEvent(NEvent& Event) 
{
  // Reconstruct alignments, aspect and pointing...

  // Step 1: Interpolate at event time
  NTime Time = Event.GetTime();

  // Step 1.1: Metrology data

  mimp<<"Interpolations are dummies & the implementation is very inefficient!"<<show;

  // Find the first data set after Time in the Metrology data set stream:
  list<NMetrologyData>::iterator IterMetrology;
  list<NMetrologyData>::iterator LowerBoundMetrology;
  list<NMetrologyData>::iterator UpperBoundMetrology;

  IterMetrology = upper_bound(m_MetrologyData.begin(), m_MetrologyData.end(), Time, NMetrologyDataTimeComparator());

  if (IterMetrology == m_MetrologyData.begin()) {
    merr<<"NModuleObservatoryReconstructor: No smaller or equal time than "
        <<Time<<" in metrology list (smallest: "<<m_MetrologyData.front().GetTime()
        <<")--- this should not happen, interpolation will not work. Extrapolating..."<<endl;
    // Error handling goes here
    UpperBoundMetrology = ++IterMetrology;
    LowerBoundMetrology = IterMetrology;    
  } else if (IterMetrology == m_MetrologyData.end()) {
    merr<<"NModuleObservatoryReconstructor: No bigger or equal time than "
        <<Time<<" in metrology list (largest: "<<m_MetrologyData.back().GetTime()
        <<")--- this should not happen, interpolation will not work. Extrapolating......"<<endl;
    // Error handling goes here
    UpperBoundMetrology = --IterMetrology;
    LowerBoundMetrology = --(--IterMetrology);    
  } else {
    UpperBoundMetrology = IterMetrology;
    LowerBoundMetrology = --IterMetrology;
  }

  // Remove old data, more than 5 elements before the current data set:
  IterMetrology = LowerBoundMetrology;
  for (int i = 0; i < 5; ++i) {
    if (IterMetrology == m_MetrologyData.begin()) break;
    --IterMetrology;
  }
  if (IterMetrology != m_MetrologyData.begin()) {
    m_MetrologyData.erase(m_MetrologyData.begin(), IterMetrology);
    //cout<<"Erasing up to: "<<(*LowerBoundMetrology).GetTime()<<" at "<<Event.GetTime()<<endl;
  }


  // Find the first data set after Time in the StarTracker data set stream:
  list<NStarTrackerData>::iterator IterStarTracker;
  list<NStarTrackerData>::iterator LowerBoundStarTracker;
  list<NStarTrackerData>::iterator UpperBoundStarTracker;

  IterStarTracker = upper_bound(m_StarTrackerData.begin(), m_StarTrackerData.end(), Time, NStarTrackerDataTimeComparator());

  if (IterStarTracker == m_StarTrackerData.begin()) {
    merr<<"NModuleObservatoryReconstructor: No smaller or equal time than "
        <<Time<<" in star tracker list (smallest: "<<m_StarTrackerData.front().GetTime()
        <<")--- this should not happen, interpolation will not work. Extrapolating..."<<endl;
    // Error handling goes here
    UpperBoundStarTracker = ++IterStarTracker;
    LowerBoundStarTracker = IterStarTracker;    
  } else if (IterStarTracker == m_StarTrackerData.end()) {
    merr<<"NModuleObservatoryReconstructor: No bigger or equal time than "
        <<Time<<" in star tracker list (largest: "<<m_StarTrackerData.back().GetTime()
        <<")--- this should not happen, interpolation will not work. Extrapolating......"<<endl;
    // Error handling goes here
    UpperBoundStarTracker = --IterStarTracker;
    LowerBoundStarTracker = --(--IterStarTracker);    
  } else {
    UpperBoundStarTracker = IterStarTracker;
    LowerBoundStarTracker = --IterStarTracker;
  }

  // Remove old data, more than 5 elements before the current data set:
  IterStarTracker = LowerBoundStarTracker;
  for (int i = 0; i < 5; ++i) {
    if (IterStarTracker == m_StarTrackerData.begin()) break;
    --IterStarTracker;
  }
  if (IterStarTracker != m_StarTrackerData.begin()) {
    m_StarTrackerData.erase(m_StarTrackerData.begin(), IterStarTracker);
    //cout<<"Erasing up to: "<<(*LowerBoundStarTracker).GetTime()<<" at "<<Event.GetTime()<<endl;
  }
  // We have to do the same for the star tracker data...


  
  // Now do the same with the observatory reconstructor
  m_ObservatoryReconstructor.SetTime(Time);
  m_ObservatoryReconstructor.SetStarTrackerData((*LowerBoundStarTracker), (*UpperBoundStarTracker));
  m_ObservatoryReconstructor.SetMetrologyData((*LowerBoundMetrology), (*UpperBoundMetrology));

  NOrientation OB = m_Satellite.GetOrientationOpticalBench(Time);  // for diagnostics
  
  for (unsigned int h = 0; h < Event.GetNHits(); ++h) {
    m_ObservatoryReconstructor.Reconstruct(Event.GetHitRef(h));
    //cout << "OB =" << OB.ToString() << endl;  // for diagnostics
  }
  
  // For debugging add all current orientations to the event
  // Event.SetOrientations(m_Satellite.GetAllOrientations(Event.GetTime()));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::HasEnoughData(NTime Time)
{
  // Verify that enough data is available

  // It depends on the interpolation mechanism, how much data we must have in 
  // advance and afterwards to make sure we can do various interpolation mechanisms

  // We at least need 5 data sets after the current set to ensure we can do
  // the various (spline) interpolation schemes envisioned
  int MinSetsAfter = 5;

  int SetsAfterST = 0; 
  list<NStarTrackerData>::reverse_iterator IterST;
  for (IterST = m_StarTrackerData.rbegin(); IterST != m_StarTrackerData.rend(); ++IterST) {
    if ((*IterST).GetTime() >= Time) {
      SetsAfterST++;
    } else {
      break;
    }
    if (SetsAfterST >= MinSetsAfter) {
      break;
    }
  }

  int SetsAfterME = 0; 
  list<NMetrologyData>::reverse_iterator IterME;
  for (IterME = m_MetrologyData.rbegin(); IterME != m_MetrologyData.rend(); ++IterME) {
    if ((*IterME).GetTime() >= Time) {
      SetsAfterME++;
    } else {
      break;
    }
    if (SetsAfterME >= MinSetsAfter) {
      break;
    }
  }

  if (SetsAfterST >= MinSetsAfter && SetsAfterME >= MinSetsAfter) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleObservatoryReconstructor::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsTemplate* Options = new NGUIOptionsTemplate(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryReconstructor::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode.GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleObservatoryReconstructor::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleObservatoryReconstructor.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
