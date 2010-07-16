/*
 * MFileManager.cxx
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


////////////////////////////////////////////////////////////////////////////////
//
// MFileManager
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MFileManager.h"

// Standard libs:
#include "MStreams.h"

// ROOT libs:
#include <TGFileDialog.h>
#include <TSystem.h>

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MFileManager)
#endif


////////////////////////////////////////////////////////////////////////////////


MFileManager::MFileManager()
{
  // Construct an instance of MFileManager
}


////////////////////////////////////////////////////////////////////////////////


MFileManager::~MFileManager()
{
  // Delete this instance of MFileManager
}


////////////////////////////////////////////////////////////////////////////////


bool MFileManager::SelectFileToLoad(TString& FileName, const char** FileTypes)
{
  //

  TGFileInfo Info;
  Info.fFilename = StrDup(gSystem->BaseName(FileName));
  Info.fIniDir = StrDup(gSystem->DirName(FileName));
  Info.fFileTypes = FileTypes;
  
  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDOpen, &Info);

  // Get the filename ...
  if ((char *) Info.fFilename != 0) {
    FileName = TString((char *) Info.fFilename);
    //mlog<<"New Name: "<<FileName<<endl;
    if (FileName.CompareTo("") == 0) {
      return false;
    }
  } 
  // ... or return when cancel has been pressed
  else {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MFileManager::SelectFileToSave(TString& FileName, const char** FileTypes)
{
  //

  TGFileInfo Info;
  Info.fFilename = StrDup(gSystem->BaseName(FileName));
  Info.fIniDir = StrDup(gSystem->DirName(FileName));
  Info.fFileTypes = FileTypes;
  new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), kFDSave, &Info);

  // Get the filename ...
  if ((char *) Info.fFilename != 0) {
    FileName = TString((char *) Info.fFilename);
    if (FileName.CompareTo("") == 0) {
      return false;
    }
  } 
  // ... or return when cancel has been pressed
  else {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString MFileManager::SearchFile()
{
  // 

  Fatal("Not implemented", "Not implemented");

  return "";
}


////////////////////////////////////////////////////////////////////////////////


int MFileManager::ErrorCode()
{
  //

  return m_ErrorCode;
}


////////////////////////////////////////////////////////////////////////////////


bool MFileManager::FileExists(TString FileName)
{
  // Return true if the file exists in the current directory (selected in the dialog)

  gSystem->ExpandPathName(FileName);
  gSystem->ExpandPathName(FileName);

  if (FileName.CompareTo(gSystem->DirName((char *) FileName.Data())) == 0)
    return false;

  FILE *File;
  if ((File = fopen((char *) FileName.Data(), "r")) == NULL) {
    return false;
  }

  fclose(File);

  return true;
}

////////////////////////////////////////////////////////////////////////////////


bool MFileManager::DirectoryExists(TString Filename)
{
  // Return true if the directory exists

  gSystem->ExpandPathName(Filename);
  gSystem->ExpandPathName(Filename);

  if (Filename.CompareTo(gSystem->DirName((char *) Filename.Data())) == 0) {
    return false;
  }

  if (gSystem->OpenDirectory(Filename) == NULL) {
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


TString MFileManager::MakeAbsolutePath(TString FileName)
{
  gSystem->ExpandPathName(FileName);
  gSystem->ExpandPathName(FileName);

  return FileName;
}


// MFileManager.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
