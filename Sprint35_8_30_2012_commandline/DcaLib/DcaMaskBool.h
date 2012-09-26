// $Header: /CAMCAD/DcaLib/DcaMaskBool.h 1     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaMaskBool_h__)
#define __DcaMaskBool_h__

#pragma once

//_____________________________________________________________________________
class CMaskBool
{
protected:
   CArray<bool,bool> m_mask;
   bool m_initialValue;

public:
   CMaskBool(bool initialValue = false);
   virtual ~CMaskBool();
   CMaskBool(const CMaskBool& other);
   CMaskBool(int maskIndex);
   CMaskBool(int maskIndex1,int maskIndex2);
   CMaskBool& operator=(const CMaskBool& other);
   virtual bool operator==(const CMaskBool& other) const;
   virtual bool operator!=(const CMaskBool& other) const;

   virtual void add(int maskIndex);
   virtual void add(const CMaskBool& other);
   virtual void addAll();
   virtual void remove(int maskIndex);
   virtual void remove(const CMaskBool& other);
   virtual void removeAll();

   virtual bool contains(int maskIndex) const;
   virtual bool isEmpty() const;
   virtual bool isFull() const;
   virtual CString getContentsString() const;
};

#endif
