// $Header: /CAMCAD/4.5/MruFileManager.cpp 1     8/15/06 7:13p Kurt Van Ness $

////////////////////////////////////////////////////////////////
// 1999 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
//
// Compiles with Visual C++ 5.0 or later on Windows 9x
// 
////////////////////////////////////////////////////////////////////////
// CMruFileManager - class to manage multiple recent file lists in MFC

#include "StdAfx.h"
#include "MruFileManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//_____________________________________________________________________________
IMPLEMENT_DYNAMIC(CMruFileManager, CCmdTarget)

BEGIN_MESSAGE_MAP(CMruFileManager, CCmdTarget)
   // MRU - most recently used file menu
   ON_UPDATE_COMMAND_UI_RANGE(0, 0xFFFF, OnUpdateRecentFileMenu)
   ON_COMMAND_EX_RANGE(0, 0xFFFF, OnOpenRecentFile)
END_MESSAGE_MAP()

CMruFileManager::CMruFileManager(CWinApp* pApp)
: m_pApp(pApp)
{
}

//////////////////
// Cleanup: destroy all the recent file lists
//
CMruFileManager::~CMruFileManager()
{
   for (POSITION pos = m_listRfl.GetHeadPosition();pos != NULL;)
   {
      CManagedRecentFileList* prfl = m_listRfl.GetNext(pos);
      prfl->WriteList();
   }
}

//////////////////
// Add a recent file list to the MRU manager
//
DWORD CMruFileManager::Add(UINT nBaseID,  // base command ID
   LPCTSTR lpszSection,                   // registry section (keyname)
   LPCTSTR lpszEntryFormat,               // registry value name
   MRUFILEFN pMruFn,                      // filter function
   UINT nMaxMRU,                          // max num menu entries
   BOOL bNoLoad,                          // don't load from reg (rarely used)
   int nMaxDispLen,                       // display length
   UINT nStart)                           // size to start
{
   CManagedRecentFileList* prfl = new CManagedRecentFileList(nBaseID,
      lpszSection, lpszEntryFormat, pMruFn, nMaxMRU, nMaxDispLen, nStart);

   if (!bNoLoad)
      prfl->ReadList();                 // load from registry

   m_listRfl.AddTail(prfl);             // add to my list

   return (DWORD)prfl;  
}

//////////////////
// Remove a recent file list. WARNING: never tested!!
//
BOOL CMruFileManager::Remove(DWORD dwRfl)
{
   POSITION pos = m_listRfl.Find((CManagedRecentFileList*)dwRfl);

   if (pos)
   {
      m_listRfl.RemoveAt(pos);
      return TRUE;
   }

   return FALSE;
}

void CMruFileManager::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
   CManagedRecentFileList* prfl = FindRFL(pCmdUI->m_nID);

   if (prfl)
   {
      pCmdUI->Enable(prfl->GetSize()>0);
      prfl->UpdateMenu(pCmdUI);
   }
   else
   {
      pCmdUI->ContinueRouting();
   }
}

BOOL CMruFileManager::OnOpenRecentFile(UINT nID)
{
   CManagedRecentFileList* prfl = FindRFL(nID);

   if (prfl) {

      int nIndex = nID - prfl->m_nBaseID;
      ASSERT((*prfl)[nIndex].GetLength() != 0);
      TRACE2("CMruFileManager: open file (%d) '%s'.\n", nIndex + 1,
         (LPCTSTR)(*prfl)[nIndex]);

      /*if (m_pApp->OpenDocumentFile((*prfl)[nIndex]) == NULL)
         prfl->Remove(nIndex);*/

      return TRUE;
   }

   return FALSE;
}

CManagedRecentFileList* CMruFileManager::FindRFL(UINT nID)
{
   for (POSITION pos = m_listRfl.GetHeadPosition();pos != NULL;)
   {
      CManagedRecentFileList* prfl = m_listRfl.GetNext(pos);

      if (prfl->m_nBaseID <= nID && nID < prfl->m_nBaseID + prfl->GetSize())
      {
         return prfl;
      }
   }

   return NULL;
}

extern BOOL AFXAPI AfxFullPath(LPTSTR lpszPathOut, LPCTSTR lpszFileIn);

BOOL CMruFileManager::AddToRecentFileList(LPCTSTR lpszPathName)
{
   // fully qualify the path name
   TCHAR szTemp[_MAX_PATH];
   AfxFullPath(szTemp, lpszPathName);
   
   for (POSITION pos = m_listRfl.GetHeadPosition();pos != NULL;)
   {
      CManagedRecentFileList* prfl = m_listRfl.GetNext(pos);

      if (prfl->IsMyKindOfFile(szTemp))
      {
         prfl->Add(szTemp);

         return TRUE;
      }
   }

   return FALSE;
}

//_____________________________________________________________________________
//IMPLEMENT_DYNAMIC(CManagedRecentFileList, CRecentFileList)

CManagedRecentFileList::CManagedRecentFileList(UINT nBaseID, LPCTSTR lpszSection,
   LPCTSTR lpszEntryFormat, MRUFILEFN pMruFn,
   int nMaxMRU, int nMaxDispLen, UINT nStart)
: CRecentFileList(nStart, lpszSection, lpszEntryFormat, nMaxMRU)
{
      m_nBaseID = nBaseID;
      m_pMruFn = pMruFn;
}

CManagedRecentFileList::~CManagedRecentFileList()
{
}

BOOL CManagedRecentFileList::IsMyKindOfFile(LPCTSTR lpszPathName)
{
   return m_pMruFn && (*m_pMruFn)(lpszPathName);
}

void CManagedRecentFileList::UpdateMenu(CCmdUI* pCmdUI)
{
   CMenu* pMenu = pCmdUI->m_pMenu;

   if (pCmdUI->m_pSubMenu)
      pCmdUI->m_pMenu = pCmdUI->m_pSubMenu;

   CRecentFileList::UpdateMenu(pCmdUI);

   if (pCmdUI->m_pSubMenu)
      pCmdUI->m_pMenu = pMenu;
}
 