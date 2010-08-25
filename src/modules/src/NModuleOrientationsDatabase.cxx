/*
 * NModuleOrientationsDatabase.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModuleOrientationsDatabase
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModuleOrientationsDatabase.h"

// Standard libs:
#include <fstream>
#include <iostream>
using namespace std;

// ROOT libs:
#include "TGClient.h"

// MEGAlib libs:

// NuSTAR libs:
#include "NGUIOptionsOrientationsDatabase.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModuleOrientationsDatabase)
#endif


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsDatabase::NModuleOrientationsDatabase(NSatellite& Satellite) : NModule(Satellite), NModuleInterfaceOrientations()
{
  // Construct an instance of NModuleOrientationsDatabase

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Database";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModuleOrientationsDatabase";

  // Set the tool tip text
  m_ToolTip = "This module determines the orientations and alignments of optics, apertiure, and metrology\n"
    "However, in this special version everything is read in from a database file.";

  // Set all types this modules handles
  m_ModuleType = c_OrientationsEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsOpticsJitter)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsOpticsJitter();
}


////////////////////////////////////////////////////////////////////////////////


NModuleOrientationsDatabase::~NModuleOrientationsDatabase()
{
  // Delete this instance of NModuleOrientationsDatabase
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::Initialize()
{
  // Initialize the module 
  
  mimp<<"Optice bore sight nameing has to indicate to which this it is relative"<<endl;
  
  //! Reset all arrays:
  m_MetrologyUncertainties.clear();
  m_OpticsUncertainties.clear();
  m_CalibratedAlignments.Clear();
  m_PerturbedAlignments.clear();
  

  m_StartIndexPerturbedAlignments = 0;
  m_TimeWrapPerturbedAlignments = 0.0;
  
  m_StartIndexMetrologyUncertainties = 0;
  m_TimeWrapMetrologyUncertainties = 0.0;
  
  m_StartIndexOpticsUncertainties = 0;
  m_TimeWrapOpticsUncertainties = 0.0;

  
  // Load the data bases
  if (ReadMetrologyDB(m_MetrologyDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_MetrologyUncertainties.size(); ++i) {
  //  cout<<m_MetrologyUncertainties[i].ToString()<<endl;
  //}

  if (ReadOpticsDB(m_OpticsDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_OpticsUncertainties.size(); ++i) {
  //  cout<<m_OpticsUncertainties[i].ToString()<<endl;
  //}

  if (ReadCalibratedAlignmentsDB(m_CalibratedAlignmentsDBFileName) == false) return false;
  //cout<<m_CalibratedAlignments.ToString()<<endl;

  if (ReadPerturbedAlignmentsDB(m_PerturbedAlignmentsDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_PerturbedAlignments.size(); ++i) {
  //  cout<<m_PerturbedAlignments[i].ToString()<<endl;
  //}
  //cout<<"N perturbed entries: "<<m_PerturbedAlignments.size()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadMetrologyDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  // Read the header
  TString Line;

  Line.ReadLine(in); // version
  Line.ReadLine(in); // type
  Line.ReadLine(in); // description
  Line.ReadLine(in); // column title 1
  Line.ReadLine(in); // column title 2
  
  // Read the data
  int LineCounter = 0;
  while (!in.eof()) {
    Line.ReadLine(in);
    
    ++LineCounter;
    if (Line.Length() > 0) {
      
      NMetrologyUncertainties M;
      if (M.ParseDB(Line) == false) {
        mgui<<"Parsing failed: Something is wrong with your metrology data base"<<show;
        in.close();
        return false;            
      }
      m_MetrologyUncertainties.push_back(M);
    }
  } 
  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadOpticsDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  // Read the header
  TString Line;

  Line.ReadLine(in); // version
  Line.ReadLine(in); // type
  Line.ReadLine(in); // description
  Line.ReadLine(in); // column title 1
  Line.ReadLine(in); // column title 2
  
  // Read the data
  while (!in.eof()) {
    Line.ReadLine(in);
    
    if (Line.Length() > 0) {
      if (Line[0] == ',') continue;

      NOpticsUncertainties O;
      if (O.ParseDB(Line) == false) {
        mgui<<"Parsing failed: Something is wrong with your optics data base!"<<show;
        in.close();
        return false;            
      }
      m_OpticsUncertainties.push_back(O);
    }
  } 
  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadCalibratedAlignmentsDB(TString FileName)
{
  //! Read the data from file
  //! For the time being this is identical with the ReadPerturbedAlignmentsDB(TString FileName)
  //! But I don't expect it to stay this way since the detector orientations are missing, 
  //! and I would out which more stuff in here...

  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  // Read the header
  
  // If there is any error in the file, it will always be caught in ParseDB...
  TString Line;
  Line.ReadLine(in); // version
  Line.ReadLine(in); // type
  Line.ReadLine(in); // description
  Line.ReadLine(in); // column title 1
  
  Line.ReadLine(in);
  TString Positions = Line;
      
  Line.ReadLine(in);
  TString Rotations = Line;

  NOrientations O;
  if (O.ParseDB(Positions, Rotations) == false) {
    mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
    in.close();
    return false;            
  }
  m_CalibratedAlignments = O;
 
  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadPerturbedAlignmentsDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  // Read the header
  TString Line;

  Line.ReadLine(in); // version
  Line.ReadLine(in); // type
  Line.ReadLine(in); // description
  Line.ReadLine(in); // column title 1
  Line.ReadLine(in); // column title 2
  
  // Read the data
  TString Positions, Rotations;
  while (!in.eof()) {
    Line.ReadLine(in);
    
    if (Line.Length() > 0) {
      if (Line[0] == ',') {
        cerr<<"Parsing issue (starts with ','): \""<<FileName<<"\""<<endl;
        continue;
      }
      Positions = Line;
      
      Line.ReadLine(in);
    
      if (Line.Length() > 0) {
        if (Line[0] == ',') {
          cerr<<"Parsing issue (starts with ','): \""<<FileName<<"\""<<endl;
          continue;
        }
        Rotations = Line;

        NOrientations O;
        if (O.ParseDB(Positions, Rotations) == false) {
          mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
          in.close();
          return false;            
        }
        m_PerturbedAlignments.push_back(O);
      }
    }
  } 
  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsDatabase::AdvanceTime(const NTime& t)
{
  // Determine orientations if they are not yet present for the given time t
  // All are relative to the focal plane coordinate system
  // Remember distance are in mm!
  
  // Don't do anything if we are at the same time...
  if (m_Time == t) return;


  // Retrieve latest perturbed alignments
  if (t.GetSeconds() >= 0 && m_PerturbedAlignments.size() > 1) {
    if (m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime() + m_TimeWrapPerturbedAlignments > t) {
      m_StartIndexPerturbedAlignments = 0;
      while (m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime() + m_TimeWrapPerturbedAlignments > t) {
        m_TimeWrapPerturbedAlignments -= m_PerturbedAlignments.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndexPerturbedAlignments + 1;
      // If we reached the end of the array, rewind
      if (NextIndex >= m_PerturbedAlignments.size()) {
        NextIndex = 0;
        m_TimeWrapPerturbedAlignments += m_PerturbedAlignments.back().GetTime();
      }
      if (m_PerturbedAlignments[NextIndex].GetTime() + m_TimeWrapPerturbedAlignments > t) {
        break;
      } else {
        m_StartIndexPerturbedAlignments = NextIndex;      
      }
    } while (true);
    // Start index is always smaller the the last one, except when they are identical, then rewind:
    if (m_StartIndexPerturbedAlignments == m_PerturbedAlignments.size() - 1) m_StartIndexPerturbedAlignments = 0; 
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndexPerturbedAlignments = 0;
  }

  if (m_PerturbedAlignments.size() == 1) {
    m_LatestPerturbedAlignments = m_PerturbedAlignments[0];
  } else {
    // We should do interpolations here...
    m_LatestPerturbedAlignments = m_PerturbedAlignments[m_StartIndexPerturbedAlignments];
  }



  // Retrieve latest metrology uncertainties
  if (t.GetSeconds() >= 0 && m_MetrologyUncertainties.size() > 1) {
    if (m_MetrologyUncertainties[m_StartIndexMetrologyUncertainties].GetTime() + m_TimeWrapMetrologyUncertainties > t) {
      m_StartIndexMetrologyUncertainties = 0;
      while (m_MetrologyUncertainties[m_StartIndexMetrologyUncertainties].GetTime() + m_TimeWrapMetrologyUncertainties > t) {
        m_TimeWrapMetrologyUncertainties -= m_MetrologyUncertainties.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndexMetrologyUncertainties + 1;
      // If we reached the end of the array, rewind
      if (NextIndex >= m_MetrologyUncertainties.size()) {
        NextIndex = 0;
        m_TimeWrapMetrologyUncertainties += m_MetrologyUncertainties.back().GetTime();
      }
      if (m_MetrologyUncertainties[NextIndex].GetTime() + m_TimeWrapMetrologyUncertainties > t) {
        break;
      } else {
        m_StartIndexMetrologyUncertainties = NextIndex;      
      }
    } while (true);
    // Start index is always smaller the the last one, except when they are identical, then rewind:
    if (m_StartIndexMetrologyUncertainties == m_MetrologyUncertainties.size() - 1) m_StartIndexMetrologyUncertainties = 0; 
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndexMetrologyUncertainties = 0;
  }

  if (m_MetrologyUncertainties.size() == 1) {
    m_LatestMetrologyUncertainties = m_MetrologyUncertainties[0];
  } else {
    // We should do interpolations here...
    m_LatestMetrologyUncertainties = m_MetrologyUncertainties[m_StartIndexMetrologyUncertainties];
  }




  // Retrieve latest perturbed alignments
  if (t.GetSeconds() >= 0 && m_OpticsUncertainties.size() > 1) {
    if (m_OpticsUncertainties[m_StartIndexOpticsUncertainties].GetTime() + m_TimeWrapOpticsUncertainties > t) {
      m_StartIndexOpticsUncertainties = 0;
      while (m_OpticsUncertainties[m_StartIndexOpticsUncertainties].GetTime() + m_TimeWrapOpticsUncertainties > t) {
        m_TimeWrapOpticsUncertainties -= m_OpticsUncertainties.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndexOpticsUncertainties + 1;
      // If we reached the end of the array, rewind
      if (NextIndex >= m_OpticsUncertainties.size()) {
        NextIndex = 0;
        m_TimeWrapOpticsUncertainties += m_OpticsUncertainties.back().GetTime();
      }
      if (m_OpticsUncertainties[NextIndex].GetTime() + m_TimeWrapOpticsUncertainties > t) {
        break;
      } else {
        m_StartIndexOpticsUncertainties = NextIndex;      
      }
    } while (true);
    // Start index is always smaller the the last one, except when they are identical, then rewind:
    if (m_StartIndexOpticsUncertainties == m_OpticsUncertainties.size() - 1) m_StartIndexOpticsUncertainties = 0; 
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndexOpticsUncertainties = 0;
  }

  if (m_OpticsUncertainties.size() == 1) {
    m_LatestOpticsUncertainties = m_OpticsUncertainties[0];
  } else {
    // We should do interpolations here...
    m_LatestOpticsUncertainties = m_OpticsUncertainties[m_StartIndexOpticsUncertainties];
  }


  m_Time = t;
}


////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsDatabase::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsOrientationsDatabase* Options = new NGUIOptionsOrientationsDatabase(this);
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  MXmlNode* MetrologyDBFileNameNode = Node->GetNode("MetrologyDBFileName");
  if (MetrologyDBFileNameNode != 0) {
    m_MetrologyDBFileName = MetrologyDBFileNameNode->GetValue();
  }
  MXmlNode* OpticsDBFileNameNode = Node->GetNode("OpticsDBFileName");
  if (OpticsDBFileNameNode != 0) {
    m_OpticsDBFileName = OpticsDBFileNameNode->GetValue();
  }
  MXmlNode* CalibratedAlignmentsDBFileNameNode = Node->GetNode("CalibratedAlignmentsDBFileName");
  if (CalibratedAlignmentsDBFileNameNode != 0) {
    m_CalibratedAlignmentsDBFileName = CalibratedAlignmentsDBFileNameNode->GetValue();
  }
  MXmlNode* PerturbedAlignmentsDBFileNameNode = Node->GetNode("PerturbedAlignmentsDBFileName");
  if (PerturbedAlignmentsDBFileNameNode != 0) {
    m_PerturbedAlignmentsDBFileName = PerturbedAlignmentsDBFileNameNode->GetValue();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModuleOrientationsDatabase::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  new MXmlNode(Node, "MetrologyDBFileName", m_MetrologyDBFileName);
  new MXmlNode(Node, "OpticsDBFileName", m_OpticsDBFileName);
  new MXmlNode(Node, "CalibratedAlignmentsDBFileName", m_CalibratedAlignmentsDBFileName);
  new MXmlNode(Node, "PerturbedAlignmentsDBFileName", m_PerturbedAlignmentsDBFileName);

  return Node;
}


// NModuleOrientationsDatabase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
