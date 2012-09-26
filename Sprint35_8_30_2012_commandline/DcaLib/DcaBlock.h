// $Header: /CAMCAD/DcaLib/DcaBlock.h 10    6/30/07 1:47a Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/

#if !defined(__DcaBlock_h__)
#define __DcaBlock_h__

#pragma once

#include <afxtempl.h>
#include "Dca.h"
#include "DcaData.h"
#include "DcaMessageFilter.h"
#include "DcaApertureShape.h"
#include "DcaBlockType.h"
#include "DcaExtent.h"
#include "DcaContainer.h"

//_____________________________________________________________________________
// FLAG Definitions for Blocks
#define BL_APERTURE                    0x00000001 // Block is an APERTURE
#define BL_BLOCK_APERTURE              0x00000002 // Graphic Block assigned to be an APERTURE

#define BL_FILE                        0x00000004 // Block is a FILE
#define BL_WIDTH                       0x00000008 // Block is a WIDTH
#define BL_TOOL                        0x00000010 // Block is a TOOL
#define BL_BLOCK_TOOL                  0x00000020 // Graphic Block assigned to be a TOOL
#define BL_GLOBAL                      0x00000040 // Global block - is not dependend on file_num

#define BL_COMPLEX_PADSTACK            0x00000080 // used to identify padstacks which have complex shapes.

#define BL_LOCALBLOCK                  0x00000100 // this is a Local Copy of a block
#define BL_SMALLWIDTH                  0x00000200 // this block is CAMCAD's SMALL Width
#define BL_SPECIAL                     0x00000400 // Special Geometry
#define BL_COMPLEX_TOOL                0x00000800 // used to identify TOOLS which have complex shapes.

// for speed reasons, we can not always run thru all layers to find out if a padstack (and possible other types)
// exist on "Outer" layers. The maintaining of this flag is not automated.
#define BL_ACCESS_TOP                  0x00001000  // used in blocktype_padstack
#define BL_ACCESS_BOTTOM               0x00002000  // used in blocktype_padstack
#define BL_ACCESS_OUTER                0x00003000  // BL_MANUFACTURING_TOP | BL_MANUFACTURING_BOTTOM
#define BL_ACCESS_NONE                 0x00004000  // not visible on the outsides
#define BL_ACCESS_TOP_ONLY             0x00008000  // Mentorlike only if placed on top
#define BL_ACCESS_BOTTOM_ONLY          0x00010000  // Mentorlike only if placed on bottom

class CCamCadData;
class FileStruct;

//_____________________________________________________________________________
/* 
   This table will help to understand what is visible if with corresponding padstack design surface and insert 

   PADSTACK_DESIGN_SURFACE                               INSERT_WITH_NO_MIRROR                  INSERT_WITH_MIRRORED
   ==================================================================================================================
   designSurfaceUndefine                                 no                                     no
   designSurfaceNone                                     no                                     no
   designSurfaceBuriedSurface                            no                                     no
   designSurfacePlacedTopOppositeSurface                 bottom                                 no       
   designSurfacePlacedTopPlacedSurface                   top                                    no       
   designSurfacePlacedBottomOppositeSurface              no                                     top                                       
   designSurfacePlacedBottomPlacedSurface                no                                     bottom
   designSurfaceTopOnlySurface                           top                                    no
   designSurfaceBottomOnlySurface                        no                                     bottom
   designSurfaceOppositeSurface                          bottom                                 top               
   designSurfacePlacedSurface                            top                                    bottom
   designSurfaceBothSurface                              top & bottom                           top & bottom
*/
// Do not change the order of these enum because it is use to determine the priority for overwritting an existing value 
// The member further down the order has higher priority.  Members of lower priority can be overwritten with members
// members of higher priority.
enum DesignSurfaceValueTag
{
   designSurfaceUndefine                        = 0,  // the design surface cannot be categorized into any of the followings
   designSurfaceNone                            = 1,  // "NONE"
   designSurfaceBuriedSurface                   = 2,  // "BURIED_SURFACE"
   designSurfacePlacedTopOppositeSurface        = 3,  // "PLACED_TOP_OPPOSITE_SURFACE"       <== use for top only
   designSurfacePlacedTopPlacedSurface          = 4,  // "PLACED_BOTTOM_OPPOSITE_SURFACE"    <== use for top only
   designSurfacePlacedBottomOppositeSurface     = 5,  // "PLACED_TOP_PLACED_SURFACE"         <== use for bottom only
   designSurfacePlacedBottomPlacedSurface       = 6,  // "PLACED_BOTTOM_PLACED_SURFACE"      <== use for bottom only
   designSurfaceTopOnlySurface                  = 7,  // "TOP_ONLY_SURFACE"
   designSurfaceBottomOnlySurface               = 8,  // "BOTTOM_ONLY_SURFACE"
	designSurfaceOppositeSurface						= 9,	// "OPPOSITE_SURFACE"
	designSurfacePlacedSurface							= 10,	// "PLACED_SURFACE"
	designSurfaceBothSurface							= 11,	// "BOTH_SURFACE"
};                                              

CString DesignSurfaceValueTagToString(DesignSurfaceValueTag designSurfaceValueTag);
DesignSurfaceValueTag DesignSurfaceValueStringToTag(CString designSurfaceValueString);

//_____________________________________________________________________________
enum OutlineIndicatorOrientationTag
{
   // 3--2--1
   // |     |
   // 4     0
   // |     |
   // 5--6--7

   outlineIndicatorOrientationRight       =  0,
   outlineIndicatorOrientationTopRight    =  1,
   outlineIndicatorOrientationTop         =  2,
   outlineIndicatorOrientationTopLeft     =  3,
   outlineIndicatorOrientationLeft        =  4,
   outlineIndicatorOrientationBottomLeft  =  5,
   outlineIndicatorOrientationBottom      =  6,
   outlineIndicatorOrientationBottomRight =  7,
   outlineIndicatorOrientationUpperBound  =  7,
   outlineIndicatorOrientationLowerBound  =  0,
   outlineIndicatorOrientationUndefined   = -1
};

CString outlineIndicatorOrientationTagToString(OutlineIndicatorOrientationTag tagValue);
OutlineIndicatorOrientationTag stringToOutlineIndicatorOrientationTag(const CString& tagValue);

//_____________________________________________________________________________
class CWriteFormat;
class CAttributes;
class CCamCadDatabase;
class CBlockDirectory;

enum AttributeUpdateMethodTag;
enum ValueTypeTag;

//_____________________________________________________________________________
class BlockStruct
{
   friend CBlockDirectory;

private:
   CCamCadData* m_camCadData;

   int         m_blockNumber;

   short       m_libraryBlockNumber;    // number of block this local block was derived from
   CString     m_name;
   CString     m_originalName; // blockname, with original name, because the real blockname can not be recursive.

   short        m_fileNumber;
   short        m_marked;
   DbFlag       m_flags;       // see BL_?? dbutil.h
   BlockTypeTag m_blockType;  // see BLOCKTYPE_?? dbutil.h        
   CString      m_comment;

   CDataList    m_dataList;
   CExtent      m_extent;

   // APERTURE DATA
   ApertureShapeTag m_shape;
   double      m_sizeA;
   double      m_sizeB;
   // for thermal
   double      m_sizeC;      // gap width
   double      m_sizeD;      // spoke angle
   double      m_xOffset;
   double      m_yOffset;
   double      m_rotationRadians;
   short       m_dCode;   
   short       m_spokeCount; 

   // TOOL DATA
   short       m_tCode;
   double      m_toolSize;
   short       m_toolBlockNumber;    // block number of tool display geometry
   short       m_toolType;

   bool        m_toolHolePlated : 1;
   bool        m_toolHolePunched : 1;
   bool        m_toolDisplay    : 1;    // display a geom block or original generic tool graphic
   bool        m_package        : 1;
   bool        m_checkedFlag    : 1;    // used for data check routines, e.g. recursive reference check

   mutable CAttributes* m_attributes;

   void GenerateGeometryDesignSurfaceAttributes(CCamCadData& camCadData, bool regenerate);
   void GeneratePadstackDesignSurfaceAttributes(CCamCadData& camCadData, bool regenerate);
   DesignSurfaceValueTag getDesignSurface(DesignSurfaceValueTag topDesignSurfaceValueTag, DesignSurfaceValueTag botDesignSurfaceValueTag);

public:
   BlockStruct();
   BlockStruct(CCamCadData& camCadData,int blockNumber);
   BlockStruct(CCamCadData& camCadData,int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType);
   ~BlockStruct();

private:
   void init();
   void updateName(const CString& newName);
   void updateFileNumber(int newFileNumber);
   void updateBlockType(BlockTypeTag newBlockType);
   void removeBlock();

public:
   void setProperties(const BlockStruct& other,bool copyAttributesFlag=true);

public:
   CCamCadData* getCamCadData() { return m_camCadData; }

   // Accessors
   short getLibraryNumber() const                  { return m_libraryBlockNumber; }
   void setLibraryNumber(short libraryNumber)      { m_libraryBlockNumber = libraryNumber; }

   const CString& getName() const                  { return m_name; }
   void setName(const CString& blockName);
   // getNameRef() - should be deprecated and removed in future refactoring - knv
   CString& getNameRef()                           { return m_name; }  

   const CString& getOriginalName() const          { return m_originalName; }
   void setOriginalName(const CString& blockName);
   // getOriginalNameRef() - should be deprecated and removed in future refactoring - knv
   CString& getOriginalNameRef()                   { return m_originalName; }  

   short getFileNumber() const;
   void setFileNumber(short fileNumber);

   short getMarked() const                         { return m_marked; }
   void setMarked(short marked)                    { m_marked = marked; }

   const CString& getComment() const               { return m_comment; }
   void setComment(const CString& comment)         { m_comment = comment; }

   bool getPackage() const                         { return m_package; }
   void setPackage(bool package)                   { m_package = package; }

   double getSizeA() const                         { return m_sizeA; }
   void setSizeA(double size)                      { m_sizeA = size; }

   double getSizeB() const                         { return m_sizeB; }
   void setSizeB(double size)                      { m_sizeB = size; }

   double getSizeC() const                         { return m_sizeC; }
   void setSizeC(double size)                      { m_sizeC = size; }

   double getSizeD() const                         { return m_sizeD; }
   void setSizeD(double size)                      { m_sizeD = size; }

   double getXoffset() const                       { return m_xOffset; }
   void setXoffset(double offset)                  { m_xOffset = offset; }

   CPoint2d getOffset() const                      { return CPoint2d(m_xOffset,m_yOffset); }
   void setOffset(const CPoint2d& offset)          { m_xOffset = offset.x;  m_yOffset = offset.y; }

   double getYoffset() const                       { return m_yOffset; }
   void setYoffset(double offset)                  { m_yOffset = offset; }

   double getRotation() const                      { return m_rotationRadians; }
   void setRotation(double rotation)               { m_rotationRadians = rotation; }
   double getRotationRadians() const               { return m_rotationRadians; }
   void setRotationRadians(double radians)         { m_rotationRadians = radians; }
   double getRotationDegrees() const;
   void setRotationDegrees(double degrees);

   short getDcode() const                          { return m_dCode; }
   void setDcode(short dCode)                      { m_dCode = dCode; }

   short getSpokeCount() const                     { return m_spokeCount; }
   void setSpokeCount(short spokeCount)            { m_spokeCount = spokeCount; }

   short getTcode() const                          { return m_tCode; }
   void setTcode(short tCode)                      { m_tCode = tCode; }

   double getToolSize() const                      { return m_toolSize; }
   void setToolSize(double size)                   { m_toolSize = size; }

   BOOL getToolDisplay() const                     { return m_toolDisplay; }
   void setToolDisplay(bool display)               { m_toolDisplay = display; }
   void setToolDisplay(BOOL display)               { m_toolDisplay = (display != 0); }

   short getToolBlockNumber() const                { return m_toolBlockNumber; }
   void setToolBlockNumber(short number)           { m_toolBlockNumber = number; }

   short getToolType() const                       { return m_toolType; }
   void setToolType(short toolType)                { m_toolType = toolType; }

   bool getToolHolePlated() const                  { return m_toolHolePlated; }
   void setToolHolePlated(bool toolHolePlated)     { m_toolHolePlated = toolHolePlated; }
   void setToolHolePlated(BOOL toolHolePlated)     { m_toolHolePlated = (toolHolePlated != 0); }

   bool getToolHolePunched() const                  { return m_toolHolePunched; }
   void setToolHolePunched(bool toolHolePunched)     { m_toolHolePunched = toolHolePunched; }
   void setToolHolePunched(BOOL toolHolePunched)     { m_toolHolePunched = (toolHolePunched != 0); }

   bool getCheckedFlag() const                     { return m_checkedFlag; }
   void setCheckedFlag(bool flag)                  { m_checkedFlag = flag; }

   ApertureShapeTag getShape() const;
   void setShape(ApertureShapeTag shape);
   void setShape(short shape);

   CAttributes* getAttributes() const              { return m_attributes; }
   CAttributes*& getAttributesRef() const           { return m_attributes; }
   CAttributes*& getDefinedAttributes();
   CAttributes& attributes() const;
   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);
   bool lookUpAttrib(WORD keyword, CAttribute *&attribute);

   BlockTypeTag getBlockType() const               { return (BlockTypeTag)m_blockType; }
   void setBlockType(BlockTypeTag blockType);
   void setBlockType(short blockType)              { setBlockType(intToBlockTypeTag(blockType)); }

   DbFlag getFlags() const;
   void setFlags(DbFlag blockFlag);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   int getBlockNumber() const                      { return m_blockNumber; }
   void swapBlockNumber(BlockStruct& other);

   double getXmin() const                          { return m_extent.getXmin(); }
   double getYmin() const                          { return m_extent.getYmin(); }
   double getXmax() const                          { return m_extent.getXmax(); }
   double getYmax() const                          { return m_extent.getYmax(); }
   CExtent getExtent() const                       { return m_extent; }
   CExtent getExtent(CCamCadData& camCadData);

   bool extentIsValid() const                      { return m_extent.isValid(); }
   void resetExtent();
   void setExtent(double x0,double y0,double x1,double y1);
   void setExtent(const CExtent& extent);
   void updateExtent(double x,double y);
   void calculateVisibleBlockExtents(CCamCadData& camCadData);
   void calculateBlockExtents(CCamCadData& camCadData,bool checkOnlyVisibleEntitiesFlag=false,CMessageFilter* messageFilter=NULL);

   bool isTechnologyThruHole(CCamCadData& camCadData) const;
   bool isTechnologySmd(CCamCadData& camCadData) const;
   bool isAperture() const;
   bool isSimpleAperture() const;
   bool isComplexAperture() const;
   bool isTool() const;
   bool isDrillHole() const;
   int getComplexApertureSubBlockNumber() const    { return (int)m_sizeA; }
   void setComplexApertureSubBlockNumber(int blockNumber) { m_sizeA = blockNumber; }
   CBasesVector getApertureBasesVector() const;
   CTMatrix getApertureTMatrix() const;

   DataStruct *GetPackage() const;
   DataStruct *GetCentroidData() const;

   DataStruct* getPin() const; // any pin
   DataStruct* getPin(const CString& pinName) const; // returns the pad stack insert for the given pinName
   DataStruct* getPinUsingPadstack(CCamCadData &doc, const CString& padstackGeomName) const; // returns pin that inserts padstack named padstackGeomName
   DataStruct* getPinUsingPadstack(const int n) const; // returns pin that inserts nth padstack

   DataStruct* FindInsertData(CString refname, InsertTypeTag insertType);

   int getPadstackCount() const;  // return number of different padstack geometries used by pin inserts
   int getPinCount() const;
   int getNoConnectPinCount() const;
   bool hasNamedPin() const;
   bool hasNumberedPin() const;
   POSITION getHeadDataPosition() const;
   DataStruct* getNextData(POSITION& pos) const;
   POSITION getHeadDataInsertPosition() const;
   DataStruct* getNextDataInsert(POSITION& pos) const;
   DataStruct* getAtData(POSITION pos) const;
   int getDataCount() const;
   CDataList& getDataList() { return m_dataList; }
   const CDataList& getDataList() const { return m_dataList; }

   CPolyList* getAperturePolys(CCamCadData& camCadData, bool convertToOutline = false);
   CPolyList* convertApertureToPoly();
   CPoint2d getPinCentroid() const;
   CExtent getPinExtent() const;
   CBasesVector getPinMajorMinorAxes() const;
   bool verifyExtents() const;

   DesignSurfaceValueTag getDesignSurface(CCamCadData& camCadData);
	bool IsDesignedOnTop(CCamCadData& camCadData);
   bool IsDesignedOnBottom(CCamCadData& camCadData);
   bool IsDesignedTopOnly(CCamCadData& camCadData);
   bool IsDesignedBottomOnly(CCamCadData& camCadData);
   bool IsAccessibleFromTop(CCamCadData& camCadData, bool mirrored);
   bool IsAccessibleFromBottom(CCamCadData& camCadData, bool mirrored);
   void GenerateDesignSurfaceAttribute(CCamCadData& camCadData, bool regenerate);

   OutlineIndicatorOrientationTag getComponentOutlineIndicatorOrientation(CCamCadData& camCadData);
   void setComponentOutlineIndicatorOrientation(CCamCadData& camCadData,OutlineIndicatorOrientationTag outlineIndicatorOrientation);
   bool generateDefaultComponentOutline(CCamCadData& camCadData);

   //void RemoveDataFromList(CCEtoODBDoc *doc, DataStruct *data, POSITION dataPos = NULL);
   //void RemoveDataByGraphicClass(CCEtoODBDoc *doc, GraphicClassTag graphicClass);
   //void RemoveDataByInsertType(CCEtoODBDoc *doc, InsertTypeTag insertType);

   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL,bool doAttributes=false);
   bool isValid() const;
   void dump(CWriteFormat& writeFormat,int depth = -1);
   void Scale(double factor);

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCEtoODBDoc *doc);
   DataStruct *FindData(const CString& refdes);
   DataStruct *FindData(const CString& refdes, InsertTypeTag insertType);

   DataStruct *GetFirstPinInsertData();
   BlockStruct *GetFirstPinInsertPadstackGeometry (CCamCadData& camCadData);
   int getDiePinCount() const;
};

//_____________________________________________________________________________
class CBlockArray
{
friend BlockStruct;

private:
   CCamCadData& m_camCadData;

   CTypedPtrArrayContainer<BlockStruct*> m_blocks;

public:
   CBlockArray(CCamCadData& camCadData,int growBy=100);
   ~CBlockArray();
   void empty();

   int getSize() const;
   BlockStruct& getNewBlock();
   BlockStruct& getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct* getBlockAt(int index) const;
   BlockStruct& getDefinedBlockAt(int index);
   BlockStruct& getDefinedBlockAt(int index,const CString& name,int fileNumber,BlockTypeTag blockType);
   bool swapBlocks(BlockStruct* block0,BlockStruct* block1);

   bool isValid() const;
   void GenerateDesignSurfaceInfo(CCamCadData& camCadData, bool regenerate);

private:
   bool removeBlock(BlockStruct* block);

};

//_____________________________________________________________________________
class CBlockList
{
private:
   CTypedPtrListContainer<BlockStruct*> m_blocks;

public:
   CBlockList(bool isContainer=true);
   ~CBlockList();
   void empty();

   bool isEmpty() const;
   int getCount() const;
   BlockStruct* getHead();
   POSITION getHeadPosition() const;
   BlockStruct* getNext(POSITION& pos) const;

   void addTail(BlockStruct* block);
   bool remove(BlockStruct* block);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
//class CCamCadBlockMap : public CTypedPtrMap<CMapStringToPtr,CString,BlockStruct*>
class CMapOfBlockLists
{
private:
   CTypedMapStringToPtrContainer<CBlockList*> m_mapOfBlockLists;

public:
   CMapOfBlockLists(int hashSize);

   //void empty();
   bool contains(const CString& blockName) const;
   BlockStruct* getAt(const CString& blockName) const;  // returns first element in list, or NULL if map entry is nonexistant
   CBlockList* getListAt(const CString& blockName) const;  // returns list of blocks with the given name.

   void add(BlockStruct* block);
   bool remove(BlockStruct* block);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CArrayOfMapsOfBlockLists
{
private:
   CTypedPtrArrayContainer<CMapOfBlockLists*> m_arrayOfMapsOfBlockLists;  // indexed by block type

public:
   void SetSize(int newSize,int growBy = -1);
   int GetSize() const;
   CMapOfBlockLists* GetAt(int index) const;
   void SetAtGrow(int index,CMapOfBlockLists* mapOfBlockLists);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
//class CCamCadBlockMapArray : public CObject
class CBlockDirectoryForFile
{
private:
   CMapOfBlockLists m_map;  // indexed by block name, accomodates multple blocks with the same name
   CArrayOfMapsOfBlockLists m_arrayOfMapsOfBlockLists;  // indexed by block type

   CCamCadData& m_camCadData;

public:
   CBlockDirectoryForFile(CCamCadData& camCadData);
   //~CBlockDirectoryForFile();
   //void deleteAll();

   void add(BlockStruct* block);
   bool remove(BlockStruct* block);
   BlockStruct* getAt(const CString& name) const;                          // looks in m_map;
   BlockStruct* getAt(const CString& name,BlockTypeTag blockType) const;   // looks in m_arrayOfMapsOfBlockLists
   CBlockList*  getListAt(const CString& name) const;
   CBlockList*  getListAt(const CString& name,BlockTypeTag blockType) const;

   //CCamCadBlockMap& getMapAt(int fileNumber);
private:
   CMapOfBlockLists* getMapOfBlockListsForBlockType(BlockTypeTag blockType) const;
   CMapOfBlockLists& getDefinedMapOfBlockListsForBlockType(BlockTypeTag blockType);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
class CBlockDirectoryForFiles
{
private:
   CTypedPtrArrayContainer<CBlockDirectoryForFile*> m_blockDirectoryForFiles;

public:
   void SetSize(int newSize,int growBy = -1);
   int GetSize() const;
   CBlockDirectoryForFile* GetAt(int index) const;
   void SetAtGrow(int index,CBlockDirectoryForFile* blockDirectoryForFile);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

//_____________________________________________________________________________
//class CCamCadBlockIndex : public CObject
class CBlockDirectory
{
   friend void BlockStruct::setName(const CString& blockName);
   friend void BlockStruct::setFileNumber(short fileNumber);
   friend void BlockStruct::setBlockType(BlockTypeTag blockType);

private:
   CCamCadData& m_camCadData;

   CBlockArray m_blockArray;                   // block container
   CBlockDirectoryForFiles m_blockDirectoryForFiles;  // indexed by file number
   //int m_volatileChangeDetectedCnt;
   //bool m_indexedFlag;

public:
   CBlockDirectory(CCamCadData& camCadData);
   ~CBlockDirectory();

public:
   CBlockArray& getBlockArray()  { return m_blockArray; }

   BlockStruct& getNewBlock(const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct& getDefinedBlock(const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct& getDefinedBlock(int blockNumber,const CString& name,int fileNumber,BlockTypeTag blockType);
   BlockStruct& getDefinedBlock(int blockNumber);
   BlockStruct* getAt(int blockNumber) const;
   BlockStruct* getAt(const CString& name) const;
   BlockStruct* getAt(const CString& name,int fileNumber) const;
   BlockStruct* getAt(const CString& name,int fileNumber,BlockTypeTag blockType) const;
   CBlockList*  getListAt(const CString& name) const;
   CBlockList*  getListAt(const CString& name,int fileNumber) const;
   CBlockList*  getListAt(const CString& name,int fileNumber,BlockTypeTag blockType) const;

   BlockStruct& copyBlock(const CString& newName,BlockStruct& srcBlock,bool copyAttributesFlag=true);
   void remove(BlockStruct* block);
   void removeFile(FileStruct& file);

private:
   void updateName(BlockStruct& block,const CString& newBlockName);
   void updateFileNumber(BlockStruct& block,int newFileNumber);
   void updateBlockType(BlockStruct& block,BlockTypeTag newBlockType);
   //void invalidateIndex();
   //void reindex();
   //void verifyIndex() { if (!m_indexedFlag) reindex(); }

//private:
//   CMapOfBlockLists& getMapOfBlockListsForFile(int fileNumber);
   void add(BlockStruct* block);
   void add(BlockStruct* block,int fileNumber);

public:  // diagnostics
   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

// end BLOCKS.H
#endif
