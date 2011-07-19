/*
 * MXmlNode.cxx
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
// MXmlNode
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "MXmlNode.h"

// Standard libs:
#include <algorithm>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MAssert.h"
#include "MStreams.h"
#include "MXmlNode.h"


////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(MXmlNode)
#endif


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode() : MXmlData()
{
  // Construct an instance of MXmlNode
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name) : MXmlData(Name)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, TString Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, int Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, long Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, unsigned int Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, double Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, bool Value) : MXmlData(Name, Value)
{
  //! Constructor

  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, MVector Value)
{
  //! Constructor

  m_Name = Name;
  m_Value = "<X>";
  m_Value += TString::Format("%.8E", Value.X()).Strip(TString::kBoth);
  m_Value += "</X><Y>";
  m_Value += TString::Format("%.8E", Value.Y()).Strip(TString::kBoth);
  m_Value += "</Y><Z>";
  m_Value += TString::Format("%.8E", Value.Z()).Strip(TString::kBoth);
  m_Value += "</Z>";
  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, int ValueMin, int ValueMax)
{
  //! Constructor

  m_Name = Name;
  m_Value = "<Min>";
  m_Value += TString::Format("%i", ValueMin).Strip(TString::kBoth);
  m_Value += "</Min><Max>";
  m_Value += TString::Format("%i", ValueMax).Strip(TString::kBoth);
  m_Value += "</Max>";
  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::MXmlNode(MXmlNode* MotherNode, TString Name, double ValueMin, double ValueMax)
{
  //! Constructor

  m_Name = Name;
  m_Value = "<Min>";
  m_Value += TString::Format("%.8E", ValueMin).Strip(TString::kBoth);
  m_Value += "</Min><Max>";
  m_Value += TString::Format("%.8E", ValueMax).Strip(TString::kBoth);
  m_Value += "</Max>";
  if (MotherNode != 0) {
    MotherNode->AddNode(this);
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode::~MXmlNode()
{
  // Delete this instance of MXmlNode

  for (unsigned int n = 0; n < m_Nodes.size(); ++n) {
    delete m_Nodes[n];
  }
  for (unsigned int a = 0; a < m_Attributes.size(); ++a) {
    delete m_Attributes[a];
  }
}


////////////////////////////////////////////////////////////////////////////////


void MXmlNode::Clear()
{
  // Reset the node

  MXmlData::Clear();
  m_Nodes.clear();
  m_Attributes.clear();
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* MXmlNode::GetNode(unsigned int i)
{
  //! Return a given node
  if (i < m_Nodes.size()) {
    return m_Nodes.at(i);
  } else {
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlNode* MXmlNode::GetNode(TString Name)
{
  //! Return a node

  for (unsigned int m = 0; m < m_Nodes.size(); ++m) {
    if (m_Nodes[m]->GetName() == Name) {
      return m_Nodes[m];      
    }
  }
 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


void MXmlNode::AddNode(MXmlNode* Node)
{
  //! Add a node
  m_Value = "";
  return m_Nodes.push_back(Node);
}

 
////////////////////////////////////////////////////////////////////////////////


bool MXmlNode::RemoveNode(MXmlNode* Node)
{
  //! Remove node either from this node or from one of its daughters

  vector<MXmlNode*>::iterator I = find(m_Nodes.begin(), m_Nodes.end(), Node);
  if (I != m_Nodes.end()) {
    m_Nodes.erase(I);
    return true;
  } else {
    for (unsigned int n = 0; n < m_Nodes.size(); ++n) {
      if (m_Nodes[n]->RemoveNode(Node) == true) {
        return true;
      }
    } 
  }
  
  return false;
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute* MXmlNode::GetAttribute(unsigned int i)
{
  //! Return a given attribute
  if (i < m_Attributes.size()) {
    return m_Attributes.at(i);
  } else {
    return 0;
  }
}


////////////////////////////////////////////////////////////////////////////////


MXmlAttribute* MXmlNode::GetAttribute(TString Name)
{
  //! Return a attribute

  for (unsigned int m = 0; m < m_Attributes.size(); ++m) {
    if (m_Attributes[m]->GetName() == Name) {
      return m_Attributes[m];      
    }
  }
 
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


void MXmlNode::AddAttribute(MXmlAttribute* Attribute)
{
  //! Add a attribute
  return m_Attributes.push_back(Attribute);
}


////////////////////////////////////////////////////////////////////////////////


MVector MXmlNode::GetValueAsVector() const
{
  // Return as a vector

  bool IsGood = false;
  MVector Value;
  
  if (m_Nodes.size() == 3) {
    if (m_Nodes[0]->GetName() == "X" && m_Nodes[1]->GetName() == "Y" && m_Nodes[2]->GetName() == "Z") {
      Value.SetX(m_Nodes[0]->GetValueAsDouble());
      Value.SetY(m_Nodes[1]->GetValueAsDouble());
      Value.SetZ(m_Nodes[2]->GetValueAsDouble());
      IsGood = true;
    }
  }

  if (IsGood == false) {
    mout<<"Xml: Value error in node \""<<m_Name<<"\" --- Value is not a vector!"<<endl;
  }

  return Value;
}


////////////////////////////////////////////////////////////////////////////////


int MXmlNode::GetMinValueAsInt() const
{
  //! Return the minimum value of the node as int

  if (m_Nodes.size() == 2) {
    if (m_Nodes[0]->GetName() == "Min" && m_Nodes[1]->GetName() == "Max") {
      return m_Nodes[0]->GetValueAsInt();
    }
  }

  mout<<"Xml: Value error in node \""<<m_Name<<"\" --- Value is not a min/max node!"<<endl;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


int MXmlNode::GetMaxValueAsInt() const
{
  //! Return the minimum value of the node as int

  if (m_Nodes.size() == 2) {
    if (m_Nodes[0]->GetName() == "Min" && m_Nodes[1]->GetName() == "Max") {
      return m_Nodes[1]->GetValueAsInt();
    }
  }

  mout<<"Xml: Value error in node \""<<m_Name<<"\" --- Value is not a min/max node!"<<endl;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


double MXmlNode::GetMinValueAsDouble() const
{
  //! Return the value of the node as double

  if (m_Nodes.size() == 2) {
    if (m_Nodes[0]->GetName() == "Min" && m_Nodes[1]->GetName() == "Max") {
      return m_Nodes[0]->GetValueAsDouble();
    }
  }

  mout<<"Xml: Value error in node \""<<m_Name<<"\" --- Value is not a min/max node!"<<endl;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


double MXmlNode::GetMaxValueAsDouble() const
{
  //! Return the value of the node as double

  if (m_Nodes.size() == 2) {
    if (m_Nodes[0]->GetName() == "Min" && m_Nodes[1]->GetName() == "Max") {
      return m_Nodes[1]->GetValueAsDouble();
    }
  }

  mout<<"Xml: Value error in node \""<<m_Name<<"\" --- Value is not a min/max node!"<<endl;
  return 0;
}


////////////////////////////////////////////////////////////////////////////////


void MXmlNode::Find(const TString& Name, vector<MXmlNode*>& Nodes)
{
  //! Find all nodes with the given Name

  if (m_Name == Name) {
    Nodes.push_back(this);
  }
  
  for (unsigned int n = 0; n < m_Nodes.size(); ++n) {
    m_Nodes[n]->Find(Name, Nodes);
  }
}


////////////////////////////////////////////////////////////////////////////////


TString MXmlNode::ToString(unsigned int Indent)
{
  //! Returns the XML text

  TString Ind;
  for (unsigned int i = 0; i < Indent; ++i) Ind += " ";

  TString Xml;
  Xml += Ind;
  Xml += "<";
  Xml += m_Name;
  for (unsigned int a = 0; a < m_Attributes.size(); ++a) {
    Xml += " ";
    Xml += m_Attributes[a]->ToString();
  }
  if (m_Nodes.size() == 0 && m_Value == "") {
    Xml += " />";    
  } else {
    Xml += ">";
    if (m_Nodes.size() > 0) {
      for (unsigned int n = 0; n < m_Nodes.size(); ++n) {
        Xml += "\n";
        Xml += m_Nodes[n]->ToString(Indent+2);
      }
      Xml += "\n";
      Xml += Ind;
    } else {
      Xml += m_Value;
    } 
    Xml += "</";
    Xml += m_Name;
    Xml += ">";
  }

  return Xml;
}


////////////////////////////////////////////////////////////////////////////////


bool MXmlNode::Parse(TString Text)
{
  //! Parse text into this node

  Text = Text.Strip(TString::kBoth);

  // For all nodes with:
  while (Text.Length() >= 2 && Text[0] == '<' && Text[Text.Length()-1] == '>') {
    int FirstBegin = Text.Index('<', 1, 0, TString::kExact);
    int FirstEnd = Text.Index('>', 1, 0, TString::kExact);

    // Check for comments "<!--"
    if (Text[1] == '!' && Text.Length() >= 4 && Text[2] == '-' && Text[3] == '-') {
      FirstEnd = Text.Index("-->", 3, 0, TString::kExact);
      Text = Text.Replace(FirstBegin, FirstEnd-FirstBegin+3, "");
      continue;
    }

    // Check for specials "<?"
    if (Text[1] == '?') {
      FirstEnd = Text.Index("?>", 2, 0, TString::kExact);
      Text = Text.Replace(FirstBegin, FirstEnd-FirstBegin+2, "");
      continue;
    }

    TString Name = Text(FirstBegin+1, FirstEnd-FirstBegin-1);
    Name = Name.Strip(TString::kBoth);
    // Check if it is an empty node
    int Empty = Name.Index('/');
    // Make sure take care of attributes
    int FirstAttribute = Name.Index(' ');
    TString Attributes("");
    if (FirstAttribute != kNPOS) {
      Attributes = Name(FirstAttribute+1, Name.Length() - FirstAttribute - 1);
      Name = Name(0, FirstAttribute);
    } 
    if (Name.Length() == 0) {
      merr<<"Xml: Parse error in node \""<<m_Name<<"\" --- empty name"<<endl;
      return false;
    }
    
    // Create a new node and start the game again:
    MXmlNode* Node = new MXmlNode(this, Name);

    // Parse its content if there is any:
    if (Empty == kNPOS) {
      int LastBegin = Text.Index(TString("</") + Name + TString(">"));
      int LastSize = (TString("</") + Name + TString(">")).Length();
      
      if (LastBegin == -1) {
        merr<<"Xml: Parse error in node \""<<m_Name<<"\" --- cannot find: "<<TString("</") + Name + TString(">")<<endl;
        return false;
      }
    
      // And parse the new node
      Node->Parse(Text(FirstEnd+1, LastBegin-FirstEnd-1));
      Text = Text.Replace(0, LastBegin+LastSize, "");
      
    } else {
      // It's empty (<Name />) thus we only eliminated this tag
      Text = Text.Replace(0, FirstEnd+1, "");
    }
    Text = Text.Strip(TString::kBoth);


    // Parse its attributes:
    Attributes = Attributes.Strip(TString::kBoth);
    int Equal = kNPOS;
    while ((Equal = Attributes.Index('=')) != kNPOS) {
      TString AttributeName = Attributes(0, Equal);
      AttributeName = AttributeName.Strip(TString::kBoth);
      
      int FirstQuote = Attributes.Index('"');
      if (FirstQuote == kNPOS) {
        merr<<"Xml: Parse error in node \""<<m_Name<<"\" --- attribute not well formed"<<endl;
        return false;
      }
      int SecondQuote = Attributes.Index("\"", 1, FirstQuote+1, TString::kExact);
      if (SecondQuote == kNPOS) {
        merr<<"Xml: Parse error in node \""<<m_Name<<"\" --- attribute not well formed"<<endl;
        return false;
      }
      TString AttributeValue = Attributes(FirstQuote+1, SecondQuote-FirstQuote-1);
      
      new MXmlAttribute(Node, AttributeName, AttributeValue);
      
      Attributes = Attributes(SecondQuote+1, Attributes.Length() - SecondQuote-1);
      Attributes = Attributes.Strip(TString::kBoth);
    }
  } // all nodes loop

  // Now only the pure text content is left - the "value"
  if (Text.Length() > 0) {
    if (m_Nodes.size() > 0) {
      merr<<"Xml: Parse error in node \""<<m_Name<<"\" --- node has text and sub-nodes. Remaining text: "<<Text<<endl;
      return false;      
    } else {
      m_Value = Text;
    }
  }

  return true;
}


// MXmlNode.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
