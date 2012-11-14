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
  
public:
  
private:
  
  
};

#endif


////////////////////////////////////////////////////////////////////////////////
