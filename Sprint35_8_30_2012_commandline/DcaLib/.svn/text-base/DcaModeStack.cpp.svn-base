// $Header: /CAMCAD/DcaLib/DcaModeStack.cpp 3     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaModeStack.h"

//_____________________________________________________________________________
bool CUseVoidsInDonuts::m_useVoidsInDonuts = true;

CUseVoidsInDonuts::CUseVoidsInDonuts(bool useVoidsInDonuts)
{
   m_savedUseVoidsInDonuts = m_useVoidsInDonuts;
   m_useVoidsInDonuts      = useVoidsInDonuts;
}

CUseVoidsInDonuts::~CUseVoidsInDonuts()
{
   m_useVoidsInDonuts = m_savedUseVoidsInDonuts;
}

bool CUseVoidsInDonuts::getUseVoidsInDonuts()
{
   return m_useVoidsInDonuts;
}

void CUseVoidsInDonuts::setUseVoidsInDonuts(bool useVoidsInDonuts)
{
   m_useVoidsInDonuts = useVoidsInDonuts;
}
