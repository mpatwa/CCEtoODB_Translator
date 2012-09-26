// $Header: /CAMCAD/5.0/Dca/DcaFont.cpp 5     3/19/07 4:33a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaFont.h"
#include "DcaLib.h"
#include "Dca.h"

//_____________________________________________________________________________
int FontStruct::m_nextFontNumber = 0;

FontStruct::FontStruct()
{
   m_fontNumber = m_nextFontNumber++;
}

FontStruct::~FontStruct()
{
}

CPolyList* FontStruct::getCharacterData(int characterIndex) 
{ 
   return m_characterData.GetAt(characterIndex); 
}

void FontStruct::setCharacterData(int characterIndex,CPolyList* data) 
{ 
   m_characterData.setAtGrow(characterIndex,data); 
}

//_____________________________________________________________________________
CFontList* CFontList::m_theFontList = NULL;
bool CFontList::m_fontIsLoaded = false;

//CFontList::CFontList()
//{
//}


void CFontList::initFontList(CString fontFilePath)
{
   empty();

   m_theFontList = new CFontList;
   m_theFontList->glyphFont(fontFilePath);
}

void CFontList::empty()
{
   delete m_theFontList;
   m_theFontList  = NULL;
   m_fontIsLoaded = false;
}

CFontList& CFontList::getFontList()
{
   if (m_theFontList == NULL)
   {
      // Can't get the correct font file path from here, that is an app setting and this lib is
      // supposed to be app-independant. So at least make it not crash by making the font list non-null.
      // This will cause the glyphFont reader to report file-not-found.
      // If the initFontList() is done correctly to begin with, this here should never happen.
      initFontList("CFontList_getFontList_Error.fnt"); 
   }

   return *m_theFontList;
}

FontStruct* CFontList::getFont(int fontNumber)
{
   FontStruct* font = NULL;

   for (POSITION pos = m_fontList.GetTailPosition();pos != NULL;)
   {
      font = m_fontList.GetPrev(pos);

      if (font->getFontNumber() == fontNumber)
      {
         break;
      }
   }

   return font;
}

FontStruct* CFontList::getFirstFont()
{
   FontStruct* font = NULL;

   if (m_fontList.GetCount() > 0)
   {
      font = m_fontList.GetHead();
   }

   return font;
}

// creates letters out of text file description of lines
void CFontList::glyphFont(const char *fileName)
{
   CStdioFile file;

   if (!file.Open(fileName, file.modeRead | file.typeText))
   {
      ErrorMessage(fileName, "Unable to Open File!", MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // Check File Format
   CString line;
   file.ReadString(line);

   if (line.Compare("; CAMCAD Font File Version 2"))
   {
      ErrorMessage("This is Not a CAMCAD Font File Version 2\nSee \"http://www.camcad.com/cc_util.html\" for font files or FontGen.exe.", fileName, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   const double scale = 1.0/8.0; // makes a line from 0 to 8 equal 1 in height
   FontStruct *fontStruct = new FontStruct;
   fontStruct->setFontName(fileName);

   for (int i=0; i<MAX_CHARS; i++)
   {
      fontStruct->setCharacterData(i,NULL);
      fontStruct->setCharacterWidth(i,0);
   }

   m_fontList.AddTail(fontStruct);
   CPolyList* polyList = NULL;

   while (file.ReadString(line))
   {
      line.TrimLeft();
      line.TrimRight();

      if (line.IsEmpty())
         continue;

      char c = line[0];
      static int ascii;
      static double maxX;
      static double minX;
      BOOL Filled = FALSE, Void = FALSE;

      switch (c)
      {
      case ';':
         continue;

      case '[':
         {
            m_fontIsLoaded = true;

            int i = line.Find(']', 0);
            ascii = atoi(line.Mid(1, i-1));
            
            polyList = new CPolyList();
            fontStruct->setCharacterData(ascii,polyList);

            maxX = 0;
				minX = 0;
            continue;
         }

      case 'p':
         break;

      case 'f':
         Filled = TRUE;
         break;

      case 'v':
         Filled = TRUE;
         Void = TRUE;
         break;

      default:
         continue;
      }

      CPoly *poly = new CPoly;
      poly->setFilled(Filled);
      poly->setClosed(Filled);
      poly->setVoid(Void);
      polyList->AddTail(poly);

      while (!line.IsEmpty())
      {
         int left, comma, right;

         left = line.Find('(');
         comma = line.Find(',');
         right = line.Find(')');

         if (left == -1 || comma == -1 || right == -1)
            break;

         CPnt *pnt = new CPnt;
         pnt->x = (DbUnit)(atof(line.Mid(left+1, comma-left)) * scale);
         pnt->y = (DbUnit)(atof(line.Mid(comma+1, right-comma)) * scale);
         pnt->bulge = 0;
         poly->getPntList().AddTail(pnt);

         if (pnt->x > maxX)
            maxX = pnt->x;
         if (pnt->x < minX)
            minX = pnt->x;

         line = line.Mid(right+1);
      }

      fontStruct->setCharacterWidth(ascii,maxX - minX);
   }

   file.Close();
}

//###################################################################################

CFontProperties::CFontProperties()
{
   // All is blank already
}

CFontProperties::CFontProperties(CString facename, CString fonttype, CString weight, CString underlined, CString strikeout, 
      CString orientation, CString charset, CString pitchandfamily)
: m_facename(facename) 
, m_fonttype(fonttype) 
, m_weight(weight)
, m_underlined(underlined)
, m_strikeout(strikeout)
, m_orientation(orientation)
, m_charset(charset)
, m_pitchAndFamily(pitchandfamily)
{
}

CFontProperties& CFontProperties::operator=(const CFontProperties &other)
{
   if (&other != NULL)
   {
      m_facename = other.m_facename;
      m_fonttype = other.m_fonttype;
      m_weight = other.m_weight;
      m_underlined = other.m_underlined;
      m_strikeout = other.m_strikeout;
      m_orientation = other.m_orientation;
      m_charset = other.m_charset;
      m_pitchAndFamily = other.m_pitchAndFamily;
   }

   return *this;
}

CFontProperties& CFontProperties::operator=(const CFontProperties *other)
{
   if (other != NULL)
   {
      m_facename = other->m_facename;
      m_fonttype = other->m_fonttype;
      m_weight = other->m_weight;
      m_underlined = other->m_underlined;
      m_strikeout = other->m_strikeout;
      m_orientation = other->m_orientation;
      m_charset = other->m_charset;
      m_pitchAndFamily = other->m_pitchAndFamily;
   }
   else
   {
      // Just clear it
      m_facename.Empty();
      m_fonttype.Empty();
      m_weight.Empty();
      m_underlined.Empty();
      m_strikeout.Empty();
      m_orientation.Empty();
      m_charset.Empty();
      m_pitchAndFamily.Empty();
   }

   return *this;
}

