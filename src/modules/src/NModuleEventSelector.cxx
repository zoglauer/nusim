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
  NModuleInterfaceIO(), NModuleInterfaceEventSaverAscii(),
  NModuleInterfaceEventSaverLevel2Fits(Satellite),
  NModuleInterfaceEventSaverROOTTree(Satellite)
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

  m_EnergyMin = 3;
  m_EnergyMax = 80;

  m_DepthMax = 40;

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

  if (m_FileName != "") {
    if (m_FileName.Last('.') == kNPOS) {
      mgui<<"Unknown file suffix: \""<<m_FileName<<"\". Not saving anything!"<<error;
      return true;
    }

    TString FileBase = m_FileName(0, m_FileName.Last('.'));
    TString FileSuffix = m_FileName(m_FileName.Last('.'), m_FileName.Length() - m_FileName.Last('.'));

    if (FileSuffix.Contains("fits") == false && FileSuffix.Contains(".root") == false && FileSuffix.Contains(".dat") == false) {
      mgui<<"Unknown file suffix: \""<<m_FileName<<"\". Not saving anything!"<<error;   
      return true;
    }

    if (FileSuffix.Contains("fits") == true) {
      m_SaveAsFits = true;
    } else {
      m_SaveAsFits = false;
    }
    
    if (FileSuffix.Contains("root") == true) {
      m_SaveAsROOT = true;
      if  (FileBase.Contains("response", TString::kIgnoreCase) == true) {
	m_SaveAsResponseROOT = true;
      } else {
	m_SaveAsResponseROOT = false;
      }
    } else {
      m_SaveAsROOT = false;
    }
    
    if (FileSuffix.Contains("dat") == true) {
      m_SaveAsDat = true;
    } else {
      m_SaveAsDat = false;
    }
     

    if (m_SaveAsFits == true) {
      if (OpenLevel2FitsFile(FileBase + ".fits") == false) return false;
    } 
    if (m_SaveAsROOT == true) {
      if (OpenROOTFile(FileBase + ".root") == false) return false;
    } 
    if (m_SaveAsDat == true) {
      if (OpenAsciiFile(FileBase + ".dat", m_ChosenType) == false) return false;
    }
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
	if ( m_SaveAsResponseROOT == false ) {
	  if (SaveEventTree(Event) == false) return false;
	} else {
	  if (SaveResponse(Event) == false) return false;
	}
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
	if ( m_SaveAsResponseROOT == false ) {
	  if (SaveEventTree(Event) == false) return false;
	} else {
	  if (SaveResponse(Event) == false) return false;
	}
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
  if (m_SaveAsDat == true) {
    if (IsAsciiFileOpen() == true) {
      if (CloseAsciiFile() == false) return false;
    }
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

  NGUIOptionsEventSelector* Options = new NGUIOptionsEventSelector(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleEventSelector::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* FileNameNode = Node->GetNode("FileName");
  if (FileNameNode != 0) {
    m_FileName = FileNameNode->GetValue();
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
  MXmlNode* SaveBeforeSelectionsNode = Node->GetNode("SaveBeforeSelections");
  if (SaveBeforeSelectionsNode != 0) {
    m_SaveBeforeSelections = SaveBeforeSelectionsNode->GetValueAsBoolean();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleEventSelector::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  new MXmlNode(Node, "FileName", CleanPath(m_FileName));
  new MXmlNode(Node, "EnergyMin", m_EnergyMin);
  new MXmlNode(Node, "EnergyMax", m_EnergyMax);
  new MXmlNode(Node, "DepthMax",  m_DepthMax);
  new MXmlNode(Node, "SelectByBadDepthCal", m_SelectByBadDepthCal);
  new MXmlNode(Node, "SelectByDepthCut",    m_SelectByDepthCut);
  new MXmlNode(Node, "SaveBeforeSelections", m_SaveBeforeSelections);

  return Node;
}


// NModuleEventSelector.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
