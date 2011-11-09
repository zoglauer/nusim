/*
 * NShieldHit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NShieldHit__
#define __NShieldHit__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <fstream>
using namespace std;

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief Object describing a measured pixel hit in the detector associated with pixel IDs and ADunits
//! This class represents a measured hit in a pixel. It contains the telescope ID, the detector ID, the X and Y pixel ID as well as the measured AD converter units.
//! In the analysis chain this event representation is preceeded by NInteraction objects and succeeded by reconstucted hits NHit

class NShieldHit
{
  // public interface:
 public:
  //! Default constructor
  NShieldHit();
  //! Default destructor
  virtual ~NShieldHit();

  //! Reset all data
  void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the Telescope ID
  void SetTelescope(int Telescope) { m_Empty = false; m_Telescope = Telescope; }
  //! Return the Telescope ID
  int GetTelescope() const { return m_Telescope; }

  //! Set the ideal energy deposit
  void SetIdealEnergy(double IdealEnergy) { m_Empty = false; m_IdealEnergy = IdealEnergy; }
  //! Return the ideal energy deposit
  double GetIdealEnergy() const { return m_IdealEnergy; }

  //! Set the noised energy deposit
  void SetNoisedEnergy(double NoisedEnergy) { m_Empty = false; m_NoisedEnergy = NoisedEnergy; }
  //! Return the noised energy deposit
  double GetNoisedEnergy() const { return m_NoisedEnergy; }

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S, int Version);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line, int Version);


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

  //! Telescope ID
  int m_Telescope;
 
  //! The ideal energy deposit
  double m_IdealEnergy;
  //! The noised energy deposit
  double m_NoisedEnergy;

  
  // Is there any information missing, timing?


#ifdef ___CINT___
 public:
  ClassDef(NShieldHit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
