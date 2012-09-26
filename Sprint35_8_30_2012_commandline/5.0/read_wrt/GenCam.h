// $Header: /CAMCAD/4.3/read_wrt/GenCam.h 6     8/12/03 9:06p Kurt Van Ness $

/**********************************************************************
*
*/

#include "stdio.h"
#include "geomlib.h"

void wGENCAM_Graph_Layer(FILE *fp, const char *l);
void wGENCAM_Graph_MaskLevel(FILE *fp, const char *l);
void wGENCAM_Graph_Artwork(FILE *fp, const char *l, long artworkcnt);
void wGENCAM_Graph_Artwork1(FILE *fp, const char *l, long artworkcnt);

int  wGENCAM_Graph_Init_Layout(int output_units_accuracy);
void wGENCAM_Graph_File_Open_Layout();

void wGENCAM_Graph_File_Close_Layout();

void wGENCAM_Graph_Header(FILE *fp, char *GENCAM, double Version, const char *filename);

void wGENCAM_Graph_Aperture(FILE *fp, double x, double y, double sizea, double sizeb, 
                            double xoffset, double yoffset, int form, double rot);
void wGENCAM_Graph_ComplexAperture(FILE *fp, double x, double y, double sizea, double sizeb, 
                            double xoffset, double yoffset, int form, double rot);

void wGENCAM_Graph_Circle(FILE *fp, double center_x, double center_y, double radius);

void wGENCAM_Graph_Arc(const char *ident,
                        FILE *fp,  double center_x, double center_y, double radius,
                        double startangle, double deltaangle);

void wGENCAM_Graph_Line(const char *ident,
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2);
void wGENCAM_Graph_Rectangle(FILE *fp, double x1, double y1, double x2, double y2);

void wGENCAM_Graph_Text(const char *ident, FILE *fp, char *text, double x1, double y1,
      double height, double width, double angle, int mirror );

void wGENCAM_Graph_StartAt(const char *ident, FILE *fp, double x1, double y1);

void wGENCAM_Graph_NextAt(const char *ident, 
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2);

void wGENCAM_Graph_EndAt(const char *ident, 
                        FILE *fp, double x1, double y1, double bulge1, double x2, double y2, double bulge2);

/**********************************************************************
*  GenCamPadRotStruct
*/
typedef struct
{
   CString  padname;
   CString  newpadname;
   double   rotation;   // rotation in degree
} GenCamPadRotStruct;

class CGenCamPadRotArray : public CTypedArrayContainer<CPtrArray, GenCamPadRotStruct*>
{
public:
   CGenCamPadRotArray(){};
   ~CGenCamPadRotArray();

   bool Lookup(CString padname, double rotation);
   void Set(CString padname, CString newpadname, double   rotation);
   GenCamPadRotStruct* Get(int idx, CString padname);
};

/**********************************************************************
*  CGenCamPadstack
*/
class CGenCamPadstack
{
public:
	CGenCamPadstack(BlockStruct* block)
	{		
		m_pBlock = block;
		m_sName = block->getName();
		m_bIsPadstack = (block->getBlockType() == blockTypePadstack)?true:false;
      m_bIsbondPadstack = (block->getBlockType() == blockTypeBondPad)?true:false;
      m_insertLayer = -1;
	}
	~CGenCamPadstack()
	{
		m_pBlock = NULL;
	}

private:
	CString m_sName;
	bool m_bIsPadstack;			// Indicate if a real padstack or BL_TOOL because GENCAD output both as padstack with some syntax difference
   bool m_bIsbondPadstack;	   // Indicate if a real bond padstack or BL_TOOL because GENCAD output both as padstack with some syntax difference
	BlockStruct* m_pBlock;		// Pointer to the block
   int  m_insertLayer;        // layer in the insert of block

public:
	CString GetName() const									{ return m_sName;							}
	bool GetIsPadstack() const								{ return m_bIsPadstack;					}
	bool GetIsbondPadstack() const					   { return m_bIsbondPadstack;			}
	BlockStruct* GetBlock() const							{ return m_pBlock;						}
   int  GetInsertLayer() const					      { return m_insertLayer;					}
   void  SetInsertLayer(int layer)				      { m_insertLayer = layer;					}
};

class CGenCamPadstackMap : public CTypedMapStringToPtrContainer<CGenCamPadstack*>
{
public:
	CGenCamPadstackMap()	{}
	~CGenCamPadstackMap()	{ this->empty();	}
};
// end GENCAM.H
