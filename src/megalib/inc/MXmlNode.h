/*
 * MXmlNode.h
 *
 * Copyright (C) 2005-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MXmlNode__
#define __MXmlNode__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <cstdlib>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MGlobal.h"
#include "MXmlData.h"
#include "MXmlAttribute.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class MXmlNode : public MXmlData
{
  // public interface:
 public:
  //! Default constructor
  MXmlNode();
  //! Constructor -- no values
  MXmlNode(MXmlNode* MotherNode, TString Name);
  //! Constructor -- content is string
  MXmlNode(MXmlNode* MotherNode, TString Name, TString Value);
  //! Constructor -- content is integer
  MXmlNode(MXmlNode* MotherNode, TString Name, int Value);
  //! Constructor -- content is unsigned integer
  MXmlNode(MXmlNode* MotherNode, TString Name, unsigned int Value);
  //! Constructor -- content is double
  MXmlNode(MXmlNode* MotherNode, TString Name, double Value);
  //! Constructor -- content is a vector
  MXmlNode(MXmlNode* MotherNode, TString Name, MVector Value);
  //! Constructor -- content is a boolean
  MXmlNode(MXmlNode* MotherNode, TString Name, bool Value);
  //! Constructor -- content is an integer min/max-value
  MXmlNode(MXmlNode* MotherNode, TString Name, int ValueMin, int ValueMax);
  //! Constructor content is a double min/max-value
  MXmlNode(MXmlNode* MotherNode, TString Name, double ValueMin, double ValueMax);
  //! Default destructor
  virtual ~MXmlNode();

  //! Reset the node
  virtual void Clear();

  //! Return the value of the node as vector
  MVector GetValueAsVector() const;
  //! Return the minimum value of the node as int
  int GetMinValueAsInt() const;
  //! Return the maximum value of the node as int
  int GetMaxValueAsInt() const;
  //! Return the minimum value of the node as double
  double GetMinValueAsDouble() const;
  //! Return the maximum value of the node as double
  double GetMaxValueAsDouble() const;


  //! Return the number of nodes
  unsigned int GetNNodes() { return m_Nodes.size(); }
  //! Return a given node
  MXmlNode* GetNode(unsigned int i);
  //! Return a given node
  MXmlNode* GetNode(TString Name);
  //! Add a node
  void AddNode(MXmlNode* Node);
  //! Remove node either from this node or from one of its daughters
  bool RemoveNode(MXmlNode* Node);

  //! Return the number of attributes
  unsigned int GetNAttributes() { return m_Attributes.size(); }
  //! Return a given attribute
  MXmlAttribute* GetAttribute(unsigned int i);
  //! Return a given attribute
  MXmlAttribute* GetAttribute(TString Name);
  //! Add a attribute
  void AddAttribute(MXmlAttribute* Attribute);


  //! Returns the XML text
  virtual TString ToString(unsigned int Indent = 0);

  //! Find all nodes with the given Name
  void Find(const TString& Name, vector<MXmlNode*>& Nodes);


  // protected methods:
 protected:
  //! Parse text into this node
  virtual bool Parse(TString Text);

  // private methods:
 private:



  // protected members:
 protected:
  //! All sub nodes
  vector<MXmlNode*> m_Nodes; 
  //! All attributes
  vector<MXmlAttribute*> m_Attributes; 


  // private members:
 private:


#ifdef ___CINT___
 public:
  ClassDef(MXmlNode, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
