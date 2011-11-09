/*
 * NInteraction.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NInteraction__
#define __NInteraction__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "MVector.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a interaction in the detector
//! This class represents an interaction in the detector, i.e. the ideal 
//! positions and energy amounts deposited by the original particle before the detector effects engine.
//! In the analysis chain this event representation is preceeded by a photon and suceeded by a list of pixel hits.

class NInteraction
{
  // public interface:
 public:
  //! Standard constructor
  NInteraction();
  //! Default destructor
  virtual ~NInteraction();

  //! Reset all data
  void Clear();

  //! Set whether or not this is a detector hit (shield hit otherwise
  void SetIsDetector(bool IsDetector) { m_Empty = false; m_IsDetector = IsDetector; }
  //! Return if this is a detector hit (shield hit otherwise
  bool IsDetector() { return m_IsDetector; }

  //! Set the Telescope ID
  void SetTelescope(int Telescope) { m_Empty = false; m_Telescope = Telescope; }
  //! Return the Telescope ID
  int GetTelescope() const { return m_Telescope; }

  //! Set the Detector ID
  void SetDetector(int Detector) { m_Empty = false; m_Detector = Detector; }
  //! Return the Detector ID
  int GetDetector() const { return m_Detector; }

  //! Return true if the interaction is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the position of the hit (in detector coordinates if hit --- irrelevant for shield)
  void SetPosition(const MVector& Position) { m_Empty = false; m_Position = Position; }
  //! Return the position of the hit (in detector coordinates if hit --- irrelevant for shield)
  MVector GetPosition() const { return m_Position; }

  //! Set the energy in keV
  void SetEnergy(double Energy) { m_Empty = false; m_Energy = Energy; }
  //! Return the energy in keV
  double GetEnergy() const { return m_Energy; }
  
  //! Dump the content to a string
  TString ToString() const;

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);


  // protected methods:
 protected:
  //NInteraction() {};
  //NInteraction(const NInteraction& NCTInteraction) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! True if this hit is still empty, which is set during Clear()
  bool m_Empty;

  //! True if this hit has haqppened in the detector (deafult) - if false it happened in the shield
  bool m_IsDetector;

  //! Telescope ID [1..2]
  int m_Telescope;
  //! Detector ID [1..4]
  int m_Detector;

  //! Position of the hit
  MVector m_Position;

  //! Energy of the hit
  double m_Energy;


#ifdef ___CINT___
 public:
  ClassDef(NInteraction, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
