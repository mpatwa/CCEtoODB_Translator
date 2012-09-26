
#include "StdAfx.h"
#include "GerberThermalDfm.h"
#include "CamCadDatabase.h"
#include "WriteFormat.h"
#include "TMState.h"
#include "RwUiLib.h"

CString unitsString(int units);

// defined in Sch_Link.cpp
BOOL GetLicenseString(CLSID clsid, BSTR &licStr);

CString getClearanceFlagsString(int clearanceFlags)
{
   CString retval;

   if ((clearanceFlags & solidConnection) != 0)
   {
      retval += "[Solid Connection] ";
   }

   if ((clearanceFlags & clearanceViolationConnection) != 0)
   {
      retval += "[Clearance Violation Connection] ";
   }

   if ((clearanceFlags & thermalClearanceViolationConnection) != 0)
   {
      retval += "[Thermal Clearance Violation Connection] ";
   }

   if ((clearanceFlags & thermalConnection) != 0)
   {
      retval += "[Thermal Connection] ";
   }

   if ((clearanceFlags & noConnection) != 0)
   {
      retval += "[No Connection] ";
   }

   if ((clearanceFlags & unknownConnection) != 0)
   {
      retval += "[Unknown Connection] ";
   }

   return retval;
}

//_____________________________________________________________________________
bool DFM_GerberThermalBarrelPlugCheck(CCEtoODBDoc& camCadDoc,FileStruct& ecadFile)
{
   CCamCadDatabase camCadDatabase(camCadDoc);

   bool retval = true;

   // put component pins in map
   BlockStruct* ecadBlock = ecadFile.getBlock();
   CDataList& ecadDataList = ecadBlock->getDataList();
   CQfePinMap pinMap(nextPrime2n(10000),true);

   // package loop
   for (POSITION ecadDataPos = ecadDataList.GetHeadPosition();ecadDataPos != NULL;)
   {
      DataStruct* ecadDataStruct = ecadDataList.GetNext(ecadDataPos);

      if (ecadDataStruct != NULL && ecadDataStruct->getDataType() == dataTypeInsert)
      {
         InsertStruct* packageInsert = ecadDataStruct->getInsert();

         if (packageInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            CString refDes      = packageInsert->getRefname();
            int packageBlockNum = packageInsert->getBlockNumber();
            BlockStruct* packageBlock = camCadDoc.getBlockAt(packageBlockNum);
            CDataList& packageDataList = packageBlock->getDataList();

            CTMatrix packageMatrix;

            packageMatrix.rotateRadians(packageInsert->getAngle());
            packageMatrix.scale(((packageInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);
            packageMatrix.translate(packageInsert->getOriginX(),packageInsert->getOriginY());

            // padstack loop
            for (POSITION packageDataPos = packageDataList.GetHeadPosition();packageDataPos != NULL;)
            {
               DataStruct* packageDataStruct = packageDataList.GetNext(packageDataPos);

               if (packageDataStruct != NULL && packageDataStruct->getDataType() == dataTypeInsert)
               {
                  InsertStruct* padstackInsert = packageDataStruct->getInsert();

                  if (padstackInsert->getInsertType() == INSERTTYPE_PIN)
                  {
                     CString pinName      = padstackInsert->getRefname();
                     int padstackBlockNum = padstackInsert->getBlockNumber();
                     BlockStruct* padstackBlock = camCadDoc.getBlockAt(padstackBlockNum);
                     CDataList& padstackDataList = padstackBlock->getDataList();
                     double finishedHoleSize = 0.;

                     // pad loop
                     for (POSITION padstackDataPos = padstackDataList.GetHeadPosition();
                        padstackDataPos != NULL && finishedHoleSize <= 0.;)
                     {
                        DataStruct* padstackDataStruct = padstackDataList.GetNext(padstackDataPos);

                        if (padstackDataStruct != NULL && padstackDataStruct->getDataType() == dataTypeInsert)
                        {
                           InsertStruct* toolInsert = padstackDataStruct->getInsert();

                           int toolBlockNum = toolInsert->getBlockNumber();
                           BlockStruct* toolBlock = camCadDoc.getBlockAt(toolBlockNum);

                           if (toolBlock->getBlockType() == BLOCKTYPE_DRILLHOLE)
                           {
                              finishedHoleSize = toolBlock->getToolSize();
                           }
                        }
                     }

                     if (finishedHoleSize > 0.)
                     {
                        CPoint2d padstackOrigin(padstackInsert->getOriginX(),padstackInsert->getOriginY());
                        packageMatrix.transform(padstackOrigin);

                        CString pinReference = refDes + "." + pinName;
                        CQfePin* pin;

                        if (! pinMap.lookup(pinReference,pin))
                        {
                           pin = new CQfePin(refDes,pinName,padstackOrigin,finishedHoleSize);
                           pinMap.setAt(pinReference,pin);
                        }
                     }
                  }
               }
            }
         }
      }
   }

   CTraceFormat errorLog;
   const CString gerberPadClearanceAttributeName("GerberPadConnections");
   int gerberPadClearanceKeyword = camCadDatabase.registerKeyWord(gerberPadClearanceAttributeName,1,valueTypeString,errorLog);
   CSupString gerberPadClearance;
   CStringArray params;
   int badFlagsMask = (solidConnection | clearanceViolationConnection | 
                        thermalClearanceViolationConnection | unknownConnection );
   int algorithmIndex = GetAlgorithmNameIndex(&camCadDoc,"Gerber Thermal Barrel Plug Check");

   ClearDRCListForAlogorithm(&camCadDoc,algorithmIndex);

   for (int pinIndex = 0;pinIndex < pinMap.getCount();pinIndex++)
   {
      CQfePin* pin = pinMap.getAt(pinIndex);

      CompPinStruct* compPin = camCadDatabase.getPin(&ecadFile,pin->getRefDes(),pin->getPinName());

      if (compPin != NULL)
      {
         CAttributes** attributeMap = getAttributeMap(compPin);

         if (attributeMap != NULL)
         {
            Attrib* attribute;

            if ((*attributeMap)->Lookup(gerberPadClearanceKeyword,attribute))
            {
               gerberPadClearance = camCadDatabase.getAttributeStringValue(attribute);
               int numParams = gerberPadClearance.Parse(params,",");

               for (int ind = 0;ind < numParams;ind++)
               {
                  const char* p = params[ind];
                  char* q;

                  int layerNumber = strtol(p,&q,10);
                  CString layerName;
                  LayerStruct* layer = camCadDatabase.getLayerAt(layerNumber);

                  if (layer != NULL)
                  {
                     layerName = layer->getName();
                  }
                  else
                  {
                     layerName.Format("%d",layerNumber);
                  }

                  if (*q == '=')
                  {
                     int clearanceFlags = strtol(q + 1,&q,10);
                     CString clearanceFlagsString = getClearanceFlagsString(clearanceFlags);

                     if (*q == '\0')
                     {
                        if (((clearanceFlags & badFlagsMask) != 0) && ((clearanceFlags & noConnection) == 0))
                        {
                           // add DRC here
                           CString violationDescription;
                           violationDescription.Format(
                              "Potential Barrel Plug: layer='%s', status='%s', Pinref='%s', fhs=%.3f",
                              (const char*)layerName,(const char*)clearanceFlagsString,
                              (const char*)pin->getPinReference(),pin->getFinishedHoleSize());

                           DRCStruct* drc = camCadDatabase.addDrc(ecadFile,pin->getOrigin().x,pin->getOrigin().y,
                              violationDescription,drcSimple,drcCriticalFailure,algorithmIndex,drcGeneric);
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return retval;
}

//_____________________________________________________________________________
CGerberThermalLayer::CGerberThermalLayer(int layerNumber)
{
   m_layerNumber      = layerNumber;
   m_rowIndex         = -1;
   m_thermalMask      =  0;
   m_thermalLayerFlag = false;
   m_layerStruct      = NULL;
}

//_____________________________________________________________________________
CGerberThermalLayerArray::CGerberThermalLayerArray() :
   m_layers(40),m_rows(40,false)
{
}

CGerberThermalLayerArray::~CGerberThermalLayerArray()
{
   empty();
}

void CGerberThermalLayerArray::empty()
{
   m_layers.empty();
   m_rows.empty();
}

CGerberThermalLayer& CGerberThermalLayerArray::getAt(int layerNumber)
{
   CGerberThermalLayer* gerberThermalLayer = NULL;

   if (layerNumber < m_layers.GetSize())
   {
      gerberThermalLayer = m_layers.GetAt(layerNumber);
   }

   if (gerberThermalLayer == NULL)
   {
      gerberThermalLayer = new CGerberThermalLayer(layerNumber);
      m_layers.SetAtGrow(layerNumber,gerberThermalLayer);
   }

   return *gerberThermalLayer;
}

CGerberThermalLayer* CGerberThermalLayerArray::getAtRowIndex(int rowIndex)
{
   CGerberThermalLayer* gerberThermalLayer = NULL;

   if (rowIndex < m_rows.GetSize())
   {
      gerberThermalLayer = m_rows.GetAt(rowIndex);
   }

   return gerberThermalLayer;
}

void CGerberThermalLayerArray::setRowIndex(int layerNumber,int rowIndex)
{
   CGerberThermalLayer& gerberThermalLayer = getAt(layerNumber);
   gerberThermalLayer.setRowIndex(rowIndex);
   m_rows.SetAtGrow(rowIndex,&gerberThermalLayer);
}

int CGerberThermalLayerArray::getSize()
{
   return m_layers.GetSize();
}

//_____________________________________________________________________________
CConnectionEvaluation::CConnectionEvaluation(connectionTag connection,BlockStruct* aperture,double clearance)
{
   m_connection = connection;
   m_aperture   = aperture;
   m_clearance  = clearance;
}

void CConnectionEvaluation::writeConnectionEvaluationReport(CWriteFormat& writeFormat)
{
   writeFormat.writef("connection=%s, apertureName='%s', apertureSize=(%.3f,%.3f), clearance=%.3f\n",
      (const char*)getClearanceFlagsString(m_connection),
      (const char*)m_aperture->getName(),
      m_aperture->getSizeA(),m_aperture->getSizeB(),
      m_clearance);
}

//_____________________________________________________________________________
CQfePin::CQfePin(const CString& refDes,const CString& pinName,const CPoint2d& origin,double finishedHoleSize)   
{
   m_refDes           = refDes;
   m_pinName          = pinName;
   m_origin           = origin;
   m_finishedHoleSize = finishedHoleSize;

   m_connectionFlags.SetSize(0,40);
}

CQfePin::~CQfePin()
{
}

int CQfePin::getObjectType() const
{
   return 0;
}

CExtent CQfePin::getExtent() const
{
   return CExtent(getOrigin(),getOrigin());
}

bool CQfePin::isPointWithinDistance(const CPoint2d& point,double distance)
{
   double edgeDistance = _hypot(getOrigin().x - point.x,
                                getOrigin().y - point.y);

   bool retval = (edgeDistance < distance);

   return retval;
}

bool CQfePin::isExtentIntersecting(const CExtent& extent)
{
   bool retval = extent.distanceTo(getOrigin()) == 0.;

   return retval;
}

bool CQfePin::isInViolation(CObject2d& otherObject)
{
   ASSERT(0);
   return false;
}

CString CQfePin::getInfoString() const
{
   CString retval;

   retval.Format("CQfePin: pinReference='%s', origin=(%s,%s), extent=(%s,%s),(%s,%s)",
      (const char*)getPinReference(),
      fpfmt(getOrigin().x),
      fpfmt(getOrigin().y),
      fpfmt(getExtent().getXmin()),
      fpfmt(getExtent().getYmin()),
      fpfmt(getExtent().getXmax()),
      fpfmt(getExtent().getYmax())   );

   return retval;
}

CString CQfePin::getGerberPadClearanceAttributeValue(CGerberThermalLayerArray& gerberThermalLayerArray) const
{
   CString attributeValue,delimiter;

   for (int layerNumber = 0;layerNumber < m_connectionFlags.GetSize();layerNumber++)
   {
      int connectionFlag = 0;

      if (layerNumber >= m_connectionFlags.GetSize())
      {
         if (layerNumber >= gerberThermalLayerArray.getSize())
         {
            break;
         }
      }
      else
      {
         connectionFlag = m_connectionFlags.GetAt(layerNumber);
      }

      if (connectionFlag == 0 && layerNumber < gerberThermalLayerArray.getSize())
      {
         CGerberThermalLayer& gerberThermalLayer = gerberThermalLayerArray.getAt(layerNumber);

         if (gerberThermalLayer.getThermalLayerFlag())
         {
            connectionFlag = solidConnection;
         }
      }

      if (connectionFlag != 0)
      {
         CString flagValue;
         flagValue.Format("%d=%d",layerNumber,connectionFlag);
         attributeValue += delimiter + flagValue;
         delimiter = ",";
      }
   }

   return attributeValue;
}

int CQfePin::evaluateConnection(double dx,double dy,int layerNumber,
   BlockStruct* apertureBlock,double clearanceTolerance)
{
   connectionTag connection = undefinedConnection;

   double size = apertureBlock->getSizeA();
   double minXySize = min(apertureBlock->getSizeA(),apertureBlock->getSizeB());
   double clearance = 0.;

   switch(apertureBlock->getShape())
   {
   case apertureRound:
   case apertureSquare:
   case apertureOctagon:
      clearance = (size - m_finishedHoleSize)/2.;

      if (size <= m_finishedHoleSize)
      {
         connection = solidConnection;
      }
      else if (clearance < clearanceTolerance)
      {
         connection = clearanceViolationConnection;
      }
      else
      {
         connection = noConnection;
      }

      break;
   case apertureRectangle:
   case apertureOblong:
      clearance = (minXySize - m_finishedHoleSize)/2.;

      if (minXySize <= m_finishedHoleSize)
      {
         connection = solidConnection;
      }
      else if (clearance < clearanceTolerance)
      {
         connection = clearanceViolationConnection;
      }
      else
      {
         connection = noConnection;
      }

      break;
   case apertureTarget:
      clearance = (size - m_finishedHoleSize)/2.;

      connection = ((size < m_finishedHoleSize) ? solidConnection : clearanceViolationConnection);

      break;
   case apertureThermal:
      clearance = (size - m_finishedHoleSize)/2.;

      if (size <= m_finishedHoleSize)
      {
         connection = solidConnection;
      }
      else if (clearance < clearanceTolerance)
      {
         connection = thermalClearanceViolationConnection;
      }
      else
      {
         connection = thermalConnection;
      }

      break;
   case apertureDonut:
      clearance = (size - m_finishedHoleSize)/2.;

      if (size <= m_finishedHoleSize)
      {
         connection = solidConnection;
      }
      else if (clearance < clearanceTolerance)
      {
         connection = clearanceViolationConnection;
      }
      //else if (apertureBlock->getSizeB() > m_finishedHoleSize)
      //{
      //   // inner diameter of donut is outside of hole
      //   clearance = (apertureBlock->getSizeB() - m_finishedHoleSize)/2.;

      //   if (clearance < clearanceTolerance)
      //   {
      //      connection = clearanceViolationConnection;
      //   }
      //}
      else
      {
         connection = noConnection;
      }

      break;
   case apertureBlank:
      connection = solidConnection;

      break;
   case apertureComplex:
   case apertureUnknown:
   default:
      connection = unknownConnection;

      break;
   }

   int oldConnection = 0;

   if (layerNumber < m_connectionFlags.GetSize())
   {
      oldConnection = m_connectionFlags.GetAt(layerNumber);
   }

   m_connectionFlags.SetAtGrow(layerNumber,oldConnection | connection);

   addConnectionEvaluation(layerNumber,connection,apertureBlock,clearance);

   return connection;
}

void CQfePin::addConnectionEvaluation(int layerIndex,connectionTag connection,
   BlockStruct* aperture,double clearance)
{
   CConnectionEvaluation* evaluation = new CConnectionEvaluation(connection,aperture,clearance);
   CConnectionEvaluationList* evaluationList = NULL;

   if (layerIndex < m_evaluations.GetSize())
   {
      evaluationList = m_evaluations.GetAt(layerIndex);
   }

   if (evaluationList == NULL)
   {
      evaluationList = new CConnectionEvaluationList();
      m_evaluations.SetAtGrow(layerIndex,evaluationList);
   }

   evaluationList->AddTail(evaluation);
}

void CQfePin::writeConnectionEvaluationReport(CWriteFormat& writeFormat)
{
   for (int layerIndex = 0;layerIndex < m_evaluations.GetSize();layerIndex++)
   {
      CConnectionEvaluationList* evaluationList = m_evaluations.GetAt(layerIndex);
      CString header;
      header.Format("Connection layer index=%d: ",layerIndex);

      writeFormat.pushHeader(header);

      if (evaluationList != NULL)
      {
         for (POSITION pos = evaluationList->GetHeadPosition();pos != NULL;)
         {
            CConnectionEvaluation* evaluation = evaluationList->GetNext(pos);

            evaluation->writeConnectionEvaluationReport(writeFormat);
         }
      }

      writeFormat.popHeader();
   }
}

//_____________________________________________________________________________
CQfePinList::CQfePinList(bool isContainer,int nBlockSize) : 
   CTypedPtrList<CQfeList,CQfePin*>(nBlockSize)
{
   m_isContainer = isContainer;
}

CQfePinList::~CQfePinList()
{
   empty();
}

void CQfePinList::empty()
{
   if (m_isContainer)
   {
      for (POSITION pos = GetHeadPosition();pos != NULL;)
      {
         CQfePin* qfeInsert = GetNext(pos);
         delete qfeInsert;
      }
   }

   RemoveAll();
}

//_____________________________________________________________________________
CQfePinTree::CQfePinTree() : CQfeExtentLimitedContainer(CSize2d(0.,0.),false)
{
}

CQfePinTree::~CQfePinTree()
{
}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage, CPropertyPage)
CGerberThermalDfmPropertyPage::CGerberThermalDfmPropertyPage(UINT idTemplate) :
   CPropertyPage(idTemplate)
{
   m_flexGrid = NULL;
}

CGerberThermalDfmPropertyPage::~CGerberThermalDfmPropertyPage()
{
   delete m_flexGrid;
}

//CExtendedFlexGrid& CGerberThermalDfmPropertyPage::getFlexGrid()
//{
//   if (m_flexGrid == NULL)
//   {
//      // License the flex grid
//      BSTR licenseKey = NULL;
//
//      m_flexGrid = new CExtendedFlexGrid();
//
//      if (!GetLicenseString(m_flexGrid->GetClsid(),licenseKey))
//      {
//         licenseKey = NULL;
//      }
//
//      CRect controlRect;
//
//      GetDlgItem(IDC_VsFlexGridPlaceHolder)->ShowWindow(SW_HIDE);
//      GetDlgItem(IDC_VsFlexGridPlaceHolder)->GetWindowRect(&controlRect);
//      ScreenToClient(&controlRect);
//
//      m_flexGrid->Create("FlexGrid",WS_VISIBLE,controlRect,this,IDC_VSFLEX,
//                        NULL,FALSE,licenseKey);
//   }
//
//   return *m_flexGrid;
//}

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage1, CGerberThermalDfmPropertyPage)
CGerberThermalDfmPropertyPage1::CGerberThermalDfmPropertyPage1()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage1::IDD)
{
}

CGerberThermalDfmPropertyPage1::~CGerberThermalDfmPropertyPage1()
{
}

void CGerberThermalDfmPropertyPage1::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_WizardDescription, m_wizardDescription);
}

// fool class wizard
#define PropertyPage CGerberThermalDfmPropertyPage
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage1, PropertyPage)
END_MESSAGE_MAP()
#undef PropertyPage

// CGerberThermalDfmPropertyPage1 message handlers

BOOL CGerberThermalDfmPropertyPage1::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   m_wizardDescription.SetWindowText(
"The Gerber Thermal DFM Wizard assists in the setup of the Gerber Thermal DFM Check.\n"
"This check compares flashes on Gerber layers with pads on thru hole pins on ECAD layers.\n"
"The result of the test is the placement of \"GerberPadClearance\" attribute on thru hole pins.\n"
"\n"
"The following steps should be performed before continuing with this wizard:\n"
"\n"
"1) Import or read in the ECAD and Gerber data.\n"
"2) Align the Gerber data with ECAD data.\n"
"3) Turn \"on\" one and only one \"Board\" or \"Panel\" file.\n"
"4) Turn \"on\" at least one file of type \"Gerber\".\n"
"\n"
"Press the \"Cancel\" button to abort the wizard at this time to complete the above steps.\n"
"Press the \"Next\" button to continue with the wizard.\n"
"\n"
"The following steps are performed with the wizard:\n"
"\n"
"1) Select which layers are ECAD layers and which are Gerber layers.\n"
"2) Associate one or more ECAD layers with each Gerber layer.\n"
"3) Select thermal apertures.\n"
"4) Edit power signal names.\n"
"5) Flag signal names found on thermal flashes as either \"accepted\" or \"rejected\"\n"
"\n"
      );

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage1::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   
   getParent().SetWizardButtons(PSWIZB_NEXT);
   getParent().setCaption("Overview");

   return CPropertyPage::OnSetActive();
}

// CGerberThermalDfmPropertyPage2 dialog
//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage2, CGerberThermalDfmPropertyPage)
CGerberThermalDfmPropertyPage2::CGerberThermalDfmPropertyPage2()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage2::IDD)
   , m_clearanceTolerance(0.)
{
}

CGerberThermalDfmPropertyPage2::~CGerberThermalDfmPropertyPage2()
{
}

void CGerberThermalDfmPropertyPage2::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_VsFlexGrid, m_flexGridControl);
   DDX_Text(pDX, IDC_ClearanceTolerance, m_clearanceTolerance);
}

void CGerberThermalDfmPropertyPage2::initGrid()
{
   getFlexGrid().clear();
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   getFlexGrid().put_Cols(2);
   getFlexGrid().put_Rows(2);
   getFlexGrid().put_FixedCols(1);
   getFlexGrid().put_FixedRows(1);

   getFlexGrid().setCell(0,m_colGerberLayerName,"Gerber Layer Name");
   getFlexGrid().setCellStyleBold(0,m_colGerberLayerName);
   getFlexGrid().setCell(0,m_colPlaneLayer,"Plane Layer");
   getFlexGrid().setCellStyleBold(0,m_colPlaneLayer);

   getFlexGrid().autoSizeColumns();
}

double CGerberThermalDfmPropertyPage2::getClearanceTolerance()
{ 
   return m_clearanceTolerance * Units_Factor(getParent().getCamCadDoc().getSettings().getPageUnits(),UNIT_INCHES);
}

// fool class wizard
#define PropertyPage CGerberThermalDfmPropertyPage
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage2, PropertyPage)
   ON_BN_CLICKED(IDC_ScanPlanesButton, OnBnClickedScanPlanesButton)
   ON_BN_CLICKED(IDC_InsertDrcsButton, OnBnClickedInsertDrcsButton)
END_MESSAGE_MAP()
#undef PropertyPage

// CGerberThermalDfmPropertyPage2 message handlers

BOOL CGerberThermalDfmPropertyPage2::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   initGrid();

   // default to .005 inch
   short pageUnits = getParent().getCamCadDoc().getSettings().getPageUnits();
   m_clearanceTolerance = .005 * Units_Factor(UNIT_INCHES,pageUnits);
   GetDlgItem(IDC_PageUnits)->SetWindowText(unitsString(pageUnits));

   UpdateData(false);

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage2::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   getParent().SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
   getParent().setCaption("Select ECAD and Gerber layers");

   return CPropertyPage::OnSetActive();
}

void CGerberThermalDfmPropertyPage2::OnBnClickedScanPlanesButton()
{
   // TODO: Add your control notification handler code here
   CCEtoODBDoc& camCadDoc = getParent().getCamCadDoc();
   CCamCadDatabase camCadDatabase(camCadDoc);
   CGerberThermalLayerArray& gerberThermalLayerArray = getParent().getGerberThermalLayerArray();

   bool errorFlag = false;

   gerberThermalLayerArray.empty();
   FileStruct* fileStruct = NULL;
   getParent().setEcadFile(NULL);
   CSimpleFileList& gerberList = getParent().getGerberList(); 
   gerberList.RemoveAll();

   for (POSITION pos = camCadDoc.getFileList().GetHeadPosition();pos != NULL;)
   {
      fileStruct = camCadDoc.getFileList().GetNext(pos);

      if (fileStruct != NULL && fileStruct->isShown())
      {
         if (fileStruct->getCadSourceFormat() == Type_Gerber)
         {
            gerberList.AddTail(fileStruct);
         }
         else if (fileStruct->getBlockType() == BLOCKTYPE_PCB || 
                  fileStruct->getBlockType() == BLOCKTYPE_PANEL   )
         {
            if (getParent().getEcadFile() != NULL)
            {
               formatMessageBox("More than one Board or Panel file is on!");
               errorFlag = true;
               break;
            }
            else
            {
               getParent().setEcadFile(fileStruct);
            }
         }
      }
   }

   if (!errorFlag && getParent().getEcadFile() == NULL)
   {
      formatMessageBox("No Board or Panel file is on!");
      errorFlag = true;
   }

   if (!errorFlag && gerberList.GetCount() < 1)
   {
      formatMessageBox("No Gerber files are on!");
      errorFlag = true;
   }

   if (!errorFlag)
   {
      CWordArray gerberLayerThermalFlags;
      gerberLayerThermalFlags.SetSize(20,20); 
      int gerberLayerCount = 0;

      for (POSITION gerberPos = gerberList.GetHeadPosition();gerberPos != NULL;)
      {
         FileStruct* fileStruct = gerberList.GetNext(gerberPos);
         BlockStruct* fileBlock = fileStruct->getBlock();
         CDataList& dataList = fileBlock->getDataList();
         DataStruct* dataStruct = NULL;

         for (POSITION dataPos = dataList.GetHeadPosition();dataPos != NULL;)
         {
            dataStruct = dataList.GetNext(dataPos);

            if (dataStruct != NULL && dataStruct->getDataType() == dataTypeInsert)
            {
               int layerNumber = dataStruct->getLayerIndex();

               CGerberThermalLayer& gerberThermalLayer = gerberThermalLayerArray.getAt(layerNumber);
               int thermalMask = gerberThermalLayer.getThermalMask();

               if (thermalMask == BothPadsMask)
               {
                  continue;
               }

               InsertStruct* insertStruct = dataStruct->getInsert();
               int blockNum = insertStruct->getBlockNumber();

               BlockStruct* block = camCadDoc.getBlockAt(blockNum);
               int apertureIndex = camCadDatabase.getWidthIndex(block);

               if (apertureIndex >= 0)
               {
                  if (thermalMask == 0)
                  {
                     gerberLayerCount++;
                  }

                  thermalMask |= ((block->getShape() == T_THERMAL) ? ThermalPadMask : NonThermalPadMask);

                  gerberThermalLayer.setThermalMask(thermalMask);
               }
            }
         }
      }

      initGrid();
      getFlexGrid().put_Rows(gerberLayerCount + 1);
      int rowIndex = 1;

      for (int layerNumber = 0;layerNumber < gerberThermalLayerArray.getSize();layerNumber++)
      {
         CGerberThermalLayer& gerberThermalLayer = gerberThermalLayerArray.getAt(layerNumber);
         int thermalMask = gerberThermalLayer.getThermalMask();

         if (thermalMask != 0)
         {
            LayerStruct* layer = camCadDoc.getLayerArray().GetAt(layerNumber);
            CString layerName = layer->getName();

            gerberThermalLayer.setLayerStruct(layer);
            gerberThermalLayerArray.setRowIndex(layerNumber,rowIndex);

            getFlexGrid().setCell(rowIndex,m_colGerberLayerName,layerName);
            getFlexGrid().setCellChecked(rowIndex,m_colPlaneLayer,(thermalMask & ThermalPadMask) != 0);
            rowIndex++;
         }
      }

      getFlexGrid().autoSizeColumns();
   }
}

BOOL CGerberThermalDfmPropertyPage2::OnKillActive()
{
   // TODO: Add your specialized code here and/or call the base class
   UpdateData(true);

   for (int rowIndex = 1;rowIndex < getFlexGrid().get_Rows();rowIndex++)
   {
      CGerberThermalLayer* gerberThermalLayer = getParent().getGerberThermalLayerArray().getAtRowIndex(rowIndex);

      if (gerberThermalLayer != NULL)
      {
         bool checkedFlag = getFlexGrid().getCellChecked(rowIndex,m_colPlaneLayer);
         gerberThermalLayer->setThermalLayerFlag(checkedFlag);
      }
   }

   return CGerberThermalDfmPropertyPage::OnKillActive();
}

void CGerberThermalDfmPropertyPage2::OnBnClickedInsertDrcsButton()
{
   // TODO: Add your control notification handler code here
   CCEtoODBDoc& camCadDoc = getParent().getCamCadDoc();
   bool errorFlag = false;

   for (POSITION pos = camCadDoc.getFileList().GetHeadPosition();pos != NULL;)
   {
      FileStruct* fileStruct = camCadDoc.getFileList().GetNext(pos);

      if (fileStruct != NULL && fileStruct->isShown())
      {
         if (fileStruct->getBlockType() == BLOCKTYPE_PCB || 
             fileStruct->getBlockType() == BLOCKTYPE_PANEL   )
         {
            if (getParent().getEcadFile() != NULL)
            {
               formatMessageBox("More than one Board or Panel file is on!");
               errorFlag = true;
               break;
            }
            else
            {
               getParent().setEcadFile(fileStruct);
            }
         }
      }
   }

   FileStruct* ecadFile = getParent().getEcadFile();

   if (!errorFlag && ecadFile != NULL)
   {
      bool success = DFM_GerberThermalBarrelPlugCheck(camCadDoc,*ecadFile);
   }
}

BEGIN_EVENTSINK_MAP(CGerberThermalDfmPropertyPage2, CGerberThermalDfmPropertyPage)
   ON_EVENT(CGerberThermalDfmPropertyPage2, IDC_VsFlexGrid, 24, AfterEditVsflexgrid, VTS_I4 VTS_I4)
   ON_EVENT(CGerberThermalDfmPropertyPage2, IDC_VsFlexGridPlaceHolder, 24, AfterEditVsflexgrid, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

void CGerberThermalDfmPropertyPage2::AfterEditVsflexgrid(long row, long col)
{
   // TODO: Add your message handler code here
   //if (col == 1 || col == 2)
   //{
   //   getFlexGrid().setRadioCheck(row,col,1,2);
   //}
}

// CGerberThermalDfmPropertyPage3 dialog
//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage3, CPropertyPage)
CGerberThermalDfmPropertyPage3::CGerberThermalDfmPropertyPage3()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage3::IDD)
{
}

CGerberThermalDfmPropertyPage3::~CGerberThermalDfmPropertyPage3()
{
}

void CGerberThermalDfmPropertyPage3::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_Summary, m_summaryListBox);
}

void CGerberThermalDfmPropertyPage3::setGerberPadClearanceAttributes()
{
   // TODO: Add your control notification handler code here
   CCEtoODBDoc& camCadDoc = getParent().getCamCadDoc();
   CCamCadDatabase camCadDatabase(camCadDoc);

   bool errorFlag = false;

   FileStruct* fileStruct = NULL;
   FileStruct* ecadFile = NULL;
   CSimpleFileList& gerberList = getParent().getGerberList();

   if (getParent().getEcadFile() == NULL)
   {
      formatMessageBox("No Board or Panel file is on!");
   }
   else
   {
      // put component pins in tree
      BlockStruct* ecadBlock = getParent().getEcadFile()->getBlock();
      CDataList& ecadDataList = ecadBlock->getDataList();
      CQfePinTree pinTree;
      CQfePinMap pinMap(nextPrime2n(10000),false);
      COperationProgress operationProgress(ecadDataList.GetCount());
      operationProgress.updateStatus("Updating Gerber pad clearance attributes");
      CWaitCursor waitCursor;

      // package loop
      for (POSITION ecadDataPos = ecadDataList.GetHeadPosition();ecadDataPos != NULL;)
      {
         operationProgress.incrementProgress();
         waitCursor.Restore();

         DataStruct* ecadDataStruct = ecadDataList.GetNext(ecadDataPos);

         if (ecadDataStruct != NULL && ecadDataStruct->getDataType() == dataTypeInsert)
         {
            InsertStruct* packageInsert = ecadDataStruct->getInsert();

            if (packageInsert->getInsertType() == INSERTTYPE_PCBCOMPONENT)
            {
               CString refDes      = packageInsert->getRefname();
               int packageBlockNum = packageInsert->getBlockNumber();
               BlockStruct* packageBlock = camCadDoc.getBlockAt(packageBlockNum);
               CDataList& packageDataList = packageBlock->getDataList();

               CTMatrix packageMatrix;

               packageMatrix.rotateRadians(packageInsert->getAngle());
               packageMatrix.scale(((packageInsert->getMirrorFlags() & MIRROR_FLIP) != 0) ? -1. : 1.,1.);
               packageMatrix.translate(packageInsert->getOriginX(),packageInsert->getOriginY());

               // padstack loop
               for (POSITION packageDataPos = packageDataList.GetHeadPosition();packageDataPos != NULL;)
               {
                  DataStruct* packageDataStruct = packageDataList.GetNext(packageDataPos);

                  if (packageDataStruct != NULL && packageDataStruct->getDataType() == dataTypeInsert)
                  {
                     InsertStruct* padstackInsert = packageDataStruct->getInsert();

                     if (padstackInsert->getInsertType() == INSERTTYPE_PIN)
                     {
                        CString pinName      = padstackInsert->getRefname();
                        int padstackBlockNum = padstackInsert->getBlockNumber();
                        BlockStruct* padstackBlock = camCadDoc.getBlockAt(padstackBlockNum);
                        CDataList& padstackDataList = padstackBlock->getDataList();
                        double finishedHoleSize = 0.;

                        // pad loop
                        for (POSITION padstackDataPos = padstackDataList.GetHeadPosition();
                           padstackDataPos != NULL && finishedHoleSize <= 0.;)
                        {
                           DataStruct* padstackDataStruct = padstackDataList.GetNext(padstackDataPos);

                           if (padstackDataStruct != NULL && padstackDataStruct->getDataType() == dataTypeInsert)
                           {
                              InsertStruct* toolInsert = padstackDataStruct->getInsert();

                              int toolBlockNum = toolInsert->getBlockNumber();
                              BlockStruct* toolBlock = camCadDoc.getBlockAt(toolBlockNum);

                              if (toolBlock->getBlockType() == BLOCKTYPE_DRILLHOLE)
                              {
                                 finishedHoleSize = toolBlock->getToolSize();
                              }
                           }
                        }

                        if (finishedHoleSize > 0.)
                        {
                           CPoint2d padstackOrigin(padstackInsert->getOriginX(),padstackInsert->getOriginY());
                           packageMatrix.transform(padstackOrigin);

                           CString pinReference = refDes + "." + pinName;
                           CQfePin* pin;

                           if (! pinMap.lookup(pinReference,pin))
                           {
                              pin = new CQfePin(refDes,pinName,padstackOrigin,finishedHoleSize);
                              pinMap.setAt(pinReference,pin);
                              pinTree.setAt(pin);
                           }
                        }
                     }
                  }
               }
            }
         }
      }

      // calculate connection values
      CGerberThermalLayerArray& gerberThermalLayerArray = getParent().getGerberThermalLayerArray(); 
      CExtent searchExtent;
      double searchTolerance = .001;
      double clearanceTolerance = getParent().getClearanceTolerance();

      // gerber layer loop
      for (POSITION gerberPos = gerberList.GetHeadPosition();gerberPos != NULL;)
      {
         FileStruct* fileStruct = gerberList.GetNext(gerberPos);
         BlockStruct* fileBlock = fileStruct->getBlock();
         CDataList& dataList = fileBlock->getDataList();
         DataStruct* dataStruct = NULL;

         for (POSITION dataPos = dataList.GetHeadPosition();dataPos != NULL;)
         {
            dataStruct = dataList.GetNext(dataPos);

            if (dataStruct != NULL && dataStruct->getDataType() == dataTypeInsert)
            {
               int layerNumber = dataStruct->getLayerIndex();

               if (layerNumber < 0)
               {
                  continue;
               }

               CGerberThermalLayer& gerberThermalLayer = gerberThermalLayerArray.getAt(layerNumber);

               if (! gerberThermalLayer.getThermalLayerFlag())
               {
                  continue;
               }

               InsertStruct* apertureInsert = dataStruct->getInsert();
               int apertureBlockNum = apertureInsert->getBlockNumber();
               BlockStruct* apertureBlock = camCadDoc.getBlockAt(apertureBlockNum);
               int apertureIndex = camCadDatabase.getWidthIndex(apertureBlock);

               if (apertureIndex >= 0)
               {
                  double x = apertureInsert->getOriginX();
                  double y = apertureInsert->getOriginY();

                  searchExtent.set(x - searchTolerance,y - searchTolerance,
                                   x + searchTolerance,y + searchTolerance  );

                  CQfePinList foundList;

                  pinTree.search(searchExtent,foundList);

                  for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
                  {
                     CQfePin* pin = foundList.GetNext(foundPos);
                     double dx = fabs(x - pin->getOrigin().x);
                     double dy = fabs(y - pin->getOrigin().y);

                     pin->evaluateConnection(dx,dy,layerNumber,apertureBlock,clearanceTolerance);
                  }
               }
            }
         }
      }

      // add attributes to pins
      bool writeReportFlag = true;
      CStdioFileWriteFormat* reportWriteFormat = NULL;

      if (writeReportFlag)
      {
         reportWriteFormat = new CStdioFileWriteFormat();
         CFilePath filePath(GetLogfilePath("GerberThermalDfm.txt"));
         filePath.pushLeaf("GerberThermalDfm.txt");

         if (! reportWriteFormat->open(filePath.getPath()))
         {
            delete reportWriteFormat;
            reportWriteFormat = NULL;
         }
      }

      CTraceFormat errorLog;
      FileStruct* ecadFile = getParent().getEcadFile();
      const CString gerberPadClearanceAttributeName("GerberPadConnections");
      int gerberPadClearanceKeyword = camCadDatabase.registerKeyWord(gerberPadClearanceAttributeName,1,valueTypeString,errorLog);

      for (int pinIndex = 0;pinIndex < pinMap.getCount();pinIndex++)
      {
         CQfePin* pin = pinMap.getAt(pinIndex);

         CompPinStruct* compPin = camCadDatabase.getPin(ecadFile,pin->getRefDes(),pin->getPinName());

         if (compPin == NULL)
         {
            compPin = camCadDatabase.getDefinedPin(ecadFile,pin->getRefDes(),pin->getPinName(),NET_UNUSED_PINS);
         }

         CString gerberPadClearance = pin->getGerberPadClearanceAttributeValue(gerberThermalLayerArray);

         camCadDatabase.removeAttribute(getAttributeMap(compPin),gerberPadClearanceKeyword);
         camCadDatabase.addAttribute(getAttributeMap(compPin),gerberPadClearanceKeyword,
            gerberPadClearance,errorLog);

         if (reportWriteFormat != NULL)
         {
            CString clearance = gerberPadClearanceAttributeName + "='" + gerberPadClearance + "'";

            reportWriteFormat->writef("%-10s (%10.3f,%10.3f) - %-35s, fhs=%12.3f\n",
               (const char*)pin->getPinReference(),
               pin->getOrigin().x,pin->getOrigin().y,
               (const char*)clearance,
               pin->getFinishedHoleSize());

            pin->writeConnectionEvaluationReport(*reportWriteFormat);
         }
      }

      //m_summaryEditBox.SetWindow
      CString summaryString;
      summaryString.Format("Added %d %s attributes.",pinMap.getCount(),(const char*)gerberPadClearanceAttributeName);
      m_summaryListBox.AddString(summaryString);

      delete reportWriteFormat;
   }
}

// fool class wizard
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage3, CGerberThermalDfmPropertyPage)
END_MESSAGE_MAP()

// CGerberThermalDfmPropertyPage3 message handlers
BOOL CGerberThermalDfmPropertyPage3::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage3::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   
   getParent().SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK | PSWIZB_FINISH);
   getParent().setCaption("Summary");

   setGerberPadClearanceAttributes();

   return CPropertyPage::OnSetActive();
}


// CGerberThermalDfmPropertyPage4 dialog
//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage4, CGerberThermalDfmPropertyPage)
CGerberThermalDfmPropertyPage4::CGerberThermalDfmPropertyPage4()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage4::IDD)
{
}

CGerberThermalDfmPropertyPage4::~CGerberThermalDfmPropertyPage4()
{
}

void CGerberThermalDfmPropertyPage4::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_VsFlexGrid, m_flexGridControl);
}

void CGerberThermalDfmPropertyPage4::loadTestData()
{
   // test data
   int rowInd = 1;
   getFlexGrid().setCell(rowInd,m_colDcode,"1");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"rect20x40");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"2");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"rect40x20");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"3");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"rnd20");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"4");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"sq20");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"5");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"iso40x20");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"6");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"iso60x40");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"7");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"oval20x30");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colDcode,"8");
   getFlexGrid().setCell(rowInd,m_colAperture  ,"target");
   getFlexGrid().setCellChecked(rowInd,m_colThermal,false);
}


// fool class wizard
#define PropertyPage CGerberThermalDfmPropertyPage
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage4, PropertyPage)
END_MESSAGE_MAP()
#undef PropertyPage


// CGerberThermalDfmPropertyPage4 message handlers
BOOL CGerberThermalDfmPropertyPage4::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   getFlexGrid().put_Cols(3);
   getFlexGrid().put_Rows(10);
   getFlexGrid().put_FixedCols(2);
   getFlexGrid().put_FixedRows(1);

   getFlexGrid().setCell(0,m_colDcode,"D-Code");
   getFlexGrid().setCellStyleBold(0,m_colDcode);
   getFlexGrid().setCell(0,m_colAperture,"Aperture Name");
   getFlexGrid().setCellStyleBold(0,m_colAperture);
   getFlexGrid().setCell(0,m_colThermal,"Thermal");
   getFlexGrid().setCellStyleBold(0,m_colThermal);

   if (getParent().getTestDataFlag())
   {
      loadTestData();
   }

   getFlexGrid().autoSizeColumns();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage4::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   
   getParent().SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
   getParent().setCaption("Select thermal apertures");

   return CPropertyPage::OnSetActive();
}

// CGerberThermalDfmPropertyPage5 dialog
//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage5, CGerberThermalDfmPropertyPage)
CGerberThermalDfmPropertyPage5::CGerberThermalDfmPropertyPage5()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage5::IDD)
{
}

CGerberThermalDfmPropertyPage5::~CGerberThermalDfmPropertyPage5()
{
}

void CGerberThermalDfmPropertyPage5::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_VsFlexGrid, m_flexGridControl);
}

void CGerberThermalDfmPropertyPage5::loadTestData()
{
   // test data
   int rowInd = 1;
   getFlexGrid().setCell(rowInd,m_colSignal,"+5V");
   getFlexGrid().setCellChecked(rowInd,m_colPower,true);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"+3_3V");
   getFlexGrid().setCellChecked(rowInd,m_colPower,true);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"GND");
   getFlexGrid().setCellChecked(rowInd,m_colPower,true);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00001");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00002");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00003");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00004");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00005");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00006");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00007");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00008");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00009");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00010");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00011");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00012");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00013");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00014");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00015");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colSignal,"NET00016");
   getFlexGrid().setCellChecked(rowInd,m_colPower,false);
}

// fool class wizard
#define PropertyPage CGerberThermalDfmPropertyPage
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage5, PropertyPage)
END_MESSAGE_MAP()
#undef PropertyPage

// CGerberThermalDfmPropertyPage5 message handlers
BOOL CGerberThermalDfmPropertyPage5::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   getFlexGrid().put_Cols(2);
   getFlexGrid().put_Rows(20);
   getFlexGrid().put_FixedCols(1);
   getFlexGrid().put_FixedRows(1);

   getFlexGrid().setCell(0,m_colSignal,"Signal Name");
   getFlexGrid().setCellStyleBold(0,m_colSignal);
   getFlexGrid().setCell(0,m_colPower,"Power Signal");
   getFlexGrid().setCellStyleBold(0,m_colPower);

   if (getParent().getTestDataFlag())
   {
      loadTestData();
   }

   getFlexGrid().autoSizeColumns();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage5::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   
   getParent().SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK);
   getParent().setCaption("Edit power signals");

   return CPropertyPage::OnSetActive();
}

// CGerberThermalDfmPropertyPage6 dialog
//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CGerberThermalDfmPropertyPage6, CGerberThermalDfmPropertyPage)
CGerberThermalDfmPropertyPage6::CGerberThermalDfmPropertyPage6()
   : CGerberThermalDfmPropertyPage(CGerberThermalDfmPropertyPage6::IDD)
{
}

CGerberThermalDfmPropertyPage6::~CGerberThermalDfmPropertyPage6()
{
}

void CGerberThermalDfmPropertyPage6::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_VsFlexGrid, m_flexGridControl);
}

void CGerberThermalDfmPropertyPage6::loadTestData()
{
   // test data
   int rowInd = 1;
   getFlexGrid().setCell(rowInd,m_colGerber,"c2");
   getFlexGrid().setCell(rowInd,m_colEcad  ,"L2");
   getFlexGrid().setCell(rowInd,m_colSignal,"+5V");
   getFlexGrid().setCellChecked(rowInd,m_colAccept,false);
   getFlexGrid().setCellChecked(rowInd,m_colReject,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colGerber,"c2");
   getFlexGrid().setCell(rowInd,m_colEcad  ,"L2");
   getFlexGrid().setCell(rowInd,m_colSignal,"+3_3V");
   getFlexGrid().setCellChecked(rowInd,m_colAccept,false);
   getFlexGrid().setCellChecked(rowInd,m_colReject,false);

   rowInd++;
   getFlexGrid().setCell(rowInd,m_colGerber,"c5");
   getFlexGrid().setCell(rowInd,m_colEcad  ,"L5");
   getFlexGrid().setCell(rowInd,m_colSignal,"GND");
   getFlexGrid().setCellChecked(rowInd,m_colAccept,false);
   getFlexGrid().setCellChecked(rowInd,m_colReject,false);
}

// fool class wizard
#define PropertyPage CGerberThermalDfmPropertyPage
BEGIN_MESSAGE_MAP(CGerberThermalDfmPropertyPage6, PropertyPage)
END_MESSAGE_MAP()
#undef PropertyPage

// CGerberThermalDfmPropertyPage6 message handlers
BOOL CGerberThermalDfmPropertyPage6::OnInitDialog()
{
   CPropertyPage::OnInitDialog();

   // TODO:  Add extra initialization here
   CStringArray& ecadLayerNames   = getParent().getEcadLayerNames();
   CStringArray& gerberLayerNames = getParent().getGerberLayerNames();

   getFlexGrid().put_Editable(CFlexGrid::flexEDKbdMouse);
   getFlexGrid().put_AllowUserResizing(CFlexGrid::flexResizeColumns);
   getFlexGrid().put_AutoResize(true);
   getFlexGrid().put_AutoSizeMode(CFlexGrid::flexAutoSizeColWidth);

   getFlexGrid().put_Cols(5);
   getFlexGrid().put_Rows(10);
   getFlexGrid().put_FixedCols(3);
   getFlexGrid().put_FixedRows(1);

   getFlexGrid().put_MergeCells(true);
   getFlexGrid().put_MergeRow(0,true);
   getFlexGrid().put_MergeCol(m_colGerber,true);
   getFlexGrid().put_MergeCol(m_colEcad,true);

   getFlexGrid().setCell(0,m_colGerber,"Gerber Layer Name");
   getFlexGrid().setCellStyleBold(0,m_colGerber);
   getFlexGrid().setCell(0,m_colEcad,"ECAD Layer Name");
   getFlexGrid().setCellStyleBold(0,m_colEcad);
   getFlexGrid().setCell(0,m_colSignal,"Signal Name");
   getFlexGrid().setCellStyleBold(0,m_colSignal);
   getFlexGrid().setCell(0,m_colAccept,"Accept");
   getFlexGrid().setCellStyleBold(0,m_colAccept);
   getFlexGrid().setCell(0,m_colReject,"Reject");
   getFlexGrid().setCellStyleBold(0,m_colReject);

   if (getParent().getTestDataFlag())
   {
      loadTestData();
   }

   getFlexGrid().autoSizeColumns();

   return TRUE;  // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CGerberThermalDfmPropertyPage6::OnSetActive()
{
   // TODO: Add your specialized code here and/or call the base class

// PSWIZB_BACK   Back button 
// PSWIZB_NEXT   Next button 
// PSWIZB_FINISH   Finish button 
// PSWIZB_DISABLEDFINISH   Disabled Finish button 
   
   getParent().SetWizardButtons(PSWIZB_NEXT | PSWIZB_BACK | PSWIZB_FINISH);
   getParent().setCaption("Select acceptable layer power signals");

   return CPropertyPage::OnSetActive();
}

//_____________________________________________________________________________
CGerberThermalPropertySheet::CGerberThermalPropertySheet(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
{
   AddPage(&m_page1);
   AddPage(&m_page2);
   AddPage(&m_page3);
   //AddPage(&m_page4);
   //AddPage(&m_page5);
   //AddPage(&m_page6);

   m_ecadFile = NULL;
}

CGerberThermalPropertySheet::~CGerberThermalPropertySheet()
{
}

void CGerberThermalPropertySheet::addLayer(const CString& layerName)
{
   m_layerNames.Add(layerName);
}

void CGerberThermalPropertySheet::addEcadLayer(const CString& layerName)
{
   m_ecadLayerNames.Add(layerName);
}

void CGerberThermalPropertySheet::addGerberLayer(const CString& layerName)
{
   m_gerberLayerName.Add(layerName);
}

//void CGerberThermalPropertySheet::loadTestData()
//{
//   // Layers
//   addLayer("Assembly Top");
//   addLayer("Assembly Bottom");
//   addLayer("Board Outline");
//   addLayer("Pads All");
//   addLayer("Pads Top");
//   addLayer("Pads Bottom");
//   addLayer("L1");
//   addLayer("L2");
//   addLayer("L3");
//   addLayer("L4");
//   addLayer("L5");
//   addLayer("L6");
//   addLayer("Silkscreen Top");
//   addLayer("Silkscreen Bottom");
//   addLayer("Resist Top");
//   addLayer("Resist Bottom");
//   addLayer("Paste Top");
//   addLayer("Paste Bottom");
//   addLayer("L1 - restricts");
//   addLayer("L2 - restricts");
//   addLayer("L3 - restricts");
//   addLayer("L4 - restricts");
//   addLayer("L5 - restricts");
//   addLayer("L6 - restricts");
//
//   addLayer("c1");
//   addLayer("c2");
//   addLayer("c3");
//   addLayer("c4");
//   addLayer("c5");
//   addLayer("c6");
//   addLayer("skp");
//   addLayer("sks");
//   addLayer("srp");
//   addLayer("srs");
//   addLayer("spp");
//   addLayer("sps");
//
//   // ECAD layers
//   addEcadLayer("Pads All");
//   addEcadLayer("Pads Top");
//   addEcadLayer("Pads Bottom");
//   addEcadLayer("L1");
//   addEcadLayer("L2");
//   addEcadLayer("L3");
//   addEcadLayer("L4");
//   addEcadLayer("L5");
//   addEcadLayer("L6");
//   addEcadLayer("Silkscreen Top");
//   addEcadLayer("Silkscreen Bottom");
//   addEcadLayer("Resist Top");
//   addEcadLayer("Resist Bottom");
//   addEcadLayer("Paste Top");
//   addEcadLayer("Paste Bottom");
//
//   // Gerber layers
//   addGerberLayer("c1");
//   addGerberLayer("c2");
//   addGerberLayer("c3");
//   addGerberLayer("c4");
//   addGerberLayer("c5");
//   addGerberLayer("c6");
//   addGerberLayer("skp");
//   addGerberLayer("sks");
//   addGerberLayer("srp");
//   addGerberLayer("srs");
//   addGerberLayer("spp");
//   addGerberLayer("sps");
//}
//
//void CGerberThermalPropertySheet::load(CCEtoODBDoc& camCadDoc)
//{
//   // loop thru camCadDoc layers and add layer names 
//
//   if (getTestDataFlag())
//   {
//      loadTestData();
//   }
//
//   //camCadDoc.
//}

void CGerberThermalPropertySheet::setCaption(const CString& caption)
{
   CString newCaption(m_caption);

   if (caption != "")
   {
      newCaption += " - " + caption;
   }

   SetWindowText(newCaption);

   CString string;
   GetWindowText(string);
}

// CGerberThermalPropertySheet message handlers
BOOL CGerberThermalPropertySheet::OnInitDialog()
{
   BOOL bResult = CPropertySheet::OnInitDialog();

   // TODO:  Add your specialized code here
   GetWindowText(m_caption);

   return bResult;
}

