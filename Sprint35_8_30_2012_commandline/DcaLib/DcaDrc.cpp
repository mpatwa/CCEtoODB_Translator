// $Header: /CAMCAD/DcaLib/DcaDrc.cpp 4     6/30/07 2:58a Kurt Van Ness $

#include "StdAfx.h"
#include "DcaDrc.h"
#include "DcaAttributes.h"
#include "DcaCamCadData.h"

//_____________________________________________________________________________
DRCStruct::DRCStruct(CCamCadData& camCadData,const CString& string,int entityNumber)
: m_camCadData(camCadData)
, m_entityNumber(entityNumber)
, m_string(string)
, m_drcClass(0)
, m_voidPtr(NULL)
, m_priority(0)
, m_reviewed(0)
, m_failureRange(0)
, m_algorithmIndex(0)
, m_algorithmType(0)
, m_insertEntityNumber(0)
, m_attributes(NULL)
{
   if (m_entityNumber < 0)
   {
      m_entityNumber = m_camCadData.allocateEntityNumber();
   }
}

DRCStruct::~DRCStruct()
{
   delete m_attributes;
}

void DRCStruct::copyData(const DRCStruct& other)
{
   setString(other.getString());
   setOrigin(other.getOrigin());
   setDrcClass(other.getDrcClass());
   setVoidPtr(other.getVoidPtr());
   setPriority(other.getPriority());
   setReviewed(other.getReviewed());
   setFailureRange(other.getFailureRange());
   setComment(other.getComment());
   setAlgorithmIndex(other.getAlgorithmIndex());
   setAlgorithmType(other.getAlgorithmType());
   setInsertEntityNumber(other.getInsertEntityNumber());

   if (other.getAttributes() != NULL)
   {
      attributes().CopyAll(*(other.getAttributes()));
   }
}

// accessors
long DRCStruct::getEntityNumber() const
{
   return m_entityNumber;
}

//void DRCStruct::setEntityNumber(long entityNumber)
//{
//   entity = entityNumber;
//}

CString DRCStruct::getString() const
{
   return m_string;
}

void DRCStruct::setString(const CString& drcString)
{
   m_string = drcString;
}

const CPoint2d& DRCStruct::getOrigin() const
{
   return m_origin;
}

void DRCStruct::setOrigin(const CPoint2d& origin)
{
   m_origin = origin;
}

void DRCStruct::setOriginX(double originX)
{
   m_origin.x = originX;
}

void DRCStruct::setOriginY(double originY)
{
   m_origin.y = originY;
}

int DRCStruct::getDrcClass() const
{
   return m_drcClass;
}

void DRCStruct::setDrcClass(int drcClass)
{
   m_drcClass = drcClass;
}

void* DRCStruct::getVoidPtr() const
{
   return m_voidPtr;
}

void DRCStruct::setVoidPtr(void* voidPtr)
{
   m_voidPtr = voidPtr;
}

int DRCStruct::getPriority() const
{
   return m_priority;
}

void DRCStruct::setPriority(int priority)
{
   m_priority = priority;
}

int DRCStruct::getReviewed() const
{
   return m_reviewed;
}

void DRCStruct::setReviewed(int reviewed)
{
   m_reviewed = reviewed;
}

int DRCStruct::getFailureRange() const
{
   return m_failureRange;
}

void DRCStruct::setFailureRange(int failureRange)
{
   m_failureRange = failureRange;
}

CString DRCStruct::getComment() const
{
   return m_comment;
}

void DRCStruct::setComment(const CString& comment)
{
   m_comment = comment;
}

int DRCStruct::getAlgorithmIndex() const
{
   return m_algorithmIndex;
}

void DRCStruct::setAlgorithmIndex(int index)
{
   m_algorithmIndex = index;
}

int DRCStruct::getAlgorithmType() const
{
   return m_algorithmType;
}

void DRCStruct::setAlgorithmType(int type)
{
   m_algorithmType = type;
}

int DRCStruct::getInsertEntityNumber() const
{
   return m_insertEntityNumber;
}

void DRCStruct::setInsertEntityNumber(int entityNumber)
{
   m_insertEntityNumber = entityNumber;
}

CAttributes*& DRCStruct::getAttributesRef()
{
   return m_attributes;
}

CAttributes* DRCStruct::getAttributes() const
{
   return m_attributes;
}

CAttributes& DRCStruct::attributes()
{
   if (m_attributes == NULL)
   {
      m_attributes = new CAttributes();
   }

   return *m_attributes;
}

//_____________________________________________________________________________
CDRCList::CDRCList(FileStruct& fileStruct)
: m_fileStruct(fileStruct)
{
}

void CDRCList::empty()
{
   m_drcList.empty();
}

CCamCadData& CDRCList::getCamCadData() const
{
   return m_fileStruct.getCamCadData();
}

DRCStruct* CDRCList::addDrc(const CString& string,int entityNumber)
{
   getCamCadData().allocateEntityNumber(entityNumber);

   DRCStruct* drcStruct = new DRCStruct(m_fileStruct.getCamCadData(),string,entityNumber);

   m_drcList.AddTail(drcStruct);

   return drcStruct;
}

void CDRCList::deleteAt(POSITION pos)
{
   DRCStruct* drc = m_drcList.GetAt(pos);
   m_drcList.RemoveAt(pos);

   if (drc != NULL)
   {
      delete drc;
   }
}

POSITION CDRCList::GetHeadPosition() const
{
   return m_drcList.GetHeadPosition();
}

DRCStruct* CDRCList::GetNext(POSITION& pos) const
{
   return m_drcList.GetNext(pos);
}

DRCStruct* CDRCList::GetAt(POSITION pos) const
{
   return m_drcList.GetAt(pos);
}

POSITION CDRCList::Find(DRCStruct* drc) const
{
   return m_drcList.Find(drc);
}

int CDRCList::GetCount() const
{
   return m_drcList.GetCount();
}

bool CDRCList::IsEmpty() const
{
   return (m_drcList.GetCount() == 0);
}

void CDRCList::takeData(CDRCList& otherList)
{
   for (POSITION pos = otherList.GetHeadPosition();pos != NULL;)
   {
      DRCStruct* drc = otherList.GetNext(pos);

      DRCStruct* newDrc = addDrc(drc->getString(),drc->getEntityNumber());

      newDrc->copyData(*drc);
   }
}

////////////////////////////////////////////////////////////////////////////////////////

void DRCStruct::RebuildDrcString(int decimals)
{
   // For use after DRC List has been scaled, rebuilds the drc "string" for those 
   // that have linear values in the string that may have changed due to scaling (i.e. units change).

   if (this->getAttributesRef() != NULL)
   {
      double actualValue = 0.0;
      double checkValue = 0.0;

      Attrib *attr = NULL;

      if ((attr = this->getAttributesRef()->lookupAttribute(standardAttributeDfmActualValue)) != NULL)
         if (attr->getValueType() == valueTypeDouble || attr->getValueType() == valueTypeUnitDouble)
            actualValue = attr->getDoubleValue();

      if ((attr = this->getAttributesRef()->lookupAttribute(standardAttributeDfmCheckValue)) != NULL)
         if (attr->getValueType() == valueTypeDouble || attr->getValueType() == valueTypeUnitDouble)
            checkValue = attr->getDoubleValue();

      if (this->getDrcClass() == DRC_CLASS_MEASURE)
      {       
         CString ref1;
         CString ref2;

         if ((attr = this->getAttributesRef()->lookupAttribute(standardAttributeDfmRef1)) != NULL)
            if (attr->getValueType() == valueTypeString)
               ref1 = attr->getStringValue();

         if ((attr = this->getAttributesRef()->lookupAttribute(standardAttributeDfmRef2)) != NULL)
            if (attr->getValueType() == valueTypeString)
               ref2 = attr->getStringValue();

         CString string;
         if (!ref1.IsEmpty() && !ref2.IsEmpty())
         {
            string += ref1;
            string += "<->";
            string += ref2;
         }
         else if (!ref1.IsEmpty())
         {
            string += ref1;
         }
         else if (!ref2.IsEmpty())
         {
            string += ref2;
         }

         CString buf;
         buf.Format(" => %.*lf ", decimals, actualValue);
         string += buf;

         this->setString(string);
      }
      else
      {
         // Check for relative measurement. The flags for class and type are insufficient to 
         // determine detail content. Checking names directly is problematic,names may change
         // elsewhere or new ones added. So here we check the form, if it looks like a
         // relative measurement then update the actual and check values.

         CSupString supstring( this->getString() );
         CStringArray fields;
         supstring.ParseWhite(fields);

         // The form we are looking for is "xxxxx actual < check" (or >) so that means there has
         // to be at least four fields, and the 2nd to last has to be ">" or "<".
         if (fields.GetCount() > 3)
         {
            CString secondToLast( fields.GetAt( fields.GetCount() - 2 ) );
            if (secondToLast.Compare("<") == 0 || secondToLast.Compare(">") == 0)
            {
               CString string;
               CString actualStr;
               CString checkStr;
               actualStr.Format("%.*f", decimals, actualValue);
               checkStr.Format("%.*f", decimals, checkValue);

               for (int i = 0; i < fields.GetCount(); i++)
               {
                  if (!string.IsEmpty())
                     string += " ";

                  if (i == fields.GetCount() - 3) // replace old first value
                     string += actualStr;
                  else if (i == fields.GetCount() - 1) // replace old second value
                     string += checkStr;
                  else  // keep what ever was there
                     string += fields.GetAt(i);
               }

               this->setString(string);
            }
         }
      }
   }
}

void CDRCList::Scale(double factor, int decimals)
{
   for (POSITION drcPos = this->GetHeadPosition(); drcPos != NULL;)
   {
      DRCStruct* drc = this->GetNext(drcPos);

      // DRC xy
      CPoint2d location( drc->getOrigin() );
      drc->setOriginX( location.x * factor );
      drc->setOriginY( location.y * factor );

      // Scale "actual" and "check" value attributes, except not for tlass "NO MARKER".
      // Class "NO MARKER" tends to have counts stored in these.
      if (drc->getAttributesRef() != NULL && drc->getDrcClass() != DRC_CLASS_NO_MARKER)
      {
         // The attrib values are supposed to be double or dbunits, if they aren't then skip them
         Attrib *attr = NULL;

         if ((attr = (drc->getAttributesRef()->lookupAttribute(standardAttributeDfmActualValue))) != NULL)
         {
            if (attr->getValueType() == valueTypeDouble || attr->getValueType() == valueTypeUnitDouble)
            {
               double val = attr->getDoubleValue();
               val *= factor;
               drc->getAttributesRef()->setAttribute(getCamCadData(), standardAttributeDfmActualValue, val, attributeUpdateOverwrite);
            }
         }

         if ((attr = (drc->getAttributesRef()->lookupAttribute(standardAttributeDfmCheckValue))) != NULL)
         {
            if (attr->getValueType() == valueTypeDouble || attr->getValueType() == valueTypeUnitDouble)
            {
               double val = attr->getDoubleValue();
               val *= factor;
               drc->getAttributesRef()->setAttribute(getCamCadData(), standardAttributeDfmCheckValue, val, attributeUpdateOverwrite);
            }
         }
      }


      // Update whatever the voidPtr points to
      switch (drc->getDrcClass())
      {
      case DRC_CLASS_SIMPLE:     // voidPtr is (supposed to be) NULL
      case DRC_CLASS_NO_MARKER:
         // No op
         break;

      case DRC_CLASS_MEASURE:    // voidPtr is DRC_MeasureStruct*
         if (drc->getVoidPtr() != NULL)
         {
            DRC_MeasureStruct* meas = (DRC_MeasureStruct*)drc->getVoidPtr();
            meas->x1 *= (DbUnit)factor;
            meas->y1 *= (DbUnit)factor;
            meas->x2 *= (DbUnit)factor;
            meas->y2 *= (DbUnit)factor;
         }
         break;

      case DRC_CLASS_NETS:       // voidPtr is CStringList
         // No op
         break;
      }

      // The DRS String may contain some data values, so it is nice to adjust it, though it makes
      // no real difference in DRC List operation. The strings are formed in various ways.
      // Do this last, as it uses the data within the DRC to rebuild the string.
      drc->RebuildDrcString(decimals);
   }
}
