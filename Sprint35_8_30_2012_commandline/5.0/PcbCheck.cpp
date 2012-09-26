// $Header: /CAMCAD/4.5/PcbCheck.cpp 22    4/28/06 12:03p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include "CCEtoODB.h"
#include "pcbcheck.h"
#include "gauge.h"
#include "pcbutil.h"
#include "polylib.h"
#include "RwUiLib.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


typedef  struct
{
   char  *name;
   int   (*function)(FILE *, int, CCEtoODBDoc *doc, FileStruct *f);
} List;

static int  verify_closed_outline_components(FILE *, int, CCEtoODBDoc *doc, FileStruct *f);
static int  verify_closed_board_outline(FILE *, int, CCEtoODBDoc *doc, FileStruct *f);
static int  verify_component_mirror(FILE *, int, CCEtoODBDoc *doc, FileStruct *f);
static int  verify_hierachical_padstacks(FILE *, int, CCEtoODBDoc *doc, FileStruct *f);

static List validate_processes[] =
{
   //"Hierachical Nets",               fnull,
   //"Hierachical Components",         fnull,
   "Unique Closed Outline Components", verify_closed_outline_components,
   "Hierachical Padstacks",            verify_hierachical_padstacks,
   //"Boundary Outlines",              fnull,
   //"Empty Padstacks",                fnull,
   //"Complex padstacks",              fnull,
   //"Etch without netnames",          fnull,
   "Odd Mirrored Components",          verify_component_mirror,
   //"Padstacks without drill",        fnull,
   //"Free Voids",                     fnull,
   "Closed Boardoutline",              verify_closed_board_outline,
   //"Netlist Consistency",            fnull,
   //"ZeroSize Aperture",              fnull,
   //"PCBComponents without electrical pins", fnull,
   //"Single Layer Via",               fnull,
};
#define  max_validate_processes  (sizeof(validate_processes) / sizeof(List))

static BOOL runProcess[max_validate_processes];

/////////////////////////////////////////////////////////////////////////////
// ValidatePCBdlg dialog
ValidatePCBdlg::ValidatePCBdlg(CWnd* pParent /*=NULL*/)
   : CDialog(ValidatePCBdlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(ValidatePCBdlg)
   m_action = 0;
   //}}AFX_DATA_INIT
}

void ValidatePCBdlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ValidatePCBdlg)
   DDX_Control(pDX, IDC_LIST1, m_list);
   DDX_Radio(pDX, IDC_ACTION, m_action);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ValidatePCBdlg, CDialog)
   //{{AFX_MSG_MAP(ValidatePCBdlg)
   ON_BN_CLICKED(IDC_SELECT, OnSelect)
   ON_BN_CLICKED(IDC_UNSELECT, OnUnselect)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ValidatePCBdlg message handlers
BOOL ValidatePCBdlg::OnInitDialog() 
{
   CDialog::OnInitDialog();

   for (int i=0; i<max_validate_processes; i++)
   {
      runProcess[i] = FALSE;
      m_list.AddString(validate_processes[i].name);
   }

   OnSelect();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void ValidatePCBdlg::OnOK() 
{
   int count = m_list.GetSelCount();
   
   if (!count) return;

   int *indices = (int*)calloc(count, sizeof(int));
   m_list.GetSelItems(count, indices);
   for (int i=0; i<count; i++)
      runProcess[indices[i]] = TRUE;

   free(indices);
   
   CDialog::OnOK();
}

void ValidatePCBdlg::OnSelect() 
{
   m_list.SelItemRange(TRUE, 0, m_list.GetCount());   
}

void ValidatePCBdlg::OnUnselect() 
{
   m_list.SelItemRange(FALSE, 0, m_list.GetCount());  
}


/****************************************************************************
* OnVerifyDesignData
*/
void CCEtoODBDoc::OnVerifyDesignData() 
{
   ValidatePCBdlg dlg;
   if (dlg.DoModal() != IDOK) return;

   CString  f;
   FILE     *flog;

   CString verifyLogFile = GetLogfilePath("verifypcb.log");

   remove(verifyLogFile);

   if ((flog = fopen(verifyLogFile,"wt")) == NULL)
   {
      CString  t;
      t.Format("Error open [%s] file", verifyLogFile);
      ErrorMessage(t, "Error");
      return;
   }

   CTime t;
   t = t.GetCurrentTime();
   fprintf(flog,"Verify Design Files - %s\n\n", t.Format("%A, %B %d, %Y at %H:%M:%S"));

   CWaitCursor wait;

   //CProgressDlg *progress;

   //progress = new CProgressDlg("Prepare Data for Verification...", FALSE);
   //progress->Create();
   
   // only need this once
   generate_PADSTACKACCESSFLAG(this, FALSE);

   //progress->DestroyWindow();
   //delete progress;

   // doc is this
   POSITION filePos = this->getFileList().GetHeadPosition();
   while (filePos != NULL)
   {
      FileStruct *file = this->getFileList().GetNext(filePos);

      if (!file->isShown() || file->isHidden())
         continue;

      if (file->getBlockType() != BLOCKTYPE_PCB)
         continue;

      CProgressDlg *gauge = new CProgressDlg;

      gauge->caption.Format("Verify Design Data for %s", file->getName());
      gauge->Create();
      gauge->SetStatus("Prepare Data for Verification...");
      gauge->SetPos(0);

      generate_PINLOC(this,file,0); // this function generates the PINLOC argument for all pins.

      for (int i=0; i<max_validate_processes; i++)
      {
         if (!runProcess[i])  continue;
         //validate_processes[i];
         gauge->SetStatus(validate_processes[i].name);
         gauge->SetPos(10*i);
         if (((*validate_processes[i].function)(flog, i, this, file)) < 0)
         {
            fprintf(flog,"Error in Function ???\n");
         }

      }
      delete gauge;
   }

   fprintf(flog,"End of verify\n");

   fclose(flog);
   Notepad(verifyLogFile);

   return;
}

//--------------------------------------------------------------
/*
*/
static int verify_compoutline( CCEtoODBDoc *doc, CDataList *DataList, int insertLayer)
{
   DataStruct *np;
   int         found = 0;
   int         mirror = 0;
   int         layer = insertLayer;
   LayerStruct *ll;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         ll = doc->FindLayer(layer);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            POSITION polyPos;

            if (np->getGraphicClass() != GR_CLASS_COMPOUTLINE && ll->getLayerType() != LAYTYPE_COMPONENTOUTLINE)  break;

            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isClosed()) found++;
            }
         }
         break;
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else
            {
               found += verify_compoutline(doc, &(block->getDataList()), layer);
            }
         }
         break;                                                                
      } // end switch
   } // end verify_outline 
   return found;
}

//--------------------------------------------------------------
/*
   return 0 = no graphic, emtpy
   return 1 = good
   return > 1 nesting found
   
   here I need to check by layer.

*/
static int verify_padstack( FILE *fp, CCEtoODBDoc *doc, CDataList *DataList, int insertLayer, int insertlevel)
{
   DataStruct *np;
   int         found = 0;
   int         layer = insertLayer;
   LayerStruct *ll;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, 0))
            continue;
         ll = doc->FindLayer(layer);
      }

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            ll = doc->FindLayer(block_layer);

            CString  typ;
            typ = "Insert";
            if (block->getFlags() & BL_TOOL)       typ = "Drill";
            if (block->getFlags() & BL_APERTURE)   typ = "Aperture";

            fprintf(fp,"\t%*s %s [%s] [%s]\n", insertlevel, "-", typ, block->getName(), (ll)?ll->getName():"no layer");

            if ( (block->getFlags() & BL_TOOL) ||(block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {

               // if it is a normal aperture, OK
               if (block->getShape() == T_COMPLEX)
               {
                  BlockStruct *subblock = doc->Find_Block_by_Num((int)block->getSizeA());
                  verify_padstack(fp, doc, &(subblock->getDataList()), block_layer, insertlevel+1);
                  found = 2;
               }
               else
               {
                  found = 1;
               }
            }
            else
            {
               found += verify_padstack(fp, doc, &(block->getDataList()), block_layer, insertlevel+1);
            }
         }
         break;
         case T_POLY:
         {
            CString  typ = "";
            CPoly    *poly;
            POSITION polyPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               double   cx, cy, radius;
               double   llx, lly, uux, uuy;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  typ = "Circle";
               }
               else
               if (PolyIsRectangle(poly, &llx, &lly, &uux, &uuy))
               {
                  typ = "Rectangle";
               }
               else
               {
                  typ = "Poly";
               } // not rectangle
               fprintf(fp,"\t%*s POLY %s [%s]\n", insertlevel, "-", typ, (ll)?ll->getName():"no layer");
            }  // while
         }
         break;
         default:
            // other contens
            CString  typ = "Unknown";

            if (np->getDataType() == T_DRAW)    typ = "Draw";
            if (np->getDataType() == T_TEXT)    typ = "Text";
            if (np->getDataType() == T_DRAW)    typ = "Draw";
            if (np->getDataType() == T_POINT)   typ = "Point";
            if (np->getDataType() == T_BLOB)    typ = "Blob";

            fprintf(fp,"\t%*s %s [%s]\n", insertlevel, "-", typ, (ll)?ll->getName():"no layer");

         break;
      } // end switch
   } // end verify_padstack 
   return found;
}

/****************************************************************************/
/*
*/
int verify_closed_outline_components(FILE *fp, int funcptr, CCEtoODBDoc *doc, FileStruct *f)
{
   fprintf(fp,"Verify %s\n\n", validate_processes[funcptr].name);
   fprintf(fp,"PCB Component Shapes are checked for a unique, closed Outline\n");
   fprintf(fp,"1. as PRIMARY OUTLINE\n");
   fprintf(fp,"2. on the COMPONENT OUTLINE layer type\n");
   fprintf(fp,"\n");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      // create necessary aperture blocks
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
      {  
         fprintf(fp,"\tChecking %s ", block->getName());
         int res = verify_compoutline(doc, &(block->getDataList()), -1);

         if (res == 0)
            fprintf(fp,"No outline graphic found");
         else
         if (res == 1)
            fprintf(fp," Verified OK");
         else
            fprintf(fp,"Multiple outline graphic found");

         fprintf(fp,"\n");
      }
   }

   fprintf(fp,"\n");
   fprintf(fp,"To fix these components, use ADD | Outline Selected function\n");

   fprintf(fp,"\n");
   return 1;
}

/****************************************************************************/
/*
*/
int verify_hierachical_padstacks(FILE *fp, int funcptr, CCEtoODBDoc *doc, FileStruct *f)
{

   // here remove all empty or zerosize apertures.
   remove_zero_size_pads(doc);


   fprintf(fp,"Verify %s\n\n", validate_processes[funcptr].name);
   fprintf(fp,"Hierachical Padstacks\n");
   fprintf(fp,"\n");

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)   continue;
      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if (block->getBlockType() == BLOCKTYPE_PADSTACK)
      {  
         fprintf(fp,"Checking %s\n", block->getName());

         int res = verify_padstack(fp, doc, &(block->getDataList()), -1, 1);

         if (res == 0)
            fprintf(fp,"Empty Padstack - No outline graphic found!");
         else
         if (res == 1)
            fprintf(fp,"Verified OK");
         else
         {
            fprintf(fp,"Nested");
            //flatten_padstack(doc, block);
         }
         fprintf(fp,"\n");
      }
   }

   fprintf(fp,"\n");
   return 1;
}

//--------------------------------------------------------------
/*
*/
static int verify_boardoutline( CCEtoODBDoc *doc, CDataList *DataList, int insertLayer)
{
   DataStruct *np;
   int         found = 0;
   int         mirror = 0;
   int         layer = insertLayer;
   LayerStruct *ll;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
            continue;
         ll = doc->FindLayer(layer);
      }

      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            POSITION polyPos;

            if (np->getGraphicClass() != GR_CLASS_BOARDOUTLINE && ll->getLayerType() != LAYTYPE_BOARD_OUTLINE) break;

            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isClosed()) found++;
            }
         }
         break;
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
            }
            else
            {
               found += verify_boardoutline(doc, &(block->getDataList()), layer);
            }
         }
         break;                                                                
      } // end switch
   } // end verify_boardoutline 
   return found;
}

//--------------------------------------------------------------
/*
*/
static int verify_componentmirror( FILE *fp, CCEtoODBDoc *doc, CDataList *DataList, int insertLayer)
{
   DataStruct *np;
   int         found = 0;
   int         mirror = 0;
   int         layer = insertLayer;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      if (np->getDataType() != T_INSERT)  continue;


      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)   
            {
               int good_mirror = (MIRROR_FLIP | MIRROR_LAYERS);

               // not bottom and not mirrored.
               if (!np->getInsert()->getPlacedBottom() && !np->getInsert()->getMirrorFlags()) break;
               // bottom and mirrored
               if (np->getInsert()->getPlacedBottom() && np->getInsert()->getMirrorFlags() == good_mirror)   break;

               fprintf(fp,"Component [%s] is placed %s and layers %s and placed on %s\n",
                        np->getInsert()->getRefname(), 
                        (np->getInsert()->getMirrorFlags() & MIRROR_FLIP)?"Flipped":"Not Flipped",
                        (np->getInsert()->getMirrorFlags() & MIRROR_LAYERS)?"Mirrored":"Not Mirrored",
                        (np->getInsert()->getPlacedBottom())?"Bottom":"Top");
            }
         }
         break;                                                                
      } // end switch
   } // end verify_componentmirror 
   return found;
}


/****************************************************************************/
/*
*/
int verify_closed_board_outline(FILE *fp, int funcptr, CCEtoODBDoc *doc, FileStruct *f)
{
   fprintf(fp,"Verify %s\n\n", validate_processes[funcptr].name);
   fprintf(fp,"PCB File is checked for a unique, closed Board Outline\n");
   fprintf(fp,"1. as PRIMARY BOARDOUTLINE\n");
   fprintf(fp,"2. on the BOARD OUTLINE layer type\n");
   fprintf(fp,"\n");

   int res = verify_boardoutline(doc, &(f->getBlock()->getDataList()), -1);

   if (res == 0)
      fprintf(fp,"\tNo board outline  found");
   else
   if (res == 1)
      fprintf(fp,"\tVerified OK");
   else
      fprintf(fp,"\tMultiple board outline graphic found");
   fprintf(fp,"\n");

   fprintf(fp,"\n");
   fprintf(fp,"To fix the Boardoutline, use Join or create one\n");

   fprintf(fp,"\n");
   return 1;
}

/****************************************************************************/
/*
*/
int verify_component_mirror(FILE *fp, int funcptr, CCEtoODBDoc *doc, FileStruct *f)
{
   fprintf(fp,"Verify %s\n\n", validate_processes[funcptr].name);
   fprintf(fp,"PCB File is checked for Components, which are oddly placed and mirrored\n");
   fprintf(fp,"\n");

   int res = verify_componentmirror(fp, doc, &(f->getBlock()->getDataList()), -1);

   fprintf(fp,"\n");
   return 1;
}

/*Ende **********************************************************************/

/*
CheckForNetnameAttribsNotInNetlist()
{
   CMapWordToPtr netnameMap;
   void *voidPtr;
   POSITION dataPos = file->getBlock()->getDataList().GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = file->getBlock()->getDataList().GetNext(dataPos);

      if (data->getAttributesRef() && data->getAttributesRef()->Lookup(kw, voidPtr))
      {
         Attrib *attrib = (Attrib*)voidPtr;
         if (!netnameMap.Lookup(attrib->getStringValueIndex(), voidPtr))
            netnameMap.SetAt(attrib->getStringValueIndex(), NULL);
      }
   }

   CStringArray missingNets;
   int maxMissingNets = 0;
   POSITION mapPos = netnameMap.GetStartPosition();
   while (mapPos)
   {
      WORD val_str;
      netnameMap.GetNextAssoc(mapPos, val_str, voidPtr);
      CString netname = ValueArray[val_str];

      BOOL Found = FALSE;
      POSITION netPos = file->getNetList().GetHeadPosition();
      while (netPos)
      {
         NetStruct *net = file->getNetList().GetNext(netPos);
         if (net->getNetName() == netname)
         {
            Found = TRUE;
            break;
         }
      }

      if (!Found)
         missingNets.SetAtGrow(maxMissingNets++, netname);
   }

   if (maxMissingNets)
   {
      ErrorMessage("Missing Nets");
   }
}
*/