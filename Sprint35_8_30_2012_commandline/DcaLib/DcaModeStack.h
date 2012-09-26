// $Header: /CAMCAD/DcaLib/DcaModeStack.h 3     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaModeStack_h__)
#define __DcaModeStack_h__

#pragma once

//_____________________________________________________________________________
class CUseVoidsInDonuts
{
private:
   static bool m_useVoidsInDonuts;

   bool m_savedUseVoidsInDonuts;

public:
   CUseVoidsInDonuts(bool useVoidsInDonuts);
   ~CUseVoidsInDonuts();

   static bool getUseVoidsInDonuts();
   static void setUseVoidsInDonuts(bool useVoidsInDonuts);
};

#endif
