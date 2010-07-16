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
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a space craft pointing --- it's currrently still empty
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
  void SetRaDec(double Ra, double Dec) { m_Empty = false; m_Ra = Ra; m_Dec = Dec; RaDecToQuaternion(); }
  //! Set quaternion (do automatic conversion to quaternion)
  void SetQuaternion(const NQuaternion& Q) { m_Empty = false; m_Q = Q; QuaternionToRaDec(); }

  //! Get the right ascension
  double GetRa() const { return m_Ra; }
  //! Get the declination
  double GetDec() const { return m_Dec; } 
  //! Get the quaternoin representation
  NQuaternion GetQuaternion() const { return m_Q; }

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }


  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, TString Keyword = "OP");

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);


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
  //! The right ascension
  double m_Ra;
  //! The declination
  double m_Dec;
  //! Representaion of the pointing as a quaternion
  NQuaternion m_Q;

  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;




#ifdef ___CINT___
 public:
  ClassDef(NPointing, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
