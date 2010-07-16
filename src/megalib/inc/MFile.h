/*
 * MFile.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MFile__
#define __MFile__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TFile.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MGUIProgressBar.h"

// Forward declarations:
#include <fstream>
#include <streambuf>
using namespace std;

////////////////////////////////////////////////////////////////////////////////


class MFile
{
  // public interface:
 public:
  //! Default constructor
  MFile();
  //! Default destructor
  virtual ~MFile();

  //! Close the file and reset all data to default values
  virtual void Reset();

  //! Open the file for reading or writing 
  virtual bool Open(TString FileName, unsigned int Way = c_Read);
  //! Close the file
  virtual bool Close();
  //! Rewind the file
  virtual bool Rewind();

  //! Return true if the file is open
  virtual bool IsOpen();
  //! Return the file length
  virtual streampos GetFileLength();

  TString GetFileName() const;

  //! Set the file version
  void SetVersion(const int Version);
  //! Get the current file version
  int GetVersion() const;

  //! Set the file type e.g. "sim" or "tra"
  void SetFileType(const TString& Type) { m_FileType = Type; }
  //! Get the file type e.g. "sim" or "tra"
  TString GetFileType() const { return m_FileType; }

  //! Returns true if the progress dialog is shown
  bool IsShowProgress();
  //! Show the progress dialog GUI
  virtual void ShowProgress(bool Show = true);
  //! Update the progress dialog GUI
  virtual bool UpdateProgress();
  //! Set the titles of the progress dialog GUI
  void SetProgressTitle(TString Main, TString Sub);
  //! Use another progress dialog GUI instead of this one
  void SetProgress(MGUIProgressBar* ProgressBar, int Level);

  //! Return true if the file extensions are correct
  virtual bool CheckFileExtension(TString Extension);

  //! Return true is the file exists
  static bool Exists(TString FileName);
  //! Return true is the file exists
  static bool FileExists(const char* FileName); // depreciated
  //! Check if a program exists
  static bool ProgramExists(TString Program);
  //! Sets the path of "Path" as new path to "FileName" IF FileName has a relative path 
  static bool ApplyPath(TString& FileName, const TString& Path);
  //! Expand a file name e.g. $MEGALIB/src to /home/andreas/Software/MEGAlib/src
  static void ExpandFileName(TString& FileName, const TString& WorkingDir = "");
  // Make RelFileName relative to AbsFileName
  // Relative: /home/andreas/Test/MyFile.tra
  // Absolute: /home/andreas/MasterTest.tra
  // Return:   ./Test/MyFile.tra
  static TString RelativeFileName(TString RelFileName, TString AbsFileName);

  //! The file modes: Write to a new file  
  static unsigned int c_Write;
  //! The file modes: Write to a new file  
  static unsigned int c_Create;
  //! The file modes: Read from an existing file  
  static unsigned int c_Read;

  // protected methods:
 protected:

  // private methods:
 private:

  //public members
 public:

  // protected members:
 protected:
  //! Name of the file (empty string if not yet open)
  TString m_FileName;

  TString m_FileType;
  int m_Version;

  //! True if the file is open
  bool m_IsOpen;      

  //! The Mode: read or write
  unsigned int m_Way;

  //! The basic file stream
  fstream m_File;

  //! The known file length in READ mode
  streampos m_FileLength;
  //! The maximum allowed file length
  streamsize m_MaxFileLength;

  //! The Progress bar
  MGUIProgressBar* m_Progress;
  //! Files might be deeply nested - show a progressbar for every nesting
  int m_ProgressLevel;
  //! True, if the progress bar has been created here
  bool m_OwnProgress;

  //! True if the original file was compress
  bool m_WasZipped;
  //! FileName of the original zipped file
  TString m_ZippedFileName;

  //! String indicating an unknwon file type
  static const TString c_TypeUnknown;
  //! ID indicating an unknown version ID
  static const int c_VersionUnknown;

  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(MFile, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
