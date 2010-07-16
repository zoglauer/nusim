/*
 * MXmlDocument.cxx
 *
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
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
// MXmlDocument
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MXmlDocument.h"

// Standard libs:

// ROOT libs:

// MEGAlib libs:


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MXmlDocument)
#endif


////////////////////////////////////////////////////////////////////////////////


MXmlDocument::MXmlDocument()
{
  // Construct an instance of MXmlDocument
}


////////////////////////////////////////////////////////////////////////////////


MXmlDocument::MXmlDocument(TString Name)
{
  // Construct an instance of MXmlDocument

  m_Name = Name;
}


////////////////////////////////////////////////////////////////////////////////


MXmlDocument::~MXmlDocument()
{
  // Delete this instance of MXmlDocument
}


////////////////////////////////////////////////////////////////////////////////


bool MXmlDocument::Load(TString FileName)
{
  //! Load an xml document

  TString AllContent;

  ifstream in(FileName); 
  if (in.is_open() == false) { 
    cout<<"Xml parser: Error opening file: "<<FileName<<endl; 
    return false; 
  } 
 
  string tmp;
  while(!in.eof()) {
    getline(in, tmp);
    AllContent += tmp;
    AllContent += "\n";
  }

  // Remove all comments:
  int Begin;
  int End;
  while ((Begin = AllContent.Index("<!--", 4, 0, TString::kExact)) != kNPOS) {
    if (AllContent.Index(">", 1, Begin, TString::kExact) < AllContent.Index("-->", 1, Begin, TString::kExact)) {
      cout<<"Xml parser: Error with comments!"<<endl; 
      return false; 
    }
    End = AllContent.Index("-->", 3, Begin, TString::kExact);
    AllContent = AllContent.Replace(Begin, End-Begin+3, "");
  }
  while ((Begin = AllContent.Index("<?", 2, 0, TString::kExact)) != kNPOS) {
    if (AllContent.Index(">", 1, Begin, TString::kExact) < AllContent.Index("?>", 2, Begin, TString::kExact)) {
      cout<<"Xml parser: Error with <? ... ?>!"<<endl; 
      return false; 
    }
    End = AllContent.Index("?>", 2, Begin, TString::kExact);
    AllContent = AllContent.Replace(Begin, End-Begin+2, "");
  }

  // Remove all returns:
  AllContent = AllContent.ReplaceAll("\n", "");
  AllContent = AllContent.ReplaceAll("\r", "");

  // Detect the outer name:
  int FirstBegin = AllContent.Index('<', 1, 0, TString::kExact);
  int FirstEnd = AllContent.Index('>', 1, FirstBegin, TString::kExact);

  m_Name = AllContent(FirstBegin+1, FirstEnd-FirstBegin-1);

  // Make sure to ignore attributes
  int FirstAttribute = m_Name.First(' ');
  if (FirstAttribute != kNPOS) {
    m_Name = m_Name(0, FirstAttribute);
  }

  int LastBegin = AllContent.Index(TString("</") + m_Name + TString(">"));

  Parse(AllContent(FirstEnd+1, LastBegin-FirstEnd-1));

  return true;
}


////////////////////////////////////////////////////////////////////////////////


bool MXmlDocument::Save(TString FileName)
{
  //! Save an Xml ldocument

  ofstream out;
  out.open(FileName);
  out<<ToString()<<endl;
  out.close();

  return true;
}


// MXmlDocument.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
