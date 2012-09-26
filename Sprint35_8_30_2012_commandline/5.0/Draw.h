// $Header: /CAMCAD/4.5/Draw.h 15    11/10/05 3:29p Kurt Van Ness $
                                                           
/*****************************************************************************/
/*           
    Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-98. All Rights Reserved.
*/

/* 

- Special character Draw:
    %O = Overline per char
    %U = Underline per char
    %% = percent char
       
- All datas have a layer
- "-1" is only legal data->getLayerIndex() for block inserts
- any and several layers may be floating (IsFloatingLayer())

- An entity on a floating layer takes the insert layer if there is one
  if (IsFloatingLayer(data->getLayerIndex()) && insertLayer != -1)
      layer = insertLayer;
  else
      layer = data->getLayerIndex();

- Block Aperture or Block Tool are drawn on same layer as first data entity of block

*/

#include "ccview.h"
#include "ccdoc.h"

void DrawDirectionArrow(CCEtoODBDoc *doc, CPoly* poly, BOOL Show, double insert_x, double insert_y, double scale, Mat2x2 *m, int mirror);

// DRAW_ENT.CPP
void InitPens(CDC *pDC, int fillStyle);
void FreePens(CDC *pDC);

// DRAW_MSC.CPP
BOOL NeedToDrawBlock(SettingsStruct *Settings, BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror, 
      CDC *pDC, double scaleFactor);

void TransformInsertExtents(double *xmin, double *xmax, double *ymin, double *ymax,
      BlockStruct *block, double insert_x, double insert_y, double scale, double rotation, int mirror);

// returns true if a nonplaced pinloc was found.
int DrawPinNumsNets(CDC *pDC, CCEtoODBView *view);

bool IsEntityVisible(DataStruct *data, BlockStruct** subblock, CCEtoODBDoc *doc, int mirror, int *layer, BOOL IgnoreClass);

void DrawArrow(CDC *pDC, double x1, double y1, double x2, double y2, double scaleFactor, double arrowsize, COLORREF color);

void DrawShadeRect(CDC *pDC,const ColorStruct *color, BlockStruct *block, double scaleFactor,
                   double insert_x, double insert_y, double scale, double rotation, int mirror);


