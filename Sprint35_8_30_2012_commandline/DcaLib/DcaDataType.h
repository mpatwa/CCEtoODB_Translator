// $Header: /CAMCAD/DcaLib/DcaDataType.h 4     3/09/07 5:15p Kurt Van Ness $

#if !defined(__DcaDataType_h__)
#define __DcaDataType_h__

#pragma once

//_____________________________________________________________________________
enum DataTypeTag
{
   dataTypeDraw       =  0,  // T_DRAW
   dataTypeInsert     =  1,  // T_INSERT
   dataTypePoly       =  2,  // T_POLY
   dataTypeText       =  3,  // T_TEXT
   dataTypeTool       =  4,  // T_TOOL
   dataTypePoint      =  5,  // T_POINT
   dataTypeBlob       =  6,  // T_BLOB
   dataTypePoly3D     =  7,  // T_POLY3D
   dataTypeUndefined  = -1,
   DataTypeTagMin     =  0,  // \used to iterate over valid values by EnumIterator
   DataTypeTagMax     =  7,  // /
};

CString dataTypeTagToString(DataTypeTag tagValue);
CString dataStructTypeToString(int dataStructType);
DataTypeTag intToDataTypeTag(int tagValue);

#endif
