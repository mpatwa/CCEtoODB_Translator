// $Header: /CAMCAD/5.0/Bom.cpp 43    6/17/07 8:50p Kurt Van Ness $

/*****************************************************************************/
/*
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-2000. All Rights Reserved.
*/

/*****************************************************************************/
/*
; sample BOM list for CAMCAD

; All values are loaded into the current active document.

; valtype1 
;    String    1
;    Double    2
;    Integer   3
;    None      4

; valtype2     - only works for string
;    Append    1
;    Overwrite 2
;    return    3     return if exist

; All attributekeys are input keywords

; .ASSIGN attributekey input cc output

; syntax is line1 .attachxxx
;           line2    keyword valtype value
; keyword must be one word, value is delimited by lineend


; If Graph_Block_Reference name and PCB flag not found - ignore
.ATTACH2PCBCOMP  C1  valtype1 valtype2  
        attributekey  attributevalue

  ; Loop thru all devices with this attribute and value and
; attach a new one.
.ATTACH2COMPATTRIBUTE  TYPE 7400 valtype1 valtype2
     attributekey  attributevalue

; If type not found - create one
.ATTACH2DEVICE  7400 valtype1 valtype2
        attributekey  attributevalue

; If layer not found - ignore
.ATTACH2LAYER COMP valtype1 valtype2
        attributekey  attributevalue

; attach to geom also allows the fileblock
; if geom not found - ignore
.ATTACH2GEOM DIP14
        attributekey valtype1 valtype2 attributevalue

; if net not found - ignore
.ATTACH2NET SIGNAL1  valtype1 valtype2
        attributekey attributevalue

; if comppin not found - ignore
.ATTACH2COMPPIN     C1 5 valtype1 valtype2
     attributekey attributevalue   
; end


here are now listings:

// component placement
.PCBCOMPPLACE  Name
units x y rot top/bottom

// Centroid placement
.GEOMCENTROID geomname
units x y rot placelayer

*/
/*****************************************************************************/

// BOM.CPP

#include "stdafx.h"
#include "ccdoc.h"               
#include "bom.h"
#include "file.h"
#include "dbutil.h"
#include "attrib.h"
#include "pcbutil.h"
#include "graph.h"             
#include "centroid.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

/////////////////////////////////////////////////////////////////////////////
// BOMReader dialog
BOMReader::BOMReader(CWnd* pParent /*=NULL*/)
   : CDialog(BOMReader::IDD, pParent)
{
   //{{AFX_DATA_INIT(BOMReader)
   m_logfile = _T("");
   m_macfile = _T("");
   //}}AFX_DATA_INIT
}

void BOMReader::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(BOMReader)
   DDX_Text(pDX, IDC_LOGFILE, m_logfile);
   DDX_Text(pDX, IDC_MAC_FILE, m_macfile);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(BOMReader, CDialog)
   //{{AFX_MSG_MAP(BOMReader)
   ON_BN_CLICKED(IDC_CHANGE_MAC_FILE, OnChangeMacFile)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BOMReader message handlers
void BOMReader::OnChangeMacFile() 
{
   CFileDialog FileDialog(TRUE, "BOM", m_macfile,
         OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "BOM Macro File (*.BOM)|*.BOM||", this);
   if (FileDialog.DoModal() != IDOK) return;

   m_macfile = FileDialog.GetPathName();

   UpdateData(FALSE);
}

//--------------------------------------------------------------
CString  BOM_clean_text(const char *c)
{
   CString  ws;
   int      i;
   int      quote_needed = FALSE;

   ws = "";

   for (i=0; i<(int)strlen(c); i++)
   {
      if (isspace(c[i]))
      {
         quote_needed = TRUE;
      }
      else
      if (c[i] == '\\')
      {
         quote_needed = TRUE;
         ws += '\\';
      }
      else
      if (c[i] == '"')
      {
         quote_needed = TRUE;
         ws += '\\';
      }
      ws += c[i];
   }

   if (quote_needed)
   {
      CString  ws1;
      ws1 = '"';
      ws1 += ws;
      ws1 += '"';
      ws = ws1;
   }

   return ws;
}

//--------------------------------------------------------------
// also set component to loaded
static int BOM_LoopPCBCOMP(CCEtoODBDoc *doc, CDataList *DataList, const char *refname,
                           const char *key, const char *val, int valtype1, int valtype2)
{
   DataStruct *np;
   int found = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT && !STRICMP(np->getInsert()->getRefname(),refname))
         {
            if(!np->getAttributesRef()->getAttribute(standardAttributeLoaded))
               doc->SetUnknownAttrib(&np->getAttributesRef(), ATT_LOADED, "TRUE", SA_OVERWRITE, NULL);

            doc->SetUnknownAttrib(&np->getAttributesRef(), key, val, valtype2, NULL);
            found++;
         }
      }
   }

   return found;
} 

/******************************************************************************
* BOM_LoopPCBCompAttrib
*/
static int BOM_LoopPCBCompAttrib(CCEtoODBDoc *doc, CDataList *DataList, CString compAttribKey, CString compAttribValue,
											CString key, CString value, int valtype1, int valtype2)
{
   int found = 0;
	Attrib *attrib = NULL;
	WORD keyword = doc->IsKeyWord(compAttribKey, 0); 

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      DataStruct *data = DataList->GetNext(pos);

		if (data->getDataType() != dataTypeInsert || data->getInsert()->getInsertType() != insertTypePcbComponent)
			continue;

		// Find the attribute keyword
		if (data->getAttributes() == NULL || !data->getAttributes()->Lookup(keyword, attrib))
			continue;
		if (attrib == NULL)
			continue;

		CString tmpValue = "";		
		if (attrib->getValueType() == VT_INTEGER)
			tmpValue.Format("%d", attrib->getIntValue());
		else if (attrib->getValueType() == VT_DOUBLE)
			tmpValue.Format("%.3f", attrib->getDoubleValue());
		else
         tmpValue = attrib->getStringValue();

		// Check to see if the value is the same
		if (tmpValue.CompareNoCase(compAttribValue))
			continue;

		// Set the attribute
		doc->SetUnknownAttrib(&data->getAttributeMap(), key, value, valtype2, NULL);
      found++;
   }

   return found;
} 

//--------------------------------------------------------------
int BOM_LoopNetList(CCEtoODBDoc *doc, CNetList *NetList, const char *refname, const char *pinname,
                           const char *key, const char *val, int valtype1, int valtype2)
{
   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;
   int            found = 0;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         compPin = net->getNextCompPin(compPinPos);

         // check that this is not a tooling or fiducial
         if (!STRICMP(compPin->getRefDes(), refname) && !STRICMP(compPin->getPinName(), pinname))
         {
            doc->SetUnknownAttrib(&compPin->getAttributesRef(), key, val, valtype2, NULL);
            found++;
         }
      }
   }

   return found;
} /* end BOM_loopNetList */

//-----------------------------------------------------------------------------
void BOM_write_attributes(FILE *fp, CCEtoODBDoc *doc, const char *s, CAttributes* map)
{
   if (map == NULL)
      return;

   WORD keyword;
   Attrib* attrib;

   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      //int k = doc->IsKeyWord(ATT_REFNAME, 0); // return index

      // do not write its own refdes.
      if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))     continue;

      CString  key = BOM_clean_text(doc->getKeyWordArray()[keyword]->out);

      switch (attrib->getValueType())
      {
      case VT_NONE:
         fprintf(fp,"%s %d 2\n\t%s NONE\n",s,attrib->getValueType(), key);
         break;
      case VT_INTEGER:
         fprintf(fp,"%s %d 2\n\t%s %d\n",s,attrib->getValueType(), key, attrib->getIntValue());
         break;
      case VT_UNIT_DOUBLE:
         fprintf(fp,"%s %d 2\n\t%s %lg\n",s,attrib->getValueType(), key, attrib->getDoubleValue());
         break;
      case VT_DOUBLE:
         fprintf(fp,"%s %d 2\n\t%s %lg\n",s,attrib->getValueType(), key, attrib->getDoubleValue());
         break;
      case VT_STRING:
      case VT_EMAIL_ADDRESS:
      case VT_WEB_ADDRESS:
         {
            char *tok, *temp = STRDUP(attrib->getStringValue());
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               if (attrib->getStringValueIndex() != -1)
               {
                  fprintf(fp,"%s %d 2\n\t%s %s\n",s,attrib->getValueType(), key, BOM_clean_text(tok));
               }
               else
               {
                  fprintf(fp,"%s %d 2\n\t%s\n",s,attrib->getValueType(), key);
               }
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }

         break;
      }
   }

   return;
}

//--------------------------------------------------------------
static int BOM_WriteATTACH2COMPPIN(FILE *bom, CCEtoODBDoc *doc, CNetList *NetList)
{
   NetStruct      *net;
   CompPinStruct  *compPin;
   POSITION       compPinPos, netPos;
   int            found = 0;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);

      // SaveAttribs(stream, &net->getAttributesRef());
      compPinPos = net->getHeadCompPinPosition();
      while (compPinPos != NULL)
      {
         CString  attach_string;

         compPin = net->getNextCompPin(compPinPos);

         CString  cc, pp;
         cc = BOM_clean_text(compPin->getRefDes());
         pp = BOM_clean_text(compPin->getPinName());
         attach_string.Format(".ATTACH2COMPPIN %s %s", cc, pp);
         BOM_write_attributes(bom, doc, attach_string, compPin->getAttributesRef());
         found++;
      }
   }

   fprintf(bom,"\n");
   return found;
} /* end BOM_loopNetList */

//--------------------------------------------------------------
static int BOM_WriteATTACH2DEVICE(FILE *bom, CCEtoODBDoc *doc, CTypeList *TypeList)
{
   TypeStruct     *typ;
   POSITION       typePos;
   int            found = 0;

   typePos = TypeList->GetHeadPosition();
   while (typePos != NULL)
   {
      typ = TypeList->GetNext(typePos);
      
      CString  attach_string;

      // SaveAttribs(stream, &net->getAttributesRef());
      attach_string.Format(".ATTACH2DEVICE %s", typ->getName());
      BOM_write_attributes(bom, doc, attach_string, typ->getAttributesRef());
      found++;
   }

   fprintf(bom,"\n");
   return found;
} /* end BOM_looptypeList */

//--------------------------------------------------------------
static void BOM_WriteCOMPONENTData(FILE *wfp, CCEtoODBDoc *doc, FileStruct *file,
      CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer)
{
   Mat2x2      m;
   Point2      point2;
   DataStruct  *np;

   RotMat2(&m, rotation);

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      
      if (np->getDataType() != T_INSERT)        continue;

      switch(np->getDataType())
      {
         case T_INSERT:
         {
            if (np->getInsert()->getInsertType() != INSERTTYPE_PCBCOMPONENT)   break;   

            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror)
               point2.x = -point2.x;

            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot = RadToDeg(rotation + np->getInsert()->getAngle());

            while (block_rot >= 360)   
               block_rot = block_rot - 360;
            while (block_rot < 0)   
               block_rot = block_rot + 360;

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());

            CString compname = BOM_clean_text(np->getInsert()->getRefname());

            fprintf(wfp,".PCBCOMPPLACE %s\n", compname);
            fprintf(wfp,"%lf %lf %1.3lf %s\n",point2.x, point2.y, block_rot, 
               (block_mirror)?"BOTTOM":"TOP");
            fprintf(wfp,"\n");
         } // case INSERT
         break;
      } // end switch
   } // end BOM_WriteCOMPONENTData */
}

//--------------------------------------------------------------
static int write_components(FILE *fp, CCEtoODBDoc *doc, FileStruct *file)
{
   double unitsFactor = 1;
   
   BOM_WriteCOMPONENTData(fp, doc, file,  &(file->getBlock()->getDataList()), 
         file->getInsertX() * unitsFactor, file->getInsertY() * unitsFactor,
         file->getRotation(), file->isMirrored(), file->getScale() * unitsFactor, 0, -1);

   return 1;
}

//--------------------------------------------------------------
static int write_geomcentroids(FILE *fp, CCEtoODBDoc *doc)
{
   DataStruct  *d;
   double      scale = 1;
   CString     unit;

   unit = GetUnitName(doc->getSettings().getPageUnits());

   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);
      if (block == NULL)            continue;
      if (block->getFlags() & BL_WIDTH)   continue;
      if (strlen(block->getName()) == 0) continue;
   
      // find if already exist
      if ((d = centroid_exist_in_block(block)) != NULL)
      {
         fprintf(fp,".GEOMCENTROID %s\n", BOM_clean_text(block->getName()));
         fprintf(fp,"%s %lf %lf %1.3lf\n", unit, 
            d->getInsert()->getOriginX()*scale, d->getInsert()->getOriginY()*scale, RadToDeg(d->getInsert()->getAngle()));
      }
   }

   return 1;
}

//--------------------------------------------------------------
static int BOM_WriteATTACH2PCBCOM(FILE *bom, CCEtoODBDoc *doc, CDataList *DataList)
                           
{
   DataStruct *np;
   int         found = 0;

   POSITION pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
      if (np->getDataType() == T_INSERT)
      {
         if (np->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
         {
            // here 
            CString  attach_string;

            // do not write if attribute is ATT_REFNAME

            attach_string.Format(".ATTACH2PCBCOMP %s", BOM_clean_text(np->getInsert()->getRefname()));
            BOM_write_attributes(bom, doc, attach_string, np->getAttributesRef());
            fprintf(bom,"\n");
            found++;
         }
      }
   } // end for
   return found;
 } /* end BOM_WriteATTACH2PCBCOM */

// UniversalBOMRead
void UniversalBOMRead(CCEtoODBDoc *doc, const char *bomFile)
{
   // return success
   FILE *wfp, *flog;
   FileStruct *file;
   int display_error = FALSE;
	int attributeFlag = 0;

   //here I need to ask of Components 

   // open file for reading
   if ((wfp = fopen(bomFile, "rt")) == NULL)
   {
     CString tmp;
     tmp.Format("Can not open file [%s]", bomFile);
     ErrorMessage(tmp);
     return;
   }

   CString logFile = GetLogfilePath("bom.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   char line[255];
   char *lp;
   long lcnt = 0;

   while (fgets(line, 255, wfp))
   {
      lcnt++;
      if ((lp = get_string(line," \t\n")) == NULL)
         continue;
      CString cmd = lp;
      cmd.TrimLeft();
      cmd.TrimRight();

      if (cmd.Left(1) != '.')
         continue;

      // just written , but never read.
      if (!STRCMPI(lp, ".PCBCOMPPLACE"))
      {
         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2PCBCOMP")) // this also sets a component loaded.
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attach = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString key = lp;
   
         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         // loop throu visible files and find the first compnent
         // write file blocks as exploded
         POSITION pos = doc->getFileList().GetHeadPosition();
         int found = 0;
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
				if (file == NULL || !file->isShown() || file->getBlock() == NULL)
					continue;

            doc->PrepareAddEntity(file);

				//checking the tolerance value 
				if (!STRCMPI(key, ATT_TOLERANCE))
            {
					double ptol, mtol, tol;
					CString t = "";

					check_tolerance(val, &mtol, &ptol, &tol);

					if (tol > -1)
					{
						char tt[50];
						_gcvt(tol, 9, tt);
						found += BOM_LoopPCBCOMP(doc, &(file->getBlock()->getDataList()), attach, key, tt, valtype1, valtype2);
					}

					if (mtol > -1)
					{
						char tt[50];
						_gcvt(mtol, 9, tt);
						found += BOM_LoopPCBCOMP(doc, &(file->getBlock()->getDataList()), attach, ATT_MINUSTOLERANCE, tt, valtype1, valtype2);
					} 

					if (ptol > -1)
					{    
						char tt[50];
						_gcvt(ptol, 9, tt);
						found += BOM_LoopPCBCOMP(doc, &(file->getBlock()->getDataList()), attach, ATT_PLUSTOLERANCE, tt, valtype1, valtype2);
					}
				}
				else           
				{
					found += BOM_LoopPCBCOMP(doc, &(file->getBlock()->getDataList()), attach, key, val, valtype1, valtype2);
				}
         }


         if (!found)
         {
            fprintf(flog, "Could not locate PCBCOMP [%s]\n", attach);
            display_error++;
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2DEVICE"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attach = lp;
   
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line,255,wfp))              
         {
            fprintf(flog, "Format error in BOM at %ld\n",lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n",lcnt);
            display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n",lcnt);
            display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         POSITION pos = doc->getFileList().GetHeadPosition();
         int found = 0;
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (!file->isShown())  
               continue;   // do not write files, which are not shows.
            doc->PrepareAddEntity(file);  

            TypeStruct *t = AddType(file, attach);
            doc->SetUnknownAttrib(&t->getAttributesRef(), key, val, valtype2, NULL);
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2LAYER"))
      {
         fprintf(flog, "Not implemented BOM command [%s]\n", cmd);
         display_error++;
      }
      else if (!cmd.CompareNoCase(".ATTACH2GEOM"))
      {
         fprintf(flog, "Not implemented BOM command [%s]\n", cmd);
         display_error++;
      }
      else if (!cmd.CompareNoCase(".ATTACH2NET"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attach = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString key = lp;
   
         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         // loop throu visible files and find the first compnent
         // write file blocks as exploded
         POSITION pos = doc->getFileList().GetHeadPosition();
         int found = 0;
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (!file->isShown())           
               continue;   // do not write files, which are not shows.
            if (file->getBlock() == NULL)
               continue;   // empty file

            POSITION netPos = file->getNetList().GetHeadPosition();
            while (netPos != NULL)
            {
               NetStruct *net = file->getNetList().GetNext(netPos);

               if (!net->getNetName().CompareNoCase(attach))
               {
                  doc->SetUnknownAttrib(&net->getAttributesRef(), key, val, valtype2, NULL);
                  found++;
                  break;
               }
            }
         }

         if (!found)
         {
            fprintf(flog, "Not implemented BOM command [%s]\n", cmd);
            display_error++;
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2COMPPIN"))
      {
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attach = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attach2 = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);
   
         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString key = lp;

         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         // loop throu visible files and find the first compnent
         // write file blocks as exploded
         POSITION pos = doc->getFileList().GetHeadPosition();
         int found = 0;
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (!file->isShown())           
               continue;   // do not write files, which are not shows.
            if (file->getBlock() == NULL)  
               continue;   // empty file
            doc->PrepareAddEntity(file);

         

         found += BOM_LoopNetList(doc, &file->getNetList(), attach, attach2, key, val, valtype1, valtype2);
         }
         if (!found)
         {
            fprintf(flog, "Could not locate COMPPIN [%s] [%s]\n", attach, attach2);
            display_error++;
         }
      }
      else if (!cmd.CompareNoCase(".ATTACH2COMPATTRIBUTE"))
      {
			attributeFlag = 1;
         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attribKey = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         CString attribValue = lp;

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype1 = atoi(lp);

         if ((lp = get_string(NULL, " \t\n")) == NULL)
            continue;
         int valtype2 = atoi(lp);

         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString key = lp;
   
         if ((lp = get_string(NULL, "\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString val = lp;
         val.TrimLeft();
         val.TrimRight();

         // loop throu visible files and find the first compnent
         // write file blocks as exploded
         POSITION pos = doc->getFileList().GetHeadPosition();
         int found = 0;
         while (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (!file->isShown())           
               continue;   // do not write files, which are not shows.
            if (file->getBlock() == NULL)
               continue;   // empty file

				//checking the tolerance value 
				if (!STRCMPI(key, ATT_TOLERANCE))
				{
					double ptol, mtol, tol;
					CString t;

					check_tolerance(val, &mtol, &ptol, &tol);

					if (tol > -1)
					{
						char tt[50];
						_gcvt(tol, 9, tt);
						found += BOM_LoopPCBCompAttrib(doc, &file->getBlock()->getDataList(), attribKey, attribValue, key, tt, valtype1, valtype2);
					}
					if (mtol > -1)
					{
						char tt[50];
						_gcvt(mtol, 9, tt);
						found += BOM_LoopPCBCompAttrib(doc, &file->getBlock()->getDataList(), attribKey, attribValue, ATT_MINUSTOLERANCE, tt, valtype1, valtype2);
					}           
					if (ptol > -1)
					{    
						char tt[50];
						_gcvt(ptol, 9, tt);
						found += BOM_LoopPCBCompAttrib(doc, &file->getBlock()->getDataList(), attribKey, attribValue, ATT_PLUSTOLERANCE, tt, valtype1, valtype2);
					}
				}
				else 
				{
					found += BOM_LoopPCBCompAttrib(doc, &file->getBlock()->getDataList(), attribKey, attribValue, key, val, valtype1, valtype2);
            }
         }


         if (!found)
         {
            fprintf(flog, "Could not locate attribute %s [%s]\n", attribKey, attribValue);
            display_error++;
         }
   
		}
      else if (!cmd.CompareNoCase(".GEOMCENTROID"))
      {
         if ((lp = get_string(NULL," \t\n")) == NULL)
            continue;
         CString attach = lp;

         lcnt++;
         if (!fgets(line, 255, wfp))               
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         if ((lp = get_string(line, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         CString unit = lp;

         if (unit.CompareNoCase("NONE") == 0)
            continue;   // none mease there is no centroid.

         int curunit = GetUnitIndex(unit);
         
         if (curunit < 0)
         {
            fprintf(flog, "Unknown Unit [%s] error in BOM at %ld\n", unit, lcnt);
            display_error++;
            continue;
         }

         double scale = Units_Factor(curunit, doc->getSettings().getPageUnits());

         if ((lp = get_string(NULL, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         double x = atof(lp) * scale;

         if ((lp = get_string(NULL, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         double y = atof(lp) * scale;

         if ((lp = get_string(NULL, " \t\n")) == NULL)   
         {
            fprintf(flog, "Format error in BOM at %ld\n", lcnt);
            display_error++;
            continue;
         }
         double rot = atof(lp);

         int found = FALSE;

         POSITION pos = doc->getFileList().GetHeadPosition();
         if (pos != NULL)
         {
            file = doc->getFileList().GetNext(pos);
            if (!file->isShown())              
               continue;   // do not write files, which are not shows.
            if (file->getBlock() == NULL)  
               continue;   // empty file
            doc->PrepareAddEntity(file);  

            BlockStruct* centroidb = doc->GetCentroidGeometry();
            DataStruct *d;

            for (int i=0; i<doc->getMaxBlockIndex(); i++)
            {
               BlockStruct *block = doc->getBlockAt(i);
               if (block == NULL)   
                  continue;
               if (attach.Compare(block->getName()) == 0)
               {
                  // find if already exist
                  if ((d = centroid_exist_in_block(block)) != NULL)
                  {
                     // here update position
                     d->getInsert()->setOriginX(x);
                     d->getInsert()->setOriginY(y);
                     d->getInsert()->setAngle(DegToRad(rot));
                  }
                  else
                  {
                     Graph_Block_On(GBO_APPEND, block->getName(),-1,0);
                     // add a centroid if none exist
                     d = Graph_Block_Reference(centroidb->getName(), "", 0, x, y, DegToRad(rot), 0, scale, -1, TRUE);
                     d->getInsert()->setInsertType(insertTypeCentroid);
                     found = TRUE;
                     Graph_Block_Off();
                  }
                  break;
               }
            }
         }
         else
         {
            fprintf(flog, "No File loaded to attach.\n");
            display_error++;
         }

         if (!found)
         {
            fprintf(flog, "Could not locate GEOM [%s]\n", attach);
            display_error++;
         }
      }
      else
      {
         fprintf(flog, "Unknown BOM command [%s]\n", cmd);
         display_error++;
      }
   }  // while loop throu bom file

   // close write file
   fclose(wfp);
   fclose(flog);

	for (POSITION pos=doc->getFileList().GetHeadPosition(); pos!=NULL; doc->getFileList().GetNext(pos))
	{
		FileStruct *pcbFile = doc->getFileList().GetAt(pos);
		if (!pcbFile->isShown())
			continue;

		//pcbFile->HideRealpartsForUnloadedComponents(*doc);
      doc->HideRealpartsForUnloadedComponents(*pcbFile);
	}

   if (display_error && attributeFlag ==0)
      Notepad(logFile);

   return;
}

//-----------------------------------------------------------------------------
void BOM_WriteFiles(const char *filename, CCEtoODBDoc *Doc,FormatStruct *format)
{
   FILE *bom;
   FILE *flog;

   FileStruct  *file;
   int         display_error = FALSE;
   int         found = 0;

	CString logFile = GetLogfilePath("bom.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite file
   {
      MessageBox(NULL, "Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   // open file for writting
   if ((bom = fopen(filename, "wt")) == NULL)
   {
      // error - can not open the file.
      CString  tmp;
      tmp.Format("Can not open %s",filename);
      MessageBox(NULL, tmp,"Error File open", MB_OK | MB_ICONHAND);
      return;
   }

   //CString camcad;
   //camcad.LoadString(IDR_MAINFRAME);

   fprintf(bom,"; BOM listing\n\n");
   fprintf(bom,"; Created by %s\n\n", getApp().getCamCadTitle());

   //fprintf(bom,".PAGEUNIT : %s\n", Settings.PageUnits);

   POSITION pos = Doc->getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      file = Doc->getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getBlock() == NULL)  continue;   // empty file
      fprintf(bom,"; FILENAME : %s\n", file->getName());

      found += BOM_WriteATTACH2PCBCOM(bom, Doc, &(file->getBlock()->getDataList()));
      found += BOM_WriteATTACH2COMPPIN(bom, Doc, &file->getNetList());
      found += BOM_WriteATTACH2DEVICE(bom, Doc, &file->getTypeList());
      write_components(bom, Doc, file);
      write_geomcentroids(bom, Doc);
   }

   fprintf(bom,"; end of bom listing.\n");
   fclose(bom);
   
   fclose(flog);

   if (display_error)
      Notepad(logFile);

   return;
}


// end BOM.CPP
