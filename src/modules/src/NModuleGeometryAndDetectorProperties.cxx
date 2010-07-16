/*
 * NModuleGeometryAndDetectorProperties.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleGeometryAndDetectorProperties
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleGeometryAndDetectorProperties.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:
#include "MFile.h"

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleGeometryAndDetectorProperties)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleGeometryAndDetectorProperties::NModuleGeometryAndDetectorProperties(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceGeometryAndDetectorProperties()
{
  // Construct an instance of NModuleGeometryAndDetectorProperties

  // Set all module relevant information

  // Set the module name --- make sure that "ModuleTypeName + m_Name" is unique
  m_Name = "Default detector characteristics";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagDefaultDetectorCharacteristics";

  // Set the tool tip text
  m_ToolTip = "This module contains all geometry and detectors.";

  // Set the module type
  m_ModuleType = c_GeometryAndDetectorProperties;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsGeometryAndDetectorProperties)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsGeometryAndDetectorProperties();

  // Add additional initializations here:
}


////////////////////////////////////////////////////////////////////////////////


NModuleGeometryAndDetectorProperties::~NModuleGeometryAndDetectorProperties()
{
  // Delete this instance of NModuleGeometryAndDetectorProperties
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleGeometryAndDetectorProperties::Initialize()
{
  // Initialize the module 

  if (LoadCrossSections() == false) return false;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleGeometryAndDetectorProperties::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsGeometryAndDetectorProperties* Options = new NGUIOptionsGeometryAndDetectorProperties(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleGeometryAndDetectorProperties::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  /*
  MXmlNode* SomeTagNode = Node->GetNode("SomeTag");
  if (SomeTagNode != 0) {
    m_SomeTagValue = SomeTagNode->GetValue();
  }
  */

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleGeometryAndDetectorProperties::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleGeometryAndDetectorProperties.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
