/*
 * MBinnerBayesianBlocks.h
 *
 * Copyright (C) by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


#ifndef __MBinnerBayesianBlocks__
#define __MBinnerBayesianBlocks__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
//#include <math.h>

// ROOT libs:

// MEGAlib libs:
#include "MBinner.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! A binning class using Bayesian blocks for binning
//! Reference for bayesian blocks:
//! Jeffrey D. Scargle et al. "Studies in Astronomical Time Series Analysis. VI. Bayesian Block Representations", ApJ 764, 2013 
class MBinnerBayesianBlocks : public MBinner
{
  // public interface:
 public:
  //! Default constructor
  MBinnerBayesianBlocks();
  //! Default destuctor 
  virtual ~MBinnerBayesianBlocks();
  
  //! Set the minimum bin width
  void SetMinimumBinWidth(double MinimumBinWidth) { m_MinimumBinWidth = MinimumBinWidth; }
  
  //! Set the minimum counts per bin
  void SetMinimumCountsPerBin(double MinimumCountsPerBin) { m_MinimumCountsPerBin = MinimumCountsPerBin; }
  
  //! Set the minimum bin width
  void SetPrior(double Prior) { m_Prior = Prior; }
  
  // protected methods:
 protected:
  //! The actual histogramming process
  virtual void Histogram(); 

  
  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:
  //! The minimum bin width
  double m_MinimumBinWidth;
  //! The minimum counts per bin
  double m_MinimumCountsPerBin;
  //! The prior
  double m_Prior;
  //! True if we use binning
  bool m_UseBinning;

  
#ifdef ___CINT___
 public:
  ClassDef(MBinnerBayesianBlocks, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
