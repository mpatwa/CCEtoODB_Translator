// $Header: /CAMCAD/5.0/DeviceTyper.cpp 115   6/17/07 8:50p Kurt Van Ness $

#include "StdAfx.h"
#include "CcDoc.h"
#include "DeviceTyper.h"
#include "gauge.h"
#include "CCEtoODB.h"
#include "Find.h"


///////////////////////////////////Externals//////////////////////////////////////////////
extern CView *activeView; // from CCVIEW.CPP

/////////////////////////////////Declarations/////////////////////////////////////////////

#define PINMAP_NONE "<None>"

static CDeviceTypeArray Devices;

static CStringArray prefixes;
static int          prefixesCnt;

static CStringArray enabledDevices;
static int enabledDevicesCount;

static CStringArray geomPinMaps;
static int geomPinMapsCount;
static CStringArray PNPinMaps;
static int PNPinMapsCount;

char *get_string(char *line, const char *delimeter, BOOL BackslashIsEscape);
void EditGeometry(CCEtoODBDoc *doc, BlockStruct *block);
BOOL ComponentsExtents(CCEtoODBDoc *doc, const char *string, double *xmin, double *xmax, double *ymin, double *ymax, int SELECT_COMP);
int PanReference(CCEtoODBView *view, const char *ref) ;
void Notepad(const char *file);
BOOL GetLicenseString(CLSID clsid, BSTR &licStr);

//_____________________________________________________________________________
void CCEtoODBDoc::OnPcbGeneratedevicetype()
{
   //check to make sure there is a single PCB board active
   BOOL panel = FALSE;
   int pcbCount = 0;

   POSITION filePos = this->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct *file = this->getFileList().GetNext(filePos);

      if (!file)
         continue;

      if (file->isShown() && file->getBlockType() == BLOCKTYPE_PCB)
      {
         pcbCount++;
      }
      else if (file->isShown() && file->getBlockType() == BLOCKTYPE_PANEL)
      {
         panel = TRUE;
      }
   }

   if (pcbCount != 1 || panel)
   {
      ErrorMessage("No single visible PCB board found. \nDevice Type Generator doesn't work on panels or multiple PCB boards.", "Device Type Generator");
      return;
   }

   FILE  *fp;

   //------------------------------------------------------
   //read attrib file

   CString devicesStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_DEVICES_STP) );

   if ((fp = fopen(devicesStpFilename, "r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("Cannot open [%s] for reading.", devicesStpFilename);
      ErrorMessage(tmp,"", MB_OK | MB_ICONHAND);

      return;
   }
   fclose(fp);

   CDeviceTyperPropertySheet deviceTyperPropertySheet(*this);
   deviceTyperPropertySheet.DoModal();
 
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CDeviceTyperCompPropertyPage, CPropertyPage)

CDeviceTyperCompPropertyPage::CDeviceTyperCompPropertyPage()
   : CPropertyPage(CDeviceTyperCompPropertyPage::IDD)
   , m_radioView(0), m_radioGrouping(0)
{
   m_activeFileNumber = 0;
   m_imageList = new CImageList();
}

CDeviceTyperCompPropertyPage::CDeviceTyperCompPropertyPage(CCEtoODBDoc& document)
   : CPropertyPage(CDeviceTyperCompPropertyPage::IDD)
   , m_radioView(0)
   , m_radioGrouping(0)
{
   doc = &document;
   m_activeFileNumber = doc->getFileList().GetOnlyShown(blockTypePcb)->getFileNumber();
   m_imageList = new CImageList();
}

CDeviceTyperCompPropertyPage::~CDeviceTyperCompPropertyPage()
{
   if (m_imageList)
   {
      m_imageList->DeleteImageList();

      delete m_imageList;
   }

   Devices.DestroyAll();
}

void CDeviceTyperCompPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_COMPS_TREE, m_CompsTree);
   DDX_Control(pDX, IDC_LIST_DEVICES, m_ListDevices);
   DDX_Radio(pDX, IDC_RADIO_VIEWALL, m_radioView);
   DDX_Radio(pDX, IDC_RADIO_FLAT, m_radioGrouping);
   DDX_Control(pDX, IDC_COMBO_PREFIXGEOM, m_cboPrefixGeom);
   DDX_Control(pDX, IDC_EDIT_FIND, m_compToFind);
   DDX_Control(pDX, IDC_COMBO_KWVALUE, m_cboKwValue);
   DDX_Control(pDX, IDC_STATIC_TOTAL, m_lblTotal);
   DDX_Control(pDX, IDC_STATIC_ASSIGNED, m_lblAssigned);
   DDX_Control(pDX, IDC_STATIC_NOTASSIGNED, m_lbl_NotAssigned);
   DDX_Control(pDX, IDC_STATIC_PERCENTAGE, m_lblPercentage);
   DDX_Control(pDX, IDC_CHECK1, m_addToStpFile);
}


BEGIN_MESSAGE_MAP(CDeviceTyperCompPropertyPage, CPropertyPage)
   ON_BN_CLICKED(IDC_BTN_AUTO_ASSIGN, OnBnClickedBtnAutoAssign)
   ON_BN_CLICKED(IDC_RADIO_VIEWALL, OnBnClickedRadioViewall)
   ON_BN_CLICKED(IDC_RADIO_VIEWUNASSIGNED, OnBnClickedRadioViewunassigned)
   ON_BN_CLICKED(IDC_RADIOVIEWVIOLATIONS, OnBnClickedRadioviewviolations)
   ON_BN_CLICKED(IDC_RADIO_FLAT, OnBnClickedRadioFlat)
   ON_BN_CLICKED(IDC_RADIO_KWVALUE, OnBnClickedRadioKwvalue)
   ON_BN_CLICKED(IDC_RADIO_PREFIXGEOM, OnBnClickedRadioPrefixgeom)
   ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
   ON_NOTIFY(TVN_SELCHANGED, IDC_COMPS_TREE, OnTvnSelchangedCompsTree)
   ON_CBN_SELCHANGE(IDC_COMBO_PREFIXGEOM, OnCbnSelchangeComboPrefixgeom)
   ON_BN_CLICKED(IDC_BUTTON_MANUAL, OnBnClickedButtonManual)
   ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
   ON_BN_CLICKED(IDC_BUTTON_ZOOM1TO1, OnBnClickedButtonZoom1to1)
   ON_BN_CLICKED(IDC_BUTTON_CLRKW, OnBnClickedButtonClrkw)
   ON_BN_CLICKED(IDC_BUTTON_CLRPRE, OnBnClickedButtonClrpre)
   ON_CBN_SELCHANGE(IDC_COMBO_KWVALUE, OnCbnSelchangeComboKwvalue)
   ON_BN_CLICKED(IDC_BUTTON_REPORT, OnBnClickedButtonReport)
   ON_BN_CLICKED(IDC_BUTTON_CLEARALL, OnBnClickedButtonClearall)
   ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

// CDeviceTyperCompPropertyPage message handlers

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnInitDialog
*
*/
BOOL CDeviceTyperCompPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_imageList->Create(16, 16, TRUE, 32, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_RED_DOT));
   m_imageList->Add(app->LoadIcon(IDI_GREEN_DOT));
   m_imageList->Add(app->LoadIcon(IDI_YELLOW_DOT));
   m_imageList->Add(app->LoadIcon(IDI_DOT_HOLLOW));
   
   m_CompsTree.SetImageList(m_imageList, TVSIL_NORMAL);

   prefixGeomArray.SetSize(100,100);
   prefixGeomArrayCount = 0;

   keyValArray.SetSize(100,100);
   keyValArrayCount = 0;

   Devices.Init(doc);

   deviceTypeKeyword     = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);
   tempDeviceTypeKeyword = doc->RegisterKeyWord("TEMPDEVICETYPE", 0, VT_STRING);
   deviceToPackagePinMap = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);

   m_compsCount = 0;
   m_unassignedCount = 0;
   m_assignedCount = 0;
   m_voilationCount = 0;
   m_onInit = TRUE;

   if (!ReadDeviceTypeMasterList())
      return FALSE;

   for (int i=0; i<Devices.GetCount();i++)
   {
      DeviceTypeStruct *device = Devices[i];

      if (device)
      {
         m_ListDevices.InsertString(i, device->deviceTypeName);
      }
   }

   GetGeomPinMapping();
   GetPartNumPinMapping();

   GetPrefixGeomArray();
   GetKeyValArray();

   AssignPinMappingBasedOnPinName();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct* data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
               continue;

            CString type;
            Attrib* attrib = NULL;

            if (data->getAttributesRef()->Lookup(deviceTypeKeyword, attrib))
            {
               if (attrib)
                  type = attrib->getStringValue();

               void* valuePtr = NULL;
               valuePtr = (void*)type.GetString();
               void* tempValue = NULL;

               doc->SetAttrib(&data->getAttributesRef(), tempDeviceTypeKeyword, VT_STRING, valuePtr, SA_OVERWRITE, NULL);
            }
         }
      }
   }

   // Case 1188
   // The calls to fill the tree view are what take all the time.
   // This call is apparantly not needed. It will be called again
   // by OnSetActive() before the dialog page is displayed. The remainder
   // of Init does not seem to require this call to have happened. 
   // So, it justs uses time.
   //OnBnClickedRadioViewall();

   FillKeywordValueComboList();
   FillPrefixGeomComboList();

   UpdateCountsDisplay();

   prefixesCnt = 0;

   for (int j=0; j<doc->getMaxBlockIndex(); j++)
   {
      BlockStruct* block = doc->getBlockAt(j);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         CString pre;

         while (dataPos)
         {
            DataStruct* data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT
               ||(pre.GetLength() != 0 && !data->getInsert()->getRefname().Left(pre.GetLength()).CompareNoCase(pre)))
               continue;

            pre = data->getInsert()->getRefname();
            pre = pre.TrimRight("0123456789._");

            if (m_cboPrefixGeom.FindString(0, pre) == CB_ERR)
            {
               m_cboPrefixGeom.AddString(pre);
               prefixes.SetAtGrow(prefixesCnt++, pre);            
            }     
         }
      }
   }

   m_addToStpFile.EnableWindow(FALSE);

   return TRUE;
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnSetActive
*
*/
BOOL CDeviceTyperCompPropertyPage::OnSetActive()
{
   CPropertyPage::OnSetActive();

   GetGeomPinMapping();
   GetPartNumPinMapping();
   UpdateFromFile();

   if (m_radioView == 0)
      OnBnClickedRadioViewall();
   else if (m_radioView == 1)
      OnBnClickedRadioViewunassigned();
   else if (m_radioView == 2)
      OnBnClickedRadioviewviolations();

   FillKeywordValueComboList();
   FillPrefixGeomComboList();

   UpdateCountsDisplay();


   return TRUE;
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::AssignPinMappingBasedOnPinName
*
*/
void CDeviceTyperCompPropertyPage::AssignPinMappingBasedOnPinName()
{
   FileStruct* file = doc->getFileList().GetOnlyShown(blockTypePcb);

   POSITION netPos = file->getNetList().GetHeadPosition();
   while (netPos)
   {
      NetStruct* net = file->getNetList().GetNext(netPos);

      if (!net)
         continue;

      POSITION compPinPos = net->getCompPinList().getHeadPosition();
      while (compPinPos)
      {
         CompPinStruct* compPin = net->getCompPinList().getNext(compPinPos);

         if (!compPin)
            continue;

         if (!compPin->getPinName().CompareNoCase("B"))
         {
            CString type = "Base";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("C"))
         {
            CString type = "Collector";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("E"))
         {
            CString type = "Emitter";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("P"))
         {
            CString type = "Positive";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("N"))
         {
            CString type = "Negative";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("A"))
         {
            CString type = "Anode";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("K"))
         {
            CString type = "Cathode";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("D"))
         {
            CString type = "Drain";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("G"))
         {
            CString type = "Gate";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("S"))
         {
            CString type = "Source";
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
         else if (!compPin->getPinName().CompareNoCase("NC"))
         {
            CString type = ATT_VALUE_NO_CONNECT;
            void* voidPtr = (void*)type.GetString();
            doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
         }
      }
   }
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::GetPrefixGeomArray
*
*/
void CDeviceTyperCompPropertyPage::GetPrefixGeomArray()
{
   FILE *fp;
   char  line[255];
   char  *lp;
   CString PrefixFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_REFDESPREFIX_STP) );
   
   if ((fp = fopen(PrefixFile,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",PrefixFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   CString prefix;
   CString geom;
   CString type;

   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line, ",", 1)) == NULL) 
         continue;

      prefix = lp;
      prefix.Trim();
      
      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      geom = lp;      

      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      type = lp;
      type.Trim();

      CString item;
      item.Format("%s,%s,%s", prefix, geom, type);
      prefixGeomArray.SetAtGrow(prefixGeomArrayCount++, item);
   }
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::SetPrefixGeomFile
*
*/
void CDeviceTyperCompPropertyPage::SetPrefixGeomFile()
{
   FILE *fp;
   CString PrefixFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_REFDESPREFIX_STP) );

   if ((fp = fopen(PrefixFile,"w")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",PrefixFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }    

   for (int i=0; i<prefixGeomArrayCount; i++)
   {
      fprintf(fp, "%s\n", prefixGeomArray.GetAt(i));
   }

   fclose(fp);
}
/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::AddPrefixGeomItem()
*
*/
void CDeviceTyperCompPropertyPage::AddPrefixGeomItem(CString newPrefix, CString newGeom, CString newType)
{
   char* tmp;
   BOOL alreadyInFile = FALSE;
   int  nIndex = -1;

   for (int i=0; i<prefixGeomArrayCount; i++)
   {
      CString item = prefixGeomArray.GetAt(i);

      CString prefix;
      CString geom = " ";
      CString type;
      
      if ((tmp = get_string(STRDUP(item), ",", 1)) != NULL) 
         prefix = tmp;
      
      if ((tmp = get_string(NULL, ",", 1)) != NULL) 
         geom = tmp;

      if ((tmp = get_string(NULL, ",", 1)) != NULL) 
         type = tmp;
   
      if (!prefix.CompareNoCase(newPrefix) && !geom.CompareNoCase(newGeom))
      {
         alreadyInFile = TRUE;
         nIndex = i;

         break;
      }
   }

   CString newItem;   
   newItem.Format("%s,%s,%s", newPrefix, newGeom, newType);

   if (alreadyInFile)
      prefixGeomArray.SetAt(nIndex, newItem);
   else
      prefixGeomArray.SetAtGrow(prefixGeomArrayCount++, newItem);
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::GetPrefixGeomArray
*
*/
void CDeviceTyperCompPropertyPage::GetKeyValArray()
{
   FILE *fp;
   char  line[255];
   char  *lp;
   CString attribFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_ATTRIB_STP) );

   if ((fp = fopen(attribFile,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",attribFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   CString key;
   CString val;
   CString type;

   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line, ",", 1)) == NULL) 
         continue;

      key = lp;
      key.Trim();
      
      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      val = lp;      

      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      type = lp;
      type.Trim();

      CString item;
      item.Format("%s,%s,%s", key, val, type);
      keyValArray.SetAtGrow(keyValArrayCount++, item);
   }
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::SetKeyValFile
*
*/
void CDeviceTyperCompPropertyPage::SetKeyValFile()
{
   FILE *fp;
   CString attribFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_ATTRIB_STP) );

   if ((fp = fopen(attribFile,"w")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",attribFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }    

   for (int i=0; i<keyValArrayCount; i++)
   {
      fprintf(fp, "%s\n", keyValArray.GetAt(i));
   }

   fclose(fp);
}
/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::AddKeyValItem()
*
*/
void CDeviceTyperCompPropertyPage::AddKeyValItem(CString newKey, CString newVal, CString newType)
{
   char* tmp;
   BOOL alreadyInFile = FALSE;
   int  nIndex = -1;

   for (int i=0; i<keyValArrayCount; i++)
   {
      CString item = keyValArray.GetAt(i);

      CString key;
      CString val = " ";
      CString type;
      
      if ((tmp = get_string(STRDUP(item), ",", 1)) != NULL) 
         key = tmp;
      
      if ((tmp = get_string(NULL, ",", 1)) != NULL) 
         val = tmp;

      if ((tmp = get_string(NULL, ",", 1)) != NULL) 
         type = tmp;
   
      if (!key.CompareNoCase(newKey) && !val.CompareNoCase(newVal))
      {
         alreadyInFile = TRUE;
         nIndex = i;

         break;
      }
   }

   CString newItem;
   
   newItem.Format("%s,%s,%s", newKey, newVal, newType);

   if (alreadyInFile)
      keyValArray.SetAt(nIndex, newItem);
   else
      keyValArray.SetAtGrow(keyValArrayCount++, newItem);
}


/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::ReadDeviceTypeMasterList
*
*/
BOOL CDeviceTyperCompPropertyPage::ReadDeviceTypeMasterList()
{
   //CProgressDlg* progress = NULL;
   FILE  *fp;
   char  line[255];
   char  *lp;

   //------------------------------------------------------
   //read attrib file

   CString devicesStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_DEVICES_STP) );

   if ((fp = fopen(devicesStpFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found", devicesStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return FALSE;
   }

// progress = new CProgressDlg("Reading Device Type Master List...",FALSE);

   CWaitCursor hourglass;
   //progress->Create();
   
   Devices.DestroyAll();

   while (fgets(line,255,fp))
   {
       // progress->SetPos(0);
     //   progress->SetStatus(line);
      RestoreWaitCursor();

      DeviceTypeStruct *device = new DeviceTypeStruct; 
      Devices.Add(device);

      //device type
      if ((lp = get_string(line, ",", 1)) == NULL) 
         continue;

      if (lp[0] != ';')
      {
         device->deviceTypeName = lp;

         //min pins
         if ((lp = get_string(NULL, ",", 1)) == NULL) 
            continue;

         if (!strncmp(lp,"*",1))
         {
            device->minPinCount = -1;
         }
         else
         {
            device->minPinCount = atoi(lp);
         }

         //max pins
         if ((lp = get_string(NULL, ",", 1)) == NULL) 
            continue;

         if (!strncmp(lp,"*",1))
         {
            device->maxPinCount = -1;
         }
         else
         {
            device->maxPinCount = atoi(lp);
         }

         //expected pin names
         if ((lp = get_string(NULL, ",", 1)) == NULL) 
            continue;

         device->expPinNames = lp;
         device->expPinNames.Replace("\n", ""); // nuke newline
      }
   }

   //progress->DestroyWindow();
   //delete progress;
   //progress = NULL;
   //FlushQueue();
   fclose(fp);
   return TRUE;
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::FillKeywordValueComboList
*
*/
void CDeviceTyperCompPropertyPage::FillKeywordValueComboList()
{
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::FillPrefixGeomComboList
*
*/
void CDeviceTyperCompPropertyPage::FillPrefixGeomComboList()
{
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedBtnAutoAssign
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedBtnAutoAssign()
{
   m_onInit = FALSE;
   //CProgressDlg* kwValueProgress = NULL;
   //CProgressDlg* preGeomProgress = NULL;
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();

   UpdateFromFile();
   
   FILE  *fp, *kwValFp;
   char  line[255];
   char  *lp;

   //-----------------------------------------------------------------------
   //read prefixGeom file
   CString PrefixFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_REFDESPREFIX_STP) );

   if ((fp = fopen(PrefixFile,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",PrefixFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      if (m_radioView == 0)
         OnBnClickedRadioViewall();
      else if (m_radioView == 1)
         OnBnClickedRadioViewunassigned();
      else if (m_radioView == 2)
         OnBnClickedRadioviewviolations();

      m_CompsTree.ShowWindow(SW_SHOW);

      return;
   }

   //preGeomProgress = new CProgressDlg("Assigning by Refdes Prefix...",FALSE);

   //preGeomProgress->Create();
    
   CString pre;
   CString geom;
   CString type;
   
   while (fgets(line,255,fp))
   {
      //preGeomProgress->SetPos(0);
      //preGeomProgress->SetStatus(line);
      RestoreWaitCursor();

      if ((lp = get_string(line, ",", 1)) == NULL) 
         continue;

      pre = lp;
      pre.TrimLeft();
      pre.TrimRight();

      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      geom = lp;
      geom.TrimLeft();
      geom.TrimRight();

      if ((lp = get_string(NULL, ",", 1)) == NULL) 
         continue;

      type = lp;
      type.TrimLeft();
      type.TrimRight();

      AssignTypeByPrefix(pre, geom, type);
   }

   fclose(fp);

   CString kw;
   CString value;

   //------------------------------------------------------
   //read attrib file
   CString attribFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_ATTRIB_STP) );

   if ((kwValFp = fopen(attribFile,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",attribFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      if (m_radioView == 0)
         OnBnClickedRadioViewall();
      else if (m_radioView == 1)
         OnBnClickedRadioViewunassigned();
      else if (m_radioView == 2)
         OnBnClickedRadioviewviolations();

      m_CompsTree.ShowWindow(SW_SHOW);
   }
   else
   {
      //kwValueProgress = new CProgressDlg("Assigning by Keyword/Value...",FALSE);

      CWaitCursor hourglass;
      //kwValueProgress->Create();
       
      while (fgets(line,255,kwValFp))
      {
         //kwValueProgress->SetPos(0);
         //kwValueProgress->SetStatus(line);
         RestoreWaitCursor();

         if ((lp = get_string(line, ",", 1)) == NULL) 
            continue;

         kw = lp;
         kw.TrimLeft();
         kw.TrimRight();

         if ((lp = get_string(NULL, ",", 1)) == NULL) 
            continue;

         value = lp;
         value.TrimLeft();
         value.TrimRight();

         if ((lp = get_string(NULL, ",", 1)) == NULL) 
            continue;

         type = lp;
         type.TrimLeft();
         type.TrimRight();
         AssignTypeByKeywordValue(kw, value, type);
      }

      fclose(kwValFp);

      //kwValueProgress->DestroyWindow();
      //delete kwValueProgress;
      //kwValueProgress = NULL;
      //FlushQueue();
   }

   if (m_radioGrouping == 0)
   {
      if (m_radioView == 0)
      {
         HTREEITEM compsItem;

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data);   
                        }
                     }
                     else
                     {
                        compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);   
                     }
                  }
               }
            }
         }        
      }
      else if (m_radioView == 1)
      {
         m_CompsTree.DeleteAllItems();
         HTREEITEM compsItem;

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data);   
                        }
                     }
                     else
                     {
                        compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);   
                     }
                  }
               }
            }
         }
      }
      else if (m_radioView == 2)
      {
         m_CompsTree.DeleteAllItems();
      }
   }
   else if (m_radioGrouping == 1)
   {
   }
   else if (m_radioGrouping == 2)
   {
      m_CompsTree.ShowWindow(SW_HIDE);
      m_CompsTree.DeleteAllItems();
      UpdateData();
      HTREEITEM preItem;

      for (int i=0; i< m_cboPrefixGeom.GetCount(); i++)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(i, prefix);
         preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(preItem, (DWORD)NULL);
      }

      m_CompsTree.ShowWindow(SW_SHOW); 
   }

   m_CompsTree.ShowWindow(SW_SHOW);

   //preGeomProgress->DestroyWindow();
   //delete preGeomProgress;
   //preGeomProgress = NULL;
   //FlushQueue();

   UpdateCountsDisplay();

   if (m_radioView == 0)
        OnBnClickedRadioViewall();
   else if (m_radioView == 1)
        OnBnClickedRadioViewunassigned();
   else if (m_radioView == 2)
        OnBnClickedRadioviewviolations();
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::AssignTypeByKeywordValue
*
*/
void CDeviceTyperCompPropertyPage::AssignTypeByKeywordValue(CString keyword, CString value, CString type)
{
   WORD attribKeyword;
   Attrib* attrib = NULL;
   void* valuePtr = NULL;
   valuePtr = (void*)type.GetString();
   HTREEITEM compsItem;
   //WORD  = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               POSITION attribPos = data->getAttributesRef()->GetStartPosition();
               while (attribPos)
               {
                  CString keywordStr;
                  data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                  const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                  keywordStr = kw->cc;

                  if (!keywordStr.CompareNoCase(keyword))
                  {
                     CString valueStr;
                     WORD kwWord = doc->RegisterKeyWord(keyword, 0, kw->getValueType()); 
                     data->getAttributesRef()->Lookup(kwWord, attrib);

                     if (attrib && attrib->getValueType() == VT_STRING)
                     {
                        valueStr = attrib->getStringValue();
                     }
                     else if (attrib && attrib->getValueType() == VT_INTEGER)
                     {
                        valueStr.Format("%d", attrib->getIntValue());
                     }
                     else if (attrib && (attrib->getValueType() == VT_DOUBLE || attrib->getValueType() == VT_UNIT_DOUBLE))
                     {
                        valueStr.Format("%f", attrib->getDoubleValue());
                     }
                     else
                     {
                        continue;
                     }

                     valueStr.TrimLeft();
                     valueStr.TrimRight();
                     valueStr.Trim(" ");

                     if (!valueStr.CompareNoCase(value))
                     {
                        if (m_radioView == 0)
                        {
                           CString name = data->getInsert()->getRefname();

                           int pinCnt = 0;
                           int illegalYellow = FALSE;

                           DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                           if (!device)
                              continue;
                        
                           Attrib* attrib = NULL;

                           if (data->getAttributesRef() && !data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                              m_assignedCount++;
                           else if (!data->getAttributesRef())
                              m_assignedCount++;

                           doc->SetAttrib(&data->getAttributesRef(),tempDeviceTypeKeyword , VT_STRING, valuePtr, SA_OVERWRITE, NULL);

                           int colorDot = Devices.GetPinMapStatusIcon(data);
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname() + "-" + type, colorDot, colorDot, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data); 
                        }
                        else
                        {
                           Attrib* attrib = NULL;

                           if (data->getAttributesRef() && !data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                              m_assignedCount++;
                           else if (!data->getAttributesRef())
                              m_assignedCount++;

                           doc->SetAttrib(&data->getAttributesRef(),tempDeviceTypeKeyword , VT_STRING, valuePtr, SA_OVERWRITE, NULL);                        
                        }
                     }
                  }
               }
            }
         }
      }
   }
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::AssignTypeByPrefix
*
*/
void CDeviceTyperCompPropertyPage::AssignTypeByPrefix(CString pre, CString geomName, CString type)
{
   Attrib* attrib = NULL;
   void* value = (void*)type.GetString();
   HTREEITEM compsItem;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString name = data->getInsert()->getRefname();

               if (!name.TrimRight("0123456789._").CompareNoCase(pre))
               {
                  DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                  if (device)
                  {
                     BlockStruct* geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                     if (geomName.GetLength() > 0 && geomName.CompareNoCase(geom->getName()))
                        continue;

                     POSITION pos = geom->getDataList().GetHeadPosition();

                     if (data->getAttributesRef() && !data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                     {
                        m_assignedCount++;
                     }
                     else if (!data->getAttributesRef())
                     {
                        m_assignedCount++;
                     }

                     BOOL pinTyped = FALSE;

                     doc->SetAttrib(&data->getAttributesRef(),tempDeviceTypeKeyword , VT_STRING, value, SA_OVERWRITE, NULL);
                     
                     int colorDot = Devices.GetPinMapStatusIcon(data);
                     compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, TVI_ROOT, TVI_SORT);                    
                     m_CompsTree.SetItemData(compsItem, (DWORD)data);

                     if (colorDot != ICON_GREEN_DOT)
                     {
                        data->getAttributesRef()->deleteAttribute(deviceTypeKeyword);

                        //void* tempValue = NULL;

                        //if (data->getAttributesRef()->Lookup(deviceTypeKeyword, tempValue))
                        //{
                        //   data->getAttributesRef()->deleteKey(deviceTypeKeyword);
                        //}
                     }
                  }
               }
            }
         }
      }
   }
}


/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioViewall
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioViewall()
{
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();

   HTREEITEM compsItem;
   if (m_radioGrouping == 0)
   {
      if (m_radioView == 0)
      {
         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               // component loop
               for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
               {
                  DataStruct* data = pcbComponentIterator.getNext();

                  if (m_onInit)
                  {
                     m_compsCount++;
                  }

                  Attrib* attrib = NULL;

                  if (data->getAttributesRef())
                  {
                     if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                     {
                        compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                     }
                     else
                     {
                        CString type;
                        CString name = data->getInsert()->getRefname();

                        if (attrib && attrib->getValueType() == VT_STRING)
                        {
                           type = attrib->getStringValue();
                        }

                        DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                        if (device != NULL)
                        {
                           int colorDot = Devices.GetPinMapStatusIcon(data);
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname() + "-" + type, colorDot, colorDot, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data); 

                           if (m_onInit)
                           {
                              m_assignedCount++;
                           }
                        }

                     }
                  }
                  else
                  {
                     compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                     m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                  }
               }
            }
         }
      }
   }
   else if (m_radioGrouping == 1)
   {
      HTREEITEM kwValItem;
      int nIndex = m_cboKwValue.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboKwValue.GetLBText(nIndex, prefix);
         kwValItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(kwValItem, (DWORD)m_cboKwValue.GetItemData(nIndex));        
      }
      else
      {
         for (int i=0; i< m_cboKwValue.GetCount(); i++)
         {
            CString prefix;
            m_cboKwValue.GetLBText(i, prefix);
            kwValItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
            m_CompsTree.SetItemData(kwValItem, (DWORD)m_cboKwValue.GetItemData(i));
         }
      }
   }
   else if (m_radioGrouping == 2)
   {
      m_CompsTree.ShowWindow(SW_HIDE);
      m_CompsTree.DeleteAllItems();
      UpdateData();
      HTREEITEM preItem;
      int nIndex = m_cboPrefixGeom.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(nIndex, prefix);
         preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(preItem, (DWORD)NULL);        
      }
      else
      {
         for (int i=0; i< m_cboPrefixGeom.GetCount(); i++)
         {
            CString prefix;
            m_cboPrefixGeom.GetLBText(i, prefix);
            preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
            m_CompsTree.SetItemData(preItem, (DWORD)NULL);
         }
      }

      m_CompsTree.ShowWindow(SW_SHOW);
   }

   m_CompsTree.ShowWindow(SW_SHOW);
   m_onInit = FALSE;
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioViewunassigned
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioViewunassigned()
{
   m_onInit = FALSE;
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();

   HTREEITEM compsItem;
   //WORD  = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);

   if (m_radioGrouping == 0)
   {
      if (m_radioView == 1)
      {
         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                        }
                     }
                     else
                     {
                        compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                     }
                  }
               }
            }
         }
      }
   }
   else if (m_radioGrouping == 1)
   {
      m_CompsTree.ShowWindow(SW_HIDE);
      m_CompsTree.DeleteAllItems();
      UpdateData();
      HTREEITEM kwValItem;
      int nIndex = m_cboKwValue.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboKwValue.GetLBText(nIndex, prefix);
         kwValItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(kwValItem, (DWORD)m_cboKwValue.GetItemData(nIndex));        
      }
      else
      {
         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  WORD attribKeyword;
                  Attrib* attrib = NULL;
                  CString keywordStr;
                  POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                  while (attribPos)
                  {
                     data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                     const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                     keywordStr = kw->cc;

                     if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                     {
                        m_CompsTree.DeleteItem(keywordStr);

                        HTREEITEM kwValItem = m_CompsTree.InsertItem(keywordStr, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(kwValItem, (DWORD)kw);
                     }
                  }
               }
            }
         }
      }

      m_CompsTree.ShowWindow(SW_SHOW);
   }
   else if (m_radioGrouping == 2)
   {
      m_CompsTree.ShowWindow(SW_HIDE);
      m_CompsTree.DeleteAllItems();
      UpdateData();
      HTREEITEM preItem;
      int nIndex = m_cboPrefixGeom.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(nIndex, prefix);
         preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(preItem, (DWORD)NULL);        
      }
      else
      {
         for (int j=0; j< m_cboPrefixGeom.GetCount(); j++)
         {
            CString prefix;
            m_cboPrefixGeom.GetLBText(j, prefix);

            for (int i=0; i<doc->getMaxBlockIndex(); i++)
            {
               BlockStruct *block = doc->getBlockAt(i);

               if (block && (block->getFileNumber() == m_activeFileNumber))
               {
                  POSITION dataPos = block->getDataList().GetHeadPosition();
                  while (dataPos)
                  {
                     DataStruct *data = block->getDataList().GetNext(dataPos);

                     if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                     {
                        CString currentPre = data->getInsert()->getRefname();
                           
                        int index = currentPre.FindOneOf(".0123456789_");

                        if (index >0)
                           currentPre = currentPre.Left(index);

                        if (currentPre.CompareNoCase(prefix))
                           continue;

                        Attrib* attrib = NULL;

                        if (data->getAttributesRef())
                        {
                           if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                           {
                              m_CompsTree.DeleteItem(prefix);

                              preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                              m_CompsTree.SetItemData(preItem, (DWORD)NULL);  
                           }
                        }
                        else
                        {
                           m_CompsTree.DeleteItem(prefix);

                           preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(preItem, (DWORD)NULL);
                        }
                     }
                  }
               }
            }
         }
      }

      m_CompsTree.ShowWindow(SW_SHOW);
   }

   m_CompsTree.ShowWindow(SW_SHOW);
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioviewviolations
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioviewviolations()
{
   m_onInit = FALSE;
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();

   if (m_radioGrouping == 0)
   {
      if (m_radioView == 2)
      {
         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     Attrib* attrib = NULL;
                     data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib);
                     CString type;
                     CString name = data->getInsert()->getRefname();

                     if (attrib && attrib->getValueType() == VT_STRING)
                     {
                        type = attrib->getStringValue();
                     }

                     DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                     if (!device)
                        continue;

                     int colorDot = Devices.GetPinMapStatusIcon(data);

                     if (colorDot == ICON_RED_DOT || colorDot == ICON_YELLOW_DOT)
                     {
                        HTREEITEM compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);
                     }

                  }
               }
            }
         }
      }
   }
   else if (m_radioGrouping == 1)
   {
      HTREEITEM kwValItem;
      int nIndex = m_cboKwValue.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboKwValue.GetLBText(nIndex, prefix);
         kwValItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(kwValItem, (DWORD)m_cboKwValue.GetItemData(nIndex));        
      }
      else
      {
         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  WORD attribKeyword;
                  Attrib* attrib = NULL;
                  CString keywordStr;
                  POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                  while (attribPos)
                  {
                     data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                     const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                     keywordStr = kw->cc;

                     if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        continue;

                     CString type;
                     CString name = data->getInsert()->getRefname();

                     if (attrib && attrib->getValueType() == VT_STRING)
                     {
                        type = attrib->getStringValue();
                     }

                     DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                     if (!device)
                        continue;

                     PinmapViolationTag vio = Devices.GetPinMapViolation(data);

                     if (vio == pinmapYellowViolation || vio == pinmapRedViolation)
                     {
                        m_CompsTree.DeleteItem(keywordStr);

                        HTREEITEM kwValItem = m_CompsTree.InsertItem(keywordStr, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(kwValItem, (DWORD)kw);
                     }

                  }
               }
            }
         }
      }
   }
   else if (m_radioGrouping == 2)
   {
      HTREEITEM preItem;
      int nIndex = m_cboPrefixGeom.GetCurSel();

      if (nIndex != CB_ERR)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(nIndex, prefix);
         preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
         m_CompsTree.SetItemData(preItem, (DWORD)NULL);        
      }
      else
      {
         for (int j=0; j< m_cboPrefixGeom.GetCount(); j++)
         {
            CString prefix;
            m_cboPrefixGeom.GetLBText(j, prefix);

            for (int j=0; j<doc->getMaxBlockIndex(); j++)
            {
               BlockStruct *block = doc->getBlockAt(j);

               if (!block)
                  continue;

               if (block->getFileNumber() != m_activeFileNumber)
                  continue;

               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     CString currentPre = data->getInsert()->getRefname();
                       
                     int index = currentPre.FindOneOf(".0123456789_");

                     if (index >0)
                        currentPre = currentPre.Left(index);

                     if (currentPre.CompareNoCase(prefix))
                        continue;

                     Attrib* attrib = NULL;

                     if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        continue;

                     CString type;
                     CString name = data->getInsert()->getRefname();

                     if (attrib && attrib->getValueType() == VT_STRING)
                     {
                        type = attrib->getStringValue();
                     }

                     DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                     if (!device)
                        continue;

                     PinmapViolationTag vio = Devices.GetPinMapViolation(data);

                     if (vio == pinmapYellowViolation || vio == pinmapRedViolation)
                     {
                        m_CompsTree.DeleteItem(prefix);

                        HTREEITEM preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(preItem, (DWORD)NULL);
                     }
                  }
               }
            }
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);
}


/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioFlat
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioFlat()
{
   m_addToStpFile.EnableWindow(FALSE);
   m_onInit = FALSE;
   m_cboPrefixGeom.EnableWindow(FALSE);
   m_cboKwValue.EnableWindow(FALSE);
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();
   HTREEITEM compsItem;
   //WORD  = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);

   if (m_radioGrouping == 0)
   {
      if (m_radioView == 2)
      {
         OnBnClickedRadioviewviolations();

         return;
      }

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block && (block->getFileNumber() == m_activeFileNumber))
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  if (m_radioView == 0 || m_radioView == 1)
                  {
                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                        }
                        else if (m_radioView == 0)
                        {
                           CString type;
                           CString name = data->getInsert()->getRefname();

                           if (attrib && attrib->getValueType() == VT_STRING)
                           {
                              type = attrib->getStringValue();
                           }

                           int pinCnt = 0;
                           int illegalYellow = FALSE;
                           BOOL pinTyped = FALSE;
                           DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                           if (!device)
                              continue;

                           BlockStruct* geom = doc->getBlockAt(data->getInsert()->getBlockNumber());
                           POSITION pos = geom->getDataList().GetHeadPosition();

                           while (pos)
                           {
                              DataStruct* pin = geom->getDataList().GetNext(pos);

                              if (!pin)
                                 continue;

                              if (!pin->getInsert() || pin->getInsert()->getInsertType() != INSERTTYPE_PIN)
                                 continue;

                              if (IsDataPinTyped(data, pin))
                                 pinTyped = TRUE;

                              if (device && device->expPinNames.GetLength() > 0)
                              {
                                 illegalYellow = TRUE;
                                 CString pinName = pin->getInsert()->getRefname();
                                 int curPos = 0;
                                 CString token = device->expPinNames.Tokenize(" ",curPos);

                                 while (!token.IsEmpty())
                                 {
                                    if (!token.CompareNoCase(pinName))
                                       illegalYellow = FALSE;

                                    token = device->expPinNames.Tokenize(" ", curPos);
                                 }
                              }

                              if (pin->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                                 pinCnt++;
                           }

                           int colorDot = Devices.GetPinMapStatusIcon(data);
                           compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, TVI_ROOT, TVI_SORT);                     
                           m_CompsTree.SetItemData(compsItem, (DWORD)data);
                        }
                     }
                     else
                     {
                        compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(compsItem, (DWORD)data);         
                     }
                  }
               }
            }
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioKwvalue
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioKwvalue()
{
   m_addToStpFile.EnableWindow(TRUE);
   m_onInit = FALSE;
   m_cboKwValue.EnableWindow(TRUE);
   m_cboPrefixGeom.EnableWindow(FALSE);
   m_cboPrefixGeom.SetWindowText("");
   m_cboPrefixGeom.SetCurSel(-1);
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();

   if (m_radioGrouping == 1)
   {
      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct* block = doc->getBlockAt(i);

         if (block && (block->getFileNumber() == m_activeFileNumber))
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct* data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  CString keywordStr;
                  WORD attribKeyword;
                  Attrib* attrib = NULL;                  

                  for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
                  {
                     data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);

                     const KeyWordStruct* keyword = doc->getKeyWordArray()[attribKeyword];
                     keywordStr = keyword->cc;
                     int nIndex = 0;

                     while ((nIndex = m_cboKwValue.FindString(nIndex, keywordStr)) != CB_ERR)
                     {
                        m_cboKwValue.DeleteString( nIndex );
                     }

                     m_cboKwValue.SetItemData(m_cboKwValue.AddString(keywordStr), (DWORD)keyword);
                  }
               }
            }
         }
      }

      if (m_radioView == 0)
      {
         m_CompsTree.ShowWindow(SW_HIDE);
         m_CompsTree.DeleteAllItems();
         UpdateData();
         HTREEITEM kwValItem;

         for (int i=0; i< m_cboKwValue.GetCount(); i++)
         {
            CString attKw;
            m_cboKwValue.GetLBText(i, attKw);
            kwValItem = m_CompsTree.InsertItem(attKw, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
            m_CompsTree.SetItemData(kwValItem, (DWORD)m_cboKwValue.GetItemData(i));
         }

         m_CompsTree.ShowWindow(SW_SHOW);       
      }
      else if (m_radioView == 1)
      {
         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     WORD attribKeyword;
                     Attrib* attrib = NULL;
                     CString keywordStr;
                     POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                     while (attribPos)
                     {
                        data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                        const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                        keywordStr = kw->cc;

                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           m_CompsTree.DeleteItem(keywordStr);

                           HTREEITEM kwValItem = m_CompsTree.InsertItem(keywordStr, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(kwValItem, (DWORD)kw);
                        }
                     }
                  }
               }
            }
         }
      }
      else if (m_radioView == 2)
      {
         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     WORD attribKeyword;
                     Attrib* attrib = NULL;
                     CString keywordStr;
                     POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                     while (attribPos)
                     {
                        data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                        const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                        keywordStr = kw->cc;

                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                           continue;

                        CString type;
                        CString name = data->getInsert()->getRefname();

                        if (attrib && attrib->getValueType() == VT_STRING)
                        {
                           type = attrib->getStringValue();
                        }

                        int pinCnt = 0;
                        BOOL illegalYellow = FALSE;
                        DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                        if (!device)
                           continue;

                        BlockStruct* tempGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                        POSITION pos = tempGeom->getDataList().GetHeadPosition();
                        while (pos)
                        {
                           DataStruct* tempData = tempGeom->getDataList().GetNext(pos);

                           if (!tempData)
                              continue;

                           if (!tempData->getInsert() || tempData->getInsert()->getInsertType() != INSERTTYPE_PIN)
                              continue;

                           if (device && device->expPinNames.GetLength() > 0)
                           {
                              illegalYellow = TRUE;
                              CString pinName = tempData->getInsert()->getRefname();
                              int curPos = 0;
                              CString token = device->expPinNames.Tokenize(" ",curPos);

                              while (!token.IsEmpty())
                              {
                                 if (!token.CompareNoCase(pinName))
                                    illegalYellow = FALSE;

                                 token = device->expPinNames.Tokenize(" ", curPos);
                              }
                           }

                           if (tempData->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                              pinCnt++;
                        }

                        HTREEITEM kwValItem;

                        if (illegalYellow && pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
                        {
                           m_CompsTree.DeleteItem(keywordStr);

                           kwValItem = m_CompsTree.InsertItem(keywordStr, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(kwValItem, (DWORD)kw);

                           continue;
                        }

                        if (!(pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1)))
                        {
                           m_CompsTree.DeleteItem(keywordStr);

                           kwValItem = m_CompsTree.InsertItem(keywordStr, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(kwValItem, (DWORD)kw);
                        }
                     }
                  }
               }
            }
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);
}

/*****************************************************************************
*
*CDeviceTyperCompPropertyPage::OnBnClickedRadioPrefixgeom
*
*/
void CDeviceTyperCompPropertyPage::OnBnClickedRadioPrefixgeom()
{
   m_addToStpFile.EnableWindow(TRUE);
   m_onInit = FALSE;
   m_cboPrefixGeom.EnableWindow(TRUE);
   m_cboKwValue.EnableWindow(FALSE);
   m_cboKwValue.SetWindowText("");
   m_cboKwValue.SetCurSel(-1);
   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.SelectItem(NULL);
   m_CompsTree.SelectDropTarget(NULL);
   m_CompsTree.DeleteAllItems();
   doc->UnselectAll(FALSE);
   UpdateData();
   HTREEITEM preItem;

   if (m_radioView == 0)
   {
      if (m_radioGrouping == 2)
      {
         for (int i=0; i< prefixesCnt; i++)
         {
            CString prefix = prefixes.GetAt(i);
            preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
            m_CompsTree.SetItemData(preItem, (DWORD)NULL);
         }
      }
   }
   else if (m_radioView == 1)
   {
      for (int j=0; j< m_cboPrefixGeom.GetCount(); j++)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(j, prefix);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     CString currentPre = data->getInsert()->getRefname();

                     int index = currentPre.FindOneOf(".0123456789_");

                     if (index >0)
                        currentPre = currentPre.Left(index);

                     if (currentPre.CompareNoCase(prefix))
                        continue;

                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           m_CompsTree.DeleteItem(prefix);

                           preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                           m_CompsTree.SetItemData(preItem, (DWORD)NULL);  
                        }
                     }
                     else
                     {
                        m_CompsTree.DeleteItem(prefix);

                        preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                        m_CompsTree.SetItemData(preItem, (DWORD)NULL);
                     }
                  }
               }
            }
         }
      }
   }
   else if (m_radioView == 2)
   {
      for (int j=0; j< m_cboPrefixGeom.GetCount(); j++)
      {
         CString prefix;
         m_cboPrefixGeom.GetLBText(j, prefix);

         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  CString currentPre = data->getInsert()->getRefname();
                  
                  if (currentPre.FindOneOf("f") != -1)
                     currentPre = currentPre;                   

                  int index = currentPre.FindOneOf(".0123456789_");

                  if (index >0)
                     currentPre = currentPre.Left(index);

                  if (currentPre.CompareNoCase(prefix))
                     continue;

                  Attrib* attrib = NULL;

                  if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                     continue;

                  CString type;
                  CString name = data->getInsert()->getRefname();

                  if (attrib && attrib->getValueType() == VT_STRING)
                  {
                     type = attrib->getStringValue();
                  }

                  int pinCnt = 0;
                  BOOL illegalYellow = FALSE;
                  DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                  if (!device)
                     continue;

                  BlockStruct* tempGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                  POSITION pos = tempGeom->getDataList().GetHeadPosition();
                  while (pos)
                  {
                     DataStruct* tempData = tempGeom->getDataList().GetNext(pos);

                     if (!tempData)
                        continue;

                     if (!tempData->getInsert() || tempData->getInsert()->getInsertType() != INSERTTYPE_PIN)
                        continue;

                     if (device && device->expPinNames.GetLength() > 0)
                     {
                        illegalYellow = TRUE;
                        CString pinName = tempData->getInsert()->getRefname();
                        int curPos = 0;
                        CString token = device->expPinNames.Tokenize(" ",curPos);

                        while (!token.IsEmpty())
                        {
                           if (!token.CompareNoCase(pinName))
                              illegalYellow = FALSE;

                           token = device->expPinNames.Tokenize(" ", curPos);
                        }
                     }

                     if (tempData->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                        pinCnt++;
                  }

                  HTREEITEM preItem;

                  if (illegalYellow && pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
                  {
                     m_CompsTree.DeleteItem(prefix);

                     preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                     m_CompsTree.SetItemData(preItem, (DWORD)NULL);

                     continue;
                  }

                  if (!(pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1)))
                  {
                     m_CompsTree.DeleteItem(prefix);

                     preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                     m_CompsTree.SetItemData(preItem, (DWORD)NULL);
                  }
               }
            }
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonFind
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonFind() 
{
   m_onInit = FALSE;
   CString comp;   
   m_compToFind.GetWindowText(comp);

   if (m_radioGrouping == 2)
   {
      CString geomName;
      CString pre = comp.Left(2);
      pre.Trim("0123456789._");

      for (int i=0; i<doc->getMaxBlockIndex() && (geomName.GetLength() == 0); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block && (block->getFileNumber() == m_activeFileNumber))
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  if (!data->getInsert()->getRefname().Left(comp.GetLength()).CompareNoCase(comp))
                  {
                     if (doc->getBlockAt(data->getInsert()->getBlockNumber()))
                        geomName = doc->getBlockAt(data->getInsert()->getBlockNumber())->getName();

                     break;
                  }
               }
            }
         }
      }

      HTREEITEM root = m_CompsTree.GetRootItem();

      while (root)
      {
         if (!m_CompsTree.GetItemText(root).CompareNoCase(pre))
         {
            m_CompsTree.SelectItem(root);
            HTREEITEM subRoot = m_CompsTree.GetChildItem(root);

            while (subRoot)
            {
               if (!m_CompsTree.GetItemText(subRoot).CompareNoCase(geomName))
               {
                  m_CompsTree.SelectItem(subRoot);
                  HTREEITEM subSubRoot = m_CompsTree.GetChildItem(subRoot);

                  while (subSubRoot)
                  {
                     if (!m_CompsTree.GetItemText(subSubRoot).Left(comp.GetLength()).CompareNoCase(comp))
                     {
                        m_CompsTree.SelectItem(subSubRoot);
                        m_CompsTree.Select(subSubRoot, TVGN_DROPHILITE);

                        return;
                     }

                     subSubRoot = m_CompsTree.GetNextSiblingItem(subSubRoot);
                  }
               }

               subRoot = m_CompsTree.GetNextSiblingItem(subRoot);
            }
         }

         root = m_CompsTree.GetNextSiblingItem(root);
      }
   }
   else if (m_radioGrouping == 1)
   {
      HTREEITEM root = m_CompsTree.GetRootItem();

      while (root)
      {
         KeyWordStruct* kw = (KeyWordStruct*)m_CompsTree.GetItemData(root);

         if (!kw)
            continue;

         WORD key = doc->RegisterKeyWord(kw->cc, 0, kw->getValueType());
         Attrib* attrib = NULL;

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
                  !data->getInsert()->getRefname().Left(comp.GetLength()).CompareNoCase(comp) && data->getAttributesRef()->Lookup(key, attrib))
               {
                  CString valStr;

                  if (attrib && attrib->getValueType() == VT_STRING)
                  {
                     valStr = attrib->getStringValue();
                  }
                  else if (attrib && attrib->getValueType() == VT_INTEGER)
                  {
                     valStr.Format("%d", attrib->getIntValue());
                  }
                  else if (attrib && (attrib->getValueType() == VT_DOUBLE || attrib->getValueType() == VT_UNIT_DOUBLE))
                  {
                     valStr.Format("%f", attrib->getDoubleValue());
                  }
                  else
                  {
                     continue;
                  }

                  m_CompsTree.SelectItem(root);
                  HTREEITEM subRoot = m_CompsTree.GetChildItem(root);

                  while (subRoot)
                  {
                     if (!m_CompsTree.GetItemText(subRoot).CompareNoCase(valStr))
                     {
                        m_CompsTree.SelectItem(subRoot);
                        HTREEITEM subSubRoot = m_CompsTree.GetChildItem(subRoot);

                        while (subSubRoot)
                        {
                           if (!m_CompsTree.GetItemText(subSubRoot).Left(comp.GetLength()).CompareNoCase(comp))
                           {
                              m_CompsTree.SelectItem(subSubRoot);
                              m_CompsTree.Select(subSubRoot, TVGN_DROPHILITE);

                              return;
                           }

                           subSubRoot = m_CompsTree.GetNextSiblingItem(subSubRoot);
                        }
                     }

                     subRoot = m_CompsTree.GetNextSiblingItem(subRoot);
                  }
               }
            }
         }

         root = m_CompsTree.GetNextSiblingItem(root);
      }
   }
   else
   {
      HTREEITEM root = m_CompsTree.GetRootItem();

      while (root)
      {
         if (!m_CompsTree.GetItemText(root).Left(comp.GetLength()).CompareNoCase(comp))
         {
            m_CompsTree.SelectItem(root);
            m_CompsTree.Select(root, TVGN_DROPHILITE);

            return;
         }

         HTREEITEM subRoot = m_CompsTree.GetChildItem(root);

         while (subRoot)
         {
            if (!m_CompsTree.GetItemText(subRoot).Left(comp.GetLength()).CompareNoCase(comp))
            {
               m_CompsTree.SelectItem(subRoot);
               m_CompsTree.Select(subRoot, TVGN_DROPHILITE);

               return;
            }

            HTREEITEM subSubRoot = m_CompsTree.GetChildItem(subRoot);

            while (subSubRoot)
            {
               if (!m_CompsTree.GetItemText(subSubRoot).Left(comp.GetLength()).CompareNoCase(comp))
               {
                  m_CompsTree.SelectItem(subSubRoot);
                  m_CompsTree.Select(subSubRoot, TVGN_DROPHILITE);

                  return;
               }
            }

            subRoot = m_CompsTree.GetNextSiblingItem(subRoot);
         }

         root = m_CompsTree.GetNextSiblingItem(root);
      }
   }
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnTvnSelchangedCompsTree
*/
void CDeviceTyperCompPropertyPage::OnTvnSelchangedCompsTree(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   m_onInit = FALSE;
   HTREEITEM item = m_CompsTree.GetSelectedItem();

   if (!item)
      return;

   m_CompsTree.SelectItem(item);
   m_CompsTree.Select(item, TVGN_DROPHILITE);
   m_CompsTree.ShowWindow(SW_HIDE);

   BlockStruct *geom = ItemIsGeom(item);
   DataStruct *compData = ItemIsComp(item);
   HTREEITEM preItem, geomItem, compsItem;
   CString itemText = m_CompsTree.GetItemText(item);

   if (!compData)
        OnBnClickedButton1();
   
   if (m_radioGrouping == 0) //if flat grouping
   {
      if (compData)
      {
         BlockStruct* tempGeom = doc->getBlockAt(compData->getInsert()->getBlockNumber());
         int pinCnt = 0;

         if (tempGeom)
         {
            POSITION dataPos = tempGeom->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct* data = tempGeom->getDataList().GetNext(dataPos);

               if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  if (data->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                     pinCnt++;
               }
            }

            for(;m_ListDevices.DeleteString(0) != LB_ERR;)
            {
            }

            for (int i=0; i<Devices.GetCount(); i++)
            {
               DeviceTypeStruct* device = Devices[i];

               if (!device)
                  continue;

               if (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
               {
                  m_ListDevices.AddString(device->deviceTypeName);
               }
            }
         }

         doc->OnDoneEditing();
         compsItem = item;
         double xmin, xmax, ymin, ymax;

         if (compData->getInsert() && compData->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            if (!activeView)
               return;

            UpdateData();

            CString m_ref = compData->getInsert()->getRefname();

            if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");

               return;
            }
               
            double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;
            double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;

            double marginSize = max(marginSizeX, marginSizeY);

            xmin -= marginSize;
            xmax += marginSize;
               
            ymin -= marginSize;
            ymax += marginSize;

            ((CCEtoODBView*)activeView)->ZoomBox(xmin, xmax, ymin, ymax);
         }
      }
   }
   else if (m_radioGrouping == 1) //if by keyword/value
   {
      //if keyword is selected
      if (!m_CompsTree.GetParentItem(item))
      {
         HTREEITEM kwValItem = item;
         CString attKw = m_CompsTree.GetItemText(kwValItem);
         CString keywordStr;
         WORD attribKeyword;
         Attrib* attrib = NULL;

         for (int j=0; j<doc->getMaxBlockIndex(); j++)
         {
            BlockStruct *block = doc->getBlockAt(j);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  HTREEITEM valItem;
                  CString value;
                  int attribIntVal;
                  double attribDblVal;
                  POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                  while (attribPos)
                  {
                     data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                     const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                     keywordStr = kw->cc;

                     if (!keywordStr.CompareNoCase(attKw))
                     {
                        WORD keyword = doc->RegisterKeyWord(attKw, 0, kw->getValueType()); 
                        data->getAttributesRef()->Lookup(keyword, attrib);

                        if (attrib && attrib->getValueType() == VT_STRING)
                        {
                           value = attrib->getStringValue();
                        }
                        else if (attrib && attrib->getValueType() == VT_INTEGER)
                        {
                           attribIntVal = attrib->getIntValue();
                           value.Format("%d", attribIntVal);
                        }
                        else if (attrib && (attrib->getValueType() == VT_DOUBLE || attrib->getValueType() == VT_UNIT_DOUBLE))
                        {
                           attribDblVal = attrib->getDoubleValue();
                           //int decimals = GetDecimals(doc->Settings.PageUnits);
                           value.Format("%f", attribDblVal);
                        }
                        else
                        {
                           continue;
                        }

                        
                        value.TrimLeft();
                        value.TrimRight();
                        value.Trim(" ");

                        HTREEITEM valRoot = m_CompsTree.GetChildItem(kwValItem);

                        if (m_radioView == 0)
                        {
                           m_CompsTree.DeleteItem(valRoot, value);

                           valItem = m_CompsTree.InsertItem(value, ICON_FOLDER, ICON_OPENFOLDER, kwValItem, TVI_SORT);
                           m_CompsTree.SetItemData(valItem, (DWORD)NULL);
                        }
                        else if (m_radioView == 1)
                        {
                           if(!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                           {
                              m_CompsTree.DeleteItem(valRoot, value);

                              valItem = m_CompsTree.InsertItem(value, ICON_FOLDER, ICON_OPENFOLDER, kwValItem, TVI_SORT);
                              m_CompsTree.SetItemData(valItem, (DWORD)NULL);
                           }
                        }
                        else if (m_radioView == 2)
                        {
                           if (data->getAttributesRef())
                           {
                              Attrib* attrib = NULL;

                              if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                                 continue;

                              CString type;
                              CString name = data->getInsert()->getRefname();

                              if (attrib && attrib->getValueType() == VT_STRING)
                              {
                                 type = attrib->getStringValue();
                              }

                              int pinCnt = 0;
                              BOOL illegalYellow = FALSE;
                              DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                              if (!device)
                                 continue;

                              BlockStruct* tempGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                              POSITION pos = tempGeom->getDataList().GetHeadPosition();
                              while (pos)
                              {
                                 DataStruct* tempData = tempGeom->getDataList().GetNext(pos);

                                 if (!tempData)
                                    continue;

                                 if (!tempData->getInsert() || tempData->getInsert()->getInsertType() != INSERTTYPE_PIN)
                                    continue;

                                 if (device && device->expPinNames.GetLength() > 0)
                                 {
                                    illegalYellow = TRUE;
                                    CString pinName = tempData->getInsert()->getRefname();
                                    int curPos = 0;
                                    CString token = device->expPinNames.Tokenize(" ",curPos);

                                    while (!token.IsEmpty())
                                    {
                                       if (!token.CompareNoCase(pinName))
                                          illegalYellow = FALSE;

                                       token = device->expPinNames.Tokenize(" ", curPos);
                                    }
                                 }

                                 if (tempData->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                                    pinCnt++;
                              }

                              valRoot = m_CompsTree.GetChildItem(item);

                              if (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
                              {
                                 if (illegalYellow)
                                 {
                                    m_CompsTree.DeleteItem(valRoot, value);

                                    valItem = m_CompsTree.InsertItem(value, ICON_FOLDER, ICON_OPENFOLDER, kwValItem, TVI_SORT);
                                    m_CompsTree.SetItemData(valItem, (DWORD)NULL);
                                 }

                                 continue;
                              }

                              m_CompsTree.DeleteItem(valRoot, value);

                              valItem = m_CompsTree.InsertItem(value, ICON_FOLDER, ICON_OPENFOLDER, kwValItem, TVI_SORT);
                              m_CompsTree.SetItemData(valItem, (DWORD)NULL);
                           }
                        }

                        break;
                     }
                  }
               }
            }
         }
      }
      else if (!m_CompsTree.GetParentItem(m_CompsTree.GetParentItem(item)))//if value is selected
      {
         WORD attribKeyword;
         Attrib* attrib = NULL;
         CString keyword = m_CompsTree.GetItemText(m_CompsTree.GetParentItem(item));
         CString value = m_CompsTree.GetItemText(item);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct *data = block->getDataList().GetNext(dataPos);

               if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
               {
                  CString keywordStr;
                  CString valueStr;
                  int attribIntVal;
                  double attribDblVal;
                  POSITION attribPos = data->getAttributesRef()->GetStartPosition();

                  while (attribPos)
                  {
                     data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                     const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                     keywordStr = kw->cc;

                     if (!keywordStr.CompareNoCase(keyword))
                     {
                        WORD kwWord = doc->RegisterKeyWord(keyword, 0, kw->getValueType()); 
                        data->getAttributesRef()->Lookup(kwWord, attrib);

                        if (attrib && attrib->getValueType() == VT_STRING)
                        {
                           valueStr = attrib->getStringValue();
                        }
                        else if (attrib && attrib->getValueType() == VT_INTEGER)
                        {
                           attribIntVal = attrib->getIntValue();
                           valueStr.Format("%d", attribIntVal);
                        }
                        else if (attrib && (attrib->getValueType() == VT_DOUBLE || attrib->getValueType() == VT_UNIT_DOUBLE))
                        {
                           attribDblVal = attrib->getDoubleValue();
                           //int decimals = GetDecimals(doc->Settings.PageUnits);
                           valueStr.Format("%f", attribDblVal);
                        }
                        else
                        {
                           continue;
                        }

                        valueStr.TrimLeft();
                        valueStr.TrimRight();
                        valueStr.Trim(" ");

                        if (!value.CompareNoCase(valueStr))
                        {
                           if (m_radioView == 0 || m_radioView == 1)
                           {
                              void* voidPtr = NULL;
                              HTREEITEM subRoot = m_CompsTree.GetChildItem(item);

                              while (subRoot)
                              {
                                 CString compName;
                                 int nIndex = m_CompsTree.GetItemText(subRoot).Find("-",0);

                                 if (nIndex > 0)
                                    compName = m_CompsTree.GetItemText(subRoot).Left(nIndex);
                                 else
                                    compName = m_CompsTree.GetItemText(subRoot);

                                 if (!compName.CompareNoCase(data->getInsert()->getRefname()))
                                 {
                                    m_CompsTree.DeleteItem(subRoot); 

                                    break;
                                 }

                                 subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                              }

                              if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                              {
                                 compsItem = m_CompsTree.InsertItem(data->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, item, TVI_SORT);
                                 m_CompsTree.SetItemData(compsItem, (DWORD)data);                           
                              }
                              else if (m_radioView == 0)
                              {
                                 CString type;
                                 CString name = data->getInsert()->getRefname();
                                 Attrib *attrib = (Attrib*)voidPtr;

                                 if (attrib && attrib->getValueType() == VT_STRING)
                                 {
                                    type = attrib->getStringValue();
                                 }

                                 DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                                 if (!device)
                                    continue;

                                 BlockStruct* geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                                 int colorDot = Devices.GetPinMapStatusIcon(data);
                                 compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, item, TVI_SORT);                   
                                 m_CompsTree.SetItemData(compsItem, (DWORD)data);

                              }

                              break;
                           }
                           else if (m_radioView == 2)
                           {
                              if (data->getAttributesRef())
                              {
                                 void* voidPtr = NULL;

                                 if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                                    continue;

                                 CString type;
                                 CString name = data->getInsert()->getRefname();
                                 Attrib *attrib = (Attrib*)voidPtr;

                                 if (attrib && attrib->getValueType() == VT_STRING)
                                 {
                                    type = attrib->getStringValue();
                                 }

                                 DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                                 if (!device)
                                    continue;

                                 BlockStruct* tempGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                                 int colorDot = Devices.GetPinMapStatusIcon(data);
                                 {
                                    HTREEITEM subRoot = m_CompsTree.GetChildItem(item);

                                    while (subRoot)
                                    {
                                       CString compName;
                                       int nIndex = m_CompsTree.GetItemText(subRoot).Find("-",0);

                                       if (nIndex > 0)
                                          compName = m_CompsTree.GetItemText(subRoot).Left(nIndex);
                                       else
                                          compName = m_CompsTree.GetItemText(subRoot);

                                       if (!compName.CompareNoCase(name))
                                       {
                                          m_CompsTree.DeleteItem(subRoot);  

                                          break;
                                       }

                                       subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                                    }

                                    compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, item, TVI_SORT);                   
                                    m_CompsTree.SetItemData(compsItem, (DWORD)data);
                                 }

                              }

                              break;
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      else if (compData)
      {
         BlockStruct* tempGeom = doc->getBlockAt(compData->getInsert()->getBlockNumber());
         int pinCnt = 0;

         if (tempGeom)
         {
            POSITION dataPos = tempGeom->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct* data = tempGeom->getDataList().GetNext(dataPos);

               if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  if (data->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                     pinCnt++;
               }
            }

            for(;m_ListDevices.DeleteString(0) != LB_ERR;)
            {
            }

            for (int i=0; i<Devices.GetCount(); i++)
            {
               DeviceTypeStruct* device = Devices[i];

               if (!device)
                  continue;

               if (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
               {
                  m_ListDevices.AddString(device->deviceTypeName);
               }
            }
         }

         doc->OnDoneEditing();
         compsItem = item;
         double xmin, xmax, ymin, ymax;

         if (compData->getInsert() && compData->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            if (!activeView)
               return;

            UpdateData();

            CString m_ref = compData->getInsert()->getRefname();

            if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");

               return;
            }
               
            double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;
            double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;

            double marginSize = max(marginSizeX, marginSizeY);

            xmin -= marginSize;
            xmax += marginSize;
               
            ymin -= marginSize;
            ymax += marginSize; 

            ((CCEtoODBView*)activeView)->ZoomBox(xmin, xmax, ymin, ymax);
         }
      }
   }
   else if (m_radioGrouping == 2) //if by prefix/Geom
   {
      //if item has no parent, then it is a prefix
      if (!m_CompsTree.GetParentItem(item))
      {
         doc->OnDoneEditing();

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block && (block->getFileNumber() == m_activeFileNumber))
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
                  {
                     CString currentPre = data->getInsert()->getRefname();
                     
                     int index = currentPre.FindOneOf(".0123456789_");

                     if (index >0)
                        currentPre = currentPre.Left(index);

                     if (currentPre.CompareNoCase(itemText))
                        continue;

                     BlockStruct *geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                     if (geom)
                     {
                        if (m_radioView == 0)
                        {

                           HTREEITEM subRoot = m_CompsTree.GetChildItem(item);
                           m_CompsTree.DeleteItem(subRoot, geom->getName());

                           geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, item, TVI_SORT);
                           m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                        }
                        else if (m_radioView == 1)
                        {
                           Attrib* attrib = NULL;

                           if (data->getAttributesRef())
                           {
                              if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                              {
                                 HTREEITEM subRoot = m_CompsTree.GetChildItem(item);
                                 m_CompsTree.DeleteItem(subRoot, geom->getName());

                                 geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, item, TVI_SORT);
                                 m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                              }
                           }
                           else
                           {
                              HTREEITEM subRoot = m_CompsTree.GetChildItem(item);

                              while (subRoot)
                              {
                                 if (!m_CompsTree.GetItemText(subRoot).CompareNoCase(geom->getName()))
                                 {
                                    m_CompsTree.DeleteItem(subRoot);    

                                    break;
                                 }

                                 subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                              }

                              geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, item, TVI_SORT);
                              m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                           }
                        }
                        else if (m_radioView == 2)
                        {
                           if (data->getAttributesRef())
                           {
                              Attrib* attrib = NULL;

                              if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                                 continue;

                              CString type;
                              CString name = data->getInsert()->getRefname();

                              if (attrib && attrib->getValueType() == VT_STRING)
                              {
                                 type = attrib->getStringValue();
                              }

                              int pinCnt = 0;
                              int illegalYellow = FALSE;
                              DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                              if (!device)
                                 continue;

                              BlockStruct* tempGeom = doc->getBlockAt(data->getInsert()->getBlockNumber());

                              POSITION pos = geom->getDataList().GetHeadPosition();
                              while (pos)
                              {
                                 DataStruct* tempData = tempGeom->getDataList().GetNext(pos);

                                 if (!tempData)
                                    continue;

                                 if (!tempData->getInsert() || tempData->getInsert()->getInsertType() != INSERTTYPE_PIN)
                                    continue;

                                 if (device && device->expPinNames.GetLength() > 0)
                                 {
                                    illegalYellow = TRUE;
                                    CString pinName = tempData->getInsert()->getRefname();
                                    int curPos = 0;
                                    CString token = device->expPinNames.Tokenize(" ",curPos);

                                    while (!token.IsEmpty())
                                    {
                                       if (!token.CompareNoCase(pinName))
                                          illegalYellow = FALSE;

                                       token = device->expPinNames.Tokenize(" ", curPos);
                                    }
                                 }

                                 if (tempData->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                                    pinCnt++;
                              }

                              if (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
                              {
                                 if (illegalYellow)
                                 {
                                    HTREEITEM geomRoot = m_CompsTree.GetChildItem(item);
                                    m_CompsTree.DeleteItem(geomRoot, geom->getName());

                                    geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, item, TVI_SORT);
                                    m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                                 }

                                 continue;
                              }
                              else
                              {
                                 HTREEITEM geomRoot = m_CompsTree.GetChildItem(item);
                                 m_CompsTree.DeleteItem(geomRoot, geom->getName());

                                 geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, item, TVI_SORT);
                                 m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      else if (geom) //if item is a Geom
      {
         EditGeometry(doc, geom);
         geomItem = item;
         preItem = m_CompsTree.GetParentItem(geomItem);

         if (!preItem)
            return;

         CString prefix = m_CompsTree.GetItemText(preItem);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            POSITION compPos = block->getDataList().GetHeadPosition();
            while (compPos)
            {
               DataStruct* compData = block->getDataList().GetNext(compPos);

               if (compData && compData->getInsert() && compData->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
                  geom == (BlockStruct*)doc->getBlockAt(compData->getInsert()->getBlockNumber()))
               {
                  CString currentPre = compData->getInsert()->getRefname();
                   
                  int index = currentPre.FindOneOf(".0123456789_");

                  if (index >0)
                     currentPre = currentPre.Left(index);

                  if (currentPre.CompareNoCase(prefix))
                     continue;

                  if (m_radioView == 0 || m_radioView == 1 || m_radioView == 2)
                  {
                     Attrib* attrib = NULL;

                     if (compData->getAttributesRef())
                     {
                        if (m_radioView != 2)
                        {
                           HTREEITEM subRoot = m_CompsTree.GetChildItem(geomItem);

                           while (subRoot)
                           {
                              CString compName;
                              int nIndex = m_CompsTree.GetItemText(subRoot).Find("-",0);

                              if (nIndex > 0)
                                 compName = m_CompsTree.GetItemText(subRoot).Left(nIndex);
                              else
                                 compName = m_CompsTree.GetItemText(subRoot);

                              if (!compName.CompareNoCase(compData->getInsert()->getRefname()))
                              {
                                 m_CompsTree.DeleteItem(subRoot);    

                                 break;
                              }

                              subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                           }

                           if (!compData->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                           {
                              compsItem = m_CompsTree.InsertItem(compData->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, geomItem, TVI_SORT);
                              m_CompsTree.SetItemData(compsItem, (DWORD)compData);                          
                           }
                           else if (m_radioView == 0)
                           {
                              CString type;
                              CString name = compData->getInsert()->getRefname();

                              if (attrib && attrib->getValueType() == VT_STRING)
                              {
                                 type = attrib->getStringValue();
                              }

                              DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                              if (!device)
                                 continue;

                              BlockStruct* geom = doc->getBlockAt(compData->getInsert()->getBlockNumber());

                              int colorDot = Devices.GetPinMapStatusIcon(compData);
                              {
                                 {
                                    HTREEITEM subRoot = m_CompsTree.GetChildItem(item);

                                    while (subRoot)
                                    {
                                       CString compName;
                                       int nIndex = m_CompsTree.GetItemText(subRoot).Find("-",0);

                                       if (nIndex > 0)
                                          compName = m_CompsTree.GetItemText(subRoot).Left(nIndex);
                                       else
                                          compName = m_CompsTree.GetItemText(subRoot);

                                       if (!compName.CompareNoCase(name))
                                       {
                                          m_CompsTree.DeleteItem(subRoot);     

                                          break;
                                       }

                                       subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                                    }

                                    compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, item, TVI_SORT);                   
                                    m_CompsTree.SetItemData(compsItem, (DWORD)compData);
                                 }
                              }

                           }
                        }
                        else if (m_radioView == 2)
                        {
                           Attrib* attrib = NULL;
                           compData->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib);
                           CString type;
                           CString name = compData->getInsert()->getRefname();

                           if (attrib && attrib->getValueType() == VT_STRING)
                           {
                              type = attrib->getStringValue();
                           }

                           DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

                           if (!device)
                              continue;

                           BlockStruct* geom = doc->getBlockAt(compData->getInsert()->getBlockNumber());

                           int colorDot = Devices.GetPinMapStatusIcon(compData);
                           {
                              HTREEITEM subRoot = m_CompsTree.GetChildItem(item);

                              while (subRoot)
                              {
                                 CString compName;
                                 int nIndex = m_CompsTree.GetItemText(subRoot).Find("-",0);

                                 if (nIndex > 0)
                                    compName = m_CompsTree.GetItemText(subRoot).Left(nIndex);
                                 else
                                    compName = m_CompsTree.GetItemText(subRoot);

                                 if (!compName.CompareNoCase(name))
                                 {
                                    m_CompsTree.DeleteItem(subRoot);  

                                    break;
                                 }

                                 subRoot = m_CompsTree.GetNextSiblingItem(subRoot); 
                              }

                              compsItem = m_CompsTree.InsertItem(name + "-" + type, colorDot, colorDot, item, TVI_SORT);                   
                              m_CompsTree.SetItemData(compsItem, (DWORD)compData);
                           }
                        }
                     }
                     else
                     {
                        if (m_radioView != 2)
                        {
                           HTREEITEM subRoot = m_CompsTree.GetChildItem(geomItem);
                           m_CompsTree.DeleteItem(subRoot, compData->getInsert()->getRefname());

                           compsItem = m_CompsTree.InsertItem(compData->getInsert()->getRefname(),ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, geomItem, TVI_SORT);
                           m_CompsTree.SetItemData(compsItem, (DWORD)compData);  
                        }
                     }
                  }
               }
            }
         }
      }
      else if (compData)
      {
         BlockStruct* tempGeom = doc->getBlockAt(compData->getInsert()->getBlockNumber());
         int pinCnt = 0;

         if (tempGeom)
         {
            POSITION dataPos = tempGeom->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct* data = tempGeom->getDataList().GetNext(dataPos);

               if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PIN)
               {
                  if (data->getInsert()->getRefname().CompareNoCase("NC")) //if pin is not nc
                     pinCnt++;
               }
            }

            for(;m_ListDevices.DeleteString(0) != LB_ERR;)
            {
            }

            for (int i=0; i<Devices.GetCount(); i++)
            {
               DeviceTypeStruct* device = Devices[i];

               if (!device)
                  continue;

               if (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
               {
                  m_ListDevices.AddString(device->deviceTypeName);
               }
            }
         }

         doc->OnDoneEditing();
         compsItem = item;
         double xmin, xmax, ymin, ymax;

         if (compData->getInsert() && compData->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            if (!activeView)
               return;

            UpdateData();

            CString m_ref = compData->getInsert()->getRefname();

            if (!ComponentsExtents(doc, m_ref, &xmin, &xmax, &ymin, &ymax, TRUE))
            {
               PanReference(((CCEtoODBView*)activeView), m_ref);
               ErrorMessage("Component has no extents", "Panning");

               return;
            }
               
            double marginSizeX = (doc->getSettings().getXmax() - doc->getSettings().getXmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;
            double marginSizeY = (doc->getSettings().getYmax() - doc->getSettings().getYmin()) * ((CCEtoODBApp*)AfxGetApp())->schSettings.PcbMargin / 100;

            double marginSize = max(marginSizeX, marginSizeY);

            xmin -= marginSize;
            xmax += marginSize;
               
            ymin -= marginSize;
            ymax += marginSize;   

            ((CCEtoODBView*)activeView)->ZoomBox(xmin, xmax, ymin, ymax);
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);
   *pResult = 0;
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::ItemIsGeom
*/
BlockStruct* CDeviceTyperCompPropertyPage::ItemIsGeom(HTREEITEM item)
{
   void* voidPtr = NULL;
   voidPtr = (void*)m_CompsTree.GetItemData(item);

   if (!voidPtr)
      return NULL;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               BlockStruct *geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

               if (geom)
               {
                  if (item && geom == (BlockStruct*)voidPtr)
                     return geom;
               }
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::ItemIsComp
*/
DataStruct* CDeviceTyperCompPropertyPage::ItemIsComp(HTREEITEM item)
{
   void* voidPtr = NULL;
   voidPtr = (void*)m_CompsTree.GetItemData(item);

   if (!voidPtr)
      return NULL;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && block->getFileNumber() == m_activeFileNumber)
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               if (item && data == (DataStruct*)voidPtr)
                  return data;
            }
         }
      }
   }

   return NULL;
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnCbnSelchangeComboPrefixgeom
*/
void CDeviceTyperCompPropertyPage::OnCbnSelchangeComboPrefixgeom()
{
   m_onInit = FALSE;

   if (m_radioGrouping != 2)
      return;

   m_CompsTree.ShowWindow(SW_HIDE);
   m_CompsTree.DeleteAllItems();
   UpdateData();
   CString prefix;
   m_cboPrefixGeom.GetLBText(m_cboPrefixGeom.GetCurSel(), prefix);
   HTREEITEM geomItem;
   HTREEITEM preItem = m_CompsTree.InsertItem(prefix, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
   m_CompsTree.SetItemData(preItem, (DWORD)NULL);  

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && block->getFileNumber() == m_activeFileNumber)
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString currentPre = data->getInsert()->getRefname();
               
               int index = currentPre.FindOneOf(".0123456789_");

               if (index >0)
                  currentPre = currentPre.Left(index);

               if (currentPre.CompareNoCase(prefix))
                  continue;

               BlockStruct *geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

               if (geom)
               {
                  if (m_radioView == 0)
                  {
                     HTREEITEM subRoot = m_CompsTree.GetChildItem(preItem);
                     m_CompsTree.DeleteItem(subRoot, geom->getName());

                     geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, preItem, TVI_SORT);
                     m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                  }
                  else if (m_radioView == 1)
                  {
                     Attrib* attrib = NULL;

                     if (data->getAttributesRef())
                     {
                        if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                        {
                           HTREEITEM subRoot = m_CompsTree.GetChildItem(preItem);
                           m_CompsTree.DeleteItem(subRoot, geom->getName());

                           geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, preItem, TVI_SORT);
                           m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                        }
                     }
                     else
                     {
                        HTREEITEM subRoot = m_CompsTree.GetChildItem(preItem);
                        m_CompsTree.DeleteItem(subRoot, geom->getName());

                        geomItem = m_CompsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, preItem, TVI_SORT);
                        m_CompsTree.SetItemData(geomItem, (DWORD)geom); 
                     }
                  }
                  else if (m_radioView == 2)
                  {
                  }
               }
            }
         }
      }
   }

   m_CompsTree.ShowWindow(SW_SHOW);

}

/******************************************************************************
*/
void CDeviceTyperCompPropertyPage::UpdateCountsDisplay()
{
   m_unassignedCount = m_compsCount - m_assignedCount;
   lblTotal.Format("%d", m_compsCount);
   lblAssigned.Format("%d", m_assignedCount);
   lblNotAssigned.Format("%d", m_unassignedCount);

   if (m_compsCount != 0)  // 1F not LF
      lblPercentage.Format("%.1f %%", ( (float)m_assignedCount / (float)m_compsCount) * 100.0);
   else
      lblPercentage.Format("0.0");

   m_lblTotal.SetWindowText(lblTotal);
   m_lblAssigned.SetWindowText(lblAssigned);
   m_lbl_NotAssigned.SetWindowText(lblNotAssigned);
   m_lblPercentage.SetWindowText(lblPercentage);
}

/******************************************************************************
*/
void CDeviceTyperCompPropertyPage::RemoveFromUnassignedView(HTREEITEM selItem)
{
   // If currently viewing Unassigned list, remove the selected item.
   // Presumably this item has just been assigned, but that is up to the caller to track.

   if (selItem != NULL && m_radioView == 1 /*Unassigned view*/)
   {
      int typeSel = m_ListDevices.GetCurSel();

      m_CompsTree.DeleteItem(selItem);
      m_CompsTree.UpdateData();

      // Reset type selection, the operations above clear the selection
      if (typeSel != CB_ERR)
         m_ListDevices.SetCurSel(typeSel);
   }   
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonManual
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonManual()
{
   HTREEITEM selItem = m_CompsTree.GetSelectedItem();

   if (!selItem)
      return;

   int typeSel = -1;

   if ((typeSel = m_ListDevices.GetCurSel()) == CB_ERR)
   {
      MessageBox("Please select a device type to assign.", "Device Typer", MB_OK);
      return;
   }

   m_onInit = FALSE;
   BlockStruct* geom = ItemIsGeom(selItem);
   DataStruct*  compsData = ItemIsComp(selItem);

   if (compsData) //if item selected is a comp
   {
      CString newType;
      m_ListDevices.GetText(m_ListDevices.GetCurSel(), newType);

      Attrib* oldType = NULL;

      if (!compsData->getDefinedAttributes()->Lookup(tempDeviceTypeKeyword, oldType))
         m_assignedCount++;

      CString name = compsData->getInsert()->getRefname();
      doc->SetAttrib(&compsData->getAttributesRef(), tempDeviceTypeKeyword, VT_STRING, newType.GetBuffer(0), SA_OVERWRITE, NULL);
      m_CompsTree.SetItemText(selItem, name + "-" + newType); 

      int colorDot = Devices.GetPinMapStatusIcon(compsData);
      m_CompsTree.SetItemImage(selItem, colorDot, colorDot);

      RemoveFromUnassignedView(selItem);

      UpdateCountsDisplay();

      return;

   }
   else if (geom && m_radioGrouping == 2) // if item selected is a geom
   {
      if (m_addToStpFile.GetCheck() == BST_CHECKED)
      {
         CString type;
         m_ListDevices.GetText(m_ListDevices.GetCurSel(), type);
         AddPrefixGeomItem(m_CompsTree.GetItemText(m_CompsTree.GetParentItem(selItem)), m_CompsTree.GetItemText(selItem), type);
      }

      //CString prefix = m_CompsTree.GetItemText(m_CompsTree.GetParentItem(selItem));

      ApplySelectedTypeToSelectedTreeComps();

      RemoveFromUnassignedView(selItem);

      UpdateCountsDisplay();

      SetPrefixGeomFile();

      return;
   }
   else if (!m_CompsTree.GetParentItem(selItem) && m_radioGrouping == 2) //if item selected is a prefix
   {
      if (m_addToStpFile.GetCheck() == BST_CHECKED)
      {
         CString type;
         m_ListDevices.GetText(m_ListDevices.GetCurSel(), type);
         AddPrefixGeomItem(m_CompsTree.GetItemText(selItem), " ", type);
      }

      CString prefix = m_CompsTree.GetItemText(selItem);

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block != NULL && block->getFileNumber() == m_activeFileNumber)
         {
            POSITION dataPos = block->getDataList().GetHeadPosition();
            while (dataPos)
            {
               DataStruct* data = block->getDataList().GetNext(dataPos);

               if (data != NULL && data->isInsertType(insertTypePcbComponent))
               {
                  CString currentPre = data->getInsert()->getRefname();

                  int index = currentPre.FindOneOf(".0123456789_");

                  if (index >0)
                     currentPre = currentPre.Left(index);

                  if (currentPre.CompareNoCase(prefix))
                     continue;

                  CString newType;
                  m_ListDevices.GetText(m_ListDevices.GetCurSel(), newType);

                  Attrib* oldType = NULL;

                  if (!data->getDefinedAttributes()->Lookup(tempDeviceTypeKeyword, oldType))
                     m_assignedCount++;

                  doc->SetAttrib(&data->getAttributesRef(), tempDeviceTypeKeyword, VT_STRING, newType.GetBuffer(0), SA_OVERWRITE, NULL);
               }
            }
         }
      }

      UpdateCountsDisplay();
      
      RemoveFromUnassignedView(selItem);

      SetPrefixGeomFile();

      return;
   }

   if (m_radioGrouping ==1)
   {
      if (!m_CompsTree.GetParentItem(selItem)) // if selected item is a keyword
      {
         CString keywordStr = m_CompsTree.GetItemText(selItem);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block != NULL && block->getFileNumber() == m_activeFileNumber)
            {
               for (POSITION dataPos = block->getDataList().GetHeadPosition();dataPos != NULL;)
               {
                  DataStruct* data = block->getDataList().GetNext(dataPos);

                  if (data != NULL && data->isInsertType(insertTypePcbComponent))
                  {
                     for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
                     {
                        Attrib* attrib = NULL;
                        WORD attribKeyword;

                        data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);

                        const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                        CString keyword = kw->cc;

                        if (!keywordStr.CompareNoCase(keyword))
                        {
                           CString newType;
                           m_ListDevices.GetText(m_ListDevices.GetCurSel(), newType);

                           Attrib* oldType = NULL;

                           if (!data->getDefinedAttributes()->Lookup(tempDeviceTypeKeyword, oldType))
                              m_assignedCount++;

                           doc->SetAttrib(&data->getDefinedAttributes(), tempDeviceTypeKeyword, VT_STRING, newType.GetBuffer(0), SA_OVERWRITE, NULL);
                        }
                     }
                  }
               }
            }
         }
      }
      else if (!m_CompsTree.GetParentItem(m_CompsTree.GetParentItem(selItem))) // if item is a value
      {
         if (m_addToStpFile.GetCheck() == BST_CHECKED)
         {
            CString type;
            m_ListDevices.GetText(m_ListDevices.GetCurSel(), type);
            AddKeyValItem(m_CompsTree.GetItemText(m_CompsTree.GetParentItem(selItem)), m_CompsTree.GetItemText(selItem), type);
         }

         //CString valItemStr = m_CompsTree.GetItemText(selItem);
         ApplySelectedTypeToSelectedTreeComps();
      }

      SetKeyValFile();
   }

   UpdateCountsDisplay();
}

/******************************************************************************
*/
void CDeviceTyperCompPropertyPage::ApplySelectedTypeToSelectedTreeComps()
{
   HTREEITEM selItem = m_CompsTree.GetSelectedItem();

   if (selItem)
   {
      CString newType;
      m_ListDevices.GetText(m_ListDevices.GetCurSel(), newType);

      DeviceTypeStruct* device = Devices.FindDeviceTypeByName(newType);

      if (device != NULL)
      {
         HTREEITEM compItem = m_CompsTree.GetChildItem(selItem);

         while (compItem)
         {
            CString itemText = m_CompsTree.GetItemText(compItem);
            int nIndex = itemText.Find("-",0);
            if (nIndex > 0)
               itemText.Truncate(nIndex);

            DataStruct *compData = (DataStruct*)m_CompsTree.GetItemData(compItem);

            if (compData != NULL)
            {
               Attrib* oldType;

               if (!compData->getDefinedAttributes()->Lookup(tempDeviceTypeKeyword, oldType))
                  m_assignedCount++;

               doc->SetAttrib(&compData->getDefinedAttributes(), tempDeviceTypeKeyword, VT_STRING, newType.GetBuffer(0), SA_OVERWRITE, NULL);

               int colorDot = Devices.GetPinMapStatusIcon(compData);
               m_CompsTree.SetItemImage(compItem, colorDot, colorDot);
               m_CompsTree.SetItemText(compItem, itemText + "-" + newType); 
            }

            compItem = m_CompsTree.GetNextSiblingItem(compItem);
         }
      }
   }
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonClear
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonClear()
{
   HTREEITEM selItem = m_CompsTree.GetSelectedItem();

   if (!selItem)
      return;

   m_onInit = FALSE;
   BlockStruct* geom = ItemIsGeom(selItem);
   DataStruct*  compsData = ItemIsComp(selItem);

   if (compsData != NULL) //if item selected is a comp
   {
      void* voidPtr = NULL;

      if (compsData->getAttributesRef() != NULL && compsData->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword))
      {
         m_CompsTree.SetItemImage(selItem, ICON_HOLLOW_DOT, ICON_HOLLOW_DOT);
         m_assignedCount--;
      }
   }
   else if (geom != NULL) // if item selected is a geom
   {
      void* voidPtr = NULL;
      CString prefix = m_CompsTree.GetItemText(m_CompsTree.GetParentItem(selItem));

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block == NULL)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
         {
            DataStruct* data = pcbComponentIterator.getNext();

            if (geom == doc->getBlockAt(data->getInsert()->getBlockNumber()))
            {
               CString currentPre = data->getInsert()->getRefname();
               
               int index = currentPre.FindOneOf(".0123456789_");

               if (index >0)
                  currentPre = currentPre.Left(index);

               if (currentPre.CompareNoCase(prefix))
                  continue;

               if (data->getAttributesRef() != NULL && data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword))
               {
                  m_assignedCount--;
               }
            }
         }
      }

      HTREEITEM compItem = m_CompsTree.GetChildItem(selItem);

      while (compItem != NULL)
      {
         m_CompsTree.SetItemImage(compItem, ICON_HOLLOW_DOT, ICON_HOLLOW_DOT);
         compItem = m_CompsTree.GetNextSiblingItem(compItem);
      }

      m_CompsTree.UpdateData();
   }
   else if (!m_CompsTree.GetParentItem(selItem) && m_radioGrouping == 2) //if item selected is a prefix
   {
      void* voidPtr = NULL;
      CString prefix = m_CompsTree.GetItemText(selItem);

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (block == NULL)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;
         
         for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
         {
            DataStruct* data = pcbComponentIterator.getNext();
            CString currentPre = data->getInsert()->getRefname();
            
            int index = currentPre.FindOneOf(".0123456789_");

            if (index >0)
               currentPre = currentPre.Left(index);

            if (currentPre.CompareNoCase(prefix))
               continue;

            if (data->getAttributesRef() != NULL && data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword))
            {
               m_assignedCount--;
            }
         }
      }
   }

   if (m_radioGrouping ==1)
   {
      if (!m_CompsTree.GetParentItem(selItem)) // if selected item is a keyword
      {
         CString keywordStr = m_CompsTree.GetItemText(selItem);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (!block)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;

            for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
            {
               DataStruct* data = pcbComponentIterator.getNext();
               
               for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
               {
                  CString keyword;
                  Attrib* attrib = NULL;
                  WORD attribKeyword;

                  data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);

                  const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                  keyword = kw->cc;

                  if (!keywordStr.CompareNoCase(keyword))
                  {
                     if (data->getAttributesRef() != NULL && data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword))
                     {
                        m_assignedCount--;
                     }
                  }
               }
            }
         }
      }
      else if (!m_CompsTree.GetParentItem(m_CompsTree.GetParentItem(selItem))) // if item is a value
      {
         CString valItemStr = m_CompsTree.GetItemText(selItem);

         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block == NULL)
               continue;

            if (block->getFileNumber() != m_activeFileNumber)
               continue;
            
            for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
            {
               DataStruct* data = pcbComponentIterator.getNext();
               
               for (POSITION attribPos = data->getAttributesRef()->GetStartPosition();attribPos != NULL;)
               {
                  CString keyword;
                  Attrib* attrib = NULL;
                  WORD attribKeyword;
                  CString keywordStr;

                  data->getAttributesRef()->GetNextAssoc(attribPos, attribKeyword, attrib);
                  const KeyWordStruct* kw = doc->getKeyWordArray()[attribKeyword];
                  keywordStr = kw->cc;

                  WORD kwWord = doc->RegisterKeyWord(keywordStr, 0, kw->getValueType()); 
                  data->getAttributesRef()->Lookup(kwWord, attrib);
                  CString valueStr;

                  if (attrib && attrib->getValueType() == VT_STRING)
                  {
                     valueStr = attrib->getStringValue();
                  }
                  else if (attrib && attrib->getValueType() == VT_INTEGER)
                  {
                     valueStr.Format("%d", attrib->getIntValue());
                  }
                  else if (attrib && (attrib->getValueType() == VT_DOUBLE || attrib->getValueType() == VT_UNIT_DOUBLE))
                  {
                     valueStr.Format("%f", attrib->getDoubleValue());
                  }
                  else
                  {
                     continue;
                  }

                  valueStr.TrimLeft();
                  valueStr.TrimRight();

                  if (!valueStr.CompareNoCase(valItemStr))
                  {
                     if (data->getAttributesRef() != NULL && data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword))
                     {
                        m_assignedCount--;
                     }
                  }
               }
            }
         }
      }
   }

   UpdateCountsDisplay();

   CString itemText = m_CompsTree.GetItemText(selItem);
   int nIndex;

   if (nIndex = itemText.Find("-",0) > 0)
      itemText = itemText.Left(nIndex+1);

   m_CompsTree.SetItemText(selItem, itemText);
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonZoom1to1
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonZoom1to1()
{
   m_onInit = FALSE;

   if (activeView)
      ((CCEtoODBView*)activeView)->OnZoom1To1();
   
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonClrkw
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonClrkw()
{
   m_onInit = FALSE;
   m_cboKwValue.Clear();
   m_cboKwValue.SetCurSel(-1);
   m_cboKwValue.SetWindowText("");

   if (m_radioGrouping == 1)
   {
      OnBnClickedRadioKwvalue();
   }
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonClrpre
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonClrpre()
{
   m_onInit = FALSE;
   m_cboPrefixGeom.Clear();
   m_cboPrefixGeom.SetCurSel(-1);
   m_cboPrefixGeom.SetWindowText("");

   if (m_radioGrouping == 2)
   {
      OnBnClickedRadioPrefixgeom();
   }
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnCbnSelchangeComboKwvalue
*/
void CDeviceTyperCompPropertyPage::OnCbnSelchangeComboKwvalue()
{
   m_onInit = FALSE;

   if (m_radioGrouping != 1)
      return;

   m_CompsTree.ShowWindow(SW_HIDE);
   UpdateData();
   CString keyword;
   m_cboKwValue.GetLBText(m_cboKwValue.GetCurSel(), keyword);
   OnBnClickedRadioKwvalue();

   HTREEITEM rootItem = m_CompsTree.GetRootItem();
   HTREEITEM tempItem;

   while (rootItem)
   {
      tempItem = m_CompsTree.GetNextSiblingItem(rootItem);

      if (m_CompsTree.GetItemText(rootItem).CompareNoCase(keyword))
      {
         m_CompsTree.DeleteItem(rootItem);
      }

      rootItem = tempItem;
   }

   m_cboKwValue.SelectString(0, keyword);
   m_CompsTree.UpdateData();
   m_CompsTree.ShowWindow(SW_SHOW);
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonReport
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonReport()
{
   m_onInit = FALSE;
   FILE  *fpReport;

   CString reportStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_REPORT_STP) );

   if ((fpReport = fopen(reportStpFilename,"w")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",reportStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   fprintf(fpReport, "Total number of components: %d\n", m_compsCount);
   fprintf(fpReport, "Number of assigned components: %d\n", m_assignedCount);
   fprintf(fpReport, "Number of unassigned components: %d\n", m_unassignedCount);
   m_voilationCount = 0; //case 994 - violation count was not initialized to zero

   // get violations count
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               Attrib* attrib = NULL;
               data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib);
               CString type;
               CString name = data->getInsert()->getRefname();

               if (attrib && attrib->getValueType() == VT_STRING)
               {
                  type = attrib->getStringValue();
               }

               int pinCnt = 0;
               int illegalYellow = FALSE;
               DeviceTypeStruct* device = Devices.FindDeviceTypeByName(type);

               if (!device)
                  continue;

               BlockStruct* geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

               POSITION pos = geom->getDataList().GetHeadPosition();
               while (pos)
               {
                  DataStruct* pindata = geom->getDataList().GetNext(pos);

                  if (pindata)
                  {

                     if (!pindata->getInsert() || pindata->getInsert()->getInsertType() != INSERTTYPE_PIN)
                        continue;
                     
                     CString pintypeName;
                     int isPinTyped = IsDataPinTyped(/*comp*/data, /*pin*/pindata, pintypeName);
                     
                     if (device && device->expPinNames.GetLength() > 0)
                     {
                        illegalYellow = TRUE;
                        CString pinName = pindata->getInsert()->getRefname();
                        int curPos = 0;
                        CString token = device->expPinNames.Tokenize(" ",curPos);

                        while (!token.IsEmpty())
                        {
                           // Case 1425 -problem-
                           // This test is left in for posterity. Maybe it works in come cases.
                           // Or maybe it is just wrong. It is comparing the pin "refname" (usually
                           // a number) with the device pin name (often a string, like "Emitter").
                           // I.e. it is comparing apples and oranges.
                           if (!token.CompareNoCase(pinName))
                              illegalYellow = FALSE;

                           // Case 1425 -fix-
                           // This test is much better. Comparing apples and apples.
                           if (isPinTyped)
                           {
                              if (!token.CompareNoCase(pintypeName))
                                 illegalYellow = FALSE;
                           }

                           token = device->expPinNames.Tokenize(" ", curPos);
                        }
                     }

                     // Count pin only if pin is not a NC (no connect)
                     if (pindata->getInsert()->getRefname().CompareNoCase("NC") != 0 &&
                        pintypeName.CompareNoCase(ATT_VALUE_NO_CONNECT) != 0)
                        pinCnt++;
                  }
               }


               // Case 1425. The original version of this didn't work right. It assumed the part
               // was violated=TRUE, and then scanned the tree to see if the refdes had a
               // "green dot", and cleared the violation if so. The problem is if the
               // view was "view unassigned" or "view violation", then
               // the tree will not contain all parts, hence "good" parts may not be in tree,
               // and no "green dot" icon will be found, and then the part will get counted
               // as a violation.
               // The fix goes the other way: assume the part is okay, and if it is in the
               // tree and does not have a green dot, then flag it as a violation. This
               // might end up having a violation count that is too low. We'll see.
               
               BOOL violated = false;
               HTREEITEM root = m_CompsTree.GetRootItem();

               while (root)
               {
                  CString itemText = m_CompsTree.GetItemText(root);

                  if (!itemText.Left(itemText.FindOneOf("-")).CompareNoCase(data->getInsert()->getRefname()))
                  {
                     int nImage, nSelImage;
                     m_CompsTree.GetItemImage(root, nImage, nSelImage);

                     if (nImage != ICON_GREEN_DOT)
                     {
                        violated = TRUE;
                        break;
                     }
                  }

                  root = m_CompsTree.GetNextSiblingItem(root);
               }

               if (violated && illegalYellow && pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
               {
                  m_voilationCount++;

                  continue;
               }

               if (!violated && pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1))
                  continue;

               m_voilationCount++;
            }
         }
      }
   }

   fprintf(fpReport, "Number of violations: %d\n", m_voilationCount);

   fprintf(fpReport, "\n\nDevice Types:\n");
   fprintf(fpReport, "--------------\n");

   Attrib* attrib = NULL;
   CString name;

   for (int j=0; j<Devices.GetCount(); j++)
   {
      DeviceTypeStruct* device = Devices[j];

      if (!device)
         continue;

      device->refCount = 0;

      for (int i=0; i<doc->getMaxBlockIndex(); i++)
      {
         BlockStruct *block = doc->getBlockAt(i);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct* data = block->getDataList().GetNext(dataPos);

            if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
                  continue;

               if (attrib && attrib->getValueType() == VT_STRING)
               {
                  name = attrib->getStringValue();
               }
               else
               {
                  continue;
               }

               name.TrimLeft();
               name.TrimRight();
               name.Trim(" ");

               if(name.CompareNoCase(device->deviceTypeName))
                  continue;

               CString refName = data->getInsert()->getRefname();
               device->refNames.SetAtGrow(device->refCount++, refName);
            }
         }
      }   
   }

   for (int i=0; i<Devices.GetCount(); i++)
   {
      DeviceTypeStruct* device = Devices[i];

      if (!device)
         continue;

      if (!device->refCount)
      {
         if (!device->deviceTypeName.Right(1).CompareNoCase("y") && device->deviceTypeName.Right(2).CompareNoCase("ay"))
            fprintf(fpReport, "0 %sies: \n", device->deviceTypeName.Left(device->deviceTypeName.GetLength()-1));
         else if (!device->deviceTypeName.Right(2).CompareNoCase("ch"))
            fprintf(fpReport, "%d %ses: \n", device->refCount, device->deviceTypeName);
         else if (!device->deviceTypeName.Right(2).CompareNoCase("ed"))
            fprintf(fpReport, "0 %s: \n", device->deviceTypeName);
         else
            fprintf(fpReport, "0 %ss: \n", device->deviceTypeName);

         continue;
      }

      if (device->refCount >1)
      {
         if (!device->deviceTypeName.Right(1).CompareNoCase("y") && device->deviceTypeName.Right(2).CompareNoCase("ay"))
            fprintf(fpReport, "%d %sies: ", device->refCount, device->deviceTypeName.Left(device->deviceTypeName.GetLength()-1));
         else if (!device->deviceTypeName.Right(2).CompareNoCase("ch"))
            fprintf(fpReport, "%d %ses: ", device->refCount, device->deviceTypeName);
         else if (!device->deviceTypeName.Right(2).CompareNoCase("ed"))
            fprintf(fpReport, "%d %s: ", device->refCount, device->deviceTypeName);
         else
            fprintf(fpReport, "%d %ss: ", device->refCount, device->deviceTypeName);
      }
      else if (device->refCount == 1)
      {
         fprintf(fpReport, "1 %s: ", device->deviceTypeName);
      }

      for (int j=0; j<device->refCount; j++)
      {
         if (j==0)
            fprintf(fpReport, " %s", device->refNames.GetAt(j));
         else
            fprintf(fpReport, ", %s", device->refNames.GetAt(j));
      }

      fprintf(fpReport, "\n");
   }

   fclose(fpReport);

   CString commandLine = "notepad";
   commandLine += " ";
   commandLine += "\"";
   commandLine += reportStpFilename; // make a quote for long filenames
   commandLine += "\"";

   WinExec(commandLine, SW_SHOW);
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnOk
*/
void CDeviceTyperCompPropertyPage::OnOK()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFileNumber() != m_activeFileNumber)
         continue;

      for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
      {
         DataStruct* data = pcbComponentIterator.getNext();

         if (data->getAttributesRef() == NULL)
            continue;

         Attrib* attrib = NULL;
         CString type;

         if (data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
         {
            if (attrib != NULL)
               type = attrib->getStringValue();

            void* valuePtr = NULL;
            valuePtr = (void*)type.GetString();

            doc->SetAttrib(&data->getAttributesRef(), deviceTypeKeyword, VT_STRING, valuePtr, SA_OVERWRITE, NULL);
            data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword);
         }
         /*else if (data->getAttributesRef()->Lookup(deviceTypeKeyword, voidPtr))
         {
            data->getAttributesRef()->RemoveKey(deviceTypeKeyword);
         }*/
      }
   }

   doc->OnDoneEditing();
   CPropertyPage::OnOK();
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnCancel
*/
void CDeviceTyperCompPropertyPage::OnCancel()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFileNumber() != m_activeFileNumber)
         continue;

      for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
      {
         DataStruct* data = pcbComponentIterator.getNext();

         data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword);
      }
   }
   
   doc->OnDoneEditing();
   CPropertyPage::OnCancel();
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButtonClearall
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButtonClearall()
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block == NULL)
         continue;

      if (block->getFileNumber() != m_activeFileNumber)
         continue;

      for (CDataListIterator pcbComponentIterator(*block,insertTypePcbComponent);pcbComponentIterator.hasNext();)
      {
         DataStruct* data = pcbComponentIterator.getNext();

         data->getAttributesRef()->deleteAttribute(tempDeviceTypeKeyword);
      }
   }
   
   m_assignedCount = 0;

   UpdateCountsDisplay();

   if (m_radioView == 0)
      OnBnClickedRadioViewall();
   else if (m_radioView == 1)
      OnBnClickedRadioViewunassigned();
   else if (m_radioView == 2)
      OnBnClickedRadioviewviolations();
}

/******************************************************************************
* CDeviceTyperCompPropertyPage::OnBnClickedButton1
*/
void CDeviceTyperCompPropertyPage::OnBnClickedButton1()
{
   ReadDeviceTypeMasterList();

   for(;m_ListDevices.DeleteString(0) != LB_ERR;)
   {
   }

   for (int i=0; i<Devices.GetCount();i++)
   {
      DeviceTypeStruct *device = Devices[i];

      if (device)
         m_ListDevices.InsertString(i, device->deviceTypeName);
   }  
}


/******************************************************************************
* CDeviceTyperCompPropertyPage::GetDeviceTypeByPrefix
*/
CString CDeviceTyperCompPropertyPage::GetDeviceTypeByPrefix(CString prefix)
{
   for (int i=0; i< prefixesCnt; i++)
   {
      CString pre = prefixes.GetAt(i);

      if (pre)
         pre.Trim();   
   }

   return "";
}

/**************************************************88
* CDeviceTyperCompPropertyPage::IsDataPinTyped
*/
BOOL CDeviceTyperCompPropertyPage::IsDataPinTyped(DataStruct* data, DataStruct *pin)
{
   CString pinType;
   return IsDataPinTyped(data, pin, pinType);
}
   
BOOL CDeviceTyperCompPropertyPage::IsDataPinTyped(DataStruct* data, DataStruct *pin, CString& pinType)
{
   pinType = "";

   if (!data)
      return FALSE;

   WORD deviceToPackagePinMap = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
   Attrib* attrib = NULL;

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct* file = doc->getFileList().GetNext(filePos);

      if (!file)
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct* net = file->getNetList().GetNext(netPos);

         if (!net)
            continue;

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);

            if (compPin != NULL && compPin->getRefDes().CompareNoCase(data->getInsert()->getRefname()) == 0)
            {
               // We have the right comp Refname, now check pin
               if (compPin->getPinName().CompareNoCase(pin->getInsert()->getRefname()) == 0)
               {
                  // We have teh right comp and right pin, now check for pin mapping
                  if (compPin->getAttributesRef() != NULL && compPin->getAttributesRef()->Lookup(deviceToPackagePinMap, attrib))
                  {
                     if (attrib && attrib->getValueType() == VT_STRING)
                     {
                        pinType = attrib->getStringValue();
                     }

                     // Case 1824, a blank pin map value is no longer considered mapped.
                     if (!pinType.IsEmpty())
                        return TRUE;
                  }

                  // Did not have pin map attr or attr was empty
                  return FALSE;
               }
            }
         }
      }
   }

   return FALSE;
}

/**************************************************88
* CDeviceTyperCompPropertyPage::GetGeomPinMapping
*/
void CDeviceTyperCompPropertyPage::GetGeomPinMapping()
{
   FILE *fp;
   char  line[255];
   char  *lpGeom;
   CString pinMapFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_GEOMPINMAP_STP) );

   if ((fp = fopen(pinMapFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",pinMapFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    

   while (fgets(line,255,fp))
   {
      CString tmp;
      CString tmp1;
      CString tmp2;
      CString tmp3;

      //geom name
      if ((lpGeom = get_string(line, ",", 1)) == NULL) 
         continue;

      tmp1 = lpGeom;
      tmp1.Trim();
      
      //pin #
      if ((lpGeom = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp2 = lpGeom;
      tmp2.Trim();

      //pin name
      if ((lpGeom = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp3 = lpGeom;
      tmp3.Trim();

      tmp.Format("%s %s %s", tmp1, tmp2, tmp3);
      tmp.Trim();

      geomPinMaps.SetAtGrow(geomPinMapsCount++, tmp);      
   }
}

/**************************************************88
* CDeviceTyperCompPropertyPage::GetPartNumPinMapping
*/
void CDeviceTyperCompPropertyPage::GetPartNumPinMapping()
{
   FILE *fp;
   char  line[255];
   char  *lpPN;
   CString pinMapStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_PNPINMAP_STP) );

   if ((fp = fopen(pinMapStpFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",pinMapStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }    

   while (fgets(line,255,fp))
   {
      CString tmp;
      CString tmp1;
      CString tmp2;
      CString tmp3;

      //part number
      if ((lpPN = get_string(line, ",", 1)) == NULL) 
         continue;

      tmp1 = lpPN;
      tmp1.Trim();
      
      //pin name
      if ((lpPN = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp2 = lpPN;
      tmp2.Trim();

      //pin type
      if ((lpPN = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp3 = lpPN;
      tmp3.Trim();

      tmp.Format("%s %s %s", tmp1, tmp2, tmp3);
      tmp.Trim();

      PNPinMaps.SetAtGrow(PNPinMapsCount++, tmp);      
   }
}

/**************************************************88
* CDeviceTyperCompPropertyPage::UpdateFromFile
*/
void CDeviceTyperCompPropertyPage::UpdateFromFile()
{
   for (int i=0; i<geomPinMapsCount; i++)
   {
      int nIndex = geomPinMaps.GetAt(i).FindOneOf(" ");

      if (nIndex < 0)
         continue;

      CString geomName = geomPinMaps.GetAt(i).Left(nIndex);
      BlockStruct *geom = doc->Find_Block_by_Name(geomName, -1);

      if (!geom || geom->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         BlockStruct *block = doc->getBlockAt(j);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT
               || geom != doc->getBlockAt(data->getInsert()->getBlockNumber()))
               continue;

            void* voidPtr = NULL;

            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos)
            {
               FileStruct* file = doc->getFileList().GetNext(filePos);

               if (!file)
                  continue;

               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct* net = file->getNetList().GetNext(netPos);

                  if (!net)
                     continue;

                  POSITION compPinPos = net->getHeadCompPinPosition();
                  while (compPinPos)
                  {
                     CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                     if (!compPin)
                        continue;

                     if (!compPin->getRefDes().CompareNoCase(data->getInsert()->getRefname()))
                     {
                        // get the pin name     
                        CString tempName = geomPinMaps.GetAt(i);
                        nIndex = tempName.FindOneOf(" ");
                        CString pinName = tempName.Right(tempName.GetLength() - nIndex - 1);
                        nIndex = pinName.FindOneOf(" ");

                        if (nIndex > 0)
                           pinName = pinName.Left(nIndex);

                        nIndex = tempName.ReverseFind(' ');
                        CString type = tempName.Right(tempName.GetLength() - nIndex - 1);
                        type.Trim();

                        if (!compPin->getPinName().CompareNoCase(pinName))
                        {
                           if (type.Find("Connect",0) != -1)
                              voidPtr = (void*)ATT_VALUE_NO_CONNECT;
                           else
                              voidPtr = (void*)type.GetString();

                           doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
                           UpdateData();
                        }
                     }
                  } // close while compPinPos
               }  // close while netPos
            }  // close while filePos
         } // close while dataPos
      }// close block for
   } // close for loop

   for (int i=0; i<PNPinMapsCount; i++)
   {
      int nIndex = PNPinMaps.GetAt(i).FindOneOf(" ");

      if (nIndex < 0)
         continue;

      CString partNum = PNPinMaps.GetAt(i).Left(nIndex);

      WORD pnKw = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);

      Attrib* attrib = NULL;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         BlockStruct *block = doc->getBlockAt(j);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            attrib = NULL;
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT ||
               !data->getAttributesRef()->Lookup(pnKw, attrib))
               continue;

            CString refName = data->getInsert()->getRefname();
            CString pn;

            if (attrib)
               pn = attrib->getStringValue();

            //if data doesn't have the same partnumber, continue
            if (pn.CompareNoCase(partNum))
               continue;

            attrib = NULL;

            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos)
            {
               FileStruct* file = doc->getFileList().GetNext(filePos);

               if (!file)
                  continue;

               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct* net = file->getNetList().GetNext(netPos);

                  if (!net)
                     continue;

                  POSITION compPinPos = net->getHeadCompPinPosition();
                  while (compPinPos)
                  {
                     CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                     if (!compPin || compPin->getRefDes().CompareNoCase(refName))
                        continue;

                     CString tempName = PNPinMaps.GetAt(i);
                     nIndex = PNPinMaps.GetAt(i).FindOneOf(" ");
                     CString pinName = tempName.Right(tempName.GetLength() - nIndex - 1);
                     nIndex = pinName.FindOneOf(" ");

                     if (nIndex > 0)
                        pinName = pinName.Left(nIndex);

                     nIndex = tempName.ReverseFind(' ');
                     CString type = tempName.Right(tempName.GetLength() - nIndex - 1);
                     type.Trim();

                     if (compPin->getPinName().CompareNoCase(pinName))
                        continue;

                     void* voidPtr;

                     if (type.Find("Connect",0) != -1)
                        voidPtr = (void*)ATT_VALUE_NO_CONNECT;
                     else
                        voidPtr = (void*)type.GetString();

                     doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
                     
                     UpdateData();

                     break;
                  } // close while compinpos
               }// close while netpos
            } // close while filepos
         } // close while datapos
      } // close for block
   } // close for PNPinMaps array
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////


// CDeviceTyperPinPropertyPage dialog

IMPLEMENT_DYNAMIC(CDeviceTyperPinPropertyPage, CPropertyPage)
CDeviceTyperPinPropertyPage::CDeviceTyperPinPropertyPage()
   : CPropertyPage(CDeviceTyperPinPropertyPage::IDD)
{
   m_activeFileNumber = 0;
   m_imageList = new CImageList();
   m_radioGeom = 0;
}

CDeviceTyperPinPropertyPage::CDeviceTyperPinPropertyPage(CCEtoODBDoc& document)
   : CPropertyPage(CDeviceTyperPinPropertyPage::IDD)
{
   doc = &document;
   m_activeFileNumber = doc->getFileList().GetOnlyShown(blockTypePcb)->getFileNumber();
   m_radioGeom = 0;
   m_imageList = new CImageList();
}

CDeviceTyperPinPropertyPage::~CDeviceTyperPinPropertyPage()
{
   if (m_imageList)
   {
      m_imageList->DeleteImageList();

      delete m_imageList;
   }

   Devices.DestroyAll();
}

void CDeviceTyperPinPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_TREE_DEVICETYPE_PIN, m_pinsTree);
   DDX_Radio(pDX, IDC_RADIO_GEOM, m_radioGeom);
   DDX_Control(pDX, IDC_CHECK1, m_addToStpFile);
}


BEGIN_MESSAGE_MAP(CDeviceTyperPinPropertyPage, CPropertyPage)
   ON_BN_CLICKED(IDC_RADIO_GEOM, OnBnClickedRadioGeom)
   ON_BN_CLICKED(IDC_RADIO_PARTNUM, OnBnClickedRadioPartnum)
   ON_BN_CLICKED(IDC_RADIO_REF, OnBnClickedRadioRef)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_DEVICETYPE_PIN, OnTvnSelchangedTreeDevicetypePin)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BEGIN_EVENTSINK_MAP(CDeviceTyperPinPropertyPage, CPropertyPage)
   ON_EVENT(CDeviceTyperPinPropertyPage, IDC_VSFLEX, DISPID_MOUSEUP, MouseUpVsflex, VTS_I2 VTS_I2 VTS_R4 VTS_R4)
   ON_EVENT(CDeviceTyperPinPropertyPage, IDC_VSFLEX, 24, AfterEditPinsGrid, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

BOOL CDeviceTyperPinPropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   GridSetup();
   
   initFlag = TRUE;

   m_imageList->Create(16, 16, TRUE, 32, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_RED_DOT));
   m_imageList->Add(app->LoadIcon(IDI_GREEN_DOT));
   m_imageList->Add(app->LoadIcon(IDI_YELLOW_DOT));
   m_imageList->Add(app->LoadIcon(IDI_DOT_HOLLOW));
   
   m_pinsTree.SetImageList(m_imageList, TVSIL_NORMAL);

   prefixArray.SetSize(100,100);
   prefixArrayCount = 0;

   geomPinMaps.SetSize(100, 100);
   geomPinMapsCount = 0;

   for (int i=0; i <100; i++)
      geomPinMaps.SetAtGrow(i, "");

   PNPinMaps.SetSize(100, 100);
   PNPinMapsCount = 0;

   for (int i=0; i <100; i++)
      PNPinMaps.SetAtGrow(i, "");

   prevSelected = NULL;
   messageSent  = FALSE; 
   Updated      = FALSE;

   GetGeomPinMapping();
   GetPartNumPinMapping();

   deviceTypeKeyword     = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);
   tempDeviceTypeKeyword = doc->RegisterKeyWord("TEMPDEVICETYPE", 0, VT_STRING);
   deviceToPackagePinMap = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);

   // case 1188, speed issues, the following seems uneccessary, building trees takes biggest time
   // OnSetActive() will be called before tab opens, and it will call this or the part number version.
   //OnBnClickedRadioGeom();
   GetPrefix();

   //update pinmappings from file
   //case 992
   UpdateFromFile();

   return TRUE;
}


BOOL CDeviceTyperPinPropertyPage::OnSetActive()
{
   CPropertyPage::OnSetActive();

   prefixArray.SetSize(100,100);
   prefixArrayCount = 0;

   COleVariant rowi;
   int j = m_flexGrid.get_Rows()-1;

   for (int i =j; i > 0 ; i--)
   {
      rowi = (GRID_CNT_TYPE)i;
      m_flexGrid.RemoveItem(rowi);
   }

   GetGeomPinMapping();
   GetPartNumPinMapping();


   // Case 1188 addendum, if there is at least one part attribute present
   // then default view to part Numbers
   if (PartNumberAttributePresent())
   {
      OnBnClickedRadioPartnum();
      UpdateData(FALSE);
   }
   else 
   {
      OnBnClickedRadioGeom();
      UpdateData(FALSE);
   }


   m_pinsTree.ShowWindow(SW_SHOW);
   GetPrefix();

   //update pinmappings from file    ?-Shouldn't this be done Before we check for violations-?
   //case 992
   UpdateFromFile();

   return TRUE;
}
void CDeviceTyperPinPropertyPage::OnBnClickedRadioGeom()
{
   m_radioGeom = 0;

   m_pinsTree.SelectItem(NULL);
   m_pinsTree.SelectDropTarget(NULL);
   m_pinsTree.ShowWindow(SW_HIDE);

   m_pinsTree.DeleteAllItems();

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block && (block->getFileNumber() == m_activeFileNumber))
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            BOOL alreadyTyped = FALSE;
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data != NULL && data->isInsertType(insertTypePcbComponent))
            {
               CString name = data->getInsert()->getRefname();

               DeviceTypeStruct *device = Devices.GetDevice(data);
               BlockStruct *geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

               // If it has device assigned and device has non-empty pin type name list
               // or component raw pin count does not match device
               // then add it to tree.
               if (geom != NULL && device != NULL && 
                  (!device->expPinNames.IsEmpty() || Devices.HasPinCountAnomaly(data)))
               {
                  m_pinsTree.DeleteItem(geom->getName());

                  HTREEITEM geomItem = m_pinsTree.InsertItem(geom->getName(), ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                  m_pinsTree.SetItemData(geomItem, (DWORD)geom);
 
               }
            }
         }
      }
   }

   // need to make sure that tree selection is cleared
   m_pinsTree.SelectItem(NULL);
   m_pinsTree.SelectDropTarget(NULL);

   m_pinsTree.ShowWindow(SW_SHOW);

   initFlag = FALSE;
}

void CDeviceTyperPinPropertyPage::OnBnClickedRadioPartnum()
{
   m_radioGeom = 1;

   m_pinsTree.SelectItem(NULL);
   m_pinsTree.SelectDropTarget(NULL);
   m_pinsTree.ShowWindow(SW_HIDE);

   m_pinsTree.DeleteAllItems();

   fillPartNumberTree();

   m_pinsTree.SelectItem(NULL);
   m_pinsTree.SelectDropTarget(NULL);

   m_pinsTree.ShowWindow(SW_SHOW);
}

void CDeviceTyperPinPropertyPage::OnBnClickedRadioRef()
{
   // TODO: Add your control notification handler code here
   m_radioGeom = 2;
   m_pinsTree.SelectItem(NULL);
   m_pinsTree.SelectDropTarget(NULL);
   m_pinsTree.ShowWindow(SW_HIDE);
   m_pinsTree.DeleteAllItems();

   for (int i=0; i<prefixArrayCount; i++)
   {
      m_pinsTree.DeleteItem(prefixArray.GetAt(i));

      HTREEITEM preItem = m_pinsTree.InsertItem(prefixArray.GetAt(i), ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
      m_pinsTree.SetItemData(preItem, (DWORD)(prefixArray.GetAt(i).GetString()));
   }
   
   m_pinsTree.ShowWindow(SW_SHOW);
}

void CDeviceTyperPinPropertyPage::OnTvnSelchangedTreeDevicetypePin(NMHDR *pNMHDR, LRESULT *pResult)
{
   if (Updated && !messageSent && prevSelected && MessageBox("Would you like to update the pin mapping you just did for the previous Geometry/Partnumber?","", MB_YESNO) == IDYES)
   {
      // THIS code does not work. Changing selection causes re-entry into this
      // function at SelectItem() call, so changes get lost before the subsequent
      // OnBnClickedOk() call. Looks like no one ever tested this before.
      Updated     = FALSE;
      m_pinsTree.SelectItem(prevSelected);  // this causes recursive call to OnTvSelchangedTree... ie this function
      messageSent = TRUE;
      OnBnClickedOk();

      return;
   }
   
   Updated = FALSE;

   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   HTREEITEM selectedItem = m_pinsTree.GetSelectedItem();

   if (!selectedItem)
      return;

   if (prevSelected != selectedItem)
   {
      COleVariant rowi;
      int j = m_flexGrid.get_Rows()-1;

      for (int i =j; i > 0 ; i--)
      {
         rowi = (GRID_CNT_TYPE)i;
         m_flexGrid.RemoveItem(rowi);
         
      }
   }

   if (m_radioGeom == 0) // if GEOM mode
   {
      if (!m_pinsTree.GetParentItem(selectedItem)) // if item has no parent, then it is a geom item
      {
         BlockStruct* selGeom = (BlockStruct*)m_pinsTree.GetItemData(selectedItem);

         void* voidPtr = NULL;

         for (int i = 0; i < doc->getMaxBlockIndex(); i++)
         {
            BlockStruct* block = doc->getBlockAt(i);

            if (block != NULL && block->getFileNumber() == m_activeFileNumber)
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct * data = block->getDataList().GetNext(dataPos);

                  if (data != NULL && data->getInsert() != NULL &&
                     data->getInsert()->getInsertType() == insertTypePcbComponent &&
                     selGeom == doc->getBlockAt(data->getInsert()->getBlockNumber()))
                  {
                     UpdateTreePeculiarFashion(selectedItem, data);
                  }
               }

            }
         }
         

         // This isn't really valid. It assumes all items inserting the same geometry
         // are of the same device type as the first one in the list. We have sample
         // data on hand where geometry "1206" is inserted by both Capacitors and Resistors.
         ////DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(m_pinsTree.GetChildItem(selectedItem));
         ////UpdatePinsGrid(selData);
         ////SetPinTypeOptionMenu(selData);        
         // Could possibly detect if geoms in list have more than one device type,
         // go ahead and set like PNs if only one dev type, otherwise clear/disable.

         // At parent Geom level, clear pin map grid and device type name.
         m_flexGrid.EditCell();
         GetDlgItem(IDC_STATIC_DT)->SetWindowText(""/*deviceType*/);

         UpdateNodeStatusIndicators(selectedItem);
      }
      else //it is a selected component.  Could need some checking to make sure that it is a component
      {
         DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(selectedItem);
         UpdatePinsGrid(selData);
         SetPinTypeOptionMenu(selData);
      }
   }
   else if (m_radioGeom == 1) // if PART NUMBER mode
   {
      if (m_pinsTree.GetParentItem(selectedItem)) // if component is selected
      {
         DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(selectedItem);
         UpdatePinsGrid(selData);
         SetPinTypeOptionMenu(selData);
      }
      else if (!m_pinsTree.GetParentItem(selectedItem)) //if it is a part number
      {
         DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(m_pinsTree.GetChildItem(selectedItem));
         UpdatePinsGrid(selData);
         SetPinTypeOptionMenu(selData);
         UpdateNodeStatusIndicators(selectedItem);
      }

   }
   else if (m_radioGeom == 2) //if prefix is selected
   {
      HTREEITEM selectedItem = m_pinsTree.GetSelectedItem();

      if (!selectedItem)
         return;

      if (!m_pinsTree.GetParentItem(selectedItem)) // if the prefix is selected
      {
         void* voidPtr = NULL;
         CString pre = m_pinsTree.GetItemText(selectedItem);

         for (int i = 0; i < doc->getMaxBlockIndex(); i++)
         {
            BlockStruct* block = doc->getBlockAt(i);

            if (block != NULL && block->getFileNumber() == m_activeFileNumber)
            {
               POSITION dataPos = block->getDataList().GetHeadPosition();
               while (dataPos)
               {
                  DataStruct *data = block->getDataList().GetNext(dataPos);

                  if (data != NULL && data->getInsert() != NULL &&
                     data->getInsert()->getInsertType() == insertTypePcbComponent)
                  {
                     CString tempPre = data->getInsert()->getRefname();

                     if (!tempPre.TrimRight("0123456789_.").CompareNoCase(pre))
                     {
                        UpdateTreePeculiarFashion(selectedItem, data);
                     }
                  }
               }
            }
         }
      }
      else if (m_pinsTree.GetParentItem(selectedItem)) // if component is selected
      {
         DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(selectedItem);
         UpdatePinsGrid(selData);
         SetPinTypeOptionMenu(selData);
      }
   }

   prevSelected = selectedItem;
   *pResult = 0;
}

void CDeviceTyperPinPropertyPage::UpdateNodeStatusIndicators(HTREEITEM selectedItem)
{
   if (selectedItem != NULL)
   {
      HTREEITEM treeitem = selectedItem; // assume selectedItem is a leaf (sibling) in tree
      if (!m_pinsTree.GetParentItem(selectedItem))
         treeitem = m_pinsTree.GetChildItem(selectedItem); // selectedItem is root in tree

      while (treeitem)
      {
         DataStruct *data = (DataStruct*)m_pinsTree.GetItemData(treeitem);
         int colorDot = Devices.GetPinMapStatusIcon(data);

         m_pinsTree.SetItemImage(treeitem, colorDot, colorDot);
         treeitem = m_pinsTree.GetNextSiblingItem(treeitem);
      }
   }
}

void CDeviceTyperPinPropertyPage::UpdatePinsGrid(DataStruct *componentData)
{
   if (componentData != NULL && componentData->getInsert() != NULL)
   {
      BlockStruct *insertedBlock = doc->getBlockAt(componentData->getInsert()->getBlockNumber());

      if (insertedBlock != NULL)
      {
         int rowNum = 1;

         POSITION pos = insertedBlock->getDataList().GetHeadPosition();
         while (pos)
         {
            DataStruct* pinData = insertedBlock->getDataList().GetNext(pos);

            if (pinData != NULL && pinData->getInsert() != NULL &&
               pinData->getInsert()->getInsertType() == INSERTTYPE_PIN)
            {
               setItemToGrid(rowNum, 0, pinData->getInsert()->getRefname());

               CString pinType;
               if (IsPinTyped(componentData, pinData, pinType))
                  setItemToGrid(rowNum, 1, pinType.Trim());

               rowNum++;
            }
         }
      }
   }
}

void CDeviceTyperPinPropertyPage::UpdateTreePeculiarFashion(HTREEITEM selectedItem, DataStruct *data)
{
   // This pattern was repeated in several places, I consolidated it.
   // And then distilled it.
   // It seem rather peculiar to look for something in the tree,
   // and if found, delete. Then add it to the tree. Maybe it is
   // a work-around for a bug in the tree tool? I kept the style,
   // just in case it does something useful that is not obvious.
   // It can be looked into... some other time.

   if (selectedItem != NULL && data != NULL && data->getInsert() != NULL)
   {
      CString refName = data->getInsert()->getRefname();
      HTREEITEM root = m_pinsTree.GetChildItem(selectedItem);
      m_pinsTree.DeleteItem(root, refName);

      DeviceTypeStruct *device = Devices.GetDevice(data);

      if (device != NULL && (!device->expPinNames.IsEmpty() || Devices.HasPinCountAnomaly(data)))
      {
         int colorDot = Devices.GetPinMapStatusIcon(data);

         HTREEITEM compItem = m_pinsTree.InsertItem(refName, colorDot, colorDot, selectedItem, TVI_SORT);
         m_pinsTree.SetItemData(compItem, (DWORD)data);
      }
   }
}

CString CDeviceTyperPinPropertyPage::GetPinTypeOptionMenuString(CString deviceType)
{
   CString optionStr;

   if (!deviceType.IsEmpty())
   {
      DeviceTypeStruct* device = Devices.FindDeviceTypeByName(deviceType);

      if (device != NULL && device->expPinNames.GetLength() > 0)
      {
         optionStr = device->expPinNames;
         optionStr.Replace(" ", "|");
         optionStr += "|"; // ready to add No-Connect
      }
   }

   if (optionStr.IsEmpty())
      optionStr = (CString)(PINMAP_NONE) + (CString)("|"); // Offer a blank choice, main purpose is to clear No-Connect

   return optionStr + "No-Connect";
}

bool CDeviceTyperPinPropertyPage::SetPinTypeOptionMenu(DataStruct *data)
{
   if (data != NULL && data->getAttributesRef() != NULL)
   {
      CString deviceType;
      Attrib* attrib = NULL;

      if (data->getAttributesRef()->Lookup(tempDeviceTypeKeyword,attrib) ||
          data->getAttributesRef()->Lookup(deviceTypeKeyword,attrib))
      {
         if (attrib && attrib->getValueType() == VT_STRING)
         {
            deviceType = attrib->getStringValue();
         }

         CString Buf = GetPinTypeOptionMenuString(deviceType);

         m_flexGrid.put_ColComboList(1, Buf);
         ///m_flexGrid.EditCell();

         GetDlgItem(IDC_STATIC_DT)->SetWindowText(deviceType);

         return true;
      }
   }

   // Couldn't find options, so didn't set options
   return false;
}

void CDeviceTyperPinPropertyPage::GridSetup()
{
   BSTR licenseKey = NULL;
   CRect rc;
       
   if (!GetLicenseString(m_flexGrid.GetClsid(), licenseKey))
      licenseKey = NULL;
      
   GetDlgItem(IDC_OCXBOX)->ShowWindow(SW_HIDE);
   GetDlgItem(IDC_OCXBOX)->GetWindowRect(&rc);

   ScreenToClient(&rc);

   m_flexGrid.Create("FlexGrid", WS_VISIBLE, rc, this, IDC_VSFLEX,
                  NULL, FALSE, licenseKey);

   m_flexGrid.put_Editable(CFlexGrid::flexEDNone);
   m_flexGrid.put_ExplorerBar(CFlexGrid::flexExSortShow);
   m_flexGrid.put_AllowSelection(TRUE);
   m_flexGrid.put_SelectionMode(CFlexGrid::flexSelectionByRow);
   m_flexGrid.put_AllowBigSelection(FALSE);
   m_flexGrid.put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   m_flexGrid.put_GridLines(CFlexGrid::flexGridFlat);
   m_flexGrid.put_ExtendLastCol(TRUE);
   m_flexGrid.put_WordWrap(FALSE);
   m_flexGrid.put_AutoSearch(CFlexGrid::flexSearchFromCursor);
   m_flexGrid.put_AutoResize(FALSE);
   m_flexGrid.put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   m_flexGrid.put_ColAlignment(0,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(1,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(2,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(3,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(4,CFlexGrid::flexAlignLeftBottom);
   m_flexGrid.put_ColAlignment(5,CFlexGrid::flexAlignLeftBottom);     
     
   m_flexGrid.put_Cols(2);
   m_flexGrid.put_Rows(1);
   m_flexGrid.put_FixedCols(0);
   m_flexGrid.put_FixedRows(1);
      
   COleVariant row1, col, newValue;
   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)0;
   newValue = "PIN";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);
      
   row1 = (GRID_CNT_TYPE)0;
   col = (GRID_CNT_TYPE)1;
   newValue = "MAPPING";
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col, row1, col, newValue);
}

void CDeviceTyperPinPropertyPage::setItemToGrid(GRID_CNT_TYPE row, GRID_CNT_TYPE col, CString itemValue)
{
    COleVariant row1(row), col1(col), newValue(itemValue);

   if (row >= m_flexGrid.get_Rows())
        m_flexGrid.put_Rows(row+1);
   
   m_flexGrid.put_Cell(CFlexGrid::flexcpText, row1, col1, row1, col1, newValue);
}

void CDeviceTyperPinPropertyPage::AfterEditPinsGrid(long row, long col)
{
   if (row > 0 && col == 1)
   {
      COleVariant olerow, olecol;
      olerow = row;
      olecol = col;
      CString val = m_flexGrid.get_Cell(CFlexGrid::flexcpText, olerow, olecol, olerow, olecol);
      val.Trim();
      if (val.Compare(PINMAP_NONE) == 0)
      {
         COleVariant newValue;
         newValue = "";
         m_flexGrid.put_Cell(CFlexGrid::flexcpText, olerow, olecol, olerow, olecol, newValue);
      }
   }
}

void CDeviceTyperPinPropertyPage::MouseUpVsflex(short Button, short Shift, float X, float Y)
{
   COleVariant col1;
   long rowIndex = m_flexGrid.get_Row();
   col1 = (GRID_CNT_TYPE)1;

   if (m_flexGrid.get_Col() == 1 && rowIndex > 0)
   {
      m_flexGrid.EditCell();     
   }

   Updated = TRUE;
}

void CDeviceTypeArray::Init(CCEtoODBDoc *doc)
{
   m_doc = doc;
   m_deviceTypeKeyword = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);
   m_tempDeviceTypeKeyword = doc->RegisterKeyWord("TEMPDEVICETYPE", 0, VT_STRING);
   m_deviceToPackagePinMap = doc->RegisterKeyWord(ATT_DEVICETOPACKAGEPINMAP, 0, VT_STRING);
}

void CDeviceTypeArray::DestroyAll()
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      if (this->GetAt(i) != NULL)
         delete this->GetAt(i);

      this->SetAt(i, NULL);
   }

   this->RemoveAll();
}

DeviceTypeStruct *CDeviceTypeArray::FindDeviceTypeByName(CString deviceName)
{
   for (int i = 0; i < this->GetCount(); i++)
   {
      DeviceTypeStruct* device = Devices[i];

      if (device != NULL)
      {
         if (!device->deviceTypeName.CompareNoCase(deviceName))
         return device;
      }
   }

   return NULL;
}

DeviceTypeStruct *CDeviceTypeArray::GetDevice(DataStruct *data)
 {
    if (data != NULL && data->getAttributesRef() != NULL)
    {
       Attrib* attrib;

       if (data->getAttributesRef()->Lookup(m_tempDeviceTypeKeyword,attrib) ||
           data->getAttributesRef()->Lookup(m_deviceTypeKeyword,attrib))
       {
          CString deviceType;

          if (attrib && attrib->getValueType() == VT_STRING)
          {
             deviceType = attrib->getStringValue();
          }

          DeviceTypeStruct* device = FindDeviceTypeByName(deviceType);
          return device;
       }
    }

    return NULL;
 }

bool CDeviceTypeArray::IsPinTyped(CompPinStruct *compPin,  CString &pinType)
{
   pinType = "";

   Attrib* attrib = NULL;

   if (compPin != NULL &&
       compPin->getAttributesRef() != NULL &&
       compPin->getAttributesRef()->Lookup(m_deviceToPackagePinMap, attrib))
   {
      if (attrib != NULL && attrib->getValueType() == VT_STRING)
      {
         pinType = attrib->getStringValue();
      } 

      if (!pinType.IsEmpty())
         return true;
   }

   // No comp pin or no pin map attr or pin map attr is not considered mapped
   return false;  
}

PinmapViolationTag CDeviceTypeArray::GetPinMapViolation(DataStruct *data)
{
   // This is going to be slow, all that searching for compPins.
   // We really need to cache compPins earlier on.

   CString compRefname;
   BlockStruct *insertedBlock = NULL;

   
   if (data != NULL && data->isInsertType(insertTypePcbComponent))
   {
      compRefname = data->getInsert()->getRefname();
      int insertedBlockNum = data->getInsert()->getBlockNumber();
      insertedBlock = m_doc->getBlockAt(insertedBlockNum);
   }

   if (insertedBlock != NULL)
   {
      bool allPinsMapped = true; // optimist
      bool somePinsMapped = false; // pessimist
      bool illegalYellow = false;
      bool illegalRed = false;

      int devicePinTypeCount = 0;
      int noConnectPinCount = 0;

      DeviceTypeStruct *device = this->GetDevice(data);
      if (device == NULL)
         return pinmapDeviceUnassigned;

      if (device != NULL)
      {
         CString pinNameList = device->expPinNames;

         // Check that all pins in device are mapped to some component pin
         int curPos = 0;
         CString devicePinType = pinNameList.Tokenize(" ",curPos);

         while (!devicePinType.IsEmpty())
         {
            devicePinTypeCount++;
            int foundCount = 0;

            POSITION insertPos = insertedBlock->getHeadDataInsertPosition();
            while (insertPos != NULL)
            {
               DataStruct *pinData = insertedBlock->getNextDataInsert(insertPos);
               InsertStruct *pinInsert = pinData->getInsert();
               if (pinInsert != NULL && pinInsert->getInsertType() == insertTypePin)
               {
                  CString pinRefname = pinInsert->getRefname();
                  NetStruct *net = NULL;
                  CompPinStruct *compPin = FindCompPin_ByName(m_doc, compRefname, pinRefname, NULL, &net);

                  CString pinType;
                  if (IsPinTyped(compPin, pinType))
                  {
                     pinType.Trim();

                     if (pinType.CompareNoCase(devicePinType) == 0)
                     {
                        foundCount++;
                     }
                  }
               }
            }

            if (foundCount != 1)
            {
               // Either pinType is not mapped to any pin or is mapped to
               // more than one pin. Either case, yellow error.
               illegalYellow = true;
            }

            devicePinType = pinNameList.Tokenize(" ", curPos);
         }

      }

      // Check that all pins are mapped to something, and
      // count no-connect pins. Do this even if the device has no pin types, we still
      // need to check on No-Connect's anyway, for pin count.
      noConnectPinCount = 0;
      int componentPinCount = 0;
      POSITION insertPos = insertedBlock->getHeadDataInsertPosition();
      while (insertPos != NULL)
      {
         DataStruct *pinData = insertedBlock->getNextDataInsert(insertPos);
         InsertStruct *pinInsert = pinData->getInsert();
         if (pinInsert != NULL && pinInsert->getInsertType() == insertTypePin)
         {
            componentPinCount++;

            CString pinRefname = pinInsert->getRefname();
            NetStruct *net = NULL;
            CompPinStruct *compPin = FindCompPin_ByName(m_doc, compRefname, pinRefname, NULL, &net);

            CString pinType;
            if (IsPinTyped(compPin, pinType))
            {
               if (pinType.IsEmpty())
               {
                  allPinsMapped = false;
               }
               else
               {
                  somePinsMapped = true;

                  if (pinType.MakeUpper().Find("CONNECT") > -1)
                  {
                     noConnectPinCount++;
                  }
               }
            }
            else
            {
               allPinsMapped = false;
            }
         }
      }

      // Special case override. Actually, a pretty common case.
      // If device pin type list is empty, then mapping is not required.
      if (device != NULL && device->expPinNames.IsEmpty())
         allPinsMapped = somePinsMapped = true;  // little white lie

      // Check pin count, "no connect" pins are considered non-existent
      ////if ((componentPinCount - noConnectPinCount) != devicePinTypeCount)
      ////  illegalYellow = true;
      if (device != NULL)
      {
         int activePinCount = componentPinCount - noConnectPinCount;
         if (activePinCount < device->minPinCount || (device->maxPinCount != -1 && activePinCount > device->maxPinCount))
            illegalRed = true;
      }


      // Red violations take precedence, then Yellow, then non-violation Green
      if (illegalRed || !somePinsMapped)
         return pinmapRedViolation;
      else if (illegalYellow || !allPinsMapped)
         return pinmapYellowViolation;

      return pinmapNoViolation;

   }

   // No part, no foul
   return pinmapNoViolation;
}

int CDeviceTypeArray::GetPinMapStatusIcon(DataStruct *selectedData)
{
   int colorDot = ICON_HOLLOW_DOT;

   if (selectedData != NULL)
   {
      PinmapViolationTag vio = this->GetPinMapViolation(selectedData);
      
      if (vio == pinmapRedViolation)
         colorDot = ICON_RED_DOT;
      else if (vio == pinmapYellowViolation)
         colorDot = ICON_YELLOW_DOT;
      else if (vio == pinmapNoViolation)
         colorDot = ICON_GREEN_DOT;
   }

   return colorDot;
}

void CDeviceTyperPinPropertyPage::UpdateNodeStatus(HTREEITEM treeitem)
{
   if (treeitem)
   {
      DataStruct *data = (DataStruct*)m_pinsTree.GetItemData(treeitem);

      int colorDot = Devices.GetPinMapStatusIcon(data);
      m_pinsTree.SetItemImage(treeitem, colorDot, colorDot);

      UpdateData();
   }
}

void CDeviceTyperPinPropertyPage::UpdateItemsInTree(HTREEITEM child, HTREEITEM selectedItem)
{
   // Incoming child should be first child under parent

   //If flexgrid rows is zero, there is nothing to do, just exit (time saver).
   if (m_flexGrid.get_Rows() < 1)
      return;

   // To qualify for update, the child component must have the same device type
   // as the selectedItem component.
   DeviceTypeStruct *selectedDevice = NULL;
   if (selectedItem)
   {
      DataStruct * selData = (DataStruct*)m_pinsTree.GetItemData(selectedItem);
      selectedDevice = Devices.GetDevice(selData);
   }


   while (child)
   {
      DataStruct * data = (DataStruct*)m_pinsTree.GetItemData(child);

      // If we have data and the data's device matches the selected device, do update.
      if (data != NULL && (selectedDevice == NULL || selectedDevice == Devices.GetDevice(data)))
      {
         void* voidPtr = NULL;

         POSITION filePos = doc->getFileList().GetHeadPosition();
         while (filePos)
         {
            FileStruct* file = doc->getFileList().GetNext(filePos);

            if (!file)
               continue;

            POSITION netPos = file->getNetList().GetHeadPosition();
            while (netPos)
            {
               NetStruct* net = file->getNetList().GetNext(netPos);

               if (!net)
                  continue;

               POSITION compPinPos = net->getHeadCompPinPosition();
               while (compPinPos)
               {
                  CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                  if (!compPin)
                     continue;

                  if (!compPin->getRefDes().CompareNoCase(data->getInsert()->getRefname()))
                  {
                     // get the pin name
                     // compare it with the grid and then add the attribute
                     COleVariant rowi, col0, col1;
                     col0 = (GRID_CNT_TYPE)0;
                     col1 = (GRID_CNT_TYPE)1;

                     for (int i = 1; i< m_flexGrid.get_Rows();i++)
                     {
                        rowi = (GRID_CNT_TYPE)i;

                        if (!compPin->getPinName().CompareNoCase((CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col0, rowi, col0)))
                        {
                           CString type = (CString)m_flexGrid.get_Cell(CFlexGrid::flexcpText, rowi, col1, rowi, col1);
                           type.Trim();

                           if (type.Find("Connect",0) != -1)
                              voidPtr = (void*)ATT_VALUE_NO_CONNECT;
                           else
                              voidPtr = (void*)type.GetString();

                           doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);

                           break;
                        }
                     }
                  }
               }
            } 
         }
      }
      
      UpdateNodeStatus(child);

      child = m_pinsTree.GetNextSiblingItem(child);
   } 
}

void CDeviceTyperPinPropertyPage::OnBnClickedOk()
{
   // OK aka "Update" in GUI

   HTREEITEM selectedItem = m_pinsTree.GetSelectedItem();

   if (!selectedItem)
      return;

   if (m_pinsTree.GetParentItem(m_pinsTree.GetSelectedItem()))  
   {
      // It is a CHILD entry
      // Get the first child of parent to update all siblings
      HTREEITEM firstchild = m_pinsTree.GetChildItem(m_pinsTree.GetParentItem(selectedItem));

      UpdateItemsInTree(firstchild, selectedItem);
   }
   else
   {
      // It is a PARENT.  We need to update all child items.
      // Is only valid in PartNumber mode.
      // If any other mode, skip it.

      if (m_radioGeom == 1)
      {
         HTREEITEM selectedItem = m_pinsTree.GetSelectedItem();
         HTREEITEM firstchild = m_pinsTree.GetChildItem(selectedItem);

         UpdateItemsInTree(firstchild, NULL);
      }
   }


   //if update file is checked//
   if (m_addToStpFile.GetCheck() == BST_CHECKED)
   {
      CString geomPinMapStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_GEOMPINMAP_STP) );
      CString PNPinMapStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_PNPINMAP_STP) );

      FILE *fp = NULL;

      if (m_radioGeom == 0)
      {
         if ((fp = fopen(geomPinMapStpFilename,"a")) == NULL)
         {
            // no settings file found
            CString tmp;
            tmp.Format("File [%s] not found",geomPinMapStpFilename);
            MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

            return;
         }

         HTREEITEM root = m_pinsTree.GetRootItem();

         while (root)
         {
            int nImage, nSelImage;
            int nImage1, nSelImage1;
            m_pinsTree.GetItemImage(m_pinsTree.GetChildItem(root), nImage1, nSelImage1);
            m_pinsTree.GetItemImage(root, nImage, nSelImage);
            CString rootText = m_pinsTree.GetItemText(root);
            CString selText  = m_pinsTree.GetItemText(selectedItem);

            if (nImage == ICON_FOLDER && nImage1 == ICON_GREEN_DOT || 
               !rootText.CompareNoCase(selText))
            {
               CString geomName = m_pinsTree.GetItemText(root);
               COleVariant rowi, col0, col1;
               col0 = (GRID_CNT_TYPE)0;
               col1 = (GRID_CNT_TYPE)1;
               
               HTREEITEM subRoot = m_pinsTree.GetChildItem(root);

               while (subRoot)
               {
                  DataStruct* comp = (DataStruct*)m_pinsTree.GetItemData(subRoot);

                  if (!comp)
                     continue;

                  BlockStruct* geom = doc->getBlockAt(comp->getInsert()->getBlockNumber());

                  if (!geom)
                     continue;

                  POSITION pinPos = geom->getHeadDataInsertPosition();
                  while (pinPos)
                  {
                     DataStruct* pin = geom->getNextDataInsert(pinPos);

                     if (!pin || pin->getInsert()->getInsertType() != insertTypePin)
                        continue;
                     
                     CString pinName;
                     CString pinNum = pin->getInsert()->getRefname();

                     if (IsPinTyped(comp, pin, pinName) && !ItemIsInArray(geomName + " " + pinNum + " " + pinName, 1, geomPinMapsCount))
                     {
                        fprintf(fp, "%s,%s,%s\n", geomName, pinNum, pinName);
                        geomPinMaps.SetAtGrow(geomPinMapsCount++, geomName + " " + pinNum + " " + pinName);
                     }
                  }

                  subRoot = m_pinsTree.GetNextSiblingItem(subRoot);
               }
            }

            root = m_pinsTree.GetNextSiblingItem(root);
         }

         fclose(fp);
      }
      else if (m_radioGeom == 1)
      {
         if ((fp = fopen(PNPinMapStpFilename,"a")) == NULL)
         {
            // no settings file found
            CString tmp;
            tmp.Format("File [%s] not found",PNPinMapStpFilename);
            MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

            return;
         }

         HTREEITEM root = m_pinsTree.GetRootItem();

         while (root)
         {
            int nImage, nSelImage;
            int nImage1, nSelImage1;
            m_pinsTree.GetItemImage(m_pinsTree.GetChildItem(root), nImage1, nSelImage1);
            m_pinsTree.GetItemImage(root, nImage, nSelImage);

            if (nImage == ICON_FOLDER && nImage1 == ICON_GREEN_DOT)
            {
               CString partNum = m_pinsTree.GetItemText(root);
               COleVariant rowi, col0, col1;
               col0 = (GRID_CNT_TYPE)0;
               col1 = (GRID_CNT_TYPE)1;
               
               HTREEITEM subRoot = m_pinsTree.GetChildItem(root);

               while (subRoot)
               {
                  DataStruct* comp = (DataStruct*)m_pinsTree.GetItemData(subRoot);

                  if (!comp)
                     continue;

                  BlockStruct* geom = doc->getBlockAt(comp->getInsert()->getBlockNumber());

                  if (!geom)
                     continue;

                  POSITION pinPos = geom->getHeadDataInsertPosition();
                  while (pinPos)
                  {
                     DataStruct* pin = geom->getNextDataInsert(pinPos);

                     if (!pin || pin->getInsert()->getInsertType() != insertTypePin)
                        continue;
                     
                     CString pinName;
                     CString pinNum = pin->getInsert()->getRefname();

                     if (IsPinTyped(comp, pin, pinName) && !ItemIsInArray(partNum + " " + pinNum + " " + pinName, 0, PNPinMapsCount))
                     {
                        fprintf(fp, "%s,%s,%s\n", partNum, pinNum, pinName);
                        PNPinMaps.SetAtGrow(PNPinMapsCount++, partNum + " " + pinNum + " " + pinName);
                     }
                  }

                  subRoot = m_pinsTree.GetNextSiblingItem(subRoot);
               }
            }

            root = m_pinsTree.GetNextSiblingItem(root);
         }

         fclose(fp);
      }
   }   

   prevSelected = NULL;
   messageSent  = FALSE;
}

//-------------------------------------------------------------------------

bool CDeviceTyperPinPropertyPage::PartNumberAttributePresent()
{
   WORD pnKw = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);
   Attrib *attrib = NULL;

   for (int i = 0; i < doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (block!= NULL && block->getFileNumber() == m_activeFileNumber)
      {
         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
               data->getAttributesRef()->Lookup(pnKw, attrib))
            {
               if (attrib)
               {
                  CString pnVal = attrib->getStringValue();
                  if (!pnVal.IsEmpty())
                     return true;  // found one, only need one
               }
            }
         }
      }
   }

   return false;
}
/****************************************************************
*/
int CDeviceTypeArray::GetRawPinCount(DataStruct *compData)
{
   int pinCount = 0;
   BlockStruct *insertedBlock = NULL;

   if (compData != NULL && compData->isInsertType(insertTypePcbComponent))
   {
      int insertedBlockNum = compData->getInsert()->getBlockNumber();
      insertedBlock = m_doc->getBlockAt(insertedBlockNum);
   }

   if (insertedBlock != NULL)
   {  
      POSITION insertPos = insertedBlock->getHeadDataInsertPosition();
      while (insertPos != NULL)
      {
         DataStruct *pinData = insertedBlock->getNextDataInsert(insertPos);
         if (pinData != NULL && pinData->isInsertType(insertTypePin))
            pinCount++;
      }
   }

   return pinCount;
}

/****************************************************************
*/
bool CDeviceTypeArray::HasPinCountAnomaly(DataStruct *compData)
{
   DeviceTypeStruct *device = this->GetDevice(compData);

   if (device != NULL)
   {
      int pinCnt = this->GetRawPinCount(compData);
      bool pinCntOk = (pinCnt >= device->minPinCount && (pinCnt <= device->maxPinCount || device->maxPinCount == -1));
      return !pinCntOk;
   }

   return false;
}

/****************************************************************
*/
void CDeviceTyperPinPropertyPage::fillPartNumberTree()
{

   WORD pnKw = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);
   Attrib* attrib = NULL;

   for (int j=0; j<doc->getMaxBlockIndex(); j++)
   {
      BlockStruct *block = doc->getBlockAt(j);

      if (!block)
         continue;

      if (block->getFileNumber() != m_activeFileNumber)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         BOOL alreadyMapped = FALSE;
         attrib = NULL;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (data && data->getAttributesRef() && data->getInsert() && data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT &&
            data->getAttributesRef()->Lookup(pnKw, attrib))
         {
            // Must have partnumber to be put in part number tree!
            if (!attrib)
               continue;

            CString pnVal = attrib->getStringValue();

            if (pnVal.IsEmpty())
               continue;

            // Must have device assignment
            DeviceTypeStruct *device = Devices.GetDevice(data);
            
            // If comp has device assignment and device has non-empty pin name list or
            // component raw pin count does not match device pin count, then put it in the list
            if (device != NULL && (!device->expPinNames.IsEmpty() || Devices.HasPinCountAnomaly(data)))
            {
               // Okay, it is going in the tree
               CString refName = data->getInsert()->getRefname();           
               const KeyWordStruct* kw = doc->getKeyWordArray()[pnKw];
               //BlockStruct* geom = doc->getBlockAt(data->getInsert()->getBlockNumber());

               int colorDot = Devices.GetPinMapStatusIcon(data);
               
               BOOL added = FALSE;
               HTREEITEM root = m_pinsTree.GetRootItem();

               while (root)
               {
                  if (!m_pinsTree.GetItemText(root).CompareNoCase(pnVal))
                  {
                     HTREEITEM compItem = m_pinsTree.InsertItem(refName, colorDot, colorDot, root, TVI_SORT);
                     m_pinsTree.SetItemData(compItem, (DWORD)data);
                     added = TRUE;

                     break;
                  }

                  root = m_pinsTree.GetNextSiblingItem(root);
               }

               if (!added)
               {
                  HTREEITEM pnItem = m_pinsTree.InsertItem(pnVal, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
                  m_pinsTree.SetItemData(pnItem, (DWORD)kw);               

                  HTREEITEM compItem = m_pinsTree.InsertItem(refName, colorDot, colorDot, pnItem, TVI_SORT);
                  m_pinsTree.SetItemData(compItem, (DWORD)data);              
               }
            }

         }
      }
   }

}

void CDeviceTyperPinPropertyPage::GetPrefix()
{
   // This function was the cause of LONG time delay in opening Pin Map tab.

   // This is a terrible function. It is looping "the long way", with multiple
   // passes through the block list. What's worse, the result is not even used.
   // It is only used if the top level pin map mode is "Ref Des", and that
   // feature is disabled.
   // The code is not nuked, in case we ever want this feature back.
   // But until then, just skip out.   

#ifdef EAT_UP_SOME_CPU_TIME
   FILE *fp;
   char  line[255];
   char  *lp;
   CString PrefixFile( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_REFDESPREFIX_STP) );

   if ((fp = fopen(PrefixFile,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",PrefixFile);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   CString pre;

   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line, ",", 1)) == NULL) 
         continue;

      pre = lp;
      pre.TrimLeft();
      pre.TrimRight();

      if (pre.GetLength() == 0)
         continue;

      BOOL keep = FALSE;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         BlockStruct* block = doc->getBlockAt(j);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct* data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
               continue;

            CString refName = data->getInsert()->getRefname();

            PinmapViolationTag vio = Devices.GetPinMapViolation(data);

            if (vio == pinmapNoViolation || vio == pinmapDeviceUnassigned)
               continue;

            CString currentPre = data->getInsert()->getRefname();
            
            int index = currentPre.FindOneOf(".0123456789_");

            if (index >0)
               currentPre = currentPre.Left(index);

            if (!currentPre.CompareNoCase(pre))
               keep = TRUE;
         }
      }

      if (keep)
      {
         prefixArray.SetAtGrow(prefixArrayCount++, pre);
      }

      UpdateData();
   }

   fclose(fp);
#endif
}

void CDeviceTyperPinPropertyPage::GetGeomPinMapping()
{
   FILE *fp;
   char  line[255];
   char  *lpGeom;
   CString pinMapFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_GEOMPINMAP_STP) );

   if ((fp = fopen(pinMapFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",pinMapFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }    

   while (fgets(line,255,fp))
   {
      CString tmp;
      CString tmp1;
      CString tmp2;
      CString tmp3;

      //geom name
      if ((lpGeom = get_string(line, ",", 1)) == NULL) 
         continue;

      tmp1 = lpGeom;
      tmp1.Trim();
      
      //pin #
      if ((lpGeom = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp2 = lpGeom;
      tmp2.Trim();

      //pin name
      if ((lpGeom = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp3 = lpGeom;
      tmp3.Trim();

      tmp.Format("%s %s %s", tmp1, tmp2, tmp3);
      tmp.Trim();

      if (ItemIsInArray(tmp, 1, geomPinMapsCount))
         continue;

      geomPinMaps.SetAtGrow(geomPinMapsCount++, tmp);
      
   }

   fclose(fp);
}

void CDeviceTyperPinPropertyPage::GetPartNumPinMapping()
{
   FILE *fp;
   char  line[255];
   char  *lpPN;
   CString pinMapStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_PNPINMAP_STP) );

   if ((fp = fopen(pinMapStpFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",pinMapStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }

   while (fgets(line,255,fp))
   {
      CString tmp;
      CString tmp1;
      CString tmp2;
      CString tmp3;

      //part number
      if ((lpPN = get_string(line, ",", 1)) == NULL) 
         continue;

      tmp1 = lpPN;
      tmp1.Trim();
      
      //pin name
      if ((lpPN = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp2 = lpPN;
      tmp2.Trim();

      //pin type
      if ((lpPN = get_string(NULL, ",", 1)) == NULL) 
         continue;

      tmp3 = lpPN;
      tmp3.Trim();

      tmp.Format("%s %s %s", tmp1, tmp2, tmp3);
      tmp.Trim();
      
      if (ItemIsInArray(tmp, 0, PNPinMapsCount))
         continue;

      PNPinMaps.SetAtGrow(PNPinMapsCount++, tmp);
      
   }

   fclose(fp);
}

BOOL CDeviceTyperPinPropertyPage::ItemIsInArray(CString item, int pnOrGeom, int arrayCount)
{
   for (int i=0; i < arrayCount; i++)
   {
      if (pnOrGeom == 0)
      {
         if (PNPinMaps.GetAt(i).Find(item, 0) != CB_ERR)
            return TRUE;
      }
      else if (pnOrGeom == 1)
      {
         if (geomPinMaps.GetAt(i).Find(item, 0) != CB_ERR)
            return TRUE;
      }
   }

   return FALSE;
}

void CDeviceTyperPinPropertyPage::OnOK()
{
   geomPinMaps.RemoveAll();

   PNPinMaps.RemoveAll();
   CPropertyPage::OnOK();
}

void CDeviceTyperPinPropertyPage::UpdateFromFile()
{
   for (int i=0; i<geomPinMapsCount; i++)
   {
      int nIndex = geomPinMaps.GetAt(i).FindOneOf(" ");

      if (nIndex < 0)
         continue;

      CString geomName = geomPinMaps.GetAt(i).Left(nIndex);
      BlockStruct *geom = doc->Find_Block_by_Name(geomName, -1);

      if (!geom || geom->getBlockType() != BLOCKTYPE_PCBCOMPONENT)
         continue;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         BlockStruct *block = doc->getBlockAt(j);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT
               || geom != doc->getBlockAt(data->getInsert()->getBlockNumber()))
               continue;

            void* voidPtr = NULL;

            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos)
            {
               FileStruct* file = doc->getFileList().GetNext(filePos);

               if (!file)
                  continue;

               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct* net = file->getNetList().GetNext(netPos);

                  if (!net)
                     continue;

                  POSITION compPinPos = net->getHeadCompPinPosition();
                  while (compPinPos)
                  {
                     CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                     if (!compPin)
                        continue;

                     if (!compPin->getRefDes().CompareNoCase(data->getInsert()->getRefname()))
                     {
                        // get the pin name     
                        CString tempName = geomPinMaps.GetAt(i);
                        nIndex = tempName.FindOneOf(" ");
                        CString pinName = tempName.Right(tempName.GetLength() - nIndex - 1);
                        nIndex = pinName.FindOneOf(" ");

                        if (nIndex > 0)
                           pinName = pinName.Left(nIndex);

                        nIndex = tempName.ReverseFind(' ');
                        CString type = tempName.Right(tempName.GetLength() - nIndex - 1);
                        type.Trim();

                        if (!compPin->getPinName().CompareNoCase(pinName))
                        {
                           if (type.Find("Connect",0) != -1)
                              voidPtr = (void*)ATT_VALUE_NO_CONNECT;
                           else
                              voidPtr = (void*)type.GetString();

                           doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
                           UpdateData();
                        }
                     }
                  } // close while compPinPos
               }  // close while netPos
            }  // close while filePos
         } // close while dataPos
      }// close block for
   } // close for loop

   for (int i=0; i<PNPinMapsCount; i++)
   {
      int nIndex = PNPinMaps.GetAt(i).FindOneOf(" ");

      if (nIndex < 0)
         continue;

      CString partNum = PNPinMaps.GetAt(i).Left(nIndex);

      WORD pnKw = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);

      Attrib* attrib = NULL;

      for (int j=0; j<doc->getMaxBlockIndex(); j++)
      {
         BlockStruct *block = doc->getBlockAt(j);

         if (!block)
            continue;

         if (block->getFileNumber() != m_activeFileNumber)
            continue;

         POSITION dataPos = block->getDataList().GetHeadPosition();
         while (dataPos)
         {
            attrib = NULL;
            DataStruct *data = block->getDataList().GetNext(dataPos);

            if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT ||
               !data->getAttributesRef()->Lookup(pnKw, attrib))
               continue;

            CString refName = data->getInsert()->getRefname();
            CString pn;

            if (attrib)
               pn = attrib->getStringValue();

            //if data doesn't have the same partnumber, continue
            if (pn.CompareNoCase(partNum))
               continue;

            attrib = NULL;

            POSITION filePos = doc->getFileList().GetHeadPosition();
            while (filePos)
            {
               FileStruct* file = doc->getFileList().GetNext(filePos);

               if (!file)
                  continue;

               POSITION netPos = file->getNetList().GetHeadPosition();
               while (netPos)
               {
                  NetStruct* net = file->getNetList().GetNext(netPos);

                  if (!net)
                     continue;

                  POSITION compPinPos = net->getHeadCompPinPosition();
                  while (compPinPos)
                  {
                     CompPinStruct* compPin = net->getNextCompPin(compPinPos);

                     if (!compPin || compPin->getRefDes().CompareNoCase(refName))
                        continue;

                     CString tempName = PNPinMaps.GetAt(i);
                     nIndex = PNPinMaps.GetAt(i).FindOneOf(" ");
                     CString pinName = tempName.Right(tempName.GetLength() - nIndex - 1);
                     nIndex = pinName.FindOneOf(" ");

                     if (nIndex > 0)
                        pinName = pinName.Left(nIndex);

                     nIndex = tempName.ReverseFind(' ');
                     CString type = tempName.Right(tempName.GetLength() - nIndex - 1);
                     type.Trim();

                     if (compPin->getPinName().CompareNoCase(pinName))
                        continue;

                     void* voidPtr;

                     if (type.Find("Connect",0) != -1)
                        voidPtr = (void*)ATT_VALUE_NO_CONNECT;
                     else
                        voidPtr = (void*)type.GetString();

                     doc->SetAttrib(&compPin->getAttributesRef(), deviceToPackagePinMap, VT_STRING, voidPtr, SA_OVERWRITE, NULL);
                     
                     UpdateData();

                     break;
                  } // close while compinpos
               }// close while netpos
            } // close while filepos
         } // close while datapos
      } // close for block
   } // close for PNPinMaps array
}

/**************************************************
*/
bool CDeviceTyperPinPropertyPage::IsPinTyped(CompPinStruct *compPin,  CString &pinType)
{
   pinType = "";

   Attrib* attrib = NULL;

   if (compPin != NULL &&
       compPin->getAttributesRef() != NULL &&
       compPin->getAttributesRef()->Lookup(deviceToPackagePinMap, attrib))
   {
      if (attrib != NULL && attrib->getValueType() == VT_STRING)
      {
         pinType = attrib->getStringValue();
      } 

      if (!pinType.IsEmpty())
         return true;
   }

   // No comp pin or no pin map attr or pin map attr is not considered mapped
   return false;  
}

/**************************************************
*/
bool CDeviceTyperPinPropertyPage::IsPinTyped(DataStruct* data, DataStruct* pin,  CString &pinType)
{
   pinType = "";

   if (!data)
      return false;

   void* voidPtr = NULL;
   CString refName = data->getInsert()->getRefname();
   CString pinName = pin->getInsert()->getRefname();

   POSITION filePos = doc->getFileList().GetHeadPosition();
   while (filePos)
   {
      FileStruct* file = doc->getFileList().GetNext(filePos);

      if (!file)
         continue;

      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct* net = file->getNetList().GetNext(netPos);

         if (!net)
            continue;

         POSITION compPinPos = net->getHeadCompPinPosition();
         while (compPinPos)
         {
            CompPinStruct* compPin = net->getNextCompPin(compPinPos);

            // Previous version of code here would continue searching all nets for
            // a given compPin refname/pin after finding a match, if that match did not
            // have the deviceToPackagePinMap attribute present. That seemed a waste of
            // time, and case 1188 is about time, i.e. this dialog being slow to react
            // in general. So this has been changed to stop looking after a refname/pinnumber
            // match has been found. CAMCAD, in general, asusmes there will be only one
            // such match, by virtue of the many seaches that correlate compPins and 
            // pcb component inserts on the first match of refname/pinnumber found.

            if (compPin != NULL && (compPin->getRefDes().CompareNoCase(refName) == 0) &&
               (compPin->getPinName().CompareNoCase(pinName) == 0))
            {
               return IsPinTyped(compPin, pinType);
            }
         }
      }
   }

   return false;
}

//////////////////////////////////////////////////////////////////////
// CDeviceTyperStylePropertyPage dialog

IMPLEMENT_DYNAMIC(CDeviceTyperStylePropertyPage, CPropertyPage)
CDeviceTyperStylePropertyPage::CDeviceTyperStylePropertyPage()
   : CPropertyPage(CDeviceTyperStylePropertyPage::IDD)
   , m_showStyle(0)
{
   m_activeFileNumber = 0;
   m_imageList = new CImageList();
}

CDeviceTyperStylePropertyPage::CDeviceTyperStylePropertyPage(CCEtoODBDoc& document)
   : CPropertyPage(CDeviceTyperStylePropertyPage::IDD)
   , m_showStyle(0)
{
   doc = &document;
   m_activeFileNumber = doc->getFileList().GetOnlyShown(blockTypePcb)->getFileNumber();
   m_imageList = new CImageList();
}

CDeviceTyperStylePropertyPage::~CDeviceTyperStylePropertyPage()
{
   if (m_imageList)
   {
      m_imageList->DeleteImageList();

      delete m_imageList;
   }

   Devices.DestroyAll();
}

void CDeviceTyperStylePropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_TREE_PNS, m_PNTree);
   DDX_Control(pDX, IDC_EDIT_STYLE, m_style);
   DDX_Radio(pDX, IDC_RADIO_SHOWSTYLE, m_showStyle);
}

BEGIN_MESSAGE_MAP(CDeviceTyperStylePropertyPage, CPropertyPage)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDC_BUTTON_CONFIG, OnBnClickedButtonConfig)
   ON_BN_CLICKED(IDC_RADIO_SHOWSTYLE, OnBnClickedRadioShowstyle)
   ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PNS, OnTvnSelchangedTreePns)
   ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_PNS, OnTvnItemexpandedTreePns)
   ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_PNS, OnTvnItemexpandingTreePns)
   ON_NOTIFY(NM_CLICK, IDC_TREE_PNS, OnTvnClickTreePns)
   ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
END_MESSAGE_MAP()

// CPropertyPageStyleDeviceType message handlers

/******************************************************************************
* CDeviceTyperStylePropertyPage::OnInitDialog
*/
BOOL CDeviceTyperStylePropertyPage::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   m_imageList->Create(16, 16, TRUE, 32, 0);
   CWinApp *app = AfxGetApp();
   m_imageList->Add(app->LoadIcon(IDI_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_OPEN_FOLDER));
   m_imageList->Add(app->LoadIcon(IDI_FILE));
   m_imageList->Add(app->LoadIcon(IDI_RED_DOT));
   m_imageList->Add(app->LoadIcon(IDI_GREEN_DOT));
   m_imageList->Add(app->LoadIcon(IDI_YELLOW_DOT));
   m_imageList->Add(app->LoadIcon(IDI_DOT_HOLLOW));
   
   if (IsWindow(m_PNTree))
      m_PNTree.SetImageList(m_imageList, TVSIL_NORMAL);

   enabledDevices.SetSize(100,100);
   enabledDevicesCount = 0;

   for (int i=0; i<100; i++)
      enabledDevices[i] = "";

   styleKeyword = doc->RegisterKeyWord(ATT_SUBCLASS, 0, VT_STRING);

   GetEnabledDevices(&enabledDevices);

   compArray.SetSize(100,100);
   compArrayCount = 0;
   sendMessageOnInit = FALSE;

   GetCompsWithEnabledDevices(&compArray);

   UpdateData(FALSE);
   FillTree();
   UpdateData();

   return TRUE;
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::OnSetActive
*/
BOOL CDeviceTyperStylePropertyPage::OnSetActive()
{
   m_showStyle = 0;

   CPropertyPage::OnSetActive();

   GetCompsWithEnabledDevices(&compArray);
   sendMessageOnInit = FALSE;

   FillTree();
   UpdateData();

   return TRUE;
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::FillTree
*/
void CDeviceTyperStylePropertyPage::FillTree()
{
   m_PNTree.ShowWindow(SW_HIDE);
   m_PNTree.SelectItem(NULL);
   m_PNTree.DeleteAllItems();

   WORD PNKeyword = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);
   WORD deviceTypeKeyword = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);
   WORD tempDeviceTypeKeyword = doc->RegisterKeyWord("TEMPDEVICETYPE", 0, VT_STRING);
   Attrib* attrib = NULL;
   CString PNval;
   CString deviceType;
   HTREEITEM dataItem = NULL;
   HTREEITEM pnItem = NULL;
   HTREEITEM typeItem = NULL;
   
   for (int i=0; i<compArrayCount; i++)
   {
      BOOL done = FALSE;
      DataStruct *data = compArray[i];

      if (!data)
         continue;

      data->getAttributesRef()->Lookup(PNKeyword, attrib);

      if (attrib)
         PNval = attrib->getStringValue();

      attrib = NULL;
      data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib);

      /*if (!data->getAttributesRef()->Lookup(deviceTypeKeyword, voidPtr))
      {
         voidPtr = NULL;
         data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, voidPtr);
      }
      else if (data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, voidPtr))
      {
      }*/

      if (attrib)
         deviceType = attrib->getStringValue();

      if (m_showStyle == 1 && data->getAttributesRef()->Lookup(styleKeyword, attrib))
         continue;

      HTREEITEM root = m_PNTree.GetRootItem();

      while (root)
      {
         if (!m_PNTree.GetItemText(root).CompareNoCase(deviceType))
         {
            HTREEITEM subRoot = m_PNTree.GetChildItem(root);

            while (subRoot && !done)
            {
               if (!m_PNTree.GetItemText(subRoot).CompareNoCase(PNval))
               {
                  dataItem = m_PNTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, subRoot, TVI_SORT);
                  m_PNTree.SetItemData(dataItem, (DWORD)data);

                  if (data->getAttributesRef()->Lookup(styleKeyword, attrib))
                     m_PNTree.SetItemImage(dataItem, ICON_GREEN_DOT, ICON_GREEN_DOT); 

                  done = TRUE;

                  break;
               }

               subRoot = m_PNTree.GetNextSiblingItem(subRoot); 
            }

            if (!done)
            {
               pnItem = m_PNTree.InsertItem(PNval, ICON_FOLDER, ICON_OPENFOLDER, root, TVI_SORT);
               m_PNTree.SetItemData(pnItem, (DWORD)NULL);

               dataItem = m_PNTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, pnItem, TVI_SORT);
               m_PNTree.SetItemData(dataItem, (DWORD)data);

               if (data->getAttributesRef()->Lookup(styleKeyword, attrib))
                  m_PNTree.SetItemImage(dataItem, ICON_GREEN_DOT, ICON_GREEN_DOT); 

               done = TRUE;

               break;
            }
         }

         root = m_PNTree.GetNextSiblingItem(root); 
      }

      if (done)
         continue;

      typeItem = m_PNTree.InsertItem(deviceType, ICON_FOLDER, ICON_OPENFOLDER, TVI_ROOT, TVI_SORT);
      m_PNTree.SetItemData(pnItem, (DWORD)NULL);

      pnItem = m_PNTree.InsertItem(PNval, ICON_FOLDER, ICON_OPENFOLDER, typeItem, TVI_SORT);
      m_PNTree.SetItemData(pnItem, (DWORD)NULL);

      attrib = NULL;
      dataItem = m_PNTree.InsertItem(data->getInsert()->getRefname(), ICON_HOLLOW_DOT, ICON_HOLLOW_DOT, pnItem, TVI_SORT);
      m_PNTree.SetItemData(dataItem, (DWORD)data);

      if (data->getAttributesRef()->Lookup(styleKeyword, attrib))
         m_PNTree.SetItemImage(dataItem, ICON_GREEN_DOT, ICON_GREEN_DOT); 
   }

   m_PNTree.SelectItem(m_PNTree.GetFirstVisibleItem());
   m_PNTree.ShowWindow(SW_SHOW);
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::GetEnabledDevices
*/
void CDeviceTyperStylePropertyPage::GetEnabledDevices(CStringArray *enabledDevices)
{
   FILE *fp;
   char  line[255];
   char  *lp;
   CString configStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_CONFIG_STP) );


   if ((fp = fopen(configStpFilename,"r")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",configStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   CString type;

   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line, " \n\t", 1)) == NULL) 
         continue;

      type = lp;
      type.Trim();
      enabledDevices->SetAtGrow(enabledDevicesCount++, type);
   }

   fclose(fp);
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::DeviceEnabled
*/
BOOL CDeviceTyperStylePropertyPage::DeviceEnabled(CString deviceName)
{
   for (int i=0; i<enabledDevicesCount; i++)
   {
      if (!enabledDevices[i].CompareNoCase(deviceName))
      {
         return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::GetCompsWithEnabledDevices
*/
void CDeviceTyperStylePropertyPage::GetCompsWithEnabledDevices(CDeviceTypeCompArray *compArray)
{
   WORD deviceTypeKeyword = doc->RegisterKeyWord("DEVICETYPE", 0, VT_STRING);
   WORD tempDeviceTypeKeyword = doc->RegisterKeyWord("TEMPDEVICETYPE", 0, VT_STRING);
   WORD PNKeyword = doc->RegisterKeyWord("PARTNUMBER", 0, VT_STRING);
   Attrib* attrib = NULL;
   Attrib* PNAttrib = NULL;
   CString deviceType;
   compArrayCount = 0;
   BOOL sendMessage = FALSE;
   CString compNames;
   int cnt = 0;

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct* block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFileNumber() != m_activeFileNumber)
         continue;

      POSITION dataPos = block->getDataList().GetHeadPosition();
      while (dataPos)
      {
         BOOL missingPN = FALSE;
         DataStruct *data = block->getDataList().GetNext(dataPos);

         if (!data || !data->getAttributesRef() || !data->getInsert() || data->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)
            continue;

         if (!data->getAttributesRef()->Lookup(deviceTypeKeyword, attrib))
         {
            attrib = NULL;

            if (!data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
               continue;

            if(!data->getAttributesRef()->Lookup(PNKeyword, PNAttrib))
               missingPN = TRUE;
         }
         else if (!data->getAttributesRef()->Lookup(PNKeyword, PNAttrib))
         {
            missingPN = TRUE;
         }
         else if (data->getAttributesRef()->Lookup(tempDeviceTypeKeyword, attrib))
         {
            /*device type udated recently without applying, 
            and it had a different device type before*/
         }

         if (!missingPN)
         {
            if (attrib)
               deviceType = (CString)attrib->getStringValue();

            if (!DeviceEnabled(deviceType))
               continue;
         }

         if (missingPN)
         {
            cnt++;
            sendMessage = TRUE;
            compNames += data->getInsert()->getRefname();

            if (cnt%10 == 0)
               compNames += "\n";
            else
               compNames += ",";

            continue;
         }

         compArray->SetAtGrow(compArrayCount++, data);
      }
   }

   if (sendMessage && !sendMessageOnInit)
   {
      compNames.TrimRight(",");
      sendMessageOnInit = TRUE;
      MessageBox("Part numbers expected for device type subclasses.\n\nComponents missing part numbers:\n" + compNames,0,MB_OK); 
   }
}

/******************************************************************************
* CDeviceTyperStylePropertyPage::OnBnClickedOk
*/
void CDeviceTyperStylePropertyPage::OnBnClickedOk()
{
   void* valuePtr = NULL;
   CString style;
   m_style.GetWindowText(style);

   valuePtr = (void*)style.GetString();

   HTREEITEM selItem = m_PNTree.GetSelectedItem();
   HTREEITEM childItem = NULL;
   
   if (!m_PNTree.GetChildItem(selItem)) //if component selected
   {
      childItem = m_PNTree.GetChildItem(m_PNTree.GetParentItem(selItem));

      while (childItem)
      {
         DataStruct *data = (DataStruct*)m_PNTree.GetItemData(childItem);

         if (data)
         {
            doc->SetAttrib(&data->getAttributesRef(), styleKeyword, VT_STRING, valuePtr, SA_OVERWRITE, NULL);
            m_PNTree.SetItemImage(childItem, ICON_GREEN_DOT, ICON_GREEN_DOT);
         }

         childItem = m_PNTree.GetNextSiblingItem(childItem);
      }
   }
   else if (m_PNTree.GetChildItem(selItem) != NULL &&
      m_PNTree.GetChildItem(m_PNTree.GetChildItem(selItem))) //device type selected
   {
      return;
   }
   else if (m_PNTree.ItemHasChildren(selItem)) //part number selected
   {
      childItem = m_PNTree.GetChildItem(selItem);

      while (childItem)
      {
         DataStruct *data = (DataStruct*)m_PNTree.GetItemData(childItem);

         if (data)
         {
            doc->SetAttrib(&data->getAttributesRef(), styleKeyword, VT_STRING, valuePtr, SA_OVERWRITE, NULL);
            m_PNTree.SetItemImage(childItem, ICON_GREEN_DOT, ICON_GREEN_DOT);
         }

         childItem = m_PNTree.GetNextSiblingItem(childItem);
      }
   }
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedButtonConfig
*/
void CDeviceTyperStylePropertyPage::OnBnClickedButtonConfig()
{
   CDeviceTyperStyleConfigPropertyPage StyleConfigDlg;
   StyleConfigDlg.DoModal();

   GetCompsWithEnabledDevices(&compArray);
   FillTree(); 
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedRadioShowstyle
*/
void CDeviceTyperStylePropertyPage::OnBnClickedRadioShowstyle()
{
   UpdateData();

   m_showStyle = 0;
   m_style.SetWindowText("");

   FillTree();
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedRadio3
*/
void CDeviceTyperStylePropertyPage::OnBnClickedRadio3()
{
   UpdateData();

   m_showStyle = 1;
   m_style.SetWindowText("");

   FillTree();
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnTvnClickTreePns
*/
void CDeviceTyperStylePropertyPage::OnTvnClickTreePns(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   //m_PNTree.SelectItem(NULL);
   *pResult = 0;
}


/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnTvnItemexpandingTreePns
*/
void CDeviceTyperStylePropertyPage::OnTvnItemexpandingTreePns(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   //m_PNTree.SelectItem(NULL);
   *pResult = 0;
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnTvnItemexpandedTreePns
*/
void CDeviceTyperStylePropertyPage::OnTvnItemexpandedTreePns(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

   *pResult = 0;
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnTvnSelchangedTreePns
*/
void CDeviceTyperStylePropertyPage::OnTvnSelchangedTreePns(NMHDR *pNMHDR, LRESULT *pResult)
{
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   
   HTREEITEM selItem = m_PNTree.GetSelectedItem();

   if (!selItem)
      return;

   Attrib* attrib = NULL;
   CString styleStr;

   if (!m_PNTree.GetChildItem(selItem)) //if component selected
   {
      DataStruct* data = (DataStruct*)m_PNTree.GetItemData(selItem);

      if (!data)
         return;

      if (data->getAttributesRef()->Lookup(styleKeyword, attrib))
      {
         if (attrib)
            styleStr = attrib->getStringValue();

         m_style.SetWindowText(styleStr);
      }
      else
      {
         m_style.SetWindowText("");
      }        
   }
   else if (m_PNTree.GetChildItem(selItem)!= NULL && 
      m_PNTree.GetChildItem(m_PNTree.GetChildItem(selItem))) //if device type selected
   {
      m_style.SetWindowText("");
   }
   else if (m_PNTree.ItemHasChildren(selItem)) //part number selected
   {
      DataStruct* data = (DataStruct*)m_PNTree.GetItemData(m_PNTree.GetChildItem(selItem));

      if (!data)
         return;

      if (data->getAttributesRef()->Lookup(styleKeyword, attrib))
      {
         if (attrib)
            styleStr = attrib->getStringValue();

         m_style.SetWindowText(styleStr);
      }
      else
      {
         m_style.SetWindowText("");
      }
   }

   *pResult = 0;
}

///////////////////////////////////////////////////////////////////////
// CDeviceTyperStyleConfigPropertyPage dialog

IMPLEMENT_DYNAMIC(CDeviceTyperStyleConfigPropertyPage, CDialog)
CDeviceTyperStyleConfigPropertyPage::CDeviceTyperStyleConfigPropertyPage(CWnd* pParent /*=NULL*/)
   : CDialog(CDeviceTyperStyleConfigPropertyPage::IDD, pParent)
{
}

CDeviceTyperStyleConfigPropertyPage::~CDeviceTyperStyleConfigPropertyPage()
{
}

void CDeviceTyperStyleConfigPropertyPage::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_LIST_DISABLED, m_disabledList);
   DDX_Control(pDX, IDC_LIST_ENABLED, m_enabledList);
}

BEGIN_MESSAGE_MAP(CDeviceTyperStyleConfigPropertyPage, CDialog)
   ON_BN_CLICKED(IDC_BUTTON_MOVERIGHT, OnBnClickedButtonMoveright)
   ON_BN_CLICKED(IDC_BUTTONMOVELEFT, OnBnClickedButtonmoveleft)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

// CDeviceTyperStyleConfigPropertyPage message handlers
/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnInitDialog
*/
BOOL CDeviceTyperStyleConfigPropertyPage::OnInitDialog()
{
   CDialog::OnInitDialog();

   UpdateData();
   FillEnabledTypesList();

   UpdateData();
   FillDisabledTypesList();
   
   return TRUE;
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::FillDisabledTypesList
*/
void CDeviceTyperStyleConfigPropertyPage::FillDisabledTypesList()
{
   int n = 0;

   for (int i=0; i<Devices.GetCount(); i++)
   {
      DeviceTypeStruct *device = Devices[i];

      if (device && !DeviceEnabled(device->deviceTypeName))
      {
         if (device->deviceTypeName.GetLength() != 0 && device->deviceTypeName.CompareNoCase(""))
            m_disabledList.InsertString(n++, device->deviceTypeName.Trim());
      }
   }
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::FillEnabledTypesList
*/
void CDeviceTyperStyleConfigPropertyPage::FillEnabledTypesList()
{
   int n = 0;

   for (int i=0; i< enabledDevicesCount; i++)
   {
      if (enabledDevices.GetAt(i).GetLength() != 0 && enabledDevices.GetAt(i).CompareNoCase(""))
         m_enabledList.InsertString(n++, enabledDevices.GetAt(i));
   }
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedButtonMoveright
*/
void CDeviceTyperStyleConfigPropertyPage::OnBnClickedButtonMoveright()
{
   CString deviceToMove;
   int nIndex = m_disabledList.GetCurSel();

   if (nIndex == CB_ERR)
      return;

   m_disabledList.GetText(nIndex, deviceToMove);
   enabledDevices.SetAtGrow(enabledDevicesCount++, deviceToMove);
   m_enabledList.AddString(deviceToMove);
   m_disabledList.DeleteString(nIndex);

}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedButtonmoveleft
*/
void CDeviceTyperStyleConfigPropertyPage::OnBnClickedButtonmoveleft()
{
   CString deviceToMove;
   int nIndex = m_enabledList.GetCurSel();

   if (nIndex == CB_ERR)
      return;

   m_enabledList.GetText(nIndex, deviceToMove);
   m_enabledList.DeleteString(nIndex);

   for (int i=0; i<enabledDevicesCount; i++)
   {
      if (!enabledDevices[i].CompareNoCase(deviceToMove))
      {
         enabledDevices.RemoveAt(i);

         break;
      }
   }

   m_disabledList.AddString(deviceToMove);
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::DeviceEnabled
*/
BOOL CDeviceTyperStyleConfigPropertyPage::DeviceEnabled(CString deviceName)
{
   for (int i=0; i<enabledDevicesCount; i++)
   {
      if (!enabledDevices[i].CompareNoCase(deviceName))
      {
         return TRUE;
      }
   }

   return FALSE;
}

/******************************************************************************
* CDeviceTyperStyleConfigPropertyPage::OnBnClickedOk
*/
void CDeviceTyperStyleConfigPropertyPage::OnBnClickedOk()
{
   FILE *fp;
   CString configStpFilename( getApp().getSystemSettingsFilePath(QDEV_TYPER_FILENAME_CONFIG_STP) );

   if ((fp = fopen(configStpFilename,"w")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",configStpFilename);
      MessageBox(tmp, "Generate Device Types", MB_OK | MB_ICONHAND);

      return;
   }
    
   CString type;

   int nIndex = 0;
   //m_enabledList.GetText(nIndex, type);

   while (nIndex < m_enabledList.GetCount()/*type.CompareNoCase("")*/)
   {
      m_enabledList.GetText(nIndex++, type);
      fprintf(fp, "%s\n", type);
      type = "";
   }

   fclose(fp);
   OnOK();
}

//_____________________________________________________________________________
CDeviceTyperPropertySheet::CDeviceTyperPropertySheet(CCEtoODBDoc& camCadDoc)
   : CPropertySheet("Generate Device Type")
   , m_compPage(camCadDoc)
   , m_pinPage(camCadDoc)
   , m_stylePage(camCadDoc)
{
   AddPage(&m_compPage);
   AddPage(&m_pinPage);
   AddPage(&m_stylePage);
}

//_____________________________________________________________________________

void CDevtyperTreeCtrl::DeleteItem(CString ItemName)
{
   HTREEITEM root = this->GetRootItem();

   DeleteItem(root, ItemName);

}

//_____________________________________________________________________________

void CDevtyperTreeCtrl::DeleteItem(HTREEITEM root, CString ItemName)
{
   HTREEITEM item = FindItem(root, ItemName);

   if (item != NULL)
      this->DeleteItem(item);
}

//_____________________________________________________________________________

HTREEITEM CDevtyperTreeCtrl::FindItem(CString ItemName)
{
   HTREEITEM root = this->GetRootItem();

   return FindItem(root, ItemName);

}

//_____________________________________________________________________________

HTREEITEM CDevtyperTreeCtrl::FindItem(HTREEITEM root, CString ItemName)
{
   while (root)
   {
      if (!this->GetItemText(root).CompareNoCase(ItemName))
      {
         return root;
      }

      root = this->GetNextSiblingItem(root);
   }

   return NULL;
}