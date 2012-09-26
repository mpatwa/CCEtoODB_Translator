// $Header: /CAMCAD/5.0/DftDialog.cpp 4     6/17/07 8:51p Kurt Van Ness $
     
/****************************************************************************/
/*  Project CAMCAD                                     
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/           

#include "StdAfx.h"
#include <math.h>
#include <float.h>
#include "CCEtoODB.h"
#include "DftDialog.h"
#include "ccdoc.h"
#include "attrib.h"
#include "graph.h"
#include "find.h"
#include "lyrmanip.h"
#include "dft_func.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "compvalues.h"
#include "net_util.h"
#include "Debug.h"
#include "Dft.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
static   CProbeArray    probearray;
static   int            probecnt;

static   DFT_ProbeGraphic  probegraphic;

/*****************************************************************************/
/*
*/
static void put_probes(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, const char *prefix,
                     double refheight, int layernr, int nevermirror)

{
   // update refname and possible ATT_REFNAME
   CString  pname;
   CUnits curUnit(doc->getSettings().getPageUnits());

   for (int i=0;i<probecnt;i++)
   {
      CDataList *dataList;
      DataStruct *data = FindDataEntity(doc, probearray[i]->getEntityNumber(), &dataList, NULL);
      if (data)
      {
         pname.Format("%s%d",prefix,probearray[i]->sequencenr/* + nameincrement*/);
         data->getInsert()->setRefname(STRDUP(pname));

         // get the probe drill size, use 100mil as default
         double drillSize = 0.1;
         BlockStruct *probeBlock = doc->getBlockAt(data->getInsert()->getBlockNumber());
         if (probeBlock != NULL)
         {
            POSITION pos = probeBlock->getHeadDataInsertPosition();
            while (pos)
            {
               DataStruct *drillData = probeBlock->getNextDataInsert(pos);
               BlockStruct *drillBlock = doc->getBlockAt(drillData->getInsert()->getBlockNumber());

               if (drillBlock != NULL && drillBlock->getBlockType() == blockTypeDrillHole)
               {
                  drillSize = drillBlock->getToolSize();
                  break;
               }
            }
         }

         // Here fix up the ATT_REFNAME
         CreateTestProbeRefnameAttr(doc, data, pname, drillSize, nevermirror?true:false);

     }
      else
      {
         // not found ????
         ErrorMessage("Entity not found in put_probe", "DFT.CPP");
      }
   }

   return;
}

/*****************************************************************************/
/*
*/
static void get_probes(CCEtoODBDoc *doc, FileStruct *file, CDataList *DataList, TestProbeSequenceSurface tpSurface)
{
   DataStruct  *np;
   int         cnt = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      BOOL placed = TRUE;
      if (np->getInsert()->getInsertType() != INSERTTYPE_TEST_PROBE)  
         continue;

      if (Attrib* attrib = is_attvalue(doc, np->getAttributeMap(), ATT_PROBEPLACEMENT, 0))
      {
         CString placement = attrib->getStringValue();
         if (!placement.CompareNoCase("Unplaced"))
            placed = FALSE;
      }

      if (placed && tpSurface == tpssUnplaced)
         continue;
      if (!placed && tpSurface != tpssUnplaced)
         continue;

      if (np->getInsert()->getPlacedBottom() != (tpSurface == tpssBottom))               
         continue;

      BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

      DFT_probes *c = new DFT_probes;
      probearray.SetAtGrow(probecnt++, c);

      c->setEntityNumber(np->getEntityNumber());
      c->probename = np->getInsert()->getRefname();
      c->x = np->getInsert()->getOriginX();
      c->y = np->getInsert()->getOriginY();
      c->bottom = np->getInsert()->getPlacedBottom();
      c->sequencenr = ++cnt;
      c->placed = placed;
   } // end get_probes */
}

/*****************************************************************************/
/* 

*/
static   int   read_probedeffile(CCEtoODBDoc *doc)
{
   char     line[255];
   char     *lp;
   FILE     *fp;

   probegraphic.scale = -1;
   probegraphic.PROBEGRIDLAYER = "PROBEGRID";
   probegraphic.PROBEDRILLTOP = "PROBEDRILLTOP";
   probegraphic.PROBEDRILLBOTTOM = "PROBEDRILLBOTTOM";
   probegraphic.PROBETOP = "PROBETOP";
   probegraphic.PROBEBOTTOM = "PROBEBOTTOM";
   probegraphic.PROBENAMETOP = "PROBENAMETOP";
   probegraphic.PROBENAMEBOTTOM = "PROBENAMEBOTTOM";
   probegraphic.PROBEPATHTOP = "PROBEPATHTOP";
   probegraphic.PROBEPATHBOTTOM = "PROBEPATHBOTTOM";
   probegraphic.framewidth = -1;

   CString probeFile(getApp().getSystemSettingsFilePath("default.prb"));
   if ((fp = fopen(probeFile,"rt")) != NULL)
   {
      while (fgets(line,255,fp))
      {
         if ((lp = get_string(line," \t\n")) == NULL) continue;
         if (lp[0] != '.')                         continue;

         if (!STRCMPI(lp,".PROBEUNITS"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            if (!STRNICMP(lp,"IN",2))
            {
               probegraphic.scale = Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MI",2))
            {
               probegraphic.scale = Units_Factor(UNIT_MILS, doc->getSettings().getPageUnits());
            }
            else
            if (!STRNICMP(lp,"MM",2))
            {
               probegraphic.scale = Units_Factor(UNIT_MM, doc->getSettings().getPageUnits());
            }
         }
         else
         if (!STRCMPI(lp,".PROBEPATHTOP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEPATHTOP = lp;
         }
         else
         if (!STRCMPI(lp,".PROBEPATHBOTTOM"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEPATHBOTTOM = lp;
         }
         else
         if (!STRCMPI(lp,".PROBENAMETOP"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBENAMETOP = lp;
         }
         else
         if (!STRCMPI(lp,".PROBENAMEBOTTOM"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBENAMEBOTTOM = lp;
         }
         else
         if (!STRCMPI(lp,".PROBEGRIDLAYER"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.PROBEGRIDLAYER = lp;
         }
         else
         if (!STRCMPI(lp,".FRAMEWIDTH"))
         {
            if ((lp = get_string(NULL," \t\n")) == NULL) continue; 
            probegraphic.framewidth = atof(lp) * probegraphic.scale;
         }
      }
      fclose(fp);
   }
   else
   {
      CString tmp;
      tmp.Format("[%s] not found!", probeFile);
      ErrorMessage("Missing Probe Definition File. No probes loaded.", tmp);
   }
   return 1;
}

/******************************************************************************
* 
*/
static int do_snake_sort(int startingPoint, double stripewidth)
{
   ErrorMessage("Snake Algorithm not implemented -> Rename to Read Order", "DFT");

   return 1;
}

/******************************************************************************
* 
*/
static int plot_probes(CCEtoODBDoc *doc, TestProbeSequenceSurface tpSurface, const char *probepathlayer)
{
   if (tpSurface == tpssUnplaced)
      return 1;

   doc->UnselectAll(FALSE);

   LayerStruct *ll;

   if ((ll = doc->FindLayer_by_Name(probepathlayer)) != NULL)
      DeleteLayer(doc, ll, TRUE);


   // top layer
   int l = Graph_Level(probepathlayer, "", 0);
   LayerStruct *lp = doc->FindLayer(l);

   if (lp != NULL)  // We just made it, so lp should not be NULL, but Klocwork can't tell that
   {
      if (tpSurface == tpssBottom) // only if no type yet assigned
         lp->setLayerType(LAYTYPE_BOTTOM);
      else if (tpSurface == tpssTop)
         lp->setLayerType(LAYTYPE_TOP);
   }

   DataStruct *d = Graph_PolyStruct(l,0L,0);
   Graph_Poly(NULL, 0, 0, 0, 0);

   for (int i=0;i<probecnt;i++)
   {
#ifdef _DEBUG
      DFT_probes *pp = probearray[i];
#endif
      if (probearray[i]->bottom != (tpSurface == tpssBottom))   continue;
      Graph_Vertex(probearray[i]->x, probearray[i]->y, 0.0);
   }

   return 1;
}

/*****************************************************************************/
/*
   -1
   0
   +1
*/
static int probekooCompareFunc( const void *arg1, const void *arg2 )
{
   DFT_probes **a1, **a2;
   a1 = (DFT_probes**)arg1;
   a2 = (DFT_probes**)arg2;
   
   double dist1, dist2;

   //dist1 = (*a1)->xnorm + (*a1)->ynorm;
   //dist2 = (*a2)->xnorm + (*a2)->ynorm;
   dist1 = (*a1)->ynorm;
   dist2 = (*a2)->ynorm;
   
   if (dist1 < dist2)   return -1;
   if (dist1 > dist2)   return +1;

   // if equal use the lower x
   dist1 = (*a1)->xnorm;
   dist2 = (*a2)->xnorm;

   if (dist1 < dist2)   return -1;
   if (dist1 > dist2)   return +1;

   return 0;
}

/*****************************************************************************/
/*
   If actually calls sort twice, once without any pins, once with. That is 
   because of the 2 pass to get artwork info.
*/
static void SortprobeData_LowerLeft_Stripe(int maxsteps)
{
   if (probecnt < 2) return;

   
   // set all to unused
   int   i;
   int   used = 0;

   for (i=0;i<probecnt;i++)
      probearray[i]->used = used;   // init to 0

/*
   // loop thru all xstep
   for (int s=0;s<maxsteps;s++)
   {
      double dist = FLT_MAX;
      int    distindex = -1;

      for (i=0;i<probecnt;i++)
      {
         if (probearray[i]->xstep != s)   continue;
         if (probearray[i]->used)         continue;

         // find lower left and unused.
         if ((probearray[i]->xnorm + probearray[i]->ynorm) < dist)
         {

            dist = probearray[i]->xnorm + probearray[i]->ynorm;
            distindex = i;
         }
      }

   }
*/
   qsort(probearray.GetData(),probecnt, sizeof(DFT_probes *), probekooCompareFunc);

   return;
}

/******************************************************************************
* 
*/
static int do_stripe_sort(FileStruct *file, int startingPoint, double stripewidth)
{
   int      xstep = 0;
   int      ystep = 0;
   double   maxx = file->getBlock()->getXmax(); //-FLT_MAX;
   double   maxy = file->getBlock()->getYmax(); //-FLT_MAX;
   double   minx = file->getBlock()->getXmin(); // FLT_MAX;
   double   miny = file->getBlock()->getYmin(); // FLT_MAX;
/*
   for (i=0;i<probecnt;i++)
   {
      if (probearray[i]->x > maxx)
         maxx = probearray[i]->x;
      if (probearray[i]->y > maxy)
         maxy = probearray[i]->y;
      if (probearray[i]->x < minx)
         minx = probearray[i]->x;
      if (probearray[i]->y < miny)
         miny = probearray[i]->y;
   }
*/
   int   maxxsteps;

   maxxsteps = (int)floor((maxx - minx) / stripewidth) + 1;

   // assume starting point lowerleft
   for (int i=0;i<probecnt;i++)
   {
#ifdef _DEBUG
   DFT_probes *pp = probearray[i];
#endif
      probearray[i]->xstep = (int)floor((probearray[i]->x - minx) / stripewidth); // if starting point is lower left
      probearray[i]->ystep = 0;
      probearray[i]->xnorm = probearray[i]->x - minx + probearray[i]->ystep*(maxx - minx);
      probearray[i]->ynorm = probearray[i]->y - miny + probearray[i]->xstep*(maxy - miny);
   }

   // the sort checks the xsteps first and than sorts inside the xsteps for distance
   SortprobeData_LowerLeft_Stripe(maxxsteps*2);

   return 1;
}

/*****************************************************************************/
/*
*/
static int do_layer(CCEtoODBDoc *doc, FileStruct *file, TestProbeSequenceSurface tpSurface, int algorithm, 
                     const char *prefix, int startingPoint, double bandwidth, 
                     int startNumber, int nameIncrement)
{
   probearray.SetSize(100,100);
   probecnt = 0;

   // get all probes
   get_probes(doc, file, &(file->getBlock()->getDataList()), tpSurface);

   // sort
   if (algorithm == 0)  // snake
      do_snake_sort(startingPoint, bandwidth);
   else
   if (algorithm == 1)  // stripe
      do_stripe_sort(file, startingPoint, bandwidth);
   else
      ErrorMessage("Unknown Sort Algorithm -> Rename to Read Order", "DFT");

   CString  probepathlayer;
   if (tpSurface == tpssBottom)
      probepathlayer = probegraphic.PROBEPATHBOTTOM;
   else
      probepathlayer = probegraphic.PROBEPATHTOP;

   plot_probes(doc, tpSurface, probepathlayer);

   // update the sequence number, used in names.
   int probeIndex = startNumber;
	int i=0;
   for (i=0;i<probecnt;i++)
      probearray[i]->sequencenr = probeIndex + (nameIncrement*i);
   probeIndex += nameIncrement * probecnt;

   int neverMirror = FALSE;
   double refnameheight = 0.05 * Units_Factor(UNIT_INCHES, doc->getSettings().getPageUnits());

   // make refdes layers
   int l = Graph_Level(probegraphic.PROBENAMETOP, "", 0);
   LayerStruct *lp = doc->FindLayer(l);
   if (lp != NULL && lp->getLayerType() == 0) // only if no type yet assigned
      lp->setLayerType(LAYTYPE_TOP);
   l = Graph_Level(probegraphic.PROBENAMEBOTTOM, "", 0);
   lp = doc->FindLayer(l);
   if (lp != NULL && lp->getLayerType() == 0) // only if no type yet assigned
      lp->setLayerType(LAYTYPE_BOTTOM);

   Graph_Level_Mirror(probegraphic.PROBENAMETOP, probegraphic.PROBENAMEBOTTOM, "");

   // update names
   put_probes(doc, file, &(file->getBlock()->getDataList()), prefix, refnameheight,
               Graph_Level(probegraphic.PROBENAMETOP,"",0) , neverMirror);

   for (i=0;i<probecnt;i++)
   {
      delete probearray[i];
   }

   probearray.RemoveAll();
   probecnt = 0;

   return probeIndex;
}

/*****************************************************************************/
/*
*/
static int plot_frame(CCEtoODBDoc *doc, double stripewidth, double x_min, double y_min, double x_max, double y_max)
{
   LayerStruct *ll;
   if ((ll = doc->FindLayer_by_Name(probegraphic.PROBEGRIDLAYER)) != NULL)
      DeleteLayer(doc, ll, TRUE);

   int l = Graph_Level(probegraphic.PROBEGRIDLAYER, "", 0);

   int xintstep = (int)floor((x_max - x_min) / stripewidth) + 1;
   int yintstep = (int)floor((y_max - y_min) / stripewidth) + 1;

   double   totxwidth = xintstep * stripewidth;
   double   totywidth = yintstep * stripewidth;
   double   frameminx = x_min, framemaxx = x_max, frameminy= y_min, framemaxy = y_max;

   frameminx = x_min - (totxwidth - (x_max - x_min))/2;
   frameminy = y_min - (totywidth - (y_max - y_min))/2;
   framemaxx = frameminx + totxwidth;
   framemaxy = frameminy + totywidth;

   DataStruct *d = Graph_PolyStruct(l,0L,0);

   // do not make the first alpha devider !
   double step = frameminx + stripewidth;
   int   i;
   for (i=0;i<xintstep-1;i++) // this make the X deviders, not first, not last
   {
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( step, frameminy, 0.0);
      Graph_Vertex( step, frameminy-probegraphic.framewidth, 0.0);
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( step, framemaxy, 0.0);
      Graph_Vertex( step, framemaxy+probegraphic.framewidth, 0.0);
      step += stripewidth;
   }

   // do not make the first numner devider !
   step = frameminy + stripewidth;
   for (i=0;i<yintstep-1;i++) // this make the Y deviders, not first, not last
   {
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( frameminx, step, 0.0);
      Graph_Vertex( frameminx-probegraphic.framewidth, step, 0.0);
      Graph_Poly(NULL, 0, 0, 0, 0);
      Graph_Vertex( framemaxx, step, 0.0);
      Graph_Vertex( framemaxx+probegraphic.framewidth, step, 0.0);
      step += stripewidth;
   }

   // inner frame
   Graph_Poly(NULL, 0, 0, 0, 0);
   Graph_Vertex( frameminx, frameminy, 0.0);
   Graph_Vertex( framemaxx, frameminy, 0.0);
   Graph_Vertex( framemaxx, framemaxy, 0.0);
   Graph_Vertex( frameminx, framemaxy, 0.0);
   Graph_Vertex( frameminx, frameminy, 0.0);

   // outer frame
   Graph_Poly(NULL, 0, 0, 0, 0);
   Graph_Vertex( frameminx-probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);
   Graph_Vertex( framemaxx+probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);
   Graph_Vertex( framemaxx+probegraphic.framewidth, framemaxy+probegraphic.framewidth, 0.0);
   Graph_Vertex( frameminx-probegraphic.framewidth, framemaxy+probegraphic.framewidth, 0.0);
   Graph_Vertex( frameminx-probegraphic.framewidth, frameminy-probegraphic.framewidth, 0.0);

   // now nomenclature X = A..Z, Y = 1..9


   return 1;
}

/******************************************************************************
* OnDftTestprobeSequence
*/
void CCEtoODBDoc::OnDftTestprobeSequence() 
{
   TestProbeSequence dlg;

   dlg.m_algorithm = 1;          // 0=Snake, 1=Stripe
   dlg.m_startingPoint = 0;      // 0=LL, 1=UL, 2=LR, 3=UR

   double width = 1 * Units_Factor(UNIT_INCHES, getSettings().getPageUnits()); // 1 inch default
   int decimals = GetDecimals(getSettings().getPageUnits());

   dlg.m_prefix.Empty();
   dlg.m_name_increment = 1;
   dlg.m_iStartNumber = 1;
   dlg.m_width.Format("%.*lf", decimals, width);

   read_probedeffile(this);

   if (dlg.DoModal() != IDOK)
      return;

   CWaitCursor w;

   //dlg.m_prefix
   // do here
   POSITION filePos = getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;
      if (file->getBlockType()  != BLOCKTYPE_PCB)
         continue;

      DFT_TestProbeSequence(this, file, dlg.m_algorithm, dlg.m_startingPoint, 
         dlg.m_prefix, atof(dlg.m_width), dlg.m_iStartNumber, dlg.m_name_increment);

   }

   UpdateAllViews(NULL);
   /*CMainFrame* frame = (CMainFrame*)AfxGetMainWnd();
   frame->navigator.setDoc(this);*/

   return;
}


/////////////////////////////////////////////////////////////////////////////
// TestProbeSequence dialog
TestProbeSequence::TestProbeSequence(CWnd* pParent /*=NULL*/)
   : CDialog(TestProbeSequence::IDD, pParent)
   , m_iStartNumber(1)
{
   //{{AFX_DATA_INIT(TestProbeSequence)
   m_algorithm = -1;
   m_startingPoint = -1;
   m_width = _T("");
   m_prefix = _T("");
   m_name_increment = 1;
   //}}AFX_DATA_INIT
}

void TestProbeSequence::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(TestProbeSequence)
   DDX_Radio(pDX, IDC_ALGORITM, m_algorithm);
   DDX_Radio(pDX, IDC_STARTING_POINT, m_startingPoint);
   DDX_Text(pDX, IDC_WIDTH, m_width);
   DDX_Text(pDX, IDC_PREFIX, m_prefix);
   DDX_Text(pDX, IDC_NAME_INCREMENT, m_name_increment);
   DDV_MinMaxInt(pDX, m_name_increment, 1, 10);
   DDX_Text(pDX, IDC_NAME_STARTNUMBER, m_iStartNumber);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(TestProbeSequence, CDialog)
   //{{AFX_MSG_MAP(TestProbeSequence)
      // NOTE: the ClassWizard will add message map macros here
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/******************************************************************************
* CKelvinAnalysisDialog dialog
******************************************************************************/
IMPLEMENT_DYNAMIC(CKelvinAnalysisDialog, CDialog)
CKelvinAnalysisDialog::CKelvinAnalysisDialog(CCEtoODBDoc *doc, CWnd* pParent /*=NULL*/)
   : CDialog(CKelvinAnalysisDialog::IDD, pParent)
   , m_useResistor(FALSE)
   , m_useCapacitor(FALSE)
   , m_useInductor(FALSE)
   , m_resistorValue(0)
   , m_capacitorValue(0)
{
   pDoc = doc;
}

CKelvinAnalysisDialog::~CKelvinAnalysisDialog()
{
}

void CKelvinAnalysisDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Check(pDX, IDC_RESISTOR_CHK, m_useResistor);
   DDX_Check(pDX, IDC_CAPACITOR_CHK, m_useCapacitor);
   DDX_Check(pDX, IDC_INDUCTOR_CHK, m_useInductor);
   DDX_Text(pDX, IDC_RESISTOR_VAL, m_resistorValue);
   DDX_Text(pDX, IDC_CAPACITOR_VAL, m_capacitorValue);
}


BEGIN_MESSAGE_MAP(CKelvinAnalysisDialog, CDialog)
   ON_BN_CLICKED(IDC_RESISTOR_CHK, OnBnClickedResistorChk)
   ON_BN_CLICKED(IDC_CAPACITOR_CHK, OnBnClickedCapacitorChk)
   ON_BN_CLICKED(IDC_PROCESS, OnBnClickedProcess)
END_MESSAGE_MAP()


// CKelvinAnalysisDialog message handlers
BOOL CKelvinAnalysisDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   loadDefaultValues();

   GetDlgItem(IDC_RESISTOR_VAL)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_RESISTOR_UNIT)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_CAPACITOR_VAL)->EnableWindow(m_useCapacitor);
   GetDlgItem(IDC_CAPACITOR_UNIT)->EnableWindow(m_useCapacitor);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CKelvinAnalysisDialog::loadDefaultValues()
{
   UINT value = 0;
   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();
   
   m_useResistor = (pApp->GetProfileInt("Kelvin Analysis", "UseResistor", 0) == 0)?false:true;
   m_useCapacitor = (pApp->GetProfileInt("Kelvin Analysis", "UseCapacitor", 0) == 0)?false:true;
   m_useInductor = (pApp->GetProfileInt("Kelvin Analysis", "UseInductor", 0) == 0)?false:true;

   value = pApp->GetProfileInt("Kelvin Analysis", "ResistorVal", 0);
   m_resistorValue = (double)value / 1.0e3;
   value = pApp->GetProfileInt("Kelvin Analysis", "CapacitorVal", 0);
   m_capacitorValue = (double)value / 1.0e3;

   UpdateData(FALSE);
}

void CKelvinAnalysisDialog::saveDefaultValues()
{
   UpdateData(TRUE);

   CCEtoODBApp *pApp = (CCEtoODBApp*)AfxGetApp();

   pApp->WriteProfileInt("Kelvin Analysis", "UseResistor", (m_useResistor)?1:0);
   pApp->WriteProfileInt("Kelvin Analysis", "UseCapacitor", (m_useCapacitor)?1:0);
   pApp->WriteProfileInt("Kelvin Analysis", "UseInductor", (m_useInductor)?1:0);

   if (m_useResistor)
      pApp->WriteProfileInt("Kelvin Analysis", "ResistorVal", (int)(m_resistorValue * 1.0e3));
   if (m_useCapacitor)
      pApp->WriteProfileInt("Kelvin Analysis", "CapacitorVal", (int)(m_capacitorValue * 1.0e3));
}

void CKelvinAnalysisDialog::OnBnClickedResistorChk()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_RESISTOR_VAL)->EnableWindow(m_useResistor);
   GetDlgItem(IDC_RESISTOR_UNIT)->EnableWindow(m_useResistor);
}

void CKelvinAnalysisDialog::OnBnClickedCapacitorChk()
{
   UpdateData(TRUE);

   GetDlgItem(IDC_CAPACITOR_VAL)->EnableWindow(m_useCapacitor);
   GetDlgItem(IDC_CAPACITOR_UNIT)->EnableWindow(m_useCapacitor);
}

void CKelvinAnalysisDialog::OnBnClickedProcess()
{
   int netAffectedCount = 0;

   saveDefaultValues();

   netAffectedCount = ::doKelvinAnalysis(pDoc, m_useResistor?true:false, m_resistorValue, m_useCapacitor?true:false, m_capacitorValue, m_useInductor?true:false);

   CString buf;
   buf.Format("A total of %d net(s) were affected.", netAffectedCount);
   MessageBox(buf, "Kelvin Analysis Complete");
}



/////////////////////////////////////////////////////////////////////////////
// CDFTTarget
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDFTTargetPriority
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CDFTProbeTemplate
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CDFTFeature
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDFTPreconditions
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
// CDFTProbeableNet
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CDFTNetConditions
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTestPlan
/////////////////////////////////////////////////////////////////////////////



