// $Header: /CAMCAD/5.0/Gerb_Mrg.cpp 40    6/17/07 8:52p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "gerb_mrg.h"
#include "gauge.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CProgressDlg  *progress;

void CCEtoODBDoc::OnGerberInheritecaddata()
{
   CInheritEcadDialog dlg(*this);

   if (!dlg.GetGerberEcadFiles())
      return;
   dlg.DoModal();
}

// CInheritEcadDialog dialog
IMPLEMENT_DYNAMIC(CInheritEcadDialog, CDialog)
CInheritEcadDialog::CInheritEcadDialog(CCEtoODBDoc& camCadDoc,CWnd* pParent /*=NULL*/)
   : CDialog(CInheritEcadDialog::IDD, pParent),
     m_camCadDoc(camCadDoc)
{
   m_gerberFile = m_cadFile = NULL;
   m_boardName  = m_compName = "";
   m_placedBottom = false;
   m_smdKw = m_boardKw = m_compKw = m_pinKw = m_geometryNameKw = m_progressPos = 0;

   m_topPasteLayer    = NULL;
   m_bottomPasteLayer = NULL;

#if defined(Debug_CInheritEcadDialog)
   m_debugFileStruct        = NULL;
   m_debugExtentLayer       = NULL;
   m_debugSearchExtentLayer = NULL;
   m_debugPinLayer          = NULL;
#endif
}

CInheritEcadDialog::~CInheritEcadDialog()
{
}

void CInheritEcadDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInheritEcadDialog, CDialog)
   ON_BN_CLICKED(IDC_HIGHLIGHT_FLASHES_WO_ATTRIBS, OnBnClickedHighlightFlashesWithoutAttributes)
   ON_BN_CLICKED(IDC_HIGHLIGHT_FLASHES_W_ATTRIBS, OnBnClickedHighlightFlashesWithAttributes)
   ON_BN_CLICKED(IDC_CLEAR_HIGHLIGHTS, OnBnClickedClearHighlights)
   ON_WM_CLOSE()
END_MESSAGE_MAP()

// CInheritEcadDialog message handlers
BOOL CInheritEcadDialog::OnInitDialog() 
{
   CDialog::OnInitDialog();

   bool layerErrorFlag = false;
   CString layerListString;

   for (int layerIndex = 0;layerIndex < getCamCadDoc().getLayerCount();layerIndex++)
   {
      LayerStruct* layer = getCamCadDoc().getLayerAt(layerIndex);

      if (layer != NULL && layer->isVisible())
      {
         if (layer->getLayerType() == layerTypePasteTop)
         {
            layerListString += (layerListString.IsEmpty() ? "" : ",") + layer->getName();

            if (m_topPasteLayer == NULL)
            {
               m_topPasteLayer = layer;
            }
            else
            {
               layerErrorFlag = true;
            }
         }
         else if (layer->getLayerType() == layerTypePasteBottom)
         {
            layerListString += (layerListString.IsEmpty() ? "" : ",") + layer->getName();

            if (m_bottomPasteLayer == NULL)
            {
               m_bottomPasteLayer = layer;
            }
            else
            {
               layerErrorFlag = true;
            }
         }
      }
   }

   if (layerErrorFlag)
   {
      formatMessageBox("Only one each of layer types pasteTop and pasteBottom are allowed to be visible - \n" + layerListString);
      return TRUE;
   }

   if ((m_topPasteLayer == NULL) && (m_bottomPasteLayer == NULL))
   {
      formatMessageBox("At least one layer of layer types pasteTop or pasteBottom must be visible");
      return TRUE;
   }

   progress = new CProgressDlg("Inheriting ECAD Data");
   progress->Create(AfxGetMainWnd());
   progress->ShowWindow(SW_SHOW);
   progress->SetStatus("Regenerating Extents...");
   getCamCadDoc().OnRegen();

   m_smdKw          = getCamCadDoc().RegisterKeyWord(ATT_SMDSHAPE, 0, VT_NONE);

   // Create and/or get the keyword index for ECAD_REFDES AND ECAD_PIN
   m_boardKw        = getCamCadDoc().RegisterKeyWord(ATT_ECAD_BOARD_ON_PANEL, 0, VT_STRING);
   m_geometryNameKw = getCamCadDoc().RegisterKeyWord(ATT_ECAD_GEOMETRY_NAME , 0, VT_STRING);
   m_compKw         = getCamCadDoc().RegisterKeyWord(ATT_ECAD_REFDES        , 0, VT_STRING);
   m_pinKw          = getCamCadDoc().RegisterKeyWord(ATT_ECAD_PIN           , 0, VT_STRING);


   ////////////////////////////////////////////////////////////////////////////////////
   // Fill gerber tree
   ////////////////////////////////////////////////////////////////////////////////////
   progress->SetStatus("Gathering gerber data...");

   CUnits units(getCamCadDoc().getUnits());
   double granularity    = units.convertFrom(pageUnitsMils,8.);
   double maxFeatureSize = units.convertFrom(pageUnitsMils,512.);
   CQfeFlashTree flashTree(granularity,maxFeatureSize);
   int totalFlashes = fillGerberTree(&flashTree);

   double fileX     = m_cadFile->getInsertX();
   double fileY     = m_cadFile->getInsertY();
   double fileRot   = m_cadFile->getRotation();
   double fileScale = m_cadFile->getScale();
   int fileMirror   = (m_cadFile->isMirrored() ? MIRROR_FLIP | MIRROR_LAYERS : 0);

   progress->SetStatus("Inheriting ECAD Data...");
   progress->SetRange32(0, ecadPinCount(m_cadFile->getBlock()));
   mergeGerberEcad(m_cadFile->getBlock(),"cadFile",&flashTree,m_cadFile->getTMatrix(), 1);
   mergeGerberEcad(m_cadFile->getBlock(),"cadFile",&flashTree,m_cadFile->getTMatrix(), 2);

   int inherited, uninherited;
   countGerberFlashes(inherited, uninherited);

   m_cadFile->setShow(false);
   getCamCadDoc().UpdateAllViews(NULL);

   CString buf;
   buf.Format("%i", totalFlashes);
   GetDlgItem(IDC_TOTALFLASHES)->SetWindowText(buf);
   buf.Format("%i", uninherited);
   GetDlgItem(IDC_FLASHES_WO_ATTRIBS)->SetWindowText(buf);
   buf.Format("%i", inherited);
   GetDlgItem(IDC_FLASHES_W_ATTRIBS)->SetWindowText(buf);

   progress->ShowWindow(SW_HIDE);
   if (progress->DestroyWindow())
   {
      delete progress;
      progress = NULL;
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CInheritEcadDialog::GetGerberEcadFiles()
{
   POSITION pos = NULL;

   // find gerberFile
   pos = getCamCadDoc().getFileList().GetHeadPosition();
   while (pos)
   {
      FileStruct *file = getCamCadDoc().getFileList().GetNext(pos);
      if (!file->isShown())  continue;
      if (file->getCadSourceFormat() == Type_Gerber)
      {
         if (!m_gerberFile)
            m_gerberFile = file;
         else
         {
            ErrorMessage("Multiple visible Gerber files found!", "Gerber/ECAD Component Merge");
            return FALSE;
         }
      }
   }

   if (!m_gerberFile)
   {
      ErrorMessage("No visible Gerber file found!", "Gerber/ECAD Component Merge");
      return FALSE;
   }

   // find cadFile
   pos = getCamCadDoc().getFileList().GetHeadPosition();
   while (pos != NULL)
   {
      FileStruct *file = getCamCadDoc().getFileList().GetNext(pos);
      if (!file->isShown())        continue;
      if (file == m_gerberFile) continue;

      if (file->getBlockType() == BLOCKTYPE_PCB || file->getBlockType() == BLOCKTYPE_PANEL)
      {
         if (!m_cadFile)
            m_cadFile = file;
         else
         {
            ErrorMessage("Multiple visible CAD files found!", "Gerber/ECAD Component Merge");
            return FALSE;
         }
      }
   }

   if (!m_cadFile)
   {
      ErrorMessage("No visible CAD file found!", "Gerber/ECAD Component Merge");
      return FALSE;
   }

   return TRUE;
}

long CInheritEcadDialog::ecadPinCount(BlockStruct *block)
{
   long pinCount = 0;

   if (!block)
      return 0;

   POSITION pos = block->getDataList().GetHeadPosition();
   while (pos)
   {
      DataStruct *data = m_cadFile->getBlock()->getDataList().GetNext(pos);

      if (data->getDataType() != T_INSERT)
         continue;

      BlockStruct *subblock = NULL;

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:
      case insertTypePcb:
         subblock = getCamCadDoc().getBlockAt(data->getInsert()->getBlockNumber());
         
         pinCount += ecadPinCount(subblock);

         break;
      case insertTypePin:
         if (data->getInsert()->getInsertType() != insertTypePin)
            continue;

         pinCount++;
         break;
      }
   }

   return pinCount;
}

bool CInheritEcadDialog::isPasteFlash(DataStruct& flashData)
{
   bool retval = ((m_topPasteLayer    != NULL && flashData.getLayerIndex() == m_topPasteLayer->getLayerIndex()) ||
                  (m_bottomPasteLayer != NULL && flashData.getLayerIndex() == m_bottomPasteLayer->getLayerIndex())   );

   return retval;
}

int CInheritEcadDialog::fillGerberTree(CQfeFlashTree *flashTree)
{
   int flashCount = 0;
   CUnits units(getCamCadDoc().getPageUnits());

   CTMatrix matrix = m_gerberFile->getTMatrix();

   for (CDataListIterator insertIterator(*(m_gerberFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* flashData = insertIterator.getNext();

      if (isPasteFlash(*flashData))
      {
         // Case #1479 - don't remove prior inherited attributes
         //flashData->getDefinedAttributeMap()->RemoveKey(this->m_boardKw);
         //flashData->getDefinedAttributeMap()->RemoveKey(this->m_geometryNameKw);
         //flashData->getDefinedAttributeMap()->RemoveKey(this->m_compKw);
         //flashData->getDefinedAttributeMap()->RemoveKey(this->m_pinKw);

         CPoint2d origin = flashData->getInsert()->getOrigin2d();
         matrix.transform(origin);
         BlockStruct* geometry = getCamCadDoc().getBlockAt(flashData->getInsert()->getBlockNumber());
         CExtent extent = geometry->getExtent(getCamCadData());

         if (!extent.isValid())
         {
            extent.reset();
            extent.update(origin);
            extent.expand(units.convertFrom(pageUnitsMils,20.));
         }

#if defined(Debug_CInheritEcadDialog)
         addDebugExtent("",extent,flashData->getInsert()->getTMatrix());
#endif

         CGerberFlash* flash = new CGerberFlash(origin.x, origin.y, flashData, extent);

         flashTree->setAt(flash);

         flashCount++;
      }
   }

   return flashCount;
}

void CInheritEcadDialog::countGerberFlashes(int &inherited, int &uninherited)
{
   inherited = 0;
   uninherited = 0;

   for (CDataListIterator insertIterator(*(m_gerberFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* flashData = insertIterator.getNext();

      if (isPasteFlash(*flashData))
      {
         Attrib* value;

         if (flashData->getDefinedAttributeMap()->Lookup(m_pinKw, value))
         {
            inherited++;
         }
         else
         {
            uninherited++;
         }
      }
   }
}

void CInheritEcadDialog::mergeGerberEcad(BlockStruct *block, const CString& refname,CQfeFlashTree *flashTree,CTMatrix& matrix, int passNumber)
{
   static int level = 0;

   level++;

   CString geometryName = block->getName();
   
   for (CDataListIterator insertIterator(*block,dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* data = insertIterator.getNext();

      CPoint2d insertOriginInDocumentSpace = data->getInsert()->getOrigin2d();
      matrix.transform(insertOriginInDocumentSpace);

      BlockStruct *subblock = getCamCadDoc().getBlockAt(data->getInsert()->getBlockNumber());

      switch (data->getInsert()->getInsertType())
      {
      case insertTypePcbComponent:
         {
            m_placedBottom = data->getInsert()->getPlacedBottom();
            m_compName = data->getInsert()->getRefname();

            // use premultiplication
            CTMatrix insertMatrix = data->getInsert()->getTMatrix() * matrix;

            mergeGerberEcad(subblock,data->getInsert()->getRefname(),flashTree,insertMatrix, passNumber);

            break;
         }
      case insertTypePin:
         {
            progress->SetPos(++m_progressPos);

				// Search radius for pass 1, and default for pass 2
				double searchRadius = getCamCadDoc().convertToPageUnits(pageUnitsMils, 1.0);

				// For pass 2 use size of pin to determine search radius
				if (passNumber > 1)
				{
					CExtent geometryExtent = subblock->getExtent(getCamCadData());
					if (geometryExtent.isValid())
					{
						double length = geometryExtent.getSize().getLength();
						searchRadius = length / 2.0;
					}
				}
            
				CExtent searchExtent(insertOriginInDocumentSpace,searchRadius);
            CFlashList foundList;
            int foundCount = flashTree->search(searchExtent,foundList);

#if defined(Debug_CInheritEcadDialog)
            CString pinRef(refname + "." + data->getInsert()->getRefname());

            addDebugPin(pinRef,insertOriginInDocumentSpace);

            CTMatrix identityMatrix;
            addDebugSearchExtent(pinRef,searchExtent,identityMatrix);
#endif

            CDataList attributePads(false);

            int topFoundCount = 0, botFoundCount = 0;
            
            for (POSITION foundPos = foundList.GetHeadPosition();foundPos != NULL;)
            {
               CGerberFlash* flash = foundList.GetNext(foundPos);
               DataStruct* flashData = flash->getFlash();

					// Skip false positives
					CExtent pinExtent = subblock->getExtent(getCamCadData());
					CExtent flashExtent = flash->getExtent();
					if (pinExtent.isValid() && flashExtent.isValid())
					{
						CTMatrix gerbermatrix = m_gerberFile->getTMatrix();
						pinExtent.transform(data->getInsert()->getTMatrix() * matrix);
						flashExtent.transform(flash->getFlash()->getInsert()->getTMatrix() * gerbermatrix);
						if (!pinExtent.intersects(flashExtent)) continue;
					}

               LayerStruct* flashLayer = getCamCadDoc().getLayerAt(flashData->getLayerIndex());

               if (flashLayer == NULL)
                  continue;

					// -1 = not on expected layers
					//  0 = on PAD TOP or SIGNAL TOP
					//  1 = on PAD BOTTOM or SIGNAL BOTTOM
               int padLayer = getPadLayer(data, matrix.getMirror() != data->getInsert()->getGraphicMirrored());

					if (padLayer < 0)
						continue;

					if ((padLayer == 1 && flashLayer->getLayerType() != layerTypePasteBottom) ||
                   (padLayer == 0 && flashLayer->getLayerType() != layerTypePasteTop   )    )
                     continue;

               CAttributes*& flashDataAttributes = flashData->getDefinedAttributeMap();

               Attrib *value;

               if (!m_boardName.IsEmpty() && !flashDataAttributes->Lookup(m_boardKw, value))
                  getCamCadDoc().SetUnknownAttrib(&(flashDataAttributes), getCamCadDoc().getKeyWordArray()[m_boardKw]->cc, m_boardName, SA_OVERWRITE, NULL);

               if (!geometryName.IsEmpty() && !flashDataAttributes->Lookup(m_geometryNameKw, value))
                  getCamCadDoc().SetUnknownAttrib(&(flashDataAttributes), getCamCadDoc().getKeyWordArray()[m_geometryNameKw]->cc, geometryName, SA_OVERWRITE, NULL);

               if (!flashDataAttributes->Lookup(m_compKw, value))
                  getCamCadDoc().SetUnknownAttrib(&(flashDataAttributes), getCamCadDoc().getKeyWordArray()[m_compKw]->cc, m_compName, SA_OVERWRITE, NULL);

					// Assign pin attrib if not already present for both passes 1 and 2
					if (!flashDataAttributes->Lookup(m_pinKw, value))
					{
						attributePads.AddTail(flashData);
					}
					
					if (passNumber > 1)
					{
						// Pass 2, need to adjust pin "duplicate" numbering to coalesce assignments
						// from passes 1 and 2. If this flash is assigned to same component and 
						// same pin, then add to attributePads so they all get renumbered below.
						if (flashDataAttributes->Lookup(m_compKw, value))
						{
							CString flashCompName = value->getStringValue();
							if (flashCompName.Compare(m_compName) == 0)
							{
								if (flashDataAttributes->Lookup(m_pinKw, value))
								{
									CString pinNameAttr = value->getStringValue();
									int tokPos = 0;
									CString flashPinName = pinNameAttr.Tokenize("-", tokPos);
									CString curPinName = data->getInsert()->getRefname();
									if (flashPinName.Compare(curPinName) == 0)
									{
										attributePads.AddTail(flashData);
									}
								}
							}
						}

					}

               if (flashLayer->getLayerType() == layerTypePasteBottom)
                  botFoundCount++;

               if (flashLayer->getLayerType() == layerTypePasteTop)
                  topFoundCount++;
            }

            int index = 1;
            for (POSITION dataPos = attributePads.GetHeadPosition();dataPos != NULL;index++)
            {
               DataStruct* flashData = attributePads.GetNext(dataPos);
               CString value = data->getInsert()->getRefname();

               if (attributePads.GetCount() > 1)
               {
                  value.AppendFormat("-%i", index);
               }

               getCamCadDoc().SetUnknownAttrib(&(flashData->getDefinedAttributeMap()), getCamCadDoc().getKeyWordArray()[m_pinKw]->cc, value, SA_OVERWRITE, NULL);
            }

            break;
         }
      case insertTypePcb:
         {
            m_boardName = data->getInsert()->getRefname();

            // use premultiplication
            CTMatrix insertMatrix = data->getInsert()->getTMatrix() * matrix;

            mergeGerberEcad(subblock,"pcb",flashTree,insertMatrix, passNumber);
            break;
         }
      }
   }

   level--;
}

int CInheritEcadDialog::getPadLayer(DataStruct* data,bool mirrorFlag)
{
	// This function return one of the followings:
	// -1 = not on expected layers
	//  0 = on PAD TOP or SIGNAL TOP
	//  1 = on PAD BOTTOM or SIGNAL BOTTOM
   int retval = -1;

	BlockStruct* padstack = getCamCadDoc().getBlockAt(data->getInsert()->getBlockNumber());

   for (CDataListIterator padIterator(*padstack,dataTypeInsert);padIterator.hasNext();)
	{
      DataStruct* pad = padIterator.getNext();

      LayerStruct* layer = getCamCadDoc().getLayer(pad->getLayerIndex());

		if (layer->getLayerType() == layerTypePadTop || layer->getLayerType() == layerTypeSignalTop)
		{
         retval = (mirrorFlag ? 1 : 0);
         break;
		}
		else if (layer->getLayerType() == layerTypePadBottom || layer->getLayerType() == layerTypeSignalBottom)
		{
         retval = (mirrorFlag ? 0 : 1);
         break;
		}
	}

	return retval;
}

void CInheritEcadDialog::OnBnClickedHighlightFlashesWithoutAttributes()
{
   for (CDataListIterator insertIterator(*(m_gerberFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* flashData = insertIterator.getNext();

      if (isPasteFlash(*flashData))
      {
         Attrib* value;

         if (!(flashData->getAttributesRef() && flashData->getAttributesRef()->Lookup(m_pinKw, value)))
         {
            flashData->setColorOverride(true);
            flashData->setOverrideColor(colorDkGray);

            SelectStruct *s = new SelectStruct();

            s->setData(flashData);
            s->setParentDataList(&(m_gerberFile->getBlock()->getDataList()));
            s->filenum  = m_gerberFile->getFileNumber();
            s->insert_x = m_gerberFile->getInsertX();
            s->insert_y = m_gerberFile->getInsertY();
            s->mirror   = m_gerberFile->isMirrored();
            s->rotation = m_gerberFile->getRotation();
            s->scale    = m_gerberFile->getScale();
            s->layer    = -1;

            getCamCadDoc().DrawEntity(s, 0, TRUE);

            delete s;
         }
      }
   }
}

void CInheritEcadDialog::OnBnClickedHighlightFlashesWithAttributes()
{
   for (CDataListIterator insertIterator(*(m_gerberFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* flashData = insertIterator.getNext();

      if (isPasteFlash(*flashData))
      {
         Attrib* value;

         if (flashData->getAttributesRef() && flashData->getAttributesRef()->Lookup(m_pinKw, value))
         {
            flashData->setColorOverride(true);
            flashData->setOverrideColor(colorLtGray);
            
            SelectStruct *s = new SelectStruct();

            s->setData(flashData);
            s->setParentDataList(&(m_gerberFile->getBlock()->getDataList()));
            s->filenum  = m_gerberFile->getFileNumber();
            s->insert_x = m_gerberFile->getInsertX();
            s->insert_y = m_gerberFile->getInsertY();
            s->mirror   = m_gerberFile->isMirrored();
            s->rotation = m_gerberFile->getRotation();
            s->scale    = m_gerberFile->getScale();
            s->layer    = -1;

            getCamCadDoc().DrawEntity(s, 0, TRUE);

            delete s;
         }
      }
   }
}

void CInheritEcadDialog::OnBnClickedClearHighlights()
{
   for (CDataListIterator insertIterator(*(m_gerberFile->getBlock()),dataTypeInsert);insertIterator.hasNext();)
   {
      DataStruct* flashData = insertIterator.getNext();

      if (isPasteFlash(*flashData))
      {
         bool needRedraw = flashData->getColorOverride();
         flashData->setColorOverride(false);

         if (needRedraw)
         {
            SelectStruct *s = new SelectStruct();

            s->setData(flashData);
            s->setParentDataList(&(m_gerberFile->getBlock()->getDataList()));
            s->filenum  = m_gerberFile->getFileNumber();
            s->insert_x = m_gerberFile->getInsertX();
            s->insert_y = m_gerberFile->getInsertY();
            s->mirror   = m_gerberFile->isMirrored();
            s->rotation = m_gerberFile->getRotation();
            s->scale    = m_gerberFile->getScale();
            s->layer    = -1;

            getCamCadDoc().DrawEntity(s, 0, TRUE);

            delete s;
         }
      }
   }
}

void CInheritEcadDialog::OnOK()
{
   OnBnClickedClearHighlights();

   CDialog::OnOK();
}

void CInheritEcadDialog::OnCancel()
{
   OnBnClickedClearHighlights();

   CDialog::OnCancel();
}

#if defined(Debug_CInheritEcadDialog)

FileStruct* CInheritEcadDialog::getDebugFileStruct()
{
   if (m_debugFileStruct == NULL)
   {
      m_debugFileStruct = getCamCadDoc().Add_File("Debug_CInheritEcadDialog",0);
      m_debugFileStruct->setNotPlacedYet(false);
   }

   return m_debugFileStruct;
}

LayerStruct* CInheritEcadDialog::getDebugExtentLayer()
{
   if (m_debugExtentLayer == NULL)
   {
      m_debugExtentLayer = getCamCadDoc().Add_Layer("DebugExtentLayer");
      m_debugExtentLayer->setColor(colorHtmlDeepPink);
   }

   return m_debugExtentLayer;
}

LayerStruct* CInheritEcadDialog::getDebugSearchExtentLayer()
{
   if (m_debugSearchExtentLayer == NULL)
   {
      m_debugSearchExtentLayer = getCamCadDoc().Add_Layer("DebugSearchExtentLayer");
      m_debugSearchExtentLayer->setColor(colorBlue);
   }

   return m_debugSearchExtentLayer;
}

LayerStruct* CInheritEcadDialog::getDebugPinLayer()
{
   if (m_debugPinLayer == NULL)
   {
      m_debugPinLayer = getCamCadDoc().Add_Layer("DebugPinLayer");
      m_debugPinLayer->setColor(colorHtmlLawnGreen);
   }

   return m_debugPinLayer;
}

void CInheritEcadDialog::addDebugSearchExtent(const CString& comment,const CExtent& extent,const CTMatrix& matrix)
{
   addDebugExtent(comment,extent,matrix,getDebugSearchExtentLayer());
}

void CInheritEcadDialog::addDebugExtent(const CString& comment,const CExtent& extent,const CTMatrix& matrix,LayerStruct* layer)
{
   if (layer == NULL)
   {
      layer = getDebugExtentLayer();
   }

   DataStruct* polyStruct = new DataStruct(dataTypePoly);
   polyStruct->setLayerIndex(layer->getLayerIndex());

   getDebugFileStruct()->getBlock()->getDataList().AddTail(polyStruct);

   CPoly* poly = new CPoly(extent);
   poly->transform(matrix);

   polyStruct->getPolyList()->AddTail(poly);

   CAttributes*& attributes = polyStruct->getDefinedAttributeMap();
   getCamCadDoc().SetAttrib(&(attributes),getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeName),comment);
}

void CInheritEcadDialog::addDebugPin(const CString& pinRef,const CPoint2d& origin)
{
   CUnits units(getCamCadDoc().getPageUnits());
   double size = units.convertFrom(pageUnitsMils,20.);

   DataStruct* polyStruct = new DataStruct(dataTypePoly);

   CPoly* poly = new CPoly();
   poly->addVertex(origin.x - size,origin.y);
   poly->addVertex(origin.x + size,origin.y);
   polyStruct->getPolyList()->AddTail(poly);

   poly = new CPoly();
   poly->addVertex(origin.x,origin.y - size);
   poly->addVertex(origin.x,origin.y + size);
   polyStruct->getPolyList()->AddTail(poly);

   polyStruct->setLayerIndex(getDebugPinLayer()->getLayerIndex());

   getDebugFileStruct()->getBlock()->getDataList().AddTail(polyStruct);

   CAttributes*& attributes = polyStruct->getDefinedAttributeMap();
   getCamCadDoc().SetAttrib(&(attributes),getCamCadDoc().getStandardAttributeKeywordIndex(standardAttributeName),pinRef);
}

#endif

//_____________________________________________________________________________
//CGerberFlash::CGerberFlash()
//{
//   flash = NULL;
//}

CGerberFlash::CGerberFlash(double x, double y, DataStruct *data,const CExtent& extent)
{
   insPoint.x = x;
   insPoint.y = y;
   flash = data;
   m_extent = extent;
}

//CGerberFlash::CGerberFlash(CPoint2d point, DataStruct *data)
//{
//   insPoint.x = point.x;
//   insPoint.y = point.y;
//   flash = data;
//}

CGerberFlash::~CGerberFlash()
{
}

CPoint2d CGerberFlash::getOrigin() const
{
   return insPoint;
}

CExtent CGerberFlash::getExtent() const
{
   return m_extent;
}

CString CGerberFlash::getInfoString() const
{
   return "";
}

bool CGerberFlash::isExtentIntersecting(const CExtent& extent)
{
   return (psInside == extent.pointState(insPoint));
}

bool CGerberFlash::isPointWithinDistance(const CPoint2d& point,double distance)
{
   return (distance <= insPoint.distance(point));
}

bool CGerberFlash::isInViolation(CObject2d& otherObject)
{
   return FALSE;
}

int CGerberFlash::getObjectType() const
{
   return 0;
}

DataStruct* CGerberFlash::getFlash()
{
   return flash;
}







