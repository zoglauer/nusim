/*
 * NPointing.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPointing__
#define __NPointing__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "MStreams.h"
#include "MXmlNode.h"

// NuSTAR libs:
#include "NGlobal.h"
#include "NTime.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a space craft pointing 
//! This class represents a space craft pointing. No details are implemented yet

class NPointing
{
  // public interface:
 public:
  //! Standard constructor
  NPointing();
  //! Default destructor
  virtual ~NPointing();

  //! Resets all data to default values
  virtual void Clear();

  //! Set right ascension and declination (do automatic conversion to quaternion)
  void SetRaDecYaw(double Ra, double Dec, double Yaw) { m_Empty = false; m_Ra = Ra; m_Dec = Dec; m_Yaw = Yaw; RaDecToQuaternion(); }
  //! Set a new yaw
  void SetYaw(double Yaw);
  //! Set quaternion (do automatic conversion to quaternion)
  void SetQuaternion(const NQuaternion& Q) { m_Empty = false; m_Q = Q; QuaternionToRaDec(); }
  //! Set the time spent in this pointing
  void SetTime(const NTime& Time) { m_Time = Time; }

  //! Get the right ascension
  double GetRa() const { return m_Ra; }
  //! Get the declination
  double GetDec() const { return m_Dec; }
  //! Get the declination
  double GetYaw() const { return m_Yaw; }
  //! Get the time to be spent at this pointing
  NTime GetTime() const { return m_Time; }
  
  //! Get the quaternion representation
  NQuaternion GetQuaternion() const { return m_Q; }

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }


  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version, TString Keyword = "OP");

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);

  //! Dump content to a string
  TString ToString() const;
  
  //! Read the configuration data from an XML node
  virtual bool ReadXmlConfiguration(MXmlNode* Node);
  //! Create an XML node tree from the configuration
  virtual MXmlNode* CreateXmlConfiguration();


  // protected methods:
 protected:
  //NPointing() {};
  //NPointing(const NPointing& NCTHit) {};

  //! Convert from Ra/Dec to a quaternion
  void RaDecToQuaternion();
  //! Convert from a quaternion to Ra/Dec
  void QuaternionToRaDec();

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The right ascension in arcmin (!)
  double m_Ra;
  //! The declination in arcmin (!)
  double m_Dec;
  //! The yaw of the spacecraft in radians
  double m_Yaw;
  //! Representation of the pointing as a quaternion
  NQuaternion m_Q;
  //! Representation of the yaw quaternion of the spacecraft
  NQuaternion m_QR;
  //! The time to be spent at this pointing
  NTime m_Time;
  
  
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;




#ifdef ___CINT___
 public:
  ClassDef(NPointing, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
