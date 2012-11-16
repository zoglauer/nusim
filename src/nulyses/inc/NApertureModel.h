/*
 * NApertureModel.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NApertureModel__
#define __NApertureModel__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
#include <iostream>
#include <map>
using namespace std;

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"
#include "MFile.h"
#include "NPhaFile.h"

// FITS libs:
#include "fitsio.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


class NApertureModel
{
public:
  NApertureModel();
  virtual ~NApertureModel();

  bool Initialize();

  double GetApertureShape(int detx, int dety, int Module);

  NPhaFile m_SpectrumA0;
  NPhaFile m_SpectrumA1;
  NPhaFile m_SpectrumA2;
  NPhaFile m_SpectrumA3;
  NPhaFile m_SpectrumB0;
  NPhaFile m_SpectrumB1;
  NPhaFile m_SpectrumB2;
  NPhaFile m_SpectrumB3;

public:
  
private:
  
};

#endif


////////////////////////////////////////////////////////////////////////////////
