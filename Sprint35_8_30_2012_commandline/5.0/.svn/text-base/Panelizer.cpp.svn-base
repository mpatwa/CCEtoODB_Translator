
#include "StdAfx.h"
#include "Panelizer.h"
#include "DcaCamCadData.h"
#include "DcaEnumIterator.h"
#include "Units.h"
#include "DcaWriteFormat.h"
#include "CcDoc.h"
#include "Graph.h"
#include "DcaLayer.h"
#include "Debug.h"
#include "ODBC_Lib.h"

CString PanelizerOriginTypeTagToString(PanelizerOriginTypeTag tagValue)
{
   const char* pValue;

   switch (tagValue)
   {
   case panelizerOriginTypeBoardOrigin:  pValue = "Board Origin";  break;
   case panelizerOriginTypeBoardCenter:  pValue = "Board Center";  break;
   default:                              pValue = "?";             break;
   }

   return pValue;
}

PanelizerOriginTypeTag StringToPanelizerOriginTypeTag(const CString& tagValue)
{
   PanelizerOriginTypeTag retval = panelizerOriginTypeUndefined;

   for (EnumIterator(PanelizerOriginTypeTag,iterator);iterator.hasNext();)
   {
      PanelizerOriginTypeTag originType = iterator.getNext();

      if (PanelizerOriginTypeTagToString(originType).Compare(tagValue) == 0)
      {
         retval = originType;
         break;
      }
   }

   return retval;
}

CString PanelEntityTypeTagToString(PanelEntityTypeTag tagValue)
{
   const char* pValue;

   switch (tagValue)
   {
   case panelEntityTypeOutline:      pValue = "Outline";      break;
   case panelEntityTypePcbInstance:  pValue = "PcbInstance";  break;
   case panelEntityTypePcbArray:     pValue = "PcbArray";     break;
   case panelEntityTypeFiducial:     pValue = "Fiducial";     break;
   case panelEntityTypeToolingHole:  pValue = "ToolingHole";  break;
   default:                          pValue = "?";            break;
   }

   return pValue;
}

PanelEntityTypeTag StringToPanelEntityTypeTag(const CString& tagValue)
{
   PanelEntityTypeTag retval = panelEntityTypeUndefined;

   for (EnumIterator(PanelEntityTypeTag,iterator);iterator.hasNext();)
   {
      PanelEntityTypeTag panelEntityType = iterator.getNext();

      if (PanelEntityTypeTagToString(panelEntityType).Compare(tagValue) == 0)
      {
         retval = panelEntityType;
         break;
      }
   }

   return retval;
}

//_________________________________________________________________________________________________
CPanelTemplateEntity::CPanelTemplateEntity(CPanelizer& panelizer)
: m_panelizer(panelizer)
, m_id(0)
, m_modifiedFlag(true)              // local update flag, goes on and off all the time
, m_modifiedSinceLoadFlag(true)     // tracks at higher level, for do-you-want-to-save upon Panelizer close
, m_entityDataList(NULL)
{
}

CPanelTemplateEntity::~CPanelTemplateEntity()
{
   delete m_entityDataList;
}

int CPanelTemplateEntity::getId() const
{
   return m_id;
}

void CPanelTemplateEntity::setId(int id)
{
   m_id = id;
}

bool CPanelTemplateEntity::getModifiedFlag() const
{
   return m_modifiedFlag;
}

void CPanelTemplateEntity::setModifiedFlag(bool flag)
{
   m_modifiedFlag = flag;

   // If item is being modified then it is also modified since load.
   // Here it is one sided, we set it but never clear it here, unlike the value above which can be changed either way.
   if (flag)
      m_modifiedSinceLoadFlag = true;
}

bool CPanelTemplateEntity::getModifiedSinceLoadFlag() const
{
   return m_modifiedSinceLoadFlag;
}

void CPanelTemplateEntity::setModifiedSinceLoadFlag(bool flag)
{
   m_modifiedSinceLoadFlag = flag;
}


CPanelizer& CPanelTemplateEntity::getPanelizer() const
{
   return m_panelizer;
}

CCamCadData& CPanelTemplateEntity::getCamCadData()
{
   return m_panelizer.getCamCadData();
}

FileStruct& CPanelTemplateEntity::getPanelizerSubFile()
{
   return m_panelizer.getPanelizerSubFile();
}

LayerStruct& CPanelTemplateEntity::getPanelOutlineLayer()
{
   LayerStruct& panelOutlineLayer = getCamCadData().getDefinedLayer("PanelOutline",false,layerTypePanelOutline);

   return panelOutlineLayer;
}

PanelEntityTypeTag CPanelTemplateEntityOutline::getPanelEntityType() const
{
   return panelEntityTypeOutline;
}

CString CPanelTemplateEntity::getUnitString(double value) const
{
   CString unitString = CUnitsInfo::formatUnits(value,getPanelizer().getCamCadData().getPageUnits());

   return unitString;
}

CDataList& CPanelTemplateEntity::getEmptyEntityList()
{
   if (m_entityDataList == NULL)
   {
      m_entityDataList = new CDataList(false);
   }
   else
   {
      m_entityDataList->empty();
   }

   return *m_entityDataList;
}

CString CPanelTemplateEntity::getDegreesString(double value) const
{
   CString degreesString;
   degreesString = fpfmt(value,1);

   return degreesString;
}

CString CPanelTemplateEntity::getIntString(int value) const
{
   CString intString;
   intString.Format("%d",value);

   return intString;
}

CString CPanelTemplateEntity::getMirroredString(bool value) const
{
   CString mirroredString(value ? "y" : "n");

   return mirroredString;
}

bool CPanelTemplateEntity::parseYesNoParam(CString value,bool& flag)
{
   if (value.GetLength() > 0)
   {
      value = value.Left(1);

      if (value.CompareNoCase("y") == 0 || value.CompareNoCase("t") == 0)
      {
         flag = true;
      }
      else if (value.CompareNoCase("n") == 0 || value.CompareNoCase("f") == 0)
      {
         flag = false;
      }
   }

   return flag;
}

CString CPanelTemplateEntity::getDescriptor() const
{
    CString descriptor;
    descriptor = PanelEntityTypeTagToString(getPanelEntityType());

    for (int index = 0;index < getParamCount();index++)
    {
       descriptor += " " + quoteString(getText(index));
    }

    return descriptor;
}

void CPanelTemplateEntity::write(CWriteFormat& writeFormat)
{
   CString line = getDescriptor() + "\n";

   writeFormat.write(line);
}

void CPanelTemplateEntity::regenerateFile()
{
}

void CPanelTemplateEntity::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
}

void CPanelTemplateEntity::highlightEntity(bool highlightFlag)
{
   if (m_entityDataList != NULL)
   {
      CCEtoODBDoc& camCadDoc = getPanelizer().getCamCadDoc();
      FileStruct& subFile = getPanelizerSubFile();
      int fileMirror = ((camCadDoc.getBottomView() ? MIRROR_FLIP : 0) ^ (subFile.isMirrored()) ? MIRROR_FLIP : 0);
      DrawStateTag drawState = (highlightFlag ? drawStateHighlighted : drawStateNormal);

      for (POSITION pos = m_entityDataList->GetHeadPosition();pos != NULL;)
      {
         DataStruct* data = m_entityDataList->GetNext(pos);

	      SelectStruct* selectStruct = camCadDoc.InsertInSelectList(data, subFile.getBlock()->getFileNumber(),
			      subFile.getInsertX(), subFile.getInsertY(),  subFile.getScale(), 
			      subFile.getRotation(), fileMirror, &(subFile.getBlock()->getDataList()), FALSE, TRUE);

	      camCadDoc.DrawEntity(*selectStruct,drawStateHighlighted);
      }
   }
}

//void CPanelTemplateEntity::mapLogicalNameToPhysicalName()
//{
//}

//_________________________________________________________________________________________________
CPanelTemplateEntityOutline::CPanelTemplateEntityOutline(CPanelizer& panelizer)
: CPanelTemplateEntity(panelizer)
, m_xOrigin(0.)
, m_yOrigin(0.)
, m_xSize(0.)
, m_ySize(0.)
{
}

CPanelTemplateEntityOutline& CPanelTemplateEntityOutline::operator=(const CPanelTemplateEntityOutline& other)
{
   if (&other != this)
   {
      m_xOrigin = other.m_xOrigin;
      m_yOrigin = other.m_yOrigin;
      m_xSize   = other.m_xSize;
      m_ySize   = other.m_ySize;
   }

   return *this;
}

bool CPanelTemplateEntityOutline::setValue(int col,const CString& stringValue,PageUnitsTag pageUnits)
{
   char* endChar;
   double doubleValue = strtod(stringValue,&endChar);
   double unitsValue  = ((pageUnits != pageUnitsUndefined) ? getCamCadData().convertToPageUnits(pageUnits,doubleValue) : doubleValue);
   bool   retval = (*endChar == '\0');

   switch (col)
   {
   case 0:  m_xOrigin = unitsValue;  break;
   case 1:  m_yOrigin = unitsValue;  break;
   case 2:  m_xSize   = unitsValue;  break;
   case 3:  m_ySize   = unitsValue;  break;
   default:  retval = false;  break;
   }

   setModifiedFlag(true);

   return retval;
}

bool CPanelTemplateEntityOutline::set(const CPanelTemplateEntity& other)
{
   bool retval = (other.getPanelEntityType() == getPanelEntityType());

   if (retval)
   {
      *this = (CPanelTemplateEntityOutline&)other;
   }

   return retval;
}

int CPanelTemplateEntityOutline::getParamCount() const
{
   return 4;
}

CString CPanelTemplateEntityOutline::getText(int index) const
{
   CString text;

   switch (index)
   {
   case 0:  text = getUnitString(m_xOrigin);  break;
   case 1:  text = getUnitString(m_yOrigin);  break;
   case 2:  text = getUnitString(m_xSize  );  break;
   case 3:  text = getUnitString(m_ySize  );  break;
   }

   return text;
}

//double CPanelTemplateEntityOutline::getXOrigin() const
//{
//   return m_xOrigin;
//}
//
//void CPanelTemplateEntityOutline::setXOrigin(double ordinate)
//{
//   m_xOrigin = ordinate;
//}
//
//double CPanelTemplateEntityOutline::getYOrigin() const
//{
//   return m_yOrigin;
//}
//
//void CPanelTemplateEntityOutline::setYOrigin(double ordinate)
//{
//   m_yOrigin = ordinate;
//}
//
//double CPanelTemplateEntityOutline::getXSize() const
//{
//   return m_xSize;
//}
//
//void CPanelTemplateEntityOutline::setXSize(double size)
//{
//   m_xSize = size;
//}
//
//double CPanelTemplateEntityOutline::getYSize() const
//{
//   return m_ySize;
//}
//
//void CPanelTemplateEntityOutline::setYSize(double size)
//{
//   m_ySize = size;
//}

void CPanelTemplateEntityOutline::regenerateFile()
{
   instantiateFile(getPanelizerSubFile(),getEmptyEntityList());
}

void CPanelTemplateEntityOutline::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
   LayerStruct& panelOutlineLayer = getPanelOutlineLayer();
   int widthIndex = getCamCadData().getDefinedWidthIndex(getCamCadData().convertToPageUnits(pageUnitsMils,10.));

   DataStruct* polyStruct = getCamCadData().addPolyStruct(*(targetPanelFile.getBlock()),panelOutlineLayer.getLayerIndex(),graphicClassPanelOutline);
   polyStruct->setLayerIndex(getPanelizer().getPanelOutlineLayer().getLayerIndex());

   CPoly* poly = getCamCadData().addClosedPoly(*polyStruct,widthIndex);
   poly->addVertex(m_xOrigin          ,m_yOrigin         );
   poly->addVertex(m_xOrigin + m_xSize,m_yOrigin          );
   poly->addVertex(m_xOrigin + m_xSize,m_yOrigin + m_ySize);
   poly->addVertex(m_xOrigin          ,m_yOrigin + m_ySize);
   poly->close();

   instantiatedData.AddTail(polyStruct);
}

CDBPanelTemplateOutline* CPanelTemplateEntityOutline::AllocDBPanelTemplateOutline()
{
   CDBPanelTemplateOutline* thing = new CDBPanelTemplateOutline(m_xOrigin, m_yOrigin, m_xSize, m_ySize);

   return thing;
}

void CPanelTemplateEntityOutline::AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateOutline *dbitem)
{
   if (dbitem != NULL)
   {
      this->setValue(0, dbitem->GetOriginXStr(), pageUnits);
      this->setValue(1, dbitem->GetOriginYStr(), pageUnits);
      this->setValue(2, dbitem->GetSizeXStr(),   pageUnits);
      this->setValue(3, dbitem->GetSizeYStr(),   pageUnits);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityPcbInstance::CPanelTemplateEntityPcbInstance(CPanelizer& panelizer)
: CPanelTemplateEntity(panelizer)
, m_name(panelizer.getUniqueLogicalName())
, m_xOrigin(0.)
, m_yOrigin(0.)
, m_angleRadians(0.)
, m_originType(panelizerOriginTypeBoardOrigin)
, m_mirrored(false)
{
}

CPanelTemplateEntityPcbInstance& CPanelTemplateEntityPcbInstance::operator=(const CPanelTemplateEntityPcbInstance& other)
{
   if (&other != this)
   {
      m_name         = other.m_name;
      m_xOrigin      = other.m_xOrigin;
      m_yOrigin      = other.m_yOrigin;
      m_angleRadians = other.m_angleRadians;
      m_mirrored     = other.m_mirrored;
      m_originType   = other.m_originType;
   }

   return *this;
}

PanelEntityTypeTag CPanelTemplateEntityPcbInstance::getPanelEntityType() const
{
   return panelEntityTypePcbInstance;
}

bool CPanelTemplateEntityPcbInstance::setValue(int col,const CString& stringValue,PageUnitsTag pageUnits)
{
   bool retval = true;
   char* endChar;
   double doubleValue = strtod(stringValue,&endChar);
   double unitsValue  = ((pageUnits != pageUnitsUndefined) ? getCamCadData().convertToPageUnits(pageUnits,doubleValue) : doubleValue);
   bool   validNumericValue = (*endChar == '\0');

   switch (col)
   {
   case 0:
      {
         m_name    = stringValue;

         //m_physicalBoardName = getPanelizer().getPhysicalNameForLogicalName(m_name);
      }

      break;
   case 1:  m_xOrigin      = unitsValue;   retval = validNumericValue;  break;
   case 2:  m_yOrigin      = unitsValue;   retval = validNumericValue;  break;
   case 3:  m_angleRadians = degreesToRadians(doubleValue);  retval = validNumericValue;  break;
   case 4:  m_mirrored     = false;  parseYesNoParam(stringValue,m_mirrored);  break;
   case 5:  m_originType   = StringToPanelizerOriginTypeTag(stringValue);  retval = (m_originType != panelizerOriginTypeUndefined);  break;
   case 6:
      {
         //m_physicalBoardName = stringValue;
         getPanelizer().mapLogicalNameToPhysicalName(m_name,stringValue);
      }
      
      break;
   default:  retval = false;  break;
   }

   setModifiedFlag(true);

   return retval;
}

bool CPanelTemplateEntityPcbInstance::set(const CPanelTemplateEntity& other)
{
   bool retval = (other.getPanelEntityType() == getPanelEntityType());

   if (retval)
   {
      *this = (CPanelTemplateEntityPcbInstance&)other;
   }

   return retval;
}

int CPanelTemplateEntityPcbInstance::getParamCount() const
{
   return 7;
}

CString CPanelTemplateEntityPcbInstance::getText(int index) const
{
   CString text;

   switch (index)
   {
   case 0:  text = m_name;  break;
   case 1:  text = getUnitString(m_xOrigin);  break;
   case 2:  text = getUnitString(m_yOrigin);  break;
   case 3:  text = getDegreesString(radiansToDegrees(m_angleRadians));  break;
   case 4:  text = getMirroredString(m_mirrored);  break;
   case 5:  text = PanelizerOriginTypeTagToString(m_originType);  break;
   case 6:
      {
         text = getPanelizer().getPhysicalNameForLogicalName(m_name);
      }
      
      break;
   }

   return text;
}

void CPanelTemplateEntityPcbInstance::regenerateFile()
{
   BlockStruct* outlineGeometry = getPanelizer().getOutlineGeometryForLogicalName(m_name);

   CString refName;
   refName.Format("Pcb%d",getId());

   CPoint2d origin(m_xOrigin,m_yOrigin);
   CBasesVector placementBasesVector(origin,radiansToDegrees(m_angleRadians),m_mirrored);

   //CDebug::addDebugAxis(getCamCadData(),placementBasesVector,20.,"db placementBasesVector - %s",m_name);

   if (m_originType == panelizerOriginTypeBoardCenter)
   {
      CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(m_name);

      CBasesVector boardOriginBasesVector(-pcbExtent.getCenter());
      boardOriginBasesVector.transform(placementBasesVector.getTransformationMatrix());

      if (boardOriginBasesVector.getMirror())
      {
         placementBasesVector.set(boardOriginBasesVector.getOrigin(),-boardOriginBasesVector.getRotationDegrees(),boardOriginBasesVector.getMirror());
      }
      else
      {
         placementBasesVector = boardOriginBasesVector;
      }
   }

   DataStruct* pcbInsertData = getCamCadData().insertBlock(*outlineGeometry,insertTypePcb,refName,-1,
                                  placementBasesVector.getOrigin().x,placementBasesVector.getOrigin().y,
                                  placementBasesVector.getRotationRadians(),placementBasesVector.getMirror());

   getPanelizer().getPanelizerSubFile().getBlock()->getDataList().AddTail(pcbInsertData);

   getEmptyEntityList().AddTail(pcbInsertData);
}

void CPanelTemplateEntityPcbInstance::GetPlacementBasesVector(CBasesVector& placementBasesVector)
{
   ///BlockStruct* pcbGeometry = getPanelizer().getPcbGeometryForLogicalName(m_name);

   CPoint2d origin(m_xOrigin,m_yOrigin);
   //CBasesVector placementBasesVector(origin,radiansToDegrees(m_angleRadians),m_mirrored);
   placementBasesVector.set(origin,radiansToDegrees(m_angleRadians),m_mirrored);

   //CDebug::addDebugAxis(getCamCadData(),placementBasesVector,20.,"db placementBasesVector - %s",m_name);

   if (m_originType == panelizerOriginTypeBoardCenter)
   {
      CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(m_name);

      CBasesVector boardOriginBasesVector(-pcbExtent.getCenter());
      boardOriginBasesVector.transform(placementBasesVector.getTransformationMatrix());

      if (boardOriginBasesVector.getMirror())
      {
         placementBasesVector.set(boardOriginBasesVector.getOrigin(),-boardOriginBasesVector.getRotationDegrees(),boardOriginBasesVector.getMirror());
      }
      else
      {
         placementBasesVector = boardOriginBasesVector;
      }
   }
}

void CPanelTemplateEntityPcbInstance::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
   BlockStruct* pcbGeometry = getPanelizer().getPcbGeometryForLogicalName(m_name);

   CString refName;
   refName.Format("Pcb%d",getId());  // Default refname

   if (!getText(0).IsEmpty()) // Use name from grid for refname
      refName = getText(0);

   CBasesVector placementBasesVector;
   GetPlacementBasesVector(placementBasesVector);

   //CDebug::addDebugAxis(getCamCadData(),placementBasesVector,20.,"db placementBasesVector - %s",m_name);

   DataStruct* pcbInsertData = getCamCadData().insertBlock(*pcbGeometry,insertTypePcb,refName,-1,
                                  placementBasesVector.getOrigin().x,placementBasesVector.getOrigin().y,
                                  placementBasesVector.getRotationRadians(),placementBasesVector.getMirror());


   targetPanelFile.getBlock()->getDataList().AddTail(pcbInsertData);
   instantiatedData.AddTail(pcbInsertData);
}

CDBPanelTemplatePcbInstance* CPanelTemplateEntityPcbInstance::AllocDBPanelTemplatePcbInstance()
{
   double angleDegrees = normalizeDegrees( RadToDeg(this->m_angleRadians) );
   CString originTypeStr( PanelizerOriginTypeTagToString(m_originType) );

   CDBPanelTemplatePcbInstance* thing = new CDBPanelTemplatePcbInstance(m_name, m_xOrigin, m_yOrigin, angleDegrees, m_mirrored, originTypeStr);

   return thing;
}

void CPanelTemplateEntityPcbInstance::AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplatePcbInstance *dbitem)
{
   if (dbitem != NULL)
   {
      this->setValue(0, dbitem->GetName(),         pageUnits);
      this->setValue(1, dbitem->GetOriginXStr(),   pageUnits);
      this->setValue(2, dbitem->GetOriginYStr(),   pageUnits);
      this->setValue(3, dbitem->GetAngleStr(),     pageUnits);
      this->setValue(4, dbitem->GetMirroredStr(),  pageUnits);
      this->setValue(5, dbitem->GetOriginType(),   pageUnits);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityPcbArray::CPanelTemplateEntityPcbArray(CPanelizer& panelizer)
: CPanelTemplateEntity(panelizer)
, m_name(panelizer.getUniqueLogicalName())
, m_xOrigin(0.)
, m_yOrigin(0.)
, m_angleRadians(0.)
, m_originType(panelizerOriginTypeBoardOrigin)
, m_xCount(0)
, m_yCount(0)
, m_xSize(0.)
, m_ySize(0.)
, m_mirrored(false)
{
}

CPanelTemplateEntityPcbArray& CPanelTemplateEntityPcbArray::operator=(const CPanelTemplateEntityPcbArray& other)
{
   if (&other != this)
   {
      m_name         = other.m_name;
      m_xOrigin      = other.m_xOrigin;
      m_yOrigin      = other.m_yOrigin;
      m_angleRadians = other.m_angleRadians;
      m_mirrored     = other.m_mirrored;
      m_originType   = other.m_originType;
      m_xCount       = other.m_xCount;
      m_yCount       = other.m_yCount;
      m_xSize        = other.m_xSize;
      m_ySize        = other.m_ySize;
   }

   return *this;
}

PanelEntityTypeTag CPanelTemplateEntityPcbArray::getPanelEntityType() const
{
   return panelEntityTypePcbArray;
}

bool CPanelTemplateEntityPcbArray::setValue(int col,const CString& stringValue,PageUnitsTag pageUnits)
{
   bool retval = true;
   char* endChar;
   double doubleValue = strtod(stringValue,&endChar);
   double unitsValue  = ((pageUnits != pageUnitsUndefined) ? getCamCadData().convertToPageUnits(pageUnits,doubleValue) : doubleValue);
   int    intValue    = DcaRound(doubleValue);
   bool   validNumericValue = (*endChar == '\0');

   switch (col)
   {
   case 0:
      {
         m_name    = stringValue;

         //m_physicalBoardName = getPanelizer().getPhysicalNameForLogicalName(m_name);
      }

      break;
   case 1:  m_xOrigin = unitsValue;   retval = validNumericValue;  break;
   case 2:  m_yOrigin = unitsValue;   retval = validNumericValue;  break;
   case 3:  m_angleRadians = degreesToRadians(doubleValue);  retval = validNumericValue;  break;
   case 4:  m_mirrored     = false;  parseYesNoParam(stringValue,m_mirrored);  break;
   case 5:  m_originType = StringToPanelizerOriginTypeTag(stringValue);  retval = (m_originType != panelizerOriginTypeUndefined);  break;
   case 6:  m_xCount  = intValue;     retval = validNumericValue;  break;
   case 7:  m_yCount  = intValue;     retval = validNumericValue;  break;
   case 8:  m_xSize   = unitsValue;   retval = validNumericValue;  break;
   case 9:  m_ySize   = unitsValue;   retval = validNumericValue;  break;
   case 10:
      {
         //m_physicalBoardName = stringValue;
         getPanelizer().mapLogicalNameToPhysicalName(m_name,stringValue);
      }
      
      break;
   default:  retval = false;  break;
   }

   setModifiedFlag(true);

   return retval;
}

bool CPanelTemplateEntityPcbArray::set(const CPanelTemplateEntity& other)
{
   bool retval = (other.getPanelEntityType() == getPanelEntityType());

   if (retval)
   {
      *this = (CPanelTemplateEntityPcbArray&)other;
   }

   return retval;
}

int CPanelTemplateEntityPcbArray::getParamCount() const
{
   return 11;
}

CString CPanelTemplateEntityPcbArray::getText(int index) const
{
   CString text;

   switch (index)
   {
   case 0:  text = m_name;  break;
   case 1:  text = getUnitString(m_xOrigin);  break;
   case 2:  text = getUnitString(m_yOrigin);  break;
   case 3:  text = getDegreesString(radiansToDegrees(m_angleRadians));  break;
   case 4:  text = getMirroredString(m_mirrored);  break;
   case 5:  text = PanelizerOriginTypeTagToString(m_originType);  break;
   case 6:  text = getIntString(m_xCount);    break;
   case 7:  text = getIntString(m_yCount);    break;
   case 8:  text = getUnitString(m_xSize);    break;
   case 9:  text = getUnitString(m_ySize);    break;
   case 10:
      {
         text = getPanelizer().getPhysicalNameForLogicalName(m_name);
      }
      
      break;
   }

   return text;
}

void CPanelTemplateEntityPcbArray::GetPlacementBasesVector(int colIndexIN, int rowIndexIN, CBasesVector& placementBasesVectorOUT)
{
   CTMatrix matrix;
   matrix.translateCtm(m_xOrigin,m_yOrigin);
   matrix.mirrorAboutYAxisCtm(m_mirrored);
   matrix.rotateRadiansCtm(m_angleRadians);

   CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(m_name);

   CPoint2d placementPoint;
   placementPoint.x = colIndexIN * m_xSize;
   placementPoint.y = rowIndexIN * m_ySize;

   if (m_originType == panelizerOriginTypeBoardCenter)
   {
      placementPoint = placementPoint - pcbExtent.getCenter();
   }

   placementBasesVectorOUT.set(placementPoint, 0.);
   placementBasesVectorOUT.transform(matrix);

   if (m_mirrored)
   {
      placementBasesVectorOUT.set(placementBasesVectorOUT.getOrigin(),-placementBasesVectorOUT.getRotationDegrees(),placementBasesVectorOUT.getMirror());
   }
}

void CPanelTemplateEntityPcbArray::insertArray(FileStruct& targetPanelFile,BlockStruct& geometry,CDataList& instantiatedData,bool drawGridLinesFlag)
{
   //CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(m_name);
/*
   CTMatrix matrix;
   matrix.translateCtm(m_xOrigin,m_yOrigin);
   matrix.mirrorAboutYAxisCtm(m_mirrored);
   matrix.rotateRadiansCtm(m_angleRadians);
   CPoint2d placementPoint;
*/
   for (int rowIndex = 0;rowIndex < m_yCount;rowIndex++)
   {
      for (int colIndex = 0;colIndex < m_xCount;colIndex++)
      {
         CString refName;
         refName.Format("PcbArray%d-r%d-c%d",getId(),rowIndex,colIndex);
/*
         placementPoint.x = colIndex * m_xSize;
         placementPoint.y = rowIndex * m_ySize;

         if (m_originType == panelizerOriginTypeBoardCenter)
         {
            placementPoint = placementPoint - pcbExtent.getCenter();
         }

         CBasesVector placementBasesVector(placementPoint);
         placementBasesVector.transform(matrix);


         {
            if (m_mirrored)
            {
               placementBasesVector.set(placementBasesVector.getOrigin(),-placementBasesVector.getRotationDegrees(),placementBasesVector.getMirror());
            }
         }
*/
         CBasesVector placementBasesVector;
         this->GetPlacementBasesVector(colIndex, rowIndex, placementBasesVector);

         DataStruct* pcbInsertData = getCamCadData().insertBlock(geometry,insertTypePcb,refName,-1,
                                       placementBasesVector.getOrigin().x,placementBasesVector.getOrigin().y,
                                       placementBasesVector.getRotationRadians(),placementBasesVector.getMirror());

         targetPanelFile.getBlock()->getDataList().AddTail(pcbInsertData);
         instantiatedData.AddTail(pcbInsertData);
      }
   }


   if (drawGridLinesFlag)
   {
      CTMatrix matrix;
      matrix.translateCtm(m_xOrigin,m_yOrigin);
      matrix.mirrorAboutYAxisCtm(m_mirrored);
      matrix.rotateRadiansCtm(m_angleRadians);

      DataStruct* gridData = getCamCadData().addPolyStruct(*(targetPanelFile.getBlock()),getPanelizer().getPcbArrayGridLayer().getLayerIndex(),graphicClassNormal);
      instantiatedData.AddTail(gridData);

      bool centeredFlag = (m_originType == panelizerOriginTypeBoardCenter);

      for (int rowIndex = 0;rowIndex <= m_yCount - centeredFlag;rowIndex++)
      {
         double x0 = (centeredFlag ? -m_xSize/2. : 0.);
         double y0 = rowIndex * m_ySize;
         double x1 = m_xCount * m_xSize + (centeredFlag ? -m_xSize/2. : 0.);
         double y1 = y0;

         matrix.transform(x0,y0);
         matrix.transform(x1,y1);

         CPoly* poly = getCamCadData().addOpenPoly(*gridData,getCamCadData().getZeroWidthIndex());
         getCamCadData().addVertex(*poly,x0,y0);
         getCamCadData().addVertex(*poly,x1,y1);
      }

      for (int colIndex = 0;colIndex <= m_xCount - centeredFlag;colIndex++)
      {
         double x0 = colIndex * m_xSize;
         double y0 = (centeredFlag ? -m_ySize/2. : 0.);
         double x1 = x0;
         double y1 = m_yCount * m_ySize + (centeredFlag ? -m_ySize/2. : 0.);

         matrix.transform(x0,y0);
         matrix.transform(x1,y1);

         CPoly* poly = getCamCadData().addOpenPoly(*gridData,getCamCadData().getZeroWidthIndex());
         getCamCadData().addVertex(*poly,x0,y0);
         getCamCadData().addVertex(*poly,x1,y1);
      }
   }
}

void CPanelTemplateEntityPcbArray::regenerateFile()
{
   BlockStruct* outlineGeometry = getPanelizer().getOutlineGeometryForLogicalName(m_name);
   insertArray(getPanelizer().getPanelizerSubFile(),*outlineGeometry,getEmptyEntityList(),true);

   //CTMatrix matrix;
   //matrix.translateCtm(m_xOrigin,m_yOrigin);
   //matrix.rotateRadiansCtm(m_angleRadians);
   ////matrix.invert();

   //for (int rowIndex = 0;rowIndex < m_yCount;rowIndex++)
   //{
   //   for (int colIndex = 0;colIndex < m_xCount;colIndex++)
   //   {
   //      CString refName;
   //      refName.Format("PcbArray%d-r%d-c%d",getId(),rowIndex,colIndex);

   //      double x = colIndex * m_xSize;
   //      double y = rowIndex * m_ySize;

   //      matrix.transform(x,y);

   //      DataStruct* pcbInsertData = getCamCadData().insertBlock(*outlineGeometry,insertTypePcb,refName,-1,x,y,m_angleRadians);

   //      getPanelizer().getPanelizerSubFile().getBlock()->getDataList().AddTail(pcbInsertData);
   //   }
   //}

   //DataStruct* gridData = getCamCadData().addPolyStruct(*(getPanelizer().getPanelizerSubFile().getBlock()),getPanelizer().getPcbArrayGridLayer().getLayerIndex(),graphicClassNormal);

   //for (int rowIndex = 0;rowIndex <= m_yCount;rowIndex++)
   //{
   //   double x0 = 0.;
   //   double y0 = rowIndex * m_ySize;
   //   double x1 = m_xCount * m_xSize;
   //   double y1 = y0;

   //   matrix.transform(x0,y0);
   //   matrix.transform(x1,y1);

   //   CPoly* poly = getCamCadData().addOpenPoly(*gridData,getCamCadData().getZeroWidthIndex());
   //   getCamCadData().addVertex(*poly,x0,y0);
   //   getCamCadData().addVertex(*poly,x1,y1);
   //}

   //for (int colIndex = 0;colIndex <= m_xCount;colIndex++)
   //{
   //   double x0 = colIndex * m_xSize;
   //   double y0 = 0.;
   //   double x1 = x0;
   //   double y1 = m_yCount * m_ySize;

   //   matrix.transform(x0,y0);
   //   matrix.transform(x1,y1);

   //   CPoly* poly = getCamCadData().addOpenPoly(*gridData,getCamCadData().getZeroWidthIndex());
   //   getCamCadData().addVertex(*poly,x0,y0);
   //   getCamCadData().addVertex(*poly,x1,y1);
   //}
}

void CPanelTemplateEntityPcbArray::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
   BlockStruct* pcbGeometry = getPanelizer().getPcbGeometryForLogicalName(m_name);
   int pcbFileNum = pcbGeometry->getFileNumber();
   int curFileNum = targetPanelFile.getFileNumber();
   insertArray(targetPanelFile,*pcbGeometry,getEmptyEntityList(),false);
}

CDBPanelTemplatePcbArray* CPanelTemplateEntityPcbArray::AllocDBPanelTemplatePcbArray()
{
   double angleDegrees = normalizeDegrees( RadToDeg(this->m_angleRadians) );
   CString originTypeStr( PanelizerOriginTypeTagToString(m_originType) );

   CDBPanelTemplatePcbArray* thing = new CDBPanelTemplatePcbArray(m_name, m_xOrigin, m_yOrigin, angleDegrees, m_mirrored, 
      originTypeStr, m_xCount, m_yCount, m_xSize, m_ySize);

   return thing;
}

void CPanelTemplateEntityPcbArray::AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplatePcbArray *dbitem)
{
   if (dbitem != NULL)
   {
      this->setValue(0, dbitem->GetName(),         pageUnits);
      this->setValue(1, dbitem->GetOriginXStr(),   pageUnits);
      this->setValue(2, dbitem->GetOriginYStr(),   pageUnits);
      this->setValue(3, dbitem->GetAngleStr(),     pageUnits);
      this->setValue(4, dbitem->GetMirroredStr(),  pageUnits);
      this->setValue(5, dbitem->GetOriginType(),   pageUnits);
      this->setValue(6, dbitem->GetCountXStr(),    pageUnits);
      this->setValue(7, dbitem->GetCountYStr(),    pageUnits);
      this->setValue(8, dbitem->GetStepXStr(),     pageUnits);
      this->setValue(9, dbitem->GetStepYStr(),     pageUnits);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityFiducial::CPanelTemplateEntityFiducial(CPanelizer& panelizer)
: CPanelTemplateEntity(panelizer)
, m_fiducialType(fiducialTypeRound)
, m_size(0.)
, m_xOrigin(0.)
, m_yOrigin(0.)
, m_angleRadians(0.)
, m_mirrored(false)
{
}

CPanelTemplateEntityFiducial& CPanelTemplateEntityFiducial::operator=(const CPanelTemplateEntityFiducial& other)
{
   if (&other != this)
   {
      m_fiducialType = other.m_fiducialType;
      m_size         = other.m_size;
      m_xOrigin      = other.m_xOrigin;
      m_yOrigin      = other.m_yOrigin;
      m_angleRadians = other.m_angleRadians;
      m_mirrored     = other.m_mirrored;
   }

   return *this;
}

PanelEntityTypeTag CPanelTemplateEntityFiducial::getPanelEntityType() const
{
   return panelEntityTypeFiducial;
}

bool CPanelTemplateEntityFiducial::setValue(int col,const CString& stringValue,PageUnitsTag pageUnits)
{
   bool retval = true;
   char* endChar;
   double doubleValue = strtod(stringValue,&endChar);
   double unitsValue  = ((pageUnits != pageUnitsUndefined) ? getCamCadData().convertToPageUnits(pageUnits,doubleValue) : doubleValue);
   bool   validNumericValue = (*endChar == '\0');

   switch (col)
   {
   case 0:  m_fiducialType = StringToFiducialTypeTag(stringValue);  retval = (m_fiducialType != fiducialTypeUndefined);  break;
   case 1:  m_size    = unitsValue;   retval = validNumericValue;  break;
   case 2:  m_xOrigin = unitsValue;   retval = validNumericValue;  break;
   case 3:  m_yOrigin = unitsValue;   retval = validNumericValue;  break;
   case 4:  m_angleRadians = degreesToRadians(doubleValue);  retval = validNumericValue;  break;
   case 5:  m_mirrored     = false;  parseYesNoParam(stringValue,m_mirrored);  break;
   default:  retval = false;  break;
   }

   setModifiedFlag(true);

   return retval;
}

bool CPanelTemplateEntityFiducial::set(const CPanelTemplateEntity& other)
{
   bool retval = (other.getPanelEntityType() == getPanelEntityType());

   if (retval)
   {
      *this = (CPanelTemplateEntityFiducial&)other;
   }

   return retval;
}

int CPanelTemplateEntityFiducial::getParamCount() const
{
   return 6;
}

CString CPanelTemplateEntityFiducial::getText(int index) const
{
   CString text;

   switch (index)
   {
   case 0:  text = FiducialTypeTagToString(m_fiducialType);  break;
   case 1:  text = getUnitString(m_size);      break;
   case 2:  text = getUnitString(m_xOrigin);   break;
   case 3:  text = getUnitString(m_yOrigin);   break;
   case 4:  text = getDegreesString(radiansToDegrees(m_angleRadians));  break;
   case 5:  text = getMirroredString(m_mirrored);  break;
   }

   return text;
}

void CPanelTemplateEntityFiducial::regenerateFile()
{
   instantiateFile(getPanelizer().getPanelizerSubFile(),getEmptyEntityList());
}

void CPanelTemplateEntityFiducial::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
   BlockStruct& fiducialGeometry = getPanelizer().getFiducialGenerator().getDefinedFiducialGeometry(m_fiducialType,m_size,targetPanelFile.getFileNumber());

   CString refName;
   refName.Format("FM%d",getId());

   DataStruct* fiducialInsertData = getCamCadData().insertBlock(fiducialGeometry,insertTypeFiducial,refName,-1,m_xOrigin,m_yOrigin,m_angleRadians,m_mirrored);

   targetPanelFile.getBlock()->getDataList().AddTail(fiducialInsertData);
   instantiatedData.AddTail(fiducialInsertData);
}


CDBPanelTemplateFiducial* CPanelTemplateEntityFiducial::AllocDBPanelTemplateFiducial()
{
   CString fidTypeStr( FiducialTypeTagToString(this->m_fiducialType) );
   double angleDegrees = normalizeDegrees( RadToDeg(this->m_angleRadians) );

   CDBPanelTemplateFiducial* dbth = new CDBPanelTemplateFiducial(fidTypeStr, m_size, m_xOrigin, m_yOrigin, angleDegrees, m_mirrored);

   return dbth;
}

void CPanelTemplateEntityFiducial::AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateFiducial *dbitem)
{
   if (dbitem != NULL)
   {
      this->setValue(0, dbitem->GetType(),          pageUnits);
      this->setValue(1, dbitem->GetSizeStr(),       pageUnits);
      this->setValue(2, dbitem->GetOriginXStr(),    pageUnits);
      this->setValue(3, dbitem->GetOriginYStr(),    pageUnits);
      this->setValue(4, dbitem->GetAngleStr(),      pageUnits);
      this->setValue(5, dbitem->GetMirroredStr(),   pageUnits);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityToolingHole::CPanelTemplateEntityToolingHole(CPanelizer& panelizer)
: CPanelTemplateEntity(panelizer)
, m_size(0.)
, m_xOrigin(0.)
, m_yOrigin(0.)
{
}

CPanelTemplateEntityToolingHole& CPanelTemplateEntityToolingHole::operator=(const CPanelTemplateEntityToolingHole& other)
{
   if (&other != this)
   {
      m_size    = other.m_size;
      m_xOrigin = other.m_xOrigin;
      m_yOrigin = other.m_yOrigin;
   }

   return *this;
}

PanelEntityTypeTag CPanelTemplateEntityToolingHole::getPanelEntityType() const
{
   return panelEntityTypeToolingHole;
}

bool CPanelTemplateEntityToolingHole::setValue(int col,const CString& stringValue,PageUnitsTag pageUnits)
{
   char* endChar;
   double doubleValue = strtod(stringValue,&endChar);
   double unitsValue  = ((pageUnits != pageUnitsUndefined) ? getCamCadData().convertToPageUnits(pageUnits,doubleValue) : doubleValue);
   bool   retval = (*endChar == '\0');

   switch (col)
   {
   case 0:  m_size    = unitsValue;  break;
   case 1:  m_xOrigin = unitsValue;  break;
   case 2:  m_yOrigin = unitsValue;  break;
   default:  retval = false;  break;
   }

   setModifiedFlag(true);

   return retval;
}

bool CPanelTemplateEntityToolingHole::set(const CPanelTemplateEntity& other)
{
   bool retval = (other.getPanelEntityType() == getPanelEntityType());

   if (retval)
   {
      *this = (CPanelTemplateEntityToolingHole&)other;
   }

   return retval;
}

int CPanelTemplateEntityToolingHole::getParamCount() const
{
   return 3;
}

CString CPanelTemplateEntityToolingHole::getText(int index) const
{
   CString text;

   switch (index)
   {
   case 0:  text = getUnitString(m_size);     break;
   case 1:  text = getUnitString(m_xOrigin);  break;
   case 2:  text = getUnitString(m_yOrigin);  break;
   }

   return text;
}

void CPanelTemplateEntityToolingHole::regenerateFile()
{
   instantiateFile(getPanelizer().getPanelizerSubFile(),getEmptyEntityList());
}

void CPanelTemplateEntityToolingHole::instantiateFile(FileStruct& targetPanelFile,CDataList& instantiatedData)
{
   int widthIndex = getCamCadData().getDefinedToolIndex(m_size);
   BlockStruct* toolGeometry = getCamCadData().getWidthBlock(widthIndex);
   LayerStruct& drillLayer = getCamCadData().getDefinedLayer(ccLayerDrillHoles);

   CString refName;
   refName.Format("H%d",getId());

   DataStruct* toolingHoleInsertData = getCamCadData().insertBlock(*toolGeometry,insertTypeDrillTool,refName,drillLayer.getLayerIndex(),m_xOrigin,m_yOrigin);

   targetPanelFile.getBlock()->getDataList().AddTail(toolingHoleInsertData);
   instantiatedData.AddTail(toolingHoleInsertData);
}

CDBPanelTemplateToolingHole* CPanelTemplateEntityToolingHole::AllocDBPanelTemplateToolingHole()
{
   CDBPanelTemplateToolingHole* dbth = new CDBPanelTemplateToolingHole(m_size, m_xOrigin, m_yOrigin);

   return dbth;
}

void CPanelTemplateEntityToolingHole::AdopDBValues(PageUnitsTag pageUnits, CDBPanelTemplateToolingHole *dbitem)
{
   if (dbitem != NULL)
   {
      this->setValue(0, dbitem->GetSizeStr(), pageUnits);
      this->setValue(1, dbitem->GetXStr(),    pageUnits);
      this->setValue(2, dbitem->GetYStr(),    pageUnits);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityArray::CPanelTemplateEntityArray(CPanelizer& panelizer)
: m_panelizer(panelizer)
{
}

CPanelizer& CPanelTemplateEntityArray::getPanelizer()
{
   return m_panelizer;
}

bool CPanelTemplateEntityArray::getModifiedFlag() const
{
   bool modifiedFlag = false;

   for (int index = 0;index < getSize() && !modifiedFlag;index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      modifiedFlag = entity->getModifiedFlag();
   }

   return modifiedFlag;
}

void CPanelTemplateEntityArray::setModifiedFlag(bool flag)
{
   for (int index = 0;index < getSize();index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      entity->setModifiedFlag(flag);
   }
}

bool CPanelTemplateEntityArray::getModifiedSinceLoadFlag() const
{
   bool modifiedFlag = false;

   for (int index = 0; index < getSize() && !modifiedFlag; index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      modifiedFlag = entity->getModifiedSinceLoadFlag();
   }

   return modifiedFlag;
}

void CPanelTemplateEntityArray::setModifiedSinceLoadFlag(bool flag)
{
   for (int index = 0; index < getSize(); index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      entity->setModifiedSinceLoadFlag(flag);
   }
}

CPanelTemplateEntity* CPanelTemplateEntityArray::addParsedEntity(const CStringArray& params,PageUnitsTag pageUnits,int offset)
{
   CPanelTemplateEntity* entity = addEntity();
   int paramCount = params.GetCount();
   int entityParamCount = entity->getParamCount();

   //if (params.GetCount() - offset >= entityParamCount)
   //{
      for (int index = 0;index < entityParamCount && index + offset < paramCount;index++)
      {
         CString value = params.GetAt(index + offset);

         if (!entity->setValue(index,value,pageUnits))
         {
            deleteLast();
            entity = NULL;

            break;
         }
      }
   //}

   return entity;
}

void CPanelTemplateEntityArray::deleteFirst()
{
   deleteAt(0);
}

void CPanelTemplateEntityArray::deleteLast()
{
   deleteAt(getSize() - 1);
}

void CPanelTemplateEntityArray::copyEntities(const CPanelTemplateEntityArray& sourceArray,int insertIndex)
{
   if (getPanelEntityType() == sourceArray.getPanelEntityType())
   {
      if (insertIndex < 0) insertIndex = 0;
      else if (insertIndex > getSize()) insertIndex = getSize();

      for (int sourceIndex = 0;sourceIndex < sourceArray.getSize();sourceIndex++,insertIndex++)
      {
         CPanelTemplateEntity* sourceEntity = sourceArray.getEntityAt(sourceIndex);
         CPanelTemplateEntity* entity = addEntityAt(insertIndex);

         entity->set(*sourceEntity);
      }
   }
}

void CPanelTemplateEntityArray::renumber()
{
   for (int index = 0;index < getSize();index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      entity->setId(index);
   }
}

void CPanelTemplateEntityArray::write(CWriteFormat& writeFormat)
{
   int recordCount = getSize();

   for (int recordIndex = 0;recordIndex < recordCount;recordIndex++)
   {
      CPanelTemplateEntity* entity = getEntityAt(recordIndex);
      entity->write(writeFormat);
   }
}

void CPanelTemplateEntityArray::regenerateFile()
{
   for (int index = 0;index < getSize();index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      entity->regenerateFile();
   }
}

void CPanelTemplateEntityArray::instantiateFile(FileStruct& targetPanelFile)
{
   for (int index = 0;index < getSize();index++)
   {
      CPanelTemplateEntity* entity = getEntityAt(index);

      CDataList instantiatedData(false);
      entity->instantiateFile(targetPanelFile,instantiatedData);
   }
}

//_________________________________________________________________________________________________
CPanelTemplateEntityOutlineArray::CPanelTemplateEntityOutlineArray(CPanelizer& panelizer)
: CPanelTemplateEntityArray(panelizer)
{
}

void CPanelTemplateEntityOutlineArray::empty()
{
   m_array.empty();
}

int CPanelTemplateEntityOutlineArray::getSize() const
{
   return m_array.GetSize();
}

PanelEntityTypeTag CPanelTemplateEntityOutlineArray::getPanelEntityType() const
{
   return panelEntityTypeOutline;
}

CPanelTemplateEntity* CPanelTemplateEntityOutlineArray::addEntity()
{
   return add();
}

CPanelTemplateEntity* CPanelTemplateEntityOutlineArray::addEntityAt(int index)
{
   if (index < 0) index = 0;
   else if (index > getSize()) index = getSize();

   CPanelTemplateEntityOutline* entity = new CPanelTemplateEntityOutline(getPanelizer());
   m_array.InsertAt(index,entity);

   renumber();

   return entity;
}

CPanelTemplateEntity* CPanelTemplateEntityOutlineArray::getEntityAt(int index) const
{
   return getAt(index);
}

CPanelTemplateEntityOutline* CPanelTemplateEntityOutlineArray::add()
{
   CPanelTemplateEntityOutline* entity = new CPanelTemplateEntityOutline(getPanelizer());
   m_array.Add(entity);

   entity->setId(m_array.GetSize());

   return entity;
}

CPanelTemplateEntityOutline* CPanelTemplateEntityOutlineArray::getAt(int index) const
{
   CPanelTemplateEntityOutline* entity = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      entity = m_array.GetAt(index);
   }

   return entity;
}

void CPanelTemplateEntityOutlineArray::deleteAt(int index)
{
   if (index >= 0 && index < m_array.GetSize())
   {
      CPanelTemplateEntityOutline* entity = m_array.GetAt(index);
      m_array.RemoveAt(index);
      delete entity;

      renumber();
   }
}

CExtent CPanelTemplateEntityOutlineArray::GetExtent()
{
   CExtent extent;

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityOutline* entity = m_array.GetAt(index);

      if (index == 0)
      {
         extent = entity->GetExtent();
      }
      else
      {
         CExtent newextent = entity->GetExtent();
         if (newextent.getXmin() < extent.getXmin())
            extent.setXmin( newextent.getXmin() );
         if (newextent.getXmax() > extent.getXmax())
            extent.setXmax( newextent.getXmax() );
         if (newextent.getYmin() < extent.getYmin())
            extent.setYmin( newextent.getYmin() );
         if (newextent.getYmax() > extent.getYmax())
            extent.setYmax( newextent.getYmax() );
      }
   }

   return extent;
}

bool CPanelTemplateEntityOutlineArray::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   dbpanelTemplate.GetOutlineList().empty();

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityOutline* entity = m_array.GetAt(index);

      CDBPanelTemplateOutline* dbentity = entity->AllocDBPanelTemplateOutline();
      dbpanelTemplate.GetOutlineList().Add(dbentity);
   }

   return true;
}

bool CPanelTemplateEntityOutlineArray::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->m_array.empty();
   
   for (int i = 0; i < dbpanelTemplate.GetOutlineList().GetCount(); i++)
   {
      CDBPanelTemplateOutline *dbitem = dbpanelTemplate.GetOutlineList().GetAt(i);

      CPanelTemplateEntityOutline* entity = this->add();
      entity->AdopDBValues(dbpanelTemplate.GetPageUnits(), dbitem);
   }

   return true;
}

//_________________________________________________________________________________________________
CPanelTemplateEntityPcbInstanceArray::CPanelTemplateEntityPcbInstanceArray(CPanelizer& panelizer)
: CPanelTemplateEntityArray(panelizer)
{
}

void CPanelTemplateEntityPcbInstanceArray::empty()
{
   m_array.empty();
}

int CPanelTemplateEntityPcbInstanceArray::getSize() const
{
   return m_array.GetSize();
}

PanelEntityTypeTag CPanelTemplateEntityPcbInstanceArray::getPanelEntityType() const
{
   return panelEntityTypePcbInstance;
}

CPanelTemplateEntity* CPanelTemplateEntityPcbInstanceArray::addEntity()
{
   return add();
}

CPanelTemplateEntity* CPanelTemplateEntityPcbInstanceArray::addEntityAt(int index)
{
   if (index < 0) index = 0;
   else if (index > getSize()) index = getSize();

   CPanelTemplateEntityPcbInstance* entity = new CPanelTemplateEntityPcbInstance(getPanelizer());
   m_array.InsertAt(index,entity);

   entity->setId(index);

   for (++index;index < getSize();index++)
   {
      getAt(index)->setId(index);
   }

   return entity;
}

CPanelTemplateEntity* CPanelTemplateEntityPcbInstanceArray::getEntityAt(int index) const
{
   return getAt(index);
}

CPanelTemplateEntityPcbInstance* CPanelTemplateEntityPcbInstanceArray::add()
{
   CPanelTemplateEntityPcbInstance* entity = new CPanelTemplateEntityPcbInstance(getPanelizer());
   m_array.Add(entity);

   entity->setId(m_array.GetSize());

   return entity;
}

CPanelTemplateEntityPcbInstance* CPanelTemplateEntityPcbInstanceArray::getAt(int index) const
{
   CPanelTemplateEntityPcbInstance* entity = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      entity = m_array.GetAt(index);
   }

   return entity;
}

void CPanelTemplateEntityPcbInstanceArray::deleteAt(int index)
{
   if (index >= 0 && index < m_array.GetSize())
   {
      CPanelTemplateEntityPcbInstance* entity = m_array.GetAt(index);
      m_array.RemoveAt(index);
      delete entity;

      renumber();
   }
}


bool CPanelTemplateEntityPcbInstanceArray::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   dbpanelTemplate.GetPcbInstanceList().empty();

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityPcbInstance* entity = m_array.GetAt(index);

      CDBPanelTemplatePcbInstance* dbentity = entity->AllocDBPanelTemplatePcbInstance();
      dbpanelTemplate.GetPcbInstanceList().Add(dbentity);
   }

   return true;
}

bool CPanelTemplateEntityPcbInstanceArray::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->m_array.empty();
   
   for (int i = 0; i < dbpanelTemplate.GetPcbInstanceList().GetCount(); i++)
   {
      CDBPanelTemplatePcbInstance *dbitem = dbpanelTemplate.GetPcbInstanceList().GetAt(i);

      CPanelTemplateEntityPcbInstance* entity = this->add();
      entity->AdopDBValues(dbpanelTemplate.GetPageUnits(), dbitem);
   }

   return true;
}

//_________________________________________________________________________________________________
CPanelTemplateEntityPcbArrayArray::CPanelTemplateEntityPcbArrayArray(CPanelizer& panelizer)
: CPanelTemplateEntityArray(panelizer)
{
}

void CPanelTemplateEntityPcbArrayArray::empty()
{
   m_array.empty();
}

int CPanelTemplateEntityPcbArrayArray::getSize() const
{
   return m_array.GetSize();
}

PanelEntityTypeTag CPanelTemplateEntityPcbArrayArray::getPanelEntityType() const
{
   return panelEntityTypePcbArray;
}

CPanelTemplateEntity* CPanelTemplateEntityPcbArrayArray::addEntity()
{
   return add();
}

CPanelTemplateEntity* CPanelTemplateEntityPcbArrayArray::addEntityAt(int index)
{
   if (index < 0) index = 0;
   else if (index > getSize()) index = getSize();

   CPanelTemplateEntityPcbArray* entity = new CPanelTemplateEntityPcbArray(getPanelizer());
   m_array.InsertAt(index,entity);

   entity->setId(index);

   for (++index;index < getSize();index++)
   {
      getAt(index)->setId(index);
   }

   return entity;
}

CPanelTemplateEntity* CPanelTemplateEntityPcbArrayArray::getEntityAt(int index) const
{
   return getAt(index);
}

CPanelTemplateEntityPcbArray* CPanelTemplateEntityPcbArrayArray::add()
{
   CPanelTemplateEntityPcbArray* entity = new CPanelTemplateEntityPcbArray(getPanelizer());
   m_array.Add(entity);

   entity->setId(m_array.GetSize());

   return entity;
}

CPanelTemplateEntityPcbArray* CPanelTemplateEntityPcbArrayArray::getAt(int index) const
{
   CPanelTemplateEntityPcbArray* entity = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      entity = m_array.GetAt(index);
   }

   return entity;
}

void CPanelTemplateEntityPcbArrayArray::deleteAt(int index)
{
   if (index >= 0 && index < m_array.GetSize())
   {
      CPanelTemplateEntityPcbArray* entity = m_array.GetAt(index);
      m_array.RemoveAt(index);
      delete entity;

      renumber();
   }
}

bool CPanelTemplateEntityPcbArrayArray::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   dbpanelTemplate.GetPcbArrayList().empty();

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityPcbArray* entity = m_array.GetAt(index);

      CDBPanelTemplatePcbArray* dbentity = entity->AllocDBPanelTemplatePcbArray();
      dbpanelTemplate.GetPcbArrayList().Add(dbentity);
   }

   return true;
}


bool CPanelTemplateEntityPcbArrayArray::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->m_array.empty();
   
   for (int i = 0; i < dbpanelTemplate.GetPcbArrayList().GetCount(); i++)
   {
      CDBPanelTemplatePcbArray *dbitem = dbpanelTemplate.GetPcbArrayList().GetAt(i);

      CPanelTemplateEntityPcbArray* entity = this->add();
      entity->AdopDBValues(dbpanelTemplate.GetPageUnits(), dbitem);
   }

   return true;
}

//_________________________________________________________________________________________________
CPanelTemplateEntityFiducialArray::CPanelTemplateEntityFiducialArray(CPanelizer& panelizer)
: CPanelTemplateEntityArray(panelizer)
{
}

void CPanelTemplateEntityFiducialArray::empty()
{
   m_array.empty();
}

int CPanelTemplateEntityFiducialArray::getSize() const
{
   return m_array.GetSize();
}

PanelEntityTypeTag CPanelTemplateEntityFiducialArray::getPanelEntityType() const
{
   return panelEntityTypeFiducial;
}

CPanelTemplateEntity* CPanelTemplateEntityFiducialArray::addEntity()
{
   return add();
}

CPanelTemplateEntity* CPanelTemplateEntityFiducialArray::addEntityAt(int index)
{
   if (index < 0) index = 0;
   else if (index > getSize()) index = getSize();

   CPanelTemplateEntityFiducial* entity = new CPanelTemplateEntityFiducial(getPanelizer());
   m_array.InsertAt(index,entity);

   entity->setId(index);

   for (++index;index < getSize();index++)
   {
      getAt(index)->setId(index);
   }

   return entity;
}

CPanelTemplateEntity* CPanelTemplateEntityFiducialArray::getEntityAt(int index) const
{
   return getAt(index);
}

CPanelTemplateEntityFiducial* CPanelTemplateEntityFiducialArray::add()
{
   CPanelTemplateEntityFiducial* entity = new CPanelTemplateEntityFiducial(getPanelizer());
   m_array.Add(entity);

   entity->setId(m_array.GetSize());

   return entity;
}

CPanelTemplateEntityFiducial* CPanelTemplateEntityFiducialArray::getAt(int index) const
{
   CPanelTemplateEntityFiducial* entity = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      entity = m_array.GetAt(index);
   }

   return entity;
}

void CPanelTemplateEntityFiducialArray::deleteAt(int index)
{
   if (index >= 0 && index < m_array.GetSize())
   {
      CPanelTemplateEntityFiducial* entity = m_array.GetAt(index);
      m_array.RemoveAt(index);
      delete entity;

      renumber();
   }
}

bool CPanelTemplateEntityFiducialArray::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   dbpanelTemplate.GetFiducialList().empty();

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityFiducial* entity = m_array.GetAt(index);

      CDBPanelTemplateFiducial* dbentity = entity->AllocDBPanelTemplateFiducial();
      dbpanelTemplate.GetFiducialList().Add(dbentity);
   }

   return true;
}


bool CPanelTemplateEntityFiducialArray::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->m_array.empty();
   
   for (int i = 0; i < dbpanelTemplate.GetFiducialList().GetCount(); i++)
   {
      CDBPanelTemplateFiducial *dbitem = dbpanelTemplate.GetFiducialList().GetAt(i);

      CPanelTemplateEntityFiducial* entity = this->add();
      entity->AdopDBValues(dbpanelTemplate.GetPageUnits(), dbitem);
   }

   return true;
}

//_________________________________________________________________________________________________
CPanelTemplateEntityToolingHoleArray::CPanelTemplateEntityToolingHoleArray(CPanelizer& panelizer)
: CPanelTemplateEntityArray(panelizer)
{
}

void CPanelTemplateEntityToolingHoleArray::empty()
{
   m_array.empty();
}

int CPanelTemplateEntityToolingHoleArray::getSize() const
{
   return m_array.GetSize();
}

PanelEntityTypeTag CPanelTemplateEntityToolingHoleArray::getPanelEntityType() const
{
   return panelEntityTypeToolingHole;
}

CPanelTemplateEntity* CPanelTemplateEntityToolingHoleArray::addEntity()
{
   return add();
}

CPanelTemplateEntity* CPanelTemplateEntityToolingHoleArray::addEntityAt(int index)
{
   if (index < 0) index = 0;
   else if (index > getSize()) index = getSize();

   CPanelTemplateEntityToolingHole* entity = new CPanelTemplateEntityToolingHole(getPanelizer());
   m_array.InsertAt(index,entity);

   entity->setId(index);

   for (++index;index < getSize();index++)
   {
      getAt(index)->setId(index);
   }

   return entity;
}

CPanelTemplateEntity* CPanelTemplateEntityToolingHoleArray::getEntityAt(int index) const
{
   return getAt(index);
}

CPanelTemplateEntityToolingHole* CPanelTemplateEntityToolingHoleArray::add()
{
   CPanelTemplateEntityToolingHole* entity = new CPanelTemplateEntityToolingHole(getPanelizer());
   m_array.Add(entity);

   entity->setId(m_array.GetSize());

   return entity;
}

CPanelTemplateEntityToolingHole* CPanelTemplateEntityToolingHoleArray::getAt(int index) const
{
   CPanelTemplateEntityToolingHole* entity = NULL;

   if (index >= 0 && index < m_array.GetSize())
   {
      entity = m_array.GetAt(index);
   }

   return entity;
}

void CPanelTemplateEntityToolingHoleArray::deleteAt(int index)
{
   if (index >= 0 && index < m_array.GetSize())
   {
      CPanelTemplateEntityToolingHole* entity = m_array.GetAt(index);
      m_array.RemoveAt(index);
      delete entity;

      renumber();
   }
}

bool CPanelTemplateEntityToolingHoleArray::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   dbpanelTemplate.GetToolingHoleList().empty();

   for (int index = 0; index < m_array.GetSize(); index++)
   {
      CPanelTemplateEntityToolingHole* entity = m_array.GetAt(index);

      CDBPanelTemplateToolingHole* dbentity = entity->AllocDBPanelTemplateToolingHole();
      dbpanelTemplate.GetToolingHoleList().Add(dbentity);
   }

   return true;
}


bool CPanelTemplateEntityToolingHoleArray::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->m_array.empty();
   
   for (int i = 0; i < dbpanelTemplate.GetToolingHoleList().GetCount(); i++)
   {
      CDBPanelTemplateToolingHole *dbitem = dbpanelTemplate.GetToolingHoleList().GetAt(i);

      CPanelTemplateEntityToolingHole* entity = this->add();
      entity->AdopDBValues(dbpanelTemplate.GetPageUnits(), dbitem);
   }

   return true;
}

//_________________________________________________________________________________________________
CPanelizerPcbSubFileEntry::CPanelizerPcbSubFileEntry(CPanelizer& panelizer,const CString& subFileName,FileStruct& subFile)
: m_panelizer(panelizer)
, m_subFileName(subFileName)
, m_subFile(subFile)
, m_pcbOutlineGeometry(NULL)
, m_pcbExtent(NULL)
{
}

CPanelizerPcbSubFileEntry::~CPanelizerPcbSubFileEntry()
{
   delete m_pcbExtent;
}

CCamCadData& CPanelizerPcbSubFileEntry::getCamCadData()
{
   return m_panelizer.getCamCadData();
}

CPanelizer& CPanelizerPcbSubFileEntry::getPanelizer()
{
   return m_panelizer;
}

CString CPanelizerPcbSubFileEntry::getSubFileName() const
{
   return m_subFileName;
}

FileStruct& CPanelizerPcbSubFileEntry::getFileStruct() const
{
   return m_subFile;
}

BlockStruct* CPanelizerPcbSubFileEntry::getPcbOutlineGeometry()
{
   if (m_pcbOutlineGeometry == NULL)
   {
      m_pcbOutlineGeometry = constructPcbOutlineGeometry();
   }

   return m_pcbOutlineGeometry;
}

BlockStruct* CPanelizerPcbSubFileEntry::constructPcbOutlineGeometry()
{
   BlockStruct* outlineGeometry = NULL;

   BlockStruct* subFileBlock = m_subFile.getBlock();

   for (CDataListIterator polyIterator(*subFileBlock,dataTypePoly);polyIterator.hasNext();)
   {
      DataStruct* polyStruct = polyIterator.getNext();

      if (polyStruct->getGraphicClass() == graphicClassBoardOutline)
      {
         DataStruct* outlinePolyStruct = getCamCadData().getNewDataStruct(*polyStruct,false);
         outlinePolyStruct->setLayerIndex(getPanelizer().getPcbOutlineLayer().getLayerIndex());

         CString outlineGeometryName;
         outlineGeometryName.Format("%s_%d_%d",m_subFile.getName(),m_subFile.getFileNumber(),subFileBlock->getBlockNumber());

         outlineGeometry = &(getCamCadData().getDefinedBlock(outlineGeometryName,blockTypePcb,m_panelizer.getPanelizerSubFile().getFileNumber()));

         outlineGeometry->getDataList().AddTail(outlinePolyStruct);

         break;
      }
   }

   if (outlineGeometry == NULL)
   {
      outlineGeometry = getPanelizer().getDefaultPcbOutlineGeometry();

      //LayerStruct& boardOutlineLayer = getCamCadData().getDefinedLayer(ccLayerBoardOutline);

      //CString outlineGeometryName;
      //outlineGeometryName.Format("Default-%s_%d_%d",m_subFile.getName(),m_subFile.getFileNumber(),subFileBlock->getBlockNumber());

      //outlineGeometry = &(getCamCadData().getDefinedBlock(outlineGeometryName,blockTypePcb,m_panelizer.getPanelizerSubFile().getFileNumber()));

      //DataStruct* outlinePolyStruct = getCamCadData().getNewDataStruct(dataTypePoly);
      //double unitLength = getCamCadData().convertToPageUnits(pageUnitsMils,5000.);
      //double width = getCamCadData().convertToPageUnits(pageUnitsMils,20.);
      //int widthIndex = getCamCadData().getDefinedWidthIndex(width);

      //CPoly* poly = getCamCadData().addClosedPoly(*outlinePolyStruct,widthIndex);

      //getCamCadData().addVertex(*poly,0.        ,0.);
      //getCamCadData().addVertex(*poly,unitLength,0.);
      //getCamCadData().addVertex(*poly,unitLength,unitLength/2.);
      //getCamCadData().addVertex(*poly,0.        ,unitLength/2.);

      //poly->close();

      //outlineGeometry->getDataList().AddTail(outlinePolyStruct);
   }

   return outlineGeometry;
}

const CExtent& CPanelizerPcbSubFileEntry::getExtent()
{
   if (m_pcbExtent == NULL)
   {
      m_pcbExtent = new CExtent();

      BlockStruct* pcbOutlineGeometry = getPcbOutlineGeometry();

      if (pcbOutlineGeometry != NULL)
      {
         *m_pcbExtent = pcbOutlineGeometry->getExtent(getCamCadData());
      }
   }

   return *m_pcbExtent;
}

//_________________________________________________________________________________________________
CPanelizerPcbSubFileMap::CPanelizerPcbSubFileMap(CPanelizer& panelizer)
: m_panelizer(panelizer)
{
}

CPanelizerPcbSubFileEntry* CPanelizerPcbSubFileMap::addEntry(FileStruct* subFile)
{
   CString subFileName = subFile->getName();
   CPanelizerPcbSubFileEntry* entry = NULL;

   for (int suffix = 1;m_subFileMap.Lookup(subFileName,entry);suffix++)
   {
      subFileName.Format("%s_%d",subFile->getName(),suffix);
   }

   entry = new CPanelizerPcbSubFileEntry(m_panelizer,subFileName,*subFile);

   m_subFileMap.SetAt(subFileName,entry);

   return entry;
}

CPanelizerPcbSubFileEntry* CPanelizerPcbSubFileMap::getEntry(const CString& physicalName)
{
   CPanelizerPcbSubFileEntry* entry;

   if (! m_subFileMap.Lookup(physicalName,entry))
   {
      entry = NULL;
   }

   return entry;
}

int CPanelizerPcbSubFileMap::getPhysicalPcbFileCount() const
{
   return m_subFileMap.GetCount();
}

const CPanelizerPcbSubFileEntry *CPanelizerPcbSubFileMap::getHead() const
{
   const CPanelizerPcbSubFileEntry* entry = NULL;

   POSITION pos = this->getStartPosition();
   if (pos != NULL)
      entry = this->getNext(pos);

   return entry;
}


POSITION CPanelizerPcbSubFileMap::getStartPosition() const
{
   return m_subFileMap.GetStartPosition();
}

const CPanelizerPcbSubFileEntry* CPanelizerPcbSubFileMap::getNext(POSITION& pos) const
{
   CPanelizerPcbSubFileEntry* entry;
   CString key;

   m_subFileMap.GetNextAssoc(pos,key,entry);

   return entry;
}

CString CPanelizerPcbSubFileMap::getPcbFilesLabelString()
{
   CString pcbFilesLabelString;

   CString* key;
   CPanelizerPcbSubFileEntry* entry = NULL;

   for (m_subFileMap.GetFirstSorted(key,entry);entry != NULL;m_subFileMap.GetNextSorted(key,entry))
   {
      pcbFilesLabelString += entry->getSubFileName() + "\n";
   }

   return pcbFilesLabelString;
}

void CPanelizerPcbSubFileMap::mapLogicalNameToPhysicalName(const CString& logicalName,const CString& physicalName)
{
   m_logicalNameToPhysicalNameMap.SetAt(logicalName,physicalName);
}

CString CPanelizerPcbSubFileMap::getPhysicalNameForLogicalName(const CString& logicalName)
{
   CString physicalName;
   
   if (! m_logicalNameToPhysicalNameMap.Lookup(logicalName,physicalName))
   {
      physicalName.Empty();
   }

   return physicalName;
}

CString CPanelizerPcbSubFileMap::getUniqueLogicalName()
{
   CString uniqueLogicalName,physicalName;

   for (int suffix = 1;;suffix++)
   {
      uniqueLogicalName.Format("Board_%d",suffix);

      if (! m_logicalNameToPhysicalNameMap.Lookup(uniqueLogicalName,physicalName))
      {
         m_logicalNameToPhysicalNameMap.SetAt(uniqueLogicalName,"");

         break;
      }
   }

   return uniqueLogicalName;
}

//_________________________________________________________________________________________________
CPanelTemplate::CPanelTemplate(CPanelizer& panelizer)
: m_panelizer(panelizer)
, m_outlines(panelizer)
, m_pcbInstances(panelizer)
, m_pcbArrays(panelizer)
, m_fiducials(panelizer)
, m_toolingHoles(panelizer)
{
}

void CPanelTemplate::empty()
{
   m_outlines.empty();
   m_pcbInstances.empty();
   m_pcbArrays.empty();
   m_fiducials.empty();
   m_toolingHoles.empty();
}

CPanelizer& CPanelTemplate::getPanelizer()
{
   return m_panelizer;
}

CCEtoODBDoc& CPanelTemplate::getCamCadDoc()
{
   return getPanelizer().getCamCadDoc();
}

CCamCadData& CPanelTemplate::getCamCadData()
{
   return getPanelizer().getCamCadData();
}

FileStruct& CPanelTemplate::getPanelizerSubFile()
{
   return getPanelizer().getPanelizerSubFile();
}

bool CPanelTemplate::getModifiedFlag() const
{
   bool modifiedFlag = true;

   while (true)
   {
      if (m_outlines.getModifiedFlag()         ) break;
      if (m_pcbInstances.getModifiedFlag()     ) break;
      if (m_pcbArrays.getModifiedFlag()        ) break;
      if (m_fiducials.getModifiedFlag()        ) break;
      if (m_toolingHoles.getModifiedFlag()     ) break;

      modifiedFlag = false;

      break;
   }

   return modifiedFlag;
}


void CPanelTemplate::setModifiedFlag(bool flag)
{
   m_outlines.setModifiedFlag(flag);
   m_pcbInstances.setModifiedFlag(flag);
   m_pcbArrays.setModifiedFlag(flag);
   m_fiducials.setModifiedFlag(flag);
   m_toolingHoles.setModifiedFlag(flag);
}

bool CPanelTemplate::getModifiedSinceLoadFlag() const
{
   bool modifiedFlag = true;

   while (true)
   {
      if (m_outlines.getModifiedSinceLoadFlag()         ) break;
      if (m_pcbInstances.getModifiedSinceLoadFlag()     ) break;
      if (m_pcbArrays.getModifiedSinceLoadFlag()        ) break;
      if (m_fiducials.getModifiedSinceLoadFlag()        ) break;
      if (m_toolingHoles.getModifiedSinceLoadFlag()     ) break;

      modifiedFlag = false;

      break;
   }

   return modifiedFlag;
}


void CPanelTemplate::setModifiedSinceLoadFlag(bool flag)
{
   m_outlines.setModifiedSinceLoadFlag(flag);
   m_pcbInstances.setModifiedSinceLoadFlag(flag);
   m_pcbArrays.setModifiedSinceLoadFlag(flag);
   m_fiducials.setModifiedSinceLoadFlag(flag);
   m_toolingHoles.setModifiedSinceLoadFlag(flag);
}

CPanelTemplateEntityArray& CPanelTemplate::getEntityArray(PanelEntityTypeTag entityType)
{
   CPanelTemplateEntityArray* entityArray = &m_outlines;

   switch (entityType)
   {
   case panelEntityTypeOutline:      entityArray = &m_outlines;           break;
   case panelEntityTypePcbInstance:  entityArray = &m_pcbInstances;       break;
   case panelEntityTypePcbArray:     entityArray = &m_pcbArrays;          break;
   case panelEntityTypeFiducial:     entityArray = &m_fiducials;          break;
   case panelEntityTypeToolingHole:  entityArray = &m_toolingHoles;       break;
   }

   return *entityArray;
}

const CPanelTemplateEntityArray& CPanelTemplate::getEntityArray(PanelEntityTypeTag entityType) const
{
   const CPanelTemplateEntityArray* entityArray = &m_outlines;

   switch (entityType)
   {
   case panelEntityTypeOutline:      entityArray = &m_outlines;           break;
   case panelEntityTypePcbInstance:  entityArray = &m_pcbInstances;       break;
   case panelEntityTypePcbArray:     entityArray = &m_pcbArrays;          break;
   case panelEntityTypeFiducial:     entityArray = &m_fiducials;          break;
   case panelEntityTypeToolingHole:  entityArray = &m_toolingHoles;       break;
   }

   return *entityArray;
}

int CPanelTemplate::addCopyOfEntity(const CPanelTemplateEntity& entity)
{
   int retval = -1;

   switch (entity.getPanelEntityType())
   {
   case panelEntityTypeOutline:      m_outlines.add(     )->operator=(*((CPanelTemplateEntityOutline*    )(&entity)));  break;
   case panelEntityTypePcbInstance:  m_pcbInstances.add( )->operator=(*((CPanelTemplateEntityPcbInstance*)(&entity)));  break;
   case panelEntityTypePcbArray:     m_pcbArrays.add(    )->operator=(*((CPanelTemplateEntityPcbArray*   )(&entity)));  break;
   case panelEntityTypeFiducial:     m_fiducials.add(    )->operator=(*((CPanelTemplateEntityFiducial*   )(&entity)));  break;
   case panelEntityTypeToolingHole:  m_toolingHoles.add( )->operator=(*((CPanelTemplateEntityToolingHole*)(&entity)));  break;
   }

   return retval;
}

void CPanelTemplate::copyEntities(const CPanelTemplate& sourcePanelTemplate,PanelEntityTypeTag entityType,int insertIndex)
{
   getEntityArray(entityType).copyEntities(sourcePanelTemplate.getEntityArray(entityType),insertIndex);
}

void CPanelTemplate::deleteEntity(PanelEntityTypeTag entityType,int index)
{
   getEntityArray(entityType).deleteAt(index);
}

void CPanelTemplate::write(CWriteFormat& writeFormat)
{
   writeFormat.writef("Units %s\n",quoteString(PageUnitsTagToString(getCamCadData().getPageUnits())));

   m_outlines.write(writeFormat);
   m_pcbInstances.write(writeFormat);
   m_pcbArrays.write(writeFormat);
   m_fiducials.write(writeFormat);
   m_toolingHoles.write(writeFormat);
}

bool CPanelTemplate::PopulateDBPanelTemplate(CDBPanelTemplate &dbpanelTemplate)
{
   if (!this->GetName().IsEmpty())
   {
      dbpanelTemplate.SetName(this->GetName());
      dbpanelTemplate.SetPageUnits(this->getPanelizer().getPageUnits());

      m_outlines.PopulateDBPanelTemplate(dbpanelTemplate);
      m_pcbInstances.PopulateDBPanelTemplate(dbpanelTemplate);
      m_pcbArrays.PopulateDBPanelTemplate(dbpanelTemplate);
      m_fiducials.PopulateDBPanelTemplate(dbpanelTemplate);
      m_toolingHoles.PopulateDBPanelTemplate(dbpanelTemplate);

      return true;
   }

   return false;
}

bool CPanelTemplate::AdoptDBValues(CDBPanelTemplate &dbpanelTemplate)
{
   this->SetName( dbpanelTemplate.GetName() );

   m_outlines.AdoptDBValues(dbpanelTemplate);
   m_pcbInstances.AdoptDBValues(dbpanelTemplate);
   m_pcbArrays.AdoptDBValues(dbpanelTemplate);
   m_fiducials.AdoptDBValues(dbpanelTemplate);
   m_toolingHoles.AdoptDBValues(dbpanelTemplate);

   return true;
}

bool CPanelTemplate::SaveToDB()
{
   // Database units ALWAYS millimeters

   bool success = false;

   if (!this->GetName().IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPanelTemplate pt;
         if (PopulateDBPanelTemplate(pt))
            success = db.SavePanelTemplate(&pt);
      }
      
      if (success)
         this->setModifiedSinceLoadFlag(false);  // Now all is in sync, DB has been updated to match GUI
   }

   return success;
}

bool CPanelTemplate::LoadFromDB(CString templateName)
{
   empty();

   if (!templateName.IsEmpty())
   {
      CDBInterface db;
      if (db.Connect())
      {
         CDBPanelTemplate *pt = db.LookupPanelTemplate(templateName);

         if (pt != NULL)
         {
            // Adopt values from DB
            this->AdoptDBValues(*pt);

            delete pt;
         }
      }

      this->setModifiedSinceLoadFlag(false);  // Now in sync with fresh Load
   }

   return false;
}

void CPanelTemplate::readFile(const CString& filePath)
{
   empty();

   CStdioFile file;

   if (file.Open(filePath,CFile::modeRead | CFile::shareDenyNone))
   {
      PageUnitsTag pageUnits = getCamCadData().getPageUnits();
      CSupString line;
      CStringArray params;
      int numPar;

      while (file.ReadString(line))
      {
         line.Trim();
         numPar = line.ParseQuote(params," ");

         if (numPar > 0)
         {
            CString entityString(params.GetAt(0));

            if (entityString.Left(1).Compare(";") != 0)
            {
               PanelEntityTypeTag entityType = StringToPanelEntityTypeTag(entityString);

               CPanelTemplateEntity* entity = NULL;

               switch (entityType)
               {
               case panelEntityTypeOutline:      entity = m_outlines.addParsedEntity(params,pageUnits);           break;
               case panelEntityTypePcbInstance:  entity = m_pcbInstances.addParsedEntity(params,pageUnits);       break;
               case panelEntityTypePcbArray:     entity = m_pcbArrays.addParsedEntity(params,pageUnits);          break;
               case panelEntityTypeFiducial:     entity = m_fiducials.addParsedEntity(params,pageUnits);          break;
               case panelEntityTypeToolingHole:  entity = m_toolingHoles.addParsedEntity(params,pageUnits);       break;
               default:
                  {
                     if (entityString.CompareNoCase("Units") == 0)
                     {
                        pageUnits = StringToPageUnitsTag(entityString);

                        if (pageUnits == pageUnitsUndefined)
                        {
                           pageUnits = getCamCadData().getPageUnits();
                        }
                     }
                  }
               }

               if (entity == NULL)
               {
                  // error message
               }
            }
         }
      }

      // only one outline allowed, keep the last if multiple outlines are read.
      while (m_outlines.getSize() > 1)
      {
         m_outlines.deleteFirst();
      }
   }
}

void CPanelTemplate::regenerateFile()
{
   getCamCadDoc().UnselectAll(false);
   getPanelizerSubFile().getBlock()->getDataList().empty();

   m_outlines.regenerateFile();
   m_pcbInstances.regenerateFile();
   m_pcbArrays.regenerateFile();
   m_fiducials.regenerateFile();
   m_toolingHoles.regenerateFile();

   getPanelizerSubFile().getBlock()->calculateBlockExtents(getCamCadData());

   //getCamCadDoc().OnRedraw();
   getCamCadDoc().OnFitPageToImage();
}

void CPanelTemplate::instantiateFile(FileStruct& targetPanelFile)
{   
   getCamCadDoc().UnselectAll(false);
   targetPanelFile.getBlock()->getDataList().empty();

   m_outlines.instantiateFile(targetPanelFile);
   m_pcbInstances.instantiateFile(targetPanelFile);
   m_pcbArrays.instantiateFile(targetPanelFile);
   m_fiducials.instantiateFile(targetPanelFile);
   m_toolingHoles.instantiateFile(targetPanelFile);

   targetPanelFile.getBlock()->calculateBlockExtents(getCamCadData());

   //getCamCadDoc().OnRedraw();
   getCamCadDoc().OnFitPageToImage();
}

int CPanelTemplateEntityPcbInstanceArray::ValidateLayout(CExtent& panelExtent)
{
   int violationCount = 0;

   if (panelExtent.isValid())
   {
      for (int i = 0; i < this->getSize(); i++)
      {
         CPanelTemplateEntityPcbInstance *pcbInst = this->getAt(i);

         CBasesVector placementBasesVector;
         pcbInst->GetPlacementBasesVector(placementBasesVector);

         CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(pcbInst->getText(0));//m_name);

         pcbExtent.transform(placementBasesVector.getTransformationMatrix());

         if (!panelExtent.contains(pcbExtent))
         {
            violationCount++;
         }
      }
   }

   return violationCount;
}

int CPanelTemplateEntityPcbArrayArray::ValidateLayout(CExtent& panelExtent)
{
   int violationCount = 0;

   if (panelExtent.isValid())
   {
      for (int i = 0; i < this->getSize(); i++)
      {
         CPanelTemplateEntityPcbArray *pcbAr = this->getAt(i);

         for (int rowIndex = 0; rowIndex < pcbAr->getYCount(); rowIndex++)
         {
            for (int colIndex = 0; colIndex < pcbAr->getXCount(); colIndex++)
            {
               CBasesVector placementBasesVector;
               pcbAr->GetPlacementBasesVector(colIndex, rowIndex, placementBasesVector);

               CExtent pcbExtent = getPanelizer().getPcbExtentForLogicalName(pcbAr->getText(0));//m_name);

               pcbExtent.transform(placementBasesVector.getTransformationMatrix());

               if (!panelExtent.contains(pcbExtent))
               {
                  violationCount++;
               }
            }
         }
      }
   }

   return violationCount;
}

CExtent CPanelTemplateEntityFiducial::GetExtent()
{
   BlockStruct& fiducialGeometry = getPanelizer().getFiducialGenerator().getDefinedFiducialGeometry(m_fiducialType, m_size, getPanelizer().getPanelizerSubFile().getFileNumber());
   
   CExtent extent = getPanelizer().getCamCadData().blockExtents(fiducialGeometry.getDataList(), m_xOrigin, m_yOrigin, m_angleRadians, this->m_mirrored, 1.0, -1, false, NULL);

   return extent;
}

int CPanelTemplateEntityFiducialArray::ValidateLayout(CExtent& panelExtent)
{
   int violationCount = 0;

   for (int i = 0; i < this->getSize(); i++)
   {
      CPanelTemplateEntityFiducial *fid = this->getAt(i);

      CExtent fidExtent = fid->GetExtent();

      if (!panelExtent.contains(fidExtent))
         violationCount++;
   }

   return violationCount;
}

int CPanelTemplateEntityToolingHoleArray::ValidateLayout(CExtent& panelExtent)
{
   int violationCount = 0;

   for (int i = 0; i < this->getSize(); i++)
   {
      CPanelTemplateEntityToolingHole *th = this->getAt(i);

      CExtent thExtent = th->GetExtent();

      if (!panelExtent.contains(thExtent))
         violationCount++;
   }

   return violationCount;
}

bool CPanelTemplate::ValidateLayout(CString &msg)
{
   // Layout is deemed valid if PCBs do not extend out of panel.
   // *rcf Good place to test is all pcbs have physical assignment.

   int pcbInstViolationCount = 0;
   int pcbArViolationCount = 0;
   int fidViolationCount = 0;
   int toolViolationCount = 0;

   msg.Empty();

   // We only check when there is something to check agains, i.e. the panel outline.
   // If there is no outline then there are no violations, since so far we do not "require" any certain data.
   // (This may change, e.g. my want to check physical pcb assignment has been made).

   CExtent panelExtent = m_outlines.GetExtent();

   if (panelExtent.isValid())
   {
      pcbInstViolationCount = m_pcbInstances.ValidateLayout(panelExtent);
      pcbArViolationCount = m_pcbArrays.ValidateLayout(panelExtent);;
      fidViolationCount = m_fiducials.ValidateLayout(panelExtent);
      toolViolationCount = m_toolingHoles.ValidateLayout(panelExtent);
   }

   if ((pcbInstViolationCount + pcbArViolationCount) > 0)
   {
      int totalPcbViolations = pcbInstViolationCount + pcbArViolationCount;
      CString buf;
      buf.Format("%d Board%s extend%s beyond panel outline.", 
         totalPcbViolations, totalPcbViolations > 1 ? "s" : "", totalPcbViolations > 1 ? "" : "s");
      msg += buf;
   }

   if (fidViolationCount > 0)
   {
      CString buf;
      buf.Format("%d Fiducial%s extend%s beyond panel outline.", 
         fidViolationCount, fidViolationCount > 1 ? "s" : "", fidViolationCount > 1 ? "" : "s");
      if (!msg.IsEmpty())
         msg += "\n";
      msg += buf;
   }

   if (toolViolationCount > 0)
   {
      CString buf;
      buf.Format("%d Tool Hole%s extend%s beyond panel outline.", 
         toolViolationCount, toolViolationCount > 1 ? "s" : "", toolViolationCount > 1 ? "" : "s");
      if (!msg.IsEmpty())
         msg += "\n";
      msg += buf;
   }

   return msg.IsEmpty();
}

//_________________________________________________________________________________________________
CPanelizer::CPanelizer(CCEtoODBDoc& camCadDoc)
: m_camCadDoc(camCadDoc)
, m_panelTemplate(*this)
, m_panelizerSubFile(NULL)
, m_panelizerFileName("$$Panelizer$$")
, m_pcbSubFileMap(*this)
, m_fiducialGenerator(camCadDoc.getCamCadData())
, m_defaultPcbOutlineGeometry(NULL)
, m_panelOutlineLayer(NULL)
, m_pcbOutlineLayer(NULL)
, m_pcbArrayGridLayer(NULL)
, m_padTopLayer(NULL)
, m_padBottomLayer(NULL)
, m_maskTopLayer(NULL)
, m_maskBottomLayer(NULL)
{
   camCadDoc.StoreDocForImporting();
   scanForPcbSubFiles();
   regenerateFile();
}

CPanelizer::~CPanelizer()
{
   //m_camCadDoc.OnClearSelected();

   if (m_panelizerSubFile != NULL)
      getCamCadData().getFileList().deleteFile(m_panelizerSubFile);
}

void CPanelizer::scanForPcbSubFiles()
{
   CFileList& subFileList = getCamCadData().getFileList();

   for (POSITION pos = subFileList.GetHeadPosition();pos != NULL;)
   {
      FileStruct* subFile = subFileList.GetNext(pos);

      if (subFile->getBlockType() == blockTypePcb)
      {
         m_pcbSubFileMap.addEntry(subFile);
      }

      subFile->setShow(false);
   }
}

void CPanelizer::AutoAssignPhysicalPcb()
{
   // Auto assign will occur only if there is a single physical PCB in ccz data.
   // If there is a single PCB then assign it to all template pcb instances and pcb arrays.

   if (this->getPcbSubFileMap().getPhysicalPcbFileCount() == 1)
   {
      const CPanelizerPcbSubFileEntry *entry = this->getPcbSubFileMap().getHead();

      if (entry != NULL) // it shouldn't be NULL, but I'm paranoid
      {
         CString physicalPcbName( entry->getSubFileName() );

         for (int i = 0; i < this->m_panelTemplate.getPcbInstances().getSize(); i++)
         {
            CPanelTemplateEntityPcbInstance *pcb = this->m_panelTemplate.getPcbInstances().getAt(i);
            pcb->setValue(6, physicalPcbName);  //*rcf GOT TO DO SOMETHING about these int constants, this is the biggest WEAK LINK in Panelizer
         }

         for (int i = 0; i < this->m_panelTemplate.getPcbArrays().getSize(); i++)
         {
            CPanelTemplateEntityPcbArray *pcb = this->m_panelTemplate.getPcbArrays().getAt(i);
            pcb->setValue(10, physicalPcbName); //*rcf GOT TO DO SOMETHING about these int constants, this is the biggest WEAK LINK in Panelizer
         }
      }
   }
}

CCEtoODBDoc& CPanelizer::getCamCadDoc() const
{
   return m_camCadDoc;
}

CCamCadData& CPanelizer::getCamCadData() const
{
   return m_camCadDoc.getCamCadData();
}

CPanelTemplate& CPanelizer::getPanelTemplate()
{
   return m_panelTemplate;
}

PageUnitsTag CPanelizer::getPageUnits()
{
   return getCamCadData().getPageUnits();
}

FileStruct& CPanelizer::getPanelizerSubFile()
{
   if (m_panelizerSubFile == NULL)
   {
      m_panelizerSubFile = getCamCadData().getFileList().FindByName(m_panelizerFileName);

      if (m_panelizerSubFile == NULL)
      {
         m_panelizerSubFile = Graph_File_Start(m_panelizerFileName, fileTypeCamcadUser);
      }

      m_panelizerSubFile->setNotPlacedYet(false);
      m_panelizerSubFile->setShow(true);
   }

   return *m_panelizerSubFile;
}

const CPanelizerPcbSubFileMap& CPanelizer::getPcbSubFileMap() const
{
   return m_pcbSubFileMap;
}

CFiducialGenerator& CPanelizer::getFiducialGenerator()
{
   return m_fiducialGenerator;
}

void CPanelizer::regenerateFile()
{
   getFiducialGenerator().setPadTopLayer(&(getPadTopLayer()));
   getFiducialGenerator().setMaskTopLayer(&(getMaskTopLayer()));

   m_panelTemplate.regenerateFile();
}

FileStruct *CPanelizer::instantiateFile(CString panelName)
{
   // aka Apply Template

   const CPanelizerPcbSubFileMap& pcbSubFileMap = getPcbSubFileMap();

   for (POSITION pos = pcbSubFileMap.getStartPosition();pos != NULL;)
   {
      const CPanelizerPcbSubFileEntry* subFileEntry = pcbSubFileMap.getNext(pos);
      FileStruct& subFileStruct = subFileEntry->getFileStruct();
      getFiducialGenerator().analyzeLayers(*(subFileStruct.getBlock()));
   }

   getFiducialGenerator().selectTopLayerFromAnalysis();

   FileStruct *newFile = this->getCamCadData().getFileList().addNewFile(panelName, fileTypeCamcadUser);

   m_panelTemplate.instantiateFile(*newFile);
   newFile->setName(panelName);
   newFile->setBlockType(blockTypePanel);
   newFile->setNotPlacedYet(false);
   newFile->setShow(true);

   return newFile;
}

void CPanelizer::updateModifiedPanel()
{
   if (m_panelTemplate.getModifiedFlag())
   {
      m_panelTemplate.regenerateFile();
      m_panelTemplate.setModifiedFlag(false);
   }
}

CString CPanelizer::getPcbFilesLabelString()
{
   return m_pcbSubFileMap.getPcbFilesLabelString();
}

void CPanelizer::mapLogicalNameToPhysicalName(const CString& logicalName,const CString& physicalName)
{
   m_pcbSubFileMap.mapLogicalNameToPhysicalName(logicalName,physicalName);

   //m_panelTemplate.setPhysicalNameForLogicalName(physicalName,logicalName);
}

CString CPanelizer::getPhysicalNameForLogicalName(const CString& logicalName)
{
   CString physicalName = m_pcbSubFileMap.getPhysicalNameForLogicalName(logicalName);

   return physicalName;
}

CString CPanelizer::getUniqueLogicalName()
{
   CString uniqueLogicalName = m_pcbSubFileMap.getUniqueLogicalName();

   return uniqueLogicalName;
}

int CPanelizer::getPadTopLayerIndex()
{
   return getCamCadData().getDefinedLayer(ccLayerPadTop).getLayerIndex();
}

int CPanelizer::getMaskTopLayerIndex()
{
   return getCamCadData().getDefinedLayer(ccLayerMaskTop).getLayerIndex();
}

LayerStruct& CPanelizer::getPanelOutlineLayer()
{
   if (m_panelOutlineLayer == NULL)
   {
      m_panelOutlineLayer = &(getCamCadData().getDefinedLayer("Panelizer-PanelOutline"));

      m_panelOutlineLayer->setColor(colorCyan);
   }

   return *m_panelOutlineLayer;
}

LayerStruct& CPanelizer::getPcbOutlineLayer()
{
   if (m_pcbOutlineLayer == NULL)
   {
      m_pcbOutlineLayer = &(getCamCadData().getDefinedLayer("Panelizer-PcbOutline"));

      m_pcbOutlineLayer->setColor(colorBlue);
   }

   return *m_pcbOutlineLayer;
}

LayerStruct& CPanelizer::getPcbArrayGridLayer()
{
   if (m_pcbArrayGridLayer == NULL)
   {
      m_pcbArrayGridLayer = &(getCamCadData().getDefinedLayer("Panelizer-PcbArrayGrid"));

      m_pcbArrayGridLayer->setColor(colorYellow);
   }

   return *m_pcbArrayGridLayer;
}

LayerStruct& CPanelizer::getPadTopLayer()
{
   if (m_padTopLayer == NULL)
   {
      m_padTopLayer = &(getCamCadData().getDefinedLayer("Panelizer-PadTop"));

      m_padTopLayer->setColor(colorRed);
   }

   return *m_padTopLayer;
}

LayerStruct& CPanelizer::getPadBottomLayer()
{
   if (m_padBottomLayer == NULL)
   {
      m_padBottomLayer = &(getCamCadData().getDefinedLayer("Panelizer-PadBottom"));

      m_padBottomLayer->setColor(colorGreen);
   }

   return *m_padBottomLayer;
}

LayerStruct& CPanelizer::getMaskTopLayer()
{
   if (m_maskTopLayer == NULL)
   {
      m_maskTopLayer = &(getCamCadData().getDefinedLayer("Panelizer-MaskTop"));

      m_maskTopLayer->setColor(colorOrange);
   }

   return *m_maskTopLayer;
}

LayerStruct& CPanelizer::getMaskBottomLayer()
{
   if (m_maskBottomLayer == NULL)
   {
      m_maskBottomLayer = &(getCamCadData().getDefinedLayer("Panelizer-MaskBottom"));

      m_maskBottomLayer->setColor(colorDkGreen);
   }

   return *m_maskBottomLayer;
}

BlockStruct* CPanelizer::getDefaultPcbOutlineGeometry()
{
   if (m_defaultPcbOutlineGeometry == NULL)
   {
      LayerStruct& boardOutlineLayer = getCamCadData().getDefinedLayer(ccLayerBoardOutline);

      CString outlineGeometryName;
      outlineGeometryName.Format("DefaultPcbGeometry");

      m_defaultPcbOutlineGeometry = &(getCamCadData().getDefinedBlock(outlineGeometryName,blockTypePcb,getPanelizerSubFile().getFileNumber()));

      DataStruct* outlinePolyStruct = getCamCadData().getNewDataStruct(dataTypePoly);
      outlinePolyStruct->setLayerIndex(getPcbOutlineLayer().getLayerIndex());
      double unitLength = getCamCadData().convertToPageUnits(pageUnitsMils,5000.);
      double width = getCamCadData().convertToPageUnits(pageUnitsMils,20.);
      int widthIndex = getCamCadData().getDefinedWidthIndex(width);

      CPoly* poly = getCamCadData().addClosedPoly(*outlinePolyStruct,widthIndex);

      getCamCadData().addVertex(*poly,0.        ,0.);
      getCamCadData().addVertex(*poly,unitLength,0.);
      getCamCadData().addVertex(*poly,unitLength,unitLength/2.);
      getCamCadData().addVertex(*poly,0.        ,unitLength/2.);

      poly->close();

      m_defaultPcbOutlineGeometry->getDataList().AddTail(outlinePolyStruct);
   }

   return m_defaultPcbOutlineGeometry;
}

BlockStruct* CPanelizer::getOutlineGeometryForLogicalName(const CString& logicalName)
{
   BlockStruct* outlineGeometry = NULL;

   CString physicalName = m_pcbSubFileMap.getPhysicalNameForLogicalName(logicalName);
   CPanelizerPcbSubFileEntry* entry = m_pcbSubFileMap.getEntry(physicalName);

   if (entry != NULL)
   {
      outlineGeometry = entry->getPcbOutlineGeometry();
   }
   else
   {
      outlineGeometry = getDefaultPcbOutlineGeometry();
   }

   return outlineGeometry;
}

BlockStruct* CPanelizer::getPcbGeometryForLogicalName(const CString& logicalName)
{
   BlockStruct* pcbGeometry = NULL;

   CString physicalName = m_pcbSubFileMap.getPhysicalNameForLogicalName(logicalName);
   CPanelizerPcbSubFileEntry* entry = m_pcbSubFileMap.getEntry(physicalName);

   if (entry != NULL)
   {
      pcbGeometry = entry->getFileStruct().getBlock();
   }
   else
   {
      pcbGeometry = getDefaultPcbOutlineGeometry();
   }

   return pcbGeometry;
}

CExtent CPanelizer::getPcbExtentForLogicalName(const CString& logicalName)
{
   CExtent pcbExtent;

   CString physicalName = m_pcbSubFileMap.getPhysicalNameForLogicalName(logicalName);
   CPanelizerPcbSubFileEntry* entry = m_pcbSubFileMap.getEntry(physicalName);

   if (entry != NULL)
   {
      pcbExtent = entry->getExtent();
   }
   else
   {
      pcbExtent = getDefaultPcbOutlineGeometry()->getExtent();
   }

   return pcbExtent;
}
