/*
 * MTokenizer.h
 *
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MTokenizer__
#define __MTokenizer__


////////////////////////////////////////////////////////////////////////////////


// ROOT libs:
#include <TString.h>
#include <TArrayI.h>
#include <TArrayD.h>

// MEGAlib libs:
#include "MGlobal.h"

// Standard libs:
#include <cstdlib>
#include <vector>
using namespace std;

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MTokenizer
{
  // public interface:
 public:
  //! Default constructor
  MTokenizer();
  //! Standard constructor
  //! Separator is the separator between keywords
  //! When AllowComposed == true, then elements with "." such as "Sphere.Source" 
  //! are split in the two tokens "Sphere" and "Source", otherwise it is one token 
  MTokenizer(const char Separator, const bool AllowComposed = true);
  //! Standard constructor which automatically analyses the text
  //! Separator is the separator between keywords
  //! When AllowComposed == true, then elements with "." such as "Sphere.Source" 
  //! are split in the two tokens "Sphere" and "Source", otherwise it is one token 
  MTokenizer(const TString& Text, const char Separator = ' ', const bool AllowComposed = true);
  virtual ~MTokenizer();

  //! Set the separator between the tokens (default: space) not allowed are "{", "[", "}". "]", "!", "#"
  void SetSeparator(const char Separator);
  //! When true, then elements with "." such as "Sphere.Source" 
  //! are split in the two tokens "Sphere" and "Source", otherwise it is one token 
  void AllowComposed(const bool Composed);
  //! If true empty token are OK, e.g. if "," is a separator then ",," results in 3 empty tokens
  //! This does not work if spaces are used as separators!
  void AllowEmpty(const bool Empty) { m_AllowEmpty = Empty; }
  
  //! Split the text into tokens
  bool Analyse(TString Text, const bool AllowMaths = true);
  //! Same as Analyse
  bool Analyze(TString Text, const bool AllowMaths = true) { return Analyse(Text, AllowMaths); }

  //! Return the original text
  TString GetText() const;

  //! Return the number of tokens
  int GetNTokens() const;
  //! Check if the given Token is at position i
  bool IsTokenAt(const int i, const TString& Token, const bool IgnoreCase = false) const;
  //! Check if the irst token is composed i.e. something like "Sphere.Source"
  bool IsComposited() const;

  //! Return the token at i as string
  TString GetTokenAt(const int i) const;
  //! Return true is the given token  is empty
  bool IsTokenAtEmpty(const int i) const;

  //! Return the token at i as string --- return "" in case of error
  TString GetTokenAtAsString(const int i) const;
  //! Return all token AT AND AFTER i as string --- return "" in case of error
  TString GetTokenAfterAsString(const int i) const;
  //! Return the token at i as double --- return 0 in case of error
  double GetTokenAtAsDouble(const int i) const;
  //! Return the token at i as float --- return 0 in case of error
  float GetTokenAtAsFloat(const int i) const;
  //! Return the token at i as int --- return 0 in case of error
  int GetTokenAtAsInt(const int i) const;
  //! Return the token at i as unsigned int --- return 0 in case of error
  unsigned int GetTokenAtAsUnsignedInt(const int i) const;
  //! Return the token at i as long --- return 0 in case of error
  long GetTokenAtAsLong(const int i) const;
  //! Return the token at i as unsigned long --- return 0 in case of error
  unsigned long GetTokenAtAsUnsignedLong(const int i) const;
  //! Return the token at i as double --- return FALSE in case of error
  bool GetTokenAtAsBoolean(const int i) const;
  //! Return the token AT AND AFTER i as ROOT TArrayI --- return 0 in case of error
  TArrayI GetTokenAtAsIntArray(const int i) const;
  //! Return the token AT AND AFTER i as ROOT TArrayD --- return 0 in case of error
  TArrayD GetTokenAtAsDoubleArray(const int i) const;
  //! Return the token AT AND AFTER i as vector of doubles --- return empty array in case of error
  vector<double> GetTokenAtAsDoubleVector(const int i) const;
  //! Return the token AT AND AFTER i as vector of maths --- return empty array in case of error
  vector<float> GetTokenAtAsFloatVector(const int i) const;

  //! Evaluate the math functions in the given string
  static bool EvaluateMaths(TString& Token);
  //! Check if the string contains a math environment "{ ... }"
  static bool IsMaths(const TString& Token);

  //! Return a diagnostics string of the data content
  TString ToString();


  // protected methods:
 protected:
  //! Initializes all member variables to default values
  void Init();
  //! Resets the data of the tokenizer, but leaves m_Separator and m_AllowComposed unchanged
  void Reset();

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The original text
  TString m_Text;
  //! The original text split into tokens
  vector<TString> m_Tokens;
  //! Indicating that the given token was empty
  vector<bool> m_IsEmpty;

  //! The separator e.g. a space
  char m_Separator;

  //! When true, then elements with "." such as "Sphere.Source" 
  //! are split in the two tokens "Sphere" and "Source", otherwise it is one token 
  bool m_AllowComposed;
  //! Return whether the first token was composed e.g. "Sphere.Source"
  bool m_Composited;
  //! True, if empty token can be used
  bool m_AllowEmpty;

#ifdef ___CINT___
 public:
  ClassDef(MTokenizer, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
