// $Header: /CAMCAD/4.6/Fiducial.h 6     5/07/07 6:34p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// FIDUCIAL.H

#if ! defined (__Fiducial_h__)
#define __Fiducial_h__

#pragma once

#include "CCDoc.h"
#include "ResizingDialog.h"
#include "CamCadDatabase.h"

enum StandardApertureTypeTag;

//-----------------------------------------------------------------------------
// CAddFiducialDlg
//-----------------------------------------------------------------------------
class CAddFiducialDlg : public CResizingDialog
{
//	DECLARE_DYNAMIC(CAddFiducialDlg)

private:
   CCamCadDatabase m_camCadDatabase;
   CCEtoODBDoc& m_doc;
   FileStruct& m_pcbFile;
	CTypedMapStringToPtrContainer<BlockStruct*> m_fiducialGeomNameMap;
	int m_PadTopLayerIndex;
	int m_MaskTopLayerIndex;
	double m_xLocation;
	double m_yLocation;

   BOOL m_fiducialShape;
   BOOL m_FiducialSurface;
   CEdit m_size;
   CEdit m_xLocationEditbox;
   CEdit m_yLocationEditbox;

   bool validateAllNumbericFields(double& size, double& x, double& y);
	BlockStruct* createFiducialGeometry(StandardApertureTypeTag fiducialType, const double size);
	int getPadTopLayerIndex();
	int getMaskTopLayerIndex();
	int getLayerIndex(const LayerTypeTag layerType);

public:
	CAddFiducialDlg(CCEtoODBDoc& doc, FileStruct& pcbFile, const double x, const double y);
//	CAddFiducialDlg(CCEtoODBDoc& doc, FileStruct& pcbFile);
	virtual ~CAddFiducialDlg();
	enum { IDD = IDD_AddFiducial };
   virtual CString GetDialogProfileEntry() { return CString("AddFiducialDlg"); }

   static StandardApertureTypeTag intToApertureTypeTag(int apertureShapeIndex);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
   afx_msg void OnBnClickedAdd();
   afx_msg void OnBnClickedClose();
   afx_msg void OnClose();
};

#endif
