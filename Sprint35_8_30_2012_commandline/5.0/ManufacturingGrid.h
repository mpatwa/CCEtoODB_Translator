// $Header: /CAMCAD/4.3/ManufacturingGrid.h 4     8/12/03 9:06p Kurt Van Ness $

/*
$History: ManufacturingGrid.h $
 * 
 * *****************  Version 4  *****************
 * User: Kurt Van Ness Date: 8/12/03    Time: 9:06p
 * Updated in $/CAMCAD/4.3
*/ 

#if !defined(__ManufacturingGrid_H__)
#define __ManufacturingGrid_H__

#include "CcDoc.h"
#include "RwLib.h"
#include "QfeLib.h"

//_____________________________________________________________________________
class CManufacturingGrid : public CObject
{
private:
   CPoint2d m_origin;
   int m_xSteps;
   int m_ySteps;
   double m_xStepSize;
   double m_yStepSize;
   CExtent m_boardExtent;

public:
   CManufacturingGrid();
   CManufacturingGrid(int xSteps,double xStepSize,int ySteps,double yStepSize);
   ~CManufacturingGrid();

   void setOrigin(const CPoint2d& origin);
   void set(const CString& definitionString);
   CString getAttributeName() { return CString("ManufacturingGridDefinition"); }
   CString getDefinitionString();
   CString alphaDesignator(int number);
   CString getXGridCoordinate(double x);
   CString getYGridCoordinate(double y);
   CString getGridCoordinate(const CPoint2d& coordinate);
   CPoint2d getGridSpaceCenter(const CString& gridCoordinate);
   void regenerateGrid(CCEtoODBDoc& camCadDoc,FileStruct& pcbFile,CExtent extent,double gridLineWidth);

private:
   void getIndexCoordinates(int& x,int& y,const CString& gridCoordinate);
};

#endif


