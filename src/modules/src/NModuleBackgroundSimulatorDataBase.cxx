/*
 * NModuleBackgroundSimulatorDataBase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleBackgroundSimulatorDataBase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleBackgroundSimulatorDataBase.h"

// Standard libs:

// ROOT libs:
#include "TGClient.h"
#include "TRandom.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MTokenizer.h"

// NuSIM libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleBackgroundSimulatorDataBase)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorDataBase::NModuleBackgroundSimulatorDataBase(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceEvent(), NModuleInterfaceEntry()
{
  // Construct an instance of NModuleBackgroundSimulatorDataBase

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Sample from data base";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagBackgroundSimulatorDataBase";

  // Set the tool tip text
  m_ToolTip = "This module samples the detector background from a events retrieved from a data base.";

  // Set all types this modules handles
  m_ModuleType = c_BackgroundEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsBackgroundSimulatorDataBase)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsBackgroundSimulatorDataBase();

  // Additinal initializations
  m_FileName = "";
}


////////////////////////////////////////////////////////////////////////////////


NModuleBackgroundSimulatorDataBase::~NModuleBackgroundSimulatorDataBase()
{
  // Delete this instance of NModuleBackgroundSimulatorDataBase
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorDataBase::Initialize()
{
  // Initialize the module - since this is an entry module determine the time of the first event

  if (OpenDataBase() == false) return false;
  
  // (Random) time of next event
  m_Time = gRandom->Exp(m_BackgroundTime/m_BackgroundEvents);
  
  if (ReadNextEvent(m_Interactions) == false) return false; 

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorDataBase::AnalyzeEvent(NEvent& Event) 
{
  // From known background distributions (energy, position, generation probability)
  // generate interactions and add them to the event
  // This is an entry module: In a simulation starting from the beginning

  if (m_Interactions.size() == 0) {
    cout<<"NModuleBackgroundSimulatorDataBase: No interactions present!!"<<endl;
    return false;
  }

  Event.SetOrigin(NEvent::c_OriginBackground);

  // Check the telescope - take care if both trigger
  bool Telescope1 = false;
  bool Telescope2 = false;
  for (unsigned int i = 0; i < m_Interactions.size(); ++i) {
    if (m_Interactions[i].GetTelescope() == 1) {
      Telescope1 = true;
    } else {
      Telescope2 = true;
    }
  }

  int SelectedTelescope = 0;
  if (Telescope1 == true && Telescope2 == false) {
    SelectedTelescope = 1;
  } else if (Telescope1 == false && Telescope2 == true) {
    SelectedTelescope = 2;
  } else if (Telescope1 == true && Telescope2 == true) {
    SelectedTelescope = (gRandom->Rndm() > 0.5) ? 1 : 2;
  } else {
    cout<<"NModuleBackgroundSimulatorDataBase: No telescope has interactions!!"<<endl;
    return false;
  }

  if (SelectedTelescope == 1) {
    Event.SetTime(m_Satellite.GetTimeDetector1());
  } else {
    Event.SetTime(m_Satellite.GetTimeDetector2());
  }
  Event.SetTelescope(SelectedTelescope);

  for (unsigned int i = 0; i < m_Interactions.size(); ++i) {
    if (m_Interactions[i].GetTelescope() == SelectedTelescope) {
      Event.AddInteraction(m_Interactions[i]);
    }
  }

  if (Telescope1 == true && Telescope2 == true) {
    // Don't change time or read next event
    // Just erase all interactions in the given telescope
    vector<NInteraction>::iterator Iter = m_Interactions.begin();
    while (Iter != m_Interactions.end()) {
      if ((*Iter).GetTelescope() == SelectedTelescope) {
        Iter = m_Interactions.erase(Iter);
      } else {
        ++Iter;
      }
    }
  } else {
    m_Time += gRandom->Exp(m_BackgroundTime/m_BackgroundEvents);
    // Interaction are clear during read
    if (ReadNextEvent(m_Interactions) == false) return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorDataBase::OpenDataBase()
{
  //! Return true, if the data base could be opened sucessfully

  m_BackgroundEvents = 0;
  m_BackgroundTime = 0.0;

  if (m_DB.is_open() == true) {
    m_DB.close();
    m_DB.clear();
  }

  m_DB.open(m_FileName);
  if (m_DB.is_open() == false) return false;

  //! Read the event number and the total simulation time from the end of the file:

  // Determine file length
  m_DB.seekg(0, ios_base::end);
  streampos DBLength = m_DB.tellg();
  m_DB.seekg(0, ios_base::beg);

  m_DB.clear();
  if (DBLength > streampos(1000)) {
    m_DB.seekg(DBLength - streamoff(1000));
  } else {
    // start from the beginning - where we are already
  }

  TString Line;
  Line.ReadLine(m_DB); // Ignore the first line
  while(!m_DB.eof()) {
    Line.ReadLine(m_DB);
      
    // In case the job crashed badly we might have no TE, thus use the last TI
    if (Line.BeginsWith("TT") == true) {
      MTokenizer Tokens;
      Tokens.Analyze(Line);
      if (Tokens.GetNTokens() < 2) {
        cout<<"Error while opening file "<<m_FileName<<": "<<endl;
        cout<<"Unable to read TI keyword"<<endl;
      } else {
        m_BackgroundTime = Tokens.GetTokenAtAsDouble(1);
      }
    }
    if (Line.BeginsWith("TE") == true) {
      MTokenizer Tokens;
      Tokens.Analyze(Line);
      if (Tokens.GetNTokens() < 2) {
        cout<<"Error while opening file "<<m_FileName<<": "<<endl;
        cout<<"Unable to read TE keyword"<<endl;
      } else {
        m_BackgroundEvents = Tokens.GetTokenAtAsInt(1);
      }
    }
  }


  if (m_BackgroundEvents == 0) {
    cout<<"Error while opening file "<<m_FileName<<": "<<endl;
    cout<<"Total number of background events not found."<<endl;
    return false;
  }

  if (m_BackgroundTime == 0) {
    cout<<"Error while opening file "<<m_FileName<<": "<<endl;
    cout<<"Total time not found."<<endl;
    return false;
  }


  // Wind to random position:
  m_DB.clear();
  m_DB.seekg(streampos(gRandom->Rndm()*DBLength));

  // Find first events
  int RewindCounter = 0;
  do {
    if (m_DB.eof()) {
      m_DB.clear();
      m_DB.seekg(0);
      if (RewindCounter++ == 1) {
        cout<<"Error while opening file "<<m_FileName<<": "<<endl;
        cout<<"No Events found"<<endl;
        return false;        
      }
    }
    Line.ReadLine(m_DB);
  } while (Line.Length() < 2 && Line[0] != 'S' && Line[1] != 'E');


  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorDataBase::ReadNextEvent(vector<NInteraction>& Interactions)
{
  //! Read next event, return false on error

  Interactions.clear();

  const int LineLength = 64;
  char Line[LineLength];
  while (true) {

    if (m_DB.eof()) {
      cout<<"Rewinding background data base file..."<<endl;
      m_DB.clear();
      m_DB.seekg(0);

      do {
        m_DB.getline(Line, LineLength);
      } while (Line[0] != '-');
    }

    m_DB.getline(Line, LineLength); // This needs to be fast 

    if (Line[0] == 'D') {
      MTokenizer T;
      T.AllowComposed(false);
      T.Analyze(Line, false);
      if (T.GetNTokens() != 5) {
        cout<<"Error in file "<<m_FileName<<": "<<endl;
        cout<<"Unable to read DH keyword:"<<endl;
        cout<<Line<<endl;
        return false;
      }
      NInteraction I;
      I.SetIsDetector(true);
      I.SetTelescope(Line[1] - '0');
      I.SetDetector(Line[2] - '0');
      I.SetPosition(MVector(T.GetTokenAtAsDouble(1), T.GetTokenAtAsDouble(2), T.GetTokenAtAsDouble(3)));
      I.SetEnergy(T.GetTokenAtAsDouble(4));
      Interactions.push_back(I);
    } else if (Line[0] == 'S') {
//       int Telescope = 0;
//       double Energy = 0.0;
//       if (sscanf(Line, "S%i %lf", &Telescope, &Energy) != 2) {
//         cout<<"Error in file "<<m_FileName<<": "<<endl;
//         cout<<"Unable to read SH keyword:"<<endl;
//         cout<<Line<<endl;
//         return false;
//       }
//       NInteraction I;
//       I.SetIsDetector(false);
//       I.SetTelescope(Telescope);
//       I.SetEnergy(Energy);
//       Interactions.push_back(I);

      // Very, very unsafe, but very, very fast ...
      NInteraction I;
      I.SetIsDetector(false);
      I.SetTelescope(Line[1] - '0');
      I.SetEnergy(strtod(Line+3, NULL));
      Interactions.push_back(I);
      // cout<<I.GetTelescope()<<":"<<I.GetEnergy()<<endl;
    } else if (Line[0] == '-') {
      if (Interactions.size() > 0) {
        Mixer();
        return true;
      } else {
        // First line ....
      }
    } else if (Line[0] == 'E' && Line[1] == 'N') {
      if (Interactions.size() > 0) {
        Mixer();
        return true;
      } else {
        // Hmmmm
        break;
      }
    }
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorDataBase::Mixer()
{
  // Mix the positions, energies, etc.

  // Step 1: Unbin the data and scale it to the current CZT size:

  double HalfDetectorSizeSimulation = (1.009-0.04) * 10; // --> hard coded from sims!
  double HalfPixelSizeSimulation = HalfDetectorSizeSimulation/32;
  double ScalerX = m_Satellite.GetDetectorHalfDimension().X()/HalfDetectorSizeSimulation;
  double ScalerY = m_Satellite.GetDetectorHalfDimension().Y()/HalfDetectorSizeSimulation;

  // Switch detectors:
  int DetectorOffset = gRandom->Integer(4);
  
  // Mix: Rotate in detector
  int XChanger = 1;
  int YChanger = 1; 
  if (gRandom->Rndm() > 0.5) {
    XChanger = +1;
  } else {
    XChanger = -1;
  }
  if (gRandom->Rndm() > 0.5) {
    YChanger = +1;
  } else {
    YChanger = -1;
  }
  
  MVector Pos;
  for (unsigned int i = 0; i < m_Interactions.size(); ++i) {
    if (m_Interactions[i].IsDetector() == true) {
      m_Interactions[i].SetDetector(((m_Interactions[i].GetDetector()-1 + DetectorOffset) % 4) + 1);
      int Counter = 10;
      do {
        Pos = m_Interactions[i].GetPosition();
        Pos.SetX(XChanger*(ScalerX * (Pos.X() + 2.0*(gRandom->Rndm()-0.5)*HalfPixelSizeSimulation)));
        Pos.SetY(YChanger*(ScalerY * (Pos.Y() + 2.0*(gRandom->Rndm()-0.5)*HalfPixelSizeSimulation)));
      } while ((fabs(Pos.X() + 0.0000001 >= m_Satellite.GetDetectorHalfDimension().X()) || 
                fabs(Pos.Y() + 0.0000001 >= m_Satellite.GetDetectorHalfDimension().Y())) &&
               --Counter > 0); // This can happen due to not enough digits in the sim data

      if (Counter == 0) {
        cout<<"Position trouble: "<<Pos<<" is out of bounds"<<endl;
      }

      m_Interactions[i].SetPosition(Pos);
    }
  }

}


////////////////////////////////////////////////////////////////////////////////


void NModuleBackgroundSimulatorDataBase::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleBackgroundSimulatorDataBase::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleBackgroundSimulatorDataBase::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);

  return Node;
}


// NModuleBackgroundSimulatorDataBase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
