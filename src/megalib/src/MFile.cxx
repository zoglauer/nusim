/*
 * MFile.cxx
 *
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 *
 * This code implementation is the intellectual property of
 * Andreas Zoglauer.
 *
 * By copying, distributing or modifying the Program (or any work
 * based on the Program) you indicate your acceptance of this statement,
 * and all its terms.
 *
 */


// MEGAlib:
#include "MGlobal.h"
#include "MAssert.h"
#include "MFile.h"
#include "MTimer.h"
#include "MStreams.h"

// ROOT libs:
#include <TSystem.h>
#include <TROOT.h>
#include <TRandom.h>

// Standard libs:
#include <iostream>
#include <streambuf>
#include <limits>
#include <cstdio>
#include <cstdlib>
using namespace std;

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MFile)
#endif


const TString MFile::c_TypeUnknown = "___TypeUnknown___";
const int MFile::c_VersionUnknown = -1;


////////////////////////////////////////////////////////////////////////////////


unsigned int MFile::c_Write   = 0;
unsigned int MFile::c_Create  = 0;
unsigned int MFile::c_Read    = 1;


////////////////////////////////////////////////////////////////////////////////


MFile::MFile()
{
  // Construct an instance of MFile

  Reset();
}


////////////////////////////////////////////////////////////////////////////////


MFile::~MFile()
{
  // Delete this instance of MFile

  Close();
  
  if (m_OwnProgress == true) {
    delete m_Progress;
  } else if (m_Progress != 0) {
    m_Progress->SetValue(0, m_ProgressLevel);
  }
}


////////////////////////////////////////////////////////////////////////////////



void MFile::Reset()
{
  // Set all values to default values

  m_FileName = "";
  m_Way = c_Read;

  m_FileType = c_TypeUnknown;
  m_Version = c_VersionUnknown;

  m_IsOpen = false;

  m_FileLength = 0;

  m_Progress = 0;
  m_ProgressLevel = 0;
  m_OwnProgress = true;

  m_WasZipped = false;
  m_ZippedFileName = g_StringNotDefined;

  // The maximum allowed file length
  m_MaxFileLength = numeric_limits<streamsize>::max()/100*95;
  //m_MaxFileLength = 100000;
}


////////////////////////////////////////////////////////////////////////////////


void MFile::SetVersion(const int Version)
{
  // Set the version

  m_Version = Version;
}

 
////////////////////////////////////////////////////////////////////////////////


int MFile::GetVersion() const
{
  // Return the version of this file

  return m_Version;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::FileExists(const char* FileName)
{
  // Check if the file exists:

  TString Name(FileName);

  // Just in case: Expand file name:
  gSystem->ExpandPathName(Name);
  gSystem->ExpandPathName(Name);
 
  if (Name.EndsWith("/")) {
    return false;
  }
  
  if (Name.CompareTo(gSystem->DirName((char *) Name.Data())) == 0) {
    return false;
  }

  ifstream in;
  in.open(Name.Data());
  if (in.is_open() == false) {
    return false;
  }
  in.close();

  return true;  
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::Exists(TString FileName)
{
  // Check if the file exists:

  // Just in case: Expand file name:
  gSystem->ExpandPathName(FileName);
  gSystem->ExpandPathName(FileName);
  
  if (FileName.EndsWith("/")) {
    return false;
  }

  if (FileName.CompareTo(gSystem->DirName(FileName.Data())) == 0) {
    return false;
  }

  ifstream in;
  in.open(FileName.Data());
  if (in.is_open() == false) {
    return false;
  }
  in.close();

  return true;  
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::ApplyPath(TString& FileName, const TString& Path)
{
  // Sets the path of "Path" as new path to "FileName"
  
  if (gSystem->IsAbsoluteFileName(FileName) == true) return true;

  TString NewPath = Path;
  ExpandFileName(NewPath);
  
  TString DirName = gSystem->DirName(NewPath); 

  FileName = DirName + TString("/") + FileName;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::ProgramExists(TString Program)
{
  // Return true if the specified file exists:

  char* Path;
  Path = gSystem->Which(gSystem->Getenv("PATH"), Program);

  if (Path != 0) {
    delete Path;
    return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////


void MFile::ExpandFileName(TString& FileName, const TString& WorkingDir)
{
  // Just in case: Expand file name:
  gSystem->ExpandPathName(FileName);
  gSystem->ExpandPathName(FileName);

  if (FileName.BeginsWith("/") == false) {
    if (WorkingDir == "") {
      FileName = TString(gSystem->WorkingDirectory()) + "/" + FileName;
    } else {
      FileName = WorkingDir + "/" + FileName;
    }
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::Open(TString FileName, unsigned int Way)
{
  // Open the file in different modes

  // Close the file just in case we are open
  Close();

  m_FileName = FileName;
  if (m_FileName == "") {
    mgui<<"You need to give a file name, before I can open a file."<<error;
    abort();
    return false;
  }

  // Just in case: Expand file name:
  ExpandFileName(m_FileName);

  // If the file is zipped we have to unzip it
  m_WasZipped = false;
  if (Way == c_Read && CheckFileExtension("gz") == true) {
    TString Temp = gSystem->TempDirectory();

    // Create a new temporary file name
    TString NewFileName = "";
    do {
      NewFileName = Temp + TString("/");
      for (int i = 0; i < 32; ++i) {
        NewFileName += (char) (int('a') + gRandom->Integer(26));
      }
      NewFileName += ".";
      TString Type = m_FileType;
      Type.ToLower();
      NewFileName += Type;
    } while (Exists(NewFileName) == true);

    // Unzip
    mout<<"Unzipping file... please stand by..."<<endl;
    TString Unzip = TString("gunzip -c ") + m_FileName + TString(" > ") + NewFileName;
    int Success = gSystem->Exec(Unzip);

    if (Success != 0) {
      remove(NewFileName);
      mgui<<"Unable to unzip: "<<endl
          <<"\""<<m_FileName<<"\""<<error;
      return false;
    }

    // Store infoarmation about the old and new file
    m_WasZipped = true;
    m_ZippedFileName = m_FileName;
    m_FileName = NewFileName;
  }


  // Do a sanity check on the file type
  if (m_FileType != c_TypeUnknown) {
    if (CheckFileExtension(m_FileType) == false) {
      mgui<<"The file: "<<endl
          <<"\""<<m_FileName<<"\""
          <<endl<<"has not the correct extension \""<<m_FileType<<"\"!"<<error;
      return false;
    }
  }

  // Make sure the file exists when we try to read it
  if (Way == c_Read && FileExists(m_FileName) == false) {
    mgui<<"The file: "<<endl
        <<"\""<<m_FileName<<"\""<<endl
        <<"does not exist!"<<error;
    return false;
  }

  // Finally open it

  m_File.clear();
  if (Way == c_Write) {
    m_File.open(m_FileName, ios_base::out);
  } else {
    m_File.open(m_FileName, ios_base::in);
  }

  if (m_File.is_open() == false) {
    mgui<<"Unable to open file \""<<m_FileName<<"\""<<endl;
    return false;
  }

  // Determine the file length
  if (Way == c_Read) {
    m_File.seekg(0, ios_base::end);
    m_FileLength = m_File.tellg();
    m_File.seekg(0, ios_base::beg);
  } 

  // We are open now
  m_IsOpen = true;
  m_Way = Way;

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::Rewind()
{
  // Rewind to the beginning of the file

  if (m_IsOpen == false) {
    return false;
  }

  m_File.clear();
  m_File.seekg(0);

  if (m_Progress != 0) {
    UpdateProgress();
    m_Progress->ResetTimer();
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::Close()
{
  // If we created an unzipped copy we have to delete the unzipped version
  if (m_WasZipped == true) {
    // Sanity check: the file name must be in the temporary directory
    if (m_FileName.BeginsWith(gSystem->TempDirectory()) == true) {
      remove(m_FileName);
    }
  }

  if (IsOpen() == true) {
    m_File.close();
    m_IsOpen = false;
	}

  ShowProgress(false);

  return true;
}


////////////////////////////////////////////////////////////////////////////////


void MFile::ShowProgress(bool Show)
{
  //

  if (Show == true) {
    if (gClient == 0 || gClient->GetRoot() == 0 || gROOT->IsBatch() == true) {
      cout<<"Can't show progress bar, because you do not have a GUI"<<endl;
    } else {
      if (m_OwnProgress == true) {
        delete m_Progress;
      }
      m_Progress = new MGUIProgressBar(0, "Progress", "Progress of analysis");
      m_Progress->SetMinMax(0, 1);
      m_OwnProgress = true;
    }
  } else {
    if (m_OwnProgress == true) {
      delete m_Progress;
    }
    m_Progress = 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


void MFile::SetProgressTitle(TString Main, TString Sub)
{
  // Set the title of the current progress bar

  if (m_Progress != 0) {
    m_Progress->SetTitles(Main, Sub);
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::IsShowProgress() 
{
  // Return show progress   

  if (m_Progress != 0) return true;
  return false;
}


////////////////////////////////////////////////////////////////////////////////


void MFile::SetProgress(MGUIProgressBar* Progress, int Level)
{
  // Take over a progressbar

  delete m_Progress;
  m_Progress = Progress;
  m_ProgressLevel = Level;
  m_OwnProgress = false;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::UpdateProgress()
{
  // Update the Progress Dialog, if it is visible
  // Return false, when "Cancel" has been pressed

  if (m_Progress == 0 || m_FileLength == (streampos) 0) return true;

  double Value = (double) m_File.tellg() / (double) m_FileLength;
  m_Progress->SetValue(Value, m_ProgressLevel);
  gSystem->ProcessEvents();

  if (m_Progress->TestCancel() == true) {
    ShowProgress(false);
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::IsOpen()
{
  // Return true if file is open
  
  return m_IsOpen;
}


////////////////////////////////////////////////////////////////////////////////


bool MFile::CheckFileExtension(TString Extension)
{
  // Check if the extension of this file is "Extension"

  TString Upper(Extension);
  Upper.ToUpper();
  Upper.Prepend(".");

  TString Lower(Extension);
  Lower.ToLower();
  Lower.Prepend(".");

  if (m_FileName.EndsWith(Upper) == true || 
      m_FileName.EndsWith(Lower) == true) {
    return true;
  } else {
    return false;
  }
}


////////////////////////////////////////////////////////////////////////////////


TString MFile::RelativeFileName(TString RelFileName, TString AbsFileName) 
{
  // Make RelFileName relative to AbsFileName
  // Rel: /home/andreas/Test/MyFile.tra
  // Abs: /home/andreas/MasterTest.tra
  // Ret: ./Test/MyFile.tra

  MFile::ExpandFileName(RelFileName);
  MFile::ExpandFileName(AbsFileName);

  int MinLength = std::min(RelFileName.Last('/'), AbsFileName.Last('/'));
  
  int PreviousSlash = -1;
  for (int i = 0; i <= MinLength; ++i) {
    if (RelFileName[i] != AbsFileName[i]) {
      break;
    }
    if (RelFileName[i] == '/') PreviousSlash = i;
  }
  int RemainingSlashesAbs = 0;
  for (int i = PreviousSlash+1; i < AbsFileName.Length(); ++i) {
    if (AbsFileName[i] == '/') {
      RemainingSlashesAbs++;
    }
  }
  
  if (PreviousSlash > 0) { 
    RelFileName.Remove(0, PreviousSlash+1);

    if (RemainingSlashesAbs > 0) {
      for (int i = 0; i < RemainingSlashesAbs; ++i) {
        RelFileName.Prepend("../");
      }
    } else {
      RelFileName.Prepend("./");
    }
  }

  return RelFileName;
}


////////////////////////////////////////////////////////////////////////////////


TString MFile::GetFileName() const
{
  return m_FileName;
}


////////////////////////////////////////////////////////////////////////////////


streampos MFile::GetFileLength()
{
  // Return the file length
  // Since this is a random access operation it should be very fast...

  if (IsOpen() == false) {
    merr<<"File "<<m_FileName<<" not open!"<<show;
    return 0;
  }

  streampos Length;
  if (m_Way == c_Read) {
    streampos Current = m_File.tellg();
    m_File.seekg(0, ios_base::end);
    Length = m_File.tellg();
    m_File.seekg(Current, ios_base::beg);
  } else {
    Length = m_File.tellp();
  }

  return Length;
}




// MFile.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
