// $Header: /CAMCAD/5.0/Font.cpp 18    12/10/06 3:55p Kurt Van Ness $

#include "stdafx.h"
#include "CCEtoODB.h"
#include "font.h"
#include "graph.h"
#include "polylib.h"

 
//_____________________________________________________________________________
void CCEtoODBApp::OnLoadFont() 
{
   CFileDialog FileDialog(TRUE, "FNT", "*.FNT",
         OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, 
         "CAMCAD Font File (*.FNT)|*.FNT|All Files (*.*)|*.*||", NULL);
   if (FileDialog.DoModal() != IDOK) return;

   CFontList::getFontList().glyphFont(FileDialog.GetPathName());
}

// end FONT.CPP
