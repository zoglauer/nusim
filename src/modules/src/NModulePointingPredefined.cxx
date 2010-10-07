/*
 * NModulePointingPredefined.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NModulePointingPredefined
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NModulePointingPredefined.h"

// Standard libs:

// ROOT libs:
#include "TRandom.h"
#include "TGClient.h"

// MEGAlib libs:
#include "MStreams.h"

// NuSTAR libs:
#include "NGUIOptionsPointingPredefined.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NModulePointingPredefined)
#endif


////////////////////////////////////////////////////////////////////////////////


// make sure thos eare in sequence!
const int NModulePointingPredefined::c_MotionPatternNone        = 0;
const int NModulePointingPredefined::c_MotionPatternRandomWalk  = 1;
const int NModulePointingPredefined::c_MotionPatternDB          = 2;
// Update those too!!
const int NModulePointingPredefined::c_MotionPatternMin         = 0;
const int NModulePointingPredefined::c_MotionPatternMax         = 2;


////////////////////////////////////////////////////////////////////////////////


NModulePointingPredefined::NModulePointingPredefined(NSatellite& Satellite) : NModule(Satellite), NModuleInterfacePointing()
{
  // Construct an instance of NModulePointingPredefined

  // Set all module relevant information

  // Set the module name --- has to be unique
  m_Name = "Predefined";

  // Set the XML tag --- has to be unique --- no spaces allowed
  m_XmlTag = "XmlTagModulePointingPredefined";

  // Set the tool tip text
  m_ToolTip = "Choose between a set of predefined pointing patterns (e.g. fixed, random walk).";

  // Set all types this modules handles
  m_ModuleType = c_PointingEngine;

  // Set if this module has a diagnostics GUI
  m_HasDiagnosticsGUI = false;
  // If true, you have to derive a class from MGUIDiagnostics (use NGUIDiagnosticsPointing)
  // and implement all your GUI options
  //m_Diagnostics = new MGUIDiognosticsPointing();

  // Some default initilizations:
  NPointing N;
  N.SetRaDec(180*60, 0.0);
  N.SetTime(10000);
  m_InitialPointings.push_back(N);
  
  m_MotionPattern = c_MotionPatternNone;
}


////////////////////////////////////////////////////////////////////////////////


NModulePointingPredefined::~NModulePointingPredefined()
{
  // Delete this instance of NModulePointingPredefined
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::Initialize()
{
  // Initialize the module
  
  m_StartIndexSequencedInitialPointings = 0;
  m_TimeWrapSequencedInitialPointings = 0.0;
  
  m_StartIndexPointingJitters = 0;
  m_TimeWrapPointingJitters = 0.0;

  if (m_MotionPattern == c_MotionPatternDB) {
    if (MFile::Exists(m_PointingJitterDBFileName) == false) {
      mgui<<"Calibrated metrology DB does not exist!"<<error;
      return false;
    }
    if (ReadPointingJitterDB(m_PointingJitterDBFileName) == false) return false;
    //cout<<m_PointingJitters[0].ToString()<<endl;
  }
  
  if (m_InitialPointings.size() == 0) {
    mgui<<"You need to give at least one space craft pointing."<<error;
    return false;
  }
  
  
  // Sequence the Pointings
  m_SequencedInitialPointings.clear();
  NTime T(0);
  for (unsigned int p = 0; p < m_InitialPointings.size(); ++p) {
    m_SequencedInitialPointings.push_back(m_InitialPointings[p]);
    m_SequencedInitialPointings[p].SetTime(T);
    T += m_InitialPointings[p].GetTime();
  }
  // Add a stop point:
  m_SequencedInitialPointings.push_back(m_SequencedInitialPointings.back());
  m_SequencedInitialPointings.back().SetTime(T);
  
  if (m_AbsoluteTime == false) {
    // Scale
    double Scaler = m_ObservationTime.GetSeconds()/T.GetSeconds();
    for (unsigned int p = 0; p < m_SequencedInitialPointings.size(); ++p) {
      m_SequencedInitialPointings[p].SetTime(m_SequencedInitialPointings[p].GetTime()*Scaler);
    }
  }
  
  //for (unsigned int p = 0; p < m_SequencedInitialPointings.size(); ++p) {
  //  cout<<"I["<<p<<"] = "<<m_SequencedInitialPointings[p].ToString()<<endl;
  //}


  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::ReadPointingJitterDB(TString FileName)
{
  //! Read the data from file

  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    mgui<<"Unable to open file: \""<<FileName<<"\""<<show;
    return false;
  }

  // Read the header
  TString Line;
  
  Line.ReadLine(in); // description
  Line.ReadLine(in); // column title 1
  
  // Read the data
  m_PointingJitters.clear();
  while (!in.eof()) {
    Line.ReadLine(in);
    
    if (Line.Length() > 0) {
      if (Line[0] == ',') {
        cerr<<"Parsing issue (starts with ','): \""<<FileName<<"\""<<endl;
        continue;
      }

      NPointingJitterDBEntry J;
      if (J.ParseDB(Line) == false) {
        mgui<<"Parsing failed: Something is wrong with your calibrated alignments data base!"<<show;
        in.close();
        return false;            
      }
      m_PointingJitters.push_back(J);
    }
  } 
  in.close();

  return true;
}

////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::StopCriterionFullFilled()
{
  //! Check if stop criterion is fullfilled
  
  if (m_AbsoluteTime == true) {
    if (m_Satellite.GetTimeIdeal() >= m_SequencedInitialPointings.back().GetTime()) {
      return true;
    }
  }
  
  return false;
}

  
////////////////////////////////////////////////////////////////////////////////


NPointing NModulePointingPredefined::GetPointing(NTime t) 
{ 
  // Return the pointing of the satellite at a given time

  if (m_Time != t) {
    bool Interpolate = false;

    // First check if we have to advance the main pointing
    unsigned int OldStartIndex = m_StartIndexSequencedInitialPointings;
    
    // PART 1: Find the initial pointing:
    
    if (t.GetSeconds() >= 0 && m_SequencedInitialPointings.size() > 1) {
      if (m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings].GetTime() + m_TimeWrapSequencedInitialPointings > t) {
        m_StartIndexSequencedInitialPointings = 0;
        while (m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings].GetTime() + m_TimeWrapSequencedInitialPointings > t) {
          m_TimeWrapSequencedInitialPointings -= m_SequencedInitialPointings.back().GetTime();
        }
      }
      //cout<<t<<":"<<m_TimeWrapSequencedInitialPointings<<endl;

      unsigned int NextIndex = 0;
      do {
        NextIndex = m_StartIndexSequencedInitialPointings + 1;
        // If we reached the end of the array, rewind
        if (NextIndex >= m_SequencedInitialPointings.size()) {
          NextIndex = 0;
          m_TimeWrapSequencedInitialPointings += m_SequencedInitialPointings.back().GetTime();
        }
        if (m_SequencedInitialPointings[NextIndex].GetTime() + m_TimeWrapSequencedInitialPointings > t) {
          break;
        } else {
          m_StartIndexSequencedInitialPointings = NextIndex;      
        }
      } while (true);
    } else {
      // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
      //cout<<"Always zero"<<endl;
      m_StartIndexSequencedInitialPointings = 0;
    }
    
    //cout<<t<<": "<<m_StartIndexSequencedInitialPointings<<endl;

    // If the StartIndex changed, reset the pointing jitter to zero!
    if (m_StartIndexSequencedInitialPointings != OldStartIndex) {
      m_StartIndexPointingJitters = 0;
    }
    
    
    
    
    // PART 2: Move!
    
    if (m_MotionPattern == c_MotionPatternNone) {
      m_Pointing = m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings];
    } else if (m_MotionPattern == c_MotionPatternDB) {
      // Now apply the pointing jitter
      
      // Retrieve latest perturbed alignments
      if (t.GetSeconds() >= 0 && m_PointingJitters.size() > 1) {
        if (m_PointingJitters[m_StartIndexPointingJitters].GetTime() + m_TimeWrapPointingJitters > t) {
          m_StartIndexPointingJitters = 0;
          while (m_PointingJitters[m_StartIndexPointingJitters].GetTime() + m_TimeWrapPointingJitters > t) {
            m_TimeWrapPointingJitters -= m_PointingJitters.back().GetTime();
          }
        }

        unsigned int NextIndex = 0;
        do {
          NextIndex = m_StartIndexPointingJitters + 1;
          // If we reached the end of the array, rewind
          if (NextIndex >= m_PointingJitters.size()) {
            NextIndex = 0;
            m_TimeWrapPointingJitters += m_PointingJitters.back().GetTime();
          }
          if (m_PointingJitters[NextIndex].GetTime() + m_TimeWrapPointingJitters > t) {
            break;
          } else {
            m_StartIndexPointingJitters = NextIndex;      
          }
        } while (true);
        // Start index is always smaller the the last one, except when they are identical, then rewind:
        if (m_StartIndexPointingJitters == m_PointingJitters.size() - 1) m_StartIndexPointingJitters = 0; 
      } else {
        // If the time is lower than 0 we alwasy start with the first index, in order to have smooth start:
        m_StartIndexPointingJitters = 0;
      }

      NPointingJitterDBEntry LatestPointingJitters;
      if (m_PointingJitters.size() == 1) {
        LatestPointingJitters = m_PointingJitters[0];
      } else {
        // We should do interpolations here...
        if (Interpolate == true) {
          double Fraction = (t.GetSeconds() - m_TimeWrapPointingJitters.GetSeconds() - m_PointingJitters[m_StartIndexPointingJitters].GetTime().GetSeconds())/
            (m_PointingJitters[m_StartIndexPointingJitters+1].GetTime().GetSeconds() - m_PointingJitters[m_StartIndexPointingJitters].GetTime().GetSeconds());
        
          if (Fraction > 1) {
            merr<<"Time fraction for data base interpolation larger than 1!"<<show;
            Fraction = 1;
          }

          LatestPointingJitters.SetInterpolated(m_PointingJitters[m_StartIndexPointingJitters], 
                                                  m_PointingJitters[m_StartIndexPointingJitters+1], Fraction);
        } else {
          LatestPointingJitters = m_PointingJitters[m_StartIndexPointingJitters];
        }
      }
    
      // Apply:
      //cout<<m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings].ToString()<<endl;
      m_Pointing.SetQuaternion(m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings].GetQuaternion()*LatestPointingJitters.GetQuaternion());
      //m_Pointing.SetQuaternion(LatestPointingJitters.GetQuaternion()*m_SequencedInitialPointings[m_StartIndexSequencedInitialPointings].GetQuaternion());
    
    } // jitter end
    
    m_Time = t;
  }
  
  //cout<<t<<":"<<m_Pointing.ToString()<<endl;

  return m_Pointing;
}


////////////////////////////////////////////////////////////////////////////////


void NModulePointingPredefined::SetPointingJitterDBFileName(const TString PointingJitterDBFileName) 
{ 
  //! Set the file name of the pointing jitter data base

  m_PointingJitterDBFileName = PointingJitterDBFileName; 

  if (m_PointingJitterDBFileName == "") {
    m_MotionPattern = c_MotionPatternNone;
  } else {
    m_MotionPattern = c_MotionPatternDB;    
  }
}


////////////////////////////////////////////////////////////////////////////////



bool NModulePointingPredefined::ImportPointings(TString FileName)
{
  //! Import pointings


  // Load from file
  ifstream in;
  in.open(FileName);
  if (in.is_open() == false) {
    cerr<<"Unable to open file: \""<<FileName<<"\""<<endl;
    return false;
  }

  m_InitialPointings.clear();
  
  // Read the data
  TString Line;
  while (!in.eof()) {
    Line.ReadLine(in);
    
    if (Line.BeginsWith("RD") == true) {
      NPointing P;
      P.Parse(Line);
      if (P.IsEmpty() == false) {
        m_InitialPointings.push_back(P);
      }
    }
  } 
  in.close();

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void NModulePointingPredefined::ShowOptionsGUI()
{
  //! Show the options GUI --- has to be overwritten!

  // The default behaviour is to show the base class telling the user 
  // that there are no options to select.
  // If you want your own option dialog derive one from NGUIOptions
  // (probably you might want to use the template) and replace the following line

  NGUIOptionsPointingPredefined* Options = new NGUIOptionsPointingPredefined(this);

  // this stays always the same:
  Options->Create();
  gClient->WaitForUnmap(Options);
}


////////////////////////////////////////////////////////////////////////////////


bool NModulePointingPredefined::ReadXmlConfiguration(MXmlNode* Node)
{
  //! Read the configuration data from an XML node

  m_InitialPointings.clear();

  MXmlNode* PointingsNode = Node->GetNode("Pointings");
  if (PointingsNode != 0) {
    for (unsigned int s = 0; s < PointingsNode->GetNNodes(); ++s) {
      if (PointingsNode->GetNode(s)->GetName() == "Pointing") {
        NPointing New;
        New.ReadXmlConfiguration(PointingsNode->GetNode(s));
        m_InitialPointings.push_back(New);
      }
    }
  }

  MXmlNode* PointingJitterDBFileNameNode = Node->GetNode("PointingJitterDBFileName");
  if (PointingJitterDBFileNameNode != 0) {
    m_PointingJitterDBFileName = PointingJitterDBFileNameNode->GetValue();
  }
  SetPointingJitterDBFileName(m_PointingJitterDBFileName); // Set also m_MotionPattern

  MXmlNode* AbsoluteTimeNode = Node->GetNode("AbsoluteTime");
  if (AbsoluteTimeNode != 0) {
    m_AbsoluteTime = AbsoluteTimeNode->GetValueAsBoolean();
  }

  if (m_InitialPointings.size() == 0) {
    NPointing N;
    N.SetRaDec(180.0*60, 0.0);
    N.SetTime(10000);
    m_InitialPointings.push_back(N);
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* NModulePointingPredefined::CreateXmlConfiguration() 
{
  //! Create an XML node tree from the configuration

  MXmlNode* Node = new MXmlNode(0, m_XmlTag);
  
  MXmlNode* Pointings = new MXmlNode(Node, "Pointings");
  for (unsigned int p = 0; p < m_InitialPointings.size(); ++p) {
    Pointings->AddNode(m_InitialPointings[p].CreateXmlConfiguration());
  }
  new MXmlNode(Node, "PointingJitterDBFileName", CleanPath(m_PointingJitterDBFileName));
  new MXmlNode(Node, "AbsoluteTime", m_AbsoluteTime);

  return Node;
}


// NModulePointingPredefined.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
