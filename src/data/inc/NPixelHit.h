/*
 * NPixelHit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NPixelHit__
#define __NPixelHit__


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

class NPixelHit
{
  // public interface:
 public:
  //! Default constructor
  NPixelHit();
  //! Default destructor
  virtual ~NPixelHit();

  //! Reset all data
  void Clear();

  //! Return true if the object is empty
  bool IsEmpty() const { return m_Empty; }

  //! Set the Telescope ID
  void SetTelescope(int Telescope) { m_Empty = false; m_Telescope = Telescope; }
  //! Return the Telescope ID
  int GetTelescope() const { return m_Telescope; }

  //! Set the Detector ID
  void SetDetector(int Detector) { m_Empty = false; m_Detector = Detector; }
  //! Return the Detector ID
  int GetDetector() const { return m_Detector; }


  //! Set the pixel ID in X direction
  void SetXPixel(int XPixel) { m_Empty = false; m_XPixel = XPixel; }
  //! Return the pixel ID in X direction
  int GetXPixel() const { return m_XPixel; }

  //! Set the pixel ID in Y direction
  void SetYPixel(int YPixel) { m_Empty = false; m_YPixel = YPixel; }
  //! Return the pixel ID in Y direction
  int GetYPixel() const { return m_YPixel; }


  //! Set the ideal average depth position
  void SetIdealAverageDepth(double IdealAverageDepth) { m_Empty = false; m_IdealAverageDepth = IdealAverageDepth; }
  //! Return the ideal average depth position
  double GetIdealAverageDepth() const { return m_IdealAverageDepth; }

  //! Set the noised average depth position
  void SetNoisedAverageDepth(double NoisedAverageDepth) { m_Empty = false; m_NoisedAverageDepth = NoisedAverageDepth; }
  //! Return the noised average depth position
  double GetNoisedAverageDepth() const { return m_NoisedAverageDepth; }

  //! Set the ideal average depth position
  void SetIdealEnergy(double IdealEnergy) { m_Empty = false; m_IdealEnergy = IdealEnergy; }
  //! Return the ideal average depth position
  double GetIdealEnergy() const { return m_IdealEnergy; }

  //! Set the noised average depth position
  void SetNoisedEnergy(double NoisedEnergy) { m_Empty = false; m_NoisedEnergy = NoisedEnergy; }
  //! Return the noised average depth position
  double GetNoisedEnergy() const { return m_NoisedEnergy; }

  //! Set the pre trigger pulse height
  void SetPreTriggerSampleSum(double PreTriggerSampleSum) { m_Empty = false; m_PreTriggerSampleSum = PreTriggerSampleSum; }
  //! Add a value to the pre trigger pulse height
  void AddPreTriggerSampleSum(double PreTriggerSampleSum) { m_Empty = false; m_PreTriggerSampleSum += PreTriggerSampleSum; }
  //! Return the pre-trigger pulse height
  double GetPreTriggerSampleSum() const { return m_PreTriggerSampleSum; }

  //! Set the post-trigger pulse height
  void SetPostTriggerSampleSum(double PostTriggerSampleSum) { m_Empty = false; m_PostTriggerSampleSum = PostTriggerSampleSum; }
  //! Add a value to the post-trigger pulse height
  void AddPostTriggerSampleSum(double PostTriggerSampleSum) { m_Empty = false; m_PostTriggerSampleSum += PostTriggerSampleSum; }
  //! Return the post-trigger pulse height
  double GetPostTriggerSampleSum() const { return m_PostTriggerSampleSum; }

  //! Stream the content to an ASCII file 
  bool Stream(ofstream& S);

  //! Stream the content from a line of an ASCII file  
  bool Parse(TString& Line);


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
  //! Detector ID
  int m_Detector;
  //! X Strip ID
  int m_XPixel;
  //! Y Strip ID
  int m_YPixel;
  //! The pre-trigger sample sum (i.e. pre-pulseheight)
  double m_PreTriggerSampleSum;
  //! The post-trigger sample sum (i.e. pre-pulseheight)
  double m_PostTriggerSampleSum;
  
  //! The ideal average depth position
  double m_IdealAverageDepth;
  //! The noised average depth position
  double m_NoisedAverageDepth;
  //! The ideal energy
  double m_IdealEnergy;
  //! The noised energy
  double m_NoisedEnergy;
  


#ifdef ___CINT___
 public:
  ClassDef(NPixelHit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
