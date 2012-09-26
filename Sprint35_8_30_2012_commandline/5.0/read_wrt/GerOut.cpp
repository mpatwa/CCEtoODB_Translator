// $Header: /CAMCAD/4.5/read_wrt/GerOut.cpp 58    4/30/06 12:23p Kurt Van Ness $

/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2000. All Rights Reserved.
*/

/*   // here is an example of GerberX Format
   G04 GSI RS-274x photoplot definition file *

   D10 - round
   D11 - square
   D12 - rectangle
   D13 - target
   D14 - thermal
   D15 - donut
   D16 - octagon
   D17 - oblong

%FSLAX23Y23*%
%MOIN*%
%IPPOS*%
%AMTAD13*
1,1,0.10000,0,0*
1,0,0.09600,0,0*
1,1,0.06667,0,0*
1,0,0.06267,0,0*
20,1,0.00200,0,0.10000,0,-0.10000,0*
20,1,0.00200,0.10000,0,-0.10000,0,0*%
%AMTHD14*
1,1,0.10000,0,0*
1,0,0.06000,0,0*
21,0,0.10200,0.02000,0.000,0.000,45.000*
21,0,0.10200,0.02000,0.000,0.000,135.000*
%
%AMDOD15*
1,1,0.10000,0,0*
1,0,0.05000,0,0*%
%ADD10C,0.06000*%
%ADD11R,0.06000X0.06000*%
%ADD12R,0.10000X0.05000*%
%ADD13TAD13*%
%ADD14THD14*%
%ADD15DOD15*%
%ADD16P,0.10000X8X22.5*%
%ADD17O,0.20000X0.05000*%
%ADD18C,0.00500*%
*/

// G_GEROUT.CPP

#include "stdafx.h"
#include "ccdoc.h"
#include "gerblib.h"
#include "font.h"
#include <math.h>
#include "lyr_file.h"
#include "gauge.h"
#include "graph.h"
#include "pcbutil.h"
#include "polylib.h"
#include "attrib.h"
#include "apertur2.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
extern CFileLayerList *FileLayerList;

/****************************************************************************/
/*
*/
typedef struct
{
   CString  originalname;     // this is the original blockname
   int      originalrot;      
   CString  appname;
   CString  macroname;
} GERBERAppRotStruct;
typedef CTypedPtrArray<CPtrArray, GERBERAppRotStruct*> AppRotArray;

static int              fillDCode;     // this is the user selected fill code
static int              polyfill;      // use gerber G36/G37 polyfill            
static double           fill_width;    // temporary width for fillin output.
static double           SMALLWIDTH;

static   AppRotArray    approtarray;
static   int            approtcnt;

static void Gerb_WriteChar(CPolyList *PolyList, double insert_x, double insert_y,
            double rotation, double scale, char mirror, int oblique, int dcode);


static   CCEtoODBDoc     *doc;
static   FileLayerStruct *FileLayerPtr;
static   int            display_error;
static   FILE           *flog;

static int get_rotated_aperture(const char *n);

static void Gerb_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale,
                    int embeddedLevel, int insertlayer, double unitScale);

static void Gerb_Find_Rotated_Apertures(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale,
                    int embeddedLevel, int insertlayer, double unitScale);

static void Gerb_WriteApertureMacroData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale, const char *appname);

static void Gerb_DrawAperture(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale, const char *appname);

static int gerber_fillin(double xa, double xe, double y); // scanline routine
void FindMirBlocks(CDataList *DataList, int mirror, int insertLayer, CList<CString, CString&> *mirBlocks);

// here check that macroname does not exist yet
static CString store_macroname_aperture(const char *appname)
{
   CString  mmacroname;
   mmacroname.Format("M_%s",appname);

   return mmacroname;
}

static CString getLegalMacroName(const CString& macroName)
{
   if (macroName.GetLength() > 0 && isdigit(macroName[0]))
   {
      return "_" + macroName;
   }
   else
   {
      return macroName;
   }
}

/******************************************************************************
* Gerb_WriteFiles
*/
void Gerb_WriteFiles(CCEtoODBDoc *Doc, FormatStruct *format)
{
   char prosa[255];
   int res; // result

   doc = Doc;
   display_error = 0;
   SMALLWIDTH = 0;

   // Log File
   CString logFile = GetLogfilePath("GERBER.LOG");

   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

	// Fill Width
   if ((polyfill = format->GR.PolyFill) == 0)
   {
      fillDCode = format->GR.fillDCode; // fill DCode
      fill_width = -1.0;      // init to make sure, it will be found in aperture list.

      // find fill aperture
      for (int i = 0; i < doc->getNextWidthIndex(); i++)
      {
         if (!doc->getWidthTable()[i]) continue;   // hole in widthTable  

         if (doc->getWidthTable()[i]->getDcode() == fillDCode)
         {
            fill_width = doc->getWidthTable()[i]->getSizeA() * format->Scale;
            break;
         }
      }

      // here is fill code does not exist .. error and stop.
      if (fill_width < 0)
      {
         ErrorMessage("Fill DCode does not exist.", "User Error", MB_ICONSTOP | MB_OK);
         return;
      }
   }

   wgerb_Set_Format(format);

   // Loop FileList
   POSITION fileLayerPos = FileLayerList->GetHeadPosition();
   while (fileLayerPos != NULL)
   {      
      FileLayerPtr = FileLayerList->GetNext(fileLayerPos);

      progress->SetStatus(FileLayerPtr->file);
      
      if ((res = wgerb_Graph_File_Open(FileLayerPtr->file)) != 1)
      {
         CString  tmp;
         // error occured
         // -1 = file can not open
         // 0  = memory failed
         // 1 = all ok
         if(res == -1)
         {
            // file error
            tmp.Format("Can not open file [%s]", FileLayerPtr->file);
            ErrorMessage(tmp);
            return;
         }

         if (res == 0)
            MemErrorMessage(__FILE__, __LINE__);
      }

      approtarray.SetSize(10,10);
      approtcnt = 0;

      sprintf(prosa,"G04 CAMCAD photoplot definition file *\n");
      wgerb_write2file(prosa);
      // here write format statement
      sprintf(prosa,"%%FSLAX%d%dY%d%d*%%\n",
         format->GR.digits, format->GR.decimal, format->GR.digits, format->GR.decimal);

      wgerb_write2file(prosa);

      if (format->GR.units == 0)
         sprintf(prosa,"%%MOIN*%%\n");
      else
         sprintf(prosa,"%%MOMM*%%\n");

      wgerb_write2file(prosa);
      wgerb_write2file("%IPPOS*%\n");

		FileStruct *currentFile = NULL;
      // here find rotated apertures in the files and add them to the aperture list

      POSITION filePos = doc->getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
				continue;

         if (file->getBlock() == NULL)
				continue;

			currentFile = file;

			// here apertures can be added.
         doc->PrepareAddEntity(file);

         Gerb_Find_Rotated_Apertures(&(file->getBlock()->getDataList()), file->getInsertX() * format->Scale,
               file->getInsertY() * format->Scale,
               file->getRotation(), file->isMirrored(), file->getScale() * format->Scale, 0, -1, format->Scale);
		}

      // now update DCodes
      Generate_Unique_DCodes(doc);

      // Format.format = 0 is normal Gerber with external App.
      // Format.GR.format = 1 is RS274X
      if (format->GR.format == 1)
      {
         CList<CString,CString&> mirBlocks;
         FindMirBlocks(&currentFile->getBlock()->getDataList(), currentFile->isMirrored(), -1, &mirBlocks);  

         // first do AM - this are all non round or square app
			int i=0;
         for (i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);

            if (block == NULL)
					continue;

            // create necessary aperture blocks
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getFlags() & BL_SMALLWIDTH)
               {
                  SMALLWIDTH = block->getSizeA() * format->Scale;
               }

               if (block->getShape() == T_COMPLEX)
               {
                  int dcode = block->getDcode();
                  double rot = block->getRotation();
                  CString  appname = block->getName();
						int blocknum = (int)(block->getSizeA());
                  block = doc->Find_Block_by_Num((int)(block->getSizeA()));

                  if (!block)
							continue;

                  if (block->getDataList().IsEmpty())
							continue;

						// Use aperture macros only for rotated non-complex apertures
						if (get_rotated_aperture(appname) > -1)
						{
							int mir = 0;
							POSITION mirrorBlockPos = mirBlocks.GetHeadPosition();
							while (mirrorBlockPos != NULL)
							{
								CString complexblock = mirBlocks.GetNext(mirrorBlockPos);
								if (appname.Find(complexblock)> -1)
								{
									mir = 1;
									break;
								}
							}

							sprintf(prosa, "%%AM%s*\n", (const char*)getLegalMacroName(appname));

							wgerb_write2file(prosa);
							Gerb_WriteApertureMacroData(&(block->getDataList()),
								0.0,
								0.0,
								rot,
								mir,
								format->Scale, block->getName());
							wgerb_write2file("%\n");
						}
               }
               else
               {
                  double sizeA = block->getSizeA() * format->Scale;
                  double sizeB = block->getSizeB() * format->Scale; 

                  switch (block->getShape())
                  {
                  case T_THERMAL:
                     sprintf(prosa,"%%AMTHERMALD%d*\n",block->getDcode());
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,1,%1.5lf,0,0*\n",sizeA);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,0,%1.5lf,0,0*\n",sizeB);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"21,0,%1.5lf,0.02,0.0,0.0,45.0*\n",sizeA+0.02);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"21,0,%1.5lf,0.02,0.0,0.0,135.0*\n",sizeA+0.02);
                     wgerb_write2file(prosa);
                     wgerb_write2file("%\n");
                     break;
                  case T_DONUT:
                     sprintf(prosa,"%%AMDOD%d*\n",block->getDcode());
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,1,%1.5lf,0,0*\n",sizeA);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,0,%1.5lf,0,0*\n",sizeB);
                     wgerb_write2file(prosa);
                     wgerb_write2file("%\n");
                     break;
                  case T_TARGET:
                     sprintf(prosa,"%%AMTARGET%d*\n",block->getDcode());
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,1,%1.5lf,0,0*\n",sizeA/2);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"1,0,%1.5lf,0,0*\n",0.625 * (sizeA/2));
                     wgerb_write2file(prosa);
                     sprintf(prosa,"20,1,%1.5lf,0.0,%1.5lf,0.0,%1.5lf,0.0*\n",sizeA * 0.08,sizeA/2,-sizeA/2);
                     wgerb_write2file(prosa);
                     sprintf(prosa,"20,1,%1.5lf,%1.5lf,0.0,%1.5lf,0.0,0.0*\n",sizeA * 0.08,sizeA/2,-sizeA/2);
                     wgerb_write2file(prosa);
                     wgerb_write2file("%\n");
                     break;
                  default:
                     /* only complex apertures should be written.
                        sprintf(prosa,"%%AM%s%%\n",block->name);
                        wgerb_write2file(prosa);
                     */
                     break;
                  }
               }
            }
         }

         // now do ADD
         for (i=0;i<doc->getNextWidthIndex();i++)
         {
            if (!doc->getWidthTable()[i]) continue;// hole in widthTable  
//#ifdef _DEBUG
				BlockStruct *bb = doc->getWidthTable()[i];
//#endif
            double sizeA = doc->getWidthTable()[i]->getSizeA() * format->Scale;
            double sizeB = doc->getWidthTable()[i]->getSizeB() * format->Scale;

            switch (doc->getWidthTable()[i]->getShape())
            {
            case  T_COMPLEX:
               {
						CString  appname;
                  appname = doc->getWidthTable()[i]->getName();
						// Only used for compex apertures made for rotated non-complex apertures
						if (get_rotated_aperture(appname) > -1)
						{
							sprintf(prosa,"%%ADD%d%s*%%\n", doc->getWidthTable()[i]->getDcode(),(const char*)getLegalMacroName(appname));
						}
					}
               break;
            case  T_RECTANGLE:
               sprintf(prosa,"%%ADD%dR,%1.5lfX%1.5lf*%%\n",
                  doc->getWidthTable()[i]->getDcode(), sizeA,sizeB);
               break;               
            case  T_SQUARE:
               sprintf(prosa,"%%ADD%dR,%1.5lfX%1.5lf*%%\n",
                  doc->getWidthTable()[i]->getDcode(), sizeA,sizeA);
               break;
            case T_TARGET:
               sprintf(prosa,"%%ADD%dTARGET%d*%%\n",
                  doc->getWidthTable()[i]->getDcode(),doc->getWidthTable()[i]->getDcode());
               break;
            case T_THERMAL:
               sprintf(prosa,"%%ADD%dTHERMALD%d*%%\n",
                  doc->getWidthTable()[i]->getDcode(),doc->getWidthTable()[i]->getDcode());
               break;
            case T_DONUT:
               sprintf(prosa,"%%ADD%dDOD%d*%%\n",
                  doc->getWidthTable()[i]->getDcode(),doc->getWidthTable()[i]->getDcode());
               break;
            case T_OCTAGON:
               sprintf(prosa,"%%ADD%dP,%1.5lfX8X22.5*%%\n",
                  doc->getWidthTable()[i]->getDcode(),sizeA);
               break;
            case T_OBLONG:
               sprintf(prosa,"%%ADD%dO,%1.5lfX%1.5lf*%%\n",
                  doc->getWidthTable()[i]->getDcode(),sizeA,sizeB);
               break;
            case T_ROUND:
               if (sizeA < SMALLWIDTH) sizeA = SMALLWIDTH; // do not allow an aperture to be smaller than small width
               sprintf(prosa,"%%ADD%dC,%1.5lf*%%\n",
                  doc->getWidthTable()[i]->getDcode(),sizeA);
               break;                 
            default:
               prosa[0] = '\0';
               break;
            }

            if (strlen(prosa))
               wgerb_write2file(prosa);
         }
      }

/*
      LayerStruct *l = doc->FindLayer(FileLayerPtr->LayerList);
      int neg = doc->IsNegativeLayer(FileLayerPtr->LayerList);

      wgerb_write2file("%%LN%s*%%\n",prosa);

      %LNLayer_1*%
      %SRX1Y1I0.000J0.000*%
      G54D10*
*/
      wgerb_write2file("%LPD*%\n");

      filePos = doc->getFileList().GetHeadPosition();
      while (filePos != NULL)
      {
         FileStruct *file = doc->getFileList().GetNext(filePos);

         if (!file->isShown())
				continue;

         if (file->getBlock() == NULL)
				continue;

         double fileX = file->getInsertX();
         double fileY = file->getInsertY();
         double fileRot = file->getRotation();
         double fileScale = file->getScale();
         int fileMirror = (doc->getBottomView() ? MIRROR_FLIP : 0) ^ (file->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

         if (doc->getBottomView())
         {
            fileX = -fileX;
            fileRot = -fileRot;
         }

         Gerb_WriteData(&(file->getBlock()->getDataList()), fileX * format->Scale,
               fileY * format->Scale,
               fileRot, fileMirror, fileScale * format->Scale, 0, -1, format->Scale);
      }

      wgerb_Graph_File_Close();

      for (int i=0;i<approtcnt;i++)
      {
#ifdef _DEBUG
      GERBERAppRotStruct *cc = approtarray[i];
#endif

         // delete the temporary stored names
         BlockStruct *b = Graph_Block_Exists(doc, approtarray[i]->appname, -1);

         if (b)
				doc->RemoveBlock(b);

         if (strlen(approtarray[i]->macroname))
         {
            b = Graph_Block_Exists(doc, approtarray[i]->macroname, -1);

            if (b)
					doc->RemoveBlock(b); 
         }

         CString mmacroname;
         mmacroname = store_macroname_aperture(approtarray[i]->appname);

         if (strlen(mmacroname))
         {
            b = Graph_Block_Exists(doc, mmacroname, -1);

            if (b)
					doc->RemoveBlock(b);
         }

         delete approtarray[i];
      }

      approtarray.RemoveAll();
      approtcnt = 0;
   }

   fclose(flog);

   if (display_error)
      Logreader(logFile);

   return;
}

/******************************************************************************
* FindMirBlocks
*/
static void FindMirBlocks(CDataList *DataList, int mirror, int insertLayer, CList<CString, CString&> *mirBlocks)
{
   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      if (data->getDataType() != T_INSERT)
         continue;
   
      int block_mirror = mirror ^ data->getInsert()->getMirrorFlags(); 
   
      BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
      if (block == NULL)
         continue;

      if ((block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE) && block_mirror)
		{
			int layer = Get_ApertureLayer(doc, data, block, insertLayer);
			int newlayer = doc->get_layer_mirror(layer, mirror);

			if (doc->get_layer_visible(layer, mirror) && LayerInFile(FileLayerPtr, newlayer) ) // only write data item if it goes in this file
				mirBlocks->AddHead(block->getNameRef());
		}
      
      int block_layer;
      if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
         block_layer = data->getLayerIndex();
      else 
         block_layer = insertLayer;
      FindMirBlocks(&block->getDataList(), block_mirror, block_layer, mirBlocks);
   } 
} 

/******************************************************************************
* is_aperture_centered
*/
static int is_aperture_centered(int shape)
{
   int centered;

   switch (shape)
   {
      case T_BLANK:
      case T_OCTAGON:
      case T_DONUT:
      case T_THERMAL:
      case T_ROUND:
         centered = 1;
      break;
      default:
         centered = 0;
      break;
   }
   return centered;
}

/******************************************************************************
* write_gerber_text
*/
static int write_gerber_text(const char *text, double insert_x, double insert_y, 
                  double charheight, double charwidth, double angle, 
                  int proportional, int oblique, int mirror, int dcode, FontStruct* fontStruct)
{
   const double spaceratio = doc->getSettings().getTextSpaceRatio(); // turn percent into a decimal

   Mat2x2 m;
   RotMat2(&m, angle);

   double x_offset = 0;
   double y_offset = 0;

   for (unsigned int i=0; i<strlen(text); i++)
   {
      if (text[i] == ' ')
      {
         x_offset += (charwidth) + (SPACE * charheight);
         continue;
      }

      if (text[i] == '\n')
      {
         x_offset = 0;
         y_offset -= charheight * (1.0 + spaceratio);
         continue;
      }

      // do not do control char.
      if (text[i] < ' ')
			continue;

		Point2 offset;
      offset.x = x_offset;
      offset.y = y_offset;
      if (mirror)
			offset.x = -offset.x;
      TransPoint2(&offset, 1, &m, 0, 0);

      Gerb_WriteChar(fontStruct->getCharacterData(text[i]), insert_x + offset.x, insert_y + offset.y,
                  angle, charheight, mirror, oblique, dcode);

      // Find offset from insertion point for this character
      if (proportional)
         x_offset += (fontStruct->getCharacterWidth(text[i]) + SPACE) * charheight;
      else // not proprtional
         x_offset += charwidth;
   }

   return 1;
}

/******************************************************************************
* write_attributes
*/
static void write_attributes(CAttributes* map, double ix, double iy, double rotation, int mirror, double scale)
{
   if (map == NULL)
      return;


   CString gerber_line;
   POSITION pos = map->GetStartPosition();
   while (pos != NULL)
   {
		WORD keyword;
		Attrib* attrib = NULL;
      map->GetNextAssoc(pos, keyword, attrib);

      if (!doc->get_layer_visible(attrib->getLayerIndex(), mirror))
         continue;

      int layer = doc->get_layer_mirror(attrib->getLayerIndex(), mirror);
      if (!LayerInFile(FileLayerPtr, layer)) // only write data item if it goes in this file
         continue;

      double height = attrib->getHeight() * scale;
      double width = attrib->getWidth() * scale;

		if (height < SMALLNUMBER || width < SMALLNUMBER)
			continue;

		double angle;
      if (mirror)
         angle = rotation - attrib->getRotationRadians();
      else
         angle = rotation + attrib->getRotationRadians();


      Mat2x2 m;
      RotMat2(&m, rotation);

      Point2 point2;
      point2.x = attrib->getX()*scale;
      if (mirror)
         point2.x = -attrib->getX()*scale;
      point2.y = attrib->getY()*scale;
      TransPoint2(&point2, 1, &m, ix,iy);
      

      switch (attrib->getValueType())
      {
		case VT_INTEGER:
         gerber_line.Format("%d", attrib->getIntValue());
         break;

      case VT_UNIT_DOUBLE:
      case VT_DOUBLE:
         gerber_line.Format("%lg", attrib->getDoubleValue());
         break;

		case VT_STRING:
			gerber_line = get_attvalue_string(doc, attrib);
         break;
      }


		// Normalize the text alignment
		TextStruct textStruct(gerber_line, attrib->isProportionallySpaced());
		textStruct.setWidth(width);
		textStruct.setHeight(height);
		textStruct.setHorizontalPosition(attrib->getHorizontalPosition());
		textStruct.setVerticalPosition(attrib->getVerticalPosition());

		CDoubleArray lineLengths;
		double spaceratio = doc->getSettings().getTextSpaceRatio(); // turn percent into a decimal
		double maxLineLength = textStruct.getMaxLineLengthInFontUnits(spaceratio, &lineLengths);
		int lineCount = lineLengths.GetSize();
		CPoint2d initialCharacterPosition = textStruct.getInitialCharacterPosition(maxLineLength, lineCount, spaceratio);

      Point2 initialPosPoint;
		initialPosPoint.x = initialCharacterPosition.x;
      if (mirror)
			initialPosPoint.x = -initialPosPoint.x;
		initialPosPoint.y = initialCharacterPosition.y;

      RotMat2(&m, angle);
		TransPoint2(&initialPosPoint, 1, &m, 0, 0);

		point2.x += initialPosPoint.x;
		point2.y += initialPosPoint.y;


		// if an attribute is set to be never mirrored.
      if (attrib->getMirrorDisabled())
			mirror = 0;

		FontStruct* fontStruct = CFontList::getFontList().getFont(0);
      write_gerber_text(gerber_line, point2.x, point2.y, height, width, angle, 
				attrib->isProportionallySpaced(), 0, mirror, 
				doc->getWidthTable()[attrib->getPenWidthIndex()]->getDcode(), fontStruct);
   }

   return;
}

/******************************************************************************
* Gerb_WriteData
*/
void Gerb_WriteData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, 
                    double unitScale) // unitScale only used for apertures because they are not scaled
{
   Mat2x2      m;
   Point2      point2;
   int         layer;
   long        entcnt = 0;
   POSITION    pos;
   DataStruct  *np;
   int         cur_negative = FALSE;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      entcnt++;

      if (np->isHidden())  continue;

      // only draw visible layer (Inserts do not have layers)
      if (np->getDataType() != T_INSERT)
      {
         if (doc->IsFloatingLayer(np->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         layer = doc->get_layer_mirror(layer, mirror);
         if (!LayerInFile(FileLayerPtr, layer)) // only write data item if it goes in this file
            continue;
      }

      // inserts have no initial layers
      switch (np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               int dcode = doc->getWidthTable()[poly->getWidthIndex()]->getDcode();
               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();
               double width = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * unitScale;

               if (apshape == T_RECTANGLE && 
                  doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() == doc->getWidthTable()[poly->getWidthIndex()]->getSizeB())
                  apshape = T_SQUARE;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL negative   = poly->isVoid();

               if (cur_negative && !negative)
               {
                  wgerb_write2file("%LPD*%\n");
                  cur_negative = FALSE;
               }
               else
               if (!cur_negative && negative)
               {
                  wgerb_write2file("%LPC*%\n");
                  cur_negative = TRUE;
               }

               int cnt = 0, first = TRUE;
               Point2    firstp;
               Point2   p;
               int      ninty_degree = TRUE;

               // if filled, aperture does not matter
               if (!polyFilled || !polyfill) 
               {
                  if (apshape != T_ROUND && apshape != T_SQUARE)
                  {
                     fprintf(flog, "Draw [D%d] other than ROUND or SQUARE -> set to ROUND !\n", dcode);
                     display_error++;
                     apshape = T_ROUND;
                  }
               }

               if (polyFilled && !polyfill)
               {
                  fprintf(flog, "POLYFILL not implemented in RS274 (Standard Gerber) -> use RS274X !\n");
                  display_error++;
               }

               // this is drawn with G36/#7, which only fills 
               // to the outline of the fill.
               if (polyFilled && polyfill)   
               {
                  wgerb_write2file("G36*\n");  // fill on
                  first = TRUE;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     Point2   p;
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror & MIRROR_FLIP) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     if (mirror & MIRROR_FLIP) p.bulge = -p.bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (first)
                     {
                        // do dcode 0, so that no aperture is written in G36 code.
                        wgerb_Graph_Polyline(p, 0, apshape, width);
                        first = FALSE;
                     }
                     else
                        wgerb_Graph_Vertex(p);
                  }
                  wgerb_write2file("G37*\n"); // fill off
                  wgerb_Load_Aperture(-1);   // force a new aperture load
                  
                  // if the actual width is smaller than the fill dcode width
                  // width is in either MM or  INCHES
                  if (width <= SMALLWIDTH)   
                     continue; // do next polyline and do not surround the outline.
               }

               if (!closed)
               {
                  //here test is Gerberline is 90 degree
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p.x = pnt->x * scale;
                     if (mirror) p.x = -p.x;
                     p.y = pnt->y * scale;
                     p.bulge = pnt->bulge;
                     TransPoint2(&p, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     if (!cnt)
                     {
                        firstp = p;
                     }
                     else
                     {
                        // test if 90 degree
                        if (firstp.x != p.x && firstp.y != p.y)
                        {
                           ninty_degree = FALSE;
                        }
                        firstp = p;
                     }
                     cnt++;
                  }
               
                  if (apshape != T_ROUND && !ninty_degree)
                  {
                     // if only should write out if an endpoint of start or end is wrong.
                     fprintf(flog, "\"Non orthogonal draw\" of \"Non Round Aperture [D%d]\"\n", dcode);
                     //fprintf(flog,"Segment %lf %lf %lf %lf\n",start.x, start.y, end.x, end.y);
                     display_error++;
                  }
               }  // if not closed.

               // first draw a outline with the correct Gerber aperture, because the
               // polyfill only fills to the outline of the poly.
               first = TRUE;
               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  p.x = pnt->x * scale;
                  if (mirror) p.x = -p.x;
                  p.y = pnt->y * scale;
                  p.bulge = pnt->bulge;
                  if (mirror)    p.bulge = -p.bulge;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);
                  // here deal with bulge
                  if (first)
                  {
                     wgerb_Graph_Polyline(p, dcode, apshape, width);
                     first = FALSE;
                     firstp = p;
                  }
                  else
                     wgerb_Graph_Vertex(p);
               }
            }  // while all polygons.
         }
         break;

         case T_TEXT:
         {
            if (cur_negative && !np->isNegative())
            {
               wgerb_write2file("%LPD*%\n");
               cur_negative = FALSE;
            }
            else if (!cur_negative && np->isNegative())
            {
               wgerb_write2file("%LPC*%\n");
               cur_negative = TRUE;
            }

            Point2 point;

            point.x = np->getText()->getPnt().x * scale;
            if (mirror) point.x = -point.x;
            point.y = np->getText()->getPnt().y * scale;
            TransPoint2(&point, 1, &m, insert_x, insert_y);

            double text_rot = rotation + np->getText()->getRotation();

				TextStruct* textStruct = np->getText();
				CDoubleArray lineLengths;
				double spaceratio = doc->getSettings().getTextSpaceRatio(); // turn percent into a decimal
				double maxLineLength = textStruct->getMaxLineLengthInFontUnits(spaceratio, &lineLengths);
				int lineCount = lineLengths.GetSize();
				CPoint2d initialCharacterPosition = textStruct->getInitialCharacterPosition(maxLineLength, lineCount, spaceratio);

				Point2 initialPosPoint;
				initialPosPoint.x = initialCharacterPosition.x;
				if (mirror)
					initialPosPoint.x = -initialPosPoint.x;
				initialPosPoint.y = initialCharacterPosition.y;

				Mat2x2      mtext_rot;
				RotMat2(&mtext_rot, text_rot);
				TransPoint2(&initialPosPoint, 1, &mtext_rot, 0, 0);

				point.x += initialPosPoint.x * scale;
				point.y += initialPosPoint.y * scale;


            // text is mirrored if mirror is set or text.mirror but not if none or both
            int text_mirror = np->getText()->getResultantMirror(mirror);
            if (np->getText()->getMirrorDisabled())
					text_mirror = 0;

				FontStruct* fontStruct = CFontList::getFontList().getFont(np->getText()->getFontNumber());
				write_gerber_text(np->getText()->getText(), point.x, point.y, 
                  np->getText()->getHeight() * scale, np->getText()->getWidth() * scale, text_rot, 
                  np->getText()->isProportionallySpaced(), np->getText()->getOblique(), text_mirror,
                  doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getDcode(), fontStruct);
         }
         break;
         
         case T_INSERT:
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror & MIRROR_FLIP) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot;
            
            if (mirror & MIRROR_FLIP)
               block_rot = rotation - np->getInsert()->getAngle();
            else
               block_rot = rotation + np->getInsert()->getAngle();

            if (cur_negative && !np->isNegative())
            {
               wgerb_write2file("%LPD*%\n");
               cur_negative = FALSE;
            }
            else
            if (!cur_negative && np->isNegative())
            {
               wgerb_write2file("%LPC*%\n");
               cur_negative = TRUE;
            }

				int blocknum = np->getInsert()->getBlockNumber();
            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ((block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layer = Get_ApertureLayer(doc, np, block, insertLayer);
               int centered = is_aperture_centered(block->getShape());

               int newlayer = doc->get_layer_mirror(layer, mirror);

               if ( doc->get_layer_visible(layer, mirror) &&
                    LayerInFile(FileLayerPtr, newlayer) ) // only write data item if it goes in this file
               {
                  int r = round(RadToDeg(block_rot + block->getRotation()));
                  while (r < 0)
							r += 360;
                  while (r >= 360)
							r -= 360;

                  int dcode = block->getDcode();

						if (block->getShape() != T_COMPLEX && !centered && r != block->getRotation())
						{
							CString tmp;
							tmp.Format("%s_A%d", block->getName(), r);

							BlockStruct *b = doc->Find_Block_by_Name(tmp, 0);
							if (b)
								dcode = b->getDcode();
							else
							{
								fprintf(flog, "Aperture %s not defined\n", tmp);
								display_error++;
								dcode = 99;
							}
						}

						// offset point2
						Mat2x2 n;
						RotMat2(&n, rotation + block->getRotation());

						Point2 off;
						off.x = block->getXoffset() * scale;
						off.y = block->getYoffset() * scale;
						if (mirror)
							off.x = -off.x;

						TransPoint2(&off, 1, &n, 0.0, 0.0);

						point2.x += off.x;
						point2.y += off.y;

                  // for debugging
                  //CString comment;
                  //comment.Format("Entity=%d",np->getEntityNumber());
                  //wgerb_comment(comment);

						if (block->getShape() != T_COMPLEX)
						{
							// Still use flash for T_THERMAL and others
							wgerb_Flash_App(point2, dcode);
						}
						else
						{
							// Draw complex aerture
							int dcode = block->getDcode();
							double rot = block->getRotation();
							CString  appname = block->getName();
							int complexAppBlockNum = (int)(block->getSizeA());
							BlockStruct *complexAppBlock = doc->Find_Block_by_Num((int)(block->getSizeA()));

							if (!complexAppBlock)
								continue;
							if (complexAppBlock->getDataList().IsEmpty())
								continue;

							int mir = ((mirror & MIRROR_FLIP) != 0);
							//POSITION mirrorBlockPos = mirBlocks.GetHeadPosition();
							//while (mirrorBlockPos != NULL)
							//	{
							//	CString complexblock = mirBlocks.GetNext(mirrorBlockPos);
							//	if (appname.Find(complexblock)> -1)
							//	{
							//		mir = 1;
							//		break;
							//	}
							//}
							////sprintf(prosa, "%%AM%s*\n", (const char*)getLegalMacroName(appname));
							
							// Draw the camcad aperture
							Gerb_DrawAperture(&(complexAppBlock->getDataList()), 
								point2.x, point2.y, block_rot + rot, mir, scale, "aperturename");

						}
               }
            }
            else
            {
               int block_layer;
               if (!doc->IsFloatingLayer(np->getLayerIndex()) || insertLayer == -1)
                  block_layer = np->getLayerIndex();
               else 
                  block_layer = insertLayer;

               Gerb_WriteData(&(block->getDataList()),
                     point2.x,
                     point2.y,
                     block_rot,
                     block_mirror,
                     scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, unitScale);
               // here write out attribute
               write_attributes(np->getAttributesRef(), point2.x, point2.y,
                     block_rot, block_mirror, scale * np->getInsert()->getScale());

            }
         break;
      } // end switch
   } // end for
} // end Gerb_WriteData

/******************************************************************************
* get_rotated_aperture
*/
static int get_rotated_aperture(const char *n)
{
   int i;

   for (i=0; i<approtcnt; i++)
   {
      if (approtarray[i]->appname.Compare(n) == 0)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static int get_rotated_aperture(const char *n, int rot)
{
   int   i;

   for (i=0;i<approtcnt;i++)
   {
      if (approtarray[i]->originalname.Compare(n) == 0 && approtarray[i]->originalrot == rot)
         return i;
   }

   return -1;
}

//--------------------------------------------------------------
static CString store_rotated_aperture(const char *bname, int rot)
{
   CString  tmp;
   tmp.Format("%s_A%d", bname, rot);

/*
   while (Graph_Block_Exists(doc, tmp, -1))
   {
      tmp += "_";
   }
*/

   if (get_rotated_aperture(tmp) < 0)
   {
      GERBERAppRotStruct *c = new GERBERAppRotStruct;
      approtarray.SetAtGrow(approtcnt, c);
      approtcnt++;
      c->originalname = bname;
      c->originalrot = rot;
      c->appname = tmp;
      c->macroname = "";
   }

   return tmp;
}


/******************************************************************************
* Gerb_Find_Rotated_Apertures
*/
void Gerb_Find_Rotated_Apertures(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale,
                    int embeddedLevel, int insertLayer, 
                    double unitScale) // unitScale only used for apertures because they are not scaled
{
   Mat2x2 m;
   RotMat2(&m, rotation);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos != NULL)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      // only draw visible layer (Inserts do not have layers)
      if (data->getDataType() != T_INSERT)
      {
         int layer;

         if (doc->IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
            layer = insertLayer;
         else
            layer = data->getLayerIndex();

         if (!doc->get_layer_visible(layer, mirror))
            continue;

         layer = doc->get_layer_mirror(layer, mirror);

         if (!LayerInFile(FileLayerPtr, layer)) // only write data item if it goes in this file
            continue;
      }

      // inserts have no initial layers
      switch (data->getDataType())
      {
      case T_INSERT:
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
         double block_rot;
         
         if (mirror)
            block_rot = rotation - data->getInsert()->getAngle();
         else
            block_rot = rotation + data->getInsert()->getAngle();

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());

         if (block->getFlags() & BL_APERTURE || block->getFlags() & BL_BLOCK_APERTURE)
         {
            int layer = Get_ApertureLayer(doc, data, block, insertLayer);
            int centered = is_aperture_centered(block->getShape());

            int newlayer = doc->get_layer_mirror(layer, mirror);

            if (doc->get_layer_visible(layer, mirror) &&
                  LayerInFile(FileLayerPtr, newlayer) ) // only write data item if it goes in this file
            {
               int r = round(RadToDeg(block_rot + block->getRotation()));

               while (r < 0)
						r += 360;

               while (r >= 360)
						r -= 360;

               if (r != block->getRotation() && !centered)
               {
                  if (get_rotated_aperture(block->getName(), r) > -1)
							break;   // already stored.

                  fprintf(flog, "Rotated aperture detected and added to aperture list : D%d Angle %d\n",block->getDcode(),r);
                  display_error++;
               
                  // here add to list.
                  int err;
                  CString  tmp;

                  tmp = store_rotated_aperture(block->getName(), r);   // the name could have changed

                  // 
                  double sizeA = block->getSizeA();
                  double sizeB = block->getSizeB();
                  double offX = block->getXoffset();
                  double offY = block->getYoffset();
                                                                                                         
                  // on square there in no B value.
                  if (block->getShape() == T_SQUARE)
                     sizeB = sizeA;

                  if (block->getShape() == T_COMPLEX)
                  {
                     Graph_Aperture(tmp, block->getShape(), block->getSizeA(), block->getSizeB(), 
                           block->getXoffset(), block->getYoffset(), DegToRad(r), 0, BL_APERTURE, FALSE, &err);
                  }
                  else
                  {
                     if ((r % 90) == 0 && 
                           ( block->getShape() == T_SQUARE ||
                           block->getShape() == T_RECTANGLE ||
                           block->getShape() == T_OBLONG ))
                     {
                        // here is a 90 degree rotation of "standard" shapes
                        for (int i = 0;i < (r / 90); i ++)
                        {
                           double t;
                           t = sizeA;
                           sizeA = sizeB;
                           sizeB = t;
                           t = offX;
                           offX = -offY;
                           offY = t; 
                        }

                        Graph_Aperture(tmp, block->getShape(), sizeA, sizeB, 
                              offX, offY, 0.0, 0, BL_APERTURE, FALSE, &err);
                     }
                     else
                     {
                        CString  mmacroname;

                        // here check that macroname does not exist yet
                        mmacroname = store_macroname_aperture(tmp);
/*
                        if (Graph_Block_Exists(doc, mmacroname, -1))
                           break;
*/
                        Graph_Block_On(GBO_APPEND, mmacroname, -1, BL_GLOBAL);  // generate sub block

                        switch (block->getShape())
                        {
								case T_RECTANGLE:
								case T_SQUARE:
                           {
                              double x1,y1,x2,y2,x3,y3,x4,y4;

                              // lower left
                              Rotate(-sizeA/2+offX,-sizeB/2+offY,1.0*r,&x1,&y1);
                              Rotate(-sizeA/2+offX,sizeB/2+offY,1.0*r,&x2,&y2);
                              Rotate(sizeA/2+offX,sizeB/2+offY,1.0*r,&x3,&y3);
                              Rotate(sizeA/2+offX,-sizeB/2+offY,1.0*r,&x4,&y4);
                              // a zerosize polygon
                              Graph_PolyStruct(Graph_Level("0","",1), 0,0);
                              Graph_Poly(NULL,0,  1, 0, 1);
                              Graph_Vertex(x1, y1, 0.0);
                              Graph_Vertex(x2, y2, 0.0);
                              Graph_Vertex(x3, y3, 0.0);
                              Graph_Vertex(x4, y4, 0.0);
                              Graph_Vertex(x1, y1, 0.0);
                           }
                           break;

								case T_OBLONG:
                           {
                              // a line with round ends
                              double w;;
                              double x1,y1,x2,y2;

                              if (sizeA < sizeB)
                              {
                                 w = sizeA;
                                 Rotate (0.0, -(sizeB/2 - w/2),1.0*r, &x1, &y1); 
                                 Rotate (0.0, (sizeB/2 - w/2), 1.0*r, &x2, &y2); 
                              }
                              else
                              {
                                 w = sizeB;
                                 Rotate ( -(sizeA/2 - w/2), 0.0, 1.0*r, &x1, &y1); 
                                 Rotate (  (sizeA/2 - w/2), 0.0, 1.0*r, &x2, &y2); 
                              }

                              Graph_Line(Graph_Level("0","",1),   // this is a float layer
                                 x1, y1, x2, y2,
                                 0, Graph_Aperture("", T_ROUND, w,
                                    0.0,0.0, 0.0, 0.0, 0, BL_WIDTH, 0, &err), 0);
                           }
                           break;

								default:
                           {
                              CPolyList *poly_list;

                              if ((poly_list = ApertureToPoly_Base(block, 0.0, 0.0, DegToRad(r), 0)) == NULL)
                                 break;

                              CPoly *poly;
                              CPnt *pnt;
                              POSITION polyPos, pntPos;
                              
                              Graph_PolyStruct(Graph_Level("0","",1), 0,0);
                              polyPos = poly_list->GetHeadPosition();

                              while (polyPos != NULL)
                              {
                                 Graph_Poly(NULL,0,  0, 0, 0);
                                 poly = poly_list->GetNext(polyPos);
                                 pntPos = poly->getPntList().GetHeadPosition();

                                 while (pntPos != NULL)
                                 {
                                    pnt = poly->getPntList().GetNext(pntPos);
                                    Graph_Vertex(pnt->x, pnt->y, pnt->bulge);
                                 }
                              }

                              FreePolyList(poly_list);
                           }

                           break;
                        }

                        Graph_Block_Off();
                        Graph_Complex(tmp, 0, mmacroname, 0.0, 0.0, 0.0);
                     }
                  }
               }
            }
         }
         else
         {
            int block_layer;

            if (!doc->IsFloatingLayer(data->getLayerIndex()) || insertLayer == -1)
               block_layer = data->getLayerIndex();
            else 
               block_layer = insertLayer;

            Gerb_Find_Rotated_Apertures(&block->getDataList(),
                  0.0, 0.0,
                  block_rot,
                  block_mirror,
                  scale * data->getInsert()->getScale(),
                  embeddedLevel+1, block_layer, unitScale);
         }

         break;
      } // end switch
   } // end for
} // end Gerb_Find_Rotated_Apertures

/*****************************************************************************/
/* 
   Gerb_WriteChar
   flag mirror
   int oblique; // angle to slant - deg - 0 < oblique < 90]
*/
void Gerb_WriteChar(CPolyList *polylist, double insert_x, double insert_y,
      double rotation, double scale, char mirror, int oblique, int dcode)
{
   Mat2x2   m;
   RotMat2(&m, rotation);

   int   first;
   CPoly *poly;
   CPnt  *pnt;
   POSITION polyPos, pntPos;
   Point2   p, firstp;

   // loop thru polys
   polyPos = polylist->GetHeadPosition();
   while (polyPos != NULL)
   {
      poly = polylist->GetNext(polyPos);

      first = TRUE;
      pntPos = poly->getPntList().GetHeadPosition();
      while (pntPos != NULL)
      {
         pnt = poly->getPntList().GetNext(pntPos);
         p.x = pnt->x * scale;
         if (mirror) p.x = -p.x;
         p.y = pnt->y * scale;
         p.bulge = pnt->bulge;
         TransPoint2(&p, 1, &m, insert_x, insert_y);
         // here deal with bulge
         if (first)
         {
            wgerb_Graph_Polyline(p, dcode, T_ROUND, 1);
            first = FALSE;
            firstp = p;
         }
         else
            wgerb_Graph_Vertex(p);
      }
   }

   return;
} // end DrawChar

/*****************************************************************************/
/*
   from fill_polygon
*/
int gerber_fillin(double xa, double xe, double y)
{
   Point2 start, end;

   // do not draw a line, if the line is smaller it's fill code.
   if (fabs(xa - xe) < fill_width)  return 1;

   start.x = xa;
   end.x = xe;
   start.y = end.y = y;
   wgerb_Line(start, end, fillDCode, 0, 0.0);

   return 1;
}

static void writeApertureMacroPoly(CPoly* poly,double insert_x, double insert_y,double scale,int mirror,Mat2x2& m,
               int apshape,double width,BOOL onoff)
{
	// For use in Gerber "aperture macro" definition

   int cnt = 0;
   POSITION pntPos;
   CPnt* pnt;
   Point2 p;

   // cnt is vertex, but gerber needs pairs
   CString  tmp;
   Point2   firstp;
   cnt = 0;
   // first draw a outline with the correct Gerber aperture, because the
   // polyfill only fills to the outline of the poly.
   pntPos = poly->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      pnt = poly->getPntList().GetNext(pntPos);
      p.x = pnt->x * scale;

      if (mirror)
			p.x = -p.x;

      p.y = pnt->y * scale;
      p.bulge = pnt->bulge;
      TransPoint2(&p, 1, &m, insert_x, insert_y);

      if (cnt)
      {
         // add line end length
         if (apshape == T_SQUARE || apshape == T_RECTANGLE)
         {
            double newP1x, newP1y, newP2x, newP2y;
            FindPointOnLine(firstp.x, firstp.y, p.x, p.y, -width/2, &newP1x, &newP1y);
            FindPointOnLine(p.x, p.y, firstp.x, firstp.y, -width/2, &newP2x, &newP2y);

            tmp.Format("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
               onoff, width, newP1x, newP1y, newP2x, newP2y);
            wgerb_write2file(tmp);

         }
         else
         {

				if (fabs(firstp.bulge) > SMALLNUMBER)
				{
					// Cannot output arc so break the arc down to little segment
					double cx, cy, r, sa;
					double da = atan(mirror?-firstp.bulge:firstp.bulge) * 4;
					ArcPoint2Angle(firstp.x, firstp.y, p.x, p.y, da, &cx, &cy, &r, &sa);

					// make positive start angle.
					while (sa < 0)
						sa += PI2;

					int ppolycnt = 255;
					Point2 ppoly[255];

					// start center
					ArcPoly2(firstp.x, firstp.y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(5));

					for (int i=0; i<ppolycnt-1; i++)
					{
						Point2 *p1 = &ppoly[i];
						Point2 *p2 = &ppoly[i+1];

						tmp.Format("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
							onoff, width, p1->x, p1->y, p2->x, p2->y);
						wgerb_write2file(tmp);

						// circle on top
						tmp.Format("1,%d,%1.5lf,%1.5lf,%1.5lf*\n", onoff, width, p1->x, p1->y);
						wgerb_write2file(tmp);

						// circle on bottom
						tmp.Format("1,%d,%1.5lf,%1.5lf,%1.5lf*\n", onoff, width, p2->x, p2->y);
						wgerb_write2file(tmp);
					}
				}
				else
				{
					tmp.Format("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
						onoff, width, firstp.x, firstp.y, p.x, p.y);
					wgerb_write2file(tmp);

					// circle on top
					tmp.Format("1,%d,%1.5lf,%1.5lf,%1.5lf*\n", onoff, width, firstp.x, firstp.y);
					wgerb_write2file(tmp);

					// circle on bottom
					tmp.Format("1,%d,%1.5lf,%1.5lf,%1.5lf*\n", onoff, width, p.x, p.y);
					wgerb_write2file(tmp);
				}
         }
      }

      firstp = p;
      cnt++;
   }
}

static void writeApertureMacroClosedPoly(CPoly* poly,double insert_x, double insert_y,double rotation,double scale,int mirror,BOOL onoff)
{
   int cnt = 0;
   POSITION pntPos;
   CPnt* pnt;
   Point2 p;

   CString tmp = "";
	CString output = "";

   Mat2x2 mm;
   RotMat2(&mm, 0.0); // rotation is done on the end of the record.

   // first draw a outline with the correct Gerber aperture, because the
   // polyfill only fills to the outline of the poly.
   pntPos = poly->getPntList().GetHeadPosition();
   while (pntPos != NULL)
   {
      pnt = poly->getPntList().GetNext(pntPos);
      p.x = pnt->x * scale;

      if (mirror) p.x = -p.x;

      p.y = pnt->y * scale;
      p.bulge = pnt->bulge;
      TransPoint2(&p, 1, &mm, insert_x, insert_y);

		if (fabs(p.bulge) > SMALLNUMBER && pntPos)
		{
			// Cannot output arc so break the arc down to little segment

			// Get the next point
			Point2 p2;
			pnt = poly->getPntList().GetNext(pntPos);
			p2.x = pnt->x * scale;
			p2.y = pnt->y * scale;
			p2.bulge = pnt->bulge;

			if (mirror) p2.x = -p2.x;

			TransPoint2(&p2, 1, &mm, insert_x, insert_y);

			double cx, cy, r, sa;
			double da = atan(mirror?-p.bulge:p.bulge) * 4;
			ArcPoint2Angle(p.x, p.y, p2.x, p2.y, da, &cx, &cy, &r, &sa);

			// make positive start angle.
			while (sa < 0)
				sa += PI2;

			int ppolycnt = 255;
			Point2 ppoly[255];

			// start center
			ArcPoly2(p.x, p.y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(5));

			for (int i=0; i<ppolycnt; i++)
			{
				Point2 *pp = &ppoly[i];

				tmp.Format(",%1.5lf,%1.5lf", pp->x, pp->y);
				output += tmp;
				cnt++;
			}
		}
		else
		{
			tmp.Format(",%1.5lf,%1.5lf", p.x, p.y);
			output += tmp;
			cnt++;
		}
   }

   //if (cnt > 51)
   //{
   //   fprintf(flog,"More than 50 not allowed vertices in Aperture.\n");
   //   display_error++;
   //   break;
   //}

	tmp.Format("4,%d,%d%s,%1.5lf*\n", onoff, cnt-1, output, RadToDeg(rotation));

   wgerb_write2file(tmp);
}

//--------------------------------------------------------------
// no text records are translated.
void Gerb_WriteApertureMacroData(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale, const char *appname)
{
   Mat2x2      m;
   Point2      point2;
   POSITION    pos;
   DataStruct  *np;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);     

      // inserts have no initial layers
      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            //CPnt  *pnt;
            //POSITION pntPos;
            POSITION polyPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden()) continue;

               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();
               double widthInPageUnits = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
               double width = widthInPageUnits * scale;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL onoff      = 1; // on
               
               if (poly->isVoid() || np->isNegative()) // negative
                  onoff = 0;

               int cnt = 0;
               Point2   p;

               double cx, cy, radius;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  radius += widthInPageUnits/2.;
                  p.x = cx;

                  if (mirror)
						   p.x = -p.x;

                  p.y = cy;
                  TransPoint2(&p, 1, &m, insert_x, insert_y);

                  CString  tmp;
                  tmp.Format("1,%d,%1.5lf,%1.5lf,%1.5lf*\n",onoff,radius*2*scale, p.x, p.y);
                  wgerb_write2file(tmp);
               }
               else if (!closed)
               {
                  writeApertureMacroPoly(poly,insert_x,insert_y,scale,mirror,m,apshape,width,onoff);
               }
               else
               {
                  // closed

                  writeApertureMacroPoly(poly,insert_x,insert_y,scale,mirror,m,apshape,width,onoff);
                  writeApertureMacroClosedPoly(poly,insert_x,insert_y,rotation,scale,mirror,onoff);

               } 
            }  
         }
         break;

      case T_INSERT:
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;

         if (mirror) point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot;

         if (mirror)
            block_rot = rotation - np->getInsert()->getAngle();
         else
            block_rot = rotation + np->getInsert()->getAngle();

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // wgerb_write2file("Flash\n");
         }
         else
         {
            Gerb_WriteApertureMacroData(&(block->getDataList()),
                  point2.x,
                  point2.y,
                  block_rot,
                  block_mirror,
                  scale * np->getInsert()->getScale(), appname);
         }
         break;
      } // end switch
   } // end for
} // end Gerb_WriteApertureMacroData  for complex apertures

//--------------------------------------------------------------
static void drawPoly(CPoly* poly,double insert_x, double insert_y,double scale,int mirror,Mat2x2& m,
               int apshape,double width,BOOL onoff)
{
   int cnt = 0;
   CPnt* pnt;
   Point2 p;

   CString  tmp;
   Point2   firstp;
   cnt = 0;
   
   for (POSITION pntPos = poly->getPntList().GetHeadPosition();pntPos != NULL;)
   {
      pnt = poly->getPntList().GetNext(pntPos);
      p.x = pnt->x * scale;

      if (mirror)
			p.x = -p.x;

      p.y = pnt->y * scale;
      p.bulge = pnt->bulge;
      TransPoint2(&p, 1, &m, insert_x, insert_y);

      if (cnt > 0)
      {
         // add line end length
         if (apshape == T_SQUARE || apshape == T_RECTANGLE)
         {
            double newP1x, newP1y, newP2x, newP2y;
            FindPointOnLine(firstp.x, firstp.y, p.x, p.y, -width/2, &newP1x, &newP1y);
            FindPointOnLine(p.x, p.y, firstp.x, firstp.y, -width/2, &newP2x, &newP2y);

            tmp.Format("20,%d,%1.5lf,%1.5lf,%1.5lf,%1.5lf,%1.5lf,0.0*\n",
               onoff, width, newP1x, newP1y, newP2x, newP2y);
            wgerb_write2file(tmp);

         }
         else
         {
				if (fabs(firstp.bulge) > SMALLNUMBER)
				{
					double cx, cy, r, sa;
					double da = atan(mirror ? -firstp.bulge : firstp.bulge) * 4;
					ArcPoint2Angle(firstp.x, firstp.y, p.x, p.y, da, &cx, &cy, &r, &sa);

               static bool useGerberArcs = true;

               if (useGerberArcs)
               {
						if (cnt == 1)
							wgerb_Pen_Up(firstp);
                  wgerb_Arc(Point2(cx,cy),r,sa,da,wgerb_getCurrentAperture(),true);
						cnt++;
               }
               else
               {
						// Cannot output arc so break the arc down to little segment
					   // make positive start angle.
					   while (sa < 0)
						   sa += PI2;

					   int ppolycnt = 255;
					   Point2 ppoly[255];

					   // start center
					   ArcPoly2(firstp.x, firstp.y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(5));

					   for (int i=0; i<ppolycnt-1; i++)
					   {
						   Point2 *p1 = &ppoly[i];
						   Point2 *p2 = &ppoly[i+1];

						   if (i == 0)
						   { // Pen Up only to first point
							   wgerb_Pen_Up(*p1);
						   }

						   wgerb_Pen_Down(*p2);
					   }
               }
				}
				else
				{
					if (cnt == 1)
               {
                  wgerb_Pen_Up(firstp);
               }

					wgerb_Pen_Down(p);
				}
         }
      }

      firstp = p;
      cnt++;
   }
}

static void drawClosedPoly(CPoly* poly,double insert_x, double insert_y,double rotation,double scale,int mirror,BOOL onoff)
{
   int cnt = 0;
   CPnt* pnt;
   Point2 p;

   CString tmp = "";
	CString output = "";

   Mat2x2 mm;
   RotMat2(&mm, rotation); 
   
   for (POSITION pntPos = poly->getPntList().GetHeadPosition();pntPos != NULL;)
   {
      pnt = poly->getPntList().GetNext(pntPos);
      p.x = pnt->x * scale;

      if (mirror) p.x = -p.x;

      p.y = pnt->y * scale;
      p.bulge = pnt->bulge;
      TransPoint2(&p, 1, &mm, insert_x, insert_y);

		if (fabs(p.bulge) > SMALLNUMBER && pntPos != NULL)
		{
			// Get the next point
			Point2 p2;
			pnt = poly->getPntList().GetNext(pntPos);
			p2.x = pnt->x * scale;
			p2.y = pnt->y * scale;
			p2.bulge = pnt->bulge;

			if (mirror) p2.x = -p2.x;

			TransPoint2(&p2, 1, &mm, insert_x, insert_y);

			double cx, cy, r, sa;
			double da = atan(mirror?-p.bulge:p.bulge) * 4;
			ArcPoint2Angle(p.x, p.y, p2.x, p2.y, da, &cx, &cy, &r, &sa);

         static bool useGerberArcs = true;

         if (useGerberArcs)
         {
				if (cnt == 0)
					wgerb_Pen_Up(p);
				else
					wgerb_Pen_Down(p);
            wgerb_Arc(Point2(cx,cy),r,sa,da,wgerb_getCurrentAperture(),true);
				cnt++;
         }
         else
         {
			   // make positive start angle.
			   while (sa < 0)
				   sa += PI2;

			   int ppolycnt = 255;
			   Point2 ppoly[255];

			   // start center
			   ArcPoly2(p.x, p.y, cx, cy, da, 1.0, ppoly, &ppolycnt, DegToRad(5));

			   for (int i=0; i<ppolycnt; i++)
			   {
				   Point2 *pp = &ppoly[i];

				   if (cnt == 0)
					   wgerb_Pen_Up(*pp);
				   else
					   wgerb_Pen_Down(*pp);

				   cnt++;
			   }
         }
		}
		else
		{
			if (cnt == 0)
				wgerb_Pen_Up(p);
         else
				wgerb_Pen_Down(p);

			cnt++;
		}
   }

}

//--------------------------------------------------------------
void Gerb_DrawAperture(CDataList *DataList,
                    double insert_x, double insert_y,
                    double rotation,
                    int mirror,
                    double scale, const char *appname)
{
   Mat2x2      m;
   Point2      point2;
   POSITION    pos;
   DataStruct  *np;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);      

      // inserts have no initial layers
      switch(np->getDataType())
      {
      case T_POLY:
         {
            CPoly *poly;
            //CPnt  *pnt;
            //POSITION pntPos;
            POSITION polyPos;

            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);

               if (poly->isHidden()) continue;

					int dcode = doc->getWidthTable()[poly->getWidthIndex()]->getDcode();
               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();
               double widthInPageUnits = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA();
               double width = widthInPageUnits * scale;

					wgerb_Load_Aperture(dcode);

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL onoff      = 1; // on

               
               if (poly->isVoid() || np->isNegative()) // negative
                  onoff = 0;

               int cnt = 0;
               Point2   center;

               double cx, cy, radius;

               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  
                  center.x = cx;
						radius += widthInPageUnits/2.;
						radius *= scale;

                  if (mirror) center.x = -center.x;

                  //p.y = cy;
						center.y = cy;
                  TransPoint2(&center, 1, &m, insert_x, insert_y);

						if (polyFilled && polyfill) wgerb_write2file("G36*\n");  // fill on

						wgerb_Circle(center, radius, dcode);

						if (polyFilled && polyfill) wgerb_write2file("G37*\n");  // fill off

               }
               else if (!closed)
               {
                  drawPoly(poly,insert_x,insert_y,scale,mirror,m,apshape,width,onoff);
               }
               else
               {  // closed
						// If boundary is not zero width then draw the boundary
						// with regular polyline to get gerber to use the aperture.
                  if (width != 0.0)
							drawPoly(poly,insert_x,insert_y,scale,mirror,m,apshape,width,onoff);

						// Draw the filled area, the gerber aperture is not active when
						// fill is on (gerber spec, not our choice)
						if (polyFilled && polyfill) wgerb_write2file("G36*\n");  // fill on
                  drawClosedPoly(poly,insert_x,insert_y,rotation,scale,mirror,onoff);
						if (polyFilled && polyfill) wgerb_write2file("G37*\n");  // fill off
               } 

            }  
         }
         break;

      case T_INSERT:
         // EXPLODEALL - call DoWriteData() recursively to write embedded entities
         point2.x = np->getInsert()->getOriginX() * scale;

         if (mirror) point2.x = -point2.x;

         point2.y = np->getInsert()->getOriginY() * scale;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         // insert if mirror is either global mirror or block_mirror, but not if both.
         int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
         double block_rot;

         if (mirror)
			{
				double insertRot = np->getInsert()->getAngle();// radians
            block_rot = rotation - np->getInsert()->getAngle();
			}
         else
			{
				double insertRot = np->getInsert()->getAngle();// radians
            block_rot = rotation + np->getInsert()->getAngle();
			}

         BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

         if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
         {
            // wgerb_write2file("Flash\n");
         }
         else
         {
            Gerb_DrawAperture(&(block->getDataList()),
                  point2.x,
                  point2.y,
                  block_rot,
                  block_mirror,
                  scale * np->getInsert()->getScale(), appname);
         }

         break;
      } // end switch
   } // end for
} // end Gerb_DrawAperture


