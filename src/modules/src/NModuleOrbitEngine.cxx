/*
 * NModuleOrbitEngine.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrbitEngine
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrbitEngine.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsOrbitEngine.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrbitEngine)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitEngine::NModuleOrbitEngine(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceOrbit()
{
  // Construct an instance of NModuleOrbitEngine

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Averaged orbits";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrbitEngine";

  // Set the tool tip text
  m_ToolTip = "This is a orbit module which put the satellite in an average, never changing orbit";

  // Set all types this modules handles
  m_ModuleType = c_OrbitEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOrbitPosition)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOrbitPosition();

  m_OrbitDuration.Set(96.3*minute);
  m_BlackoutDuration.Set(36.5*minute);
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitEngine::~NModuleOrbitEngine()
{
  // Delete this instance of NModuleOrbitEngine
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngine::Initialize()
{
  // Initialize the module 

  //cout<<"Orbit avg. duration: "<<m_OrbitDuration<<" & avg. blackout:"<<m_BlackoutDuration<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


NOrbit NModuleOrbitEngine::GetOrbit(NTime Time)
{
  //! Return the orbit position at a given time

  NOrbit Orbit;
  Orbit.SetAltitude(575);
  Orbit.SetInclination(7.5);
  Orbit.SetLatitude(0);
  Orbit.SetLongitude(0);

  return Orbit;
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngine::StartOfNextBlackout(NTime t)
{
  //! Get the start time of the next black-out 
  
  int Orbit = int(t/m_OrbitDuration);
  NTime TimeInOrbit = (t - m_OrbitDuration*Orbit);
  if (TimeInOrbit < m_OrbitDuration - m_BlackoutDuration) {
    return m_OrbitDuration*(Orbit+1) - m_BlackoutDuration;
  } else {
    return m_OrbitDuration*(Orbit+2) - m_BlackoutDuration;
  }
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngine::EndOfNextBlackout(NTime t)
{
  //! Get the end time of the next black-out

  int Orbit = int(t/m_OrbitDuration);

  return m_OrbitDuration*(Orbit+1);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngine::IsBlackout(NTime t)
{
  //! Return true if we are within blackout

  int Orbit = int(t/m_OrbitDuration);
  NTime TimeInOrbit = (t - m_OrbitDuration*Orbit);
  if (TimeInOrbit < m_OrbitDuration - m_BlackoutDuration) {
    return false;
  } else {
    return true;    
  }
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngine::GetBlackoutDuration(NTime t1, NTime t2)
{
  //! Get the duration of the sum of all blackouts between t1 and t2

  if (IsBlackout(t1) == false) {
    if (IsBlackout(t2) == false) {
      return m_BlackoutDuration * GetNOrbitChanges(t1, t2);
    } else {
      return m_BlackoutDuration * GetNOrbitChanges(t1, t2) + (t2 - StartOfPreviousBlackout(t2));
    }
  } else { //IsBlackout(t1) == true
    if (IsBlackout(t2) == false) {
      return (EndOfNextBlackout(t1) - t1) + m_BlackoutDuration * (GetNOrbitChanges(t1, t2)-1);
    } else {
      if (GetNOrbitChanges(t1, t2) == 0) {
        return t2-t1;
      } else {
        return (EndOfNextBlackout(t1) - t1) + m_BlackoutDuration * (GetNOrbitChanges(t1, t2)-1) + (t2 - StartOfPreviousBlackout(t2));
      }
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


int NModuleOrbitEngine::GetNOrbitChanges(NTime t1, NTime t2)
{
  //! Return the number of orbit chnages:

  return int(t2/m_OrbitDuration) - int(t1/m_OrbitDuration);
}


////////////////////////////////////////////////////////////////////////////////


NTime NModuleOrbitEngine::FindIdealTime(NTime ObservationTime)
{
  //! Get the ideal time as a function of observation time

  int Orbits = ObservationTime/(m_OrbitDuration - m_BlackoutDuration);
  NTime RestTime = ObservationTime - (m_OrbitDuration - m_BlackoutDuration)*Orbits;
  
  //cout<<ObservationTime<<":"<<Orbits<<":"<<RestTime<<endl;
  
  return m_OrbitDuration*Orbits + RestTime;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrbitEngine::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsOrbitEngine* Options = new NGUIOptionsOrbitEngine(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitEngine::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* OrbitDurationNode = Node->GetNode("OrbitDuration");
  if (OrbitDurationNode != 0) {
    m_OrbitDuration.Set(OrbitDurationNode->GetValueAsDouble());
  }
  MXmlNode* BlackoutDurationNode = Node->GetNode("BlackoutDuration");
  if (BlackoutDurationNode != 0) {
    m_BlackoutDuration.Set(BlackoutDurationNode->GetValueAsDouble());
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleOrbitEngine::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  new MXmlNode(Node, "OrbitDuration", m_OrbitDuration.GetAsSeconds());
  new MXmlNode(Node, "BlackoutDuration", m_BlackoutDuration.GetAsSeconds());

  return Node;
}


// NModuleOrbitEngine.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
