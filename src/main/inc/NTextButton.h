/*
 * NTextButton.h
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


#ifndef __NTextButton__
#define __NTextButton__


////////////////////////////////////////////////////////////////////////////////


// Standard libs:

// ROOT libs:
#include "TGButton.h"

// MEGAlib libs:
#include "NGlobal.h"

// NuSTAR libs:

// Forward declarations:


////////////////////////////////////////////////////////////////////////////////


//! \brief The colorful GUI buttons
//! The class represents the data-flow buttons in the main GUI window

class NTextButton : public TGTextButton
{
  // public interface:
 public:
  //! Standard constructor
  NTextButton(const TGWindow *p, const char *s, Int_t id);
  //! Default destructor
  virtual ~NTextButton();

  //! Special version for handling right-clicks and double-clicks
  Bool_t HandleButton(Event_t* event);

  //! Special version for handling right-clicks and double-clicks
  Bool_t HandleDoubleClick(Event_t* event);


  // protected methods:
 protected:
  //NTextButton() {};
  //NTextButton(const NTextButton& NCTHit) {};

  // private methods:
 private:



  // protected members:
 protected:


  // private members:
 private:



#ifdef ___CINT___
 public:
  ClassDef(NTextButton, 0) // no description
#endif

};

#endif


////////////////////////////////////////////////////////////////////////////////
