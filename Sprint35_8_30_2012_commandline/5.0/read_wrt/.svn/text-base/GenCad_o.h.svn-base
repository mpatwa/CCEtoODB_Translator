// $Header:

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// GENCAD_O.H

#if ! defined (__GenCad_o_h__)
#define __GenCad_o_h__

#pragma once


typedef struct
{
   CString  padname;
   CString  newpadname;
   int      rotation;   // rotation in degree
} GENCADPadRotStruct;
typedef CTypedPtrArray<CPtrArray, GENCADPadRotStruct*> PadRotArray;


typedef struct
{
   CString  padname;
//   int      layer;
   int      used_as_testcomp;
}GENCADPadstack;
typedef CTypedPtrArray<CPtrArray, GENCADPadstack*> GENCADPadstackArray;

typedef struct
{
   CString  netname;
   POSITION pos;
}GENCADRoutes;
typedef CTypedPtrArray<CPtrArray, GENCADRoutes*> GENCADRoutesArray;

//
typedef struct
{
   double   pinx, piny;
   double   rotation;
   CString  padstackname;
   char     mirror;
   char     drill;      // this is a drill hole
   CString  pinname;
}GENCADCompPinInst;
typedef CTypedPtrArray<CPtrArray, GENCADCompPinInst*> CompPinInstArray;

typedef struct
{
   CString  compname;
   CString  devicename;    
   CString  geomname;
}GENCADCompList;
typedef CTypedPtrArray<CPtrArray, GENCADCompList*> CComplistArray;

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

class CGenCadDevice
{
public:
	CGenCadDevice(CString name, CString partNumber)
	{
		m_sName = name;
		m_sPartNumber = partNumber;
		m_sValue.Empty();
		m_sPlusTol.Empty();
		m_sMinusTol.Empty();
		m_sTolerance.Empty();
	}
	~CGenCadDevice(){}

private:
	CString m_sName;
	CString m_sPartNumber;
	CString m_sValue;
	CString m_sPlusTol;
	CString m_sMinusTol;
	CString m_sTolerance;

public:
	CString GetName() const								{ return m_sName;					}
	CString GetPartNubmer() const						{ return m_sPartNumber;			}
	CString GetValue() const							{ return m_sValue;				}
	CString GetPlusTol() const							{ return m_sPlusTol;				}
	CString GetMinusTol() const						{ return m_sMinusTol;			}
	CString GetTolerance() const						{ return m_sTolerance;			}

	void SetValue(const CString value)				{ m_sValue = value;				}
	void SetPlusTol(const CString plusTol)			{ m_sPlusTol = plusTol;			}
	void SetMinusTol(const CString minusTol)		{ m_sMinusTol = minusTol;		}
	void SetTolerance(const CString tolerance)	{ m_sTolerance = tolerance;	}
};

class CGenCadDeviceMap : public CTypedMapStringToPtrContainer<CGenCadDevice*>
{
public:
	CGenCadDeviceMap()	{ genericDeviceCnt = 0;	}
	~CGenCadDeviceMap()	{ this->empty();			}

private:
	int genericDeviceCnt;		// counter for creating generic device name = "Device%d"

public:
	CGenCadDevice* AddDevice(const CString deviceName, const CString partNumber);
	CGenCadDevice* GenerateDevice();

	void WriteDevice(FILE *fp);
};

class CGenCadPadstack
{
public:
	CGenCadPadstack(BlockStruct* block)
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
	~CGenCadPadstack()
	{
		m_pBlock = NULL;
	}

private:
	CString m_sName;
	CString m_sMirName;			// Name of mirred version
	bool m_bUseAsTestComp;		// This padstack was used as a testpoint converted to single pin component
	bool m_bNeedMirVersion;		// This padstack need to output a mirrored version of itself
	bool m_bIsPadstack;			// Indicate if a real padstack or BL_TOOL because GENCAD output both as padstack with some syntax difference
   bool m_bIsbondPadstack;	   // Indicate if a real bond padstack or BL_TOOL because GENCAD output both as padstack with some syntax difference
	BlockStruct* m_pBlock;		// Pointer to the block
   int  m_insertLayer;        // layer in the insert of block

public:
	CString GetName() const									{ return m_sName;							}
	CString GetMirName() const								{ return m_sMirName;						}
	bool GetUseAsTestComp() const							{ return m_bUseAsTestComp;				}
	bool GetNeedMirVersion() const						{ return m_bNeedMirVersion;			}
	bool GetIsPadstack() const								{ return m_bIsPadstack;					}
	bool GetIsbondPadstack() const					   { return m_bIsbondPadstack;			}
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

class CGenCadPadstackMap : public CTypedMapStringToPtrContainer<CGenCadPadstack*>
{
public:
	CGenCadPadstackMap()	{}
	~CGenCadPadstackMap()	{ this->empty();	}
};

#endif
