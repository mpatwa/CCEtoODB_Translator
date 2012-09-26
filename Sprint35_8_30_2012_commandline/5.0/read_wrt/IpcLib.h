// $Header: /CAMCAD/4.3/read_wrt/IpcLib.h 7     8/12/03 9:06p Kurt Van Ness $

/*******************************************************************************
*
*/

#include "geomlib.h"
#include "format_s.h"

// see also FORMAT_S.H
#define  IPC350            0
#define  IPC356            1
#define  IPC356A           2
#define  IPC_LINE_LENGTH   80

struct IPCOpcode
{
   int IPCTYPE;
   char ipcline[IPC_LINE_LENGTH+1];
   long DataContStartInd;
   long DataContEndInd;
};

struct ContData
{
   char ipcline[IPC_LINE_LENGTH+1];
};

struct IPCLine
{
   int IPCTYPE;
   BOOL opcode;
   char ipcline[IPC_LINE_LENGTH+1];
};
typedef CTypedPtrList<CPtrList, IPCLine*> CIPCLineList;

void wipc_aperturedef(int typ,int dcode, double sizeA, double sizeB, double offx, double offy, double rot); 
int  wipc_Graph_File_Open(const char *out_file, int linefeed, const char *dimchar, int ipctype, int ipcunits);
void wipc_Graph_File_Close();
void wipc_Flash_App(Point2 point, int app_num, int shape, int layer, double sizeA);
void wipc_Line(Point2 start, Point2 end, int app_num,
                           int apshape, double width);
void wipc_Circle(Point2 center, double radius,double width, int filled);
void wipc_write2file(const char *prosa);
void wipc_Graph_Polyline( Point2 *coordinates, int pairs, int dcode,
                     int apshape, double width,int filled);
void wipc_Graph_Layer(int l);
int  wipc_Graph_Text(const char *text, double x1, double y1,
                double height, double width, double char_linewidth,
                double angle, 
                     int mirror, int oblique);
void wipc_write_startcomplex(int dcode);
void wipc_write_endcomplex();
void wipc_negativelayer(int cond);
void wipc_positivelayer(int cond);

int  wipc_putchar_ipc(int,const char *);
int  wipc_write_ipcline();
int  wipc_clean_ipcline();
void wipc_set_opcode(bool flag);

void Quicksort(IPCOpcode OpcodeLine[], int first, int last);
void ChoosePivot(IPCOpcode OpcodeLine[], int first, int last);
void Partition(IPCOpcode OpcodeLine[], int first, int last, int &PivotIndex);
void Swap(IPCOpcode &A, IPCOpcode &B);
void SeparateOpcodeFromOthers(IPCOpcode *opcode, ContData *contData);
void DoOpcode();
void Writer_Footer();

// end IPCLIB.H
