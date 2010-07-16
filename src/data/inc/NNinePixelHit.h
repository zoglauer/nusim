/*
 * NNinePixelHit.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NNinePixelHit__
#define __NNinePixelHit__


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

class NNinePixelHit
{
  // public interface:
 public:
  //! Default constructor
  NNinePixelHit();
  //! Default destructor
  virtual ~NNinePixelHit();

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

  //! Set the pixel ID of the central pixel in X direction
  void SetXPixel(int XPixel) { m_Empty = false; m_XPixel = XPixel; }
  //! Return the pixel ID of the central pixel in X direction
  int GetXPixel() const { return m_XPixel; }

  //! Set the pixel ID of the central pixel in Y direction
  void SetYPixel(int YPixel) { m_Empty = false; m_YPixel = YPixel; }
  //! Return the pixel ID of the central pixel in Y direction
  int GetYPixel() const { return m_YPixel; }

  //! Set the pre-trigger pulse height
  void SetPreTriggerSampleSum(unsigned int PixelID, double PreTriggerSampleSum);
  //! Return the pre-trigger pulse height
  double GetPreTriggerSampleSum(unsigned int PixelID) const;

  //! Set the post-trigger pulse height
  void SetPostTriggerSampleSum(unsigned int PixelID, double PostTriggerSampleSum);
  //! Return the post-trigger pulse height
  double GetPostTriggerSampleSum(unsigned int PixelID) const;

  //! Set the trigger flag
  void SetTrigger(unsigned int PixelID, bool Trigger = true);
  //! Get the trigger flag
  bool GetTrigger(unsigned int PixelID) const;
  

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
  vector<double> m_PreTriggerSampleSum;
  //! The post-trigger sample sum (i.e. pre-pulseheight)
  vector<double> m_PostTriggerSampleSum;
  //! The trigger flag
  vector<int> m_Trigger;
    
  //! The ideal average depth position
  double m_IdealAverageDepth;
  //! The noised average depth position
  double m_NoisedAverageDepth;
  //! The ideal energy
  double m_IdealEnergy;
  //! The noised energy
  double m_NoisedEnergy;

  
  // Is there any information missing, timing?


#ifdef ___CINT___
 public:
  ClassDef(NNinePixelHit, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
