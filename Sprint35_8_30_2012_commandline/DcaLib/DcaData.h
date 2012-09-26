// $Header: /CAMCAD/DcaLib/DcaData.h 9     6/30/07 2:58a Kurt Van Ness $

#if !defined(__DcaData_h__)
#define __DcaData_h__

#pragma once

#include "Dca.h"
#include "DcaContainer.h"
#include "DcaInsertType.h"

class CPolyList;
class TextStruct;
class InsertStruct;
class PointStruct;
class BlobStruct;
class CAttributes;
class CCamCadData;
class LayerStruct;
class CAttribute;
class CWriteFormat;
class CPoint2d;
class CInsertTypeMask;
class CExtent;
class CBasesVector;
class CPoly3DList;

enum GraphicClassTag;
enum DataTypeTag;
enum InsertTypeTag;
enum ValueTypeTag;
enum AttributeUpdateMethodTag;

//_____________________________________________________________________________
enum SymmetryCriteriaTag
{
   symmetryCriteriaOrigin,
   symmetryCriteriaPadstack,
   symmetryCriteriaPad,
   symmetryCriteriaUndefined,
};

#if defined(EnableDcaCamCadDocLegacyCode)
//_____________________________________________________________________________
class ExtentRect 
{
public:
   double left, top, right, bottom;

   ExtentRect();
   ExtentRect& operator=(const CExtent& extent);
   void reset();

   bool isCorrupt();
   bool isValid();
};
#endif

//_____________________________________________________________________________
class DataStruct
{
friend CCamCadData;

private:
   long          m_entityNumber;

   DataTypeTag   m_dataType;

   unsigned char m_selected:1;         // selected item
   unsigned char m_marked:1;           // group mark
   unsigned char m_negative:1;         // this entity must be drawn inverse in this layer, which is like a void without a parent.
   unsigned char m_colorOverride:1;   

   COLORREF m_overrideColor;
   COLORREF m_highlightColor;         // Case 1156: temporary coloring for highlighting purposes, not saved

   GraphicClassTag m_graphicClass:7;  // class is defined in  dbutil.h CLASS_ and must be used for intelligent conversions.
                                      // all class 0 is graphic and insert only. 
                                      // Example - Netlist graphic must have graphic_class GR_CLASS_ETCH

                                      // graphic_class on insert is if the (i.e) silkscreen drawing is a geometry
                                      // containing lines, arcs, etc.... In this case the graphic_class flag
                                      // must react also to this drawing elements.
   char m_hide:1;   

   short         m_layerIndex;
   DbFlag        m_flagBits;           // this is for dbutil flags.
   int           m_physLyrStart;       //span layer range
   int           m_physLyrEnd;

   union
   {
      CPolyList*    m_polylist;
      TextStruct*   m_text;
      InsertStruct* m_insert;
      PointStruct*  m_point;
      BlobStruct*   m_blob;
      CPoly3DList*  m_poly3dlist;
   };

   CAttributes* m_attributes; // must be on the end of structure for memcpy in ExplodeBlock

private:
   //DataStruct();
   //DataStruct(const DataStruct& other);
   DataStruct(const DataStruct& other,int entityNumber,bool copyAttributesFlag=true);
   DataStruct(const DataStruct& other);
   DataStruct(DataTypeTag dataType,int entityNumber);

public:
   ~DataStruct();
   DataStruct& operator=(const DataStruct& other);

private:
   void init();
   void deallocate();

public:

   // accessors
   long getEntityNumber() const;

   DataTypeTag getDataType() const;

   bool isSelected() const;
   void setSelected(bool flag);
   void setSelected(BOOL flag);

   bool isMarked() const;
   void setMarked(bool flag);
   void setMarked(BOOL flag);

   bool isNegative() const;
   void setNegative(bool flag);
   void setNegative(BOOL flag);

   bool isHidden() const;
   void setHidden(bool flag);
   void setHidden(BOOL flag);

   bool getColorOverride() const;
   void setColorOverride(bool flag);
   void setColorOverride(BOOL flag);

   COLORREF getOverrideColor() const;
   void setOverrideColor(COLORREF color);

   bool hasHighlightColor() const;
   COLORREF getHighlightColor() const;
   void setHighlightColor(COLORREF color);

   GraphicClassTag getGraphicClass() const;
   void setGraphicClass(GraphicClassTag graphicClass);
   void setGraphicClass(int graphicClass);             

   short getLayerIndex() const;
   void setLayerIndex(short layerIndex);

   DbFlag getFlags() const;
   void setFlags(DbFlag blockFlag);
   void setFlagBits(DbFlag mask);
   void clearFlagBits(DbFlag mask);

   CAttributes* getAttributes() const;
   CAttributes*& getAttributesRef();
   CAttributes*& getAttributeMap();
   CAttributes*& getDefinedAttributes();
   CAttributes*& getDefinedAttributeMap();
   CAttributes& attributes();

   CPolyList*&    getPolyList();
   TextStruct*&   getText();
   InsertStruct*& getInsert();
   PointStruct*&  getPoint();
   BlobStruct*&   getBlob();
   CPoly3DList*&  getPoly3DList();

   const CPolyList*    getPolyList() const;
   const TextStruct*   getText()     const;
   const InsertStruct* getInsert()   const;
   const PointStruct*  getPoint()    const;
   const BlobStruct*   getBlob()     const;
   const CPoly3DList*  getPoly3DList() const;

   // properties
   DbFlag getFlag() const;
   bool getMarked() const;
   bool getHidden() const;
   bool isInsert() const;
	bool isInsertType(InsertTypeTag tag) const;

   // operations
   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL,bool doAttributes=false);
   bool isVisible(CCamCadData& camCadData,LayerStruct* parentsLayer=NULL,bool mirrorLayerFlag=false,bool filterLayersFlag=false);
   bool isSelectable(CCamCadData& camCadData,LayerStruct* parentsLayer=NULL,bool mirrorLayerFlag=false,bool filterLayersFlag=true);
   bool setAttrib(CCamCadData& camCadData, int keywordIndex, ValueTypeTag valueType, void *value, AttributeUpdateMethodTag updateMethod, CAttribute** attribPtr);
   bool lookUpAttrib(WORD keyword, CAttribute *&attribute);
   void removeAttrib(WORD keyword);
   CString getProbeStyle(CCamCadData& camCadData);

   bool HasLayerSpan() const                       { return (m_physLyrStart != -1 && m_physLyrEnd != -1);} 
   int  GetPhysicalStartLayer()  const             { return m_physLyrStart; }
   int  GetPhysicalEndLayer()   const              { return m_physLyrEnd; }
   void  SetPhysicalStartLayer(int layer)          { m_physLyrStart = layer; }
   void  SetPhysicalEndLayer(int layer)            { m_physLyrEnd = layer; }

   bool isValid() const;
   void dump(CWriteFormat& writeFormat,int depth = -1) const;

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCamCadData& camCadData);
};  /* end of DataStruct */

//_____________________________________________________________________________
class CDataList : public CTypedPtrListContainer<DataStruct*>
{
public:
   CDataList(bool isContainer);
   CDataList(const CDataList& other,CCamCadData& camCadData);
   //CDataList& CDataList::operator=(const CDataList& other);
   ~CDataList();

private:
   CDataList(const CDataList& other);

public:
   void setEqual(const CDataList& other,CCamCadData& camCadData);
   //void copyData(const CDataList& other,CCamCadData& camCadData);
   void takeData(CDataList& other);
   CPoint2d getPinCentroid() const;
   CPoint2d getCentroid(InsertTypeTag insertType=insertTypeUndefined) const;
   CPoint2d getCentroid(const CInsertTypeMask& insertTypeMask) const;
   CExtent getPinExtent() const;
   CExtent getExtent(CCamCadData* camCadData=NULL) const;
   CExtent getExtent(const CTMatrix& matrix) const;
   CExtent getExtent(InsertTypeTag insertType) const;
   CBasesVector getPinMajorMinorAxes() const;

   // only considers pin origin for symmetry
   int getPinSymmetryCount(double searchTolerance=.001) const;

   // uses SymmetryCriteriaTag
   int getPinSymmetryCount(SymmetryCriteriaTag symmetryCriteria,double searchTolerance=.001) const;

   int getSymmetryCount(InsertTypeTag insertType=insertTypeUndefined,SymmetryCriteriaTag symmetryCriteria=symmetryCriteriaOrigin,double searchTolerance=.001) const;
   int getSymmetryCount(const CInsertTypeMask& insertTypeMask,SymmetryCriteriaTag symmetryCriteria,double searchTolerance) const;

   int getInsertCount(const CInsertTypeMask& insertTypeMask) const;
   DataStruct* findEntity(int entityNumber);

   DataStruct* FindInsertData(CString refname, InsertTypeTag insertType);

   void rotateList(int rotationCount);
   CDataList* getFlattenedDataList(CCamCadData& camCadData,int parentLayerIndex,bool flattenRegularComplexAperturesFlag=true) const;
   void transform(const CTMatrix& transformationMatrix,CCamCadData* camCadData=NULL,bool doAttributes=false);

   bool isValid() const;
   void dump(CWriteFormat& writeFormat,int depth = -1) const;

   //void RemoveDataFromList(CCamCadData& camCadData, DataStruct *data, POSITION dataPos = NULL);
   //void RemoveDataByGraphicClass(CCamCadData& camCadData, GraphicClassTag graphicClass);
   //void RemoveDataByInsertType(CCamCadData& camCadData, InsertTypeTag insertType);
   
   void sortByAscendingEntityNumber();
   bool isCompatibleWithRegularComplexAperture(CCamCadData& camCadData) const;

   //void WriteXML(CWriteFormat& writeFormat, CCamCadFileWriteProgress& progress, CCamCadData& camCadData);

private:
   void flattenInsert(CDataList& flatDataList,DataStruct& parentDataInsert,int parentLayer,CCamCadData& camCadData,
      const CTMatrix& parentMatrix,bool flattenRegularComplexAperturesFlag=true) const;
};

//_____________________________________________________________________________
class CMapEntityToDataStruct : public CTypedMapIntToPtrContainer<DataStruct*>
{
public:
   CMapEntityToDataStruct(int blockSize=10,bool isContainer=true);
   CMapEntityToDataStruct(const CDataList& dataList);
};

#endif
