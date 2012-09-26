// $Header: /CAMCAD/4.6/Aperture.cpp 11    2/14/07 3:59p Kurt Van Ness $

/*****************************************************************************/
/*  
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-98. All Rights Reserved.
*/           

#include "stdafx.h"
#include "dbutil.h"
#include "aperture.h"
#include "apread.h"
#include "graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//double lastGerberScale = 1.0;

char Gprefix[MAXPREFIX+1];
AperStruct *GHead =NULL; // Global pointer to head of List
 
static int get_db_type(int typ);

/*******************************************************************************
* UniversalApertureRead()
*/
AperStruct *UniversalApertureRead(char *macroFile, const char *apertureFile, 
                                  char *logFile, int *apUnits, char *macName)
{
   GHead = NULL;    
   *apUnits = UNIT_INCHES; // default units
   apread(macroFile, apertureFile, logFile, apUnits, macName);
   return GHead;
}

/*******************************************************************************
* output_app() - called by universal aperture reader for each aperture read
*/
int output_app(Apdef *apdef)
{
   AperStruct *cur;
   static AperStruct *prev;

   cur = (AperStruct *)calloc(1, sizeof(AperStruct));

   if (GHead)
      prev->next = cur;
   else
      GHead = cur;

   prev = cur;

   sprintf(cur->Name, "%sD%d", Gprefix, apdef->dcode);
   cur->DCode = apdef->dcode;
   cur->Type = get_db_type(apdef->apptype);
   cur->Size_a = apdef->xsize;
   cur->Size_b = apdef->ysize;
   cur->Offset_x = apdef->xoffset;
   cur->Offset_y = apdef->yoffset;
   cur->Rotation = apdef->rotation;

   return TRUE;
}

/****************************************************************************/
/*
   FORM_ROUND        6
   FORM_SQUARE       7
   FORM_RECT         8
   FORM_OBLONG       9
   FORM_THERM        10
   FORM_DONUT        11
   FORM_TARGET       12
   FORM_OCTAGON      13
   FORM_BLANK        14
   FORM_COMPLEX      15
*/
int get_db_type(int typ)
{
   switch (typ)
   {
      case 6:
         return T_ROUND;
      case 7:
         return T_SQUARE;
      case 8:
         return T_RECTANGLE;
      case 9:
         return T_OBLONG;
      case 10:
         return T_THERMAL;
      case 11:
         return T_DONUT;
      case 12:
         return T_TARGET;
      case 13:
         return T_OCTAGON;
      case 14:
         return T_BLANK;
      case 15:
         return T_COMPLEX;
      default:
         return T_UNDEFINED;
   }
}

/*******************************************************************************
* ap_list
*   traverses aperature list creating blocks in database
*/
void ap_list(char *prefix, AperStruct *Head, int pageUnits, int apUnits)
{
   AperStruct *Curr;
   char fullName[80];
   DbFlag flg;
   Curr = Head;
   int   dcode;
   int   err;

   while (Curr != NULL)
   {
      strcpy(fullName,prefix); // block name is 'prefix' followed by dcode
      strcat(fullName,Curr->Name);
      flg = BL_APERTURE;
      sscanf(Curr->Name, "D%d", &dcode);

      // need rectangle to correctly draw the Gerber square
      if (Curr->Type == T_SQUARE)
      {
         Curr->Type = T_RECTANGLE;
         Curr->Size_b = Curr->Size_a;
      }

      Graph_Aperture(fullName, Curr->Type,
            Units_Factor(apUnits, pageUnits) * Curr->Size_a,
            Units_Factor(apUnits, pageUnits) * Curr->Size_b,
            Units_Factor(apUnits, pageUnits) * Curr->Offset_x,
            Units_Factor(apUnits, pageUnits) * Curr->Offset_y,
            DegToRad(Curr->Rotation), dcode, flg, TRUE, &err);
      Curr = Curr->next;
   }
}

/***************************************************************************/
void free_list(AperStruct *Head)
{
   struct AperStruct *Curr;
   struct AperStruct *Next;

   Curr = Head;

   while (Curr != NULL)
   {
      Next = Curr->next;
      if (Curr)   free(Curr);
      Curr = Next;
   }

   return ;
}

// APERTURE.CPP
