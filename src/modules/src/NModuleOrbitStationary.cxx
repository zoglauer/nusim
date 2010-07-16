/*
 * NModuleOrbitStationary.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrbitStationary
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrbitStationary.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrbitStationary)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitStationary::NModuleOrbitStationary(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceOrbit()
{
  // Construct an instance of NModuleOrbitStationary

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Miraculously stationary ";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrbitStationary";

  // Set the tool tip text
  m_ToolTip = "This is a orbit module which put the satellite in a miraculously stationary, at the equator at 575 km.";

  // Set all types this modules handles
  m_ModuleType = c_OrbitEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOrbitPosition)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOrbitPosition();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrbitStationary::~NModuleOrbitStationary()
{
  // Delete this instance of NModuleOrbitStationary
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrbitStationary::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


NOrbit NModuleOrbitStationary::GetOrbit(NTime Time)
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


void NModuleOrbitStationary::ShowOptionsGUI()
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


bool NModuleOrbitStationary::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleOrbitStationary::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleOrbitStationary.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
