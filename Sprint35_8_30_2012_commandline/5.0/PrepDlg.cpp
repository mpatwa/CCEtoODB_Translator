// $Header: /CAMCAD/5.0/read_wrt/PrepDlg.cpp 2     01/06/09 9:51a Sharry Huang $
#include "stdafx.h"
#include "PrepDlg.h"
#include "extents.h"
#include "CcDoc.h"
#include "dcamachine.h"

extern void RotMat2( Mat2x2 *m, double az );
extern void TransPoint2( Point2 *v, int n, const Mat2x2 *m, double tx, double ty );
//Utility functions

CString GetMachineAttributeName(CString machineName, bool isBottom) 
{
   return machineName + ((isBottom)?QMACHINELOCATION_BOT:QMACHINELOCATION_TOP);
}

CString GetMachineAttributeName(FileTypeTag machineType, bool isBottom)
{
   return getMachineName(machineType) + ((isBottom)?QMACHINELOCATION_BOT:QMACHINELOCATION_TOP);
}

static bool GetOutlineExtent(CCEtoODBDoc *doc, FileStruct *file, ExtentRect *outlineExtent) 
{

	if (!doc || !outlineExtent || !file)
		return false;

	outlineExtent->left = FLT_MAX;
	outlineExtent->right = -FLT_MAX;
	outlineExtent->top = -FLT_MAX;
	outlineExtent->bottom = FLT_MAX;

   Mat2x2 m;
   RotMat2(&m, file->getRotation());

   bool Found = false;

   // find lower-left of visible Primary Board Outlines
   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getDataType() == T_POLY && (data->getGraphicClass() == GR_CLASS_BOARDOUTLINE || data->getGraphicClass() == GR_CLASS_PANELOUTLINE))
      {
         ExtentRect extents;   
         PolyExtents(doc, data->getPolyList(), &extents, file->getScale(), (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0), file->getInsertX(), file->getInsertY(), &m, FALSE);

         if (extents.left < outlineExtent->left)
            outlineExtent->left = extents.left;

         if (extents.right > outlineExtent->right)
            outlineExtent->right = extents.right;

         if (extents.bottom < outlineExtent->bottom)
            outlineExtent->bottom = extents.bottom;

         if (extents.top > outlineExtent->top)
            outlineExtent->top = extents.top;
         
         Found = true;
         break;
      }
   }

   return Found;
}

static void DrawCircle(double diameter,CString Layer)
{
   int laynr = Graph_Level(Layer, "", 0);
   int widthindex,err;
   widthindex = Graph_Aperture("", T_ROUND, 0 , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
   Graph_Circle(laynr,0,0, diameter/2, 
        0L, widthindex , FALSE, TRUE);       
}

/******************************************************************************
//   saveRestoreSettings
******************************************************************************/
saveRestoreSettings::saveRestoreSettings(CCEtoODBDoc *doc, FileStruct *File, bool BottomSide)
: m_doc(doc)
, m_File(File)
, m_BottomSide(BottomSide)
{
   m_originalFileInsertPt.x = 0;
   m_originalFileInsertPt.y = 0;
   m_originalFileRotRadians = 0;
   m_originalFileMirror = false;
   m_IsRestructed = false;
   m_mirroringRequired = false;
}

void saveRestoreSettings::Save(bool isRestructed, bool mirroringRequired)
{
   if(!m_doc || !m_File) return;

   if(isRestructed)
   {
      m_originalFileInsertPt.x = m_File->getInsertX() * ((m_BottomSide)?(-1):1);
	   m_originalFileInsertPt.y = m_File->getInsertY();
	   m_originalFileRotRadians = m_File->getRotation() * ((m_BottomSide)?(-1):1);
	   m_originalFileMirror = m_File->isMirrored();
   }

   m_mirroringRequired = mirroringRequired;
   m_IsRestructed = isRestructed;
   
   //Nomalize the file
   if(m_IsRestructed) RestructureFile(m_doc,m_File);
}

void saveRestoreSettings::Restore()
{
   if(!m_doc || !m_File) return;

   if(!m_IsRestructed)
   {
	   m_File->setRotation((DbUnit)this->m_originalFileRotRadians);
	   m_File->setInsertX((DbUnit)this->m_originalFileInsertPt.x);
	   m_File->setInsertY((DbUnit)this->m_originalFileInsertPt.y);
	   m_File->setMirrored(m_originalFileMirror);
   }
   else
   {

      Mat2x2 m;
      double angle = NormalizeAngle(-m_originalFileRotRadians);
      RotMat2(&m, NormalizeAngle(angle));
      Point2 point2;
      point2.x = m_originalFileInsertPt.x;
      point2.y = m_originalFileInsertPt.y;
      TransPoint2(&point2, 1, &m, 0.0,0.0);
      double fileInsertX = -point2.x;
      double fileInsertY = -point2.y;
      
      m_File->setRotation(angle);
      m_File->setInsertX(fileInsertX);
      m_File->setInsertY(fileInsertY);
      m_File->setMirrored(m_originalFileMirror);

      RestructureFile(m_doc,m_File);
   }

   if(m_mirroringRequired)
   {
      m_File->setMirrored(true);
      RestructureFile(m_doc,m_File);
   }

}

/******************************************************************************
//   CPrepDlg Dialog
******************************************************************************/
IMPLEMENT_DYNAMIC(CPrepDlg, CDialog)
CPrepDlg::CPrepDlg(CString machineName, bool isBothSide)
	: CDialog(CPrepDlg::IDD, NULL)
	, m_radioBoardSide(0)
   , m_machineName(machineName)
   , m_IsBothSide(isBothSide)
   , m_Doc((NULL != getActiveView())?getActiveView()->GetDocument():NULL)
   , m_File((m_Doc)?m_Doc->getFileList().GetFirstShown(blockTypeUndefined):NULL)
   , m_originSetting(m_Doc,m_File,false)
{
  //TODO
   m_topFileSettings.ResetSetting();
   m_botFileSettings.ResetSetting();
   
   m_fileType = getFileType(m_machineName);
   if(m_fileType == fileTypeUnknown && !m_machineName.IsEmpty())
      m_fileType = fileTypeCustomAssembly;

   // Get file location from attributes
	if (m_File != NULL)
	{
		Attrib *atop =  is_attvalue(m_Doc, m_File->getBlock()->getDefinedAttributes(), GetMachineAttributeName(m_machineName, false), 0);	
		Attrib *abot =  is_attvalue(m_Doc, m_File->getBlock()->getDefinedAttributes(), GetMachineAttributeName(m_machineName, true), 0);
		if (atop != NULL)
		{
			CString attval = get_attvalue_string(m_Doc, atop);         
			m_topFileSettings = attval;
		}
      
		if (abot != NULL)
		{
			CString attval = get_attvalue_string(m_Doc, abot);         
			m_botFileSettings = attval;
		}
	}
}

CPrepDlg::~CPrepDlg()
{
   
}

void CPrepDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_QD_ORIGIN_CORNER, m_cboOriginCorner);
	DDX_Control(pDX, IDC_QD_ROTATION, m_cboRotation);
	DDX_Control(pDX, IDC_OFFSET_X, m_txtOffsetX);
	DDX_Control(pDX, IDC_OFFSET_Y, m_txtOffsetY);
	DDX_Radio(pDX, IDC_QD_TOP, m_radioBoardSide);
}


BEGIN_MESSAGE_MAP(CPrepDlg, CDialog)
	ON_BN_CLICKED(IDC_QD_APPLY, OnBnClickedApply)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_QD_RESET, OnBnClickedReset)
	ON_BN_CLICKED(IDC_QD_TOP, OnBnClickedTop)
	ON_BN_CLICKED(IDC_QD_BOTTOM, OnBnClickedBottom)
END_MESSAGE_MAP()


BOOL CPrepDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   if(m_IsBothSide)
   {
      GetDlgItem(IDC_QD_BOTTOM)->ShowWindow(false);
      GetDlgItem(IDC_QD_TOP)->SetWindowText("Both");
   }
	m_cboOriginCorner.AddString("Lower Left");
	m_cboOriginCorner.AddString("Lower Right");
	m_cboOriginCorner.AddString("Upper Left");
	m_cboOriginCorner.AddString("Upper Right");

	m_cboRotation.AddString("0");
	m_cboRotation.AddString("90");
	m_cboRotation.AddString("180");
	m_cboRotation.AddString("270");

   m_topFileSettings.m_rotation = Rot0;
   m_botFileSettings.m_rotation = Rot0;
   m_originSetting.Save(true,false);
   
   //TODO   
   OnBnClickedTop();

	return TRUE;
}

ExportFileSettings *CPrepDlg::getFileSettingBySide(int isBottom)
{
   ExportFileSettings *baseFileSettings[] = {&m_topFileSettings,&m_botFileSettings};
   return baseFileSettings[isBottom];
}

void CPrepDlg::getOutlineOffset(ECCorner corner, double &newOffsetX, double &newOffsetY)
{
   // Get new offset
	ExtentRect rect;
	if (GetOutlineExtent(m_Doc, m_File, &rect))
	{
		switch (corner)
		{
		case LowerLeft:
			newOffsetX = -rect.left;
			newOffsetY = -rect.bottom;
			break;

		case LowerRight:
			newOffsetX = -rect.right;
			newOffsetY = -rect.bottom;
			break;

		case UpperLeft:
			newOffsetX = -rect.left;
			newOffsetY = -rect.top;
			break;

		case UpperRight:
			newOffsetX = -rect.right;
			newOffsetY = -rect.top;
			break;
		}
	}
}

bool CPrepDlg::MoveToBoardOrigin(bool bottomSide)
{      
   double fileInsertX = m_File->getInsertX(), fileInsertY = m_File->getInsertY();
   double fileangle = m_File->getRotationRadians();

   bool isMirror = bottomSide;
   bool mirroringRequired = false;
   bool isFileRestructed = false;
   bool isOriginExisted = false;

   m_originSetting.Restore();

   if(bottomSide)
      isOriginExisted = m_botFileSettings.GetMachineOriginOffset(m_Doc,m_File,fileInsertX,fileInsertY,fileangle,isMirror,mirroringRequired,bottomSide);
   else
      isOriginExisted = m_topFileSettings.GetMachineOriginOffset(m_Doc,m_File,fileInsertX,fileInsertY,fileangle,isMirror,mirroringRequired,bottomSide);

   if(isOriginExisted)
   {
      isFileRestructed = true;      
      m_File->setMirrored(isMirror);
      m_File->setRotation(fileangle);
      m_File->setInsertX(fileInsertX);
      m_File->setInsertY(fileInsertY);
      
      // Save the machine origin so file can rotate back to view origin
      m_originSetting.setPlaceBottom(bottomSide);
      m_originSetting.Save(isFileRestructed,mirroringRequired); 
   }
   else
   {
      m_File->setMirrored(isMirror);
   }


   return isOriginExisted;
}

void CPrepDlg::ApplyToGraphic(ExportFileSettings *loc)
{
	if (loc != NULL)
	{
      double rotDegrees = loc->getRotationDegree();
		// Work with raw panel inserted at 0,0
		// Ultimately we're allowing the user to derive an offset that is based on
		// a panel outline corner, not the panel file insert offset.
		m_File->setInsertX((DbUnit)0.0);
		m_File->setInsertY((DbUnit)0.0);

		// Set new file rotation
		m_File->setRotation((DbUnit)(DegToRad(rotDegrees)));

		double newOffsetX = 0, newOffsetY = 0;
      getOutlineOffset(loc->m_corner, newOffsetX, newOffsetY);
      newOffsetX += loc->m_offsetX;
      newOffsetY += loc->m_offsetY;

		m_File->setInsertX((DbUnit)newOffsetX);
		m_File->setInsertY((DbUnit)newOffsetY);

		m_Doc->UpdateAllViews(NULL);
		m_Doc->OnFitPageToImage();
	}
}

void CPrepDlg::ApplyToGraphic(int radioBoardSide)
{
   bool isBottom = radioBoardSide?true:false;
   ExportFileSettings *setting = getFileSettingBySide(radioBoardSide);
   
   //Apply setting to graphics
   MoveToBoardOrigin(isBottom);
   ApplyToGraphic(setting);

   SetControls(setting);
}

void CPrepDlg::SaveMachineLocationSettings(bool isBottom)
{
   if (m_File != NULL && m_File->getBlock() != NULL)
	{      
      CreateMachineOrigin(isBottom);
		m_Doc->SetUnknownAttrib(&m_File->getBlock()->getDefinedAttributes(),GetMachineAttributeName(m_machineName,isBottom), getFileSettingBySide(isBottom)->GetSettingsString(), SA_OVERWRITE, NULL);      
	}
}

void CPrepDlg::CreateMachineOrigin(bool isBottom)
{
   if (m_File != NULL && m_File->getBlock() != NULL)
	{
      m_Doc->PrepareAddEntity(m_File);
      
      ExportFileSettings *fileSettings = getFileSettingBySide(isBottom);
      int blockNumber = fileSettings->getMachineOriginBlockNumber();
      CString attributeName = GetMachineAttributeName(m_machineName, isBottom);
      
      if(!MoveToBoardOrigin(isBottom))
         m_File->setMirrored(isBottom);

      double rotDegrees = fileSettings->getRotationDegree();
      m_File->setRotation((DbUnit)(DegToRad(rotDegrees)));

      double newOffsetX = 0.0, newOffsetY = 0.0;
      getOutlineOffset(fileSettings->m_corner, newOffsetX, newOffsetY);
      newOffsetX += fileSettings->m_offsetX;
      newOffsetY += fileSettings->m_offsetY;
      
      m_File->setInsertX((DbUnit)newOffsetX);
      m_File->setInsertY((DbUnit)newOffsetY);

      //remove existing inserts
      if(blockNumber)
      {
         /*POSITION exDataPos = fileSettings->FindMachineOriginDataPosition(m_Doc,m_File);
         if(exDataPos)  m_File->getBlock()->getDataList().RemoveAt(exDataPos);*/
      }

      //remove existing geometry
      if (blockNumber == 0 || !m_Doc->Find_Block_by_Num(blockNumber))
      {
         BlockStruct *newBlock = CreateOriginGeometry(attributeName);
         fileSettings->setMachineOriginBlockNumber(newBlock->getBlockNumber());
         blockNumber = newBlock->getBlockNumber();
      }
              
      //create a new geometry
      CreateOriginData(blockNumber, attributeName);          

   }//if

   m_File->setInsertX((DbUnit)0);
   m_File->setInsertY((DbUnit)0);
   m_File->setRotation(0);
}

BlockStruct *CPrepDlg::CreateOriginGeometry(CString attributeName)
{
   BlockStruct *newBlock = Graph_Block_Exists(m_Doc, attributeName, -1);
   if(!newBlock)
   {
      newBlock = Graph_Block_On(GBO_APPEND, attributeName, -1, 0L);
   
      ExtentRect rect;
      if (GetOutlineExtent(m_Doc, m_File, &rect))
      {
         double diameter = 0.5 * getUnitsFactor(pageUnitsMilliMeters,m_Doc->getSettings().getPageUnits()); //min (fabs(rect.top - rect.bottom) , fabs(rect.right - rect.left)) / 15;
         DrawCircle(diameter, attributeName);
      }
      
      Graph_Block_Off();
   }
   return newBlock;
}

DataStruct *CPrepDlg::CreateOriginData(int blockNumber, CString attributeName)
{   
   if(!m_File) return NULL;

   CString fileType = (m_File->getBlockType() == blockTypePanel)?"PANEL_":"PCB_";
   
   DataStruct *data = Graph_Block_Reference(blockNumber, fileType + attributeName, 0, 0, 
         0, 0, 1.0, Graph_Level("0","",1));

   Mat2x2 m;
   RotMat2(&m, NormalizeAngle(-m_File->getRotation()));
   Point2 point2;
   point2.x = -m_File->getInsertX();
   point2.y = -m_File->getInsertY();
   TransPoint2(&point2, 1, &m, 0.0, 0.0);
   data->getInsert()->setAngle(-m_File->getRotation());

   if (m_File->isMirrored())
      point2.x = -point2.x;

   data->getInsert()->setOriginX(point2.x);
   data->getInsert()->setOriginY(point2.y);

   if(m_File->isMirrored())
      data->getInsert()->setAngle(NormalizeAngle(- data->getInsert()->getAngle()));

   data->getInsert()->setMirrorFlags(data->getInsert()->getMirrorFlags() ^ (m_File->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0));
   if (m_File->isMirrored())
      data->getInsert()->setPlacedBottom(!data->getInsert()->getPlacedBottom());

   data->getInsert()->setInsertType(insertTypeUnknown);

   return data;
}

void CPrepDlg::RestoreFile()
{
   m_originSetting.Restore();
   m_Doc->UpdateAllViews(NULL);
	m_Doc->OnFitPageToImage();
}

void CPrepDlg::SetControls(ExportFileSettings *loc)
{
	if (loc != NULL)
	{
		m_cboOriginCorner.SetCurSel(loc->m_corner);
		m_cboRotation.SetCurSel(loc->m_rotation);

		CString txt;
		txt.Format("%f", loc->m_offsetX);
		m_txtOffsetX.SetWindowText(txt);

		txt.Format("%f", loc->m_offsetY);
		m_txtOffsetY.SetWindowText(txt);
	}
}

void CPrepDlg::GetControls()
{
	UpdateData(TRUE); // Get GUI settings

	ERotation rot = (ERotation)m_cboRotation.GetCurSel();
	ECCorner corner = (ECCorner)m_cboOriginCorner.GetCurSel();

	CString offsetXstr;
	m_txtOffsetX.GetWindowText(offsetXstr);
	double offx = atof(offsetXstr);

	CString offsetYstr;
	m_txtOffsetY.GetWindowText(offsetYstr);
	double offy = atof(offsetYstr);

   getFileSettingBySide(m_radioBoardSide)->set(corner,offx,offy,rot);
   getFileSettingBySide(m_radioBoardSide)->setFileType(m_fileType);

   if(m_IsBothSide) // Copy top setting to bottom setting
   {
      m_botFileSettings = m_topFileSettings;
   }
}

//Handle event 
void CPrepDlg::OnBnClickedApply()
{
	UpdateData(TRUE); // Get GUI settings
   GetControls();
   
   //Save settings and reset 
   if(!m_IsBothSide)
   {
      SaveMachineLocationSettings(true);
      getFileSettingBySide(true)->m_rotation = Rot0;
   }
   SaveMachineLocationSettings(false);
   getFileSettingBySide(false)->m_rotation = Rot0;

   ApplyToGraphic(m_radioBoardSide);
	UpdateData(FALSE);
}

void CPrepDlg::OnBnClickedOk()
{
	UpdateData(TRUE); // Get GUI settings
	GetControls();
	
   SaveMachineLocationSettings(false);
   if(!m_IsBothSide)
   {
	   SaveMachineLocationSettings(true);
   }
   RestoreFile();
   
   OnOK();
}

void CPrepDlg::OnBnClickedCancel()
{	
   RestoreFile();
	OnCancel();
}

void CPrepDlg::OnBnClickedReset()
{
   ExportFileSettings *settings = getFileSettingBySide(m_radioBoardSide);
   settings->set(settings->m_corner,0,0,Rot0);

   SetControls(settings);
   OnBnClickedApply();
   
   UpdateData(FALSE);
}

void CPrepDlg::OnBnClickedTop()
{
	m_radioBoardSide = 0;  
   ApplyToGraphic(m_radioBoardSide);

	UpdateData(FALSE);
}

void CPrepDlg::OnBnClickedBottom()
{
	m_radioBoardSide = 1;
   ApplyToGraphic(m_radioBoardSide);

	UpdateData(FALSE);
}

/******************************************************************************
//   ExportFileSettings
******************************************************************************/
CString ExportFileSettings::GetSettingsString()
{	
	CString offsetX;
	offsetX.Format("%f", this->m_offsetX);

	CString offsetY;
	offsetY.Format("%f", this->m_offsetY);

   CString blockNumberStr, fileType;
   blockNumberStr.Format("%d",m_MachineOriginBlockNumber);
   fileType.Format("%d",fileTypeUnknown) ;
   
   CString settings;
	settings.Format("%s,%s,%s,%s,%s", getCornerString(), offsetX, offsetY, getRotationString(),blockNumberStr);
	return settings;
}


CString ExportFileSettings::getCornerString()
{
   CString cornerStr = "LowerLeft";

	switch (this->m_corner)
	{
	case LowerLeft:
		cornerStr = "LowerLeft";
		break;
	case LowerRight:
		cornerStr = "LowerRight";
		break;
	case UpperLeft:
		cornerStr = "UpperLeft";
		break;
	case UpperRight:
		cornerStr = "UpperRight";
		break;
	default:
		cornerStr = "Invalid";
		break;
	}

   return cornerStr;
}

ECCorner ExportFileSettings::getCornerTag(CString cornerstr)
{
   ECCorner corner = LowerLeft;
   if (cornerstr.CompareNoCase("LowerRight") == 0)
	   corner = LowerRight;
   else if (cornerstr.CompareNoCase("UpperLeft") == 0)
	   corner = UpperLeft;
   else if (cornerstr.CompareNoCase("UpperRight") == 0)
	   corner = UpperRight;

   return corner;
}

CString ExportFileSettings::getRotationString()
{
   CString rotationStr = "0";
	if (this->m_rotation == Rot90)
		rotationStr = "90";
	else if (this->m_rotation == Rot180)
		rotationStr = "180";
	else if (this->m_rotation == Rot270)
		rotationStr = "270";

   return rotationStr;
}

double ExportFileSettings::getRotationDegree()
{
   double rotDegrees = 0.0;
	if (m_rotation == Rot90)
		rotDegrees = 90.0;
	else if (m_rotation == Rot180)
		rotDegrees = 180.0;
	else if (m_rotation == Rot270)
		rotDegrees = 270.0;
   
   return rotDegrees;
}

ERotation ExportFileSettings::getRotationTag(CString rotationstr)
{
   ERotation rot = Rot0;
	if (rotationstr.Compare("90") == 0)
		rot = Rot90;
	else if (rotationstr.Compare("180") == 0)
		rot = Rot180;
	else if (rotationstr.Compare("270") == 0)
		rot = Rot270;

   return rot;
}

ExportFileSettings& ExportFileSettings::operator=(const CString& recordStringPrm)
{
   CString recordString = recordStringPrm;

   CStringArray parameter;
   CSupString supOption(recordString);
   supOption.ParseQuote(parameter,",; \t");

   if(parameter.GetCount() >= MLTag_Last)
   {
      m_corner = getCornerTag(parameter.GetAt(MLTag_Corner));
	   m_offsetX = atof(parameter.GetAt(MLTag_PosX));
	   m_offsetY = atof(parameter.GetAt(MLTag_PosY));
	   m_rotation = getRotationTag(parameter.GetAt(MLTag_Rotation));
      m_MachineOriginBlockNumber = atoi(parameter.GetAt(MLTag_BlockNumber));
   }

   return *this;
}

ExportFileSettings& ExportFileSettings::operator=(ExportFileSettings& otherSetting)
{
   m_corner = otherSetting.m_corner;
	m_offsetX = otherSetting.m_offsetX;
	m_offsetY = otherSetting.m_offsetY;
	m_rotation = otherSetting.m_rotation;
   m_MachineOriginBlockNumber = otherSetting.m_MachineOriginBlockNumber;

   return *this;
}

/*POSITION ExportFileSettings::FindMachineOriginDataPosition(CCEtoODBDoc *doc, FileStruct *file)
{
   POSITION curPOS = NULL;
   if (doc && file && file->getBlock() && m_MachineOriginBlockNumber)
	{ 
      curPOS = doc->FindInsertDataAt(file,m_MachineOriginBlockNumber);
      CString machineName = (doc->getBlockAt(m_MachineOriginBlockNumber))?doc->getBlockAt(m_MachineOriginBlockNumber)->getName():"";
      m_fileType = (!machineName.IsEmpty())?getMachineFileType(machineName):fileTypeUnknown;
   }

   return curPOS;
}*/

DataStruct *ExportFileSettings::FindMachineOriginData(CCEtoODBDoc *doc, FileStruct *file)
{
   /*POSITION dataPos = FindMachineOriginDataPosition(doc,file);   
   return (dataPos)?file->getBlock()->getDataList().GetAt(dataPos):NULL;*/
	return NULL;
}

FileTypeTag ExportFileSettings::getMachineFileType(CString machineName)
{
   if(machineName.IsEmpty()) return fileTypeUnknown;

   CString machineSubName = "";
   int NamePos = machineName.Find(QMACHINELOCATION_TOP); 
   if(NamePos > 0)
   {
      machineSubName = machineName.Mid(0,NamePos);
   }
   else
   {
      NamePos = machineName.Find(QMACHINELOCATION_BOT); 
      if(NamePos > 0)
      {
         machineSubName = machineName.Mid(0,NamePos);
      }
   }

   if(machineSubName.IsEmpty()) return fileTypeUnknown;
   FileTypeTag fileType = getFileType(machineSubName);

   return  (fileType == fileTypeUnknown)?fileTypeCustomAssembly:fileType;
}

bool ExportFileSettings::GetMachineOriginOffset(CCEtoODBDoc *doc, FileStruct *file, double &fileInsertX, double &fileInsertY, 
                                                double &fileangle, bool &isMirror, bool &mirroringRequired, bool bottomSide)
{
   DataStruct  *np = NULL;
   if((np = FindMachineOriginData(doc,file)))
   {
      if((bottomSide && !np->getInsert()->getGraphicMirrored())
         || (!bottomSide && np->getInsert()->getGraphicMirrored()))
      {
         mirroringRequired = true;
         file->setMirrored(true);
         RestructureFile(doc,file);
      }
      
      //Rotate to machine origin
      Mat2x2 m;
      Point2 point2;
      int NegtiveValue = bottomSide?-1:1;
      fileangle = NormalizeAngle(-np->getInsert()->getAngleRadians() * NegtiveValue);
      RotMat2(&m, NormalizeAngle(fileangle));

      point2.x = np->getInsert()->getOriginX() * NegtiveValue;
      point2.y = np->getInsert()->getOriginY();
      TransPoint2(&point2, 1, &m, 0.0,0.0);
      
      fileInsertX = -point2.x;
      fileInsertY = -point2.y;
      isMirror = bottomSide;
      
      int blockNumber = np->getInsert()->getBlockNumber();
      CString machineName = (doc->getBlockAt(blockNumber))?doc->getBlockAt(blockNumber)->getName():"";
      m_fileType = (!machineName.IsEmpty())?getMachineFileType(machineName):fileTypeUnknown;
     
      return TRUE;
   } 

   return FALSE;
}

void ExportFileSettings::ApplyToCCZ(CCEtoODBDoc *doc, FileStruct *file, saveRestoreSettings& restSettings, FileTypeTag fileType) 
{
   if(!file) return;

   bool bottomSide = restSettings.getPlaceBottom();
   double fileInsertX = file->getInsertX();
   double fileInsertY = file->getInsertY();
   double fileangle = file->getRotationRadians();

   bool isMirror = file->isMirrored();
   bool mirroringRequired = false;
   bool isFileRestructed = false;
   
   // Get Machine Origin Offset
   GetMachineOriginOffset(doc, file, fileInsertX, fileInsertY, fileangle, isMirror, mirroringRequired, bottomSide);
   m_fileType = fileType;

   // Adjust machine origin 
   ApplyCCZByMachine(m_fileType, bottomSide, fileInsertX, fileInsertY, fileangle, isMirror, isFileRestructed);

   file->setMirrored(isMirror);
   file->setRotation(fileangle);
   file->setInsertX(fileInsertX);
   file->setInsertY(fileInsertY);

   // Save the machine origin so file can rotate back to view origin
   restSettings.Save(isFileRestructed,mirroringRequired);   
}

void ExportFileSettings::ApplyCCZByMachine(FileTypeTag fileType, bool bottomSide, double& fileInsertX, double& fileInsertY, 
                                           double& angle, bool &isMirror, bool& isFileRestructed) 
{    
   switch(fileType)
   {
   case fileTypeSiemensQd: 
      fileInsertX = fileInsertX * ((bottomSide)?-1:1);
      //Siemens Exporter provided panel normalization 
      // So there is no necessary to use RestructureFile
      // to normalize the board to 0,0,0
      isFileRestructed = false;                     
      break;
   default:
      isFileRestructed = true;
      isMirror = bottomSide;
      break;
   }
}

