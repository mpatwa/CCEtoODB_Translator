// $Header: /CAMCAD/4.6/Pcb_Grid.cpp 26    12/07/06 12:07p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "ccdoc.h"
#include "pcb_grid.h"
#include "graph.h"
#include "extents.h"
#include "pcbutil.h"
#include "float.h"
#include "ManufacturingGrid.h"
#include "CCEtoODB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CString unitsString(int units);
CString formatUnits(double inches,int units);

/******************************************************************************
* calcExtents
* calculates the board outline extents of the first visible pcb type file in the file list
*/
//bool CCEtoODBDoc::calcExtents(ExtentRect& extents,FileStruct*& pcbFile) 
//{
//   bool retval = false;
//
//   extents.left = extents.bottom = DBL_MAX;
//   extents.right = extents.top = -DBL_MAX;
//
//   pcbFile = NULL;
//   
//   for (POSITION pos = FileList.GetHeadPosition();pos != NULL;)
//   {
//      FileStruct *file = FileList.GetNext(pos);
//
//      if (file->getBlockType() == BLOCKTYPE_PCB && file->isShown())
//      {
//         pcbFile = file;
//         break;
//      }
//   }
//
//   if (pcbFile != NULL)
//   {      
//      for (POSITION pos = pcbFile->getBlock()->getDataList().GetHeadPosition();pos != NULL;)
//      {
//         DataStruct *data = pcbFile->getBlock()->getDataList().GetNext(pos);
//
//         if (data->getDataType() == T_POLY && data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
//         {
//            Mat2x2 m;
//            RotMat2(&m, 0);
//            ExtentRect tempExtents;
//            PolyExtents(this, data->getPolyList(), &tempExtents, 1, 0, 0, 0, &m, TRUE);
//
//            if (tempExtents.left   < extents.left  ) extents.left   = tempExtents.left;
//            if (tempExtents.right  > extents.right ) extents.right  = tempExtents.right;
//            if (tempExtents.bottom < extents.bottom) extents.bottom = tempExtents.bottom;
//            if (tempExtents.top    > extents.top   ) extents.top    = tempExtents.top;
//         }
//      }
//
//      retval = true;
//   }
//
//   return retval;
//}

/******************************************************************************
* calcFileOutlineExtents
* calculates the extents of the board or panel outlines in the file.
*/
CExtent CCEtoODBDoc::calcFileOutlineExtents(FileStruct* pcbFile) 
{
   CExtent extent;

   if (pcbFile != NULL)
   {    
      BlockStruct* pcbBlock = pcbFile->getBlock();

      for (POSITION pos=pcbBlock->getHeadDataPosition(); pos != NULL;)
      {
         DataStruct *data = pcbBlock->getNextData(pos);

         if (data->getDataType() == dataTypePoly && 
             ( (data->getGraphicClass() == graphicClassBoardOutline) ||
               (data->getGraphicClass() == graphicClassPanelOutline)    ) )
         {
            data->getPolyList()->updateExtent(extent);
         }
      }
   }

   return extent;
}

/******************************************************************************
* OnManufacturingGrid
*/
void CCEtoODBDoc::OnManufacturingGrid() 
{
   CExtent extents;
   FileStruct* pcbFile = NULL;
   
   for (POSITION pos=getFileList().GetHeadPosition(); pos != NULL;)
   {
      FileStruct *file = getFileList().GetNext(pos);

      if (file->getBlockType() == BLOCKTYPE_PCB && file->isShown())
      {
         pcbFile = file;
         extents = calcFileOutlineExtents(pcbFile);
         break;
      }
   }

   if (pcbFile == NULL)
   {
      ErrorMessage("No PCB File is Visible", "Manufacturing Grid");
      return;
   }

   if (!extents.isValid())
   {
      ErrorMessage("Using Page Size for Manufacturing Grid.\nCancel and set Primary Board Outline is recommended.", "No Primary Board Outline Found");
      extents.update(getSettings().getXmin(),getSettings().getYmin());
      extents.update(getSettings().getXmax(),getSettings().getYmax());
   }

   PCB_Grid dlg;
   dlg.doc = this;
   dlg.pcbFile = pcbFile;
   dlg.m_board.Format("Board Name : %s", pcbFile->getName());
   int decimals = GetDecimals(getSettings().getPageUnits());
   dlg.left   = extents.getXmin();
   dlg.right  = extents.getXmax();
   dlg.bottom = extents.getYmin();
   dlg.top    = extents.getYmax();
   dlg.sizeX = dlg.right - dlg.left;
   dlg.sizeY = dlg.top - dlg.bottom;
   dlg.m_size.Format("Board Size: Width=%.*lg, Height=%.*lg",
      decimals, dlg.sizeX * Units_Factor(UNIT_INCHES,getSettings().getPageUnits()), 
      decimals, dlg.sizeY * Units_Factor(UNIT_INCHES,getSettings().getPageUnits()) );

   dlg.DoModal(); 
}

/////////////////////////////////////////////////////////////////////////////
// PCB_Grid dialog
PCB_Grid::PCB_Grid(CWnd* pParent /*=NULL*/)
   : CDialog(PCB_Grid::IDD, pParent)
{
   //{{AFX_DATA_INIT(PCB_Grid)
   m_board = _T("");
   m_size = _T("");
   m_hSizeString = _T("1.0");
   m_horz = 0;
   m_vSizeString = _T("1.0");
   m_vert = 0;
   m_hSteps = 9;
   m_vSteps = 9;
   m_gridThicknessString = _T("0.00");
   //}}AFX_DATA_INIT

   m_pageUnits = 0;
}

double PCB_Grid::getHSizePageUnits()
{
   return m_hSizePageUnits;
}

double PCB_Grid::getVSizePageUnits()
{
   return m_vSizePageUnits;
}

void PCB_Grid::setHSizePageUnits(double pageUnits)
{
   m_hSizePageUnits = pageUnits;
}

void PCB_Grid::setVSizePageUnits(double pageUnits)
{
   m_vSizePageUnits = pageUnits;
}

CString PCB_Grid::getHSizePageUnitsString()
{
   return formatUnits(m_hSizePageUnits,m_pageUnits);
}

CString PCB_Grid::getVSizePageUnitsString()
{
   return formatUnits(m_vSizePageUnits,m_pageUnits);
}

void PCB_Grid::rectifyValues()
{
   const int maxHSteps = 999;
   const int maxVSteps = 26*26*26;

   double hSize = getHSizePageUnits();
   double vSize = getVSizePageUnits();
   double dhPageUnits = (right - left);
   double dvPageUnits = (top - bottom);

   if (m_horz == 1)
   {  // size
      if (hSize == 0.)
      {
         m_hSteps = 10;
      }
      else
      {
         m_hSteps = (int)((dhPageUnits + hSize) / hSize);
      }
   }

   if (m_vert == 1)
   {  // size
      if (vSize == 0.)
      {
         m_vSteps = 10;
      }
      else
      {
         m_vSteps = (int)((dvPageUnits + vSize) / vSize);
      }
   }

   if      (m_hSteps <         1) m_hSteps = 1;
   else if (m_hSteps > maxHSteps) m_hSteps = maxHSteps;

   if      (m_vSteps <         1) m_vSteps = 1;
   else if (m_vSteps > maxVSteps) m_vSteps = maxVSteps;

   if (m_horz != 1)
   {  // steps
      hSize = dhPageUnits / m_hSteps;
   }

   if (m_vert != 1)
   {  // steps
      vSize = dvPageUnits / m_vSteps;
   }

   setHSizePageUnits(hSize);
   setVSizePageUnits(vSize);
}

void PCB_Grid::DoDataExchange(CDataExchange* pDX)
{
   if (!pDX->m_bSaveAndValidate)
   {
      m_hSizeString = getHSizePageUnitsString();
      m_vSizeString = getVSizePageUnitsString();
      m_gridThicknessString = formatUnits(m_gridThickness,m_pageUnits);
   }

   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PCB_Grid)
   DDX_Text(pDX, IDC_BOARD_NAME, m_board);
   DDX_Text(pDX, IDC_BOARD_SIZE, m_size);
   DDX_Text(pDX, IDC_H_SIZE, m_hSizeString);
   DDX_Radio(pDX, IDC_HORZ, m_horz);
   DDX_Text(pDX, IDC_V_SIZE, m_vSizeString);
   DDX_Radio(pDX, IDC_VERT, m_vert);
   DDX_Text(pDX, IDC_H_STEPS, m_hSteps);
   DDV_MinMaxInt(pDX, m_hSteps, 1, 26*26*26 - 1);
   DDX_Text(pDX, IDC_V_STEPS, m_vSteps);
   DDV_MinMaxInt(pDX, m_vSteps, 1, 999);
   DDX_Text(pDX, IDC_GRID_THICKNESS, m_gridThicknessString);
   //}}AFX_DATA_MAP

   if (pDX->m_bSaveAndValidate)
   {
      setHSizePageUnits(atof(m_hSizeString));
      setVSizePageUnits(atof(m_vSizeString));
      m_gridThickness = atof(m_gridThicknessString);
   }
}

BEGIN_MESSAGE_MAP(PCB_Grid, CDialog)
   //{{AFX_MSG_MAP(PCB_Grid)
   ON_BN_CLICKED(IDC_SAVE, OnSave)
   ON_BN_CLICKED(IDC_LOAD, OnLoad)
   //}}AFX_MSG_MAP
ON_BN_CLICKED(IDC_HStepsButton, OnBnClickedHStepsButton)
ON_BN_CLICKED(IDC_VStepsButton, OnBnClickedVStepsButton)
ON_BN_CLICKED(IDC_HSizeButton, OnBnClickedHSizeButton)
ON_BN_CLICKED(IDC_VSizeButton, OnBnClickedVSizeButton)
ON_EN_KILLFOCUS(IDC_H_STEPS, OnEnKillfocusHSteps)
ON_EN_KILLFOCUS(IDC_V_STEPS, OnEnKillfocusVSteps)
ON_EN_KILLFOCUS(IDC_H_SIZE, OnEnKillfocusHSize)
ON_EN_KILLFOCUS(IDC_V_SIZE, OnEnKillfocusVSize)
END_MESSAGE_MAP()


BOOL PCB_Grid::OnInitDialog() 
{
   CDialog::OnInitDialog();

   m_pageUnits = doc->getSettings().getPageUnits();
   setHSizePageUnits(DBL_MIN);
   setVSizePageUnits(DBL_MIN);
   m_gridThickness = 0.;
   
   CString settingsFile = getApp().getUserPath() + "default.grd";
   LoadGridSettings(settingsFile, FALSE);

   double defaultSizePageUnits = 1.0;

   // use 1 centimeter for metric units, 1 inch for other units
   switch (m_pageUnits)
   {
   case UNIT_MM: 
   case UNIT_TNMETER: 
      defaultSizePageUnits = 10. * Units_Factor(UNIT_MM,m_pageUnits);   
      break;
   case UNIT_INCHES:     
   case UNIT_MILS:       
   case UNIT_HPPLOTTER:  
   default:
      defaultSizePageUnits = 1.0 * Units_Factor(UNIT_INCHES,m_pageUnits);   
      break;
   }

   if (getHSizePageUnits() == DBL_MIN)
   {
      setHSizePageUnits(defaultSizePageUnits);
   }

   if (getVSizePageUnits() == DBL_MIN)
   {
      setVSizePageUnits(defaultSizePageUnits);
   }

   CString unitsParamater;
   unitsParamater.Format(", Units='%s'",
      (const char*)unitsString(m_pageUnits));

   m_size += unitsParamater;

   m_vert = m_horz = true;
   rectifyValues();

   UpdateData(false);
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void PCB_Grid::OnOK() 
{
   UpdateData(true);
   rectifyValues();

   CExtent extent(left,bottom,right,top);

   CManufacturingGrid manufacturingGrid(m_hSteps,getHSizePageUnits(),m_vSteps,getVSizePageUnits());
   manufacturingGrid.regenerateGrid(*doc,*pcbFile,extent,m_gridThickness);

   doc->OnFitPageToImage();

   CDialog::OnOK();
}

void PCB_Grid::OnSave() 
{
   CString settingsFile = getApp().getUserPath() + "default.grd";
   CFileDialog FileDialog(FALSE, "GRD", settingsFile,
         OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
         "Manuf Grid File (*.GRD)|*.GRD|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   CStdioFile file;
   if (!file.Open(FileDialog.GetPathName(), file.modeCreate | file.modeWrite | file.typeText))
   {
      ErrorMessage(FileDialog.GetPathName(), "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   UpdateData();

   file.WriteString("! CAMCAD Manufacturing Grid Settings\n\n");

   int decimals = GetDecimals(doc->getSettings().getPageUnits());
   CString buf;

   buf.Format(".Units %s\n", GetUnitName(doc->getSettings().getPageUnits()));
   file.WriteString(buf);

   buf.Format(".Horizontal %d %d %.*lf\n", m_horz, m_hSteps, decimals, getHSizePageUnits());
   file.WriteString(buf);
   buf.Format(".Vertical %d %d %.*lf\n", m_vert, m_vSteps, decimals, getVSizePageUnits());
   file.WriteString(buf);
   buf.Format(".Thickness %.*lf\n", decimals,m_gridThickness);
   file.WriteString(buf);

   file.Close();
}

void PCB_Grid::OnLoad() 
{
   CFileDialog FileDialog(TRUE, "GRD", "*.grd",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Manuf Grid File (*.GRD)|*.GRD|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;
   
   LoadGridSettings(FileDialog.GetPathName(), TRUE);
}

/*****************************************************************************/
/*
*/
void PCB_Grid::LoadGridSettings(CString fileName, BOOL warnFileOpen)
{
   char     line[255];
   char     *lp;
   FILE     *fp;

   if ((fp = fopen(fileName,"rt")) == NULL)
   {
      if (warnFileOpen)
         ErrorMessage(fileName, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   double scale = 1;
   int decimal = GetDecimals(doc->getSettings().getPageUnits());

   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] != '.')                            continue;

      if (!STRCMPI(lp,".Units"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         int units = GetUnitIndex(lp);

         if (units > -1)
         {
            m_pageUnits = units;
            scale = Units_Factor(units, doc->getSettings().getPageUnits());
         }
         else
         {
            // unit error
         }
      }
      else if (!STRCMPI(lp,".Horizontal"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         m_horz = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         m_hSteps = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         setHSizePageUnits(atof(lp));
      }
      else if (!STRCMPI(lp,".Vertical"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         m_vert = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         m_vSteps = atoi(lp);

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         setVSizePageUnits(atof(lp));
      }
      else if (!STRCMPI(lp,".Thickness"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         m_gridThickness = atof(lp);
      }
   }

   fclose(fp);

   UpdateData(FALSE);
}

void PCB_Grid::OnBnClickedHStepsButton()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnBnClickedVStepsButton()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnBnClickedHSizeButton()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnBnClickedVSizeButton()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnEnKillfocusHSteps()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnEnKillfocusVSteps()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnEnKillfocusHSize()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}

void PCB_Grid::OnEnKillfocusVSize()
{
   UpdateData(true);
   rectifyValues();
   UpdateData(false);
}
