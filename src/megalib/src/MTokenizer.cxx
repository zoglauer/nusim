/*
 * MTokenizer.cxx
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
// MTokenizer
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MTokenizer.h"

// ROOT libs:
#include "TFormula.h"
#include "TString.h"

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MTokenizer)
#endif


////////////////////////////////////////////////////////////////////////////////


MTokenizer::MTokenizer()
{
  // Construct a object of type MTokenizer

  Init();
}


////////////////////////////////////////////////////////////////////////////////


MTokenizer::MTokenizer(const char Separator, const bool AllowComposed)
{
  // Construct a object of type MTokenizer and analyse the token
  // Separator is the separator between keywords
  // When AllowComposed == true, then elements with "." such as "Sphere.Source" 
  // are split in the two tokens "Sphere" and "Source", otherwise it is one token 

  Init();
  m_Separator = Separator;
  m_AllowComposed = AllowComposed;
}


////////////////////////////////////////////////////////////////////////////////


MTokenizer::MTokenizer(const TString& Text, const char Separator, const bool AllowComposed)
{
  // Construct a object of type MTokenizer and analyse the token
  // Separator is the separator between keywords
  // When AllowComposed == true, then elements with "." such as "Sphere.Source" 
  // are split in the two tokens "Sphere" and "Source", otherwise it is one token 

  Init();
  m_Separator = Separator;
  m_AllowComposed = AllowComposed;

  Analyse(Text);
}


////////////////////////////////////////////////////////////////////////////////


MTokenizer::~MTokenizer()
{
  // default destructor
}


////////////////////////////////////////////////////////////////////////////////


void MTokenizer::Init()
{
  // Action common to all constructors:

  m_Composited = false;
  m_AllowComposed = true;
  m_AllowEmpty = false;
  m_Separator = ' ';
}


////////////////////////////////////////////////////////////////////////////////


void MTokenizer::SetSeparator(const char Separator)
{
  // Set the type of separator (space, semicolon...)
  m_Separator = Separator;
}


////////////////////////////////////////////////////////////////////////////////


void MTokenizer::AllowComposed(const bool Composed)
{
  // Allow composed tokens like Bla.blabla
  m_AllowComposed = Composed;
}


////////////////////////////////////////////////////////////////////////////////


void MTokenizer::Reset()
{
  // Action common to all constructors:

  m_Tokens.clear();
  m_IsEmpty.clear();
  m_Composited = false;
}


////////////////////////////////////////////////////////////////////////////////


TString MTokenizer::GetText() const
{
  // Return the base text which has been analysed

  return m_Text;
}


////////////////////////////////////////////////////////////////////////////////


int MTokenizer::GetNTokens() const
{
  // Return the number of tokens

  return m_Tokens.size();
}


////////////////////////////////////////////////////////////////////////////////


TString MTokenizer::GetTokenAt(const int i) const
{
  // Return the token at position i

  if (i >= 0 && i < GetNTokens()) {
    return m_Tokens[i];
  } else {
    mlog<<"TString MTokenizer::GetTokenAt(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return TString("");
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::IsTokenAtEmpty(const int i) const
{
  // Return the token at position i

  if (i >= 0 && i < GetNTokens()) {
    return m_IsEmpty[i];
  } else {
    mlog<<"TString MTokenizer::IsTokenAtEmpty(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return TString("");
  }
}


////////////////////////////////////////////////////////////////////////////////


TString MTokenizer::GetTokenAtAsString(const int i) const
{
  // Return the token at position i as TString

  // Error protection is done in GetTokenAt(int i)
  return GetTokenAt(i);
}


////////////////////////////////////////////////////////////////////////////////


TString MTokenizer::GetTokenAfterAsString(const int i) const
{
  // Return the token at position i and higher as TString

  TString T;
  for (int j = i; j < GetNTokens(); j++) {
    if (j < GetNTokens()-1) {
      T += GetTokenAt(j) + " ";
    } else {
      T += GetTokenAt(j);
    }
  }

  return T;
}


////////////////////////////////////////////////////////////////////////////////


double MTokenizer::GetTokenAtAsDouble(const int i) const
{
  // Return the token at position i as double 


  if (i >= 0 && i < GetNTokens()) {
    return atof(m_Tokens[i]);
  } else {
    mlog<<"TString MTokenizer::GetTokenAtAsDouble(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


float MTokenizer::GetTokenAtAsFloat(const int i) const
{
  // Return the token at position i as double 


  if (i >= 0 && i < GetNTokens()) {
    return atof(m_Tokens[i]);
  } else {
    mlog<<"TString MTokenizer::GetTokenAtAsFloat(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


int MTokenizer::GetTokenAtAsInt(const int i) const
{
  // Return the token at position i as integer


  if (i >= 0 && i < GetNTokens()) {
    return int(m_Tokens[i].Atof()); // atoi(m_Tokens[i]); is not working in cases such as 2.35e+02
  } else {
    mlog<<"int MTokenizer::GetTokenAtAsInt(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


unsigned int MTokenizer::GetTokenAtAsUnsignedInt(const int i) const
{
  // Return the token at position i as unsigned integer


  if (i >= 0 && i < GetNTokens()) {
    int Value = int(m_Tokens[i].Atof());
    if (Value < 0) {
      mlog<<"int MTokenizer::GetTokenAtAsUnsignedInt(int i)"<<endl;
      mlog<<"Value is negative!!!"<<endl;
      return 0;
    }
    return (unsigned int) Value;
  } else {
    mlog<<"int MTokenizer::GetTokenAtAsUnsignedInt(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


long MTokenizer::GetTokenAtAsLong(const int i) const
{
  // Return the token at position i as long integer

  if (i >= 0 && i < GetNTokens()) {
    return atol(m_Tokens[i]);
  } else {
    mlog<<"long MTokenizer::GetTokenAtAsLong(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


unsigned long MTokenizer::GetTokenAtAsUnsignedLong(const int i) const
{
  // Return the token at position i as long integer

  if (i >= 0 && i < GetNTokens()) {
    return strtoul(m_Tokens[i], NULL, 0);
  } else {
    mlog<<"long MTokenizer::GetTokenAtAsLong(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


TArrayI MTokenizer::GetTokenAtAsIntArray(const int i) const
{
  // Return all tokens from position i to end as integer array

  if (i >= 0 && i < GetNTokens()) {
    TArrayI A(GetNTokens() - i);
    for (int j = i; j < GetNTokens(); ++j) {
      A[j-i] = GetTokenAtAsInt(j);
    }
    return A;
  } else {
    mlog<<"TArrayI MTokenizer::GetTokenAtAsIntArray(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return TArrayI(0);
  }
}


////////////////////////////////////////////////////////////////////////////////


TArrayD MTokenizer::GetTokenAtAsDoubleArray(const int i) const
{
  // Return all tokens from position i to end as integer array

  if (i >= 0 && i < GetNTokens()) {
    TArrayD A(GetNTokens() - i);
    for (int j = i; j < GetNTokens(); ++j) {
      A[j-i] = GetTokenAtAsDouble(j);
    }
    return A;
  } else {
    mlog<<"TArrayD MTokenizer::GetTokenAtAsDoubleArray(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return TArrayD(0);
  }
}


////////////////////////////////////////////////////////////////////////////////


vector<double> MTokenizer::GetTokenAtAsDoubleVector(const int i) const
{
  // Return all tokens from position i to end as integer array

  vector<double> A;
  if (i >= 0 && i < GetNTokens()) {
    for (int j = i; j < GetNTokens(); ++j) {
      A.push_back(GetTokenAtAsDouble(j));
    }
    return A;
  } else {
    mlog<<"vector<double> MTokenizer::GetTokenAtAsDoubleVector(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return A;
  }
}


////////////////////////////////////////////////////////////////////////////////


vector<float> MTokenizer::GetTokenAtAsFloatVector(const int i) const
{
  // Return all tokens from position i to end as integer array

  vector<float> A;
  if (i >= 0 && i < GetNTokens()) {
    for (int j = i; j < GetNTokens(); ++j) {
      A.push_back(GetTokenAtAsFloat(j));
    }
    return A;
  } else {
    mlog<<"vector<float> MTokenizer::GetTokenAtAsFloatVector(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return A;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::GetTokenAtAsBoolean(const int i) const
{
  // Return the token at position i as boolean


  if (i >= 0 && i < GetNTokens()) {
    TString Token = m_Tokens[i];
    
    if (Token.CompareTo("false") == 0 ||
        Token.CompareTo("False") == 0 ||
        Token.CompareTo("FALSE") == 0) {
      return false;
    } else if (Token.CompareTo("true") == 0 ||
               Token.CompareTo("True") == 0 ||
               Token.CompareTo("TRUE") == 0) {
      return true;
    }
    return (atoi(Token) == 0) ? false : true;
  } else {
    mlog<<"bool MTokenizer::GetTokenAtAsBoolean(int i): "<<endl;
    if (GetNTokens() > 0) {
      mlog<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")\n";
      mlog<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    } else {
      mlog<<"The Tokenizer is empty!"<<endl;
    }
    return false;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::IsTokenAt(const int i, const TString& Token, bool const IgnoreCase) const
{
  // Return true, if Token is identical with the token at position i

  if (GetNTokens() == 0) return false;

  if (i >= 0 && i < GetNTokens()) {
    if (IgnoreCase == true) {
      TString LowerCase = Token;
      LowerCase.ToLower();
      TString Original = m_Tokens[i];
      Original.ToLower();
      return (LowerCase == Original) ? true : false;
    } else {
      return (Token == m_Tokens[i]) ? true : false;
    }
  } else {
    merr<<"Index ("<<i<<") out of bounds (min=0, max="<<GetNTokens()-1<<")"<<endl;
    mout<<"The text line was: \""<<m_Text.Data()<<"\""<<endl;
    return false;
  }
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::IsComposited() const
{
  // Return true if the first token was composited, i.e. like "ACS.Material"

  return m_Composited;
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::Analyse(TString Text, const bool AllowMaths)
{
  // Split the Text into its tokens
  // White space is the prime delimiter, then the first token is splitted
  // by a point as delimiter
  // Text surrounded by [...] is one token - splitted tokens are not recognized
  // Text surrounded by {...} is considered as equation - splitted tokens are not recognized

  //cout<<Text<<endl;

  Reset();
  m_Text = Text;

  // Remove white spaces at the beginning of the line:
  Text = Text.Strip(TString::kBoth, ' ').String();
  Text.ReplaceAll("\r", "");

  TString Token;
  bool IsFirstToken = true;
  //bool IsMath = false;
  int MathDepth = 0;
  int MathMatch = 0;

  int Length = Text.Length();
  const char* pText = Text.Data();

  const char* pToken = 0;
  int TokenStart = 0;
  int TokenLength = 0;
  for (int i = 0; i < Length; ++i) {

    // # or ! at the beginning is always a comment
    if (pText[0] == '#' || pText[0] == '!') {
      break;
    }

    if (pText[i] == '{') {
      MathDepth++;
      MathMatch++;
      //IsMath = true;
    }
    if (pText[i] == '}') {
      MathMatch--;
    }

    if (TokenLength > 0 && pText[i-TokenLength] == '[') {
      TokenLength++;
      if (i < Length-1 && pText[i] != ']') continue;
    } else if (TokenLength > 0 && pText[i-TokenLength] == '{') {
      TokenLength++;
      if (i < Length-1 && pText[i] != '}') {
        continue;
      } else if (i < Length-1 && pText[i] == '}') {
        MathDepth--;
        if (MathDepth != 0) continue;
      }
    } else if (pText[i] != m_Separator && pText[i] != '\t' && pText[i] != '\n' && pText[i] != '\0') { 
      TokenLength++;
      if (i < Length-1) continue;
    }

    // Now we have to consider several cases:
    // (a) The token is empty --> skip this token if m_AllowEmtpy == false
    // (b) The token starts with '//', then this AND all following tokens are comment
    // (c) The token starts with [, then it ends with ]
    // (d) the good ones

    Token = Text(TokenStart, TokenLength);
    pToken = Token.Data();

    if (TokenLength == 0) {
      if (m_AllowEmpty == true) {
        m_Tokens.push_back("");
        IsFirstToken = false;
        m_IsEmpty.push_back(true);
      } else {
        TokenStart = i+1;
        continue;
      }
    } else if (TokenLength > 1 && (pToken[0] == '/' && pToken[1] == '/')) {
      // Standard comment
      break;
    } else if (TokenLength > 1 && (pToken[0] == '#' && pToken[1] == '#')) {
      // Standard comment
      break;
    } else if (TokenLength > 0 && pToken[0] == '[' && pToken[TokenLength-1] == ']') {
      m_Tokens.push_back(Token);
      m_IsEmpty.push_back(false);
      IsFirstToken = false;
    } else if (TokenLength > 0 && pToken[0] == '{' && pToken[TokenLength-1] == '}') {
      if (AllowMaths == true) {
        if (EvaluateMaths(Token) == false) {
          merr<<"Unable to scan math token \""<<Token<<"\"correctly!"<<endl;
          return false;
        }
      }
      m_Tokens.push_back(Token);
      m_IsEmpty.push_back(false);
      IsFirstToken = false;
    } else {
      // Now we have a good token.
      // If it is the first token, then split it again by a '.' (dot)
      //mlog<<"Good: "<<Token<<endl;
			//cout<<"Composed"<<(int) IsFirstToken<<"!"<<(int) m_AllowComposed<<endl;
      if (IsFirstToken == true && m_AllowComposed == true) {
        IsFirstToken = false;
        if (Token.Contains('.') == true) {
          // Add the token before the dot ...
          m_Tokens.push_back(Token(0, Token.First('.')));
          m_IsEmpty.push_back(false);
          // after the dot ...
          m_Tokens.push_back(Token(Token.First('.')+1, Token.Length()));
          m_IsEmpty.push_back(false);
          // This was a composited first token
          m_Composited = false;
        } else {
          m_Tokens.push_back(Token);
          m_IsEmpty.push_back(false);
          //mlog<<"Added: "<<Token<<endl;
        }
      } else {
        m_Tokens.push_back(Token);
        m_IsEmpty.push_back(false);
        IsFirstToken = false;
      }
    }

    if (pText[i] == '\n' || pText[i] == '\0') {
      break;
    }
    TokenStart = i+1;
    TokenLength = 0;
  }

  if (m_AllowEmpty == true && Length > 0 && pText[Length-1] == m_Separator) {
    m_Tokens.push_back("");
    m_IsEmpty.push_back(true);
  }


  //cout<<ToString()<<endl;

  if (MathMatch != 0) {
    merr<<"There was an error with the math environment. The number of { and } do not match!"<<endl;
    merr<<pText<<show;
    return false;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::IsMaths(const TString& Token)
{
  // Test if Token is a math expression

  if (Token.BeginsWith("{") && Token.EndsWith("}")) {
    return true;
  }

  return false;
}


////////////////////////////////////////////////////////////////////////////////


bool MTokenizer::EvaluateMaths(TString& Token)
{
  // Evaluate the maths expressions in Token

  if (IsMaths(Token) == false) return false;

  Token.ReplaceAll("{", " ");
  Token.ReplaceAll("}", " ");

  // First check, if the token is evaluable:
  TFormula Formula;
  if (Formula.Compile(Token) != 0) {
    return false;
  } else {
    ostringstream out;
    out<<Formula.Eval(0.0);
    Token = out.str().c_str();
    return true;
  }
}


////////////////////////////////////////////////////////////////////////////////


TString MTokenizer::ToString()
{
  ostringstream out;

  if (GetNTokens() > 0) {
    out<<"Tokenizer content ("<<GetNTokens()<<" Tokens):"<<endl;
    int i;
    for (i = 0; i < GetNTokens(); i++) {
      if (m_IsEmpty[i] == true) {
        out<<"["<<i<<"]: *empty token*"<<endl;        
      } else {
        out<<"["<<i<<"]: \""<<GetTokenAt(i)<<"\""<<endl;
      }
    }
  } else {
    out<<"Tokenizer empty!"<<endl;
  }

  return out.str().c_str();
}


// MTokenizer.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
