// $Header: /CAMCAD/5.0/General.h 21    12/10/06 3:55p Kurt Van Ness $

/****************************************************************************/
/*
   Project CAMCAD                                     
   Router Solutions Inc.
   Copyright © 1994-99. All Rights Reserved.

*/    

#if !defined(__General_H__)
#define __General_H__

#pragma once

#include "Dca.h"

#define STRLEN(S) ( S ? strlen(S) : 0 )

enum PageUnitsTag;

//void     MemErrorMessage(char *file, int line);
//int      ErrorMessage(const char *text, const char *caption = "ERROR", UINT type = MB_OK);
void     ErrorAccess(const char *caption);
void     Notepad(const char *file);
void     Logreader(const char *file);
int      ExecAndWait(CString commandLine, int showWindow, DWORD *exitCode);

int      FlushQueue();

// UNITS //
double   Units_Factor(int fromUnits, int toUnits);
double   getUnitsFactor(PageUnitsTag fromUnits,PageUnitsTag toUnits);
int      GetDecimals(int i);
void     SetDecimals(int i, int decimals);
const char* GetUnitName(int i);
int      GetUnitIndex(const char *nm);

// convert a string to .CSV compatible
CString  csv_string(CString string);
CString csv_string(CString string, bool option_quoteAll);

void     Window_Shape(int width, int height, long *unitsX, long *unitsY, int maxXCoord, int maxYCoord);

// this algorithm does
// A1..A10 AA1...AA10 AB1..AB10 B1..B10
//int    compare_name_old(const char *n1, const char *n2);

// this on does 
// A1 .. A10 B1 ... B10 AA1.. AA10 
int      compare_name(CString n1, CString n2);

int      wildcard_compare(const char *p1, const char *p2, int nocase);

char     *CpyStr(char *Dest, const char *Source, int MaxDest);

int      MakeDirectory(CString newDirectory);

int      round(double r);

void swap(int& a,int& b);
void swap(double& a,double& b);

int      is_number(const char *);  // checks is string is a number

void     clean_blank(char *l);
void     to_upper(char *l);
int      wildcmp(const char *wild, const char *string);

#endif
