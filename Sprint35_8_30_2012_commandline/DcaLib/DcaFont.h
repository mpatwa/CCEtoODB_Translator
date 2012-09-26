// $Header: /CAMCAD/5.0/Dca/DcaFont.h 5     3/12/07 12:47p Kurt Van Ness $

#if !defined(__DcaFont_h__)
#define __DcaFont_h__

#pragma once

#include "DcaContainer.h"
#include "DcaPoly.h"

#define MAX_CHARS    256
#define SPACE 0.25 // size of blank

class CPolyList;

//_____________________________________________________________________________

// FontStruct and CFontList are about CAMCAD stroke fonts, as found in *.fnt in the CAMCAD install

class FontStruct
{
private:
   static int m_nextFontNumber;

   CString m_fontName;
   short m_fontNumber;
   CTypedPtrArrayContainer<CPolyList*> m_characterData;
   double m_characterWidth[MAX_CHARS]; // calculated from data

public:
   FontStruct();
   ~FontStruct();

   int getFontNumber() { return m_fontNumber; }

   CString getFontName() { return m_fontName; }
   void setFontName(const CString& fontName) { m_fontName = fontName; }

   double getCharacterWidth(int characterIndex) { return m_characterWidth[characterIndex]; }
   void setCharacterWidth(int characterIndex,double width) { m_characterWidth[characterIndex] = width; }

   CPolyList* getCharacterData(int characterIndex);
   void setCharacterData(int characterIndex,CPolyList* data);

};

//_____________________________________________________________________________

class CFontList
{
private:
   static CFontList* m_theFontList;
   static bool m_fontIsLoaded;
   CTypedPtrListContainer<FontStruct*> m_fontList;

public:
   static void initFontList(CString fontFilePath);
   static CFontList& getFontList();
   static bool fontIsLoaded() { return m_fontIsLoaded; }
   static void empty();

   void glyphFont(const char *fileName);
   FontStruct* getFont(int fontNumber);
   FontStruct* getFirstFont();
   POSITION GetHeadPosition() const { return m_fontList.GetHeadPosition(); }
   FontStruct* GetNext(POSITION& pos) const { return m_fontList.GetNext(pos); }
};

//##############################################################################

// This is for the extended font capabilities coming about due to Auto Ative and
// in general the more elaborate font usage that is growing in popularity.
// E.g. True Type fonts.

// As of this writing, we are only capturing and preserving settings made
// by way of the Expedition CCZ AddIn.

// This is the structure that holds those settings.
// I wanted to call it FontSpec, but a search on the Web shows these are commonly
// referred to as font properties. So be it, then we shall call this CFontProperties.

// Currently this is only used to read ccz, retain data, and write ccz.
// So all values are simple stored and accessed as string, no interpretation
// necessary or applied. Remember, nothing golden here, this is just the
// CAMCAD internal representation, we can change it to anything we like
// when it becomes convenient to do so.

class CFontProperties
{
private:                     // Example setting from DR that prompted this implementation
   CString m_facename;       // ="VeriBest Gerber 0" 
   CString m_fonttype;       // ="2" 
   CString m_weight;         // ="400" 
   CString m_underlined;     // ="0" 
   CString m_strikeout;      // ="0" 
   CString m_orientation;    // ="0" 
   CString m_charset;        // ="0"
   CString m_pitchAndFamily; // ="0"

public:
   CFontProperties();
   CFontProperties(CString facename, CString fonttype, CString weight, CString underlined, CString strikeout, 
      CString orientation, CString charset, CString pitchandfamily);

   CFontProperties &operator=(const CFontProperties &other);
   CFontProperties &operator=(const CFontProperties *other);

   CString GetFaceName() const        { return m_facename;       } 
   CString GetFontType() const        { return m_fonttype;       }
   CString GetWeight()   const        { return m_weight;         }
   CString GetUnderlined() const      { return m_underlined;     }
   CString GetStrikeOut()  const      { return m_strikeout;      } 
   CString GetOrientation() const     { return m_orientation;    }
   CString GetCharSet()     const     { return m_charset;        }
   CString GetPitchAndFamily() const  { return m_pitchAndFamily; }

   void SetFaceName(CString val)         { m_facename = val;       } 
   void SetFontType(CString val)         { m_fonttype = val;       }
   void SetWeight(CString val)           { m_weight = val;         }
   void SetUnderlined(CString val)       { m_underlined = val;     }
   void SetStrikeOut(CString val)        { m_strikeout = val;      } 
   void SetOrientation(CString val)      { m_orientation = val;    }
   void SetCharSet(CString val)          { m_charset = val;        }
   void SetPitchAndFamily(CString val)   { m_pitchAndFamily = val; }

};

#endif
