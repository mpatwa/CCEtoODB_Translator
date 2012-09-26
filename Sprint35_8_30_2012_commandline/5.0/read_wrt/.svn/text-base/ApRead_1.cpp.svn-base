// $Header: /CAMCAD/4.3/read_wrt/ApRead_1.cpp 6     8/12/03 2:56p Kurt Van Ness $

/****************************************************************************/
/*
    This read the teradyne format
*/
/****************************************************************************/

#include "stdafx.h"
#include "apread.h"
#include "dbutil.h"

static   Apdef    apdef;        

/****************************************************************************/
/*
read_Teradyne_1   format:

INPUT-UNITS     INCHES

WHEEL     1

1     D96        cir4
2     D100       cir5
3     D97        cir6
4     D103       cir7
5     D98        cir8
6     D94        cir10
7     D108       cir11
8     D95        cir12
9     D109       cir14
10    D105       cir15
11    D102       cir20
12    D104       cir24
13    D99        cir25
14    D106       cir40
15    D107       cir50
16    D101       cir60
18    D49        cir15
19    D47        cir40
20    D48        cir44
21    D50        cir45
22    D37        cir50
23    D57        cir54
24    D51        cir57
25    D34        cir60
26    D52        cir70
27    D75        cir75
28    D69        cir80
29    D68        cir90
30    D89        cir100
31    D14        cir120
32    D93        cir125
33    D74        cir150
34    D31        cir200
35    D76        cir225
36    D32        cir250
37    D77        cir275
38    D35        rc50.0x50.0     // rectangle
39    D38        rc54.0x54.0
40    D66        rc60.0x60.0
41    D67        rc64.0x64.0
42    D88        rc70.0x70.0
43    D87        rc80.0x80.0
44    D92        rc90.0x90.0
45    D91        rc100.0x100.0
46    D27        rc14.0x64.0
47    D83        rc14.0x70.0
48    D29        rc18.0x68.0
49    D85        rc18.0x74.0
50    D79        rc24.0x80.0
51    D71        rc24.0x84.0
52    D59        rc25.0x80.0
53    D81        rc28.0x84.0
54    D73        rc28.0x88.0
55    D61        rc29.0x84.0
56    D53        rc40.0x48.0
57    D43        rc40.0x60.0
58    D55        rc44.0x52.0
59    D45        rc44.0x64.0
60    D54        rc48.0x40.0
61    D62        rc50.0x60.0
62    D56        rc52.0x44.0
63    D64        rc58.0x68.0
64    D44        rc60.0x40.0
65    D63        rc60.0x50.0
66    D16        rc60.0x100.0
67    D40        rc60.0x140.0
68    D28        rc64.0x14.0
69    D46        rc64.0x44.0
70    D18        rc64.0x104.0
71    D42        rc64.0x144.0
72    D30        rc68.0x18.0
73    D65        rc68.0x58.0
74    D82        rc70.0x14.0
75    D20        rc70.0x100.0
76    D84        rc74.0x18.0
77    D22        rc74.0x104.0
78    D78        rc80.0x24.0
79    D58        rc80.0x25.0
80    D70        rc84.0x24.0
81    D80        rc84.0x28.0
82    D60        rc84.0x29.0
83    D72        rc88.0x28.0
84    D15        rc100.0x60.0
85    D19        rc100.0x70.0
86    D23        rc100.0x120.0
87    D17        rc104.0x64.0
88    D21        rc104.0x74.0
89    D25        rc104.0x124.0
90    D24        rc120.0x100.0
91    D26        rc124.0x104.0
92    D39        rc140.0x60.0
93    D41        rc144.0x64.0
94    D11        rc160.0x220.0
95    D13        rc164.0x224.0
96    D10        rc220.0x160.0
97    D12        rc224.0x164.0
FLASH     TH100                      0  D90
FLASH     TH60                       0  D36
FLASH     TH80                       0  D86

*/
int read_teradyne_1(const char *fname, FILE *flog)
{
   FILE  *ifp;
   char  line[255];
   char  tmp[255];
   char  *lp;
   int   linecnt = 0;

   if ((ifp = fopen(fname,"rt")) != NULL)
   {
      linecnt = 0;
      while (fgets(line,255,ifp))
      {
         int err = TRUE;
         if (strlen(line) && line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
         strcpy(tmp,line);
         linecnt++;

         if ((lp = strtok(line," \t\n")) == NULL)  continue;

         if (!STRCMPI(lp,"INPUT-UNITS"))
         {
            if ((lp = strtok(NULL," \n\t")) == NULL)
            {
               fprintf(flog,"%3d:Units INCHES expected\n",linecnt);
            }
            else
            if (STRCMPI(lp,"INCHES"))
            {
               fprintf(flog,"%3d:Units INCHES expected\n",linecnt);
            }
            else
               err = FALSE;   // inches is ok.
         }
         else
         if (!STRCMPI(lp,"WHEEL"))
         {
            // do nothing
            err = FALSE;
         }
         else
         if (atoi(lp))
         {
            // 99 D19 cir4
            if ((lp = strtok(NULL," \t\n")) != NULL)
            {
               if (lp[0] == 'D')
               {
                  apdef.dcode = atoi(&lp[1]);
                  if ((lp = strtok(NULL," x\t\n")) != NULL)
                  {
                     if (!STRNICMP(lp,"cir",3))
                     {
                        apdef.xsize = atof(&lp[3]);
                        apdef.ysize = 0;
                        apdef.apptype = 6;   // for round in apread.c
                        output_app(&apdef);
                        err = FALSE;
                     }
                     else
                     if (!STRNICMP(lp,"rc",2))
                     {
                        apdef.xsize = atof(&lp[2]);
                        if ((lp = strtok(NULL," \t\n")) != NULL)
                        {
                           apdef.ysize = atof(lp);
                           apdef.apptype = 8;   // for rectangle in apread.c
                           output_app(&apdef);
                           err = FALSE;
                        }
                     }
                  }
               } // Dcode found.
            }
         }
         else
         if (!STRCMPI(lp,"FLASH"))
         {
            //FLASH TH999 0 D19

         }

         if (err)
         {
            fprintf(flog,"%3d:Could not find a match for [%s]\n",linecnt,tmp);
         }
      }
      fclose(ifp);
   }
   else
   {
      #ifdef ___DOS
      printf("Can not open [%s] for read.\n",fname);
      #else
      ErrorMessage(fname, "Can not open file for reading.", MB_ICONEXCLAMATION | MB_OK);
      #endif
   }
   return 1;
}

/****************************************************************************/
/*
*/
static int get_dcode(int i)
{
   //appcode   1..10    D10..D19
   //          11       D70
   //          12       D71
   //          13..22   D20..D29
   //          23       D72
   //          24       D73
   //          25..64   D30..70
   //          65..     D74...
   if (i>=1 && i<=10)   return i+9;
   if (i==11)           return 70;
   if (i==12)           return 71;
   if (i>=13 && i<=22)  return i+7; 
   if (i==23)           return 72;
   if (i==24)           return 73;
   if (i>=25 && i<=64)  return i+30-25;
   if (i>=65)           return i+9;
   return  i+100;
}

/****************************************************************************/
/*
*/
int read_old_pcgerber(const char *fname, FILE *flog)
{
   FILE  *ifp;
   char  line[255];
   char  *lp;
   int   linecnt = 0;
   int   i,tot_app = 0;

   if ((ifp = fopen(fname,"rt")) != NULL)
   {
      linecnt = 0;
      if (fgets(line,255,ifp) == NULL)
      {
         return FALSE;
      }
      tot_app = atoi(line);

      for (i=0;i<tot_app;i++)
      {
         if (fgets(line,255,ifp) == NULL)
         {
            ErrorMessage("Unexpected end of file.", "Error Reading Aperture File", MB_ICONEXCLAMATION | MB_OK);
            return FALSE;
         }
         apdef.apnum = i+1;
         if ((lp = strtok(line," \t\n")) == NULL)
            continue;

         apdef.dcode = get_dcode(i+1);;

         switch (atoi(lp))
         {
            case 0:  // blank
            /* Do not store blank
               apdef.apptype = FORM_BLANK;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            */
            break;
            case 1:  // round
               apdef.apptype = FORM_ROUND;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 2:  // square
               apdef.apptype = FORM_SQUARE;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 3:  // rectangle
               apdef.apptype = FORM_RECT;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 4:  // target
               apdef.apptype = FORM_TARGET;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 5:  // thermal
               apdef.apptype = FORM_THERM;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 6:  //custom
               apdef.apptype = FORM_COMPLEX;
               apdef.xsize = 0;
               apdef.ysize = 0;
               output_app(&apdef);
            break;
            case 7:  // donut
               apdef.apptype = FORM_DONUT;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 8:  // octagon
               apdef.apptype = FORM_OCTAGON;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            case 9:  // oblong
               apdef.apptype = FORM_OBLONG;
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.xsize = atof(lp);
               if ((lp = strtok(NULL," \t\n")) == NULL)
                  continue;
               apdef.ysize = atoi(lp);
               output_app(&apdef);
            break;
            default:
            {
               char  t[80];
               sprintf(t,"Unrecognized type [%d].",atoi(lp));
               ErrorMessage(t, "Unknown Aperture Type", MB_ICONEXCLAMATION | MB_OK);
            }
            break;
         }
      }
   }
   return 1;
}

/****************************************************************************/
/*
*/
/****************************************************************************/
