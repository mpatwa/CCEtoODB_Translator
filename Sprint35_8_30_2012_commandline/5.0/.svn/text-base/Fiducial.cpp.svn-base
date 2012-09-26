// $Header: /CAMCAD/5.0/Fiducial.cpp 12    5/21/07 3:47p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2003. All Rights Reserved.
*/

// FIDUCIAL.CPP

#include "stdafx.h"
#include "fiducial.h"
#include "Graph.h"
#include "RwLib.h"
#include ".\fiducial.h"
#include "StandardAperture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CDialogBar *CurrentSettings; // from MAINFRM.CPP

//-----------------------------------------------------------------------------
// CAddFiducialDlg
//-----------------------------------------------------------------------------
//IMPLEMENT_DYNAMIC(CAddFiducialDlg, CResizingDialog)
CAddFiducialDlg::CAddFiducialDlg(CCEtoODBDoc& doc, FileStruct& pcbFile, const double x, const double y)
: CResizingDialog(CAddFiducialDlg::IDD, NULL)
, m_camCadDatabase(doc)
, m_doc(doc)
, m_pcbFile(pcbFile)
, m_fiducialGeomNameMap(nextPrime2n(20), false)
, m_xLocation(x)
, m_yLocation(y)
, m_fiducialShape(0)
, m_FiducialSurface(FALSE)
{
   m_fiducialGeomNameMap.empty();
   m_PadTopLayerIndex = -1;
   m_MaskTopLayerIndex = -1;
}

CAddFiducialDlg::~CAddFiducialDlg()
{
   m_fiducialGeomNameMap.empty();
}

void CAddFiducialDlg::DoDataExchange(CDataExchange* pDX)
{
   CResizingDialog::DoDataExchange(pDX);
   DDX_Radio(pDX, IDC_RoundFiducial, m_fiducialShape);
   DDX_Radio(pDX, IDC_TopFiducial, m_FiducialSurface);
   DDX_Control(pDX, IDC_Size, m_size);
   DDX_Control(pDX, IDC_XLocation, m_xLocationEditbox);
   DDX_Control(pDX, IDC_YLocation, m_yLocationEditbox);
}


BEGIN_MESSAGE_MAP(CAddFiducialDlg, CResizingDialog)
   ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
   ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
   ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CAddFiducialDlg::OnInitDialog()
{
   CResizingDialog::OnInitDialog();

   CString xLocation, yLocation;
   xLocation.Format("%0.3f", m_xLocation);
   yLocation.Format("%0.3f", m_yLocation);
   m_xLocationEditbox.SetWindowText(xLocation);
   m_yLocationEditbox.SetWindowText(yLocation);

   m_size.SetFocus();

   return FALSE; // TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

bool CAddFiducialDlg::validateAllNumbericFields(double& size, double& x, double& y)
{
   size = 0.0;
   x = 0.0;
   y = 0.0;

   CMessageFilter messageFilterByMessage(messageFilterTypeMessage);

   CString tmpSize, tmpX, tmpY;
   m_size.GetWindowText(tmpSize);
   m_xLocationEditbox.GetWindowText(tmpX);
   m_yLocationEditbox.GetWindowText(tmpY);

   if (is_number(tmpSize) == 0)
   {
      messageFilterByMessage.formatMessageBoxApp("Please enter a numberic value for SIZE of fiducial.");
      m_size.SetSel(0, m_size.GetWindowTextLength());
      m_size.SetFocus();
      return false;
   }
   else if (is_number(tmpX) == 0)
   {
      messageFilterByMessage.formatMessageBoxApp("Please enter a numberic value for X-Location of fiducial.");
      m_xLocationEditbox.SetSel(0, m_xLocationEditbox.GetWindowTextLength());
      m_xLocationEditbox.SetFocus();
      return false;
   }
   else if (is_number(tmpY) == 0)
   {
      messageFilterByMessage.formatMessageBoxApp("Please enter a numberic value for Y-Location of fiducial.");
      m_yLocationEditbox.SetSel(0, m_yLocationEditbox.GetWindowTextLength());
      m_yLocationEditbox.SetFocus();
      return false;
   }

   size = atof(tmpSize);
   x = atof(tmpX);
   y = atof(tmpY);

   return true;
}

BlockStruct* CAddFiducialDlg::createFiducialGeometry(StandardApertureTypeTag fiducialType, const double size)
{
   BlockStruct* fiducialGeometry    = NULL;
   BlockStruct* fiducialPadGeometry = NULL;

   CStandardAperture fiducialAperture(fiducialType,m_doc.getPageUnits());

   if (size > 0.)
   {
      switch (fiducialType)
      {
      case standardApertureCircle:
      case standardApertureSquare:
         fiducialAperture.setDimensions(size);

         break;
      case standardApertureTabbedRectangle:
         fiducialAperture.setDimensions(size/4.,size/4.,size/4.,3.*size/8.);

         break;
      }

      fiducialPadGeometry = fiducialAperture.getDefinedAperture(m_doc.getCamCadData(),m_pcbFile.getFileNumber());

      if (fiducialPadGeometry != NULL)
      {
         CString fiducialGeometryName;
         fiducialGeometryName.Format("FidGeom_%s",fiducialPadGeometry->getName());
         fiducialGeometry = m_camCadDatabase.getBlock(fiducialGeometryName,m_pcbFile.getFileNumber());

         if (fiducialGeometry == NULL)
         {
            fiducialGeometry = m_camCadDatabase.getNewBlock(fiducialGeometryName,blockTypeFiducial,m_pcbFile.getFileNumber());

            int padTopLayerIndex = getPadTopLayerIndex();

            DataStruct* copperPad = m_camCadDatabase.insertBlock(fiducialPadGeometry,insertTypeUnknown,"",padTopLayerIndex);

            int maskTopLayerIndex = getMaskTopLayerIndex();

            DataStruct* maskPad = m_camCadDatabase.insertBlock(fiducialPadGeometry,insertTypeUnknown,"",maskTopLayerIndex);

            fiducialGeometry->getDataList().AddTail(copperPad);
            fiducialGeometry->getDataList().AddTail(maskPad);
         }
      }

      //CString fiducialGeomName;
      //fiducialGeomName.Format("FidGeom_%0.3f_%s", size,   apertureShapeToName(shape));
      //if (!m_fiducialGeomNameMap.Lookup(fiducialGeomName, fiducialBlock))
      //{
      //   CString apertureName;
      //   apertureName.Format("Aperture_%0.3f_%s", size, apertureShapeToName(shape));

      //   int err;
      //   int widthIndex = Graph_Aperture(apertureName, shape, size, 0.0, 0.0, 0.0, 0.0, 0, BL_APERTURE, FALSE, &err);
      //   BlockStruct* apertureBlock = m_doc.getWidthBlock(widthIndex);

      //   if (apertureBlock != NULL)
      //   {
      //      fiducialBlock = Graph_Block_On(GBO_APPEND, fiducialGeomName, -1, 0L, blockTypeFiducial);
      //      Graph_Block_Reference(apertureBlock->getName(), "", apertureBlock->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, getPadTopLayerIndex(), FALSE, apertureBlock->getBlockType()); 
      //      Graph_Block_Reference(apertureBlock->getName(), "", apertureBlock->getFileNumber(), 0.0, 0.0, 0.0, 0, 1, getMaskTopLayerIndex(), FALSE, apertureBlock->getBlockType()); 
      //      Graph_Block_Off();

      //      m_fiducialGeomNameMap.SetAt(fiducialGeomName, fiducialBlock);
      //   }
      //}
   }

   return fiducialGeometry;
}

int CAddFiducialDlg::getPadTopLayerIndex()
{
   if (m_PadTopLayerIndex == -1)
   {
      m_PadTopLayerIndex = getLayerIndex(layerTypePadTop);
   }

   return m_PadTopLayerIndex;
}

int CAddFiducialDlg::getMaskTopLayerIndex()
{
   if (m_MaskTopLayerIndex == -1)
   {
      m_MaskTopLayerIndex = getLayerIndex(layerTypeMaskTop);
   }

   return m_MaskTopLayerIndex;
}

int CAddFiducialDlg::getLayerIndex(const LayerTypeTag layerType)
{
   int retval = -1;
   for (int i=0; i<m_doc.getLayerCount(); i++)
   {
      // Make sure the layer is of the correct type and is mirrored normal and has a mirrored layer
      LayerStruct* layer = m_doc.getLayerAt(i);
      if (layer == NULL || layer->getLayerType() != layerType)
         continue;

      if (layer->getLayerFlags() != 0 || layer->getMirroredLayerIndex() == layer->getLayerIndex())
         continue;

      // Make sure the mirrored layer is of the correct type and is mirrored normal
      LayerStruct* mirroredLayer = m_doc.getLayerAt(layer->getMirroredLayerIndex());
      LayerTypeTag bottomLayerType = (layerType==layerTypePadTop)?layerTypePadBottom:layerTypeMaskBottom;
      if (mirroredLayer == NULL || mirroredLayer->getLayerType() != bottomLayerType)
         continue;

      if (mirroredLayer->getLayerFlags() != 0)
         continue;

      retval = layer->getLayerIndex();
      break;
   }

   if (retval < 0)
   {
      LayerStruct* layerTop = NULL;
      LayerStruct* layerBottom = NULL;
      if (layerType == layerTypeMaskTop)
      {
         layerTop = m_doc.AddNewLayer("FiducialMaskTop");
         layerTop->setLayerType(layerTypeMaskTop);

         layerBottom = m_doc.AddNewLayer("FiducialMaskBottom");
         layerBottom->setLayerType(layerTypeMaskBottom);

         layerTop->setMirroredLayerIndex(layerBottom->getLayerIndex());
         layerBottom->setMirroredLayerIndex(layerTop->getLayerIndex());
      }
      else
      {
         layerTop = m_doc.AddNewLayer("FiducialPadTop");
         layerTop->setLayerType(layerTypePadTop);
         layerTop->setElectricalStackNumber(1);

         layerBottom = m_doc.AddNewLayer("FiducialPadBottom");
         layerBottom->setLayerType(layerTypePadBottom);
         layerBottom->setElectricalStackNumber(m_doc.getMaxElectricalLayerNumber());
      }

      layerTop->setMirroredLayerIndex(layerBottom->getLayerIndex());
      layerBottom->setMirroredLayerIndex(layerTop->getLayerIndex());

      retval = layerTop->getLayerIndex();
   }

   return retval;
}

StandardApertureTypeTag CAddFiducialDlg::intToApertureTypeTag(int apertureShapeIndex)
{
   StandardApertureTypeTag apertureType = standardApertureUndefined;

   switch (apertureShapeIndex)
   {
   case 0:  apertureType = standardApertureCircle;           break;
   case 1:  apertureType = standardApertureSquare;           break;
   case 2:  apertureType = standardApertureTabbedRectangle;  break;
   }

   return apertureType;
}

void CAddFiducialDlg::OnBnClickedAdd()
{
   UpdateData(TRUE);

   double size, x, y;

   if (validateAllNumbericFields(size, x, y))
   {
      //int shape = m_fiducialShape==0?apertureRound:apertureSquare;
      StandardApertureTypeTag fiducialType = intToApertureTypeTag(m_fiducialShape);

      BlockStruct* fiducialBlock = createFiducialGeometry(fiducialType, size);

      if (fiducialBlock != NULL)
      {
         int count = 0;
         CString fiducialName;
         while (fiducialName.IsEmpty() || m_pcbFile.getBlock()->FindData(fiducialName) != NULL)
            fiducialName.Format("Fid_%d", ++count);

         Graph_Block_On(m_pcbFile.getBlock());
         DataStruct* data = Graph_Block_Reference(fiducialBlock->getName(), fiducialName, fiducialBlock->getFileNumber(), 
               x, y, 0.0, m_FiducialSurface==0?0:1, 1, 0, FALSE, fiducialBlock->getBlockType());
         data->getInsert()->setInsertType(insertTypeFiducial);
         Graph_Block_Off();

         m_doc.OnRedraw();
      }
   }
}

void CAddFiducialDlg::OnBnClickedClose()
{
   OnClose();
}

void CAddFiducialDlg::OnClose()
{
   CResizingDialog::OnClose();
   EndDialog(IDCANCEL);
}
