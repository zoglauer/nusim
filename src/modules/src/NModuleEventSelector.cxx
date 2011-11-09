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
#include "NGUIOptionsEventSelector.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleEventSelector)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleEventSelector::NModuleEventSelector(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), 
  NModuleInterfaceIO(), NModuleInterfaceEventSaverAscii(Satellite),
  NModuleInterfaceEventSaverLevel2Fits(Satellite),
  NModuleInterfaceEventSaverROOTTree(Satellite),
  NModuleInterfaceEventSaverROOTEnergyResponse(Satellite)
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
  //m_Diagnostics = new NGUIDiagnosticsEventSelector();
  
  m_SaveAsFits = false;
  m_SaveAsROOT = false;
  m_SaveAsResponseROOT = false;
  m_SaveAsDat = false;
  m_SaveBeforeSelections = true;

  m_EnergyMin = 3*keV;
  m_EnergyMax = 80*keV;

  m_DepthMax = 40*mm;

  m_SelectByBadDepthCal = false;
  m_SelectByDepthCut    = false;
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

  m_Counts5To10 = 0.0;
  m_Counts10To20 = 0.0;
  m_Counts20To60 = 0.0;
     

  if (m_SaveAsFits == true) {
    if (OpenLevel2FitsFile(NModuleInterfaceIO::GetBaseFileName() + ".events.fits") == false) return false;
  }
  if (m_SaveAsROOT == true) {
    if (OpenROOTFile(NModuleInterfaceIO::GetBaseFileName() + ".events.root") == false) return false;
  }
  if (m_SaveAsResponseROOT == true) {
    if (OpenEnergyResponseROOTFile(NModuleInterfaceIO::GetBaseFileName() + ".energyresponse.root") == false) return false;
  }
  if (m_SaveAsDat == true) {
    if (OpenAsciiFile(NModuleInterfaceIO::GetBaseFileName() + ".events.dat", m_ASCIIFileVersion, m_ChosenType) == false) return false;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::AnalyzeEvent(NEvent& Event) 
{
  // Main data analysis routine, which updates the event to a new level 

  if (Event.GetNHits() == 0) {
    cerr<<"Error (?): NModuleEventSelector: Event without hits..."<<endl;
    Event.SetEventCut(true);
    return true;
  }

  // The first thing we do is safe the event
  if (m_SaveBeforeSelections == true) {
    if (m_SaveAsFits == true) {
      if (IsLevel2FitsFileOpen() == true) {
        if (SaveEventLevel2Fits(Event) == false) return false;
      } 
    }
    if (m_SaveAsROOT == true) {
      if (IsROOTFileOpen() == true) {
        if (SaveEventTree(Event) == false) return false;
      }
    }
    if (m_SaveAsResponseROOT == true) {
      if (IsEnergyResponseROOTFileOpen() == true) {
        if (SaveEnergyResponse(Event) == false) return false;
      }
    }
    if (m_SaveAsDat == true) {
      if (IsAsciiFileOpen() == true) {
        if (SaveEventAscii(Event, 2) == false) return false;
      }
    }
  }

  // Check if multiple hits in same module in CZT are adjacent
  bool PatternRejection = false;
  if (Event.GetNHits() > 1) {
    bool MultipleTelescopes = false;
    bool MultipleDetectors = false;

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
      }
    }

    if (MultipleTelescopes == true || MultipleDetectors == true) {
      PatternRejection = true;
    }
  }
  

  // Deselect the event if the number of interaction > 2, the total energy > 79 < 6 keV
  double TotalEnergy = 0.0;
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    TotalEnergy += Event.GetHit(i).GetEnergy();
  }

  if (TotalEnergy >= 5 && TotalEnergy < 10) {
    m_Counts5To10++;
  }
  if (TotalEnergy >= 10 && TotalEnergy < 20) {
    m_Counts10To20++;
  }
  if (TotalEnergy >= 20 && TotalEnergy <= 60) {
    m_Counts20To60++;
  }

  if (TotalEnergy > m_EnergyMax || TotalEnergy < m_EnergyMin || PatternRejection == true) {
    Event.SetEventCut(true);
    return true;
  }

  double AverageDepth = 0.0;
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    AverageDepth += (NModule::m_Satellite.GetDetectorHalfDimension().Z() - Event.GetHit(i).GetPosition().Z());
  }
  if (AverageDepth < 0 || AverageDepth/Event.GetNHits() > m_DepthMax) {
    Event.SetEventCut(true);
    return true;
  }


  // Filter out the event with the flag for bad detph calibration
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    if (Event.GetHit(i).GetBadDepthCalibration() == true && m_SelectByBadDepthCal == true) {
      Event.SetEventCut(true);
      return true;
    }
  }


  // Filter out the event with the flag for depth cut
  for (unsigned int i = 0; i < Event.GetNHits(); ++i) {
    if (Event.GetHit(i).GetDepthCut() == true && m_SelectByDepthCut == true) {
      Event.SetEventCut(true);
      return true;
    }
  }


  // The first thing we do is safe the event
  if (m_SaveBeforeSelections == false) {
    if (m_SaveAsFits == true) {
      if (IsLevel2FitsFileOpen() == true) {
        if (SaveEventLevel2Fits(Event) == false) return false;
      } 
    }
    if (m_SaveAsROOT == true) {
      if (IsROOTFileOpen() == true) {
        if (SaveEventTree(Event) == false) return false;
      }  
    }
    if (m_SaveAsResponseROOT == true) {
      if (IsEnergyResponseROOTFileOpen() == true) {
        if (SaveEnergyResponse(Event) == false) return false;
      }
    }
    if (m_SaveAsDat == true) {
      if (IsAsciiFileOpen() == true) {
        if (SaveEventAscii(Event, 2) == false) return false;
      }
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::Finalize()
{
  // Initialize the module 

  if (m_SaveAsFits == true) {
    if (IsLevel2FitsFileOpen() == true) {
      if (CloseLevel2FitsFile() == false) return false;
    } 
  }
  if (m_SaveAsROOT == true) {
    if (IsROOTFileOpen() == true) {
      if (CloseROOTFile() == false) return false;
    } 
  }
  if (m_SaveAsResponseROOT == true) {
    if (IsEnergyResponseROOTFileOpen() == true) {
      if (CloseEnergyResponseROOTFile() == false) return false;
    }
  }
  if (m_SaveAsDat == true) {
    if (IsAsciiFileOpen() == true) {
      if (CloseAsciiFile() == false) return false;
    }
  }
  
  cout<<endl;
  cout<<"Event selector:";
  cout<<endl;
  cout<<"Counts in bands before selections: "<<endl;
  cout<<" 5 - 10 keV: "<<m_Counts5To10<<endl;
  cout<<"10 - 20 keV: "<<m_Counts10To20<<endl;
  cout<<"20 - 60 keV: "<<m_Counts20To60<<endl;

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

  NGUIOptionsEventSelector* Options = new NGUIOptionsEventSelector(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* SaveEventsAsFitsNode = Node->GetNode("SaveEventsAsFits");
  if (SaveEventsAsFitsNode != 0) {
    m_SaveAsFits = SaveEventsAsFitsNode->GetValueAsBoolean();
  }
  MXmlNode* PixelSizeNode = Node->GetNode("PixelSize");
  if (PixelSizeNode != 0) {
    m_PixelSize = PixelSizeNode->GetValueAsDouble();
  }
  MXmlNode* SaveEventsAsDatNode = Node->GetNode("SaveEventsAsDat");
  if (SaveEventsAsDatNode != 0) {
    m_SaveAsDat = SaveEventsAsDatNode->GetValueAsBoolean();
  }
  MXmlNode* SaveEventsAsROOTNode = Node->GetNode("SaveEventsAsROOT");
  if (SaveEventsAsROOTNode != 0) {
    m_SaveAsROOT = SaveEventsAsROOTNode->GetValueAsBoolean();
  }
  MXmlNode* SaveEnergyResponseAsROOTNode = Node->GetNode("SaveEnergyResponseAsROOT");
  if (SaveEnergyResponseAsROOTNode != 0) {
    m_SaveAsResponseROOT = SaveEnergyResponseAsROOTNode->GetValueAsBoolean();
  }
  MXmlNode* SaveBeforeSelectionsNode = Node->GetNode("SaveBeforeSelections");
  if (SaveBeforeSelectionsNode != 0) {
    m_SaveBeforeSelections = SaveBeforeSelectionsNode->GetValueAsBoolean();
  }
  MXmlNode* EnergyMinNode = Node->GetNode("EnergyMin");
  if (EnergyMinNode != 0) {
    m_EnergyMin = EnergyMinNode->GetValueAsDouble();
  }
  MXmlNode* EnergyMaxNode = Node->GetNode("EnergyMax");
  if (EnergyMaxNode != 0) {
    m_EnergyMax = EnergyMaxNode->GetValueAsDouble();
  }
  MXmlNode* DepthMaxNode = Node->GetNode("DepthMax");
  if (DepthMaxNode != 0) {
    m_DepthMax = DepthMaxNode->GetValueAsDouble();
  }
  MXmlNode* SelectByBadDepthCalNode = Node->GetNode("SelectByBadDepthCal");
  if (SelectByBadDepthCalNode != 0) {
    m_SelectByBadDepthCal = SelectByBadDepthCalNode->GetValueAsBoolean();
  }
  MXmlNode* SelectByDepthCutNode = Node->GetNode("SelectByDepthCut");
  if (SelectByDepthCutNode != 0) {
    m_SelectByDepthCut = SelectByDepthCutNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleEventSelector::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  new MXmlNode(Node, "SaveEventsAsFits", m_SaveAsFits);
  new MXmlNode(Node, "PixelSize", m_PixelSize);
  new MXmlNode(Node, "SaveEventsAsDat", m_SaveAsDat);
  new MXmlNode(Node, "SaveEventsAsROOT", m_SaveAsROOT);
  new MXmlNode(Node, "SaveEnergyResponseAsROOT", m_SaveAsResponseROOT);
  new MXmlNode(Node, "SaveBeforeSelections", m_SaveBeforeSelections);
  new MXmlNode(Node, "EnergyMin", m_EnergyMin);
  new MXmlNode(Node, "EnergyMax", m_EnergyMax);
  new MXmlNode(Node, "DepthMax",  m_DepthMax);
  new MXmlNode(Node, "SelectByBadDepthCal", m_SelectByBadDepthCal);
  new MXmlNode(Node, "SelectByDepthCut",    m_SelectByDepthCut);

  return Node;
}


// NModuleEventSelector.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
