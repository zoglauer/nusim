/*
 * NTextButton.cxx
 *
 * Copyright (C) 2009-2009 by the NuSTAR team.
 * All rights reserved.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
// NTextButton
//
////////////////////////////////////////////////////////////////////////////////


// Include the header:
#include "NTextButton.h"

// Standard libs:

// ROOT libs:
#include "TGToolTip.h"

// MEGAlib libs:

// NuSTAR libs:

////////////////////////////////////////////////////////////////////////////////


#ifdef ___CINT___
ClassImp(NTextButton)
#endif


////////////////////////////////////////////////////////////////////////////////


NTextButton::NTextButton(const TGWindow *p, const char *s, Int_t id) :
  TGTextButton(p, s, id)
{
  // Construct an instance of NTextButton

  // Handle any button:

  gVirtualX->GrabButton(fId, kAnyButton, kAnyModifier,
                        kButtonPressMask | kButtonReleaseMask,
                        kNone, kNone);
}


////////////////////////////////////////////////////////////////////////////////


NTextButton::~NTextButton()
{
  // Delete this instance of NTextButton
}


////////////////////////////////////////////////////////////////////////////////



Bool_t NTextButton::HandleButton(Event_t *event)
{
  // Handle mouse button event.
  // Mostly copy and paste from ROOT - only change is handling of double/right click

   Bool_t click = kFALSE;

   if (fTip) fTip->Hide();

   if (fState == kButtonDisabled) return kTRUE;

   Bool_t in = (event->fX >= 0) && (event->fY >= 0) &&
               (event->fX <= (Int_t)fWidth) && (event->fY <= (Int_t)fHeight);

   // We don't need to check the button number as GrabButton will
   // only allow button1 events
   if (event->fType == kButtonPress) {
      fgReleaseBtn = 0;

      if (fState == kButtonEngaged) {
         return kTRUE;
      }
      if (in) SetState(kButtonDown, kTRUE);
   } else { // ButtonRelease
      if (fState == kButtonEngaged) {
         if (in) SetState(kButtonUp, kTRUE);
         click = kTRUE;
      } else {
         click = (fState == kButtonDown) && in;
         if (click && fStayDown) {
            if (in) {
               SetState(kButtonEngaged, kTRUE);
               fgReleaseBtn = 0;
            }
         } else {
            if (in) {
               SetState(kButtonUp, kTRUE);
               fgReleaseBtn = fId;
            }
         }
      }
   }
   if (click) {
     if (event->fCode == kButton1) {
       SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_BUTTON), fWidgetId,
                   (Long_t) fUserData);
       fClient->ProcessLine(fCommand, MK_MSG(kC_COMMAND, kCM_BUTTON), fWidgetId,
                            (Long_t) fUserData);
     } else {
       // Bad hack - but we are trying to avoid a ROOT implementation limitation
       SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kMDI_SIZE), fWidgetId,
                   (Long_t) fUserData);
       fClient->ProcessLine(fCommand, MK_MSG(kC_COMMAND, kMDI_SIZE), fWidgetId,
                            (Long_t) fUserData);      
     }
   }

   return kTRUE;
}


////////////////////////////////////////////////////////////////////////////////



Bool_t NTextButton::HandleDoubleClick(Event_t *event)
{
  // Handle mouse button event.
  // Mostly copy and paste from ROOT - only change is handling of double/right click


   Bool_t click = kFALSE;

   if (fTip) fTip->Hide();

   if (fState == kButtonDisabled) return kTRUE;

   Bool_t in = (event->fX >= 0) && (event->fY >= 0) &&
               (event->fX <= (Int_t)fWidth) && (event->fY <= (Int_t)fHeight);

   // We don't need to check the button number as GrabButton will
   // only allow button1 events
   if (event->fType == kButtonPress) {
      fgReleaseBtn = 0;

      if (fState == kButtonEngaged) {
         return kTRUE;
      }
      if (in) SetState(kButtonDown, kTRUE);
   } else { // ButtonRelease
      if (fState == kButtonEngaged) {
         if (in) SetState(kButtonUp, kTRUE);
         click = kTRUE;
      } else {
         click = (fState == kButtonDown) && in;
         if (click && fStayDown) {
            if (in) {
               SetState(kButtonEngaged, kTRUE);
               fgReleaseBtn = 0;
            }
         } else {
            if (in) {
               SetState(kButtonUp, kTRUE);
               fgReleaseBtn = fId;
            }
         }
      }
   }
   if (click) {
      SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_BUTTON), fWidgetId,
                  (Long_t) fUserData);
      fClient->ProcessLine(fCommand, MK_MSG(kC_COMMAND, kCM_BUTTON), fWidgetId,
                           (Long_t) fUserData);
   }

   return kTRUE;
}


// NTextButton.cxx: the end...
////////////////////////////////////////////////////////////////////////////////
