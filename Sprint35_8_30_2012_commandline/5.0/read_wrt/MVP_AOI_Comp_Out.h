// $Header: /CAMCAD/4.5/read_wrt/MVP_AOI_Comp_Out.h 12    10/25/05 4:45p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2004. All Rights Reserved.
*/

#if !defined(__MVP_AOI_Comp_Out_h__)
#define __MVP_AOI_Comp_Out_h__

#pragma once

#include "CamCadDatabase.h"

#define PIN_ORIENTATION_NORTH		'N'
#define PIN_ORIENTATION_SOUTH		'S'
#define PIN_ORIENTATION_EAST		'E'
#define PIN_ORIENTATION_WEST		'W'

enum EPackageType
{
	packageTypeCONN	= 0,			// device type of connector
	packageTypeTHT		= 1,			// technology of THRU and not connector
	packageTypeCHIP	= 2,			// 2 pin SMD
	packageTypeSOIC	= 3,			// smd and 2 direction
	packageTypeQFP		= 4,			// smd and 4 direction
	packageTypeUSER	= 5,
};

enum EPinPatternType
{
	pinPatternPerimeter,
	pinPatternGrid
};

enum EQuadrantType  // Standard convention geometric quadrants
{
	quadrantOne,    // Northeast
	quadrantTwo,    // Northwest
	quadrantThree,  // Southwest
	quadrantFour    // Southeast
};

class CUniqueGeomStruct;

/******************************************************************************/
/*  CPinStruct Class
/******************************************************************************/
class CPinStruct
{
public:
	CPinStruct(DataStruct *data);
	~CPinStruct();

private:
	DataStruct* m_pData;
	char m_cOrientation;

public:
	char GetOrientation() const									{ return m_cOrientation;			}
	DataStruct* GetData()											{ return m_pData;						}
   CPoint2d getOrigin();

	void SetOrientation(const char orientation)				{ m_cOrientation = orientation;	}
	void WritePinStruct(FILE* outFile);
};

//_____________________________________________________________________________
class CPinStructList : public CTypedPtrListContainer<CPinStruct*>
{
public:
	CPinStructList(bool isContainer);
	~CPinStructList();

private:
	CString m_sPinOrientation;

public:
	int GetOrientationCount();
	void DetermineOrientation();
	void WriteCompEntry(FILE* outFile,CUniqueGeomStruct& uniqueGeom,CCEtoODBDoc& camCadDoc);
	void WritePinList(FILE* outFile,CUniqueGeomStruct& uniqueGeom,CCEtoODBDoc& camCadDoc);
   double getClosestNeighborDistance(CPinStruct& pinStruct);

   static CExtent getPinExtent(BlockStruct* pinGeom);
};

//_____________________________________________________________________________
class CPinListBucket
{
private:
   CTypedMapIntToPtrContainer<CPinStructList*> m_bucket;
   double m_pinPitch;
	double m_pinSeparation;	// The distance between the two set of pins, ex: distance between East and West or North and South
   int m_pinCount;

public:
   CPinListBucket();
   ~CPinListBucket();

public:
   bool lookup(int key, CPinStructList **pinList);
   void setAt(int ordinate,CPinStructList* pinList) { m_bucket.SetAt(ordinate,pinList); }
   POSITION getStartPosition() const { return m_bucket.GetStartPosition(); }
   void getNextAssoc(POSITION& pos,int& ordinate,CPinStructList*& pinStructList)
      { m_bucket.GetNextAssoc(pos,ordinate,pinStructList); }

   void add(int ordinate,CPinStruct* pin);
   void analyze(DbUnit centroidOrdinate,const char leOrientation,const char gtOrientation);

   int getPinPitch(CCEtoODBDoc& camCadDoc);
	int getPinSeparation(CCEtoODBDoc& camCadDoc);
   int getPinCount() const { return m_pinCount; }
};

//_____________________________________________________________________________
class CPinListBuckets
{
private:
   CPinListBucket m_xBucket;
   CPinListBucket m_yBucket;

public:
   void putPinsInBuckets(CPinStructList* pins,CCEtoODBDoc& camCadDoc);
   void analyzeBuckets(BlockStruct* geom,CCEtoODBDoc& camCadDoc);
   int getHorizontalPinPitch(CCEtoODBDoc& camCadDoc);
   int getVerticalPinPitch(CCEtoODBDoc& camCadDoc);
   int getHorizontalPinSeparation(CCEtoODBDoc& camCadDoc);
   int getVerticalPinSeparation(CCEtoODBDoc& camCadDoc);
   int getHorizontalPinCount();
   int getVerticalPinCount();
	EPinPatternType determinePinPattern();
	EQuadrantType   determinePinOneQuadrant(CPoint2d partCenter);
};

/******************************************************************************/
/*  CUniqueGeomStruct Class
/******************************************************************************/
class CUniqueGeomStruct
{
public:
	CUniqueGeomStruct(CCEtoODBDoc& camCadDoc, CString uniqueName, DataStruct* compData, bool mirrorLayers, bool mirrorFlip, int insertLayerIndex);
	~CUniqueGeomStruct();

private:
	CString m_sName;
	BlockStruct* m_pGeomBlock;
	CPinStructList m_pinList;
	EPackageType m_ePackageType;
   ComponentMountTypeTag m_mountType;

	bool m_bMirrorLayers;
	bool m_bMirrorFlip;
	int m_iInsertLayerIndex;
   int m_horizontalPinPitch;        // in mils
   int m_verticalPinPitch;          // in mils
	int m_horizontalPinSeparation;	// The distance the two set of East and West pins
	int m_verticalPinSeparation;		// The distance the two set of North and East pins
   int m_horizontalPinCount;
   int m_verticalPinCount;
	EPinPatternType m_pinPattern;    // Perimeter or Grid
	EQuadrantType m_pin1quadrant;    // Quadrant in which pin 1 lies

	void gatherPinData(CCEtoODBDoc& camCadDoc, DataStruct* compData);
	void writeCONN(FILE* outFile);
	void writeTHT(FILE* outFile);
	void writeCHIP(FILE* outFile);
	void writeSOIC(FILE* outFile);
	void writeQFP(FILE* outFile);
	void writeUSER(FILE* outFile);

public:
	BlockStruct* GetGeom()											{ return m_pGeomBlock;					}
	EPackageType GetPackageType()									{ return m_ePackageType;				}
   CString      GetPackageName()                         { return m_sName;                	}

	void SetPackageType(const EPackageType packageType)	{ m_ePackageType = packageType;		}
	void SetMirrorLayers(const bool mirrored)					{ m_bMirrorLayers = mirrored;			}
	void SetMirrorFlip(const bool mirrored)					{ m_bMirrorFlip = mirrored;			}
	void SetInsertLayerIndex(const int index)					{ m_iInsertLayerIndex = index;		}

	void WriteGeom(FILE* outFile,CCEtoODBDoc& camCadDoc);

   double getBodyLength();
   double getBodyWidth();

   double getXSize();
   double getYSize();

   int getPinPitch();
   int getHorizontalPinPitch() const							{ return m_horizontalPinPitch;		}
   int getVerticalPinPitch()   const							{ return m_verticalPinPitch;			}

	int getPinSeparation();
	int getHorizontalPinSeparation() const						{ return m_horizontalPinSeparation;	}
	int getVerticalPinSeparation() const						{ return m_verticalPinSeparation;	}
   
	int getSidePinCount();
   int getHorizontalPinCount() const							{ return m_horizontalPinCount;		}
   int getVerticalPinCount()   const							{ return m_verticalPinCount;			}

	EPinPatternType getPinPattern() const						{ return m_pinPattern;					}
//	EQuadrantType getPinOneQuadrant() const					{ return m_pin1quadrant;				}
	double getPinOneQuadrantDegree();

	CExtent getBodyExtent();
   ComponentMountTypeTag getMountType() const				{ return m_mountType;					}

   void calculateOrientations(CCEtoODBDoc& camCadDoc);
   void calculateSimpleOrientations(CCEtoODBDoc& camCadDoc);
   void doBucketAlgorithm(CCEtoODBDoc& camCadDoc);
};

//_____________________________________________________________________________
class CUniqueFiducialStruct
{
public:
	CUniqueFiducialStruct(CCEtoODBDoc& camCadDoc, CString uniqueName, DataStruct* compData);
	~CUniqueFiducialStruct();

private:
	CString m_sName;
	BlockStruct* m_pGeomBlock;
	double m_xSize;
	double m_ySize;

	void calculateXYSizeOfFiducialGeom(CCEtoODBDoc& camCadDoc);

public:
	BlockStruct* GetGeom()		{ return m_pGeomBlock;			}
	CString GetName() const		{ return m_sName;					}

	void WriteGeom(FILE* outFile,CCEtoODBDoc& camCadDoc);
};

//_____________________________________________________________________________
class CUniqueGeomStructMap : public CTypedMapStringToPtrContainer<CUniqueGeomStruct*>
{
private:
   CCEtoODBDoc& doc;
//	CTypedMapStringToPtrContainer<CUniqueGeomStruct*> m_uniqueGeomStructMap;
	CTypedMapStringToPtrContainer<CUniqueFiducialStruct*> m_uniqueFiducialStructMap;

public:
	CUniqueGeomStructMap(CCEtoODBDoc& doc);
	~CUniqueGeomStructMap();

private:
	//void addComponentGeom(CCEtoODBDoc& camCadDoc, DataStruct* data);
	//void addFiducialGeom(CCEtoODBDoc& camCadDoc, DataStruct* data);
	void writeDefinitionByPackageType(FILE* outFile, EPackageType packageType,const CMvpAoiSettings& m_mvpAoiSettings);

public:
	CUniqueGeomStruct* AddGeomStruct(CCEtoODBDoc& camCadDoc, DataStruct* data);
	CUniqueFiducialStruct* AddFiducailStruct(CCEtoODBDoc& camCadDoc, DataStruct* data);
	void WriteDefinitions(FILE* outFile,const CMvpAoiSettings& mvpAoiSettings);
};

//_____________________________________________________________________________
class CMvpAoiCompWriter
{
private:
   FILE* topFile;
   FILE* botFile;
   CCEtoODBDoc& doc;
   FileStruct* m_file;
   double unitFactor;
   CUniqueGeomStructMap m_topGeomMap;
   CUniqueGeomStructMap m_botGeomMap;
   CString m_fileName;
   CMvpAoiSettings& m_mvpAoiSettings;
	CFormatStdioFile m_logFile;

public:
   CMvpAoiCompWriter(CCEtoODBDoc& camCadDoc,const CString fileName,CMvpAoiSettings& mvpAoiSettings);
   void write();

private:
   void doInserts();
	void checkComponentCentroids();
};

#endif