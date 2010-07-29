/*
 * NGlobal.h
 *
 * Copyright (C) 1998-2009 by Robert Andritschke.
 * Copyright (C) 1998-2009 by Andreas Zoglauer.
 * All rights reserved.
 *
 * Please see the source-file for the copyright-notice.
 *
 */


////////////////////////////////////////////////////////////////////////////////


#ifndef __NGlobal__
#define __NGlobal__

#ifdef ___LINUX___
#define ___UNIX___
#endif

#ifdef ___MACOSX___
#define ___UNIX___
#endif

#include <cmath>
using namespace std;

#include <TString.h>
#include <TSystem.h>

#include "MVector.h"
#include "NQuaternion.h"


// Some globally defined variables
extern const TString g_MajorVersion;
extern const TString g_PatchVersion;
extern const TString g_Version;
extern int g_SVNRevision;

extern const TString g_CopyrightYear;
extern const TString g_Homepage;

extern const TString g_MEGAlibPath;

extern const TString g_StringNotDefined;
extern const MVector g_VectorNotDefined;
extern const int g_IntNotDefined;
extern const unsigned int g_UnsignedIntNotDefined;
extern const double g_DoubleNotDefined;
extern const float g_FloatNotDefined;
extern const NQuaternion g_QuaternionNotDefined;

extern int g_DebugLevel;

// And here a lot of other useful constants:
extern const double c_Pi;
extern const double c_Rad;            // c_Pi / 180.0
extern const double c_Deg;            // 180.0 / c_Pi
extern const double c_Sqrt2Pi;        // 2.5066283
extern const double c_SpeedOfLight;   //  
extern const double c_E0;             // rest mass electron  
extern const double c_FarAway;        // Radius for conversion from spherical to cartesian coordinates   
extern const double c_LargestEnergy;  // 
extern const MVector c_NullVector;



//! \brief Initialization of everything global
//! This is a global class taking care of initialization of ROOT and some global constants

class NGlobal
{
 public:
  NGlobal() {};
  static bool Initialize(); // Has to be called in main 
};

extern NGlobal g_Global;

#endif
