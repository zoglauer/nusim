/*
 * NExtractFitsImage.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NExtractFitsImage__
#define __NExtractFitsImage__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:

// MEGAlib libs:
#include "NGlobal.h"
#include "MFunction2D.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NExtractFitsImage
{
  // public interface:
 public:
  //! Standard constructor
  NExtractFitsImage();
  //! Default destructor
  virtual ~NExtractFitsImage();

  //! Extract the image as a (MEGAlib) 2D function. Returns false on error
  bool Extract(const TString& FileName, MFunction2D& Image);
  
  
  // protected methods:
 protected:
  //NExtractFitsImage() {};
  //NExtractFitsImage(const NExtractFitsImage& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NExtractFitsImage, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
