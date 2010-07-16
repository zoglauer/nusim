/*
 * MFileManager.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MFileManager__
#define __MFileManager__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>

// MEGAlib libs:
#include "MGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MFileManager : public TObject
{
  // public interface:
 public:
  MFileManager();
  virtual ~MFileManager();

  TString SearchFile();
  bool SelectFileToLoad(TString& DefaultName, const char** FileTypes = 0);
  bool SelectFileToSave(TString& DefaultName, const char** FileTypes = 0);
  int ErrorCode();

	static TString MakeRelativePath(TString FileName, TString Alias)

/* //////////////////////////////////////////////////////////////////////////////// */


/* TString MFileManager::MakeRelativePath(TString FileName, TString Alias) */
{
	// 

	gSystem->ExpandPathName(FileName);

	// First get the real path of the alias
	TString AliasPath = Alias;
	gSystem->ExpandPathName(AliasPath);

	// Then compare if FileName starts with AliasPath
	if (FileName.BeginsWith(AliasPath) == true) {
		// If yes replace AliasPath with Alias and return
		FileName.ReplaceAll(AliasPath, Alias);
	}
		
	// Otherwise return the original path
	return FileName;
};

	static TString MakeAbsolutePath(TString FileName);
  static bool FileExists(TString Filename);
  static bool DirectoryExists(TString Filename);

  enum Errors { NoError, Error };

  // protected methods:
 protected:


  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  int m_ErrorCode;


#ifdef ___CINT___
 public:
  ClassDef(MFileManager, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
