/*
 * NNulyses.h
 *
 * Copyright (C) by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NNulyses__
#define __NNulyses__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:
#include <vector>
using namespace std;

// ROOT libs:

// NuSTAR libs:
#include "NGlobal.h"

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////



class Nulyses
{
public:
  /// Default constructor
  Nulyses();
  /// Default destructor
  ~Nulyses();
  
  /// Parse the command line
  bool ParseCommandLine(int argc, char** argv);
  /// Interrupt the analysis
  void Interrupt() { m_Interrupt = true; }
  
  
private:
  /// True, if the analysis needs to be interrupted
  bool m_Interrupt;
  
};

#endif


////////////////////////////////////////////////////////////////////////////////
