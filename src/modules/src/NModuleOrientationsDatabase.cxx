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
  
  // Reset all arrays:
  //m_CalibratedMetrologyUncertainties.clear();
  m_PerturbedMetrologyUncertainties.clear();
  //m_CalibratedOpticsUncertainties.clear();
  m_PerturbedOpticsUncertainties.clear();
  //m_CalibratedAlignments.Clear();
  m_PerturbedAlignments.clear();
  

  m_StartIndexPerturbedAlignments = 0;
  m_TimeWrapPerturbedAlignments = 0.0;
  
  m_StartIndexPerturbedMetrologyUncertainties = 0;
  m_TimeWrapPerturbedMetrologyUncertainties = 0.0;
  
  m_StartIndexPerturbedOpticsUncertainties = 0;
  m_TimeWrapPerturbedOpticsUncertainties = 0.0;

  
  // Load the data bases
  if (MFile::Exists(m_CalibratedMetrologyDBFileName) == false) {
    mgui<<"Calibrated metrology DB does not exist!"<<error;
    return false;
  }
  if (ReadCalibratedMetrologyDB(m_CalibratedMetrologyDBFileName) == false) return false;
  //cout<<m_CalibratedMetrologyUncertainties.ToString()<<endl;

  
  if (MFile::Exists(m_PerturbedMetrologyDBFileName) == false) {
    mgui<<"Perturbed metrology DB does not exist!"<<error;
    return false;
  }
  if (ReadPerturbedMetrologyDB(m_PerturbedMetrologyDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_PerturbedMetrologyUncertainties.size(); ++i) {
  //  cout<<m_PerturbedMetrologyUncertainties[i].ToString()<<endl;
  //}

  if (MFile::Exists(m_CalibratedOpticsDBFileName) == false) {
    mgui<<"Calibrated optics DB does not exist!"<<error;
    return false;
  }
  if (ReadCalibratedOpticsDB(m_CalibratedOpticsDBFileName) == false) return false;
  //cout<<m_CalibratedOpticsUncertainties.ToString()<<endl;

  if (MFile::Exists(m_PerturbedOpticsDBFileName) == false) {
    mgui<<"Perturbed optics DB does not exist!"<<error;
    return false;
  }
  if (ReadPerturbedOpticsDB(m_PerturbedOpticsDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_PerturbedOpticsUncertainties.size(); ++i) {
  //  cout<<m_PerturbedOpticsUncertainties[i].ToString()<<endl;
  //}
  
  if (MFile::Exists(m_CalibratedAlignmentsDBFileName) == false) {
    mgui<<"Calibrated alignments DB does not exist!"<<error;
    return false;
  }
  if (ReadCalibratedAlignmentsDB(m_CalibratedAlignmentsDBFileName) == false) return false;
  //cout<<m_CalibratedAlignments.ToString()<<endl;

  if (MFile::Exists(m_PerturbedAlignmentsDBFileName) == false) {
    mgui<<"Perturbed alignments DB does not exist!"<<error;
    return false;
  }
  if (ReadPerturbedAlignmentsDB(m_PerturbedAlignmentsDBFileName) == false) return false;
  //for (unsigned int i = 0; i < m_PerturbedAlignments.size(); ++i) {
  //  cout<<m_PerturbedAlignments[i].ToString()<<endl;
  //}
  //cout<<"N perturbed entries: "<<m_PerturbedAlignments.size()<<endl;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadCalibratedMetrologyDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  MFile::ExpandFileName(FileName);
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
  Line.ReadLine(in);
  if (m_CalibratedMetrologyUncertainties.ParseDB(Line) == false) {
    mgui<<"Parsing failed: Something is wrong with your calibrated metrology data base"<<show;
    in.close();
    return false;            
  }
      
  in.close();
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadPerturbedMetrologyDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  MFile::ExpandFileName(FileName);
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
  bool Found = false;
  while (!in.eof()) {
    Line.ReadLine(in);
    
    ++LineCounter;
    if (Line.Length() > 0) {
      
      NMetrologyDBEntry M;
      if (M.ParseDB(Line) == false) {
        mgui<<"Parsing failed: Something is wrong with your perturbed metrology data base"<<show;
        in.close();
        return false;            
      }
      m_PerturbedMetrologyUncertainties.push_back(M);
      Found = true;
    }
  } 
  in.close();

  if (Found == false) {
    mgui<<"No entries found in your perturbed metrology data base"<<show;
    return false;
  }
  
  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadCalibratedOpticsDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  MFile::ExpandFileName(FileName);
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
  Line.ReadLine(in);
  if (m_CalibratedOpticsUncertainties.ParseDB(Line) == false) {
    mgui<<"Parsing failed: Something is wrong with your calibrated optics data base!"<<show;
    in.close();
    return false;            
  }

  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModuleOrientationsDatabase::ReadPerturbedOpticsDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  MFile::ExpandFileName(FileName);
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
  bool Found = false;
  while (!in.eof()) {
    Line.ReadLine(in);
    
    if (Line.Length() > 0) {
      if (Line[0] == ',') continue;

      NOpticsDBEntry O;
      if (O.ParseDB(Line) == false) {
        mgui<<"Parsing failed: Something is wrong with your perturbed optics data base!"<<show;
        in.close();
        return false;            
      }
      m_PerturbedOpticsUncertainties.push_back(O);
      Found = true;
    }
  } 
  in.close();

  if (Found == false) {
    mgui<<"No entries found in your perturbed optics data base"<<show;
    return false;
  }

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
  MFile::ExpandFileName(FileName);
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

  NAlignmentsDBEntry O;
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
  MFile::ExpandFileName(FileName);
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
  bool Found = false;
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

        NAlignmentsDBEntry O;
        if (O.ParseDB(Positions, Rotations) == false) {
          mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
          in.close();
          return false;            
        }
        m_PerturbedAlignments.push_back(O);
        Found = true;
      }
    }
  } 
  in.close();

  if (Found == false) {
    mgui<<"No entries found in your perturbed alignments data base"<<show;
    return false;
  }

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

  bool Interpolate = false;

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
    if (Interpolate == true) {
      double Fraction = (t.GetSeconds() - m_TimeWrapPerturbedAlignments.GetSeconds() - m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime().GetSeconds())/
        (m_PerturbedAlignments[m_StartIndexPerturbedAlignments+1].GetTime().GetSeconds() - m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime().GetSeconds());
        
      if (Fraction > 1) {
        merr<<"Time fraction for data base interpolation larger than 1!"<<show;
        Fraction = 1;
      }

      m_LatestPerturbedAlignments.SetInterpolated(m_PerturbedAlignments[m_StartIndexPerturbedAlignments], 
                                                  m_PerturbedAlignments[m_StartIndexPerturbedAlignments+1], Fraction);
    } else {
      m_LatestPerturbedAlignments = m_PerturbedAlignments[m_StartIndexPerturbedAlignments];
    }
  }



  // Retrieve latest metrology uncertainties
  if (t.GetSeconds() >= 0 && m_PerturbedMetrologyUncertainties.size() > 1) {
    if (m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime() + m_TimeWrapPerturbedMetrologyUncertainties > t) {
      m_StartIndexPerturbedMetrologyUncertainties = 0;
      while (m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime() + m_TimeWrapPerturbedMetrologyUncertainties > t) {
        m_TimeWrapPerturbedMetrologyUncertainties -= m_PerturbedMetrologyUncertainties.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndexPerturbedMetrologyUncertainties + 1;
      // If we reached the end of the array, rewind
      if (NextIndex >= m_PerturbedMetrologyUncertainties.size()) {
        NextIndex = 0;
        m_TimeWrapPerturbedMetrologyUncertainties += m_PerturbedMetrologyUncertainties.back().GetTime();
      }
      if (m_PerturbedMetrologyUncertainties[NextIndex].GetTime() + m_TimeWrapPerturbedMetrologyUncertainties > t) {
        break;
      } else {
        m_StartIndexPerturbedMetrologyUncertainties = NextIndex;      
      }
    } while (true);
    // Start index is always smaller the the last one, except when they are identical, then rewind:
    if (m_StartIndexPerturbedMetrologyUncertainties == m_PerturbedMetrologyUncertainties.size() - 1) m_StartIndexPerturbedMetrologyUncertainties = 0; 
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndexPerturbedMetrologyUncertainties = 0;
  }

  if (m_PerturbedMetrologyUncertainties.size() == 1) {
    m_LatestMetrologyUncertainties = m_PerturbedMetrologyUncertainties[0];
  } else {
    // We should do interpolations here...
    if (Interpolate == true) {
      double Fraction = (t.GetSeconds() - m_TimeWrapPerturbedMetrologyUncertainties.GetSeconds() - m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime().GetSeconds())/
        (m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties+1].GetTime().GetSeconds() - m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime().GetSeconds());
        
      if (Fraction > 1) {
        merr<<"Time fraction for data base interpolation larger than 1!"<<show;
        Fraction = 1;
      }
        
      m_LatestMetrologyUncertainties.SetInterpolated(m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties], 
                                                     m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties+1], Fraction);
    } else {
      m_LatestMetrologyUncertainties = m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties];
    }
  }




  // Retrieve latest perturbed alignments
  if (t.GetSeconds() >= 0 && m_PerturbedOpticsUncertainties.size() > 1) {
    if (m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime() + m_TimeWrapPerturbedOpticsUncertainties > t) {
      m_StartIndexPerturbedOpticsUncertainties = 0;
      while (m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime() + m_TimeWrapPerturbedOpticsUncertainties > t) {
        m_TimeWrapPerturbedOpticsUncertainties -= m_PerturbedOpticsUncertainties.back().GetTime();
      }
    }

    unsigned int NextIndex = 0;
    do {
      NextIndex = m_StartIndexPerturbedOpticsUncertainties + 1;
      // If we reached the end of the array, rewind
      if (NextIndex >= m_PerturbedOpticsUncertainties.size()) {
        NextIndex = 0;
        m_TimeWrapPerturbedOpticsUncertainties += m_PerturbedOpticsUncertainties.back().GetTime();
      }
      if (m_PerturbedOpticsUncertainties[NextIndex].GetTime() + m_TimeWrapPerturbedOpticsUncertainties > t) {
        break;
      } else {
        m_StartIndexPerturbedOpticsUncertainties = NextIndex;      
      }
    } while (true);
    // Start index is always smaller the the last one, except when they are identical, then rewind:
    if (m_StartIndexPerturbedOpticsUncertainties == m_PerturbedOpticsUncertainties.size() - 1) m_StartIndexPerturbedOpticsUncertainties = 0; 
  } else {
    // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
    m_StartIndexPerturbedOpticsUncertainties = 0;
  }

  if (m_PerturbedOpticsUncertainties.size() == 1) {
    m_LatestOpticsUncertainties = m_PerturbedOpticsUncertainties[0];
  } else {
    // We should do interpolations here...
    if (Interpolate == true) {
      double Fraction = (t.GetSeconds() - m_TimeWrapPerturbedOpticsUncertainties.GetSeconds() - m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime().GetSeconds())/
        (m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties+1].GetTime().GetSeconds() - m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime().GetSeconds());
        
      if (Fraction > 1) {
        merr<<"Time fraction for data base interpolation larger than 1!"<<show;
        Fraction = 1;
      }
        
      m_LatestOpticsUncertainties.SetInterpolated(m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties], 
                                                  m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties+1], Fraction);
    } else {
      m_LatestOpticsUncertainties = m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties];
    }
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

  MXmlNode* CalibratedMetrologyDBFileNameNode = Node->GetNode("CalibratedMetrologyDBFileName");
  if (CalibratedMetrologyDBFileNameNode != 0) {
    m_CalibratedMetrologyDBFileName = CalibratedMetrologyDBFileNameNode->GetValue();
  }
  MXmlNode* PerturbedMetrologyDBFileNameNode = Node->GetNode("PerturbedMetrologyDBFileName");
  if (PerturbedMetrologyDBFileNameNode != 0) {
    m_PerturbedMetrologyDBFileName = PerturbedMetrologyDBFileNameNode->GetValue();
  }
  MXmlNode* CalibratedOpticsDBFileNameNode = Node->GetNode("CalibratedOpticsDBFileName");
  if (CalibratedOpticsDBFileNameNode != 0) {
    m_CalibratedOpticsDBFileName = CalibratedOpticsDBFileNameNode->GetValue();
  }
  MXmlNode* PerturbedOpticsDBFileNameNode = Node->GetNode("PerturbedOpticsDBFileName");
  if (PerturbedOpticsDBFileNameNode != 0) {
    m_PerturbedOpticsDBFileName = PerturbedOpticsDBFileNameNode->GetValue();
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
  
  new MXmlNode(Node, "CalibratedMetrologyDBFileName", m_CalibratedMetrologyDBFileName);
  new MXmlNode(Node, "PerturbedMetrologyDBFileName", m_PerturbedMetrologyDBFileName);
  new MXmlNode(Node, "CalibratedOpticsDBFileName", m_CalibratedOpticsDBFileName);
  new MXmlNode(Node, "PerturbedOpticsDBFileName", m_PerturbedOpticsDBFileName);
  new MXmlNode(Node, "CalibratedAlignmentsDBFileName", m_CalibratedAlignmentsDBFileName);
  new MXmlNode(Node, "PerturbedAlignmentsDBFileName", m_PerturbedAlignmentsDBFileName);

  return Node;
}


// NModuleOrientationsDatabase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
