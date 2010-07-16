/*
 * NModuleBackprojector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackprojector
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackprojector.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"
#include "NGUIDiagnosticsBackprojector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackprojector)
#endif


////////////////////////////////////////////////////////////////////////////////


  NModuleBackprojector::NModuleBackprojector(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleBackprojector

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Backprojector";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackprojector";

  // Set the tool tip text
  m_ToolTip = "Performs a simple backprojection of the event.";

  // Set all types this modules handles
  m_ModuleType = c_ScienceAnalyzer;

  // Set if this module has a diagnostics GUI
  // Overwrite ShowOptionsGUI() with the call to the GUI!
  m_HasDiagnosticsGUI = true;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackprojector)
  // and implement all your GUI options
  m_Diagnostics = 0;
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackprojector::~NModuleBackprojector()
{
  // Delete this instance of NModuleBackprojector

  delete m_Diagnostics;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::Initialize()
{
  // Initialize the module 

  delete m_Diagnostics;
  m_Diagnostics = new NGUIDiagnosticsBackprojector();
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetDetectorParameters(m_Satellite.GetOrientationDetectorRelFocalPlaneModule(0, 1, 1).GetTranslation(), 
                                                                                    m_Satellite.GetDetectorHalfDimension(), 
                                                                                    m_Satellite.GetDetectorPixelsX(),
                                                                                    m_Satellite.GetDetectorPixelsY());

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  // Backproject the event...

  if (Event.GetOriginalPhoton().IsEmpty() == false) {
    dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->AddInitialDirection(-Event.GetOriginalPhoton().GetDirection());
  }

  // Do a trivial backprojection ;-)
  double Energy = 0.0;
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    // This function goes along with the trivial optics engine:
    // A position 0/0 means on axis focal spot
    // For each 0.3 mm deviation corresponds to 1 arcmin distance
    
    MVector Pos = Event.GetHit(i).GetPosition(); // Position in in focal plane module
    // Randomize within pixel:
    Pos[0] += m_Satellite.GetDetectorAveragePixelSizeX()*(gRandom->Rndm()-0.5);
    Pos[1] += m_Satellite.GetDetectorAveragePixelSizeY()*(gRandom->Rndm()-0.5);

    double RadiusInArcmin = sqrt(Pos.X()*Pos.X() + Pos.Y()*Pos.Y())/3;
    MVector Origin(-Pos.X(), -Pos.Y(), 1000.0);
    //cout<<"Origin: "<<Origin.Theta()*c_Deg<<":"<<Origin.Phi()*c_Deg<<endl;
    Origin.SetTheta(RadiusInArcmin/60*c_Rad);
    //cout<<"Origin: "<<RadiusInArcmin<<"  -->"<<RadiusInArcmin/60*c_Rad<<"  -->"<<Origin.Theta()*c_Deg<<":"<<Origin.Phi()*c_Deg<<endl;

    Energy += Event.GetHit(i).GetEnergy();

    dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->AddBackprojection(Origin);
  }
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->AddEnergy(Energy);
  dynamic_cast<NGUIDiagnosticsBackprojector*>(m_Diagnostics)->SetTime(Event.GetTime());

  //cout<<"Energy: "<<Energy<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackprojector::ShowOptionsGUI()
{
  //! Show the options GUI 

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptions* Options = new NGUIOptions(this);

  // with something like:
  // NGUIOptionsBackprojector* Options = new NGUIOptionsBackprojector(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackprojector::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleBackprojector::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleBackprojector.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
