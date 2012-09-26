// $Header: /CAMCAD/4.5/read_wrt/ApWrite.cpp 12    4/30/06 12:22p Kurt Van Ness $

/*****************************************************************************/
/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-99. All Rights Reserved.
*/           

#include "stdafx.h"
#include "ccdoc.h"
#include "CCEtoODB.h"


void WriteGAP1(CCEtoODBDoc *doc, const char *FileName, int pageUnits)
{
   FILE *stream;
   char buf[256];

/* getdate(&d);
   gettime(&t);*/

   if ((stream = fopen(FileName, "wt")) == NULL)
      ErrorMessage("Unable To Open File", FileName, MB_ICONEXCLAMATION | MB_OK);
   else
   {
      int i,
         default_DCode = 10;
      /* default dcodes */
      char used[1000]; // array of TRUE or FALSE for each index(dcode)
      for (i=0; i<1000; i++)
         used[i] = FALSE; // set everything to not used

      // set which dcodes have been used
      for (i=0; i<doc->getNextWidthIndex(); i++)
      {
         BlockStruct *b = doc->getWidthTable()[i];
         if (doc->getWidthTable()[i])
         {
            if (doc->getWidthTable()[i]->getDcode() < 0 || doc->getWidthTable()[i]->getDcode() > 999)   continue;
            if (used[doc->getWidthTable()[i]->getDcode()]) // if already used
               doc->getWidthTable()[i]->setDcode(0); // so it will get set to an unused dcode
            else
               used[doc->getWidthTable()[i]->getDcode()] = TRUE; // set as used
         }
      }

      for (i=0; i<doc->getNextWidthIndex(); i++)
         if (doc->getWidthTable()[i])
            if (doc->getWidthTable()[i]->getDcode() == 0)
            {
               // find next unused dcode
               while (default_DCode < 999 && used[default_DCode])
                  default_DCode++;
               doc->getWidthTable()[i]->setDcode(default_DCode++);
            }

      // write GAP file
      fprintf(stream,"! GAPFile Version 1.0\n");
      fprintf(stream,"! Created by %s\n", getApp().getCamCadTitle());
      fprintf(stream,"GAPFile    Version 1.0\n");
      fputs("!     Type            Width          Length     Rotate   Offset      Swap  Tool      Size      Legend\n",stream);
      fputs("!                English  Metric English  Metric     English  Metric Code  No. English  Metric\n",stream);
      fputs("!-----------------------+---------------+-------------------+-------------------------+--------------\n",stream);


      BlockStruct *b;
      for (i=0; i<doc->getNextWidthIndex(); i++)
      {
         if ((b = doc->getWidthTable()[i]) == NULL) continue;

         sprintf(buf, "D%-3d  ", b->getDcode());
         fputs(buf, stream);
         switch (b->getShape())
         {
            case T_ROUND:
               sprintf(buf, "%-10.10s ", "ROUND");
            break;
            case T_SQUARE:
               sprintf(buf, "%-10.10s ", "SQUARE");
            break;
            case T_TARGET:
               sprintf(buf, "%-10.10s ", "TARGET");
            break;
            case T_THERMAL:
               sprintf(buf, "%-10.10s ", "THERMAL");
            break;
            case T_COMPLEX:
               sprintf(buf, "%-10.10s ", "CUSTOM");
            break;
            case T_OCTAGON:
               sprintf(buf, "%-10.10s ", "OCTAGON");
            break;
            case T_RECTANGLE:
               sprintf(buf, "%-10.10s ", "RECTANGLE");
            break;
            case T_DONUT:
               sprintf(buf, "%-10.10s ", "DONUT");
            break;
            case T_OBLONG:
               sprintf(buf, "%-10.10s ", "OBLONG");
            break;
            case T_UNDEFINED:
            default:
               sprintf(buf, "%-10.10s ", "UNDEFINED");
            break;
         }
         fputs(buf,stream);

         switch (b->getShape())
         {
            case T_ROUND:
            case T_SQUARE:
            case T_TARGET:
            case T_COMPLEX:
            case T_OCTAGON:
               sprintf(buf,"%7.2lf  %6.3lf %7.2lf  %6.3lf ",
                  b->getSizeA() * Units_Factor(pageUnits, UNIT_MILS),
                  b->getSizeA() * Units_Factor(pageUnits, UNIT_MM),
                  0.0, 0.0); // Length English/Metric
               break;
            case T_RECTANGLE:
            case T_DONUT:
            case T_THERMAL:
            case T_OBLONG:
            case T_UNDEFINED:
            default:
               sprintf(buf,"%7.2lf  %6.3lf %7.2lf  %6.3lf ",
                  b->getSizeA() * Units_Factor(pageUnits, UNIT_MILS),
                  b->getSizeA() * Units_Factor(pageUnits, UNIT_MM),
                  b->getSizeB() * Units_Factor(pageUnits, UNIT_MILS),
                  b->getSizeB() * Units_Factor(pageUnits, UNIT_MM));
               break;
         }
         fputs(buf,stream);
         sprintf(buf,"%3d %7.2lf  %6.3lf  ",
            0, 0.0, 0.0); // Rotation, Offset English/Metric
         fputs(buf,stream);
         sprintf(buf,"D%-3d %3d ",
            b->getDcode(), 0); // SwapCode, Tool
         fputs(buf,stream);
         sprintf(buf,"%7.2lf  %6.3lf  ", 0.0, 0.0); // Size English/Metric
         fputs(buf,stream);
         strcpy(buf,"D0 \n"); // Legend
         fputs(buf,stream);
      }
      fclose(stream);
   }  // end else
   return;
}  // end WriteGAP(-)

// end APWRITE.CPP