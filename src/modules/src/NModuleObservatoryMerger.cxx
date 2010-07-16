/*
 * NModuleObservatoryMerger.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleObservatoryMerger
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleObservatoryMerger.h"

// Standard libs:
#include <algorithm>
using namespace std;

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs
#include "NMetrologyData.h"
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleObservatoryMerger)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleObservatoryMerger::NModuleObservatoryMerger(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceMerger(), NModuleInterfaceObservatory()
{
  // Construct an instance of NModuleObservatoryMerger

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Standard";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagObservatoryMerger";

  // Set the tool tip text
  m_ToolTip = "This is the standard merger of the observatory data into the event data stream.\n"
    "It currently only chooses the closest observatory data set and adds it to the event.";

  // Set all types this modules handles
  m_ModuleType = c_ObservatoryMerger;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsMetrologyMerger)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsMetrologyMerger();
}


////////////////////////////////////////////////////////////////////////////////


NModuleObservatoryMerger::~NModuleObservatoryMerger()
{
  // Delete this instance of NModuleObservatoryMerger
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryMerger::Initialize()
{
  // Initialize the module 

  m_ObservatoryData.clear();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryMerger::AnalyzeObservatoryData(NObservatoryData& Data) 
{
  // Simply add the data to the list

  m_ObservatoryData.push_back(Data);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryMerger::AnalyzeEvent(NEvent& Event) 
{
  // Interpolate the data and add it to the event

  // We currently only grab the closest event

  list<NObservatoryData>::iterator Iter;
  list<NObservatoryData>::iterator LowerBound;
  list<NObservatoryData>::iterator UpperBound;
  Iter = lower_bound(m_ObservatoryData.begin(), m_ObservatoryData.end(), Event.GetTime(), NObservatoryDataTimeComparator());

  if (Iter == m_ObservatoryData.begin()) {
    //merr<<"NModuleObservatoryMerger: No smaller or equal time than "<<Event.GetTime()<<" in list (smallest: "<<m_ObservatoryData.front().GetTime()<<") --- this should not happen..."<<endl;
    // Error handling goes here
    UpperBound = ++Iter;
    LowerBound = Iter;    
  } else if (Iter == m_ObservatoryData.end()) {
    //merr<<"NModuleObservatoryMerger: No bigger or equal time than "<<Event.GetTime()<<" in list (largest: "<<m_ObservatoryData.back().GetTime()<<")--- this should not happen..."<<endl;
    // Error handling goes here
    UpperBound = --Iter;
    LowerBound = --(--Iter);    
  } else {
    UpperBound = Iter;
    LowerBound = --Iter;
  }

  //cout<<"Time (lower bound): "<<(*LowerBound).GetTime()<<endl;
  //cout<<"Time (upper bound): "<<(*UpperBound).GetTime()<<endl;

  // Remove old data, more than 5 elements before the current data set:
  for (int i = 0; i < 5; ++i) {
    if (LowerBound == m_ObservatoryData.begin()) break;
    --LowerBound;
  }
  if (LowerBound != m_ObservatoryData.begin()) {
    m_ObservatoryData.erase(m_ObservatoryData.begin(), LowerBound);
    //cout<<"Erasing up to: "<<(*LowerBound).GetTime()<<" at "<<Event.GetTime()<<endl;
  }

  return true;
}



////////////////////////////////////////////////////////////////////////////////


bool NModuleObservatoryMerger::HasEnoughData(NTime Time)
{
  // Verify that enough data is available

  // It depends on the interpolation mechanism, how much data we must have in 
  // advance and afterwards to make sure we can do various interpolation mechanisms

  // We at least need 5 data sets after the current set to ensure we can do
  // the various (spline) interpolation schemes envisioned
  int MinSetsAfter = 5;

  int SetsAfter = 0; 

  list<NObservatoryData>::reverse_iterator Iter;
  for (Iter = m_ObservatoryData.rbegin(); Iter != m_ObservatoryData.rend(); ++Iter) {
    if ((*Iter).GetTime() >= Time) {
      SetsAfter++;
    } else {
      break;
    }
    if (SetsAfter >= MinSetsAfter) {
      break;
    }
  }

  if (SetsAfter >= MinSetsAfter) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleObservatoryMerger::ShowOptionsGUI()
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


bool NModuleObservatoryMerger::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleObservatoryMerger::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleObservatoryMerger.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
