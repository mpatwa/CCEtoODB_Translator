// $Header: /CAMCAD/4.5/read_wrt/Idf_out.cpp 19    4/28/06 11:43a Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994, 1999. All Rights Reserved.

   A component outline is
   1. a visible primary outline
   if not found 
      2. any visible outline
      if not found
         3. nothing -> error message.

*/
                              
#include "stdafx.h"
#include "ccdoc.h"
#include "dbutil.h"                                           
#include "format_s.h"
#include "lyr_lyr.h"
#include "attrib.h"
#include "gauge.h"
#include "pcbutil.h"
#include "geomlib.h"
#include "graph.h"
#include <math.h>
#include <string.h>
#include "outline.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

typedef CArray<Point2, Point2&> CPolyArray;

typedef struct
{
   int      blocknum;
   CString  pname;         // partnumber
   int      doublename;
   double   height;
   double   color[9];
}IDF_Block;
typedef CTypedPtrArray<CPtrArray, IDF_Block*> CSldBlockArray;

typedef struct
{
   CString  name;
   double   height;
   BOOL     ignore;
   double   color[9];
}Shape;
typedef CTypedPtrArray<CPtrArray, Shape*> CShapeArray;

typedef struct
{
   CString  name;
   CString  partnumber;
   int      blocknum;
   int      blockarraycnt;
   double   height;
   double   x,y,rot;
   int      mirror;
   int      inserttype;
}IDF_Comp;
typedef CTypedPtrArray<CPtrArray, IDF_Comp*> CCompArray;

typedef struct
{
   CString  name;
   int      number;
   int      index;
}Siglay;

extern   CProgressDlg *progress;

static   CCEtoODBDoc  *doc;
static   double      unitsFactor;

static   int         display_error;
static   FILE        *flog;

static FileStruct    *cur_file;

static   int         PageUnits;
static   int         output_units_accuracy = 3;

static   int         check_name_error;

static   CShapeArray shapearray;    // this is the device - mapping
static   int         shapecnt;

static   CCompArray  comparray;     // this is the device - mapping
static   int         compcnt;

static   CPolyArray  polyarray;
static   int         polycnt;

static   CSldBlockArray blockarray;
static   int         blockcnt;

static   double      defaultlayerthickness;

static   double      SMALL_DELTA;

static   int         SHOWOUTLINE = FALSE;

static   int         WRITE_PARTNUMBER;

// this function only loops throu entities marked with the BL_CLASS_BOARD_GEOM

static int IDF_TestBOARDData(FILE *wfp, CDataList *DataList, int insertLayer, int grclass);

static int IDF_WriteBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int *loop, int insertLayer, int grclass);

static int IDF_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary);

static void do_pcbcomponents(FILE *fp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, CTypeList *TypeList);

static void get_components(CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, CTypeList *TypeList);

/*****************************************************************************/
/*
*/
static int  get_shapearrayptr(const char *name, int update)
{
   int   i;

   for (i=0;i<shapecnt;i++)
   {
      CString  tmp = shapearray[i]->name;
      if (shapearray[i]->name.CompareNoCase(name) == 0)
         return i;
   }

   if (!update)   return -1;

   Shape *c = new Shape;
   shapearray.SetAtGrow(shapecnt,c);  
   c->name = name;
   c->height = 0;
   c->ignore = 0;
   c->color[0] = -1; // default shape color
   shapecnt++; 

   return shapecnt-1;
}

/****************************************************************************/
/*
*/
static int write_components(FILE *fp)
{
   int   i = 0;
   CString  geomname;
   CString  partname;

   fprintf(fp,".PLACEMENT\n");

   for (i=0;i<compcnt;i++)
   {
      IDF_Block *b = blockarray[comparray[i]->blockarraycnt];
      BlockStruct *block = doc->Find_Block_by_Num(b->blocknum);

      if (b->doublename > 0)
         geomname.Format("%s_%d",block->getName(), b->doublename);
      else
         geomname = block->getName();

      fprintf(fp,"\"%s\" \"%s\" \"%s\"\n", geomname, comparray[i]->partnumber, comparray[i]->name);

      fprintf(fp,"%1.*lf %1.*lf %1.2lf %s PLACED\n", 
         output_units_accuracy, comparray[i]->x, output_units_accuracy, comparray[i]->y,
         RadToDeg(comparray[i]->rot), (comparray[i]->mirror)?"BOTTOM":"TOP");

   }

   fprintf(fp,".END_PLACEMENT\n");

   return i;
}

/****************************************************************************/
/*
*/
static int load_IDFSettings(const CString fname, int idf_units)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   WRITE_PARTNUMBER = TRUE;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      MessageBox(NULL, tmp,"IDF Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = get_string(line," \t\n")) == NULL) continue;
      if (lp[0] != '.')                         continue;

      if (!STRCMPI(lp,".DEFAULTLAYERTHICKNESS"))
      {
         char  c;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;   // I = inches M = MM
         c = lp[0];
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;

         if (c == 'm' || c == 'M')
         {
            defaultlayerthickness = atof(lp) * Units_Factor(UNIT_MM, idf_units);
         }
         else
         {
            defaultlayerthickness = atof(lp) * Units_Factor(UNIT_INCHES, idf_units);
         }
      }
      else
      if (!STRCMPI(lp,".DEFAULTSHAPEHEIGHT"))
      {
         char  c;
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;   // I = inches M = MM
         c = lp[0];

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         int p = get_shapearrayptr("*",1);

         if (c == 'm' || c == 'M')
         {
            shapearray[p]->height = atof(lp) * Units_Factor(UNIT_MM, idf_units);
         }
         else
         {
            shapearray[p]->height = atof(lp) * Units_Factor(UNIT_INCHES, idf_units);
         }
      }
      else
      if (!STRCMPI(lp,".SHAPEHEIGHT"))
      {
         char  c;

         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         int p = get_shapearrayptr(lp,1);

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // I = inches M = MM
         c = lp[0];

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // heights

         if (c == 'm' || c == 'M')
         {
            shapearray[p]->height = atof(lp) * Units_Factor(UNIT_MM, idf_units);
         }
         else
         {
            shapearray[p]->height = atof(lp) * Units_Factor(UNIT_INCHES, idf_units);
         }
      }
      else
      if (!STRCMPI(lp,".SHAPEIGNORE"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         int p = get_shapearrayptr(lp,1);
         shapearray[p]->ignore = TRUE;
      }
      else
      if (!STRCMPI(lp,".DEFAULTSHAPECOLOR"))
      {
         int p = get_shapearrayptr("*",1);

         for (int i=0;i<9;i++)
         {
            if ((lp = get_string(NULL," \t\n")) != NULL) 
               shapearray[p]->color[i] = atof(lp);
         }
      }
      else
      if (!STRCMPI(lp,".SHAPECOLOR"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         int p = get_shapearrayptr(lp,1);

         for (int i=0;i<9;i++)
         {
            if ((lp = get_string(NULL," \t\n")) != NULL) 
               shapearray[p]->color[i] = atof(lp);
         }
      }
      else
      if (!STRCMPI(lp,".SHOWOUTLINE"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         if (lp[0] == 'Y' || lp[0] == 'y')
         {
            SHOWOUTLINE = TRUE;
         }
      }
      else
      if (!STRCMPI(lp,".OUTPUT_UNIT_ACCURACY"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         output_units_accuracy = atoi(lp);
      }
      else
      if (!STRCMPI(lp,".WRITE_PARTNUMBER"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL) continue;
         if (lp[0] == 'n' || lp[0] == 'N')
         {
            WRITE_PARTNUMBER = FALSE;
         }
      }
   }

   fclose(fp);
   return 1;
}

/*****************************************************************************/
/*
*/
static int write_newoutline(FILE *wfp, CPntList *pntList)
{
   CPnt     *p;
   POSITION pos;
   int      loop = 0;

   if (pntList == NULL || pntList->IsEmpty())
   {
      return 0;
   }

   polycnt = 0;
   pos = pntList->GetHeadPosition();
   double   da = 0;  // bulge is between n-1 and n
   while (pos != NULL)
   {
      p = pntList->GetNext(pos);
      fprintf(wfp,"%d %1.*lf %1.*lf %1.*lf\n", loop, 
         output_units_accuracy, p->x, output_units_accuracy, p->y, output_units_accuracy, RadToDeg(da));
      da = atan(p->bulge) * 4;
   }

   if (SHOWOUTLINE)
   {
      //doc->PrepareAddEntity(cur_file);        
      int l = Graph_Level("DEBUG","",0);
      DataStruct *d = Graph_PolyStruct(l,0L, FALSE);  // poly is always with 0

      Graph_Poly(NULL,0, 0,0, 0);

      pos = pntList->GetHeadPosition();
      while (pos != NULL)
      {
         p = pntList->GetNext(pos);
         Graph_Vertex(p->x/unitsFactor, p->y/unitsFactor, p->bulge);    // p1 coords
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   return 1;
}


/*****************************************************************************/
/*
*/
static int write_easyoutline(FILE *wfp, CPoly *pp)
{
   CPnt     *p;
   POSITION pos;
   int      loop = 0;

   if (pp == NULL)
   {
      return 0;
   }

   polycnt = 0;
   pos = pp->getPntList().GetHeadPosition();
   double   da = 0;  // bulge is between n-1 and n
   while (pos != NULL)
   {
      p = pp->getPntList().GetNext(pos);
      fprintf(wfp,"%d %1.*lf %1.*lf %1.*lf\n", loop, 
         output_units_accuracy, p->x, output_units_accuracy, p->y, output_units_accuracy, RadToDeg(da));
      da = atan(p->bulge) * 4;
   }

   if (SHOWOUTLINE)
   {
      //doc->PrepareAddEntity(cur_file);        
      int l = Graph_Level("DEBUG","",0);
      DataStruct *d = Graph_PolyStruct(l,0L, FALSE);  // poly is always with 0

      Graph_Poly(NULL,0, 0,0, 0);

      pos = pp->getPntList().GetHeadPosition();
      while (pos != NULL)
      {
         p = pp->getPntList().GetNext(pos);
         Graph_Vertex(p->x/unitsFactor, p->y/unitsFactor, p->bulge);    // p1 coords
      }
   }

   polyarray.RemoveAll();
   polycnt = 0;
   return 1;
}

//--------------------------------------------------------------
void do_components(FILE *fp, double scale, const char *unitstring)
{
   int      i;
   double   accuracy = get_accuracy(doc);

   for (i=0;i<blockcnt;i++)
   {
      BlockStruct *block = doc->Find_Block_by_Num(blockarray[i]->blocknum);

      CString  btyp;
      if (block->getBlockType() == BLOCKTYPE_PCBCOMPONENT)
         btyp = "ELECTRICAL";
      else
         btyp = "MECHANICAL";

      // first look for Primary visible outline
      Outline_Start(doc);

      int found = IDF_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, TRUE);
      if (!found)
      {
         // now look for every visible outline
         found = IDF_WriteSHAPEData(fp, &(block->getDataList()), 0.0, 0.0, 0.0, 0, scale, 0, -1, FALSE);
         if (!found)
         {
            fprintf(flog,"Did not find a any (closed) Component Outline for [%s].\n",block->getName());
            display_error++;
         }
      }

      CString  idf_blockname;

      if (blockarray[i]->doublename > 0)
         idf_blockname.Format("%s_%d",block->getName(), blockarray[i]->doublename);
      else
         idf_blockname = block->getName();

      if (found)
      {
         fprintf(fp,".%s\n", btyp);

         if (!WRITE_PARTNUMBER)
            fprintf(fp,"\"%s\" \"%s\" %s %1.*lf\n",idf_blockname, idf_blockname, unitstring, 
               output_units_accuracy, blockarray[i]->height);
         else
         {
            fprintf(fp,"\"%s\" \"%s\" %s %1.*lf\n",idf_blockname, blockarray[i]->pname, unitstring, 
               output_units_accuracy, blockarray[i]->height);
         }

         Graph_Block_On(GBO_APPEND,block->getName(),-1,0);
         int returnCode;
         write_newoutline(fp, Outline_GetOutline(&returnCode, accuracy));
         Outline_FreeResults();
         Graph_Block_Off();

         // here color
         if (blockarray[i]->color[0] > -1)
         {
            fprintf(fp,"#Color");
            for (int ii=0;ii<9;ii++)
               fprintf(fp," %lf",blockarray[i]->color[ii]);     
            fprintf(fp,"\n");
         }
         else
            fprintf(fp,"#Color 0.6 0.6 0.6 1.0 1.0 1.0 0.3 0 0\n");

         fprintf(fp,".END_%s\n", btyp);
      }
   }

   return;               
}

/*****************************************************************************/
/*
*/
static int is_block_color(double c1[9], double c2[9])
{
   int   i;

   for (i=0;i<9;i++)
   {
      if (c1[i] != c2[i])  
         return 0;
   }

   return 1;
}

/*****************************************************************************/
/*
*/
static int update_blockarray(int bnum, const char *pname, double height, double color[9])
{
   int         i, doublename = 0;

   for (i=0;i<blockcnt;i++)
   {
      // if not the same, continue
      if (blockarray[i]->pname.Compare(pname))  continue;
      if (blockarray[i]->blocknum == bnum)   ++doublename;

      if (blockarray[i]->blocknum == bnum && blockarray[i]->height == height && 
            is_block_color(blockarray[i]->color, color))
         return i;
   }
   

   IDF_Block *b = new IDF_Block;
   blockarray.SetAtGrow(blockcnt,b);
   blockcnt++;

   for (i=0;i<9;i++)
      b->color[i] = color[i];

   b->blocknum = bnum;
   b->pname = pname; // part name
   b->height = height;
   b->doublename = doublename;

   return blockcnt-1;
}

//--------------------------------------------------------------
void get_components(CDataList *DataList, double insert_x, double insert_y,
                       double rotation, int mirror,  
                       double scale, int embeddedLevel, int insertLayer,
                       CTypeList *TypeList)
{
   DataStruct  *np;
   double      cur_height;
   Mat2x2      m;
   Point2      point2;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())                                        
      {
         case T_INSERT:
         {
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            double color[9];     // default color for shape
            color[0] = -1; // set to no color

            if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT ||
                np->getInsert()->getInsertType() == BLOCKTYPE_MECHCOMPONENT)
            {
               CString compname = np->getInsert()->getRefname();
               // EXPLODEALL - call DoWriteData() recursively to write embedded entities
               point2.x = np->getInsert()->getOriginX() * scale;
               if (mirror)
                  point2.x = -point2.x;

               point2.y = np->getInsert()->getOriginY() * scale;
               TransPoint2(&point2, 1, &m, insert_x, insert_y);

               // insert if mirror is either global mirror or block_mirror, but not if both.
               int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
               double block_rot = rotation + np->getInsert()->getAngle();

               if (block_mirror)
                  block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.
               
               // first set default
               cur_height = 0.5 * scale;  // default if no sldwrks.out is found
               // now get default from .out file
               int p = get_shapearrayptr("*",0);
               if (p > -1)
               {
                  cur_height = shapearray[p]->height ;   // .defaultshape  already in correct scale
                  memcpy(color,shapearray[p]->color,sizeof(double)*9);
               }
               p = get_shapearrayptr(block->getName(),0);

               // now get individual from .out file
               if (p > -1)
               {
                  if (shapearray[p]->ignore)
                  {
                     fprintf(flog,"Shape [%s] ignored\n",block->getName());
                     continue;
                  }
                  else
                  {
                     if (shapearray[p]->height > 0)
                        cur_height = shapearray[p]->height; // .defaultshape already in correct scale
                     if (shapearray[p]->color[0] > -1)   // here check if shapecolor was done.
                        memcpy(color,shapearray[p]->color,sizeof(double)*9);
                  }
               }

               // now check if attribute on shape
               Attrib *a =  is_attvalue(doc, block->getAttributesRef(), ATT_COMPHEIGHT, 1);
               if (a)
               {
                  CString l = get_attvalue_string(doc, a);
                  cur_height = atof(l) * scale;
               }
   
               // check if there is a component height at a type for this pattern.
               TypeStruct *typ;
               POSITION typPos;

               typPos = TypeList->GetHeadPosition();
               while (typPos != NULL)
               {                                         
                  typ = TypeList->GetNext(typPos);

                  if (typ->getBlockNumber() == block->getBlockNumber())
                  {
                     Attrib *a =  is_attvalue(doc, typ->getAttributesRef(), ATT_COMPHEIGHT, 1);
                     if (a)
                     {
                        CString l = get_attvalue_string(doc, a);
                        cur_height = atof(l) * scale;
                     }
                  }
               }

               // now check if attribute on component
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_COMPHEIGHT, 1))
               {
                  CString l = get_attvalue_string(doc, a);
                  cur_height = atof(l) * scale;
               }
               CString  pname;
               pname = "";
               if (a =  is_attvalue(doc, np->getAttributesRef(), ATT_TYPELISTLINK, 1))
               {
                  pname = get_attvalue_string(doc, a);
               }

               if (!WRITE_PARTNUMBER)
                  pname = block->getName();

               // here add to comparray
               IDF_Comp *c = new IDF_Comp;
               comparray.SetAtGrow(compcnt,c);
               c->blocknum = np->getInsert()->getBlockNumber();
               c->blockarraycnt = update_blockarray(np->getInsert()->getBlockNumber(), pname, cur_height, color);
               c->name = compname;
               c->height = cur_height;
               c->x = point2.x;
               c->y = point2.y;
               c->rot = block_rot;
               c->mirror = block_mirror;
               c->inserttype = np->getInsert()->getInsertType();
               c->partnumber = pname;
               compcnt++;  
         
               break;
            }
         } // case INSERT
         break;
      } // end switch
   } // end get_components */
   return;
}

//--------------------------------------------------------------
static int write_layercolor(FILE *fp)
{
   fprintf(fp,"#Color 0 1 0 1 1 1 .3 0 0\n");
   return 1;
}

//--------------------------------------------------------------
void IDF_Layout_WriteFiles(const char *Filename, CCEtoODBDoc *Doc,FormatStruct *format,
                       int page_units, double UnitsFactor, const char *includeFile)
{
   FILE *emn, *emp;
   FileStruct *file;

   int   pcb_found = FALSE;
   doc = Doc;
   display_error = FALSE;
   check_name_error = 0;
   PageUnits = page_units;

   // here check it there is a panel file also
   // open file for writting
   char empname[_MAX_PATH];
   char emnname[_MAX_PATH];
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( Filename, drive, dir, fname, ext );
   strcpy(empname,drive);
   strcat(empname,dir);
   strcat(empname,fname);
   strcat(empname,".emp");

   strcpy(emnname,drive);
   strcat(emnname,dir);
   strcat(emnname,fname);
   strcat(emnname,".emn");
 
   CString  f;
   f.Format("Processing %s+%s", empname, emnname);
   progress->SetStatus(f);

   CString logFile = GetLogfilePath("idf.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // Set Static variables so don't have to pass them every time
   CString  unitstring;
   int      idf_units;
   // board_xy scale
   // board_xy units
   switch (page_units)
   {
      break;
      case UNIT_MM:
         unitstring = "MM";
         unitsFactor = Units_Factor(page_units, UNIT_MM);
         idf_units = UNIT_MM;
      break;
      default:
         unitstring = "THOU";
         unitsFactor = Units_Factor(page_units, UNIT_MILS);
         idf_units = UNIT_MILS;
      break;
   }

   output_units_accuracy = GetDecimals(idf_units); 

   SMALL_DELTA = 1;
	int i=0;
   for (i=0;i<output_units_accuracy;i++)
      SMALL_DELTA *= 0.1;

   // open file for writing
   if ((emn = fopen(emnname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",emnname);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

   CTime t;
   t = t.GetCurrentTime();

   fprintf(emn,".HEADER\n");
   fprintf(emn,"BOARD_FILE 2.0 \"%s\" %s 1\n", getApp().getCamCadTitle(), t.Format("%Y/%m/%d.%H:%M:%S"));
   fprintf(emn,"\"%s\" %s\n", fname, unitstring);
   fprintf(emn,".END_HEADER\n");

   if ((emp = fopen(empname, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",empname);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }
   fprintf(emp,".HEADER\n");
   fprintf(emp,"LIBRARY_FILE 2.0 \"%s\" %s 1\n", getApp().getCamCadTitle(), t.Format("%Y/%m/%d.%H:%M:%S"));
   fprintf(emp,".END_HEADER\n");

   shapearray.SetSize(100,100);
   shapecnt = 0;

   comparray.SetSize(100,100);
   compcnt = 0;

   polyarray.SetSize(100,100);
   polycnt = 0;

   blockarray.SetSize(100,100);
   blockcnt = 0;

   SHOWOUTLINE = FALSE;

   CString settingsFile( getApp().getExportSettingsFilePath("idf.out") );
   load_IDFSettings(settingsFile, idf_units);

   POSITION pos = doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      cur_file = file = doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;

      if (file->getBlockType() == BLOCKTYPE_PCB)  
      {
         doc->PrepareAddEntity(cur_file);       
         pcb_found = TRUE;

         fprintf(emn,".BOARD_OUTLINE\n");
         // here board thickness
         double   boardthickness = defaultlayerthickness;

         Attrib   *a;
         if (a =  is_attvalue(doc, file->getBlock()->getAttributesRef(), BOARD_THICKNESS, 2)) 
         {
            CString l = get_attvalue_string(doc, a);
            boardthickness = atof(l) * file->getScale() * unitsFactor;
         }

         fprintf(emn,"%1.*lf\n", output_units_accuracy, boardthickness);
         int   loop = 0;
         int outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_BOARDOUTLINE);
         IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, graphicClassBoardCutout);

         if (!outline_found)
         {
            fprintf(flog,"Did not find a \"Closed Primary BoardOutline\"!\n.");
            display_error++;
         }
         write_layercolor(emn);
         fprintf(emn,".END_BOARD_OUTLINE\n");

         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_BOARD_GEOM);
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_ALLKEEPIN);
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_ALLKEEPOUT);

         if (outline_found)
         {
            fprintf(emn,".OTHER_OUTLINE\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_BOARD_GEOM);
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_ALLKEEPIN);
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_ALLKEEPOUT);
            fprintf(emn,".END_OTHER_OUTLINE\n");
         }

         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_ROUTKEEPIN);
         if (outline_found)
         {
            fprintf(emn,".ROUTE_OUTLINE\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_ROUTKEEPIN);
            fprintf(emn,".END_ROUTE_OUTLINE\n");
         }


         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_ROUTKEEPOUT);
         if (outline_found)
         {
            fprintf(emn,".ROUTE_KEEPOUT\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_ROUTKEEPOUT);
            fprintf(emn,".END_ROUTE_KEEPOUT\n");
         }

         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_VIAKEEPOUT);
         if (outline_found)
         {
            fprintf(emn,".VIA_KEEPOUT\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_VIAKEEPOUT);
            fprintf(emn,".END_VIA_KEEPOUT\n");
         }

         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_PLACEKEEPIN);
         if (outline_found)
         {
            fprintf(emn,".PLACE_OUTLINE\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_PLACEKEEPIN);
            fprintf(emn,".END_PLACE_OUTLINE\n");
         }

         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, GR_CLASS_PLACEKEEPOUT);
         if (outline_found)
         {
            fprintf(emn,".PLACE_KEEPOUT\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, GR_CLASS_PLACEKEEPOUT);
            fprintf(emn,".END_PLACE_KEEPOUT\n");
         }


         outline_found = 0;
         outline_found += IDF_TestBOARDData(emn, &(file->getBlock()->getDataList()), -1, graphicClassPlacementRegion);
         if (outline_found)
         {
            fprintf(emn,".PLACE_REGION\n");
            loop = 0;
            outline_found = IDF_WriteBOARDData(emn, &(file->getBlock()->getDataList()), 
               file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
               file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, &loop, -1, graphicClassPlacementRegion);
            fprintf(emn,".END_PLACE_REGION\n");
         }
         // drill holes
         // placement

         // collect pcb and mechanical parts
         get_components(&(file->getBlock()->getDataList()), 
            file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
            file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1,
            &file->getTypeList());

         write_components(emn);

         // write all parts
         do_components(emp,file->getScale() * unitsFactor, unitstring);

         break; // do only one pcb file
      } // if pcb file
   }
   fclose(emn);
   fclose(emp);

   for (i=0;i<shapecnt;i++)
   {
      delete shapearray[i];
   }
   shapearray.RemoveAll();

   polyarray.RemoveAll();

   for (i=0;i<blockcnt;i++)
   {
      delete blockarray[i];
   }
   blockarray.RemoveAll();

   for (i=0;i<compcnt;i++)
   {
      delete comparray[i];
   }
   comparray.RemoveAll();

   if (!pcb_found)
   {
      MessageBox(NULL, "No PCB file found !","IDF Output", MB_OK | MB_ICONHAND);
      return;
   }

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

//--------------------------------------------------------------
int IDF_WriteSHAPEData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer, int primary)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int layer;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() == T_TEXT)    continue;
      if (np->getDataType() != T_INSERT)
      {
         if (insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         // insert has not always a layer definition.
         if (!doc->get_layer_visible(layer, mirror))
         {
            continue;
         }
         if (primary)
         {
            if (np->getGraphicClass() != GR_CLASS_COMPOUTLINE)  
               continue;
         }
         else
         {
            // do it
         }
         Outline_Add(np, scale, insert_x, insert_y, rotation, mirror);
         found++;
      }
      else
      {
         if (np->getInsert()->getInsertType() != 0)
            continue;

         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot = rotation + np->getInsert()->getAngle();

         if (mirror)
            block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // do nothing
         }
         else // not aperture
         {
            int block_layer = -1;
            if (insertLayer != -1)
               block_layer = insertLayer;
            else 
            if (np->getLayerIndex() != -1)
               block_layer = np->getLayerIndex();

            found += IDF_WriteSHAPEData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               embeddedLevel+1, block_layer, primary);
         } // end else not aperture
      } // if INSERT

   } // end IDF_WriteSHAPEData */

   return found;
}

//--------------------------------------------------------------
int IDF_WriteBOARDData(FILE *wfp, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int *loop, int insertLayer, int grclass)
{
   Point2 point2;
   DataStruct *np;
   Mat2x2 m;
   int found = 0;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch (np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            if (np->getGraphicClass() != grclass)   break; 

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();

               if (!closed)                                 break;

               if (poly->isVoid())  *loop = *loop + 1;

               found++;
               pntPos = poly->getPntList().GetHeadPosition();
               double   da = 0;  // bulge is between n-1 and n
               Point2   p2;
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p2.x = pnt->x * scale;
                  if (mirror) p2.x = -p2.x;
                  p2.y = pnt->y * scale;
                  p2.bulge = pnt->bulge;
                  TransPoint2(&p2, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  fprintf(wfp,"%d %1.*lf %1.*lf %1.*lf\n", *loop, 
                     output_units_accuracy, p2.x, output_units_accuracy, p2.y, output_units_accuracy, RadToDeg(da));
                  da = atan(p2.bulge) * 4;
               }
            }  // while
         }
         break;  // POLYSTRUCT
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != 0) // do not do any inside hierachies, only 
               continue;                     // flat graphics.

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;
   
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = rotation + np->getInsert()->getAngle();
   
            if (mirror)
               block_rot = PI2 - (rotation + np->getInsert()->getAngle());   // add 180 degree if mirrored.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
               block_layer = insertLayer;
               else 
               if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               found += IDF_WriteBOARDData(wfp,&(block->getDataList()), 
                               point2.x, point2.y,
                               block_rot, block_mirror,
                               scale * np->getInsert()->getScale(),
                               loop, block_layer, grclass);
            } // end else not aperture
         } // if INSERT
         break;
      }
   } // end IDF_WriteBOARDData */

   return found;
}
//--------------------------------------------------------------
int IDF_TestBOARDData(FILE *wfp, CDataList *DataList, int insertLayer, int grclass)
{
   DataStruct *np;
   int       found = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);

      switch (np->getDataType())
      {
         case T_POLY:
         {
            if (np->getGraphicClass() != grclass)   break; 
            found++;
            return found;

         }
         break;  // POLYSTRUCT
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != 0) // do not do any inside hierachies, only 
               continue;                     // flat graphics.

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               // do nothing
            }
            else // not aperture
            {
               int block_layer = -1;
               if (insertLayer != -1)
               block_layer = insertLayer;
               else 
               if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               found += IDF_TestBOARDData(wfp,&(block->getDataList()), block_layer, grclass);
            } // end else not aperture
         } // if INSERT
         break;
      }
   } // end IDF_TestBOARDData */

   return found;
}

//--------------------------------------------------------------
static int  write_seg(FILE *fp,double xx1, double yy1,double bulge1, 
                      double xx2, double yy2, double bulge2,
                      int layernr, int widthindex)
{
   double da = atan(bulge1) * 4;

   if (fabs(da) > SMALLANGLE)
   {
      int     i, ppolycnt;
      Point2  ppoly[255];
      double cx,cy,r,sa;

      ArcPoint2Angle(xx1,yy1,xx2,yy2,da,&cx,&cy,&r,&sa);
      // make positive start angle.
      if (sa < 0) sa += PI2;

      ppolycnt = 255;
      // start center
      ArcPoly2(xx1,yy1,cx,cy,da,
            1.0,ppoly,&ppolycnt,DegToRad(5));   // 5 degree

      for (i=1;i<ppolycnt;i++)
      {
         write_seg(fp,ppoly[i-1].x, ppoly[i-1].y, 0.0, 
                      ppoly[i].x, ppoly[i].y, 0.0,layernr, widthindex);
      }
   }
   else
   {
      fprintf(fp,"SEG: %lf, %lf, %lf, %lf, %d , %d\n",
         xx1, yy1, xx2, yy2, layernr, widthindex);
   }
   return 1;
}

/****************************************************************************/
/*
   end IDF_OUT.CPP
*/
/****************************************************************************/

