// $Header: /CAMCAD/4.5/General.cpp 24    11/18/05 6:06p Kurt Van Ness $

#include "StdAfx.h"
#include "General.h"
#include <direct.h>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern   CString  NotePadProgram;         // from CAMCAD.CPP
extern   CString  LogReaderProgram;       // from CAMCAD.CPP

/*******************************************************************************
* FlushQueue()
*  - pumps message queue being careful not to kill WM_QUIT message
*/
int FlushQueue()
{
   MSG msg;

   BOOL LButtonUp = FALSE;

   while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
   {
      if (msg.message == WM_LBUTTONUP)
      {
         LButtonUp = TRUE;
         continue;
      }

      if (msg.message == WM_QUIT)
         PostQuitMessage(msg.wParam);

      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }

   if (LButtonUp)
      PostMessage(NULL, WM_LBUTTONUP, 0, 0);

   return 0;
}

/******************************************************************************
* csv_string
   converts to a correct CSV string.
*/
CString csv_string(CString string)
{
   string.Replace('\n', '|');

   string.Replace("\"", "\"\"");

   if (string.FindOneOf(" \t.,") != -1)
      string.Format("\"%s\"", string);

   return string;
}

/******************************************************************************
* csv_string
   converts to a correct CSV string based on option_quoteAll.
*/
CString csv_string(CString string, bool option_quoteAll)
{
   CString ext_string;
   if(option_quoteAll) //Quote all fields
   {
      string.Replace('\n', '|');
      string.Replace("\"", "\"\"");
      ext_string.Format("\"%s\"", string);
   }
   else
   {
      ext_string = csv_string(string);
   }
   return ext_string;
}

/*******************************************************************************
* ExecAndWait()
*/
int ExecAndWait(CString commandLine, int showWindow, DWORD *exitCode)
{
   int CMD = FALSE;  // debug

   if (CMD)
      MessageBox(NULL, commandLine, "Cmmand Line", MB_OK | MB_ICONEXCLAMATION);

   STARTUPINFO startupInfo;
   PROCESS_INFORMATION processInfo;
   CWaitCursor w;

   memset(&startupInfo, 0, sizeof(startupInfo));
   startupInfo.cb = sizeof(startupInfo);
   startupInfo.dwFlags = STARTF_USESHOWWINDOW;
   startupInfo.wShowWindow = showWindow;

   // CreateProcess
   if (!CreateProcess(NULL, commandLine.GetBuffer(0), NULL, NULL, TRUE, 0, NULL, NULL, &startupInfo, &processInfo))
   {
      if (CMD)
         MessageBox(NULL, "Create Error", "Cmmand Line", MB_OK | MB_ICONEXCLAMATION);

      return GetLastError();
   }
   CloseHandle(processInfo.hProcess);

   // can pass NULL as exitCode
   DWORD ec;
   DWORD *lpExitCode = exitCode;
   if (!lpExitCode)
      lpExitCode = &ec;

   while (TRUE)
   {
      GetExitCodeThread(processInfo.hThread, lpExitCode);

      if (*lpExitCode != STILL_ACTIVE)
         break;

      Sleep(100);
   }

   if (CMD)
   {
      CString tmp;
      tmp.Format("Exit code %ld", *lpExitCode);
      MessageBox(NULL, tmp, "Cmmand Line", MB_OK | MB_ICONEXCLAMATION);
   }

   return 0;
}

/*****************************************************************************
* Window_Shape
*  - Adjust for shape of window
*  - returns unitsX and unitsY
*  - Number of pixels a window of that shape would hold at 1:1
*  - units_ is either max_Coord or more than max_Coord
*/
void Window_Shape(int width, int height, long *unitsX, long *unitsY, int maxXCoord, int maxYCoord)
{
   // if width times page ratio is larger than height,
   //   (short and wide window) YUnits will fit exactly, X will hold more than page width
   if (round(1.0 * width * maxYCoord / maxXCoord) >= height )
   {
      *unitsY = maxYCoord;
      *unitsX = (long)(1.0 * *unitsY * width / height);  // X can hold a ratio more units
   }
   else // if (tall and narrow window), XUnits will fit exactly, Y will hold more than page height
   {
      *unitsX = maxXCoord;
      *unitsY = (long)(1.0 * *unitsX * height / width);  // Y can hold a ratio more units
   }
}

/****************************************************************************/
/*
  This function compares names not using the straight strcmp function.
  Result of the strcmp function is U1 U11 U12 U2 U20 U21
  Result of compare_name is        U1 U2 U11 U12 U20 U21

   A1..A10 AA1..AA10 B1..B10 BA1..BA10

*
int compare_name_old(const char *n1, const char *n2)
{
   static char buf1[ 255 +1 ], *p1;
   static char buf2[ 255 +1 ], *p2;
   static int  cmp_name, cmp_num;

   n1 = CpyStr( buf1, n1, 255 );
   n2 = CpyStr( buf2, n2, 255 );

   // Find the first digit in each string.
   //
   for ( p1=buf1; *p1 && !isdigit(*p1); p1++ )
      ;
   for ( p2=buf2; *p2 && !isdigit(*p2); p2++ )
      ;

   // Compare the numeric portions.
   //
   if (is_number(p1) && is_number(p2))
   {
      cmp_num = atoi(p1) - atoi(p2);

      // Compare the alpha portions.
      //
      *p1 = *p2 = '\0';
   }
   cmp_name = STRICMP( n1, n2 );

   return cmp_name ? cmp_name : cmp_num;
}

/****************************************************************************/
/*
  This function compares names not using the straight strcmp function.
  Result of the strcmp function is U1 U11 U12 U2 U20 U21
  Result of compare_name is        U1 U2 U11 U12 U20 U21

   A1..A10 B1..B10 AA1..AA10 BA1..BA10

   res = 0 is equal
   res > 0 first is larger than second
   res < 0 second is larger than first

*
int compare_name(const char *n1, const char *n2)
{
   static char buf1[ 255+1 ], *p1;
   static char buf2[ 255+1 ], *p2;
   static int  cmp_name, cmp_num;

   n1 = CpyStr( buf1, n1, 255 );
   n2 = CpyStr( buf2, n2, 255 );

   // Find the first digit in each string.
   //
   for ( p1=buf1; *p1 && !isdigit(*p1); p1++ )
      ;
   for ( p2=buf2; *p2 && !isdigit(*p2); p2++ )
      ;

   // Compare the numeric portions.
   //
   if (is_number(p1) && is_number(p2))
   {
      cmp_num = atoi(p1) - atoi(p2);

      // Compare the alpha portions.
      //
      *p1 = *p2 = '\0';
   }

   CString  nn1, nn2;
   int      maxl = strlen(n1);
   if (strlen(n2) > maxl)
      maxl = strlen(n2);
   nn1.Format("%*s", maxl,n1);
   nn2.Format("%*s", maxl,n2);

   cmp_name = STRICMP( nn1, nn2 );

   return cmp_name ? cmp_name : cmp_num;
}

/****************************************************************************/
/*
  This function compares names not using the straight strcmp function.
  Result of the strcmp function is U1 U11 U12 U2 U20 U21
  Result of compare_name is        U1 U2 U11 U12 U20 U21

   A1..A10 B1..B10 AA1..AA10 BA1..BA10

   res = 0 is equal
   res > 0 first is larger than second
   res < 0 second is larger than first

*/
int compare_name(CString name1, CString name2)
{
   int i = name1.FindOneOf("0123456789"); // if n1 contains a number
   if (i != -1)
   {
      if (name2.FindOneOf("0123456789") == i) // if n2 contains a number at same place
      {
         int res = STRICMP(name1.Left(i), name2.Left(i)); // compare first string piece
         
         if (res) return res; // return result if not same

         int j1, j2;
         for (j1=i+1; j1<name1.GetLength() && isdigit(name1[j1]); j1++)
            ;
         for (j2=i+1; j2<name2.GetLength() && isdigit(name2[j2]); j2++)
            ;

         int num1 = atoi(name1.Mid(i, j1-i));
         int num2 = atoi(name2.Mid(i, j2-i));

         // compare number portion
         if (num1 - num2)
            return num1 - num2;

         // first letter portion and first number portion are same
         if (j1<name1.GetLength())
         {
            if (j2<name2.GetLength())
               return compare_name(name1.Mid(j1), name2.Mid(j2));
            else
               return 1;
         }
         else
         {
            if (j2<name2.GetLength())
               return -1;
            else
               return 0;
         }
      }
   }

   return STRICMP(name1, name2);
}

// Up to MaxDest-1 characters will be copied from Source to Dest.
// Dest is guaranteed to be null-terminated.  Return Dest.
//
char *CpyStr(char *Dest, const char *Source, int MaxDest)
{
   strncpy( Dest, Source, MaxDest-1 );

   // If Source is equal to MaxDest there also must be null termination
   // guarantied.
   //if ((int)(strlen(Source)) >= MaxDest-1)  <--- must always append a \0

   Dest[ MaxDest-1 ] = '\0';
   return Dest;
}

/*********************************************************************
* round
*  - rounds a double to nearest int
*/
int round(double r)
{ 
   return((int)floor(r + 0.5)); 
}

void swap(int& a,int& b)
{
   int tmp = a;
   a = b;
   b = tmp;
}

void swap(double& a,double& b)
{
   double tmp = a;
   a = b;
   b = tmp;
}

/*********************************************************************
* MakeDirectory
*  
*  DESCRIPTION:
*     Creates a directory (necessary because _mkdir() only creates one level)
*/
int MakeDirectory(CString newDirectory)
{ 
   char cwd[_MAX_PATH];
   _getcwd(cwd, _MAX_PATH);

   // if directory already exists
   if (!_chdir(newDirectory))
   {
      _chdir(cwd);
      return 0;
   }

   // simple mkdir
   if (!_mkdir(newDirectory))
      return 0;

   // make sure last character is a backslash so _splitpath does not think last part is a filename
   if (newDirectory.Right(1) != "\\")
      newDirectory += "\\";

   char drive[_MAX_DRIVE], dir[_MAX_DIR];
   _splitpath(newDirectory, drive, dir, NULL, NULL);

   CString tempPath;
   char *tok;

   tempPath = drive;

   tok = strtok(dir, "\\/");
   while (tok != NULL)
   {
      tempPath += "\\";
      tempPath += tok;
      _mkdir(tempPath);
      tok = strtok(NULL, "\\/");
   }

   // make sure it worked
   if (!_chdir(newDirectory))
   {
      _chdir(cwd);
      return 0;
   }

   _chdir(cwd);
   return -1;
}

/****************************************************************************/
/*
   this is also a number : 4.69328e-09
   this also returns TRUE on 28H -> which is wrong.

   sscanf on an empty token returns a -1
*/
int is_number(const char *t)
{
   double   n;
   int      res,i,ecnt;

   // check for NULL string
   if (STRLEN(t) < 1)   return 0;

   ecnt = 0;
   for (i=0;i<(int)(strlen(t));i++)
   {
      if (isalpha(t[i]))
      {
         if (t[i] == 'E' || t[i] == 'e')
         {
            ecnt = i;
         }
         else
         {
             // there is a character in the string which
             // shows that this is not a number
             return 0;
         }
      }
   }

   if (ecnt == (int)(STRLEN(t)))
   {
      // now it can not be the last char
      return 0;

   }

   // sscanf does not do it right
   res = sscanf(t,"%lf",&n);

   if (res < 0) return 0;

   return res;
}

/***************************************************************************/
void clean_blank(char *l)
{
   while (strlen(l) && isspace(l[strlen(l)-1]))   l[strlen(l)-1] = '\0';
   STRREV(l);
   while (strlen(l) && isspace(l[strlen(l)-1]))   l[strlen(l)-1] = '\0';
   STRREV(l);
   return;
}

/***************************************************************************/
void to_upper(char *l)
{
   for (int i=0; i<(int)strlen(l); i++)
   {
      if (islower(l[i]))
         l[i] = toupper(l[i]);
   }
   return;
}

/*******************************************************************************
* 
   compare p1 to p2
*/
int wildcard_compare(const char *p1, const char *p2, int nocase)
{
   CString  pp1, pp2;
   int      lp1 = strlen(p1);
   int      lp2 = strlen(p2);
   int      maxlen = min(lp1, lp2);
   int      i;

   if (!lp1)   return -1;  // do not compare if no length
   if (!lp2)   return +1;

   pp1 = "";
   pp2 = "";
   // never need to compare longer than any string
   if (strchr(p1,'*'))
   {
      for (i=0; i<maxlen; i++)
      {
         if (p1[i] == '*')
         {
            // fill with ? until maxlen
            for (unsigned int ii=i; ii<strlen(p2); ii++)
               pp1 += '?';
            break;
         }
         else
         {
            pp1 += p1[i];
         }
      }
   }
   else
      pp1 = p1;

   if (strchr(p2,'*'))
   {
      for (i=0;i<maxlen;i++)
      {
         if (p2[i] == '*')
         {
            // fill with ? until maxlen
            for (unsigned int ii=i;ii<strlen(pp1);ii++)
               pp2 += '?';
            break;
         }
         else
         {
            pp2 += p2[i];
         }
      }
   }
   else
      pp2 = p2;

   // now check ? against each other
   for (i=0; i<(int)strlen(pp1); i++)
   {
      if (pp1.GetAt(i) == '?')   
      {
         if (i < (int)strlen(pp2))
            pp2.SetAt(i, '?');
      }
   }

   for (i=0; i<(int)strlen(pp2); i++)
   {
      if (pp2.GetAt(i) == '?')   
      {
         if (i < (int)strlen(pp1))
            pp1.SetAt(i, '?');
      }
   }

   if (nocase)
   {
      pp1.MakeUpper();
      pp2.MakeUpper();
   }

   return strcmp(pp1, pp2);
}

/*******************************************************************************/
/* 
*/
/*******************************************************************************/
int wildcmp(const char *wild, const char *string)
{
	const char *cp = NULL, *mp = NULL;
	
	while ((*string) && (*wild != '*')) 
	{
		if ((*wild != *string) && (*wild != '?'))
			return 0;

		wild++;
		string++;
	}
		
	while (*string)
	{
		if (*wild == '*')
		{
			if (!*++wild)
				return 1;

			mp = wild;
			cp = string+1;
		}
		else if ((*wild == *string) || (*wild == '?'))
		{
			wild++;
			string++;
		}
		else
		{
			wild = mp;
			string = cp++;
		}
	}
		
	while (*wild == '*')
		wild++;

	return !*wild;
}

