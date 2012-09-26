// $Header: /CAMCAD/4.6/Menu.cpp 12    5/25/07 12:48a Rick Faltersack $

/*
Project CAMCAD
Router Solutions Inc.
Copyright ©  1994-6. All Rights Reserved.
*/

#include "stdafx.h"
#include "mainfrm.h"
#include "lic.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"


void SaveMenu(CMenu *menu, FILE *stream);

void SetProductHelpMenuItem()
{
   // Locate the help submenu
   CMenu* helpMenu = NULL;
   CMenu* topMenu = AfxGetMainWnd()->GetMenu();

   for (int iPos = topMenu->GetMenuItemCount()-1; iPos >= 0; iPos--)
   {
      CMenu* menu = topMenu->GetSubMenu(iPos);
      if (menu && menu->GetMenuItemID(0) == ID_HELP_INDEX)
      {     
         helpMenu = menu;
         break;
      }
   }
   ASSERT(helpMenu != NULL);

   CString text;
   switch (Product)
   {
      case PRODUCT_GRAPHIC:
         text = "&CAMCAD Graphic Help";
      break;
      case PRODUCT_VISION:
         text = "&CAMCAD Vision Help";
      break;
      case PRODUCT_PROFESSIONAL:
         text = "&CAMCAD Professional Help";
      break;
      case PRODUCT_PCB_TRANSLATOR:
         text = "&CAMCAD PCB Translator Help";
      break;
   }                 

   helpMenu->InsertMenu(1, MF_BYPOSITION, ID_HELP_PRODUCT, text);
}

void CMainFrame::OnSaveMenuText() 
{
   CMenu menu;
   FILE *stream;
   CString file = getApp().getUserPath();
   file += "english.txt";
   stream = fopen(file, "w+");

   fprintf(stream, "! CAMCAD Menu Language File\n");
   fprintf(stream, "!\n! CAMCAD Main Menu\n");
   menu.LoadMenu(IDR_CAMCADTYPE);
   SaveMenu(&menu, stream);
   menu.DestroyMenu();
   fprintf(stream, "! End Main Menu\n");
   fprintf(stream, "!-----------------------------------\n"); // separator
   fprintf(stream, "!\n! CAMCAD Frame Menu\n");
   menu.LoadMenu(IDR_MAINFRAME);
   SaveMenu(&menu, stream);
   menu.DestroyMenu();
   fprintf(stream, "! End Frame Menu\n");
   fprintf(stream, "!-----------------------------------\n"); // separator
   fclose(stream);

   ErrorMessage(file, "English Menu Language File saved.");
}

void SaveMenu(CMenu *menu, FILE *stream)
{
   CMenu *sub;
   CString text;
   int menuCount = menu->GetMenuItemCount(),
      id;
   for (int i=0; i < menuCount; i++)
   {
      menu->GetMenuString(i, text, MF_BYPOSITION);
      if (sub = menu->GetSubMenu(i)) // popup
      {
         fprintf(stream, "%d %s\n", i+1, text);
         SaveMenu(sub, stream);
      }
      else
      {
         id = menu->GetMenuItemID(i);
         if (id)
            fprintf(stream, "%u %s\n", id, text);
         else
            fprintf(stream, "!------------------\n"); // separator
      }
   }
   fprintf(stream, "! End Popup\n");
   fprintf(stream, "!******************\n"); // separator
}

void CMainFrame::OnLoadMenuText() 
{
   // restore window (problems with menu if maximized)
   BOOL b;
   CMDIChildWnd *activeChild = MDIGetActive(&b);
   if (b) // child is maximized
      activeChild->ShowWindow(SW_RESTORE);

   CFileDialog FileDialog(TRUE, "TXT", "*.txt",
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "Text File (*.txt)|*.TXT|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() == IDOK) 
      LoadMenuText(FileDialog.GetPathName());
}

static void ReportLastModifyMenuError(CString parentMenuName, int menuItemId, CString menuItemText, CStringArray &msgLog)
{
   DWORD sysErrCode = GetLastError();
   CString sysErrMsg;

   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, sysErrCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), sysErrMsg.GetBufferSetLength(256), 256, 0);

   sysErrMsg.ReleaseBuffer();

   CString parentMenuErrMsg;
   if (!parentMenuName.IsEmpty())
      parentMenuErrMsg.Format("Looking in menu \"%s\".\n", parentMenuName);

   CString localErrMsg;
   localErrMsg.Format("Looking for command code (%d), to assign option text (%s)\n", menuItemId, menuItemText);

   CString fullMsg = "Modify Menu Failed\n" + parentMenuErrMsg + localErrMsg + "System Error Message: " + sysErrMsg;

   msgLog.Add(fullMsg);
}

static void LoadSubMenu(CMenu *menu, FILE *stream, int level, CStringArray &msgLog)
{
   char line[121],
      *KeyWord;
   UINT id;

	int menuOffset = 1;
	CString menuStr;
	menu->GetMenuString(0, menuStr, MF_BYPOSITION);
	if (menuStr.IsEmpty())
		menuOffset = 0;

   while (fgets(line,120,stream)!=NULL)
   {
      if (level && !STRICMP(line,"! End Popup\n")) // if at end of submenu
         return;

      if (!STRICMP(line,"! End Main Menu\n") ||
            !STRICMP(line,"! End Frame Menu\n"))
            
         return;

      if (line[0] == '!') continue;
      KeyWord = strtok(line," \t");
      sscanf(KeyWord,"%u", &id);
      if (id)
      {
         KeyWord = strtok(NULL,"\n");
         if (id < 99)
         {
            if (!menu->ModifyMenu(id-menuOffset, MF_BYPOSITION | MF_STRING, 0, KeyWord))
            {
               ReportLastModifyMenuError(menuStr, id, KeyWord, msgLog);
            }

            CMenu *submenu = menu->GetSubMenu(id-menuOffset);
            if (submenu != NULL) // popup
               LoadSubMenu(submenu, stream, level+1, msgLog);
         }
         else
         {
            if (!menu->ModifyMenu(id, MF_BYCOMMAND | MF_STRING, id, KeyWord))
            {
               ReportLastModifyMenuError(menuStr, id, KeyWord, msgLog);
            }
         }
      }
   }
}

void CMainFrame::LoadMenuText(CString file) 
{
   CMenu *menu = GetMenu();
   FILE *stream;
   char line[121];

   stream = fopen(file, "r");
   if (stream == NULL)
      return;

   fgets(line,120,stream);

   if (STRICMP(line,"! CAMCAD Menu Language File\n"))
      return;

   CStringArray msgLog;

   LoadSubMenu(menu, stream, 0, msgLog);

   menu = menu->FromHandle(m_hMenuDefault);
   LoadSubMenu(menu, stream, 0, msgLog);

   fclose(stream);

   if (msgLog.GetCount() > 0)
   {
      CString logFilePath = GetLogfilePath("LoadMenuFile.log");
   
      CFormatStdioFile logFile;
      CFileException e;

      BOOL hasLogFile = logFile.Open(logFilePath, CFile::modeCreate|CFile::modeWrite, &e);

      if (hasLogFile)
      {
         logFile.WriteString("CAMCAD Load Menu Text Log File\n\n");
         for (int i = 0; i < msgLog.GetCount(); i++)
         {
            logFile.WriteString("%s\n", msgLog.GetAt(i));
         }

         logFile.Close();
      }

      // Put first couple messages in pop up, tell user rest is in log file
      CString msgBuf;
      msgBuf = "\n" + msgLog.GetAt(0);

      if (msgLog.GetCount() > 1)
         msgBuf += "\n" + msgLog.GetAt(1);

      if (msgLog.GetCount() > 2 && hasLogFile)
         msgBuf += "\nSee log file (" + logFilePath + ") for additional error messages.";

      ErrorMessage(msgBuf, "Load Menu File Encountered Errors");


   }

   DrawMenuBar();
}