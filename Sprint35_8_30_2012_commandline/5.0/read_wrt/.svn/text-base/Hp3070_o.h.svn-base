// $Header: /CAMCAD/4.6/read_wrt/Hp3070_o.h 10    3/12/07 3:36p Lynn Phung $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#pragma once

#define  SMALL_DELTA       0.001
#define  MAX_PINMAP        5000
#define  MAX_BOARDS        255

#define  HP_DEVICENAME     1
#define  HP_COMPPREF       2
#define  HP_COMPNAME       3
#define  HP_DEVICEMAP      4
#define  HP_TESTPREF       5
#define  HP_TOOLINGPREF    6
#define  HP_TOOLINGSHAPE   7
#define  HP_KEYVAL         8


// a blank value means normal access, as opposed to NO_PROBE or PREFERRED
#define HP_NORMAL_ACCESS ""  


typedef struct
{
   char *deviceName;
   int maxPinCount;
} HPDevice;

#define HPDEV_NODELIBRARY    "NODE LIBRARY"
#define HPDEV_CAPACITOR      "CAPACITOR"
#define HPDEV_CONNECTOR      "CONNECTOR"
#define HPDEV_DIODE          "DIODE"
#define HPDEV_FET            "FET"
#define HPDEV_JUMPER         "JUMPER"
#define HPDEV_FUSE           "FUSE"
#define HPDEV_INDUCTOR       "INDUCTOR"
#define HPDEV_POTENTIOMETER  "POTENTIOMETER"
#define HPDEV_RESISTOR       "RESISTOR"   
#define HPDEV_SWITCH         "SWITCH"
#define HPDEV_TRANSISTOR     "TRANSISTOR"
#define HPDEV_ZENER          "ZENER"
#define HPDEV_PINLIBRARY     "PIN LIBRARY"
#define HPDEV_UNDEFINED      "UNDEFINED"

static HPDevice hpDeviceArray[] =
{
   "NODE LIBRARY",         9999,// this are all devices, which do not fall into the other classes
   "CAPACITOR",            2,
   "CONNECTOR",            9999,
   "DIODE",                2,
   "FET",                  3,
   "JUMPER",               2,
   "FUSE",                 2,
   "INDUCTOR",             2,
   "POTENTIOMETER",        3,
   "RESISTOR",             2,    
   "SWITCH",               2,
   "TRANSISTOR",           3,
   "ZENER",                2,
   "PIN LIBRARY",          9999,
   "UNDEFINED",            9999,// must be last
};

#define HPDEVICE_NODE_LIBRARY    0
#define HPDEVICE_CAPACITOR       1
#define HPDEVICE_CONNECTOR       2
#define HPDEVICE_DIODE           3
#define HPDEVICE_FET             4
#define HPDEVICE_JUMPER          5
#define HPDEVICE_FUSE            6
#define HPDEVICE_INDUCTOR        7
#define HPDEVICE_POTENTIOMETER   8
#define HPDEVICE_RESISTOR        9
#define HPDEVICE_SWITCH          10
#define HPDEVICE_TRANSISTOR      11
#define HPDEVICE_ZENER           12
#define HPDEVICE_PIN_LIBRARY     13
#define HPDEVICE_UNDEFINED       14

typedef struct
{
   CString netname;
   POSITION pos;
} HPVias;
typedef CTypedPtrArray<CPtrArray, HPVias*> HPViaArray;

typedef struct
{
   long accessEntityNum; // 0 means access entity not found
   double accessX;
   double accessY;

   long dataEntityNum; // 0 means data entity not found

   long probeEntityNum; // 0 means probe entity not found
   double probeX;
   double probeY;
} HPAccessProbe;
typedef CTypedPtrArray<CPtrArray, HPAccessProbe*> HPAccessProbeArray;

class PinMapEntry
{
public:
   int m_hpDeviceIndex;
   CString m_oldPin;
   CString m_newPin;

   PinMapEntry(int hpDevIndx, CString oldPin, CString newPin) { m_hpDeviceIndex = hpDevIndx; m_oldPin = oldPin; m_newPin = newPin; }
};

class PinMapArray : public CTypedPtrArray<CPtrArray, PinMapEntry*>
{
public:
   bool AddPinMap(int hpDeviceIndex, CString oldPin, CString newPin);
   PinMapEntry *FindPinMap(int hpDeviceIndex, CString oldPin);
};

typedef struct
{
   CString attrName;
   int hpDeviceIndex;
   CString userName;    // set to a user type like CRYSTAL etc .. this is all mapped to PIN LIBRARY
   int attrType;
} HPAttr;
typedef CTypedPtrArray<CPtrArray, HPAttr*> HPAttrArray;

typedef struct
{
   CString name;        // list of components in the datalist
   int bottom;
   int testnone;        // flagged as not to be tested
   int testflag;        // flagged as having a forced test attribute
   int written_out;     // if false, component must be still written out
   int smdflag;         // if ATT_SMD was set
   int inserttype;      // np stored inserttype;
   int pincnt;
} HP3070Comp;
typedef CTypedPtrArray<CPtrArray, HP3070Comp*> HP3070CompArray;

//typedef struct
//{
//   CString compName;
//   int hpDeviceIndex;
//   CString testString;
//}HP3070CompTest;
//typedef CTypedPtrArray<CPtrArray, HP3070CompTest*> HP3070CompTestArray;

class HP3070CompTest
{
public:
   HP3070CompTest(const CString compName, const int hpDeviceIndex, const CString testString);
   ~HP3070CompTest();

private:
   CString m_compName;
   int m_hpDeviceIndex;
   CStringArray m_testStringArray;
   CString m_testString;

public:
   CString getCompName()const { return m_compName; }
   int getHpDeviceIndex()const { return m_hpDeviceIndex; }
   CString getTestString()const { return m_testString; }
   void addTestString(const CString testString, const CString version);
   void writeTestString(FILE *fBrd);
};
typedef CTypedMapStringToPtrContainer<HP3070CompTest*> HP3070CompTestMap;

typedef struct
{
   CString defunit;
   CString value; 
   CString plustol;
   CString mintol;
   CString typ;
}Capdefault;

typedef struct
{
   CString test;  
}Conndefault;

typedef struct
{
   CString hival; 
   CString lowval;
}Diodedefault;

typedef struct
{
   CString  hires;   
   CString  lowres;
   CString  typ;
}Fetdefault;

typedef struct
{
   CString  defunit;
   CString  maxcurrent; 
}Fusedefault;

typedef struct
{
   CString  defunit;
   CString  value;
   CString  plustol;
   CString  mintol;
   CString  series;
   CString  typ;
}Inductordefault;

typedef struct
{
   CString  typ;
}Jumperdefault;

typedef struct
{
   CString  defunit;
   CString  value;
   CString  plustol;
   CString  mintol;
}Potentiometerdefault;

typedef struct
{
   CString  defunit;
   CString  value;
   CString  plustol;
   CString  mintol;
   CString  typ;
}Resistordefault;

typedef struct
{
   CString  hibeta;
   CString  lowbeta;
   CString  typ;
}Transistordefault;

typedef struct
{
   CString  voltage; // and value
   CString  plustol;
   CString  mintol;
}Zenerdefault;

typedef struct
{
   long     x;
   long     y;
   int      layer;   // 3 all 1 top 2 bottom
} HP3070_testkoo;
typedef CTypedPtrArray<CPtrArray, HP3070_testkoo*> TestkooArray;

typedef struct
{
   int            block_num;  //
   CString        name;
   double         drill;
   int            flag;       // BL_ACCESS flags from dbutil.h
   int            smd;
} HP3070Padstack;
typedef CTypedPtrArray<CPtrArray, HP3070Padstack*> PadstackArray;

typedef struct
{
   CString refName;
   int blockNum;
   double x,y;
   int rotation;
   int mirror;
} PCBInstance;

typedef struct
{
   int blockNumber;
   int mirror;
   CString boardName;
   BlockStruct *block;
} UniqueBoard;

typedef struct
{
   int posX;
   int posY;
   bool IsProbeAccessEmpty;
   CString probeAccess;
   CString probeSide;
} HP3070UnConnectVia;

typedef CTypedPtrArrayContainer<HP3070UnConnectVia*> UnConnectViaArray;