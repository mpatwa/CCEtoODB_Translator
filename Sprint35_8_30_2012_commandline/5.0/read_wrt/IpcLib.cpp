// $Header: /CAMCAD/4.5/read_wrt/IpcLib.cpp 12    5/20/05 5:20p Lynn Phung $

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>
#include "CCview.h"
#include "ipclib.h"
#include "CCEtoODB.h"

#define  MAX_POLY       1000


static FILE    *file;
static int     cur_app;
static int     ppolycnt;
static Point2  *ppoly;
static char    ipcline[IPC_LINE_LENGTH+1];
static bool    ipcLineIsOpcode;
static long    ipccnt;
static int     roomForLineNumber;

static void    Load_Aperature(int app_num);
static void    Pen_Down(Point2 point);
static void    Pen_Up(Point2 point);
static void    Pen_Flash(Point2 point);

//static int      wipc_write_ipcline();
static int     write_ipccomment();
static int     write_ipcunit(const char *title, const char *dimchar, int ipctype, int ipcunits);
static int     write_ipclayer();
//static int      wipc_putchar_ipc(int,const char *);
static int     koo_string_LDA1(char *s,int x,int y);
static int     koo_string_LDA2(char *s,double x,double y);
static int     write_ipcheader(char *jobname);
static int     write_ipcfooter(char *f);

static int     cur_layernumber = 1;
static int     linefeed;
static int     IPCTYPE;
static CIPCLineList IpclineList;
static long arrIndex;
static long contDataIndex;

/****************************************************************************/
/*
*/
int write_ipcheader(const char *jobname, const char *dimchar, int ipctype, int ipcunits)
{
   char  tmp[IPC_LINE_LENGTH+1];
   CString  buf;

   IPCTYPE = ipctype;

   //buf.LoadString(IDR_MAINFRAME);
   write_ipccomment();

   wipc_putchar_ipc(1,"C");
   sprintf(tmp,"%s", getApp().getCamCadTitle());
   wipc_putchar_ipc(3,tmp);
   wipc_write_ipcline();

   time_t ltime;
   /* Set time zone from TZ environment variable. If TZ is not set,
     * operating system default is used, otherwise PST8PDT is used
     * (Pacific standard time, daylight savings).
   */
   _tzset();

   /* Get UNIX-style time and display as number and string. */
   time( &ltime );

   wipc_putchar_ipc(1,"C");
   sprintf(tmp,"JOBTIME : %s",ctime(&ltime));
   tmp[strlen(tmp)-1] = '\0';  // ctime returns a \n char.
   wipc_putchar_ipc(3,tmp);
   wipc_write_ipcline();

   write_ipccomment(); 

   wipc_putchar_ipc(1,"P");

   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath(jobname, drive, dir, fname, ext);

   sprintf(tmp,"JOB %s",fname);
   wipc_putchar_ipc(3,tmp);
   wipc_write_ipcline();
   
   write_ipcunit(jobname, dimchar, ipctype, ipcunits);

   if (ipctype == IPC356A) // ipc356a
   {

      // num
      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"NUM ?");
      wipc_write_ipcline();
      // rev
      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"REV ?");
      wipc_write_ipcline();
      // ver
      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"VER IPC-D-356A");
      wipc_write_ipcline();

      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"IMAGE PRIMARY");
      wipc_write_ipcline();
   }

   return 1;
}

/****************************************************************************/
/*
*/
void ChoosePivot(IPCOpcode OpcodeLine[], int first, int last)
{
   // choose a pivot and swaps it with the first item in an array
   // Since the opcode lines are almost sorted, pick the middle item as pivot.
   // This will at least avoid to pick the smallest as the pivot which leads to 
   // the worst case for quicksort(n sqrt).  
   long pivotIndex = (last - first) /2 + first;  // get the middle
   Swap(OpcodeLine[first],  OpcodeLine[pivotIndex]);

}

/****************************************************************************/
/*
*/
void Partition(IPCOpcode OpcodeLine[], int first, int last, int &PivotIndex)
{
   ChoosePivot(OpcodeLine, first, last);
   IPCOpcode Pivot = OpcodeLine[first];

   int LastS1 = first;
   int FirstUnknown = first + 1;

   // move one item at a time until unknown region is empty
   for (; FirstUnknown <= last; ++FirstUnknown)
   {     
      if (STRNICMP(OpcodeLine[FirstUnknown].ipcline, Pivot.ipcline, 21) < 0) // if opcode + net name is smaller then the pivot 
                                                         // opcode and net name takes upto 21 index in ipcline
      {
         // item from unknown belongs in S1
         ++LastS1;
         Swap(OpcodeLine[FirstUnknown], OpcodeLine[LastS1]);
      }
      // else item from unknown belongs in S2
   }
   // place pivot in proper position and mark its location
   Swap(OpcodeLine[first], OpcodeLine[LastS1]);
   PivotIndex = LastS1;
}

/****************************************************************************/
/*
*/
void Quicksort(IPCOpcode OpcodeLine[], int first, int last)
{
   // Average time for quicksort is nlog(n)
   int PivotIndex;
   if (first < last)
   {
      Partition(OpcodeLine, first, last, PivotIndex);
      // sort the regions s1 and s2
      Quicksort(OpcodeLine, first,PivotIndex -1);
      Quicksort(OpcodeLine, PivotIndex + 1, last);
   }
}

/****************************************************************************/
/*Swap
*/
void Swap(IPCOpcode &A, IPCOpcode &B)
{
   IPCOpcode tmp;
   tmp.ipcline[0] = '\0';
   strcpy(tmp.ipcline, A.ipcline);
   tmp.IPCTYPE = A.IPCTYPE;
   tmp.DataContStartInd = A.DataContStartInd;
   tmp.DataContEndInd = A.DataContEndInd;

   A.ipcline[0] = '\0';
   strcpy(A.ipcline, B.ipcline);
   A.IPCTYPE = B.IPCTYPE;
   A.DataContStartInd = B.DataContStartInd;
   A.DataContEndInd = B.DataContEndInd;

   B.ipcline[0] = '\0';
   strcpy(B.ipcline, tmp.ipcline);
   B.IPCTYPE = tmp.IPCTYPE;
   B.DataContStartInd = tmp.DataContStartInd;
   B.DataContEndInd = tmp.DataContEndInd;
}

//---------------------------------------------------------------------------

static bool wipcIsContinuationLine(char *linebuf)
{
   // opcode 078 is continuation record of trace segment (378)
   // opcode 027 is continuation record of blind via (307)
   // opcode 089 is continuation record of board edge (389) (IPC356A)

   if (linebuf != NULL && strlen(linebuf) > 2)
   {
      if (linebuf[0] == '0' && ( 
         (linebuf[1] == '7' && linebuf[2] == '8') || 
         (linebuf[1] == '2' && linebuf[2] == '7') ||
         (linebuf[1] == '8' && linebuf[2] == '9'))
         )
      {
         return true;
      }
   }

   return false;
}

/****************************************************************************/
/*SeparateOpcodeFromOthers
*/
void SeparateOpcodeFromOthers(IPCOpcode *opcode, ContData *contData)
{
   POSITION pos = IpclineList.GetHeadPosition();
   char  tmp[100];
   int index = 0, contIndex = 0;
   int ipctype;
   char curline[IPC_LINE_LENGTH+1];
   while(pos != NULL)
   {
      IPCLine *ipcline;
      //POSITION curPos = pos;
      ipcline = IpclineList.GetNext(pos);
      if (ipcline->opcode == 1)
      {
         if (wipcIsContinuationLine(ipcline->ipcline))
         {
            strcpy(contData[contIndex].ipcline, ipcline->ipcline);
            if (opcode[index -1].DataContStartInd == -1)  // if the ContData index is not assigned yet then assign it
               opcode[index -1].DataContStartInd = contIndex;
            contIndex++;
         }
         else
         {
            strcpy(opcode[index].ipcline, ipcline->ipcline);
            opcode[index].IPCTYPE =ipcline->IPCTYPE; 
            opcode[index].DataContStartInd = -1;
            opcode[index].DataContEndInd = -1;
            if (opcode[index-1].DataContStartInd > -1 && index > 0)
               opcode[index-1].DataContEndInd = contIndex -1;
            index++;
         }
      }
      else        
      {
         
         ipctype = ipcline->IPCTYPE;
         
         strcpy(curline, ipcline->ipcline);
         if (ipctype == IPC356A || ipctype == IPC356)
         {
            fprintf(file,"%s",curline);
         }
         else
         {           
            curline[72] = '\0';      // nothing is allowed to be longer than that.
            sprintf(tmp,"%sN%07ld",curline,++ipccnt);
            fprintf(file,"%s",tmp);
         }

         if (linefeed)
            fprintf(file,"\n");
      }
   }
   if (opcode[index-1].DataContStartInd > -1 && index > 0) // just in case the last opcode has the continuation data
      opcode[index-1].DataContEndInd = contIndex -1;
}

/****************************************************************************/
/*DoOpcode
*/
void DoOpcode()
{
   IPCOpcode* opcode = new IPCOpcode[arrIndex];
   ContData* contData = new ContData[contDataIndex];
   char  tmp[100];

   SeparateOpcodeFromOthers(opcode, contData);
   Quicksort(opcode, 0, arrIndex - 1);
   for(int i = 0; i < arrIndex; i++)  // now the array is sorted, so write to the file
   {
      if (opcode[i].IPCTYPE == IPC356A || opcode[i].IPCTYPE == IPC356)
         {
            fprintf(file,"%s",opcode[i].ipcline);
            if (linefeed)
               fprintf(file,"\n");
            if (opcode[i].DataContStartInd > -1)  // if there is continuation data then get the data from contData array
               for (int j = opcode[i].DataContStartInd; j <= opcode[i].DataContEndInd; j++)
               {
                  fprintf(file, "%s", contData[j].ipcline);
                  if (linefeed)
                     fprintf(file,"\n");
               }
         }
         else
         {           
            opcode[i].ipcline[72] = '\0';      // nothing is allowed to be longer than that.
            sprintf(tmp,"%sN%07ld",opcode[i].ipcline,++ipccnt);
            fprintf(file,"%s",tmp);
            if (linefeed)
               fprintf(file,"\n");
            if (opcode[i].DataContStartInd > -1) // if there is continuation data then get the data from contData array
               for (int j = opcode[i].DataContStartInd; j <= opcode[i].DataContEndInd; j++)
               {
                  contData[j].ipcline[72] = '\0';
                  sprintf(tmp,"%sN%07ld",contData[j].ipcline,++ipccnt);
                  fprintf(file,"%s",tmp);
                  if (linefeed)
                     fprintf(file,"\n");
               }
         }
   }
   delete opcode;
   delete contData;
}

/****************************************************************************/
/*write_ipcfooter
*/
int write_ipcfooter(char *f)
{  
   char  tmp[100];

   wipc_putchar_ipc(1,"999");
   Writer_Footer();

   int   i;
   ipcline[0] = 'C';
   for (i=1;i<72;i++)
      ipcline[i] = '*';
   Writer_Footer();

   sprintf(tmp,"C End File");
   wipc_putchar_ipc(1,tmp);
   Writer_Footer();

   ipcline[0] = 'C';
   for (i=1;i<72;i++)
      ipcline[i] = '*';
   Writer_Footer(); 
   return 1;
}

/******************************************************************************
* wipc_set_opcode
* true indicates current content of ipcline is an opcode record
*/
void wipc_set_opcode(bool flag)
{
   ipcLineIsOpcode = flag;
}

/******************************************************************************
* wipc_clean_ipcline
*/
int wipc_clean_ipcline()
{
   int   i;

   for (i=0; i<IPC_LINE_LENGTH; i++)
      ipcline[i] = ' ';

   ipcLineIsOpcode = false;

   return 1;
}

/****************************************************************************/
/*
*/
void Writer_Footer()
{
   char  tmp[100];

   if (IPCTYPE == IPC356A || IPCTYPE == IPC356)
   {
      fprintf(file,"%s",ipcline);
   }
   else
   {
      // N = 73
      // 74--80 = 7 characters.
      ipcline[72] = '\0';      // nothing is allowed to be longer than that.
      sprintf(tmp,"%sN%07ld",ipcline,++ipccnt);
      fprintf(file,"%s",tmp);
   }

   if (linefeed)
      fprintf(file,"\n"); 

   wipc_clean_ipcline(); 
}

/******************************************************************************
* wipc_write_ipcline
*/
int wipc_write_ipcline()
{
   IPCLine *perline = new IPCLine;
   strcpy(perline->ipcline, ipcline);
   perline->IPCTYPE = IPCTYPE;
   if (ipcLineIsOpcode)
   {
      perline->opcode = TRUE;
      if (wipcIsContinuationLine(ipcline))
         contDataIndex++;
      else
         arrIndex++;
   }
   else
      perline->opcode = FALSE;

   IpclineList.AddTail(perline);
   wipc_clean_ipcline(); 
   return 1;
}

/******************************************************************************
* wipc_putchar_ipc
*/
int wipc_putchar_ipc(int position, const char *n)
{
   position--; /* C starts with 0 */

   if (position + (int)strlen(n) > (IPC_LINE_LENGTH - roomForLineNumber))  // this is the sequence number length
   {
      // printf("Line too long : Pos %d [%s]\n",position,n);
   }
   else
   {
      for (unsigned int i=0; i<strlen(n); i++)
         ipcline[i+position] = n[i];
   }

   // jic
   ipcline[IPC_LINE_LENGTH] = '\0';

   return 1;
}

/****************************************************************************/
/*
*/
int write_ipccomment()
{
   int   i;
   ipcline[0] = 'C';
   for (i=1;i<72;i++)
      ipcline[i] = '*';
   wipc_write_ipcline();
   return 1;
}

/****************************************************************************/
/*
*/
void wipc_negativelayer(int conductive)
{
   wipc_putchar_ipc(1,"P");
   wipc_putchar_ipc(4,"IMAGE");
   if (conductive)
      wipc_putchar_ipc(10,"COND NEG");
   else
      wipc_putchar_ipc(10,"NCON NEG");
   wipc_write_ipcline();
}

/****************************************************************************/
/*
*/
void wipc_positivelayer(int conductive)
{
   wipc_putchar_ipc(1,"P");
   wipc_putchar_ipc(4,"IMAGE");
   if (conductive)
      wipc_putchar_ipc(10,"COND POS");
   else
      wipc_putchar_ipc(10,"NCON POS");

   wipc_write_ipcline();
}

/****************************************************************************/
/*
*/
int write_ipcunit(const char *title, const char *dimchar, int ipctype, int ipcunits)
{
   char tmp[IPC_LINE_LENGTH+1];
   char *unitParameter;
   char *unitComment;

   switch(ipcunits)
   {
   case pageUnitsInches:
   case pageUnitsMils:
      unitParameter = "CUST 0";
      unitComment = "CUSTOMARY (ENGLISH) UNITS";
      break;
   case pageUnitsMilliMeters:
   default:
      unitParameter = "SI";
      unitComment = "INTERNATIONAL SYSTEM (METRIC) UNITS";
      break;
   }

   wipc_putchar_ipc(1,"C");
   sprintf(tmp,"DEFINE THE UNITS USED TO BE %s",unitComment);
   wipc_putchar_ipc(3,tmp);
   wipc_write_ipcline();
   
   wipc_putchar_ipc(1,"P");
   wipc_putchar_ipc(4,"UNITS");
   wipc_putchar_ipc(10,unitParameter);
   wipc_write_ipcline();

   if (strlen(dimchar))
   {
      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"DIM");
      wipc_putchar_ipc(10,dimchar);
      wipc_write_ipcline();
   }

   wipc_putchar_ipc(1,"P");
   wipc_putchar_ipc(4,"TITLE");
   wipc_putchar_ipc(10,title);
   wipc_write_ipcline();

   if (ipctype != IPC356A)
   {
      wipc_putchar_ipc(1,"P");
      wipc_putchar_ipc(4,"IMAGE");
      wipc_putchar_ipc(10,"NCON POS");
      wipc_write_ipcline();
   }

   return 1;
}

/****************************************************************************/
/*
*/
int koo_string_LDA1(char *s,int x,int y)
{
   sprintf(s,"X%+09dY%+09d",x,y);
   return 1;
}

/****************************************************************************/
/*
*/
int koo_string_LDA2(char *s,double x,double y)
{
   long  x1,y1;
   x1 = (long)floor(10L * x + 0.5);
   y1 = (long)floor(10L * y + 0.5);

   sprintf(s,"X%+09ldY%+09ld",x1,y1);
   return 1;
}

/********************************************************
*/
int wipc_Graph_File_Open(const char *out_file, int lf, const char *dimheader, int ipctype, int ipcunits)
{
   linefeed = lf;
   if ((file = fopen(out_file, "wt")) == NULL)
   {
      ErrorMessage("Error open file", out_file);
      return -1;
   }

   // here init buffer for Polyline
   if ((ppoly = (Point2 *)calloc(MAX_POLY,sizeof(Point2))) == NULL)
   {
      return 0;
      //exit(1);
   }
   ppolycnt = 0;
   cur_app = -1;
   arrIndex = 0;
   contDataIndex = 0;
   roomForLineNumber = 0; // IPC356 and 356A
   ipcLineIsOpcode = false;
   if (ipctype == IPC350)
      roomForLineNumber = 7; // reserves 7 chars in line buffer for line number in IPC350 format

   write_ipcheader(out_file, dimheader, ipctype, ipcunits);
   return 1;
}

/********************************************************
*/
void wipc_Graph_Layer(int layernr)
{
   cur_layernumber = layernr; // if layernr == -2 do not write 
                       // layer record. this is used for
                       // free layers in complex aperture definitions.
}

/********************************************************
*/
void wipc_Graph_File_Close()
{
   DoOpcode();
   write_ipcfooter("");
   fclose(file);
   // here free buffer for Polyline.
   free((char *)ppoly);
   IpclineList.RemoveAll();
   return;
}

/****************************************************************************/
/*
*/
void wipc_aperturedef(int typ,int dcode, double sizeA, double sizeB, 
                      double offx, double offy, double rotation)
{
   char   tmp[80];
   switch (typ)
   {
      case T_THERMAL:
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  THERMAL %1.0lf %1.0lf",dcode,sizeA,sizeB);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();
     break;
      case T_DONUT:
         {
            wipc_putchar_ipc(1,"C");
            sprintf(tmp,"APERTURE D%2d  DONUT %1.0lf %1.0lf",dcode,sizeA,sizeB);
            wipc_putchar_ipc(3,tmp);
            wipc_write_ipcline();

            wipc_putchar_ipc(1,"222");
            sprintf(tmp,"D%04d",dcode);
            wipc_putchar_ipc(4,tmp);
            koo_string_LDA2(tmp,0.0,0.0);
            wipc_putchar_ipc(31,tmp);
            wipc_write_ipcline();

            Point2 p;

            wipc_positivelayer(FALSE);
            wipc_Flash_App(p, 0, T_ROUND, -2, sizeA);
            wipc_negativelayer(FALSE);
            wipc_Flash_App(p, 0, T_ROUND, -2, sizeB);
            wipc_positivelayer(FALSE);

            wipc_putchar_ipc(1,"299");  // end of def
            wipc_write_ipcline();
         }
      break;
      case  T_RECTANGLE:
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  RECTANGLE %1.0lf %1.0lf",dcode,sizeA,sizeB);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();

         wipc_putchar_ipc(1,"222");
         sprintf(tmp,"D%04d",dcode);
         wipc_putchar_ipc(4,tmp);
         koo_string_LDA2(tmp,0.0,0.0);
         wipc_putchar_ipc(31,tmp);
         wipc_write_ipcline();

         if (sizeA < sizeB)
         {
            double   x, y, x0, y0, x1, y1;
            // sizeA is width, vertical
            x = offx;
            y = -sizeB/2 + offy;
            Rotate(x,y,RadToDeg(rotation),&x0, &y0);
            y =  sizeB/2 + offy;
            Rotate(x,y,RadToDeg(rotation),&x1, &y1);

            wipc_putchar_ipc(1,"112");
            sprintf(tmp,"D%04d",round(10*sizeA));  // in Colum 4 
            wipc_putchar_ipc(4,tmp);
            wipc_putchar_ipc(26,"G3");
            koo_string_LDA2(tmp,x0,y0);
            wipc_putchar_ipc(31,tmp);
            koo_string_LDA2(tmp,x1,y1);
            wipc_putchar_ipc(51,tmp);
         }
         else
         {
            double   x, y, x0, y0, x1, y1;
            // sizeB is width, horizontal
            y = offy;
            x= -sizeA/2 + offx;
            Rotate(x,y,RadToDeg(rotation),&x0, &y0);
            x= sizeA/2 + offx;
            Rotate(x,y,RadToDeg(rotation),&x1, &y1);

            wipc_putchar_ipc(1,"112");
            sprintf(tmp,"D%04d",round(10*sizeB));  // in Colum 4 
            wipc_putchar_ipc(4,tmp);
            wipc_putchar_ipc(26,"G3");
            koo_string_LDA2(tmp,x0,y0);
            wipc_putchar_ipc(31,tmp);
            koo_string_LDA2(tmp,x1,y1);
            wipc_putchar_ipc(51,tmp);
         }

         wipc_write_ipcline();

         wipc_putchar_ipc(1,"299");  // end of def
         wipc_write_ipcline();
      break;               
      case  T_SQUARE:   // does not need a block definition
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  SQUARE %1.0lf",dcode,sizeA);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();
      break;
      case T_OCTAGON:
      {
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  OCTAGON %1.0lf",dcode,sizeA);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();

         wipc_putchar_ipc(1,"222");
         sprintf(tmp,"D%04d",dcode);
         wipc_putchar_ipc(4,tmp);
         koo_string_LDA2(tmp,0.0,0.0);
         wipc_putchar_ipc(31,tmp);
         wipc_write_ipcline();

         double radius = sizeA/2;
         double halfSide = sizeA/4.83; // size of half of one side of octagon

         int   merk_layer = cur_layernumber;
         cur_layernumber = -2;

         Point2   p[8];
         p[0].x =  offx - radius;
         p[0].y  = offy + halfSide;
         p[0].bulge = 0.0;
         p[1].x = offx - halfSide;
         p[1].y = offy + radius;
         p[1].bulge = 0.0;
         p[2].x = offx + halfSide;
         p[2].y = offy + radius;
         p[2].bulge = 0.0;
         p[3].x = offx + radius;
         p[3].y = offy + halfSide;
         p[3].bulge = 0.0;
         p[4].x = offx + radius;
         p[4].y = offy - halfSide;
         p[4].bulge = 0.0;
         p[5].x = offx + halfSide;
         p[5].y = offy - radius;
         p[5].bulge = 0.0;
         p[6].x = offx - halfSide;
         p[6].y = offy - radius;
         p[6].bulge = 0.0;
         p[7].x = offx - radius;
         p[7].y = offy - halfSide;
         p[7].bulge = 0.0;
         wipc_Graph_Polyline(p, 8, 0, 0, 0.0, TRUE);
         cur_layernumber = merk_layer;

         wipc_putchar_ipc(1,"299");  // end of def
         wipc_write_ipcline();
      }
      break;
      case T_OBLONG:
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  OBLONG %1.0lf %1.0lf %1.0lf %1.0lf %1.0lf",dcode,
            sizeA,sizeB, offx, offy, RadToDeg(rotation));
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();

         wipc_putchar_ipc(1,"222");
         sprintf(tmp,"D%04d",dcode);
         wipc_putchar_ipc(4,tmp);
         koo_string_LDA2(tmp,0.0,0.0);
         wipc_putchar_ipc(31,tmp);
         wipc_write_ipcline();

         if (sizeA < sizeB)
         {
            double   x, y, x0, y0, x1, y1;
            // sizeA is width, vertical
            x = offx;
            y = -sizeB/2 + offy;
            Rotate(x,y,RadToDeg(rotation),&x0, &y0);
            y =  sizeB/2 + offy;
            Rotate(x,y,RadToDeg(rotation),&x1, &y1);

            wipc_putchar_ipc(1,"112");
            sprintf(tmp,"D%04d",round(10*sizeA));  // in Colum 4 
            wipc_putchar_ipc(4,tmp);
            wipc_putchar_ipc(26,"G2");
            koo_string_LDA2(tmp,x0,y0);
            wipc_putchar_ipc(31,tmp);
            koo_string_LDA2(tmp,x1,y1);
            wipc_putchar_ipc(51,tmp);
         }
         else
         {
            double   x, y, x0, y0, x1, y1;
            // sizeB is width, horizontal
            y = offy;
            x= -sizeA/2 + offx;
            Rotate(x,y,RadToDeg(rotation),&x0, &y0);
            x= sizeA/2 + offx;
            Rotate(x,y,RadToDeg(rotation),&x1, &y1);

            wipc_putchar_ipc(1,"112");
            sprintf(tmp,"D%04d",round(10*sizeB));  // in Colum 4 
            wipc_putchar_ipc(4,tmp);
            wipc_putchar_ipc(26,"G2");
            koo_string_LDA2(tmp,x0,y0);
            wipc_putchar_ipc(31,tmp);
            koo_string_LDA2(tmp,x1,y1);
            wipc_putchar_ipc(51,tmp);
         }
         wipc_write_ipcline();

         wipc_putchar_ipc(1,"299");  // end of def
         wipc_write_ipcline();
      break;
      case T_ROUND:   // does not need a block defintion.
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"APERTURE D%2d  ROUND %1.0lf",dcode,sizeA);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();
      break;                 
      default:
         wipc_putchar_ipc(1,"C");
         sprintf(tmp,"UNDEFINED %d DCODE %d %1.0lf %1.0lf",typ,dcode,sizeA,sizeB);
         wipc_putchar_ipc(3,tmp);
         wipc_write_ipcline();
      break;
   }
   return;
}

/********************************************************
*/
void wipc_write_startcomplex(int dcode)
{
   char   tmp[80];

   wipc_putchar_ipc(1,"222");
   sprintf(tmp,"D%04d",dcode);
   wipc_putchar_ipc(4,tmp);
   koo_string_LDA2(tmp,0.0,0.0);
   wipc_putchar_ipc(31,tmp);
   wipc_write_ipcline();
}

/********************************************************
*/
void wipc_write_endcomplex()
{
   wipc_putchar_ipc(1,"299");
   wipc_write_ipcline();
}

/********************************************************
*/
void wipc_Flash_App(Point2 p, int dcode, int shape, int layer, double sizeA)
{
   char tmp[80];

   wipc_putchar_ipc(1,"322");

   // D code or size
   switch (shape)
   {
     case T_ROUND:
       sprintf(tmp,"D%04d",round(10*sizeA));  // in Colum 4 
     break;
     case T_SQUARE:
       sprintf(tmp,"D%04d",round(10*sizeA));  // in Colum 4 
     break;
     default:
       sprintf(tmp,"D%04d",dcode);  // in Colum 4 
     break;
   }
   wipc_putchar_ipc(4,tmp);
   // L layer
   if (layer > -2)
   {
      sprintf(tmp,"L%02d",layer);
      wipc_putchar_ipc(9,tmp);
   }
   // G code 0 = circle 1 = square 4 = shape
   switch (shape)
   {
     case T_ROUND:
       sprintf(tmp,"G0");
     break;
     case T_SQUARE:
       sprintf(tmp,"G1");
     break;
     default:
       sprintf(tmp,"G4");
     break;
   }
   wipc_putchar_ipc(26,tmp);
   // xy
   koo_string_LDA2(tmp,p.x,p.y);
   wipc_putchar_ipc(31,tmp);
   wipc_write_ipcline();
}

/***************************************************************************/
/*
  height is height of char
  width is width of char
*/
int wipc_Graph_Text( const char *text, double x1, double y1,
         double height, double width, double char_linewidth,
         double angle, int text_mirror, int oblique)
{
   char   tmp[80];
   int    need_a = FALSE;
   int    a = (int)floor(RadToDeg(angle) + 0.5);   // 1/10 degree acc

   while (a < 0)     a += 360;
   while (a >= 360)  a -= 360;

   wipc_putchar_ipc(1,"512");

   sprintf(tmp,"D%04d",round(10*char_linewidth));  // in Colum 4 
   wipc_putchar_ipc(4,tmp);                 //
   if (cur_layernumber == -2)
   {
     // do not write a layer
   }
   else
   {
     sprintf(tmp,"L%02d",cur_layernumber);
     wipc_putchar_ipc(9,tmp);
   }
   sprintf(tmp,"H%04d",round(10*height));  // in Colum 
   wipc_putchar_ipc(19,tmp);  

   switch (round(a / 90))
   {    
     case 0:
       if (text_mirror)
         strcpy(tmp,"G4");
       else
         strcpy(tmp,"G0");
     break;
     case 1:
       if (text_mirror)
         strcpy(tmp,"G5");
       else
         strcpy(tmp,"G1");
     break;
     case 2:
       if (text_mirror)
         strcpy(tmp,"G6");
       else
         strcpy(tmp,"G2");
     break;
     case 3:
       if (text_mirror)
         strcpy(tmp,"G7");
       else
         strcpy(tmp,"G3");
     break;
     default:
       if (text_mirror)
         strcpy(tmp,"G9");
       else
         strcpy(tmp,"G8");
       need_a = TRUE;
     break;
   }
   wipc_putchar_ipc(26,tmp);

   koo_string_LDA2(tmp,x1,y1);
   wipc_putchar_ipc(31,tmp);

   //51 x = length of text and angle if needed
   if (!need_a)     a = 0;
   koo_string_LDA2(tmp,width * strlen(text),1000.0 * a);
   wipc_putchar_ipc(51,tmp);
   wipc_write_ipcline();

   strcpy(tmp,text);
   tmp[72-4-1] = '\0';  // limit text to max ipc char line.
   wipc_putchar_ipc(1,"000");
   wipc_putchar_ipc(4,tmp);
   wipc_write_ipcline();

   return 1;
}

/********************************************************
*/
void wipc_Line(Point2 start, Point2 end, int app_num,int apshape, double w )
{
   ppoly[0].x = start.x;
   ppoly[0].y = start.y;
   ppoly[1].x = end.x;
   ppoly[1].y = end.y;
   ppolycnt = 2;

   wipc_Graph_Polyline(ppoly,ppolycnt,app_num, apshape,w,0);
   return;
}

/********************************************************
*/
static void wipc_Arc(int filled, int cont, Point2 center, double radius, double startradians, 
                double deltaradians, double width)
{
   char   tmp[80];
   double   endangle;
   double   dir = 100;

   if (deltaradians < 0)   dir = -100;
   endangle = startradians + deltaradians;

   if (!cont)
   {
      if (filled)
         wipc_putchar_ipc(1,"152");
      else
         wipc_putchar_ipc(1,"122");
   }
   else
   {
      if (filled)
         wipc_putchar_ipc(1,"052");
      else
         wipc_putchar_ipc(1,"022");
   }

   sprintf(tmp,"D%04d",round(10*width));  // in Colum 4 
   wipc_putchar_ipc(4,tmp);
   //
   if (cur_layernumber == -2)
   {
     // do not write a layer
   }
   else
   {
     sprintf(tmp,"L%02d",cur_layernumber);
     wipc_putchar_ipc(9,tmp);
   }

   double sa = RadToDeg(startradians), ea = RadToDeg(endangle);
   while (sa < 0) sa += 360;
   while (sa >= 360) sa -= 360;
   while (ea < 0) ea += 360;
   while (ea >= 360) ea -= 360;

   wipc_putchar_ipc,(26,"G0");
   koo_string_LDA2(tmp,center.x,center.y);
   wipc_putchar_ipc(31,tmp);
   koo_string_LDA2(tmp,sa*1000,ea*1000);  // 0.0001 degrees
   wipc_putchar_ipc(51,tmp);
   wipc_write_ipcline();

   wipc_putchar_ipc(1,"000");
   koo_string_LDA2(tmp,radius,dir);
   wipc_putchar_ipc(31,tmp);
   wipc_write_ipcline();

   return;
}

/********************************************************
*/
void wipc_Circle(Point2 center, double radius, double width, int filled)
{
   char   tmp[80];

   if (filled)
   {
      wipc_Flash_App(center, 0, T_ROUND, -2, radius*2+width);
   }
   else
   {
     wipc_putchar_ipc(1,"122");

      sprintf(tmp,"D%04d", round(10*width));  // in Colum 4 
      wipc_putchar_ipc(4,tmp);                 //
      if (cur_layernumber == -2)
      {  
         // do not write a layer
      }
      else
      {
         sprintf(tmp,"L%02d",cur_layernumber);
         wipc_putchar_ipc(9,tmp);
      }
      wipc_putchar_ipc,(26,"G0");
      koo_string_LDA2(tmp,center.x,center.y);
      wipc_putchar_ipc(31,tmp);
      koo_string_LDA2(tmp,0.0,360000.0);
      wipc_putchar_ipc(51,tmp);
      wipc_write_ipcline();

      wipc_putchar_ipc(1,"000");
      koo_string_LDA2(tmp,radius,100.0);
      wipc_putchar_ipc(31,tmp);
      wipc_write_ipcline();
   }
   return;
}

/******************************************************************************
* wipc_write2file
   this just writes out a text into the output file
*/
void wipc_write2file(const char *prosa)
{
   wipc_putchar_ipc(1, "C");
   wipc_putchar_ipc(3, prosa);
   wipc_write_ipcline();
   return;
}

/****************************************************************************/
/*
*/
void wipc_Graph_Polyline(Point2 *p, int cnt, int app_num, int shape, double width, int filled)
{
   int   i;
   char  tmp[80];

   if (cnt == 0)  return;

   // zero linelength needs to flash
   if (cnt == 1)
   {
      wipc_Flash_App(p[0], app_num, shape, cur_layernumber, width);
      return;
   }
   else
   if (cnt == 2)
   {
      if (fabs(p[0].x - p[1].x) < 1 && fabs(p[0].y - p[1].y) < 1)
      {
         wipc_Flash_App(p[0], app_num, shape, cur_layernumber, width);
         return;
      }
   }

   if (filled)
     wipc_putchar_ipc(1,"142");
   else
     wipc_putchar_ipc(1,"112");

   sprintf(tmp,"D%04d",round(10*width));  // in Colum 4 
   wipc_putchar_ipc(4,tmp);         
   if (cur_layernumber == -2)
   {
     // do not write a layer
   }
   else
   {
     sprintf(tmp,"L%02d",cur_layernumber);
     wipc_putchar_ipc(9,tmp);
   }

   switch (shape)
   {
     case T_RECTANGLE:
     case T_SQUARE:
         wipc_putchar_ipc(26,"G1");
     break;
     default:
         wipc_putchar_ipc(26,"G0");
     break;
   }

   // only the first 28 chars
   for (i=1;i<cnt;i++)
   {
      // for not fill
      // it starts 112 line x1y1x2y2
      //           022 arc
      //           012 line etc...
      // for fill 142
      //          152
      double da = atan(p[i-1].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         ArcPoint2Angle( p[i-1].x, p[i-1].y, p[i].x, p[i].y, da, &cx, &cy, &r, &sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;

         Point2   center;
         center.x = cx;
         center.y = cy;

         wipc_Arc(filled, (i > 1), center, r, sa, da, width);
      }
      else
      {
         if (i == 1) // start
         {
            if (filled)
               wipc_putchar_ipc(1,"142");
            else
               wipc_putchar_ipc(1,"112");
         }
         else
         {
            if (filled)
               wipc_putchar_ipc(1,"042");
            else
               wipc_putchar_ipc(1,"012");
         }

         // x1y1  31-50
         koo_string_LDA2(tmp,p[i-1].x,p[i-1].y);
         wipc_putchar_ipc(31,tmp);
         koo_string_LDA2(tmp,p[i].x,p[i].y);
         wipc_putchar_ipc(51,tmp);
         wipc_write_ipcline();
         //wipc_putchar_ipc(1,"000");
      } // not bulge
   }
   wipc_clean_ipcline();
   return;
}

// end IPCLIB.C
 

