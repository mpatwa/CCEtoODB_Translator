// $Header:

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// CKT_NAV_OUT.H

#if ! defined (__CKT_NAV_OUT_h__)
#define __CKT_NAV_OUT_h__

#pragma once

#include "DeviceType.h"

//#include "gencad.h";
//*rcf PROBABLY want to separate these
// For now at least, these are shared with regular gencad exporter
#include "PcbLayer.h"
typedef GenCADLayerStruct CKTNAVLayerStruct;
typedef CGenCADLayerArray CCKTNAVLayerArray;
typedef GenCADLayerDlg CKTNAVLayerDlg;

typedef struct
{
   CString  padname;
   CString  newpadname;
   int      rotation;   // rotation in degree
} CKTNAVPadRotStruct;
typedef CTypedPtrArray<CPtrArray, CKTNAVPadRotStruct*> PadRotArray;


typedef struct
{
   CString  padname;
//   int      layer;
   int      used_as_testcomp;
}CKTNAVPadstack;
typedef CTypedPtrArray<CPtrArray, CKTNAVPadstack*> CKTNAVPadstackArray;

typedef struct
{
   CString  netname;
   POSITION pos;
}CKTNAVRoutes;
typedef CTypedPtrArray<CPtrArray, CKTNAVRoutes*> CKTNAVRoutesArray;

//
typedef struct
{
   double   pinx, piny;
   double   rotation;
   CString  padstackname;
   char     mirror;
   char     drill;      // this is a drill hole
   CString  pinname;
}CKTNAVCompPinInst;
typedef CTypedPtrArray<CPtrArray, CKTNAVCompPinInst*> CompPinInstArray;

typedef struct
{
   CString  compname;
   CString  devicename;    
   CString  geomname;
}CKTNAVCompList;
typedef CTypedPtrArray<CPtrArray, CKTNAVCompList*> CComplistArray;

class CDevicePartPair
{
public:
	CDevicePartPair(CString device, CString partNumber)
	{
		m_sDevice = device;
		m_sPartNumber = partNumber;
	};

private:
	CString m_sDevice;
	CString m_sPartNumber;

public:
	CString GetDevice() const			{ return m_sDevice; };
	CString GetPartNumber() const		{ return m_sPartNumber; };
};

class CDevicePartMap : private CTypedMapStringToPtrContainer<CDevicePartPair*>
{
public:
	void AddPair(CString device, CString partNumber)
	{
		if (FindPair(device, partNumber))
			return;

		CDevicePartPair *pair = NULL;
		if (!Lookup(device, pair))
			pair = new CDevicePartPair(device, partNumber);

		// add the part number map to the device map
		SetAt(device, pair);
	};

	bool FindDevice(CString device)
	{
		CDevicePartPair *pnMap = NULL;
		return Lookup(device, pnMap)?true:false;
	};

	bool GetPartNumber(CString device, CString &partNumber)
	{
		CDevicePartPair *pair = NULL;

		if (Lookup(device, pair))
			return false;

		partNumber = pair->GetPartNumber();
		return true;
	};

	bool FindPair(CString device, CString partNumber)
	{
		CDevicePartPair *pair = NULL;
		if (!Lookup(device, pair))
			return false;

		return (pair->GetPartNumber() == partNumber);
	};
};

class CCKTNAVDevice
{
public:
	CCKTNAVDevice(CString name, CString partNumber)
	{
		m_sName = name;
		m_sPartNumber = partNumber;
		m_sValue.Empty();
		m_sPlusTol.Empty();
		m_sMinusTol.Empty();
		m_sTolerance.Empty();
		m_sType.Empty();
		m_sStyle.Empty();
	}
	~CCKTNAVDevice(){}

private:
	CString m_sName;
	CString m_sPartNumber;
	CString m_sValue;
	CString m_sPlusTol;
	CString m_sMinusTol;
	CString m_sTolerance;
	CString m_sType;
	CString m_sStyle;
	CMapStringToString m_pinMap;

public:
	CString GetName() const								{ return m_sName;					}
	CString GetPartNubmer() const						{ return m_sPartNumber;			}
	CString GetValue() const							{ return m_sValue;				}
	CString GetPlusTol() const							{ return m_sPlusTol;				}
	CString GetMinusTol() const						{ return m_sMinusTol;			}
	CString GetTolerance() const						{ return m_sTolerance;			}
	CString GetType() const								{ return m_sType;					}
	CString GetStyle() const							{ return m_sStyle;				}


	void SetValue(const CString value)				{ m_sValue = value;				}
	void SetPlusTol(const CString plusTol)			{ m_sPlusTol = plusTol;			}
	void SetMinusTol(const CString minusTol)		{ m_sMinusTol = minusTol;		}
	void SetTolerance(const CString tolerance)	{ m_sTolerance = tolerance;	}
	void SetType(const CString type)					{ m_sType = type;					}
	void SetStyle(const CString style)				{ m_sStyle = style;				}

	void SetPinMap(CString pinName, CString pinType)	{ m_pinMap.SetAt(pinName, pinType);  }
	CString GetPinType(CString pinName)						{ CString val; m_pinMap.Lookup(pinName, val); return val; }
	void FillPinMap(FileStruct *file, DataStruct *data);
	bool PinMapIsEmpty()											{ return m_pinMap.IsEmpty() ? true : false; }
	POSITION GetPinMapStartPosition()	{ POSITION pos = m_pinMap.GetStartPosition(); return pos; }
	void GetNextPinMap(POSITION &pos, CString &pinName, CString &pinType) { m_pinMap.GetNextAssoc(pos, pinName, pinType); }

	CString GetGencadPinType(CString camcadPinType);
	CString GetGencadType();  // component type


};

class CCKTNAVDeviceMap : public CTypedMapStringToPtrContainer<CCKTNAVDevice*>
{
public:
	CCKTNAVDeviceMap()	{ genericDeviceCnt = 0;	}
	~CCKTNAVDeviceMap()	{ this->empty();			}

private:
	int genericDeviceCnt;		// counter for creating generic device name = "Device%d"

public:
	CCKTNAVDevice* AddDevice(const CString deviceName, const CString partNumber);
	CCKTNAVDevice* GenerateDevice();

	void WriteDevice(FILE *fp);
};

class CCKTNAVPadstack
{
public:
	CCKTNAVPadstack(BlockStruct* block)
	{		
		m_pBlock = block;
		m_sName = block->getName();
		m_sMirName = block->getName();
		m_bUseAsTestComp = false;
		m_bNeedMirVersion = false;
		m_bIsPadstack = (block->getBlockType() == blockTypePadstack)?true:false;
      m_bIsbondPadstack = (block->getBlockType() == blockTypeBondPad)?true:false;
      m_insertLayer = -1;
	}
	~CCKTNAVPadstack()
	{
		m_pBlock = NULL;
	}

private:
	CString m_sName;
	CString m_sMirName;			// Name of mirred version
	bool m_bUseAsTestComp;		// This padstack was used as a testpoint converted to single pin component
	bool m_bNeedMirVersion;		// This padstack need to output a mirrored version of itself
	bool m_bIsPadstack;			// Indicate if a real padstack or BL_TOOL because CKTNAV output both as padstack with some syntax difference
   bool m_bIsbondPadstack;	   // Indicate if a real bond padstack or BL_TOOL because GENCAD output both as padstack with some syntax difference
	BlockStruct* m_pBlock;		// Pointer to the block
   int  m_insertLayer;        // layer in the insert of block

public:
	CString GetName() const									{ return m_sName;							}
	CString GetMirName() const								{ return m_sMirName;						}
	bool GetUseAsTestComp() const							{ return m_bUseAsTestComp;				}
	bool GetNeedMirVersion() const						{ return m_bNeedMirVersion;			}
	bool GetIsPadstack() const								{ return m_bIsPadstack;					}
	BlockStruct* GetBlock() const							{ return m_pBlock;						}
   int  GetInsertLayer() const					      { return m_insertLayer;					}
   void  SetInsertLayer(int layer)				      { m_insertLayer = layer;					}

	void SetUseAsTestComp(const bool isUsed)			{ m_bUseAsTestComp = isUsed;			}
	void SetNeedMirVersion(const bool needMirVer)	
	{
		m_bNeedMirVersion = needMirVer;	
		m_sMirName.Format("%s_BOT", m_sName);
	}

};

class CCKTNAVPadstackMap : public CTypedMapStringToPtrContainer<CCKTNAVPadstack*>
{
public:
	CCKTNAVPadstackMap()	{}
	~CCKTNAVPadstackMap()	{ this->empty();	}
};

void Teradyne228xNAV_WriteFiles   (CString     filename, CCEtoODBDoc *doc, FormatStruct *format, int page_units);

#ifdef COMMENT

RSI Device Type	Navigate TYPE Attribute	STYLE Attribute
"Battery", "ANALOG",	SUBCLASS
"Capacitor	CAP	SUBCLASS
Capacitor_Array	CPCK	SUBCLASS
Capacitor_Polarized	PCAP	SUBCLASS
Capacitor_Tantalum	TCAP	SUBCLASS
Connector	CONN	SUBCLASS
Crystal	XTAL	SUBCLASS
Diode	DIODE	SUBCLASS
Diode_Array	DPCK	SUBCLASS
Diode_LED	LED	SUBCLASS
Diode_Zener	ZENER SUBCLASS
Filter	ANALOG	SUBCLASS
Fuse	FUSE	SUBCLASS
IC	LOGIC	SUBCLASS
IC_Digital	LOGIC	SUBCLASS
IC_Linear	ANALOG	SUBCLASS
Inductor	IND	SUBCLASS
Jumper	JUMPER	SUBCLASS
No_Test	OTHER	SUBCLASS
Opto	OPTO	SUBCLASS
Oscillator	ANALOG	SUBCLASS
Potentiometer	VRES	SUBCLASS
Power_Supply	ANALOG	SUBCLASS
Relay	RELAY	SUBCLASS
Resistor	RES	SUBCLASS
Resistor_Array	RPCK	SUBCLASS
Speaker	ANALOG	SUBCLASS
Switch	SWITCH	SUBCLASS
Test_Point	TESTPAD	SUBCLASS
Transformer	XFMR	SUBCLASS
Transistor	NPN	SUBCLASS
Transistor_Array	ANALOG	SUBCLASS
Transistor_FET_NPN	NFET	SUBCLASS
Transistor_FET_PNP	PFET	SUBCLASS
Transistor_Mosfet_NPN	NJFET	SUBCLASS
Transistor_Mosfet_PNP	PJFET	SUBCLASS
 Transistor_NPN	NPN	SUBCLASS
Transistor_PNP	PNP	SUBCLASS
Transistor_SCR	SCR	SUBCLASS
Transistor_Triac	TRIAC	SUBCLASS
Voltage_Regulator	VR	SUBCLASS
#endif

#endif