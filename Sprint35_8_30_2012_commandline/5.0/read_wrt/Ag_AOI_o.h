// $Header: /CAMCAD/5.0/read_wrt/Ag_AOI_o.h 31    6/21/07 8:28p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#pragma once

double getPlxUnitsFactor();

class CPolygon;

enum PlxTypeTag
{
   plxTypePaste,
   plxTypeFlux,
   plxTypeHole,
   plxTypeUndefined
};

typedef struct
{
   CString  pinname;
   int      pinnr;
   long     x ,y;
   int      rot;
   CString  padstackname;
} PLX_Terminal;
typedef CTypedPtrArray<CPtrArray, PLX_Terminal*> TerminalArray;

typedef struct
{
   int            block_num;
   CString        name;
   double         drill;
   int            typ;        // 0x1 top
                              // 0x2 bottom
                              // 0x4 drill
   int            shapetypetop;
   int            shapetypebot;
   long           xsizetop, ysizetop, xofftop, yofftop;
   long           xsizebot, ysizebot, xoffbot, yoffbot;
} PLXPadstack;
typedef CTypedPtrArray<CPtrArray, PLXPadstack*> PLXPadstackArray;

class CPlxCompPin
{
private:
   static int m_stencilBaseThicknessKeywordIndex;
   static int m_stencilThicknessKeywordIndex;
   static CMapStringToString m_checkedGeometryNameMap;
   static bool m_exportPartGeometryInformationFlag;
   static bool m_useZeroRotationForComplexAperturesFlag;

   //long     m_x;
   //long     m_y;
   CBasesVector m_basesVector;
   CTMatrix m_BoardTransformMatrix;
   mutable CBasesVector m_normalizedBasesVector;
   mutable CExtent m_normalizedExtent;
   mutable CPoint2d m_biasOffset;

   //int      m_rotationDegrees;
   FileStruct& m_pcbFile;
   CString m_geometryName;
   CString m_shapeGeometryName;
   DataStruct* m_component;
   BlockStruct* m_geometry;
   PlxTypeTag m_type;
   CString  m_partNumber;
   CString  m_componentType;
   bool m_bottomFlag;

   mutable double m_fillFactor;
   mutable double m_stencilBaseThickness;
   mutable CPolygon* m_complexPolygon;

public:
   CString  refdes;        // list of components in the datalist
   int      bnum;
   CString  placement_machine;
   //int      bottom;
   int      boardnumber;   // on panels, board start with 1, on single it is -1
   int      test;          // true or false
   int      smd;
   int      written;       // was this component written out   1 = top, 2 = bottom
   bool loaded;

public:
   CPlxCompPin(FileStruct& pcbFile, AoiMachineTag mchTag);
   ~CPlxCompPin();

   PageUnitsTag getPageUnits() const { return m_pcbFile.getCamCadData().getPageUnits(); }

   long getPlxX() const { return round(m_basesVector.getOrigin().x * getPlxUnitsFactor()); }
   long getPlxY() const { return round(m_basesVector.getOrigin().y * getPlxUnitsFactor()); }

   int getRotationDegrees() const { return round(m_basesVector.getRotationDegrees()); }
   //void setRotationDegrees(int degrees);

   int getNormalizedRotationDegrees() const;

   const CBasesVector& getBasesVector();
   void setBasesVector(const CBasesVector& basesVector);
   void setBasesVectorPlxUnits(const CBasesVector& basesVector);
   void setBasesVectorPlxUnits(double xOrigin=0.,double yOrigin=0.,double rotationInDegrees=0.,bool mirror=false,double scale=1.);
   
   void setBoardTransformMatrix(const CTMatrix&  boardTransformMatrix) {  m_BoardTransformMatrix = boardTransformMatrix;}
   const CTMatrix & getBoardTransformMatrix() { return m_BoardTransformMatrix; }
   bool getBottomFlag() const { return m_bottomFlag; }
   void setBottomFlag(bool flag) { m_bottomFlag = flag; }

   CString getShapeGeometryName() const;
   void    setShapeGeometryName(const CString& name);

   CString getGeometryName() const;
   void    setGeometryName(const CString& name);
   CString getCheckedGeometryName() const;

   CExtent getNormalizedExtent() const;

   BlockStruct* getGeometry() const          { return m_geometry; }
   void setGeometry(BlockStruct* geometry)   { m_geometry = geometry;  invalidate(); }

   DataStruct* getComponent() const          { return m_component; }
   void setComponent(DataStruct* component)   { m_component = component; }

   FileStruct& getPcbFile() const       { return m_pcbFile; }
   //void setPcbFile(FileStruct& pcbFile) { m_pcbFile = pcbFile; }

   PlxTypeTag getType() const          { return m_type; }
   void       setType(PlxTypeTag type) { m_type = type;  invalidate(); }

   CString getPartNumber() const { return m_partNumber; }
   void    setPartNumber(const CString& partNumber="");
   CString getPartNumberDescriptor() const;

   CString getComponentType() const { return m_componentType; }
   void    setComponentType(const CString& componentType="");
   CString getComponentTypeDescriptor() const;

   CPolygon getComplexPolygon() const;
   double getFillFactor() const;
   int getDepositHeightInMicrons() const;
   double getBaseThickness();
   CString getPlxShapeCode(bool bottomFlag) const;
   CString getDescriptor() const;
   CPoint2d getBoundingBoxCenter() const;
   CPoint2d getOrigin() const;
   double getStencilBaseThickness() const;

   static int getStencilBaseThicknessKeywordIndex();
   static int getStencilThicknessKeywordIndex();
   static void resetKeywordIndexes();
   static void releaseResources();

   static int getExportPartGeometryInformationFlag() { return m_exportPartGeometryInformationFlag; }
   static void setExportPartGeometryInformationFlag(bool flag) { m_exportPartGeometryInformationFlag = flag; }

   static int getUseZeroRotationForComplexAperturesFlag() { return m_useZeroRotationForComplexAperturesFlag; }
   static void setUseZeroRotationForComplexAperturesFlag(bool flag) { m_useZeroRotationForComplexAperturesFlag = flag; }
   
   static CString getCheckedGeometryName(const CString& geometryName);

private:
	AoiMachineTag m_mchTag;
	void determineFillFactor(double shapeArea, double extentArea) const;
   void invalidate() const;
   void validate() const;

};

class CGeometryFillFactorMap : public CTypedMapPtrToPtrContainer<BlockStruct*,double*>
{
};

typedef CTypedPtrArray<CPtrArray, CPlxCompPin*> PLXCompArray;

typedef struct
{
   CString  partnumber;
   int      bnum;
   int      rotation;
   int      written;       // was this partnumber written out  1 = top, 2 = bottom
} PLXPart;
typedef CTypedPtrArray<CPtrArray, PLXPart*> PLXPartArray;

typedef struct
{
   long     x, y;
   CString  geomname;
	CString	compname;
   int      layer;      // 1 top, 2 bottom 3 both
   int boardNumber;
   bool boardFiducial;
} PLXFiducial;
typedef CTypedPtrArray<CPtrArray, PLXFiducial*> PLXFiducialArray;

typedef struct
{
   long x, y;
   CString refname;
} PLXRejectMark;
typedef CTypedPtrArray<CPtrArray, PLXRejectMark*> PLXRejectMarkArray;

typedef struct
{
   CString  name;       // list of pcb blocks in the datalist
   long     centroid_x, centroid_y;
   double   centroid_rotation;
} PLXGeom;
typedef CTypedPtrArray<CPtrArray, PLXGeom*> PLXGeomArray;

