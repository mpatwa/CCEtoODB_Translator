// $Header: /CAMCAD/5.0/GerberThermalDfm.h 26    6/30/07 2:29a Kurt Van Ness $

#if ! defined (__GerberThermalDfm_h__)
#define __GerberThermalDfm_h__

#pragma once

//#include "FlexGrid.h"
#include "Resource.h"
#include "afxwin.h"
#include "Data.h"
#include "QfeLib.h"
#include "File.h"
#include "TypedContainer.h"
#include "FlexGridLib.h"
#include "DcaQfeLib.h"
#include "DcaFile.h"
#include <afxctl.h>

#define ThermalPadMask     0x1
#define NonThermalPadMask  0x2
#define BothPadsMask  (ThermalPadMask | NonThermalPadMask)

typedef long GRID_CNT_TYPE;

class CGerberThermalPropertySheet;
class CCEtoODBDoc;
class LayerStruct;
class BlockStruct;

//_____________________________________________________________________________
class CGerberThermalLayer : public CObject
{
private:
   int m_layerNumber;
   int m_rowIndex;
   int m_thermalMask;
   bool m_thermalLayerFlag;
   LayerStruct* m_layerStruct;

public:
   CGerberThermalLayer(int layerNumber);

   int getThermalMask() { return m_thermalMask; }
   void setThermalMask(int mask) { m_thermalMask = mask; }

   int getRowIndex() { return m_rowIndex; }
   void setRowIndex(int rowIndex) { m_rowIndex = rowIndex; }

   void setLayerStruct(LayerStruct* layerStruct) { m_layerStruct = layerStruct; }

   bool getThermalLayerFlag() { return m_thermalLayerFlag; }
   void setThermalLayerFlag(bool flag) { m_thermalLayerFlag = flag; }

};

//_____________________________________________________________________________
class CGerberThermalLayerArray : public CObject
{
private:
   CTypedObArrayContainer<CGerberThermalLayer*> m_layers;
   CTypedObArrayContainer<CGerberThermalLayer*> m_rows;

public:
   CGerberThermalLayerArray();
   virtual ~CGerberThermalLayerArray();
   void empty();

   CGerberThermalLayer& getAt(int layerNumber);
   CGerberThermalLayer* getAtRowIndex(int rowIndex);
   void setRowIndex(int layerNumber,int rowIndex);
   int getSize();
};

//_____________________________________________________________________________
enum connectionTag
{
   undefinedConnection                 = 0x00,
   solidConnection                     = 0x01,
   clearanceViolationConnection        = 0x02,
   thermalClearanceViolationConnection = 0x04,
   thermalConnection                   = 0x08,
   noConnection                        = 0x10,
   unknownConnection                   = 0x20
};

//_____________________________________________________________________________
class CConnectionEvaluation : public CObject
{
private:
   connectionTag m_connection;
   BlockStruct* m_aperture;
   double m_clearance;

public:
   CConnectionEvaluation(connectionTag connection,BlockStruct* aperture,double clearance);

   void writeConnectionEvaluationReport(CWriteFormat& writeFormat);
};

//_____________________________________________________________________________
class CConnectionEvaluationList : public CTypedPtrListContainer<CConnectionEvaluation*>
{
};

//_____________________________________________________________________________
class CConnectionEvaluationListArray : public CTypedPtrArrayContainer<CConnectionEvaluationList*>
{
};

//_____________________________________________________________________________
class CQfePin : public CQfe
{
private:
   CString m_refDes;
   CString m_pinName;
   CPoint2d m_origin;
   double m_finishedHoleSize;
   CByteArray m_connectionFlags;
   CConnectionEvaluationListArray m_evaluations;

public:
   CQfePin(const CString& refDes,const CString& pinName,const CPoint2d& origin,
      double finishedHoleSize);
   ~CQfePin();

public:
   CPoint2d getOrigin() const { return m_origin; }
   CExtent getExtent() const;
   CString getRefDes() const { return m_refDes; }
   CString getPinName() const { return m_pinName; }
   CString getPinReference() const { return m_refDes + "." + m_pinName; }
   double getFinishedHoleSize() const { return m_finishedHoleSize; }
   CString getGerberPadClearanceAttributeValue(CGerberThermalLayerArray& gerberThermalLayerArray) const;
   int evaluateConnection(double dx,double dy,int layerNumber,
      BlockStruct* apertureBlock,double clearanceTolerance);
   void writeConnectionEvaluationReport(CWriteFormat& writeFormat);

   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;
   virtual CString getInfoString() const;

private:
   void addConnectionEvaluation(int layerIndex,connectionTag connection,
      BlockStruct* aperture,double clearance);
};

//_____________________________________________________________________________
typedef CTypedObMapWithArrayContainer<CQfePin> CQfePinMap;

//_____________________________________________________________________________
class CQfePinList : public CTypedPtrList<CQfeList,CQfePin*>
{
private:
   bool m_isContainer;

public:
   CQfePinList(bool isContainer=false,int nBlockSize=200);
   ~CQfePinList();
   void empty();
};

//_____________________________________________________________________________
class CQfePinTree : public CQfeExtentLimitedContainer
{
private:

public:
   CQfePinTree();
   ~CQfePinTree();

public:
   virtual int search(const CExtent& extent,CQfePinList& foundList)
      { return CQfeExtentLimitedContainer::search(extent,foundList); }

   virtual CQfePin* findFirst(const CExtent& extent)
      { return (CQfePin*)CQfeExtentLimitedContainer::findFirst(extent); }

   virtual CQfePin* findFirstViolation(const CExtent& extent,CQfe& qfe)
      { return (CQfePin*)CQfeExtentLimitedContainer::findFirstViolation(extent,qfe); }

   virtual CQfePin* findFirstEdgeToRectViolation(const CExtent& extent,int& entitiesChecked)
      { return (CQfePin*)CQfeExtentLimitedContainer::findFirstEdgeToRectViolation(extent,entitiesChecked); }

   virtual CQfePin* findFirstEdgeToPointViolation(const CExtent& extent,
      const CPoint2d& point,double distance,int& entitiesChecked)
      { return (CQfePin*)CQfeExtentLimitedContainer::findFirstEdgeToPointViolation(extent,point,
                                                        distance,entitiesChecked); }

   //virtual int findAllViolations(const CExtent& extent,CQfe& qfe,CQfeList& foundList);

   virtual void setAt(CQfePin* qfe)
      { CQfeExtentLimitedContainer::setAt(qfe); }
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage : public CPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage)

private:
   CExtendedFlexGrid* m_flexGrid;

public:
   CGerberThermalDfmPropertyPage(UINT idTemplate);
   virtual ~CGerberThermalDfmPropertyPage();

protected:
   CGerberThermalPropertySheet& getParent() { return *((CGerberThermalPropertySheet*)GetParent()); }
   //virtual CExtendedFlexGrid& getFlexGrid();

};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage1 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage1)

private:
   CStatic m_wizardDescription;

public:
   CGerberThermalDfmPropertyPage1();
   virtual ~CGerberThermalDfmPropertyPage1();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm1 };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnSetActive();
   virtual BOOL OnInitDialog();
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage2 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage2)

private:
   const static int m_colGerberLayerName = 0;
   const static int m_colPlaneLayer      = 1;
   CExtendedFlexGrid m_flexGridControl;
   double m_clearanceTolerance;
   FileStruct* m_ecadFile;
   CGerberThermalLayerArray m_gerberThermalLayerArray;

public:
   CGerberThermalDfmPropertyPage2();
   virtual ~CGerberThermalDfmPropertyPage2();

private:
   void initGrid();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm2 };

   double getClearanceTolerance();

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CExtendedFlexGrid& getFlexGrid() { return m_flexGridControl; }

   DECLARE_MESSAGE_MAP()

public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();

   DECLARE_EVENTSINK_MAP()
   void AfterEditVsflexgrid(long Row, long Col);
   afx_msg void OnBnClickedScanPlanesButton();
   virtual BOOL OnKillActive();
   afx_msg void OnBnClickedInsertDrcsButton();
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage3 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage3)

private:
   CListBox m_summaryListBox;

public:
   CGerberThermalDfmPropertyPage3();
   virtual ~CGerberThermalDfmPropertyPage3();

private:
   void setGerberPadClearanceAttributes();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm3 };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
private:
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage4 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage4)

private:
   const static int m_colDcode    = 0;
   const static int m_colAperture = 1;
   const static int m_colThermal  = 2;

   CExtendedFlexGrid m_flexGridControl;

public:
   CGerberThermalDfmPropertyPage4();
   virtual ~CGerberThermalDfmPropertyPage4();

   void loadTestData();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm4 };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CExtendedFlexGrid& getFlexGrid() { return m_flexGridControl; }

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage5 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage5)

private:
   const static int m_colSignal = 0;
   const static int m_colPower  = 1;

   CExtendedFlexGrid m_flexGridControl;

public:
   CGerberThermalDfmPropertyPage5();
   virtual ~CGerberThermalDfmPropertyPage5();

   void loadTestData();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm5 };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CExtendedFlexGrid& getFlexGrid() { return m_flexGridControl; }

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
};

//_____________________________________________________________________________
class CGerberThermalDfmPropertyPage6 : public CGerberThermalDfmPropertyPage
{
   DECLARE_DYNAMIC(CGerberThermalDfmPropertyPage6)

private:
   const static int m_colGerber = 0;
   const static int m_colEcad   = 1;
   const static int m_colSignal = 2;
   const static int m_colAccept = 3;
   const static int m_colReject = 4;

   CExtendedFlexGrid m_flexGridControl;

public:
   CGerberThermalDfmPropertyPage6();
   virtual ~CGerberThermalDfmPropertyPage6();

   void loadTestData();

public:
// Dialog Data
   enum { IDD = IDD_GerberThermalDfm6 };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual CExtendedFlexGrid& getFlexGrid() { return m_flexGridControl; }

   DECLARE_MESSAGE_MAP()
public:
   virtual BOOL OnInitDialog();
   virtual BOOL OnSetActive();
};

//_____________________________________________________________________________
class CSimpleFileList : public CTypedPtrList<CPtrList,FileStruct*>
{
};

//_____________________________________________________________________________
class CGerberThermalPropertySheet : public CPropertySheet
{
private:
   CCEtoODBDoc& m_camCadDoc;
   CString m_caption;

   CGerberThermalDfmPropertyPage1 m_page1;
   CGerberThermalDfmPropertyPage2 m_page2;
   CGerberThermalDfmPropertyPage3 m_page3;
   //CGerberThermalDfmPropertyPage4 m_page4;
   //CGerberThermalDfmPropertyPage5 m_page5;
   //CGerberThermalDfmPropertyPage6 m_page6;

   CStringArray m_layerNames;
   CStringArray m_ecadLayerNames;
   CStringArray m_gerberLayerName;
   CGerberThermalLayerArray m_gerberThermalLayerArray;
   FileStruct* m_ecadFile;
   CSimpleFileList m_gerberList;

public:
   CGerberThermalPropertySheet(CCEtoODBDoc& camCadDoc);
   virtual ~CGerberThermalPropertySheet();

   CStringArray& getLayerNames() { return m_layerNames; }
   CStringArray& getEcadLayerNames() { return m_ecadLayerNames; }
   CStringArray& getGerberLayerNames() { return m_gerberLayerName; }
   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   bool getTestDataFlag() { return true; }
   double getClearanceTolerance() { return m_page2.getClearanceTolerance(); }
   CGerberThermalLayerArray& getGerberThermalLayerArray() { return m_gerberThermalLayerArray; }
   CSimpleFileList& getGerberList() { return m_gerberList; }

   FileStruct* getEcadFile() { return m_ecadFile; }
   void setEcadFile(FileStruct* file) { m_ecadFile = file; }

   void setCaption(const CString& caption);
   void addLayer(const CString& layerName);
   void addEcadLayer(const CString& layerName);
   void addGerberLayer(const CString& layerName);
   //void loadTestData();
   //void load(CCEtoODBDoc& camCadDoc);

   virtual BOOL OnInitDialog();
};

#endif
