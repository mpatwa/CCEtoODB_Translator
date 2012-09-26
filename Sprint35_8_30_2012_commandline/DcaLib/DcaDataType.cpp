// $Header: /CAMCAD/DcaLib/DcaDataType.cpp 4     3/09/07 5:15p Kurt Van Ness $

#include "StdAfx.h"
#include "DcaDataType.h"

//_____________________________________________________________________________
CString dataTypeTagToString(DataTypeTag tagValue)
{
   CString retval;

   switch (tagValue)
   {
   case dataTypeDraw:       retval = "Draw";       break;
   case dataTypeInsert:     retval = "Insert";     break;
   case dataTypePoly:       retval = "Poly";       break;
   case dataTypeText:       retval = "Text";       break;
   case dataTypeTool:       retval = "Tool";       break;
   case dataTypePoint:      retval = "Point";      break;
   case dataTypeBlob:       retval = "Blob";       break;
   case dataTypeUndefined:  retval = "Undefined";  break;
   }

   return retval;
}

//_____________________________________________________________________________
DataTypeTag intToDataTypeTag(int tagValue)
{
   DataTypeTag retval;

   switch (tagValue)
   {
   case dataTypeDraw:      retval = dataTypeDraw;       break;
   case dataTypeInsert:    retval = dataTypeInsert;     break;
   case dataTypePoly:      retval = dataTypePoly;       break;
   case dataTypeText:      retval = dataTypeText;       break;
   case dataTypeTool:      retval = dataTypeTool;       break;
   case dataTypePoint:     retval = dataTypePoint;      break;
   case dataTypeBlob:      retval = dataTypeBlob;       break;
   default:                retval = dataTypeUndefined;  break;
   }

   return retval;
}

//_____________________________________________________________________________
CString dataStructTypeToString(int dataStructType)
{
   CString retval;

   switch (dataStructType)
   {
   case dataTypeDraw:    retval = "dataTypeDraw";     break;
   case dataTypeInsert:  retval = "dataTypeInsert";   break;
   case dataTypePoly:    retval = "dataTypePoly";     break;
   case dataTypeText:    retval = "dataTypeText";     break;
   case dataTypeTool:    retval = "dataTypeTool";     break;
   case dataTypePoint:   retval = "dataTypePoint";    break;
   case dataTypeBlob:    retval = "dataTypeBlob";     break;
   default:              retval = "Undefined";        break;
   }

   return retval;
}
