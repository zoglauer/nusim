/*
 * MStreams.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MStreams__
#define __MStreams__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <ostream>
#include <sstream>
#include <string>
#include <streambuf>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
using namespace std;

// ROOT libs:
#include <TString.h>
#include <TThread.h>

// MEGAlib libs:
#include "MGlobal.h"
#include "MStreamBuffer.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MStreams: public ostream
{
  // public interface:
public:
  MStreams();
  virtual ~MStreams() {}
  
  void Enable(bool Enable) { TThread::Lock(); m_Streams->Enable(Enable); TThread::UnLock(); }

  void SetHeader(TString Header) { TThread::Lock(); m_Streams->SetHeader(Header); TThread::UnLock(); }
  void SetPrefix(TString Prefix) { TThread::Lock(); m_Streams->SetPrefix(Prefix); TThread::UnLock(); }
  void SetRejection(TString Rejection) { TThread::Lock(); m_Streams->SetRejection(Rejection); TThread::UnLock(); }

  bool Connect(TString FileName, bool Append = false, bool TimePrefix = false) {
    TThread::Lock(); 
    bool Ret = m_Streams->Connect(FileName, Append, TimePrefix);
    TThread::UnLock();     
    return Ret;
  }
  bool Disconnect(TString FileName) {
    TThread::Lock(); 
    bool Ret =  m_Streams->Disconnect(FileName);
    TThread::UnLock();     
    return Ret;
  }

  void DumpToStdOut(bool Enable) { TThread::Lock(); m_Streams->DumpToStdOut(Enable); TThread::UnLock(); }
  void DumpToStdErr(bool Enable) { TThread::Lock(); m_Streams->DumpToStdErr(Enable); TThread::UnLock(); }
  void DumpToGui(bool Enable) { TThread::Lock(); m_Streams->DumpToGui(Enable); TThread::UnLock(); }

  void ShowOnce() { TThread::Lock(); m_Streams->ShowOnce(); TThread::UnLock(); }

	MStreams& printf(const char* Format, ...);

  // private members:
 private:
  MStreamBuffer* m_Streams;


#ifdef ___CINT___
 public:
  ClassDef(MStreams, 0) // no description
#endif
};


////////////////////////////////////////////////////////////////////////////////


ostream& endl(ostream& l);
ostream& show(ostream& l);
ostream& info(ostream& l);
ostream& warn(ostream& l);
ostream& error(ostream& l);
ostream& logonly(ostream& l);
ostream& fatal(ostream& l);


////////////////////////////////////////////////////////////////////////////////


extern MStreams mout;
extern MStreams mlog;
extern MStreams __merr;
extern MStreams mgui;

#ifdef NDEBUG
#define merr __merr
#else
#define merr __merr<<"!!!!! Error in file \""<<__FILE__<<"\" in function \""<<__func__<<"\" at line "<<__LINE__<<":"<<endl
#endif


// Safe but slow version
//std::ostream mnull(static_cast<std::streambuf*>(0));
//#define mdebug  ((Level >= 1) ? mlog : mnull)
//#define mdebug1 ((Level >= 1) ? mlog : mnull)
//#define mdebug2 ((Level >= 1) ? mlog : mnull)
//#define mdebug3 ((Level >= 1) ? mlog : mnull)
//#define mdebug4 ((Level >= 1) ? mlog : mnull)
//#define mdebug5 ((Level >= 1) ? mlog : mnull)

// Fast but unsafe version
#define mdebug  if (g_DebugLevel >= 2) mlog
#define mdebug1 if (g_DebugLevel >= 1) mlog
#define mdebug2 if (g_DebugLevel >= 2) mlog
#define mdebug3 if (g_DebugLevel >= 3) mlog
#define mdebug4 if (g_DebugLevel >= 4) mlog
#define mdebug5 if (g_DebugLevel >= 5) mlog


extern MStreams __mdep;
#define mdep { ostringstream __oss; __oss<<"Depreciated use of function \""<<__func__<<"\" in file "<<__FILE__<<" before line "<<__LINE__<<":"; __mdep.SetHeader(__oss.str()); }  { ostringstream __oss; __oss<<__func__<<" "<<__FILE__<<" "<<__LINE__; __mdep.SetRejection(__oss.str()); } __mdep.SetPrefix("  "); if (g_DebugLevel >= 1) __mdep

extern MStreams __mimp;
#define mimp { ostringstream __oss; __oss<<"***** Implementation limitation in function \""<<__func__<<"\" in file "<<__FILE__<<" around line "<<__LINE__<<":"; __mimp.SetHeader(__oss.str()); }  { ostringstream __oss; __oss<<__func__<<" "<<__FILE__<<" "<<__LINE__; __mimp.SetRejection(__oss.str()); } __mimp.SetPrefix("      "); if (g_DebugLevel >= 1) __mimp


#endif


////////////////////////////////////////////////////////////////////////////////
