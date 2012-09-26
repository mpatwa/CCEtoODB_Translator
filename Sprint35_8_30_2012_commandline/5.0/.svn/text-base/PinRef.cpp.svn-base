// $Header: /CAMCAD/4.7/PinRef.cpp 82    12/19/07 2:48p Sharry Huang $
#include "stdafx.h"
#include ".\pinref.h"

/******************************************************************************
* CPinRef
*/

CPinRef::CPinRef(int sysPageUnit)
{
   m_sysPageUnit = sysPageUnit;

   m_locationX = 0.;
   m_locationY = 0.;
   m_Rotation  = 0.;
   m_Width     = 0.;
   m_Height    = 0.;
   m_Pen       = 0.;

   m_UserdefString = "";
   m_fontName = "std";
   m_layerName = "";
   m_Visible = true;
   
   m_verticalposition = verticalPositionCenter;
   m_horizontalposition = horizontalPositionCenter;
   m_rotationType = Rotation_UnKnown;

   //set pageunit 
   m_unitMap.SetAt("TN",UNIT_TNMETER);
   m_unitMap.SetAt("IN",UNIT_INCHES);
   m_unitMap.SetAt("MI",UNIT_MILS);
   m_unitMap.SetAt("ML",UNIT_MILS);
   m_unitMap.SetAt("MM",UNIT_MM);   
}

CPinRef::~CPinRef(void)
{
   m_unitMap.RemoveAll();
}

bool CPinRef::setPinRef(CString CompName, CString PinNumber, CStringArray &propertyArray)
{
   return true;
}

double CPinRef::getScaleFactor(CString unitName)
{
   double scale_factor = 1;
   int cur_pageunit;
   
   if(m_unitMap.Lookup(unitName,cur_pageunit))
      scale_factor = Units_Factor(cur_pageunit, m_sysPageUnit);

   return scale_factor;
}

/******************************************************************************
* CPinRefListMap
*/
CPinRefListMap::~CPinRefListMap()
{
   this->removeAllNodes();
}

void CPinRefListMap::removeAllNodes()
{
   POSITION pos = this->GetStartPosition();
   while(pos)
   {
      CString key;
      CPinRef *pinref = NULL;
      this->GetNextAssoc(pos,key,pinref);
      if(pinref)
         delete pinref;
   }
   this->RemoveAll();
}

void CPinRefListMap::AddPinRefAt(CString pinNumber, int sysPageUnit, CPinRef *pinRef, CStringArray &refDesArray)
{
   if(pinRef)
   {
      SetAt(pinNumber,pinRef); 
      pinRef->setPinRef(m_CompName,pinNumber,refDesArray);
   }
}

/******************************************************************************
* CCompToPinRefListMap
*/
CCompToPinRefListMap::CCompToPinRefListMap(CCEtoODBDoc *doc, int sys_pageunit)
: m_Doc(doc)
, m_SysPageUnit(sys_pageunit)
{
}

CCompToPinRefListMap::~CCompToPinRefListMap()
{
   this->removeAllNodes();
}

void CCompToPinRefListMap::removeAllNodes()
{
   POSITION pos = this->GetStartPosition();
   while(pos)
   {
      CString key;
      CPinRefListMap *pinlistref = NULL;
      GetNextAssoc(pos,key,pinlistref);

      if(pinlistref)
         delete pinlistref;
   }

   this->RemoveAll();
}

void CCompToPinRefListMap::AddPinRefListAt(CString CompName, CPinRefListMap *&pinRefList)
{
   if(!this->Lookup(CompName,pinRefList) || !pinRefList)
   {
      pinRefList = new CPinRefListMap(CompName);
      SetAt(CompName,pinRefList);
   }
}

BlockStruct *CCompToPinRefListMap::FindComponentGeometry(FileStruct *pcbFile, CString compName, DataStruct* &component)
{   
   //Look up component and its geometry by component name
   BlockStruct *pcbBlock = pcbFile->getBlock();
   
   for (CDataListIterator insertIterator(*pcbBlock,dataTypeInsert);insertIterator.hasNext();)
   {            
      DataStruct* data = insertIterator.getNext();
      if (data && data->isInsertType(insertTypePcbComponent))
      {
         InsertStruct *insert = data->getInsert();
         if(insert && !insert->getRefname().Compare(compName))
         {
            component = data;
            return m_Doc->getBlockAt(insert->getBlockNumber());
         }
      }/*if*/ 
   }/*for*/

   return NULL;
}

BlockStruct *CCompToPinRefListMap::CloneComponentGeometry(BlockStruct *componentGeometry, CString compName)
{
   //Clone component geometry
   CString newblockName = componentGeometry->getName()+ "_" + compName;
   BlockStruct *newComponentGeometry = Graph_Block_Exists(m_Doc, newblockName, componentGeometry->getFileNumber(), blockTypePcbComponent);
   
   if(!newComponentGeometry)
   {
      newComponentGeometry = Graph_Block_On(GBO_APPEND, newblockName, componentGeometry->getFileNumber(), 0L, componentGeometry->getBlockType());
      Graph_Block_Copy(componentGeometry, 0.0, 0.0, 0.0, 0, 1, -1, TRUE, FALSE);
      Graph_Block_Off();
   }

   return newComponentGeometry;
}

bool CCompToPinRefListMap::UpdatePinRefToAttributes(DataStruct *newPinRefdata, CPinRef* pinref)
{   
   int keywordIndex = m_Doc->RegisterKeyWord(ATT_REFNAME, 0, valueTypeString);
   int penWidthIndex = m_Doc->getDefinedWidthIndex(pinref->getPen());
   double rotation = pinref->getRotationRadians();

   if(pinref->getRotationType() == Rotation_RightReading)
      rotation = pinref->getAdjustedAngleRadians(m_compAngle, newPinRefdata->getInsert()->getAngleDegrees());

   //If no attribute for refname, create one
   CAttribute *attr = newPinRefdata->getAttributesRef()->lookupAttribute(keywordIndex); 
   if(attr)
   {
      DbFlag dbFlag = attr->getFlags();
      bool neverMirrorFlag = attr->getMirrorDisabled();
      bool visibleFlag = (pinref->isVisible())?attr->isVisible():false;
      bool proportionalFlag = attr->isProportionallySpaced();      

      int layerIndex = -1;
      if(pinref->getLayerName().IsEmpty())
         layerIndex = attr->getLayerIndex();
      else
         layerIndex = Graph_Level(pinref->getLayerName(), "", false);
         
      //update pin refname attribute   
      m_Doc->SetVisAttrib(&newPinRefdata->getAttributesRef(),keywordIndex,valueTypeString,(void*)((const char*)pinref->getUserdefString()),
         pinref->getX(),pinref->getY(),rotation,
         pinref->getHeight(),pinref->getWidth(),proportionalFlag,penWidthIndex,visibleFlag,SA_OVERWRITE,dbFlag,
         layerIndex,neverMirrorFlag,pinref->getHorizontalPosition(),pinref->getVerticalPosition());      
      
      return true;
   }

   return false;
}

bool CCompToPinRefListMap::LookupPinRef(CString compName, CString pinNumber, CPinRef *&pinRef)
{
   CPinRefListMap *pinrefList = NULL;
   if(this->Lookup(compName,pinrefList) && pinrefList)
   {
      if(pinrefList->Lookup(pinNumber,pinRef) && pinRef)
         return true;
   }

   return false;
}

void CCompToPinRefListMap::AddPinRefs(CString compName, CString pinNumber, CPinRef *pinRef, CStringArray &refDesArray)
{
   CPinRefListMap *pinrefList = NULL;
   AddPinRefListAt(compName,pinrefList);
   
   if(pinrefList)
      pinrefList->AddPinRefAt(pinNumber,m_SysPageUnit,pinRef,refDesArray);
}

void CCompToPinRefListMap::UpdatePinRefAttributeToCAMCAD(FileStruct *pcbFile,CStringArray& logmessage)
{
   //No PINREFF
   if(IsEmpty())
      return;

   CString ErrMessage;

   POSITION pos = GetStartPosition();
   while(pos)
   {
      //Search Component Geometry by Component Name
      CString compName = "";
      CPinRefListMap *pinrefList = NULL;
      this->GetNextAssoc(pos,compName,pinrefList);      

      if(pinrefList && !compName.IsEmpty())
      {
         DataStruct *component;
         BlockStruct *componentGeometry = FindComponentGeometry(pcbFile, compName, component);
         if(componentGeometry)
         {
            //Create a new Component Geometry with component name  
            BlockStruct *newComponentGeometry = CloneComponentGeometry(componentGeometry,compName);
            if(newComponentGeometry)
            {
               // Instance insert references new Component Geometry  
               component->getInsert()->setBlockNumber(newComponentGeometry->getBlockNumber());
               m_compAngle = component->getInsert()->getAngleDegrees();

               //Search pin refname by pin number
               POSITION pinrefPos = pinrefList->GetStartPosition();
               while(pinrefPos)
               {
                  CString PinNumber = "";
                  CPinRef *pinref = NULL;
                  pinrefList->GetNextAssoc(pinrefPos,PinNumber,pinref);

                  if(pinref && !PinNumber.IsEmpty())
                  {
                     //Update pin refname position
                     DataStruct *newPinRefdata = newComponentGeometry->FindData(PinNumber);                           
                     if(!newPinRefdata || !UpdatePinRefToAttributes(newPinRefdata,pinref))
                     {
                        ErrMessage.Format("Refname of Pin %s-%s does not exist.",compName,PinNumber);
                        logmessage.Add(ErrMessage);
                     }/*if*/
                  }/*if*/                     
               }/*while*/
            }
            else
            {
               ErrMessage.Format("Geometry of component %s does not exist.",compName);
               logmessage.Add(ErrMessage);
            }/*if*/  
         }/*if*/                     
      }/*if*/
   }/*while*/
}
