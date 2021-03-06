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

  m_Time = -90*minute; // Any value < 0 will do... 
  
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);
  
  // Read the header
  TString Line;

  Line.ReadToDelim(in, Delimeter); // version
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  Line.ReadToDelim(in, Delimeter); // column title 2
  
  // Read the data
  Line.ReadToDelim(in, Delimeter);
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);

  // Read the header
  TString Line;

  Line.ReadToDelim(in, Delimeter); // version
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  Line.ReadToDelim(in, Delimeter); // column title 2
  
  // Read the data
  int LineCounter = 0;
  bool Found = false;
  while (!in.eof()) {
    Line.ReadToDelim(in, Delimeter);
    
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);
  
  // Read the header
  TString Line;

  Line.ReadToDelim(in, Delimeter); // version
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  Line.ReadToDelim(in, Delimeter); // column title 2
  
  // Read the data
  Line.ReadToDelim(in, Delimeter);
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);

  // Read the header
  TString Line;

  Line.ReadToDelim(in, Delimeter); // version
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  Line.ReadToDelim(in, Delimeter); // column title 2
  
  // Read the data
  bool Found = false;
  while (!in.eof()) {
    Line.ReadToDelim(in, Delimeter);
    
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);

  // Read the header
  
  // If there is any error in the file, it will always be caught in ParseDB...
  TString Line;
  Line.ReadToDelim(in, Delimeter); // version or fileformatversion
  Line.ReadToDelim(in, Delimeter); // type
  Line.ReadToDelim(in, Delimeter); // description
  Line.ReadToDelim(in, Delimeter); // column title 1
  
  Line.ReadToDelim(in, Delimeter);
  TString Positions = Line;
      
  Line.ReadToDelim(in, Delimeter);
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

  // Let's read until we find the first delimeter, then rewind
  char Delimeter = FindDelimeter(in);
  
  unsigned int Version = 1;
  
  // Read the header
  TString Line;
  MTokenizer T(',', false);
  Line.ReadToDelim(in, Delimeter); // version or fileformatversion
  if (Line.BeginsWith("fileformatversion") == true) {
    T.Analyze(Line);
    Version = T.GetTokenAtAsUnsignedInt(1);
  }
  
  if (Version == 1) {
    Line.ReadToDelim(in, Delimeter); // type
    Line.ReadToDelim(in, Delimeter); // description
    Line.ReadToDelim(in, Delimeter); // step size
    Line.ReadToDelim(in, Delimeter); // column title
    m_HasDayAndNightAlignments = false; 
  } else if (Version == 2) {
    Line.ReadToDelim(in, Delimeter); // version
    Line.ReadToDelim(in, Delimeter); // type
    Line.ReadToDelim(in, Delimeter); // description
    Line.ReadToDelim(in, Delimeter); // step size
    Line.ReadToDelim(in, Delimeter); // night
    T.Analyze(Line);
    m_NightStartAlignments = T.GetTokenAtAsUnsignedInt(1);    
    Line.ReadToDelim(in, Delimeter); // day
    T.Analyze(Line);
    m_DayStartAlignments = T.GetTokenAtAsUnsignedInt(1);
    m_HasDayAndNightAlignments = true;
    Line.ReadToDelim(in, Delimeter); // column title
  } else {
    cerr<<"While reading: "<<FileName<<":Unknown file version: "<<Version<<endl;
    return false;
  }
  
  // Read the data
  TString Positions, Rotations;
  bool Found = false;
  while (!in.eof()) {
    Line.ReadToDelim(in, Delimeter);
    
    if (Line.Length() > 0) {
      if (Line[0] == ',') {
        cerr<<"Parsing issue (starts with ','): \""<<FileName<<"\""<<endl;
        continue;
      }
      Positions = Line;
      
      Line.ReadToDelim(in, Delimeter);

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

  // Check the day and night alignments
  if (m_HasDayAndNightAlignments == true) {
    if (m_DayStartAlignments >= m_PerturbedAlignments.size()) {
      merr<<"DB error: The index of the start of the day is beyond the vector limits"<<endl;
      return false;
    }
    if (m_NightStartAlignments >= m_PerturbedAlignments.size()) {
      merr<<"DB error: The index of the start of the night is beyond the vector limits"<<endl;
      return false;
    }
    
    if (m_DayStartAlignments == 0 && m_NightStartAlignments == 0) {
      m_HasDayAndNightAlignments = false;
    }
    
    unsigned int Position = m_NightStartAlignments;
    m_PerturbedAlignmentsNight.clear();
    while (true) {
      m_PerturbedAlignmentsNight.push_back(m_PerturbedAlignments[Position]);
      Position++;
      if (Position == m_PerturbedAlignments.size()) Position = 0;
      if (Position == m_DayStartAlignments) {
        m_PerturbedAlignmentsNight.push_back(m_PerturbedAlignments[Position]);
        break;
      }
    }
    Position = m_DayStartAlignments;
    m_PerturbedAlignmentsDay.clear();
    while (true) {
      m_PerturbedAlignmentsDay.push_back(m_PerturbedAlignments[Position]);
      Position++;
      if (Position == m_PerturbedAlignments.size()) Position = 0;
      if (Position == m_NightStartAlignments) {
        m_PerturbedAlignmentsDay.push_back(m_PerturbedAlignments[Position]);
        break;
      }
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


char NModuleOrientationsDatabase::FindDelimeter(ifstream& in)
{  
  //! Find the line-delimeter of the file and rewind file
 
 
  // Let's read until we find the first delimeter:
  char Delimeter;
  do {
    in.get(Delimeter);
  } while (in.eof() == false && int(Delimeter) != 10 && int(Delimeter) != 13); // 10: \n o new line/line feed --- 13: carriage return
  if (int(Delimeter) == 13) {
    char c;
    in.get(c);
    if (int(c) == 10) Delimeter = c;
  }
  in.seekg(0, ios::beg); // rewind

  return Delimeter;
}
 
 
////////////////////////////////////////////////////////////////////////////////


void NModuleOrientationsDatabase::AdvanceTime(const NTime& t)
{
  // Determine orientations if they are not yet present for the given time t
  // All are relative to the focal plane coordinate system
  // Remember: Distances are in mm!
    
  // Don't do anything if we are at the same time...
  if (m_Time == t) return;

  bool Interpolate = true;

  // Retrieve latest perturbed alignments
  if (m_HasDayAndNightAlignments == false || m_Satellite.OrbitProvidesDayAndNightCycles() == false) {
    if (t.GetAsSeconds() >= 0 && m_PerturbedAlignments.size() > 1) {
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
        double Fraction = (t.GetAsSeconds() - m_TimeWrapPerturbedAlignments.GetAsSeconds() - m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime().GetAsSeconds())/
          (m_PerturbedAlignments[m_StartIndexPerturbedAlignments+1].GetTime().GetAsSeconds() - m_PerturbedAlignments[m_StartIndexPerturbedAlignments].GetTime().GetAsSeconds());
        
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
  } else {
    if (m_Satellite.IsNight(m_Time) == true) {
      NTime Low = m_Satellite.GetLastBeginNightTime(m_Time);
      NTime High = m_Satellite.GetNextEndNightTime(m_Time);
      double TimeBin = (m_Time - Low).GetAsSeconds() / (High - Low).GetAsSeconds() * (m_PerturbedAlignmentsNight.size()-1);
      unsigned int Bin = floor(TimeBin);
      if (Bin < 0) {
        merr<<"Bin finding failed: "<<Bin<<endl;
        Bin = 0;
      } else if (Bin == m_PerturbedAlignmentsNight.size()-1 && m_Time < High+0.0001) {
        Bin = m_PerturbedAlignmentsNight.size() - 2;
      } else if (Bin == m_PerturbedAlignmentsNight.size()-1) {
        merr<<"Bin finding failed: "<<Bin<<endl;
        Bin = m_PerturbedAlignmentsNight.size() - 2;
      }
      m_LatestPerturbedAlignments.SetInterpolated(m_PerturbedAlignmentsNight[Bin], 
                                                  m_PerturbedAlignmentsNight[Bin+1], TimeBin - Bin);      
    } else {
      NTime Low = m_Satellite.GetLastEndNightTime(m_Time);
      NTime High = m_Satellite.GetNextBeginNightTime(m_Time);
      double TimeBin = (m_Time - Low).GetAsSeconds() / (High - Low).GetAsSeconds() * (m_PerturbedAlignmentsDay.size()-1);
      unsigned int Bin = floor(TimeBin); 
      if (Bin < 0) {
        merr<<"Bin finding failed: "<<Bin<<endl;
        Bin = 0;
      } else if (Bin == m_PerturbedAlignmentsDay.size()-1 && m_Time < High+0.0001) {
        Bin = m_PerturbedAlignmentsDay.size() - 2;
      } else if (Bin >= m_PerturbedAlignmentsDay.size()-1) {
        merr<<"Bin finding failed: "<<Bin<<endl;
        Bin = m_PerturbedAlignmentsDay.size() - 2;
      }
      m_LatestPerturbedAlignments.SetInterpolated(m_PerturbedAlignmentsDay[Bin], 
                                                  m_PerturbedAlignmentsDay[Bin+1], TimeBin - Bin);      
    }
  }


  // Retrieve latest metrology uncertainties
  if (t.GetAsSeconds() >= 0 && m_PerturbedMetrologyUncertainties.size() > 1) {
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
      double Fraction = (t.GetAsSeconds() - m_TimeWrapPerturbedMetrologyUncertainties.GetAsSeconds() - m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime().GetAsSeconds())/
        (m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties+1].GetTime().GetAsSeconds() - m_PerturbedMetrologyUncertainties[m_StartIndexPerturbedMetrologyUncertainties].GetTime().GetAsSeconds());
        
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
  if (t.GetAsSeconds() >= 0 && m_PerturbedOpticsUncertainties.size() > 1) {
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
      double Fraction = (t.GetAsSeconds() - m_TimeWrapPerturbedOpticsUncertainties.GetAsSeconds() - m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime().GetAsSeconds())/
        (m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties+1].GetTime().GetAsSeconds() - m_PerturbedOpticsUncertainties[m_StartIndexPerturbedOpticsUncertainties].GetTime().GetAsSeconds());
        
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
  
  new MXmlNode(Node, "CalibratedMetrologyDBFileName", CleanPath(m_CalibratedMetrologyDBFileName));
  new MXmlNode(Node, "PerturbedMetrologyDBFileName", CleanPath(m_PerturbedMetrologyDBFileName));
  new MXmlNode(Node, "CalibratedOpticsDBFileName", CleanPath(m_CalibratedOpticsDBFileName));
  new MXmlNode(Node, "PerturbedOpticsDBFileName", CleanPath(m_PerturbedOpticsDBFileName));
  new MXmlNode(Node, "CalibratedAlignmentsDBFileName", CleanPath(m_CalibratedAlignmentsDBFileName));
  new MXmlNode(Node, "PerturbedAlignmentsDBFileName", CleanPath(m_PerturbedAlignmentsDBFileName));

  return Node;
}


// NModuleOrientationsDatabase.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
