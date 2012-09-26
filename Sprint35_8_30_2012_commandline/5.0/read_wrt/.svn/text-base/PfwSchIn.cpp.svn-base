// $Header: /CAMCAD/5.0/read_wrt/PfwschIn.cpp 20    3/12/07 12:54p Kurt Van Ness $

/****************************************************************************
*
   Converts PROTEL SCHEMATIC files -> .

   - pfw.err file under the directory of the input file.

   need to do:
   - clock pin symbol
   - layout directives
   - sim proble
   - sim stimulus]
   - no erc
   - textframe
   - round rectangle

   this will not work:
   - Elliptical arcs, which are ellipes will be converted to arc / circle
   - bitmap, pic and image.
   - berzier curves.
*/

/* Defines *****************************************************************/

#include "stdafx.h"
#include "ccdoc.h"
#include "geomlib.h"
#include "file.h"
#include "dbutil.h"
#include "format_s.h"
#include "graph.h"
#include "math.h"
#include "attrib.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"
#define  MAX_LINE          255      /* Max line length. This includes macros */
#define  MAX_WIDTH         100
#define  MAX_POLY          500
#define  MAX_FONT          255
#define  FONT_CORRECT      0.5
#define  HEIGHT_RATIO      (FONT_CORRECT *(6.0/8.0)) // height to with ratio
#define  MAX_PARTS         1000
#define  MAX_PINS          1000     // maximum pins per component.

#define  BusWidth          10 // multiplies normal line width

#define STAT_UNKNOWN       0
#define STAT_SHEET         1

/* Structures **************************************************************/

typedef struct
{
   double height;    // char height
   double width;     // char width
} sFont;

typedef struct
{
   unsigned char   b0:1;
   unsigned char   b1:1;
   unsigned char   hidden:1;
   unsigned char   showpinname:1;
   unsigned char   showpinnumber:1;
   unsigned char   rotation:3;
   double          pinlength;
   double          x,y;
   char         *pinname;
   char         *pinnumber;
   int             layernum;
} Pinlist;

typedef struct
{
   int      partcnt;    // total number of gates in comp.
   int      componentid;
   int      logcompid;
   int      normalpinlistcnt;
   Pinlist  *normalpinlist;
   int      demorganpinlistcnt;
   Pinlist  *demorganpinlist;
   int      ieeepinlistcnt;
   Pinlist  *ieeepinlist;
}Libpart;

typedef struct
{
   char *organization;
   char *address1;
   char *address2;
   char *address3;
   char *address4;
   char *title;
   char *title_text;
   char *rev_number;
   char *sheet_number;
   char *sheet_total;
} Organization;

typedef struct
{
   CString  netname;
   double   x,y;
}PFWSchemNetlabel;
typedef CTypedPtrArray<CPtrArray, PFWSchemNetlabel*> CNetlabelArray;

typedef struct
{
   double   x,y;
}PFWSchemJunction;
typedef CTypedPtrArray<CPtrArray, PFWSchemJunction*> CJunctionArray;

typedef struct
{
   DataStruct *d;
   double   x,y;
   int      netname_assigned;
   int      vertexid;
   BOOL last;
}PFWSchemVertex;
typedef CTypedPtrArray<CPtrArray, PFWSchemVertex*> CVertexArray;


static   char  *powersymbol[7] = {"Circle","Arrow","Bar","Wave","Gnd","GndSignal","GndEarth"};
static   int      pageUnits;

static   CCEtoODBDoc *doc;

static   Point2   *ppoly;
static   int      ppolycnt;

static   Organization   organization;

static   Libpart  *libpart;
static   int      libpartcnt = 0;

static   Pinlist  *pinlist;
static   int      pinlistcnt;

static   int      compcnt = 0;

static   char     line[MAX_LINE];
static   long     linecnt = 0;

static   int      w[4];    // Width index 0..4
static   int      bus[4];     // Width index 0..4

static   sFont    *font;
static   int      layer_by_color = TRUE;

static   double   cur_sheetx = 0, cur_sheety = 0;
static   double   cur_sheetsizex = 0, cur_sheetsizey = 0;

static   long     total_filelength;
static   int      cur_status = STAT_UNKNOWN;   
static   int      cur_filenum = 0;
static   int      cur_sheetstyle;

static   CNetlabelArray netlabelarray;
static   int       netlabelcnt = 0;

static   CJunctionArray junctionarray;
static   int       junctioncnt = 0;

static   CVertexArray vertexarray;
static   int       vertexcnt = 0;

static   int       vertexid;  // this is to show which vertexes belong together

/* Globals *****************************************************************/

static   FILE     *ferr;
static   int      display_error;

static   FILE     *fp;
static   double   cnv_tok(char *);
static   double   cnv_unit(double);
static   int      check_format();
static   int      do_schematic();
static   int      get_nextline();
static   int      do_sheet();
static   int      do_sheetname();
static   int      do_sheetfilename();
static   int      do_fonttable();
static   int      do_library();
static   int      do_organization();
static   int      do_component();
static   int      do_line();
static   int      do_bus();
static   int      do_arc();
static   int      do_ellipticalarc();
static   int      do_ellipse();
static   int      do_wire();
static   int      do_busentry();
static   int      do_polyline();
static   int      do_polygon();
static   int      do_pin(Pinlist *p);
static   int      do_rectangle();
static   int      do_label();
static   int      do_sheetsymbol();
static   int      do_textframe();
static   int      do_template();
static   int      do_netlabel();
static   int      do_sheetnet();
static   int      do_port();
static   int      do_powerobject();
static   int      do_future();

static   int      get_libpartname(char *pname,int partcnt, int gatecnt, int display);
static   int      place_pins(double x,double y,int rot,int mirror,
                      int compnr,int partnr,int display, int show_hidden_pins);
static   int      find_libpart(int compnr,int partnr);
static   int      define_powersymbols();
static   char     *cnv_text(const char *l);
static   int      strlen_text(const char *l);
static   int      draw_sheet(int sheetstyle);
static   int      make_netnames(FileStruct *f);

/******************************************************************************
* ReadProtelSchematic
*/
void ReadProtelSchematic(const char *fname, CCEtoODBDoc *Doc, FormatStruct *Format, int pageunits)
{
   int         err;
   FileStruct  *file;
   
   layer_by_color = Format->LayerByColor;

   doc = Doc;
   pageUnits = pageunits;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
   //    ErrorMessage("Error open file","Error");
      return;
   }

   display_error = FALSE;
  
   CString logFile = GetLogfilePath("PFWSCH.LOG");
   if ((ferr = fopen(logFile,"wt")) == NULL)
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   if ((font = (sFont *) calloc(MAX_FONT,sizeof(sFont))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if ((ppoly = (Point2 *) calloc(MAX_POLY,sizeof(Point2))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   ppolycnt = 0;

   if ((pinlist = (Pinlist *) calloc(MAX_PINS,sizeof(Pinlist))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   pinlistcnt = 0;

   if ((libpart = (Libpart *) calloc(MAX_PARTS,sizeof(Libpart))) == NULL)
      MemErrorMessage(__FILE__, __LINE__);
   libpartcnt = 0;

   linecnt = 0;
   compcnt = 0;

   netlabelarray.SetSize(100,100);
   netlabelcnt = 0;

   junctionarray.SetSize(100,100);
   junctioncnt = 0;

   vertexarray.SetSize(100,100);
   vertexcnt = 0;
   vertexid = 0;

   organization.organization = NULL;
   organization.address1 = NULL;
   organization.address2 = NULL;
   organization.address3 = NULL;
   organization.address4 = NULL;
   organization.title = NULL;
   organization.title_text = NULL;
   organization.rev_number = NULL;
   organization.sheet_number = NULL;
   organization.sheet_total = NULL;
   
   if (check_format())
   {
      char  juncname[20];

      file = Graph_File_Start(fname, fileTypeProtelSchematic);
      file->setBlockType(blockTypeSheet);
      file->getBlock()->setBlockType(file->getBlockType());

      cur_filenum = file->getFileNumber();

      // here do it.
      // junctions 0..3
      strcpy(juncname,"JUNC_0");
      Graph_Aperture(juncname, T_ROUND,
         cnv_unit(5), 0.0, 0.0, 0.0, 0.0,0, BL_APERTURE, TRUE,&err);
      strcpy(juncname,"JUNC_1");
      Graph_Aperture(juncname, T_ROUND,
         cnv_unit(10), 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
      strcpy(juncname,"JUNC_2");
      Graph_Aperture(juncname, T_ROUND,
         cnv_unit(15), 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);
      strcpy(juncname,"JUNC_3");
      Graph_Aperture(juncname, T_ROUND,
         cnv_unit(20), 0.0,0.0, 0.0, 0.0, 0, BL_APERTURE, TRUE, &err);

      // width index
      // smallest
      w[0] = Graph_Aperture("SMALLEST", T_ROUND, cnv_unit(Format->SmallestWidth) , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      w[1] = Graph_Aperture("SMALL", T_ROUND,cnv_unit(Format->SmallWidth), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      w[2] = Graph_Aperture("MEDIUM", T_ROUND, cnv_unit(Format->MediumWidth) , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      w[3] = Graph_Aperture("LARGE", T_ROUND, cnv_unit(Format->LargeWidth) , 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      bus[0] = Graph_Aperture("BUS SMALLEST", T_ROUND, cnv_unit(Format->SmallestWidth*BusWidth), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      bus[1] = Graph_Aperture("BUS SMALL", T_ROUND, cnv_unit(Format->SmallWidth*BusWidth), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      bus[2] = Graph_Aperture("BUS MEDIUM", T_ROUND, cnv_unit(Format->MediumWidth*BusWidth), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);
      bus[3] = Graph_Aperture("BUS LARGE", T_ROUND, cnv_unit(Format->LargeWidth*BusWidth), 0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err);

      // must follow width definition.
      define_powersymbols();
      do_schematic();
      make_netnames(file);
   }
   else
   {
      // wrong format
      ErrorMessage("Wrong PFW Schematic file format","Error");
   }

   if (organization.organization)   free(organization.organization);
   if (organization.address1)       free(organization.address1);
   if (organization.address2)       free(organization.address2);
   if (organization.address3)       free(organization.address3);
   if (organization.address4)       free(organization.address4);
   if (organization.title)          free(organization.title);
   if (organization.title_text)     free(organization.title_text);
   if (organization.rev_number)     free(organization.rev_number);
   if (organization.sheet_number)   free(organization.sheet_number);
   if (organization.sheet_total)    free(organization.sheet_total);

   fclose(fp);
   free(ppoly);
   free(font);
   free(pinlist); // do not free pinnames - they will be freed in the libpart list.

   int i;

   for (i=0;i<netlabelcnt;i++)
   {
      delete netlabelarray[i];
   }
   netlabelarray.RemoveAll();
   netlabelcnt = 0;

   for (i=0;i<junctioncnt;i++)
   {
      delete junctionarray[i];
   }
   junctionarray.RemoveAll();
   junctioncnt = 0;

   for (i=0;i<vertexcnt;i++)
   {
      delete vertexarray[i];
   }
   vertexarray.RemoveAll();
   vertexcnt = 0;

   for (i=0;i<libpartcnt;i++)
   {     
     if (libpart[i].normalpinlist)
     {
       for (int p=0;p<libpart[i].normalpinlistcnt;p++)
       {
         if (libpart[i].normalpinlist[p].pinname)   free(libpart[i].normalpinlist[p].pinname);
         if (libpart[i].normalpinlist[p].pinnumber)    free(libpart[i].normalpinlist[p].pinnumber);
       }
       free(libpart[i].normalpinlist);
     }
     if (libpart[i].demorganpinlist)
     {
       for (int p=0;p<libpart[i].demorganpinlistcnt;p++)
       {
         if (libpart[i].demorganpinlist[p].pinname) free(libpart[i].demorganpinlist[p].pinname);
         if (libpart[i].demorganpinlist[p].pinnumber) free(libpart[i].demorganpinlist[p].pinnumber);
      }
      free(libpart[i].demorganpinlist);
     }
     if (libpart[i].ieeepinlist)
     {
       for (int p=0;p<libpart[i].ieeepinlistcnt;p++)
       {
         if (libpart[i].ieeepinlist[p].pinname) free(libpart[i].ieeepinlist[p].pinname);
         if (libpart[i].ieeepinlist[p].pinnumber) free(libpart[i].ieeepinlist[p].pinnumber);
       }
       free(libpart[i].ieeepinlist);
     }
   }
   free(libpart);
   libpartcnt = 0;

   fclose(ferr);

   if (display_error)
      Logreader(logFile);

   return;
}

/****************************************************************************/
/*
*/
static int junction_at(double x, double y)
{

   return 1;
}

/****************************************************************************/
/*
   check if a vertex touches a vertex
*/
static int assign_more_vertex(const char *netname, double x, double y, int id)
{
   int   i;
   int   cnt = 0;

   for (i=0;i<vertexcnt;i++)
   {
      if (vertexarray[i]->netname_assigned)  continue;
      if (vertexarray[i]->vertexid == id)    continue;

      if (vertexarray[i]->x == x && vertexarray[i]->y == y && junction_at(x, y))
      {
         // assign a netname
         vertexarray[i]->netname_assigned = TRUE;
         doc->SetAttrib(&vertexarray[i]->d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)netname, SA_OVERWRITE, NULL); //
         vertexarray[i]->d->setGraphicClass(GR_CLASS_ETCH);
         cnt++;
         assign_more_vertex(netname, x, y, vertexarray[i]->vertexid);
      }
   }
   return cnt;
}

/****************************************************************************/
/*
   here i find if a vertex touches a netname
*/
static int assign_vertex(const char *netname, double x, double y)
{
   int   i;
   double x1,y1,x2,y2,slope1,slope2, minX, maxX, minY, maxY;
   

   // loop until a matching vertex is found
   for (i=0;i<vertexcnt;i++)
   {     
      if (i+1 < vertexcnt)  // don't try to get the next point if the point is the last one in the vertexarray
      {
         if (vertexarray[i]->last == TRUE)
            continue;
         x1 = vertexarray[i]->x;
         y1 = vertexarray[i]->y;
         x2 = vertexarray[i+1]->x;
         y2 = vertexarray[i+1]->y;
   
         if (x1 < x2)
         {
            minX = x1;
            maxX = x2;
         }
         else
         {
            minX = x2;
            maxX = x1;
         }
         if (y1 < y2)
         {
            minY = y1;
            maxY = y2;
         }
         else
         {
            minY = y2;
            maxY = y1;
         }

         if (x2-x1 != 0)
            slope1 = (y2-y1)/(x2-x1); // slope we get from wire location
         if (x-x1 != 0)
            slope2 = (y-y1)/(x-x1);  // slope we get from the wire point and the netlabel point
      }

      if (x2-x1 == 0)  // when the slope is undefined
      {
         if (x != x1)
            continue;
         if (y < minY || y > maxY)
            continue;
         vertexarray[i]->netname_assigned = TRUE;
         doc->SetAttrib(&vertexarray[i]->d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)netname, SA_OVERWRITE, NULL); //
            vertexarray[i]->d->setGraphicClass(GR_CLASS_ETCH);
         return 1;

         // loop with the matching vertex to all other vertex until none is found anymore
      // assign_more_vertex(netname,x,y, vertexarray[i]->vertexid);

      }
      else if (y2-y1 == 0)
      {
         if (y != y1)
            continue;
         if (x < minX || x > maxX)
            continue;      
         // here assign netname attribute
         vertexarray[i]->netname_assigned = TRUE;
         doc->SetAttrib(&vertexarray[i]->d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)netname, SA_OVERWRITE, NULL); //
            vertexarray[i]->d->setGraphicClass(GR_CLASS_ETCH);
         return 1;


         // loop with the matching vertex to all other vertex until none is found anymore
      // assign_more_vertex(netname,x,y, vertexarray[i]->vertexid);
      }
      else if (fabs(slope1 - slope2) < SMALLNUMBER)
      {
         vertexarray[i]->netname_assigned = TRUE;
         doc->SetAttrib(&vertexarray[i]->d->getAttributesRef(),doc->IsKeyWord(ATT_NETNAME, 0),
            VT_STRING, (void *)netname, SA_OVERWRITE, NULL); //
            vertexarray[i]->d->setGraphicClass(GR_CLASS_ETCH);
         return 1;


         // loop with the matching vertex to all other vertex until none is found anymore
      // assign_more_vertex(netname,x,y, vertexarray[i]->vertexid);
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int make_netnames(FileStruct *file)
{
   // here loop through all vertex
   int   n;

   for (n=0;n<netlabelcnt;n++)
   {
      assign_vertex(netlabelarray[n]->netname, netlabelarray[n]->x, netlabelarray[n]->y);
   }

   return 1;
}

/****************************************************************************/
/*
*/
char *cnv_text(const char *l)
{
   static CString t; // only gets init once.
   unsigned int   i;
   
   t = "";
   for (i=0;i<strlen(l);i++)
   {
      char  c = l[i];
      if (l[i+1] == '\\')  
      {
         t += "%O";
         t += c;
         i++;
      }
      else
      {
         t += c;
      }
   }
   return t.GetBuffer(0);
}

/****************************************************************************/
/*
*/
static int strlen_text(const char *l)
{
   unsigned int   i;
   int   len = 0;

   if (l == NULL) return 0;

   for (i=0;i<strlen(l);i++)
   {
      if (l[i] == '\\') continue;
      len++;
   }
   return len;
}

/****************************************************************************/
/*
*/
static int define_powersymbols()
{
   int   floatlayer = Graph_Level("0", "", 1);

   // there are 7 power symbols, we need to define them here.
   Graph_Block_On(GBO_APPEND,powersymbol[0],cur_filenum, 0);      // circle
   Graph_Line(floatlayer, 0.0, 0.0, 
         Units_Factor(UNIT_MILS, pageUnits)*50, 
         0.0, 0, w[0], FALSE);
   Graph_Circle(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*75,
                   0.0, 
                   Units_Factor(UNIT_MILS, pageUnits)*25, 0, w[0],FALSE, FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND, powersymbol[1], cur_filenum, 0);    // arrow
   Graph_Line(floatlayer, 0.0, 0.0, 
         Units_Factor(UNIT_MILS, pageUnits)*50, 
         0.0, 0, w[0], FALSE);

   Graph_Line(floatlayer, Units_Factor(UNIT_MILS, pageUnits)*50, 
         Units_Factor(UNIT_MILS, pageUnits)*(-25), 
         Units_Factor(UNIT_MILS, pageUnits)*50, 
         Units_Factor(UNIT_MILS, pageUnits)*25, 0, w[0], FALSE);
   Graph_Line(floatlayer, Units_Factor(UNIT_MILS, pageUnits)*50, 
         Units_Factor(UNIT_MILS, pageUnits)*25, 
         Units_Factor(UNIT_MILS, pageUnits)*100, 0.0,
         0, w[0], FALSE);
   Graph_Line(floatlayer, Units_Factor(UNIT_MILS, pageUnits)*50, 
         Units_Factor(UNIT_MILS, pageUnits)*(-25), 
         Units_Factor(UNIT_MILS, pageUnits)*100,0.0, 0, w[0], FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND,powersymbol[2],cur_filenum, 0);      // bar
   Graph_Line(floatlayer, 0.0, 0.0, Units_Factor(UNIT_MILS, pageUnits)*100, 0.0, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100 , Units_Factor(UNIT_MILS, pageUnits)*(-50),
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*50, 0, w[0], FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND, powersymbol[3], cur_filenum, 0);    // wave
   Graph_Line(floatlayer, 0.0, 0.0,Units_Factor(UNIT_MILS, pageUnits)*75,0.0, 0, w[0], FALSE);

   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*50 , Units_Factor(UNIT_MILS, pageUnits)*50,
      Units_Factor(UNIT_MILS, pageUnits)*75,Units_Factor(UNIT_MILS, pageUnits)*25, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*75 , Units_Factor(UNIT_MILS, pageUnits)*25,
      Units_Factor(UNIT_MILS, pageUnits)*75,Units_Factor(UNIT_MILS, pageUnits)*(-2.5), 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*75, Units_Factor(UNIT_MILS, pageUnits)*(-25),
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*(-50), 0, w[0], FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND, powersymbol[4], cur_filenum, 0);    // gnd
   Graph_Line(floatlayer, 0.0, 0.0,Units_Factor(UNIT_MILS, pageUnits)*100,0.0, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, Units_Factor(UNIT_MILS, pageUnits)*(-100),
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*100, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*130 , Units_Factor(UNIT_MILS, pageUnits)*(-70),
      Units_Factor(UNIT_MILS, pageUnits)*130,Units_Factor(UNIT_MILS, pageUnits)*7, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*160, Units_Factor(UNIT_MILS, pageUnits)*(-40),
      Units_Factor(UNIT_MILS, pageUnits)*160,Units_Factor(UNIT_MILS, pageUnits)*40, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*190, Units_Factor(UNIT_MILS, pageUnits)*(-10),
      Units_Factor(UNIT_MILS, pageUnits)*190,Units_Factor(UNIT_MILS, pageUnits)*10, 0, w[0], FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND,powersymbol[5],cur_filenum, 0);      // gnd signal
   Graph_Line(floatlayer,0.0,0.0,
      Units_Factor(UNIT_MILS, pageUnits)*100,0.0, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, Units_Factor(UNIT_MILS, pageUnits)*(-100),
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*100, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*10, Units_Factor(UNIT_MILS, pageUnits)*100,
      Units_Factor(UNIT_MILS, pageUnits)*200,0.0, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*200, 0.0,
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*(-100), 0, w[0], FALSE);
   Graph_Block_Off();

   Graph_Block_On(GBO_APPEND,powersymbol[6],cur_filenum, 0);      // gnd earth
   Graph_Line(floatlayer,0.0,0.0,
      Units_Factor(UNIT_MILS, pageUnits)*100,0.0, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, Units_Factor(UNIT_MILS, pageUnits)*(-100),
      Units_Factor(UNIT_MILS, pageUnits)*100,Units_Factor(UNIT_MILS, pageUnits)*100, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, Units_Factor(UNIT_MILS, pageUnits)*100,
      Units_Factor(UNIT_MILS, pageUnits)*200,Units_Factor(UNIT_MILS, pageUnits)*50, 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, 0.0,
      Units_Factor(UNIT_MILS, pageUnits)*200,Units_Factor(UNIT_MILS, pageUnits)*(-50), 0, w[0], FALSE);
   Graph_Line(floatlayer,Units_Factor(UNIT_MILS, pageUnits)*100, Units_Factor(UNIT_MILS, pageUnits)*(-100),
      Units_Factor(UNIT_MILS, pageUnits)*200,Units_Factor(UNIT_MILS, pageUnits)*(-150), 0, w[0], FALSE);

   Graph_Block_Off();

   return TRUE;
}

/****************************************************************************/
/*
*/
static int check_format()
{
   if (get_nextline())
   {
      if (!STRCMPI(line,"Protel for Windows - Schematic Capture Ascii File Version 1.2 - 2.0\n"))
         return TRUE;
   }

   return FALSE;
}

/****************************************************************************/
/*
*/
static int do_library()
{
   char  *lp;
   int   number_of_parts;
   int   i;

   if (get_nextline())
   {
      if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;
      number_of_parts = atoi(lp);

      for (i=0;i<number_of_parts;i++)
      {
         if (!get_nextline()) return FALSE;  // this the component header
         if (!do_component())
         {
            // big error
            char tmp[80];
            sprintf(tmp,"Error in Component format at %ld",linecnt);
            ErrorMessage(tmp,"Error");
            return FALSE;
         }
      }
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_organization()
{
   char *lp;

   if (!get_nextline()) return FALSE;   // organization
   if ((organization.organization = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // adress 1
   if ((organization.address1 = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // adress 2
   if ((organization.address2 = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // adress 3
   if ((organization.address3 = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // adress 4
   if ((organization.address4 = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // title    
   if ((organization.title = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // title text
   if ((organization.title_text = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // revision number
   if ((organization.rev_number = STRDUP(line)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // sheet number 
   if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;  // sheet number
   if ((organization.sheet_number = STRDUP(lp)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;  // sheet total
   if ((organization.sheet_total = STRDUP(lp)) == NULL)
      MemErrorMessage(__FILE__, __LINE__);

   if (!get_nextline()) return FALSE;   // END

   return TRUE;
}

/****************************************************************************/
/*
   from start to endcompoent
*/
static int do_component()
{
   char  *lp;
   int   logcompcnt, partcnt, cnt, i;
   double   xsize, ysize;

   // here clear pinlist
   memset(pinlist,'\0',MAX_PINS * sizeof(Pinlist));

   compcnt++;
   if (!get_nextline()) return FALSE;
   if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;  // number of parts in comp.
   partcnt = atoi(lp);
   if (!get_nextline()) return FALSE;
   if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;  // number of logical comps
   logcompcnt = atoi(lp);
   if (!get_nextline()) return FALSE;  // library name
   if (!get_nextline()) return FALSE;  // description
   if (!get_nextline()) return FALSE;  // footprint 1
   if (!get_nextline()) return FALSE;  // footprint 2
   if (!get_nextline()) return FALSE;  // footprint 3
   if (!get_nextline()) return FALSE;  // footprint 4

   if (!get_nextline()) return FALSE;  // library part 1
   if (!get_nextline()) return FALSE;  // library part 2
   if (!get_nextline()) return FALSE;  // library part 3
   if (!get_nextline()) return FALSE;  // library part 4
   if (!get_nextline()) return FALSE;  // library part 5
   if (!get_nextline()) return FALSE;  // library part 6
   if (!get_nextline()) return FALSE;  // library part 7
   if (!get_nextline()) return FALSE;  // library part 8

   if (!get_nextline()) return FALSE;  // default designator
   if (!get_nextline()) return FALSE;  // sheet part file name

   for (i=0;i<logcompcnt;i++)
   {
      char  tmp[80];

      if (!get_nextline()) return FALSE;  // list of logical component
      if ((lp = strtok(line,"'")) == NULL)   return FALSE;
      CpyStr(tmp,lp,80);
      clean_blank(tmp);
   }

   // here must be part
   for (i=0;i<partcnt;i++)
   {
      if (!get_nextline()) return FALSE;  // sheet part file name
      if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;
      if (STRCMPI(lp,"Part")) // if NOT big problem
      {
         return FALSE;
      }
      if (!get_nextline()) return FALSE;  // sheet part file name
      if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;
      xsize = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ysize = cnv_tok(lp);

      if (libpartcnt < MAX_PARTS)
      {
         libpart[libpartcnt].partcnt = partcnt;
         libpart[libpartcnt].componentid = compcnt;
         libpart[libpartcnt].logcompid = i+1;
         libpartcnt++;
      }
      else
      {
         return FALSE;
      }

      // now graphic for normal part until EndNormalPart
      for (cnt=0;cnt<3;cnt++)
      {
         char  partname[80];
         int   graphcnt = 0;
         // here now make a block
         // name is libpartname, Normal, IEEE, Demorgan, Gate A..Z
         // compoenntcnt is partcnt variable
         if (!get_libpartname(partname,compcnt,i+1,cnt)) return FALSE;
         BlockStruct *b = Graph_Block_On(GBO_APPEND,partname,cur_filenum,0L);
         b->setBlockType(BLOCKTYPE_SYMBOL);

         // here needs libpart to be done

         pinlistcnt = 0;
         // here clear pinlist
         memset(pinlist,'\0',MAX_PINS * sizeof(Pinlist));

         while (get_nextline())
         {
            if ((lp = strtok(line," \t\n")) == NULL)  continue;

            if (!STRNICMP(lp,"End",3))
            {
               // here realloc pin array
               switch (cnt)
               {
                  case 0:
                     if (pinlistcnt)
                     {
                        // copy into libpart[libpartcnt-1].normalpinlist
                        libpart[libpartcnt-1].normalpinlistcnt = pinlistcnt;
                        if ((libpart[libpartcnt-1].normalpinlist = (Pinlist *)calloc(pinlistcnt, sizeof (Pinlist))) == NULL)
                           MemErrorMessage(__FILE__, __LINE__);
                        memcpy(libpart[libpartcnt-1].normalpinlist,pinlist,pinlistcnt * sizeof (Pinlist));
                     }
                  break;
                  case 1:
                     if (pinlistcnt)
                     {
                        // copy into libpart[libpartcnt-1].normalpinlist
                        libpart[libpartcnt-1].demorganpinlistcnt = pinlistcnt;
                        if ((libpart[libpartcnt-1].demorganpinlist = (Pinlist *)calloc(pinlistcnt, sizeof (Pinlist))) == NULL)
                           MemErrorMessage(__FILE__, __LINE__);
                        memcpy(libpart[libpartcnt-1].demorganpinlist,pinlist,pinlistcnt * sizeof (Pinlist));
                     }
                  break;
                  case 2:
                     if (pinlistcnt)
                     {
                        // copy into libpart[libpartcnt-1].normalpinlist
                        libpart[libpartcnt-1].ieeepinlistcnt = pinlistcnt;
                        if ((libpart[libpartcnt-1].ieeepinlist = (Pinlist *)calloc(pinlistcnt, sizeof (Pinlist))) == NULL)
                           MemErrorMessage(__FILE__, __LINE__);
                        memcpy(libpart[libpartcnt-1].ieeepinlist,pinlist,pinlistcnt * sizeof (Pinlist));
                     }
                  break;
               }
               break;
            }
            else
            if (!STRCMPI(lp,"Line"))
            {
               graphcnt++;
               do_line();
            }
            else
            if (!STRCMPI(lp,"Polyline"))
            {
               graphcnt++;
               do_polyline();
            }
            else
            if (!STRCMPI(lp,"Polygon"))
            {
               graphcnt++;
               do_polygon();
            }
            else
            if (!STRCMPI(lp,"Rectangle"))
            {
               graphcnt++;
               do_rectangle();
            }
            else
            if (!STRCMPI(lp,"Label"))
            {
               graphcnt++;
               do_label();
            }
            else
            if (!STRCMPI(lp,"Arc"))
            {
               graphcnt++;
               do_arc();
            }
            else
            if (!STRCMPI(lp,"EllipticalArc"))
            {
               graphcnt++;
               do_ellipticalarc();
            }
            else
            if (!STRCMPI(lp,"Ellipse"))
            {
               graphcnt++;
               do_ellipse();
            }
            else
            if (!STRCMPI(lp,"Pin"))
            {
               if (pinlistcnt < MAX_PINS)
               {
                  do_pin(&pinlist[pinlistcnt]);
                  pinlistcnt++;
               }
               else
               {
                  return FALSE;
               }
            }
            else
            {
               fprintf(ferr, " Unknown Graphic [%s] at %ld\n",lp,linecnt);
               display_error++;
            }
         }

         if (!graphcnt)
         {
            // make a box of xsize,ysize;
            int   layernum = Graph_Level("BOX","", 0);
            int   widthcode = 0;
            Graph_PolyStruct(layernum,  0L, FALSE);
            Graph_Poly(NULL,w[widthcode], 0,0,1);
            Graph_Vertex(0.0, 0.0, 0.0);
            Graph_Vertex(xsize, 0.0, 0.0);
            Graph_Vertex(xsize, ysize, 0.0);
            Graph_Vertex(0.0, ysize, 0.0);
            Graph_Vertex(0.0, 0.0, 0.0);
         }

         Graph_Block_Off();
      }  // for all parts NORMAL, DEMORGAN, IEEE
   }

   if (!get_nextline()) return FALSE;  //
   if ((lp = strtok(line," \t\n")) == NULL)  return FALSE;

   if (STRCMPI(lp,"EndComponent")) // if NOT big problem
   {
      return FALSE;
   }

   return TRUE;
}

/****************************************************************************/
/*
   a component NAND Gate can have multple
   partnames 7400 74LS00 etc. but all the same shape. therefor the
   partname is not taken in considuration. Only which component and gate and diplay
*/
static int get_libpartname(char *pname,int partcnt, int gatecnt, int display)
{
   sprintf(pname,"%d_%d_%d",partcnt, gatecnt, display);
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_schematic()
{
   char  *lp;

   while (get_nextline())
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,"[Font_Table]"))
      {
         do_fonttable();
      }
      else
      if (!STRCMPI(lp,"Library"))
      {
         if (do_library() == FALSE)
            return FALSE;
      }
      else
      if (!STRCMPI(lp,"Organization"))
      {
         if (do_organization() == FALSE)
            return FALSE;
      }
      else
      if (!STRCMPI(lp,"Future"))
      {
         cur_status = STAT_SHEET;
         do_future();
         cur_status = STAT_UNKNOWN;
      }
      else
      if (!STRCMPI(lp,"Sheet"))
      {
         char  *lp;

         get_nextline();
         if ((lp = strtok(line," \t\n")) == NULL)  break;   // border style
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // sheetstyle old
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // work space
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // show border
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // show title
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // border color
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // sheet color
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // snap grid
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // snap size
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // vis grid
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // vis grid size
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // x custom
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // y custom
         if ((lp = strtok(NULL," \t\n")) == NULL)  break;   // use custom
         if (atoi(lp) == 0)
            draw_sheet(cur_sheetstyle);
         cur_status = STAT_SHEET;
         do_sheet();
         cur_status = STAT_UNKNOWN;
         break;   // this is all done;
      }
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_future()
{
   char  *lp;

   // grid enable
   if ((lp = strtok(NULL," \t\n")) == NULL)  return 0;

   // gridsize
   if ((lp = strtok(NULL," \t\n")) == NULL)  return 0;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return 0;
   cur_sheetstyle = atoi(lp);

   do
   {
      get_nextline(); // end future statement
      lp = strtok(line," \t\n");
   }while (STRICMP(lp,"EndFuture"));

   return 1;
}

/****************************************************************************/
/*
*/
static int draw_sheet(int sheetstyle)
{
   int   layernum = Graph_Level("BORDER","",0);
   double x1, y1, x2, y2;

   switch (sheetstyle)
   {
      case 0:  // A4
      break;
      case 1:  // A3
      break;
      case 2:  // A2
      break;   
      case 3:  // A1
      break;
      case 4:  // A0
      break;
      case 5:  // A
         x1 = 0;
         y1 = 0;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL, w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*730;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL, w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*370;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*370;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*190;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*190;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
         
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*190;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*190;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*370;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*370;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*560;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*560;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*730;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*730;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*730;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*560;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*560;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      break;
      case 6:  // B 1500 X 950         
         x1 = 0;
         y1 = 0;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL,  w[0],0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1480;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL, w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*250;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*250;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1250;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1250;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*950;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1480;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1480;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1480;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1250;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1250;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*250;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*250;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*240;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*470;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*20;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*710;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
      break;
      case 7:  // C
         x1 = 0;
         y1 = 0;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL, w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         Graph_PolyStruct(layernum, 0L, FALSE);
         Graph_Poly(NULL, w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1120;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1120;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*380;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*380;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1660;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1660;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1330;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1330;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*670;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*670;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*330;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*330;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*380;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*380;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*750;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1120;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1120;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*330;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*330;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*660;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*660;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
         
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1330;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1330;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1660;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1470;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1660;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

      break;
      case 8:  // D
         x1 = 0;
         y1 = 0;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_PolyStruct(layernum,  0L, FALSE);
         Graph_Poly(NULL,w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3170;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         Graph_PolyStruct(layernum,  0L, FALSE);
         Graph_Poly(NULL,w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3170;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3170;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3170;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2800;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2800;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
         
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1200;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*80;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*400;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*400;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*30;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1500;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*400;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*400;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1200;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2800;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1970;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2800;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2000;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

      break;
      case 9:  // E
         x1 = 0;
         y1 = 0;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*4200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_PolyStruct(layernum,  0L, FALSE);
         Graph_Poly(NULL,w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*4160;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         Graph_PolyStruct(layernum,  0L, FALSE);
         Graph_Poly(NULL,w[0], 0,0,1);
         Graph_Vertex(x1, y1, 0.0);
         Graph_Vertex(x2, y1, 0.0);
         Graph_Vertex(x2, y2, 0.0);
         Graph_Vertex(x1, y2, 0.0);
         Graph_Vertex(x1, y1, 0.0);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*4160;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*4200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*4160;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*4200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*4160;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*4200;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3930;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3930;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3670;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3670;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3410;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3410;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3140;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3140;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2880;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2880;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2620;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2620;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2360;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2360;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2100;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2100;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1830;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1830;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1570;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1570;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1310;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1310;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1050;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1050;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*790;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*790;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*520;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*520;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*260;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*260;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*800;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*1600;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*40;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*0;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*2400;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*260;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*260;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*520;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*520;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*780;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*780;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1050;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1050;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1310;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1310;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1570;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1570;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*1830;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*1830;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2100;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2100;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2360;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2360;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2620;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2620;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      
         x1 =  Units_Factor(UNIT_MILS, pageUnits)*2880;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*2880;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3140;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3140;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3410;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3410;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3670;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3670;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);

         x1 =  Units_Factor(UNIT_MILS, pageUnits)*3930;
         y1 =  Units_Factor(UNIT_MILS, pageUnits)*3160;
         x2 =  Units_Factor(UNIT_MILS, pageUnits)*3930;
         y2 =  Units_Factor(UNIT_MILS, pageUnits)*3200;
         Graph_Line(layernum,x1,y1,x2,y2, 0,w[0], FALSE);
      break;
      case 10: // Letter
      break;
      case 11: // Legal
      break;
      case 12: // tabloid
      break;
      case 13: // OrcadA
      break;
      case 14: // OrcadB
      break;
      case 15: // OrcadC
      break;
      case 16: // OrcadD
      break;
      case 17: // OrcadE
      break;
   }
   return 1;
}  

/****************************************************************************/
/*
*/
static int do_sheet()
{
   char  *lp;
   int    cur_partnr;
   int    cur_libpartptr;
   int    show_hidden_pins;
   DataStruct  *lastref = NULL;
   int    desccnt; // counter for description field

   while (get_nextline())
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,"EndSheet"))
      {
         break;
      }
      else
      if (!STRCMPI(lp,"Port"))
      {
         do_port();
      }
      else
      if (!STRCMPI(lp,"PowerObject"))
      {
        do_powerobject();
      }
      else
      if (!STRCMPI(lp,"Part"))
      {
         double x,y;
         int    rot;
         int    display,mirror;
         char   partname[80];
         int    compnr;

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         x = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //y
         y = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;    // display mode
         display = atoi(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // mirror
         mirror = atoi(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // rotation
         rot = atoi(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // select
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // compnr
         cur_partnr = atoi(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // partnr
         compnr = atoi(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // show hidden fields
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // show hidden pins
         show_hidden_pins = atoi(lp);
         get_libpartname(partname,compnr+1,cur_partnr,display);
         cur_libpartptr = find_libpart(compnr+1,cur_partnr);

         int   comprot = rot; // do not modify rot for place pins.
         if (mirror)
         {
            switch (comprot)
            {
               case 1:
                  comprot = 3;
               break;
               case 3:
                  comprot = 1;
               break;
               default:
               break;
            }
         }
         CString refname;
         refname.Format("$I%d", compnr);
         lastref = Graph_Block_Reference(partname, refname,  cur_filenum,x, y,DegToRad(90*comprot),mirror,
                  1.0, -1, FALSE);
         lastref->getInsert()->setInsertType(insertTypeSymbol);
         desccnt = 0;
         // here now place all pins
         place_pins(x,y,rot,mirror,compnr+1,cur_partnr,display,show_hidden_pins);
      }
      else
      if (!STRCMPI(lp,"Designator"))
      {
         char  prosa[255];
         double x,y;
         double rot;
         int   fontid,hidden, layernum;

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         x = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //y
         y = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         rot = atoi(lp) * 90;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         if (layer_by_color)
            layernum = Graph_Level(lp,"", 0);
         else
            layernum = Graph_Level("DESIGNATOR","", 0);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // fontid
         fontid = atoi(lp) - 1;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // select
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // hidden
         hidden = atoi(lp);

         if ((lp = strtok(NULL,"'")) == NULL)   continue;      // prosa
         strcpy(prosa,lp);

         if (strlen(prosa) && libpart[cur_libpartptr].partcnt > 1 && cur_partnr > 0)
         {
            char  t[80];   // need gate letter
            if (cur_partnr < 'Z'-'A')
            {
               sprintf(t,"%c",'A' + cur_partnr - 1);
               strcat(prosa,t);
            }              
         }
         if (strlen(prosa) && fontid > -1 && !hidden)
         {
            Graph_Text(layernum, cnv_text(prosa), x, y, 
               font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
               0, 0, FALSE, w[0], 1); 
         }

         // update the reference name, needed for search
         if (lastref)
         {
            x = x - lastref->getInsert()->getOriginX();
            y = y - lastref->getInsert()->getOriginY();
            doc->SetUnknownAttrib(&lastref->getAttributesRef(), "DESIGNATOR", prosa,
               SA_OVERWRITE, NULL); //
         }
         
      }
      else
      if (!STRCMPI(lp,"PartType"))
      {
         char  prosa[255];
         double x,y;
         double rot;
         int   fontid,hidden, layernum;

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         x = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //y
         y = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         rot = atoi(lp) * 90;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         if (layer_by_color)
            layernum = Graph_Level(lp,"", 0);
         else
            layernum = Graph_Level("PARTTYPE","", 0);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // fontid
         fontid = atoi(lp) - 1;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // select
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // hidden
         hidden = atoi(lp);

         if ((lp = strtok(NULL,"'")) == NULL)   continue;      // prosa
         strcpy(prosa,lp);
         if (strlen(prosa) && fontid > -1 && !hidden)
         {
            Graph_Text(layernum, cnv_text(prosa), x, y, 
               font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
               0, 0, FALSE, w[0],1); 
         }

         // update the reference name, needed for search
         if (lastref && strlen(prosa))
         {
            x = x - lastref->getInsert()->getOriginX();
            y = y - lastref->getInsert()->getOriginY();

            doc->SetUnknownAttrib(&lastref->getAttributesRef(), "PARTTYPE", prosa,
                SA_OVERWRITE, NULL); //
         }


      }
      else
      if (!STRCMPI(lp,"PartDescription"))
      {
         char  prosa[255];
         double x,y;
         double rot;
         int   fontid,hidden, layernum;
         desccnt++;

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         x = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //y
         y = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         rot = atoi(lp) * 90;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
         if (layer_by_color)
            layernum = Graph_Level(lp,"", 0);
         else
            layernum = Graph_Level("PARTDESCRIPTION","", 0);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // fontid
         fontid = atoi(lp) - 1;
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // select
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // hidden
         hidden = atoi(lp);

         if ((lp = strtok(NULL,"'")) == NULL)   continue;      // prosa
         strcpy(prosa,lp);

         if (STRCMPI(prosa,"*")) 
         {
            if (strlen(prosa) && fontid > -1 && !hidden)
            {
               Graph_Text(layernum, cnv_text(prosa), x, y, 
                  font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
                  0, 0, FALSE, w[0],1); 
            }

            // update the reference name, needed for search
            if (lastref)
            {
               x = x - lastref->getInsert()->getOriginX();
               y = y - lastref->getInsert()->getOriginY();
               CString desc;
               desc.Format("DESC_%d", desccnt);

               doc->SetUnknownAttrib(&lastref->getAttributesRef(), desc, prosa, SA_APPEND, NULL); // x, y, rot, height
            }
         } // prosa is *
      }
      else
      if (!STRCMPI(lp,"SheetPartFileName"))
      {
         // nothing
      }
      else
      if (!STRCMPI(lp,"TextFrame"))
      {
         do_textframe();
      }
      else
      if (!STRCMPI(lp,"Template"))
      {
         do_template();
      }
      else
      if (!STRCMPI(lp,"SheetSymbol"))
      {
         do_sheetsymbol();
      }
      else
      if (!STRCMPI(lp,"Label"))
      {
         do_label();
      }
      else
      if (!STRCMPI(lp,"NetLabel"))
      {
         do_netlabel();
      }
      else
      if (!STRCMPI(lp,"Junction"))
      {
         double x, y;
         char  name[20];
         int   layernum;

         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         x = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //x
         y = cnv_tok(lp);
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   //size
         sprintf(name,"JUNC_%d",atoi(lp));
         if ((lp = strtok(NULL," \t\n")) == NULL)  continue;   // color
         if (layer_by_color)
            layernum = Graph_Level(lp,"", 0);
         else
            layernum = Graph_Level("JUNCTION","", 0);

         Graph_Block_Reference(name, NULL, cur_filenum, x, y,
                     0.0, 0, 1.0, layernum, TRUE);

         PFWSchemJunction  *c = new PFWSchemJunction;
         c->x = x;
         c->y = y;
         junctionarray.SetAtGrow(junctioncnt,c); 
         junctioncnt++;
      }
      else
      if (!STRCMPI(lp,"Bus"))
      {
         do_bus();
      }
      else
      if (!STRCMPI(lp,"BusEntry"))
      {
         do_busentry();
      }
      else
      if (!STRCMPI(lp,"Wire"))
      {
         do_wire();
      }
      else
      if (!STRCMPI(lp,"Arc"))
      {
         do_arc();
      }
      else
      if (!STRCMPI(lp,"EllipticalArc"))
      {
         do_ellipticalarc();
      }
      else
      if (!STRCMPI(lp,"Ellipse"))
      {
         do_ellipse();
      }
      else
      if (!STRCMPI(lp,"Polyline"))
      {
         do_polyline();
      }
      else
      if (!STRCMPI(lp,"Polygon"))
      {
         do_polygon();
      }
      else
      if (!STRCMPI(lp,"Line"))
      {
         do_line();
      }
      else
      {
         fprintf(ferr, " Unknown Graphic [%s] at %ld\n",lp,linecnt);
         display_error++;
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int do_port()
{
   char  *lp;
   char  prosa[255];
   double x,y, width;
   int   layernum;
   int   arrowstyle;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // arrow style
   arrowstyle = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // input output
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // text align
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // width
   width  = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // ???

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // layer
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("PORT","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // text color

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   if (strlen(prosa))
   {
      double leftx,rightx;
      // draw left end
      if (arrowstyle & 1)
      {
         Graph_Line(layernum,x,y,
                 x+font[0].height/2,y+font[0].height/2, 0,w[0], FALSE);
         Graph_Line(layernum,x,y,
                 x+font[0].height/2,y-font[0].height/2, 0,w[0], FALSE);
         leftx = font[0].height/2;
      }
      else
      {
         Graph_Line(layernum,x,y-font[0].height/2,
                 x,y+font[0].height/2, 0,w[0], FALSE);
         leftx = 0.0;
      }
      // draw right end
      if (arrowstyle & 2)
      {
         Graph_Line(layernum,x+width,y,
              x+width-font[0].height/2,y+font[0].height/2, 0,w[0], FALSE);
         Graph_Line(layernum,x+width,y,
                 x+width-font[0].height/2,y-font[0].height/2, 0,w[0], FALSE);
         rightx = font[0].height/2;
      }
      else
      {
         Graph_Line(layernum,x+width,y-font[0].height/2,
                 x+width,y+font[0].height/2, 0,w[0], FALSE);
         rightx = 0.0;
      }

      // draw upper line
      Graph_Line(layernum,x+leftx,y+font[0].height/2,
              x + width - rightx,y+font[0].height/2, 0,w[0], FALSE);
      // draw lower line
      Graph_Line(layernum,x+leftx,y-font[0].height/2,
              x + width - rightx,y-font[0].height/2, 0,w[0], FALSE);

      x = x + font[0].height;
      y = y - (FONT_CORRECT * font[0].height/2);
      Graph_Text(layernum, cnv_text(prosa), x, y, 
         font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0, TRUE,
         0, 0, FALSE, w[0],1);
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_powerobject()
{
   char  *lp;
   char  prosa[255];
   double x, y;
   int   layernum;
   int   powerstyle;
   int   rot = 0;
   double offsetx, offsety;

   
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // power style
   powerstyle = atoi(lp);
   
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // rotation
   rot = atoi(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // Color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("POWEROBJECT","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // selected

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   // power Object
   Graph_Block_Reference(powersymbol[powerstyle], NULL,  cur_filenum,x, y,DegToRad(90*rot),0,
                  1.0, layernum, FALSE);

   if (strlen(prosa) && powerstyle < 4)   // 4 5 6 do not get text
   {

      switch (rot)
      {
         case 0:
            offsetx = Units_Factor(UNIT_MILS, pageUnits)*100;
            offsety = -font[0].height*FONT_CORRECT/2;
         break;
         case 1:
            offsetx = -(1.0*strlen(prosa)*font[0].height*FONT_CORRECT)/2;
            offsety = Units_Factor(UNIT_MILS, pageUnits)*100 +
               (0.2*font[0].height*FONT_CORRECT);
         break;
         case 2:
            offsetx = Units_Factor(UNIT_MILS, pageUnits)*(-100);
            offsety = font[0].height*FONT_CORRECT/2;
         break;
         case 3:
            offsetx = -(1.0*strlen(prosa)*font[0].height*FONT_CORRECT)/2;
            offsety = Units_Factor(UNIT_MILS, pageUnits)*(-100) - 
               (1.2*font[0].height*FONT_CORRECT);
         break;
      }
      Graph_Text(layernum, cnv_text(prosa), x+offsetx, y+offsety, 
         font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0, TRUE,
         0, 0, FALSE, w[0],1);
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_sheetnet()
{
   char  *lp;
   char  prosa[255];
   double x,y;
   int   layernum;
   int   arrowstyle;
   int   position;
   double   distance;
   double   width = cnv_tok("20");

   x = cur_sheetx;
   y = cur_sheety;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // input output
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // arrow style
   arrowstyle = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // position
   position = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // distance from top
   sprintf(prosa,"%d",atoi(lp) * 10);
   distance  = cnv_tok(prosa);

   switch (position)
   {
      case 0:  // left
         y = y - distance;
      break;
      case 2:  // bottom   : this is wrong in the manual
         y = y - cur_sheetsizey;
         x = x + distance;
      break;
      case 1:  // right    : this is wrong in the manual
         x = x + cur_sheetsizex - width;
         y = y - distance;
      break;
      case 3:  // top;
         x = x + distance;
      break;
   }

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // Border color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("SHEETNET","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // text color
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   if (strlen(prosa))
   {
      double leftx,rightx;
      // draw left end
      if (arrowstyle & 1)
      {
         Graph_Line(layernum,x,y,
                 x+font[0].height/2,y+font[0].height/2, 0,w[0], FALSE);
         Graph_Line(layernum,x,y,
                 x+font[0].height/2,y-font[0].height/2, 0,w[0], FALSE);
         leftx = font[0].height/2;
      }
      else
      {
         Graph_Line(layernum,x,y-font[0].height/2,
                 x,y+font[0].height/2, 0,w[0], FALSE);
         leftx = 0.0;
      }
      // draw right end
      if (arrowstyle & 2)
      {
         Graph_Line(layernum,x+width,y,
              x+width-font[0].height/2,y+font[0].height/2, 0,w[0], FALSE);
         Graph_Line(layernum,x+width,y,
                 x+width-font[0].height/2,y-font[0].height/2, 0,w[0], FALSE);
         rightx = font[0].height/2;
      }
      else
      {
         Graph_Line(layernum,x+width,y-font[0].height/2,
                 x+width,y+font[0].height/2, 0,w[0], FALSE);
         rightx = 0.0;
      }

      // draw upper line
      Graph_Line(layernum,x+leftx,y+font[0].height/2,
              x + width - rightx,y+font[0].height/2, 0,w[0], FALSE);
      // draw lower line
      Graph_Line(layernum,x+leftx,y-font[0].height/2,
              x + width - rightx,y-font[0].height/2, 0,w[0], FALSE);

      // here need to watch out for position top, etc...              

      switch (position)
      {
         case 0:  // left
            x = x + font[0].height + width;
            y = y - (FONT_CORRECT * font[0].height/2);
         break;
         case 2:  // bottom   : this is wrong in the manual : not tested
            x = x + font[0].height + width;
            y = y - (FONT_CORRECT * font[0].height/2);
         break;
         case 1:  // right    : this is wrong in the manual
            //x = x + font[0].height + width;
            y = y - (FONT_CORRECT * font[0].height/2);
            x = x - (FONT_CORRECT*font[0].height*strlen(prosa)) ;
         break;
         case 3:  // top; not tested
            x = x + font[0].height + width;
            y = y - (FONT_CORRECT * font[0].height/2);
         break;
      }

      Graph_Text(layernum, cnv_text(prosa), x, y, 
         font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0, TRUE,
         0, 0, FALSE, w[0],1);
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_label()
{
   CString  prosa;
   double x,y,rot;
   int   fontid,layernum;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);

   // this is funny text
   if (cur_status == STAT_SHEET && x < 0)   return FALSE;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);

   // this is funny text
   if (cur_status == STAT_SHEET && y < 0)   return FALSE;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   rot = atoi(lp) * 90;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("LABEL","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fontid
   fontid = atoi(lp) - 1;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   prosa = lp;

   // here check for restricted words
   if (!STRICMP(prosa,".title"))
       prosa = organization.title;
   else
   if (!STRICMP(prosa,".sheetnumber"))
       prosa = organization.sheet_number;
   else   
   if (!STRICMP(prosa,".address1"))
       prosa = organization.address1;
   else                      
   if (!STRICMP(prosa,".address2"))
       prosa = organization.address2;
   else   
   if (!STRICMP(prosa,".address3"))
       prosa = organization.address3;
   else   
   if (!STRICMP(prosa,".address4"))
       prosa = organization.address4;
   else   
   if (!STRICMP(prosa,".documentnumber"))
       prosa = organization.title_text;
   else   
   if (!STRICMP(prosa,".organization"))
       prosa = organization.organization;
   else   
   if (!STRICMP(prosa,".revision"))
       prosa = organization.rev_number;
   else
   if (!STRICMP(prosa,".sheettotal"))
       prosa = organization.sheet_total;
        
   prosa.TrimLeft();
   prosa.TrimRight();

   if (strlen(prosa) && fontid > -1)
   {
      Graph_Text(layernum, cnv_text(prosa), x, y, 
         font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, 
         TRUE, 0, 0, FALSE, w[0],1); // this is smallwidth index
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_netlabel()
{
   char  *lp;
   char  prosa[255];
   double x,y,rot;
   int   fontid,layernum;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   rot = atoi(lp) * 90;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("SHEETLABEL","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fontid
   fontid = atoi(lp) - 1;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   if (strlen(prosa) && fontid > -1)
   {
      Graph_Text(layernum, cnv_text(prosa), x, y,
         font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
         0, 0, FALSE, w[0],1);
   }

   if (strlen(prosa))
   {
      PFWSchemNetlabel  *c = new PFWSchemNetlabel;
      c->netname = prosa;
      c->x = x;
      c->y = y;
      netlabelarray.SetAtGrow(netlabelcnt,c); 
      netlabelcnt++;
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_sheetname()
{
   char  *lp;
   char  prosa[255];
   double x,y,rot;
   int   fontid,layernum, hidden;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   rot = atoi(lp) * 90;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("SHEETNAME","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fontid
   fontid = atoi(lp) - 1;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   hidden = atoi(lp);

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   if (strlen(prosa) && fontid > -1 && !hidden)
   {
      Graph_Text(layernum, cnv_text(prosa), x, y,
         font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
         0, 0, FALSE, w[0],1);
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_sheetfilename()
{
   char  *lp;
   char  prosa[255];
   double x,y,rot;
   int   fontid,layernum, hidden;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //x
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  //y
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   rot = atoi(lp) * 90;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("SHEETNAME","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fontid
   fontid = atoi(lp) - 1;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // hidden
   hidden = atoi(lp);

   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;     // prosa
   strcpy(prosa,lp);

   if (strlen(prosa) && fontid > -1 && !hidden)
   {
      Graph_Text(layernum, cnv_text(prosa), x, y, 
         font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, DegToRad(rot), 0, TRUE,
         0, 0, FALSE, w[0],1);
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_wire()
{
   int   widthcode, vcnt, i;
   int   layernum;
   char  *lp;

   vertexid++;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("WIRE","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // number of vertex
   vcnt = atoi(lp);

   ppolycnt = 0;
   for (i=0;i<vcnt;i++)
   {
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].x = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].y = cnv_tok(lp);
   }

   DataStruct *d = Graph_PolyStruct(layernum,  0L, FALSE);
   Graph_Poly(NULL,w[widthcode], 0,0,0);

   for (i=0;i<vcnt;i++)
   {
      Graph_Vertex(ppoly[i].x,ppoly[i].y,0.0);

      PFWSchemVertex *c = new PFWSchemVertex;
      c->d = d;
      c->x = ppoly[i].x;
      c->y = ppoly[i].y;
      c->netname_assigned = FALSE;
      c->vertexid = vertexid;
      if (i+1 == vcnt)
         c->last = TRUE;
      vertexarray.SetAtGrow(vertexcnt,c); 
      vertexcnt++;
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_line()
{
   int   widthcode;
   int   layernum;
   double x1,y1,x2,y2;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // style

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("LINE","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   Graph_Line(layernum,x1,y1,x2,y2, 0,w[widthcode], FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_busentry()
{
   int   widthcode;
   int   layernum;
   double x1,y1,x2,y2;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("BUSENTRY","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   Graph_Line(layernum,x1,y1,x2,y2, 0,w[widthcode], FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_arc()
{
   int   widthcode;
   int   layernum;
   double x,y,rad;
   char  *lp;
   double startangle, endangle;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   rad = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // start
   startangle = atof(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // start
   endangle = atof(lp);

   if (startangle > endangle) endangle += 360;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("ARC","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   Graph_Arc(layernum,x,y,rad,DegToRad(startangle),
            DegToRad(endangle - startangle),0,w[widthcode],FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_ellipticalarc()
{
   int   widthcode;
   int   layernum;
   double x,y,rad;
   char  *lp;
   double startangle, endangle;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // first radius
   rad = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // secord radius

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // start
   startangle = atof(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // start
   endangle = atof(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("ARC","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if (startangle > endangle) endangle += 360;

   if (fabs(startangle - endangle) < 1)
      Graph_Circle(layernum,x,y,rad,0,w[widthcode],0, FALSE);
   else
      Graph_Arc(layernum,x,y,rad,DegToRad(startangle),
            DegToRad(endangle - startangle),0,w[widthcode],0);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_ellipse()
{
   int   widthcode;
   int   layernum;
   double x,y,rad;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // first radius
   rad = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // secord radius

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // layer color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("ARC","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // solid
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   Graph_Circle(layernum,x,y,rad,0,w[widthcode],0, FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_polyline()
{
   int   widthcode, vertexcnt, i;
   int   layernum;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // style

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("POLYLINE","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // number of vertex
   vertexcnt = atoi(lp);

   ppolycnt = 0;
   for (i=0;i<vertexcnt;i++)
   {
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].x = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].y = cnv_tok(lp);
   }

   if (vertexcnt > 2)
   {
      Graph_PolyStruct(layernum,  0L, FALSE);
      Graph_Poly(NULL,w[widthcode], 0,0,0);
      for (i=0;i<vertexcnt;i++)
         Graph_Vertex(ppoly[i].x,ppoly[i].y,0.0);
   }
   else
      Graph_Line(layernum,ppoly[0].x,ppoly[0].y,ppoly[1].x, ppoly[1].y, 0,w[widthcode], FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_bus()
{
   int   widthcode, vertexcnt, i;
   int   layernum;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("BUS","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // number of vertex
   vertexcnt = atoi(lp);

   ppolycnt = 0;
   for (i=0;i<vertexcnt;i++)
   {
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].x = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].y = cnv_tok(lp);
   }

   Graph_PolyStruct(layernum,  0L, FALSE);
   Graph_Poly(NULL,bus[widthcode], 0,0,0);
   for (i=0;i<vertexcnt;i++)
      Graph_Vertex(ppoly[i].x,ppoly[i].y,0.0);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_polygon()
{
   int   widthcode, vertexcnt, i;
   int   layernum, filled;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("POLYGON","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // solid
   filled = atoi(lp);
   filled = 0;
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;   // number of vertex
   vertexcnt = atoi(lp);

   ppolycnt = 0;
   for (i=0;i<vertexcnt;i++)
   {
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].x = cnv_tok(lp);
      if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
      ppoly[i].y = cnv_tok(lp);
   }

   // here close polygon
   ppoly[vertexcnt].x = ppoly[0].x;
   ppoly[vertexcnt].y = ppoly[0].y;
   vertexcnt++;

   if (vertexcnt > 2)
   {
      Graph_PolyStruct(layernum, 0L, FALSE);
      Graph_Poly(NULL, w[widthcode], 0,0,0);
      for (i=0;i<vertexcnt;i++)
         Graph_Vertex(ppoly[i].x,ppoly[i].y,0.0);
   }
   else
      Graph_Line(layernum,ppoly[0].x,ppoly[0].y,ppoly[1].x, ppoly[1].y, 0,w[widthcode], FALSE);

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_rectangle()
{
   double x1,y1,x2,y2;
   int   widthcode;
   int   layernum, filled;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border width
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("POLYGON","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // solid
   filled = atoi(lp);
   filled = 0;
   Graph_PolyStruct(layernum, 0, FALSE);
   Graph_Poly(NULL,  w[widthcode],filled, 0,1);
   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x1,y2,0.0);
   Graph_Vertex(x1,y1,0.0);

   return TRUE;
}

/****************************************************************************/
/*
   end of text if 0x0C
*/
static int do_textframe()
{
   int   end_of_text = FALSE;
   double x1,y1,x2,y2;
   int   widthcode;
   int   layernum, fontid;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // start
   x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // end
   x2 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y2 = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border width
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("POLYGON","", 0);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // text color

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fontid
   fontid = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // 1= solid, 0 = not solid
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // Aligment 0 center, 1 = left, 2 = right
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // wordwrap
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // clip
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   Graph_PolyStruct(layernum, 0, FALSE);
   Graph_Poly(NULL,  w[widthcode], 0, 0,1);
   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x1,y2,0.0);
   Graph_Vertex(x1,y1,0.0);

   // loop from
   if (!get_nextline()) return FALSE;

   double   x,y;

   x = x1 + font[fontid].height/2;  // start little inside
   y = y2 - font[fontid].height/2;  // starts half a line down.

   // Begin
   while (!end_of_text)
   {
      if (!get_nextline()) return FALSE;
      line[strlen(line)-1] = '\0';  // kill /n lineend
      if (strlen(line) && line[strlen(line)-1] == 0xC)
      {
         end_of_text = TRUE;
         line[strlen(line)-1] = '\0';
      }
      y = y - font[fontid].height;
      if (strlen(line) && fontid > -1)
      {
         Graph_Text(layernum, line, x, y, 
            font[fontid].height*FONT_CORRECT, font[fontid].width*HEIGHT_RATIO, 0.0, 0, TRUE,
            0, 0, FALSE, w[0], 1); 
      }
   }
   // End
   if (!get_nextline()) return FALSE;

   return TRUE;
}

/****************************************************************************/
/*
   end of text if 0x0C
*/
static int do_template()
{
   char  *lp;
   int   end_of_template = FALSE;

   // Begin
   while (!end_of_template)
   {
      if (!get_nextline()) return FALSE;
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,"EndTemplate"))
      {
         end_of_template = TRUE;
      }
      else
      if (!STRCMPI(lp,"Polyline"))
      {
         do_polyline();
      }
      else
      if (!STRCMPI(lp,"Label"))
      {
         do_label();
      }
      else
      if (!STRCMPI(lp,"TextFrame"))
      {
         do_textframe();
      }
      else
      {
         fprintf(ferr, " Unknown Graphic in Template [%s] at %ld\n",lp,linecnt);
         display_error++;
      }
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_sheetsymbol()
{
   double x1,y1,x2,y2;
   int   widthcode;
   int   layernum, filled;
   char  *lp;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   cur_sheetx = x1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   cur_sheety = y1 = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   cur_sheetsizex = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   cur_sheetsizey = cnv_tok(lp);

   x2 = x1 + cur_sheetsizex;
   y2 = y1 - cur_sheetsizey;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border width
   widthcode = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // border color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("SHEETSYMBOL","", 0);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // fill color

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // select

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // solid
   filled = atoi(lp);
   filled = 0;

   Graph_PolyStruct(layernum,  0, FALSE);
   Graph_Poly(NULL, w[widthcode], filled, 0,1);
   Graph_Vertex(x1,y1,0.0);
   Graph_Vertex(x2,y1,0.0);
   Graph_Vertex(x2,y2,0.0);
   Graph_Vertex(x1,y2,0.0);
   Graph_Vertex(x1,y1,0.0);

   while (get_nextline())
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,"EndSheetSymbol"))
      {
         break;
      }
      else
      if (!STRCMPI(lp,"SheetName"))
      {
         do_sheetname();
      }
      else
      if (!STRCMPI(lp,"SheetFileName"))
      {
         do_sheetfilename();
      }
      else
      if (!STRCMPI(lp,"SheetNet"))
      {
         do_sheetnet();
      }
      else
      {
         fprintf(ferr, " Unknown Graphic [%s] at %ld\n",lp,linecnt);
         display_error++;
      }
   }

   return TRUE;
}

/****************************************************************************/
/*
   Pinb graphic only
*/
static int do_pin(Pinlist *p)
{
   int    layernum, hidden;
   char *lp;
   double x,y, len, xlen, ylen, xrad, yrad, rad;
   int    rot;
   int    widthcode = 0;
   char pinname[80],pinnumber[80];
   int    showpinname,showpinnumber;
   int    dot,clk;

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // dot type
   dot = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // clk type
   clk = atoi(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // electrical attribute
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // hidden
   hidden = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // show pinname
   showpinname = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // show pinnumber
   showpinnumber = atoi(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // pin length
   len = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   x = cnv_tok(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;
   y = cnv_tok(lp);

   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // pin rotation
   rot = atoi(lp);
   if ((lp = strtok(NULL," \t\n")) == NULL)  return FALSE;  // pin color
   if (layer_by_color)
      layernum = Graph_Level(lp,"", 0);
   else
      layernum = Graph_Level("PIN","", 0);
   if ((lp = strtok(NULL,"'")) == NULL)   return FALSE;  // pin name
   strcpy(pinname,lp);
   clean_blank(pinname);
   if ((lp = strtok(NULL," '")) == NULL)  return FALSE;  // pin number
   strcpy(pinnumber,lp);
   clean_blank(pinnumber);

   p->x = x;
   p->y = y;
   p->pinlength = len;
   p->rotation = rot;
   p->hidden   = hidden;
   p->showpinname = showpinname;
   p->showpinnumber = showpinnumber;
   p->layernum = layernum;

   if (strlen(pinname))
   {
      if ((p->pinname = STRDUP(pinname)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   }

   if (strlen(pinnumber))
   {
      if ((p->pinnumber = STRDUP(pinnumber)) == NULL)
         MemErrorMessage(__FILE__, __LINE__);
   }

   rad = Units_Factor(UNIT_MILS, pageUnits)*25;
   Rotate(len,0.0,90.0*rot,&xlen,&ylen);
   Rotate(rad,0.0,90.0*rot,&xrad,&yrad);

   // pin graphio
   if (!hidden)
   {
      if (dot)
      {
         Graph_Circle(layernum, x+xrad, y+yrad, rad, 0, w[widthcode], FALSE, FALSE);
         Graph_Line(layernum,x+xrad*2,y+yrad*2, x+xlen, y+ylen, 0, w[widthcode], FALSE);
      }
      else
         Graph_Line(layernum,x,y, x+xlen, y+ylen, 0, w[widthcode], FALSE);

      // pin name always left / center
#ifdef NO_PINNAME
      if (strlen(pinname))
      {
         double fx;
         int len= strlen_text(pinname);

         fx = (font[0].height - font[0].height*FONT_CORRECT)/2;
         layernum = Graph_Level("PINNAME","");
         switch (rot)
         {
            case 1:
               Graph_Text(layernum, cnv_text(pinname), x, y+len+fx,
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0,0L, 
                  1,0,0, FALSE,w[0],1);
            break;
            case 2:
               Graph_Text(layernum, cnv_text(pinname), x+fx, y-font[0].height*FONT_CORRECT/2, 0.0,
                   font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
            case 3:
               Graph_Text(layernum, cnv_text(pinname), x, y-len+fx, 0.0,
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
            default:
               Graph_Text(layernum, cnv_text(pinname), x-(l*font[0].height*HEIGHT_RATIO),
                  y-font[0].height*FONT_CORRECT/2, 0.0,
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,0,1);
            break;
         }
      }
      // pin number always middle / top
      if (strlen(pinnumber))
      {
         double fx;
         fx = (font[0].height - font[0].height*FONT_CORRECT)/2;
         layernum = Graph_Level("PINNUMBER","");
         switch(rot)
         {
            case 1:
               Graph_Text(layernum, cnv_text(pinnumber), x+fx, y+len/2,
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
            case 2:
               Graph_Text(layernum, cnv_text(pinnumber), x-len/2, y+fx,
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
            case 3:
               Graph_Text(layernum, cnv_text(pinnumber), x+fx, y-len/2, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
            default:
               Graph_Text(layernum, cnv_text(pinnumber), x+len/2, y+fx, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0L,
                  1,0,0, FALSE,w[0],1);
            break;
         }
      }
#endif
   }
   return TRUE;
}

/****************************************************************************/
/*
*/
static int do_fonttable()
{
   char  *lp;

   while (get_nextline())
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;

      if (!STRCMPI(lp,"EndFont"))
      {
         break;
      }
      else
      {
         int   i;
         int   fcnt = atoi(lp);

         for   (i=0;i<fcnt;i++)
         {
            if(!get_nextline())  break;
            if ((lp = strtok(line," \t\n")) == NULL)  continue;   // fontsize
            font[i].height = cnv_tok(lp); //* FONT_CORRECT; // fontsize is the complete box size
            font[i].width  = cnv_tok(lp); //* HEIGHT_RATIO;
         }
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int find_libpart(int compnr,int partnr)
{
   int   i;

   for (i=0;i<libpartcnt;i++)
   {
      if (libpart[i].componentid == compnr && libpart[i].logcompid == partnr)
         return i;
   }

   return -1;
}

/****************************************************************************/
/*
*/
static int place_pins(double cx,double cy,int crot,int mirror, int compnr,int partnr,int display,
         int show_hidden_pins)
{
   // find according libpart.
   Pinlist  *p = NULL;
   int      pcnt = 0;

   int   libpartptr = find_libpart(compnr,partnr);

   switch (display)
   {
      case 0:
         p = libpart[libpartptr].normalpinlist;
         pcnt = libpart[libpartptr].normalpinlistcnt;
      break;
      case 1:
         p = libpart[libpartptr].demorganpinlist;
         pcnt = libpart[libpartptr].demorganpinlistcnt;
      break;
      case 2:
         p = libpart[libpartptr].ieeepinlist;
         pcnt = libpart[libpartptr].ieeepinlistcnt;
      break;
   }

   for (int i=0;i<pcnt;i++)
   {
      // normalize every pin
      double x,y, px,py;
      int   rot = (crot + p[i].rotation) % 4;
      double len = p[i].pinlength;

      x = p[i].x;
      y = p[i].y;

      Rotate(x,y,90.0*crot,&px,&py);

      if (mirror)
      {
         px = -px;
      }

      x = cx + px;
      y = cy + py;

      // here write pin graphic if p[i].hidden && show_hidden_pins.
      // these hidden pins were not part of their library_definition
      if (show_hidden_pins && p[i].hidden)
      {
         switch (rot)
         {
            case 1:     
               Graph_Line(p[i].layernum,x,y, x, y+p[i].pinlength, 0, w[0], FALSE);
            break;
            case 2:     
               Graph_Line(p[i].layernum,x,y, x-p[i].pinlength, y, 0, w[0], FALSE);
            break;
            case 3:     
               Graph_Line(p[i].layernum,x,y, x, y-p[i].pinlength, 0, w[0], FALSE);
            break;
            default:    
               Graph_Line(p[i].layernum,x,y, x+p[i].pinlength, y, 0, w[0], FALSE);
            break;
         }
      }

      // write it out logical pins.
      int slen = strlen_text(p[i].pinname);
      double xx, yy;

      if (p[i].pinname && p[i].showpinname && (!p[i].hidden || show_hidden_pins))
      {
         int   layernum = Graph_Level("PINNAME","", 0);

         switch (rot)
         {
            case 1:
               if (mirror)
               {
                  //fx = -fx;
                  xx = x - (slen * font[0].height*HEIGHT_RATIO);
                  yy = y - (slen * font[0].height*FONT_CORRECT);
               }
               else
               {
                  xx = x + font[0].height*FONT_CORRECT/2;
                  yy = y - (slen*font[0].height*FONT_CORRECT);
               }
               Graph_Text(layernum, cnv_text(p[i].pinname), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, DegToRad(90), 0, TRUE,
                  0, 0, FALSE, w[0],1);
            break;
            case 2:
               if (mirror)
               {
                  //fx = -fx;
                  xx = x - (slen * font[0].height*HEIGHT_RATIO);
                  yy = y - font[0].height*FONT_CORRECT/2;
               }
               else
               {
                  xx = x + (font[0].height - font[0].height*FONT_CORRECT)/2;
                  yy = y - font[0].height*FONT_CORRECT/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinname), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 0.0, 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
            case 3:
               if (mirror)
               {
                  //fx = -fx;
                  xx = x + (slen * font[0].height*HEIGHT_RATIO);
                  yy = y + font[0].height*FONT_CORRECT/2;
               }
               else
               {
                  xx = x + font[0].height*FONT_CORRECT/2;
                  yy = y + font[0].height*FONT_CORRECT/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinname), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  DegToRad(90), 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
            default:
               if (mirror)
               {
                  //fx = -fx;
                  xx = x + (slen * font[0].height*HEIGHT_RATIO);
                  yy = y - font[0].height*FONT_CORRECT/2;
               }
               else
               {
                  xx = x-((slen+1)*font[0].height*HEIGHT_RATIO);
                  yy = y-font[0].height*FONT_CORRECT/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinname), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  0.0, 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
         }
      }

      // pin number always middle / top - physical pins
      if (p[i].pinnumber && p[i].showpinnumber && (!p[i].hidden || show_hidden_pins))
      {
         int    layernum = Graph_Level("PINNUMBER","", 0);

         switch(rot)
         {
            case 1:
               if (mirror)
               {
                  xx = x - (font[0].height - font[0].height*FONT_CORRECT)/2;
                  yy = y + len/2;
               }
               else
               {
                  xx = x - (font[0].height - font[0].height*FONT_CORRECT)/2;
                  yy = y + len/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinnumber), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  DegToRad(90.0), 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
            case 2:
               if (mirror)
               {
                  xx = x + len/2;
                  yy = y + (font[0].height - font[0].height*FONT_CORRECT)/2;
               }
               else
               {
                  xx = x - len/2;
                  yy = y + (font[0].height - font[0].height*FONT_CORRECT)/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinnumber), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  0.0, 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
            case 3:
               if (mirror)
               {
                  xx = x - (font[0].height - font[0].height*FONT_CORRECT)/2;
                  yy = y - (len/2+(slen*font[0].height*HEIGHT_RATIO));
               }
               else
               {
                  xx = x - (font[0].height - font[0].height*FONT_CORRECT)/2;
                  yy = y - (len/2+(slen*font[0].height*HEIGHT_RATIO));
               }
               Graph_Text(layernum, cnv_text(p[i].pinnumber), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  DegToRad(90.0), 0, TRUE, 0, 0, FALSE, w[0],1);
            break;
            default:
               if (mirror)
               {
                  xx = x - (len/2);
                  yy = y + (font[0].height - font[0].height*FONT_CORRECT)/2;
               }
               else
               {
                  xx = x + (len/2);
                  yy = y + (font[0].height - font[0].height*FONT_CORRECT)/2;
               }
               Graph_Text(layernum, cnv_text(p[i].pinnumber), xx, yy, 
                  font[0].height*FONT_CORRECT, font[0].width*HEIGHT_RATIO, 
                  0.0, 0, TRUE, 0, 0, FALSE, w[0],0);
            break;
         }
      }
   }

   return TRUE;
}

/****************************************************************************/
/*
*/
double cnv_tok(char *l)
{
   return Units_Factor(UNIT_MILS, pageUnits)*atof(l)*10; // 10 mil steps
}

/****************************************************************************/
/*
*/
double cnv_unit(double l)
{
   return Units_Factor(UNIT_MILS, pageUnits)*l*10; // 10 mil steps
}

/****************************************************************************/
/*
*/
static int get_nextline()
{
   if (fgets(line,MAX_LINE,fp) != NULL)
   {
      linecnt++;
      return TRUE;
   }
   return FALSE;
}

/*Ende **********************************************************************/
