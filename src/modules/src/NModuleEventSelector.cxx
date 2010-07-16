/*
 * NModuleEventSelector.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleEventSelector
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleEventSelector.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptions.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleEventSelector)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleEventSelector::NModuleEventSelector(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent()
{
  // Construct an instance of NModuleEventSelector

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Standard selector";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagEventSelector";

  // Set the tool tip text
  m_ToolTip = "This will be a universal event selector --- currently is does nothing...";

  // Set the module type
  m_ModuleType = c_EventSelector;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsEventSelector)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsEventSelector();
}


////////////////////////////////////////////////////////////////////////////////


NModuleEventSelector::~NModuleEventSelector()
{
  // Delete this instance of NModuleEventSelector
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::Initialize()
{
  // Initialize the module 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

        // Check if multiple hits in same module in CZT are adjacent
  bool PatternRejection = false;
  if (Event.GetNHits() > 1) {
    bool MultipleTelescopes = false;
    bool MultipleDetectors = false;
    bool NotAdjacent = false;

    double xPitch = 2*m_Satellite.GetDetectorHalfDimension().X()/m_Satellite.GetDetectorPixelsX();
    double yPitch = 2*m_Satellite.GetDetectorHalfDimension().Y()/m_Satellite.GetDetectorPixelsY();

    for (unsigned int h1 = 0; h1 < Event.GetNHits(); ++h1) {
      //cout<<Event.GetID()<<" - "<<h1<<": "<<Event.GetHit(h1).GetEnergy()<<":"<<Event.GetHit(h1).GetPosition()<<endl;
    }
    for (unsigned int h1 = 0; h1 < Event.GetNHits(); ++h1) {
      for (unsigned int h2 = h1+1; h2 < Event.GetNHits(); ++h2) {
        if (Event.GetHit(h1).GetTelescope() != Event.GetHit(h2).GetTelescope()) {
          MultipleTelescopes = true;
          //cout<<Event.GetID()<<": Multiple telescopes"<<endl;
        }
        if (Event.GetHit(h1).GetDetector() != Event.GetHit(h2).GetDetector()) {
          MultipleDetectors = true;
          //cout<<Event.GetID()<<": Multiple detectors"<<endl;
        }


        if ((fabs(Event.GetHit(h1).GetPosition().X() - Event.GetHit(h2).GetPosition().X()) > 1.2*xPitch) ||
            (fabs(Event.GetHit(h1).GetPosition().Y() - Event.GetHit(h2).GetPosition().Y()) > 1.2*yPitch)) {
          NotAdjacent = true;
          //cout<<Event.GetID()<<": Not adjacent"<<endl;
          break;
        }
      }
    }

    if (MultipleTelescopes == true || MultipleDetectors == true || NotAdjacent == true) {
      PatternRejection = true;
    }
  }
  



  // Deselect the event if the number of interaction > 2, the total energy > 79 < 6 keV
  double TotalEnergy = 0.0;
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    TotalEnergy += Event.GetHit(i).GetEnergy();
  }
  //cout<<"Energy: "<<TotalEnergy<<endl;

  if (TotalEnergy > 80 || TotalEnergy < 6 || PatternRejection == true) {
    Event.SetEventCut(true);
    //cout<<"Rejected"<<endl;
  } else {
    //cout<<"Accepted"<<endl;
  }


  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleEventSelector::ShowOptionsGUI()
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


bool NModuleEventSelector::ReadXmlConfiguration(MXmlNode* Node)
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


MXmlNode* NModuleEventSelector::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  /*
  MXmlNode* SomeTagNode = new MXmlNode(Node, "SomeTag", "SomeValue");
  */

  return Node;
}


// NModuleEventSelector.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
