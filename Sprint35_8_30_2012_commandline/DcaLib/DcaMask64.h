// $Header: /CAMCAD/DcaLib/DcaMask64.h 1     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaMask64_h__)
#define __DcaMask64_h__

#pragma once

//_____________________________________________________________________________
class CMask64
{
protected:
   unsigned __int64 m_mask;

public:
   CMask64();
   virtual ~CMask64();
   CMask64(const CMask64& other);
   CMask64(int maskIndex);
   CMask64(int maskIndex1,int maskIndex2);
   CMask64(int maskIndex1,int maskIndex2,int maskIndex3);
   CMask64(int maskIndex1,int maskIndex2,int maskIndex3,int maskIndex4);
   CMask64(int maskIndex1,int maskIndex2,int maskIndex3,int maskIndex4,int maskIndex5);
   CMask64& operator=(const CMask64& other);

   virtual void add(int maskIndex);
   virtual void add(const CMask64& other);
   virtual void addAll();
   virtual void remove(int maskIndex);
   virtual void remove(const CMask64& other);
   virtual void empty();
   virtual void set(int maskIndex,bool value);
   virtual void toggle(int maskIndex);

   virtual bool contains(int maskIndex) const;
   virtual bool isEmpty() const;

protected:
   virtual unsigned __int64 getBitMask(int maskIndex) const;
};

#endif
