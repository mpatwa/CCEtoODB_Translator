
#include "StdAfx.h"
#include "DcaEntityNumber.h"

#ifdef Define_CEntityNumber
//_____________________________________________________________________________

// Case 1921  Be sure to maintain m_NextEntityNumber as the Next Available entity number.
long CEntityNumber::m_NextEntityNumber = 1;

long CEntityNumber::allocate()
{
	// Case 1921  No change here, return current value of m_entityNumber, increment after.
   long retval = m_NextEntityNumber++;

   //switch (retval)
   //{
   //case 13721:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //case 2718:
   //   { 
   //      int iii = 3; 
   //   }
   //   break;
   //}

   return retval;
}

long CEntityNumber::getCurrent()
{
   return m_NextEntityNumber;
}

long CEntityNumber::setCurrent(long entityNumber)
{
   long retval = m_NextEntityNumber;

   m_NextEntityNumber = entityNumber;

   return retval;
}

long CEntityNumber::update(long entityNumber)
{
	// Case 1921  Problem was here. This function is arguably correct as-is (as-was) but 
	// the usage of it (e.g. in Xml_Cont.cpp) makes it incorrect. Calls to this set the
	// Next Entity Number to the value of the argument, if it is larger than current Next.
	// It is being called with the entity number of some existing entity (usually just made
	// from ccz read). This left the value of Next as actually the Highest Existing entity
	// number. The next call to allocate() returns this value, voila, item with duplicate 
	// entity number is created. 
	
	// The original form of this function was arguably correct, and
	// all the users of it wrong. But since all the users (to date, in camcad) use it the
	// same, I'll take that as precedent for how it should work, and designate this
	// update function as the thing that was wrong.

	long retval = m_NextEntityNumber;

	// Maintian Next Entity Number as the Next Entity Number!   
   if (entityNumber >= m_NextEntityNumber)
   {
      m_NextEntityNumber = entityNumber + 1;
   }

   return retval;
}
#endif // #ifdef Define_CEntityNumber

//_____________________________________________________________________________
CEntityNumberGenerator::CEntityNumberGenerator()
: m_nextEntityNumber(1)
{
}

int CEntityNumberGenerator::allocate()
{
   int retval = m_nextEntityNumber++;

   return retval;
}

int CEntityNumberGenerator::getCurrent()
{
   return m_nextEntityNumber;
}

int CEntityNumberGenerator::setCurrent(int entityNumber)
{
   int retval = m_nextEntityNumber;

   m_nextEntityNumber = entityNumber;

   return retval;
}

int CEntityNumberGenerator::setAsAllocated(int allocatedEntityNumber)
{
	int retval = m_nextEntityNumber;

   if (allocatedEntityNumber >= m_nextEntityNumber)
   {
      m_nextEntityNumber = allocatedEntityNumber + 1;
   }

   return retval;
}
