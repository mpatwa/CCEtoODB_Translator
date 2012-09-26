// $Header: /CAMCAD/5.0/Gerb_Mrg.h 18    3/12/07 12:40p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#if !defined(__Gerb_Mrg_h__)
#define __Gerb_Mrg_h__

#pragma once

#include "extents.h"
#include "qfelib.h"

//_____________________________________________________________________________
class CGerberFlash : public CQfe
{
private:
   CPoint2d insPoint;
   DataStruct *flash;
   CExtent m_extent;

public:
   //CGerberFlash();
   CGerberFlash(double x, double y, DataStruct *data,const CExtent& extent);
   //CGerberFlash(CPoint2d point, DataStruct *data);
   ~CGerberFlash();

public:
   virtual CPoint2d getOrigin() const;
   virtual CExtent getExtent() const;
   virtual CString getInfoString() const;

   virtual bool isExtentIntersecting(const CExtent& extent);
   virtual bool isPointWithinDistance(const CPoint2d& point,double distance);
   virtual bool isInViolation(CObject2d& otherObject);
   virtual int getObjectType() const;

   DataStruct *getFlash();
};

//_____________________________________________________________________________
class CFlashList : public CTypedPtrList<CQfeList,CGerberFlash*>
{
public:
   CFlashList(int nBlockSize=200) : CTypedPtrList<CQfeList,CGerberFlash*>(nBlockSize) { };
};

class CQfeFlashTree : public CQfeGraduatedExtentLimitedContainer
{
public:
   CQfeFlashTree(double granularity,double maxFeatureSize) : CQfeGraduatedExtentLimitedContainer(granularity,maxFeatureSize) {};
   ~CQfeFlashTree() {};

public:
   //virtual int search(const CExtent& extent,CFlashList& foundList)
   //   { return CQfeGraduatedExtentLimitedContainer::search(extent,foundList); }
   //virtual void setAt(CGerberFlash* flash)
   //   { CQfeGraduatedExtentLimitedContainer::setAt(flash); }
};

//_____________________________________________________________________________
//#define Debug_CInheritEcadDialog

class CInheritEcadDialog : public CDialog
{
   DECLARE_DYNAMIC(CInheritEcadDialog)

private:
   CCEtoODBDoc& m_camCadDoc;
   FileStruct* m_gerberFile;
   FileStruct* m_cadFile;
   LayerStruct* m_topPasteLayer;
   LayerStruct* m_bottomPasteLayer;

   CString m_boardName;
   CString m_compName;
   bool m_placedBottom;
   int m_smdKw;
   int m_boardKw;
   int m_geometryNameKw;
   int m_compKw;
   int m_pinKw;

   long m_progressPos;

   long ecadPinCount(BlockStruct *block);
   int fillGerberTree(CQfeFlashTree *flashTree);
   void countGerberFlashes(int &inherited, int &uninherited);
   void mergeGerberEcad(BlockStruct *block,const CString& refname,CQfeFlashTree *flashTree,CTMatrix& matrix, int passNumber);
	int getPadLayer(DataStruct* data,bool mirrorFlag);
   bool isPasteFlash(DataStruct& flashData);

public:
   CInheritEcadDialog(CCEtoODBDoc& camCadDoc,CWnd* pParent = NULL);   // standard constructor
   virtual ~CInheritEcadDialog();

   CCEtoODBDoc& getCamCadDoc() { return m_camCadDoc; }
   CCamCadData& getCamCadData() { return m_camCadDoc.getCamCadData(); }
   BOOL GetGerberEcadFiles();

// Dialog Data
   enum { IDD = IDD_INHERIT_ECAD_DATA };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedHighlightFlashesWithoutAttributes();
   afx_msg void OnBnClickedHighlightFlashesWithAttributes();
   afx_msg void OnBnClickedClearHighlights();

protected:
   virtual void OnOK();
   virtual void OnCancel();

private:
#if defined(Debug_CInheritEcadDialog)
   FileStruct* m_debugFileStruct;
   LayerStruct* m_debugExtentLayer;
   LayerStruct* m_debugSearchExtentLayer;
   LayerStruct* m_debugPinLayer;

   FileStruct* getDebugFileStruct();
   LayerStruct* getDebugExtentLayer();
   LayerStruct* getDebugSearchExtentLayer();
   LayerStruct* getDebugPinLayer();

   void addDebugExtent(const CString& comment,const CExtent& extent,const CTMatrix& matrix,LayerStruct* layer=NULL);
   void addDebugSearchExtent(const CString& comment,const CExtent& extent,const CTMatrix& matrix);
   void addDebugPin(const CString& pinRef,const CPoint2d& origin);
#endif
};

#endif
