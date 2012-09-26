
#if !defined(__DcaEntityNumber_h__)
#define __DcaEntityNumber_h__

#pragma once
// Valid entity numbers start a 1 and are positive, this value is used in entity number return

// values and indicates no entity number is present

#define DCA_NO_ENTITY_NUMBER  -1


#ifdef Define_CEntityNumber
//_____________________________________________________________________________
class CEntityNumber
{
private:
   static long m_NextEntityNumber;

public:
   static long allocate();
   static long getCurrent();
   static long setCurrent(long entityNumber);
   static long update(long entityNumber);
};
#endif // #ifdef Define_CEntityNumber

//_____________________________________________________________________________
class CEntityNumberGenerator
{
private:
   int m_nextEntityNumber;

public:
   CEntityNumberGenerator();

   int allocate();
   int getCurrent();
   int setCurrent(int entityNumber);
   int setAsAllocated(int allocatedEntityNumber);
};

#endif
