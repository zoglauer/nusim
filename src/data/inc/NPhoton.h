/*
 * NPhoton.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPhoton__
#define __NPhoton__


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


//! \brief Object describing a photon
//! This class describes a photon, including its position, direction, energy and polarization.

class NPhoton
{
  // public interface:
 public:
  //! Standard constructor
  NPhoton();
  //! Default destructor
  virtual ~NPhoton();

  //! Reset the photon to "NotDefined" values (polarization is zero)
  void Clear();

  //! Return true if the event is empty, i.e. the content is the same as generated during Clear()
  bool IsEmpty() const { return m_Empty; }

  //! Return the photon position
  MVector GetPosition() const { return m_Position; }
  //! Set the photon position
  void SetPosition(MVector Position) { m_Empty = false; m_Position = Position; } 

  //! Return the photon direction
  MVector GetDirection() const { return m_Direction; }
  //! Set the photon direction
  void SetDirection(MVector Direction) { m_Empty = false; m_Direction = Direction; m_Direction.Unitize(); } 

  //! Return the photon polarization
  MVector GetPolarization() const { return m_Polarization; }
  //! Set the photon polarization
  void SetPolarization(MVector Polarization) { m_Empty = false; m_Polarization = Polarization; } 

  //! Return the photon energy
  double GetEnergy() const { return m_Energy; }
  //! Set the photon energy
  void SetEnergy(double Energy) { m_Empty = false; m_Energy = Energy; } 

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, TString Keyword = "OP");

  //! Stream the content from a line of an ASCII file  
  bool Parse(const TString& Line);


  // protected methods:
 protected:

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this event is still empty, which is set during Clear()
  bool m_Empty;

  //! Photon position (in cm)
  MVector m_Position;
  //! Photon direction (normalized to 1)
  MVector m_Direction;
  //! Photon polarization (if null-vector, then no polarization)
  MVector m_Polarization;
  //! Photon energy
  double m_Energy;


#ifdef ___CINT___
 public:
  ClassDef(NPhoton, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
