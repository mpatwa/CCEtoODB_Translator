// $Header: /CAMCAD/DcaLib/DcaNet.h 7     6/30/07 3:00a Kurt Van Ness $

#if !defined(__DcaNet_h__)
#define __DcaNet_h__

#pragma once

#include "Dca.h"
#include "DcaCompPin.h"

// NET_UNUSED_PINS vs NET_NO_NET_PINS
//    - in netlist derive, NET_NO_NET_PINS pins can inherit other netnames.
//    - NET_UNUSED_PINS is fully part of drc -- do not use this pin.
#define  NET_UNUSED_PINS            "~Unused_Pins~" // NETNAME for UNUSED net in netlist
#define  NET_NO_NET_PINS            "~No_Net_Pins~" // NETNAME for NONET net in netlist

#define  NETFLAG_UNUSEDNET          0x00000001     // this net contains pins which are unused
#define  NETFLAG_UNNAMED            0x00000002     // this net had no netname or a system generated from CAD Readers
#define  NETFLAG_NONET              0x00000004     // this net contains pins which are not assigned a net
#define  NETFLAG_SINGLEPINNET       0x00000008     // this net contains only one pin

class CNetList;
class FileStruct;

//_____________________________________________________________________________
class NetStruct
{
friend CTypedListContainer<CPtrList,NetStruct*>;
friend CNetList;
friend CTypedMapContainer<CMapStringToPtr,CString,NetStruct*>;
friend CTypedArrayWithMap<CPtrArray,CMapStringToPtr,NetStruct>; // used by CCamCadNetMap

   // NetList
   // the netlist contains netnames (there also may be a net for unconnected pins
   //                         
   //                       - attributes for the net, i.e clearance, critital etc..
   //                       - comp pin entries with pinnames, which are in the netlist of the import
   //                         - attributes for pinnumbers
   //                         - attribute of Padstack, pinloc, pinrot, pinaccess etc ...
private:
   int            m_entityNumber;  // entity;

   CString        m_netName;       // NetName;
   unsigned char  m_highlighted:1; // highlight:1;      // only mark or not.
   unsigned char  m_dummy:7;       // dummy:7;
   DbFlag         m_flagBits;      // netflg;           // defined in dbutil.h  NETLIST_FLAGS
   CCompPinList   m_compPinList;   // CompPinList;      // needs to stay at end of structure
   CAttributes*   m_attributes;    // AttribMap;        // needs to stay at end of structure

   CNetList&      m_parentNetList;

private:
   NetStruct(CNetList& parentNetList,const CString& netName,int entityNumber = -1);
   ~NetStruct();

public:
   CCamCadData& getCamCadData() const;
   CNetList& getNetList() const;

   // accessors
   const CString& getNetName() const;
   void setNetName(const CString& netName);

   long getEntityNumber() const;
   //void setEntityNumber(long entityNumber);

   bool isHighlighted() const;
   void setHighlighted(int flag);
   void setHighlighted(bool flag);

   DbFlag getFlags() const;
   void setFlags(DbFlag flags);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   CAttributes& attributes();
   CAttributes* getAttributes() const;
   CAttributes*& getAttributesRef();
   CAttributes*& getDefinedAttributes();
   bool lookUpAttrib(WORD keyword, CAttribute *&attribute) const;
   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value,
      AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);

   CompPinStruct* addCompPin(const CString compName, const CString pinName,int entityNumber = -1);
   CompPinStruct* addCompPin(const CString compName, const CString pinName, const double x, const double y,
      const double rotationDegrees, const int mirror, const int padstackBlockNumber, const VisibleTag visible,
      int entityNumber = -1);
   CompPinStruct* getDefinedCompPin(const CString compName, const CString pinName, bool caseSensitive);
   CompPinStruct* findCompPin(const CString compName, const CString pinName, bool caseSensitive);
   CompPinStruct* findCompPin(CPoint2d location, double tolerance = 0.005); // find compin on this net at this location
   bool IsEmpty() const;

   // operations
   CompPinStruct* getHeadCompPin() const;              
   POSITION getHeadCompPinPosition();
   CompPinStruct* getNextCompPin(POSITION& pos);
   int getCompPinCount();

   CCompPinList&       getCompPinList();
   const CCompPinList& getCompPinList() const;

   void takeCompPins(NetStruct& otherNet);
   
   bool IsDiePin(CString compName, CString pinName);
   bool IsSingleDiePinNet();

private:


   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
};

//_____________________________________________________________________________
class CNetStructList : public CTypedPtrList<CPtrList,NetStruct*>
{
};

//_____________________________________________________________________________
class CNetListContainer : public CTypedPtrListContainer<NetStruct*>
{
};

//_____________________________________________________________________________
class CNetMap : public CTypedPtrMap<CMapStringToPtr,CString,NetStruct*>
{
};

//_____________________________________________________________________________
class CCompPinMap
{
private:
   CTypedPtrMap<CMapStringToPtr,CString,CompPinStruct*> m_pinMap;

public:
   CCompPinMap();

   void addPin(CompPinStruct& compPinStruct);
   void addPins(const CCompPinList& compPinList);

   void removePin(const CompPinStruct& compPinStruct);
   void removePins(const CCompPinList& compPinList);

   CompPinStruct* getCompPin(const CString& pinRef);
};

//_____________________________________________________________________________
class CNetList
{
friend CompPinStruct;

private:
   CNetListContainer m_netList;  // nets in netlist must have unique net names
   CNetMap m_netMap;
   CCompPinMap m_compPinMap;

   CCamCadData& m_parentCamCadData;

public:
   CNetList(CCamCadData& parentCamCadData);
   ~CNetList();

   CCamCadData& getCamCadData() const;

   NetStruct* addNet(const CString& netName,int entityNumber = -1);
   NetStruct* getNet(const CString& netName);
   NetStruct& getUnusedPinsNet();
   NetStruct& getNoNetPinsNet();

   POSITION getHeadPosition() const;
   NetStruct* getNext(POSITION& pos) const;
   NetStruct* getAt(POSITION pos) const;
   int getCount() const;
   bool isEmpty() const;
   void deleteAt(POSITION pos);
   void deleteNet(const CString& netName);
   void empty();

   void updateNetName(NetStruct *net, CString newName);

   // deprecated
   POSITION   GetHeadPosition()      const { return getHeadPosition(); }
   NetStruct* GetNext(POSITION& pos) const { return getNext(pos); }
   NetStruct* GetAt(POSITION pos)    const { return getNext(pos); }
   int        GetCount()             const { return getCount(); }
   bool       IsEmpty()              const { return isEmpty(); }

   CompPinStruct* getCompPin(const CString& pinRef);
   CompPinStruct* getCompPin(const CString& refDes,const CString& pinName);
   CompPinStruct& getDefinedCompPin(const CString& refDes,const CString& pinName);

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
	void Scale(double factor);

   static CString getUnusedPinsNetName();
   static CString getNoNetPinsNetName();

private:
   CCompPinMap& getCompPinMap();
};

#endif
