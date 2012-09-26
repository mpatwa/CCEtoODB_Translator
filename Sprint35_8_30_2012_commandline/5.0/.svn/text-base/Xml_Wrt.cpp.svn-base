// $Header: /CAMCAD/5.0/Xml_Wrt.cpp 110   6/17/07 8:54p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

#include "stdafx.h"
#include <math.h>
#include "CCEtoODB.h"
#include "ccdoc.h"
#include "attrib.h"
#include "drc.h"
#include "WriteFormat.h"
#include "RwLib.h"
#include "DftFacade.h"
#include "EntityNumber.h"
#include "dft.h"
#include "Variant.h"
#include "MultipleMachine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CString indent;
static int indentCount;

static void indentPlus();
static void indentMinus();
static int GetWholenumCnt(double number);
const int output_units_accuracy = 6;
void WriteXML(FILE *stream, CCEtoODBDoc *doc);
void WriteXML(CWriteFormat& writeFormat, CCEtoODBDoc *doc);
void WriteRedLineXML(CWriteFormat& writeFormat);

/******************************************************************************
* SwapSpecialCharacters
*/
CString SwapSpecialCharacters(CString string)
{
   CString tempString;
   tempString = string;

   // XML Reserves Chars
   tempString.Replace("&", "&amp;");
   tempString.Replace("\"", "&quot;");
   tempString.Replace("'", "&apos;");
   tempString.Replace("<", "&lt;");
   tempString.Replace(">", "&gt;");
   for (int i=0; i<tempString.GetLength(); i++)
   {
      unsigned char ch = tempString.GetAt(i);
      // We shall continue to make exceptions for tab, LF, and CR, becasue visECAD may not read the file properly
      // if we do not, and because there is already a precedent set for converting these to C-style escape
      // sequences in code specific to T_TEXT and Attribute Values, i.e. use "\n", and visECAD already supports
      // that too. It was not a good choice, it should have been done here in the normal XML style.
      // But the precedent is set, it requires more widespread change to convert to XML style
      // than to simply conform to the precedent they already set.
      // So don't convert tab (9), lf (10), or cr (13) here.
      if (ch < 32 && ch != 9 && ch != 10 && ch != 13)
      {
         CString replacement;
         replacement.Format("&#%d;", ch);
         tempString.Delete(i);
         tempString.Insert(i, replacement);
      }
   }
/*
   // UNICODE
   for (int i=0; i<tempString.GetLength(); i++)
   {
      unsigned char ch = tempString.GetAt(i);
      if (ch > 127)
      {
         CString replacement;
         replacement.Format("&#x%.04X;", ch);

//char tc[1];
//char *chPtr = tempString.GetBuffer(0) + i;
//OemToCharBuff(chPtr, tc, 1);  
//replacement.Format("&#x%.04X;", *(unsigned char*)tc);
//       ErrorMessage(tempString, string);

         tempString.Delete(i);
         tempString.Insert(i, replacement);
         i += 7;
      }
   }

//UTF-8
*/

   return tempString;
}
