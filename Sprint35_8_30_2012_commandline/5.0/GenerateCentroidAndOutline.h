// $Header: /CAMCAD/4.5/GenerateCentroidAndOutline.h 37    12/15/06 5:22p Rick Faltersack $

/////////////////////////////////////////////////////////////////////////////
// GenerateCentroidDlg dialog

#if !defined(__GenerateCentroidAndOutline_H__)
#define __GenerateCentroidAndOutline_H__

// 
// Source file name is misleading, Generate Centroids is not here anymore.
// It has moved in with the Data Doctor.
// Other non-GenCentroids stuff that was here is still here.
//

#include "flexgrid.h"
#include "afxwin.h"
#include "ResizingDialog.h"
#include "FlexGridLib.h"

#define DFT_OUTLINE_TOP						"DFT_OUTLINE_TOP"
#define DFT_OUTLINE_BOTTOM					"DFT_OUTLINE_BOTTOM"




//#define ALG_NAME_PIN_CENTER				"Pin Center"			
//#define ALG_NAME_BODY_PIN_EXTENTS		"Pin Extents"
//#define ALG_NAME_INSIDE_PADS				"Inside Pads"
//#define ALG_NAME_BODY_OUTLINE				"Body Outline"
//#define ALG_NAME_UNKNOWN					"Unknown"


/////////////////////////////////////////////////////////////////////////////

enum OutlineAlgorithmTag
{
   algUnknown         = -1,  // not a choice offered in menus, the rest are
   algBodyOutline     =  0,
   algPinCenters      =  1,
   algPinExtents      =  2,
   algInsidePads      =  3,
   algCustom          =  4,
   algUserXY          =  5,
   algPinBodyExtents  =  6
};

OutlineAlgorithmTag stringToOutlineAlgTag(CString str);
CString             outlineAlgTagToString(OutlineAlgorithmTag tag);

/////////////////////////////////////////////////////////////////////////////

DataStruct *GetPackageOutline(CDataList *DataList);
bool        HasPackageOutline(CDataList *DataList);
void        DeletePackageOutline(CCEtoODBDoc *doc, CDataList *DataList);

int GeneratePackageOutlineGeometry(CCEtoODBDoc *doc, BlockStruct *referenceBlk, BlockStruct *destinationBlk, OutlineAlgorithmTag algorithm, int rectangle, bool filled, int overwrite);
int DFT_OutlineAddPins(CCEtoODBDoc *doc, CDataList *DataList, double insert_x, double insert_y,
      double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);
int DFT_OutlineAddPins(CCEtoODBDoc& doc, CDataList& DataList, CInsertTypeMask includeInsertTypeMask, 
      double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel, int insertLayer);



#endif
