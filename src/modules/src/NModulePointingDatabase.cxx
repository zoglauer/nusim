/*
 * NModulePointingDatabase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModulePointingDatabase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModulePointingDatabase.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModulePointingDatabase)
#endif


////////////////////////////////////////////////////////////////////////////////


NModulePointingDatabase::NModulePointingDatabase(NSatellite& Satellite) : NModule(Satellite), NModuleInterfacePointing()
{
  // Construct an instance of NModulePointingDatabase

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Database";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModulePointingDatabase";

  // Set the tool tip text
  m_ToolTip = "This module reads pointing information from a file/database";

  // Set all types this modules handles
  m_ModuleType = c_PointingEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsPointing)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsPointing();
}


////////////////////////////////////////////////////////////////////////////////


NModulePointingDatabase::~NModulePointingDatabase()
{
  // Delete this instance of NModulePointingDatabase
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingDatabase::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


//! If this is a pointing jitter module:
NPointing NModulePointingDatabase::GetPointing(NTime Time) 
{ 
  // Return the pointing of the satellite at a given time

  return NPointing(); 
}



////////////////////////////////////////////////////////////////////////////////


void NModulePointingDatabase::ShowOptionsGUI()
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


bool NModulePointingDatabase::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModulePointingDatabase::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModulePointingDatabase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
