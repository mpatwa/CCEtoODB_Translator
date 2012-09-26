// $Header: /CAMCAD/DcaLib/DcaText.cpp 4     3/23/07 5:19a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaText.h"
#include "DcaHorizontalPosition.h"
#include "DcaVerticalPosition.h"
#include "DcaPoint2d.h"
#include "DcaFont.h"
#include "DcaTMatrix.h"
#include "DcaExtent.h"
#include "DcaPoly.h"
#include "DcaBasesVector.h"
#include "DcaWriteFormat.h"

//_____________________________________________________________________________
TextStruct::TextStruct()
{
   init();
}

TextStruct::TextStruct(const TextStruct& other)
{
   m_text = NULL;

   *this  = other;
}

TextStruct::TextStruct(const CString& text,bool proportionalSpacingFlag)
{
   init();
   setText(text);
   setProportionalSpacing(proportionalSpacingFlag);
}

TextStruct& TextStruct::operator=(const TextStruct& other)
{
   if (&other != this)
   {
      if (m_text != NULL)
      {
         free(m_text);
      }
      init();
      m_text           = ((other.m_text == NULL) ? NULL : STRDUP(other.m_text));
      m_pnt            = other.m_pnt;
      m_fontnum        = other.m_fontnum;
      m_mirror         = other.m_mirror;
      m_proportional   = other.m_proportional;
      m_never_mirror   = other.m_never_mirror;
      m_height         = other.m_height;
      m_width          = other.m_width;
      m_angleRadians   = other.m_angleRadians;
      m_oblique        = other.m_oblique;
      m_penWidthIndex      = other.m_penWidthIndex;
      m_horizontalPosition = other.m_horizontalPosition;
      m_verticalPosition   = other.m_verticalPosition;

      if (other.m_fontProps != NULL)
      {
         this->GetDefinedFontProperties();  // makes sure this->fontProps is defined
         *this->m_fontProps = other.m_fontProps;
      }
      else
      {
         if(NULL != this->m_fontProps)
         {
            delete this->m_fontProps;
            this->m_fontProps = NULL;
         }
      }
   }

   return *this;
}

TextStruct::~TextStruct()
{
   if (m_text != NULL)
   {
      free(m_text);

      if (m_fontProps != NULL)
         delete m_fontProps;
   }
}

void TextStruct::init()
{
   m_text           = NULL;
   m_fontnum        = 0;
   m_fontProps      = NULL;
   m_mirror         = 0;
   m_proportional   = 0;
   m_never_mirror   = 0;
   m_height         = 0.;
   m_width          = 0.;
   m_angleRadians   = 0.;
   m_oblique        = 0;
   m_penWidthIndex  = 0;

   m_horizontalPosition = horizontalPositionLeft;
   m_verticalPosition   = verticalPositionBaseline;
}

double TextStruct::getRotationDegrees() const
{
   return radiansToDegrees(m_angleRadians);
}

void TextStruct::setRotationDegrees(double degrees)
{
   m_angleRadians = (DbUnit)degreesToRadians(degrees);
}

void TextStruct::setText(char* text)
{
   if (m_text != NULL)
   {
      free(m_text);
   }

   m_text = text;
}

void TextStruct::setText(const CString& text)
{
   if (m_text != NULL)
   {
      free(m_text);
   }

   m_text = STRDUP(text);
}

CPoint2d TextStruct::getOrigin() const
{
   return m_pnt.getPoint2d();
}

int TextStruct::getNumLines() const
{
   int numLines = 1;

   if (m_text != NULL)
   {
      for (char* p = m_text;*p != '\0';p++)
      {
         if (*p == '\n')
         {
            numLines++;
         }
      }
   }

   return numLines;
}

int TextStruct::getMaxLineCharacterCount() const
{
   int maxLineLength = 0;
   int lineLength = 0;

   if (m_text != NULL)
   {
      for (char* p = m_text;*p != '\0';p++)
      {
         if (*p == '\n')
         {
            lineLength = 0;
         }
         else
         {
            lineLength++;

            if (lineLength > maxLineLength)
            {
               maxLineLength = lineLength;
            }
         }
      }
   }

   return maxLineLength;
}

double TextStruct::getMaxLineLength(double spaceRatio,double penWidth,CDoubleArray* lineLengths) const
{
   double maxLineLength = 0.;

   bool saveLineLengths = (lineLengths != NULL);

   if (saveLineLengths)
   {
      lineLengths->RemoveAll();
   }

   if (m_text != NULL)
   {
      double lineLength    = 0.;
      FontStruct* font = NULL;
      bool proportionalFlag = (m_proportional != 0);

      if (proportionalFlag)
      {
         font = CFontList::getFontList().getFont(m_fontnum);
         proportionalFlag = (font != NULL);
      }

      for (char* p = m_text;;p++)
      {
         if (*p == '\0')
         {
            if (saveLineLengths)
            {
               lineLengths->Add(lineLength);
            }

            break;
         }
         else if (*p == '\n')
         {
            if (saveLineLengths)
            {
               lineLengths->Add(lineLength);
            }

            lineLength = 0.;
         }
         else
         {
            double charLength = 0.;
            
            if (*p == '%' && (p[1] == 'O' || p[1] == 'U'))
            {
               charLength = 0.;
               p++;
            }
            else if (*p < 0 || *p == ' ') // If char is a space or foreign char
            {
               if (proportionalFlag)
                  charLength = (2. * spaceRatio) * m_width;
               else
                  charLength = (1. + spaceRatio) * m_width;
            }
            else if (isgraph(*p))
            {
               if (proportionalFlag)
               {
                  charLength = (2. * font->getCharacterWidth(*p) + spaceRatio) * m_width + penWidth;
               }
               else
               {
                  charLength = (1. + spaceRatio) * m_width + penWidth;
               }
            }

            if((p[1] == '\n') || (p[1] == '\0')) //last character of line
            {
               charLength -= spaceRatio * m_width;
               if (isgraph(*p))
                  charLength -= penWidth/2;
            }
            

            lineLength += charLength;

            if (lineLength > maxLineLength)
            {
               maxLineLength = lineLength;
            }
         }
      }
   }

   return maxLineLength;
}

double TextStruct::getMaxLineLengthInFontUnits(double spaceRatio,CDoubleArray* lineLengths) const
{
   double maxLineLength = 0.;

   bool saveLineLengths = (lineLengths != NULL);

   if (saveLineLengths)
   {
      lineLengths->RemoveAll();
   }

   if (m_text != NULL)
   {
      double lineLength    = 0.;
      FontStruct* font = NULL;
      bool proportionalFlag = (m_proportional != 0);

      if (proportionalFlag)
      {
         font = CFontList::getFontList().getFont(m_fontnum);
         proportionalFlag = (font != NULL);
      }

      for (char* p = m_text;;p++)
      {
         if (*p == '\0')
         {
            if (saveLineLengths)
            {
               lineLengths->Add(lineLength);
            }

            break;
         }
         else if (*p == '\n')
         {
            if (saveLineLengths)
            {
               lineLengths->Add(lineLength);
            }

            lineLength = 0.;
         }
         else
         {
            double charLength = 0.;

            if (*p == '%' && (p[1] == 'O' || p[1] == 'U'))
            {
               charLength = 0.;
               p++;
            }
            else if (*p == ' ')
            {
               if (proportionalFlag)
               {
                  charLength = 2. * spaceRatio;
               }
               else
               {
                  charLength = 1. + spaceRatio;
               }
            }
            else if (isgraph(*p))
            {
               if (proportionalFlag)
               {
                  charLength = 2. * font->getCharacterWidth(*p) + spaceRatio;
               }
               else
               {
                  charLength = 1. + spaceRatio;
               }
            }

            lineLength += charLength;

            if (lineLength > maxLineLength)
            {
               maxLineLength = lineLength;
            }
         }
      }
   }

   return maxLineLength;
}

CTMatrix TextStruct::getTMatrix() const
{
   CTMatrix matrix;

   matrix.translateCtm(m_pnt.x,m_pnt.y);
   matrix.rotateRadiansCtm(m_angleRadians);
   matrix.scaleCtm(((m_mirror & MIRROR_FLIP) != 0) ? -1. : 1.,1.);

   return matrix;
}

CExtent TextStruct::getTextBox(double spaceRatio, double penWidth) const
{
   const double characterSpacing = .1; 
   const double textLeading = 1.25;

   double boxWidth = getMaxLineLength(spaceRatio,penWidth);

   int numLines = getNumLines();

   double boxHeight = m_height + ((numLines - 1) * textLeading * m_height);

   CPoint2d upperLeft(0.,0.);

   switch (getVerticalPosition())
   {
   case verticalPositionBaseline:  upperLeft.y = m_height;      break;
   case verticalPositionCenter:    upperLeft.y = boxHeight/2.;  break;
   case verticalPositionBottom:    upperLeft.y = boxHeight;     break;
   case verticalPositionTop:       upperLeft.y = 0.;            break;
   }

   switch (getHorizontalPosition())
   {
   case horizontalPositionLeft:    upperLeft.x = 0.;           break;
   case horizontalPositionCenter:  upperLeft.x = -boxWidth/2.; break;
   case horizontalPositionRight:   upperLeft.x = -boxWidth;    break;
   }

   CPoint2d lowerRight = upperLeft + CPoint2d(boxWidth,-boxHeight);

   if (m_oblique != 0)
   {
      double offset = m_height * cos(degreesToRadians(m_oblique));

      if (offset < 0.) upperLeft.x  -= offset;
      else             lowerRight.x += offset;
   }

   CExtent textBox;

   textBox.update(upperLeft);
   textBox.update(lowerRight);

   return textBox;
}

// return initial character position relative to anchor point of (0.,0.)
CPoint2d TextStruct::getInitialCharacterPosition(double maxLineLengthInFontUnits,int lineCount,double spaceRatio) const
{
   CPoint2d initialCharacterPosition;
   double stringWidth = maxLineLengthInFontUnits * m_width;
   double charCellHeight = m_height * (1.0 + spaceRatio);

   switch (m_horizontalPosition)
   {
   case horizontalPositionCenter: // center align
      initialCharacterPosition.x = -stringWidth / 2;
      break;
   case horizontalPositionRight: // right align
      initialCharacterPosition.x = -stringWidth;
      break;
   case horizontalPositionLeft: // left align
   default:
      initialCharacterPosition.x = 0;
      break;
   }

   switch (m_verticalPosition)
   {
   case verticalPositionBottom: // bottom align
      initialCharacterPosition.y = charCellHeight * (lineCount - 1);
      break;
   case verticalPositionCenter: // center align
      initialCharacterPosition.y = -(m_height / 2) + (charCellHeight * (lineCount - 1) / 2);
      break;
   case verticalPositionTop: // top align
      initialCharacterPosition.y = -m_height;
      break;
   case verticalPositionBaseline: // baseline align
   default:
      initialCharacterPosition.y = 0.;
      break;
   }

   return initialCharacterPosition;
}

void TextStruct::getBoundaryPoly(CPoly& boundary,double spaceRatio) const
{
   boundary.init();

   CDoubleArray lineLengths;
   double maxLineLength = getMaxLineLengthInFontUnits(spaceRatio,&lineLengths);

   int lineCount = lineLengths.GetSize();
   double charCellHeight = m_height * (1.0 + spaceRatio);
   CPoint2d initialCharacterPosition = getInitialCharacterPosition(maxLineLength,lineCount,spaceRatio);
   CPoint2d boundaryPoint(initialCharacterPosition);

   boundaryPoint.y += charCellHeight;
   boundary.addVertex(boundaryPoint);

   for (int index = 0;index < lineLengths.GetSize();index++)
   {
      double lineLength = lineLengths.GetAt(index) * m_width;
      boundaryPoint.x = initialCharacterPosition.x + lineLength;
      boundary.addVertex(boundaryPoint);
      boundaryPoint.y -= charCellHeight;
      boundary.addVertex(boundaryPoint);
   }

   boundaryPoint.x = initialCharacterPosition.x;
   boundary.addVertex(boundaryPoint);

   boundary.close();
}

void TextStruct::transform(const CTMatrix& transformationMatrix)
{
   CBasesVector basesVector(m_pnt.x,m_pnt.y,1.,radiansToDegrees(m_angleRadians),m_mirror);
   basesVector.transform(transformationMatrix);

   m_pnt.x = (DbUnit)basesVector.getOrigin().x;
   m_pnt.y = (DbUnit)basesVector.getOrigin().y;

   double scale = basesVector.getScale();
   m_height = (DbUnit)(m_height * scale);
   m_width  = (DbUnit)(m_width  * scale);

   m_angleRadians = (DbUnit)degreesToRadians(basesVector.getRotation());
   m_mirror       = basesVector.getMirror();
}

void TextStruct::updateExtent(CExtent& extent,double spaceRatio) const
{
   extent.update(getExtent(spaceRatio));
}

void TextStruct::updateExtent(CExtent& extent,double spaceRatio,const CTMatrix& transformationMatrix) const
{
   CExtent textExtent = getExtent(spaceRatio,transformationMatrix);
   extent.update(textExtent);
}

CExtent TextStruct::getExtent(double spaceRatio, double penWidth) const
{
   CExtent extent;
   CTMatrix matrix = getTMatrix();
   CExtent textBox = getTextBox(spaceRatio,penWidth);

   CPoint2d ll = textBox.getLL();
   CPoint2d lr = textBox.getLR();
   CPoint2d ur = textBox.getUR();
   CPoint2d ul = textBox.getUL();

   matrix.transform(ll);
   matrix.transform(lr);
   matrix.transform(ur);
   matrix.transform(ul);

   extent.update(ll);
   extent.update(lr);
   extent.update(ur);
   extent.update(ul);

   return extent;
}

CExtent TextStruct::getExtent(double spaceRatio,const CTMatrix& transformationMatrix, double penWidth) const
{
   CExtent extent;
   CTMatrix matrix = getTMatrix() * transformationMatrix;
   CExtent textBox = getTextBox(spaceRatio,penWidth);

   CPoint2d ll = textBox.getLL();
   CPoint2d lr = textBox.getLR();
   CPoint2d ur = textBox.getUR();
   CPoint2d ul = textBox.getUL();

   matrix.transform(ll);
   matrix.transform(lr);
   matrix.transform(ur);
   matrix.transform(ul);

   extent.update(ll);
   extent.update(lr);
   extent.update(ur);
   extent.update(ul);

   return extent;
}

void TextStruct::dump(CWriteFormat& writeFormat,int depth) const
{
   writeFormat.writef(
      "TextStruct\n"
      "{\n"
      "   Text='%s'\n",
      "   pnt=(%.3f,%.3f)\n"
      "   height=%.3f\n"
      "   width=%.3f\n"
      "   angle=%.3f\n"
      "}\n",
      ((m_text == NULL) ? "NULL" : m_text),
      (double)m_pnt.x,(double)m_pnt.y,
      (double)m_height,
      (double)m_width,
      (double)m_angleRadians);
}
