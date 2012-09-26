// $Header: /CAMCAD/DcaLib/DcaText.h 4     3/23/07 5:19a Kurt Van Ness $

#if !defined(__DcaText_h__)
#define __DcaText_h__

#pragma once

#include "DcaPoint.h"
#include "DcaFont.h"

class CExtent;
class CPoly;

enum HorizontalPositionTag;
enum VerticalPositionTag;

//_____________________________________________________________________________
class TextStruct
{
private:
   char* m_text;

   CFontProperties *m_fontProps; // Not used in CAMCAD for display, but read/retained/written for ccz files

   char           m_fontnum;
   unsigned char  m_mirror:1;
   unsigned char  m_proportional:1; // if set, width from font structure used for spacing
   unsigned char  m_never_mirror:1; // this text is always unmirrored, regardsless of inserts etc...

   //unsigned char  textAlignment:2; // 0=left, 1=center, 2=right
   //unsigned char  lineAlignment:2; // 0=bottom, 1=center, 2=top
   unsigned char m_horizontalPosition:2; // horizontal position relative to insert point
   unsigned char m_verticalPosition:2;   // vertical position relative to insert point

   PointStruct    m_pnt;
   DbUnit         m_height;
   DbUnit         m_width;    // width of character, does not include space between chars.
                            // this is also the space of a "blank"

   DbUnit         m_angleRadians;     // angle in radians
   short          m_oblique;          // angle - deg [0..90]
   int            m_penWidthIndex;

public:

   TextStruct();
   TextStruct(const TextStruct& other);
   TextStruct(const CString& text,bool proportionalSpacingFlag);
   ~TextStruct();
   TextStruct& operator=(const TextStruct& other);
   void init();

   // accessors
   int getFontNumber() const { return m_fontnum; }
   void setFontNumber(int fontNumber) { m_fontnum = fontNumber; }

   CFontProperties *GetFontProperties() const    { return m_fontProps; }
   CFontProperties *GetDefinedFontProperties()   { if (m_fontProps == NULL) m_fontProps = new CFontProperties; return m_fontProps; }  

   bool isMirrored() const { return (m_mirror != 0); }
   void setMirrored(bool flag) { m_mirror = flag; }
   void setMirrored(BOOL flag) { m_mirror = (flag != 0); }
   bool getResultantMirror(bool mirrored) const { return m_mirror != mirrored;  }
   bool getResultantMirror(BOOL mirrored) const { return m_mirror != (mirrored != 0);  }

   bool getMirrorDisabled() const { return (m_never_mirror != 0); }
   void setMirrorDisabled(bool flag) { m_never_mirror = flag; }
   void setMirrorDisabled(BOOL flag) { m_never_mirror = (flag != 0); }

   bool isProportionallySpaced() const { return (m_proportional != 0); }
   void setProportionalSpacing(bool flag) { m_proportional = flag; }
   void setProportionalSpacing(BOOL flag) { m_proportional = (flag != 0); }

   HorizontalPositionTag getHorizontalPosition() const { return (HorizontalPositionTag)m_horizontalPosition; }
   void setHorizontalPosition(HorizontalPositionTag horizontalPosition ) { m_horizontalPosition = horizontalPosition; }

   VerticalPositionTag getVerticalPosition() const { return (VerticalPositionTag)m_verticalPosition; }
   void setVerticalPosition(VerticalPositionTag verticalPosition ) { m_verticalPosition = verticalPosition; }

   const PointStruct& getPnt() const { return m_pnt; }
   PointStruct& getPntRef() { return m_pnt; }
   void setPnt(DbUnit x,DbUnit y) { m_pnt.x = x;  m_pnt.y = y; }
   void setPnt(double x,double y) { m_pnt.x = (DbUnit)x;  m_pnt.y = (DbUnit)y; }
   void setPntX(DbUnit x ) { m_pnt.x  =  x; }
   void setPntY(DbUnit y ) { m_pnt.y  =  y; }
   void incPntX(DbUnit dx) { m_pnt.x += dx; }
   void incPntY(DbUnit dy) { m_pnt.y += dy; }

   CPoint2d getOrigin() const;
   void setOrigin(double x,double y) { m_pnt.x = (DbUnit)x;  m_pnt.y = (DbUnit)y; }

   double getWidth() const { return m_width; }
   void setWidth(double pWidth) { m_width = (DbUnit)pWidth; }

   double getHeight() const { return m_height; }
   void setHeight(double pHeight) { m_height = (DbUnit)pHeight; }

   double getRotation() const { return m_angleRadians; }
   double getRotationRadians() const { return m_angleRadians; }
   double getRotationDegrees() const;
   void setRotation(double radians) { m_angleRadians = (DbUnit)radians; }
   void setRotationDegrees(double degrees);
   void setRotationRadians(double radians) { m_angleRadians = (DbUnit)radians; }

   short getOblique() const { return m_oblique; }
   void setOblique(short obliqueAngle) { m_oblique = obliqueAngle; }

   int getPenWidthIndex() const { return m_penWidthIndex; }
   void setPenWidthIndex(int index) { m_penWidthIndex = index; }

   char* getText() const { return m_text; }
   void setText(char* text);
   void setText(const CString& text);

   // query
   int getNumLines() const;
   int getMaxLineCharacterCount() const;
   double getMaxLineLength(double spaceRatio, double penWidth=0., CDoubleArray* lineLengthArray=NULL) const;
   double getMaxLineLengthInFontUnits(double spaceRatio,CDoubleArray* lineLengthArray=NULL) const;
   CExtent getTextBox(double spaceRatio, double penWidth=0.) const;
   CTMatrix getTMatrix() const;
   CPoint2d getInitialCharacterPosition(double maxLineLengthInFontUnits,int lineCount,double spaceRatio) const;
   void getBoundaryPoly(CPoly& boundary,double spaceRatio) const;

   void transform(const CTMatrix& transformationMatrix);
   void updateExtent(CExtent& extent,double spaceRatio) const;
   void updateExtent(CExtent& extent,double spaceRatio,const CTMatrix& transformationMatrix) const;
   CExtent getExtent(double spaceRatio, double penWidth=0.) const;
   CExtent getExtent(double spaceRatio,const CTMatrix& transformationMatrix, double penWidth=0.) const;
   void getBoundaryPoly(CPoly& boundary);

   void dump(CWriteFormat& writeFormat,int depth = -1) const;
};

#endif
