
/****************************************************************************/
/* 
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2002. All Rights Reserved.
*/

#include "stdafx.h"
#include "ccdoc.h"
#include "ipclib.h"
#include "font.h"
#include <math.h>
#include "lyr_file.h"
#include "gauge.h"
#include "attrib.h"
#include "pcbutil.h"
#include "dbutil.h"
#include "graph.h"
#include "ck.h"
#include "polylib.h"
#include "xform.h"
#include <math.h>
#include <float.h>
#include "ipc_out.h"
#include "CCEtoODB.h"
#include "RwUiLib.h"

extern CProgressDlg *progress;
extern CFileLayerList *FileLayerList;

static   IPCViaArray    ipcViaArray;   
static   int            ipcViaCnt;

static CIPCPadstackMap     padstackMap;

static   CCEtoODBDoc        *doc;
static   FileLayerStruct   *FileLayerPtr;

static   int               display_error;
static   FILE              *flog;
static   int               linefeed;
static   bool              ipc356_usePinInsertLoc, ipc356_unusedpins, ipc356_vias, ipc356_single_nc, ipc356_viarefdes;
static   CString           ipc356_unusednetname;
static   CString           ipc356_panel_netlist;
static   double            ARC_ANGLE_DEGREE;
static   int               IpcUnitsPerPageUnit;
static   int               ipc_units;

static   CIPCLayerArray mLayerArr;
static   int         maxArr = 0;
static   int         max_signallayer = 0;


static void IPC_WriteData(CDataList *DataList, double insert_x, double insert_y, double rotation, int mirror, double scale,
      int embeddedLevel, int insertlayer, double unitScale);

static void IPC356A_WriteData(CDataList *DataList, CString netPrefix, double insert_x, double insert_y, double rotation,
      int mirror, double scale, int embeddedLevel, int insertLayer, double unitScale); // unitScale only used for apertures because they are not scaled
static int load_ipcsettings(const CString fname);
static void do_layerlist();
static int do_netnamemap356a(CNetList *NetList, const char *netlist_prefix);
static void do_padstacks(double scale);
static void load_testvias(CDataList *DataList, double scale);
static int do_pinloc356A(CNetList *NetList, double scale, CDataList *DataList, int ipcvias, const char *netlist_prefix, 
      int *start_netcnt, double file_x, double file_y, double file_angle, int file_mirror);
static void free_layerlist();
static int do_pinloc(CNetList *NetList, double scale, CDataList *DataList, int ipcvias, const char *netlist_prefix, int *start_netcnt,
      double file_x, double file_y, double file_angle, int file_mirror);

static void IPC_GetPADSTACKData(CIPCPadstack& padstack, BlockStruct& padstackBlock, int insertLayer, double scale);
static void updateIPCPadstack(CIPCPadstack& padstack, DataStruct& data, int insertLayerIndex, double scale);
static int getIpcUnitsPerpageUnits(PageUnitsTag units);
static int getIpcUnitsAngle(double radians);

/******************************************************************************
* IPC_WriteFiles
*/
void IPC_WriteFiles(const char *filename, CCEtoODBDoc *Doc, FormatStruct *format)
{
   int ipc_type = format->IPCSettings.IPC_Format;
   ipc_units = format->IPCSettings.Units;
   double scale = Units_Factor(Doc->getSettings().getPageUnits(), format->IPCSettings.Units) * format->Scale;   // is in 1/10th of a mil
   doc = Doc;
   display_error = FALSE;
   padstackMap.empty();

   IpcUnitsPerPageUnit = getIpcUnitsPerpageUnits(intToPageUnitsTag(ipc_units));

   CString logFile = GetLogfilePath("ipc.log");
   if ((flog = fopen(logFile, "wt")) == NULL)   // rewrite filw
   {
      ErrorMessage("Can not open Logfile !", logFile, MB_ICONEXCLAMATION | MB_OK);
      return;
   }

   fprintf(flog, "Filename: %s\n", filename);
   CTime t;
   t = t.GetCurrentTime();
   fprintf(flog,"%s\n", t.Format("Created date :%A, %B %d, %Y at %H:%M:%S"));

   CString settingsFile( getApp().getExportSettingsFilePath("ipc.out") );
   load_ipcsettings(settingsFile);
   bool ipc_vias = ipc356_vias; // from settings

   CString checkFile( getApp().getSystemSettingsFilePath("ipc.chk") );
   check_init(checkFile);
                         
   // here ipc356
   // here ipc356a
   // here ipc350
   int res;
   switch (ipc_type)
   {
   case IPC356A:
      {
         if ((res = wipc_Graph_File_Open(filename,linefeed, "", ipc_type, ipc_units)) != 1)
         {
            // error occured
            // -1 = file can not open
            // 0  = memory failed
            // 1 = all ok
            if(res == -1)
            {
               // file error
               CString  tmp;
               tmp.Format("Can not open file [%s]", filename);
               ErrorMessage(tmp);
               return;
            }
            if (res == 0)
            MemErrorMessage(__FILE__, __LINE__);
         }
         bool dontShowProgressBar = ((CCEtoODBApp*)AfxGetApp())->UsingAutomation &&
                                !AfxGetMainWnd()->IsWindowVisible();
         if(dontShowProgressBar)
            progress->ShowWindow(SW_HIDE);
         progress->SetStatus(filename);
         
         ipcViaArray.SetSize(100,100);
         ipcViaCnt = 0;
         mLayerArr.SetSize(100, 100);
         maxArr = 0;
         max_signallayer = 0;

         do_layerlist();
         
         if (max_signallayer < 2)
            max_signallayer = 2;

         wipc_write2file("START DESIGN IPC356A");

         int pcb_found = FALSE;
         int panel_found = FALSE;
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos)
         {
            FileStruct *file = doc->getFileList().GetNext(pos);

            if (!file->isShown())
               continue;

            if (file->getBlockType() == BLOCKTYPE_PCB)  
            {
               generate_PINLOC(doc, file, 0);      // this function generates the PINLOC argument for all pins.
               generate_PADSTACKACCESSFLAG(doc, 0);   // 
               do_netnamemap356a(&file->getNetList(), "");
            
               padstackMap.empty();
               do_padstacks(scale);
               pcb_found = TRUE;

               load_testvias(&(file->getBlock()->getDataList()), scale);
               int startnetcnt = 0;
               do_pinloc356A(&file->getNetList(), scale, &file->getBlock()->getDataList(), ipc_vias, "", &startnetcnt, 0.0, 0.0, 0.0, 0);

               if (ipc_type == IPC356A)
                  IPC356A_WriteData(&file->getBlock()->getDataList(), "", 0.0, 0.0, 0.0, 0, scale, 0, -1, scale);

               if (!startnetcnt && !ipcViaCnt)
                  pcb_found = FALSE;
            }
            else if (file->getBlockType() == BLOCKTYPE_PANEL) 
            {
               // do all relative pinloc calc. for all pcb's
               POSITION pos2 = doc->getFileList().GetHeadPosition();
               while (pos2)
               {
                  FileStruct *file1 = doc->getFileList().GetNext(pos2);
                  generate_PINLOC(doc, file1, 0);  // this function generates the PINLOC argument for all pins.
               }
   
               generate_PADSTACKACCESSFLAG(doc, 1);
               padstackMap.empty();
               do_padstacks(scale);

               // prepare the netlist for a pcb file.
               // make generate_ncnet for every pcb file
               int start_netcnt = 0;
               POSITION pos1 = file->getBlock()->getDataList().GetHeadPosition();
               while (pos1)
               {
                  DataStruct *data = file->getBlock()->getDataList().GetNext(pos1);
                  if (data->getDataType() != T_INSERT)         
                     continue;

                  if (data->getInsert()->getInsertType() == INSERTTYPE_PCB) 
                  {
                     // find file to switch on
                     POSITION pos2 = doc->getFileList().GetHeadPosition();
                     while (pos2)
                     {
                        FileStruct *file1 = doc->getFileList().GetNext(pos2);
                        if (file1->getBlock()->getBlockNumber() == data->getInsert()->getBlockNumber())
                        {
                           pcb_found++;
                           CString tmp;
                           tmp.Format("START DESIGN %d", pcb_found);
                           wipc_write2file(tmp);
                           load_testvias(&(file1->getBlock()->getDataList()), scale);
                           
                           CString netprefix;
                           netprefix.Format("%s%d", ipc356_panel_netlist, pcb_found);

                           do_netnamemap356a(&file->getNetList(), netprefix);

                           do_pinloc356A(&file1->getNetList(), scale, &(file1->getBlock()->getDataList()), ipc_vias, netprefix, &start_netcnt,
                              data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags());

                           if (ipc_type == IPC356A)
                           {
                              IPC356A_WriteData(&(file->getBlock()->getDataList()), netprefix,
                                 data->getInsert()->getOriginX() * scale, data->getInsert()->getOriginY() * scale,
                                 data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), scale, 0, -1, scale);
                           }

                           tmp.Format("END DESIGN %d", pcb_found);
                           wipc_write2file(tmp);
            
                           if (!start_netcnt && !ipcViaCnt)
                              --pcb_found;
                        }
                     }
                  }
               }
            }
         }

         if (!pcb_found)
         {
            CString tmp = "No PCB file with Netlist information found!";
            tmp += "\nTo output a Gerber file in IPC356(A), you first need to derive";
            tmp += "\nPadstacks and Netlist in Tools|Gerber->PCB!";
   
            ErrorMessage( tmp, "IPC356(A) Output", MB_OK | MB_ICONHAND);
         }
         
         wipc_Graph_File_Close();
         free_layerlist();

         for (int i=0; i<ipcViaCnt; i++)
         {
            ipcViaArray[i]->m_data = NULL;
            delete ipcViaArray[i];
         }
         ipcViaArray.RemoveAll();
      }
      break;

   case IPC356:
      {
         ipcViaArray.SetSize(100, 100);
         ipcViaCnt = 0;

         if ((res = wipc_Graph_File_Open(filename,linefeed, "N", ipc_type, ipc_units)) != 1)
         {
            // error occured
            // -1 = file can not open
            // 0  = memory failed
            // 1 = all ok
            if(res == -1)
            {
               // file error
               CString tmp;
               tmp.Format("Can not open file [%s]", filename);
               ErrorMessage(tmp);
               return;
            }
            if (res == 0)
               MemErrorMessage(__FILE__, __LINE__);
         }
         bool dontShowProgressBar = ((CCEtoODBApp*)AfxGetApp())->UsingAutomation &&
                                !AfxGetMainWnd()->IsWindowVisible();
         if(dontShowProgressBar)
            progress->ShowWindow(SW_HIDE);
         progress->SetStatus(filename);

         int pcb_found = FALSE;
         int panel_found = FALSE;

         mLayerArr.SetSize(100, 100);
         maxArr = 0;
         max_signallayer = 0;
         do_layerlist();
         
         if (max_signallayer < 2)
            max_signallayer = 2;

         wipc_write2file("START DESIGN");
         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos)
         {
            FileStruct *file = doc->getFileList().GetNext(pos);

            if (!file->isShown())
               continue;

            if (file->getBlockType() == BLOCKTYPE_PCB)  
            {
               generate_PINLOC(doc, file, 0);      // this function generates the PINLOC argument for all pins.
               generate_PADSTACKACCESSFLAG(doc, 0);   // 

               padstackMap.empty();
               do_padstacks(scale);
               pcb_found = TRUE;

               load_testvias(&file->getBlock()->getDataList(), scale);

               int startnetcnt = 0;
               do_pinloc(&file->getNetList(), scale, &file->getBlock()->getDataList(), ipc_vias, "", &startnetcnt, 0.0, 0.0, 0.0, 0);

               if (ipc_type == IPC356A)
                  IPC356A_WriteData(&(file->getBlock()->getDataList()), "", 0.0, 0.0, 0.0, 0, scale, 0, -1, scale);

               if (!startnetcnt && !ipcViaCnt)
                  pcb_found = FALSE;
            }
            else if (file->getBlockType() == BLOCKTYPE_PANEL) 
            {
               padstackMap.empty();

               // do all relative pinloc calc. for all pcb's
               POSITION pos2 = doc->getFileList().GetHeadPosition();
               while (pos2)
               {
                  FileStruct *file1 = doc->getFileList().GetNext(pos2);
                  generate_PINLOC(doc, file1, 0);  // this function generates the PINLOC argument for all pins.
               }
   
               generate_PADSTACKACCESSFLAG(doc, 1);
               do_padstacks(scale);

               // prepare the netlist for a pcb file.
               // make generate_ncnet for every pcb file
               int start_netcnt = 0;
               POSITION pos1 = file->getBlock()->getDataList().GetHeadPosition();
               while (pos1)
               {
                  DataStruct *data = file->getBlock()->getDataList().GetNext(pos1);
                  if (data->getDataType() != T_INSERT)         
                     continue;

                  if (data->getInsert()->getInsertType() == INSERTTYPE_PCB) 
                  {
                     // find file to switch on
                     POSITION pos2 = doc->getFileList().GetHeadPosition();
                     while (pos2)
                     {
                        FileStruct *file1 = doc->getFileList().GetNext(pos2);
                        if (file1->getBlock()->getBlockNumber() == data->getInsert()->getBlockNumber())
                        {
                           pcb_found++;
                           CString tmp;
                           tmp.Format("START DESIGN %d", pcb_found);
                           wipc_write2file(tmp);
                           load_testvias(&(file1->getBlock()->getDataList()), scale);
                           
                           CString netprefix;
                           netprefix.Format("%s%d", ipc356_panel_netlist, pcb_found);

                           do_pinloc(&file1->getNetList(), scale, &(file1->getBlock()->getDataList()), ipc_vias, netprefix, &start_netcnt,
                              data->getInsert()->getOriginX(), data->getInsert()->getOriginY(), data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags());

                           if (ipc_type == IPC356A)
                           {
                              IPC356A_WriteData(&(file->getBlock()->getDataList()), netprefix, 
                                 data->getInsert()->getOriginX() * scale, data->getInsert()->getOriginY() * scale,
                                 data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), scale, 0, -1, scale);
                           }

                           tmp.Format("END DESIGN %d", pcb_found);
                           wipc_write2file(tmp);
            
                           if (!start_netcnt && !ipcViaCnt)
                              --pcb_found;
                        }
                     }
                  }
               }
            }
         }

         if (!pcb_found)
         {
            CString tmp = "No PCB file with Netlist information found!";
            tmp += "\nTo output a Gerber file in IPC356(A), you first need to derive";
            tmp += "\nPadstacks and Netlist in Tools|Gerber->PCB!";
   
            ErrorMessage( tmp, "IPC356(A) Output", MB_OK | MB_ICONHAND);
         }
         
         wipc_Graph_File_Close();
         free_layerlist();

         for (int i=0; i<ipcViaCnt; i++)
         {
            ipcViaArray[i]->m_data = NULL;
            delete ipcViaArray[i];
         }
         ipcViaArray.RemoveAll();
      }
      break;

   case IPC350:
      {
         if ((res = wipc_Graph_File_Open(filename,linefeed, "A", ipc_type, ipc_units)) != 1)
         {
            // error occured
            // -1 = file can not open
            // 0  = memory failed
            // 1 = all ok
            if(res == -1)
            {
               // file error
               CString  tmp;
               tmp.Format("Can not open file [%s]", filename);
               ErrorMessage(tmp);
               return;
            }
            if (res == 0)
            MemErrorMessage(__FILE__, __LINE__);
         }
         bool dontShowProgressBar = ((CCEtoODBApp*)AfxGetApp())->UsingAutomation &&
                                !AfxGetMainWnd()->IsWindowVisible();
         if(dontShowProgressBar)
            progress->ShowWindow(SW_HIDE);            

         progress->SetStatus(filename);

         // here write all apertures
         wipc_write2file("APERTURE DEFINITION SUBROUTINES");

         // first do AM - this are all non round or square app
         for (int i=0; i<doc->getMaxBlockIndex(); i++)
         {
            BlockStruct *block = doc->getBlockAt(i);
            if (block == NULL)
               continue;

            // create necessary aperture blocks
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               if (block->getShape() == T_COMPLEX)
               {
                  int dcode = block->getDcode();
                  block = doc->Find_Block_by_Num((int)(block->getSizeA()));
                  if (!block)
                     continue;
                  if (block->getDataList().IsEmpty())
                     continue;

                  char tmp[80];
                  sprintf(tmp, "COMLEX Definition [%s] D%d", block->getName(), block->getDcode()); 
                  wipc_write2file(tmp);
                  wipc_write_startcomplex(dcode);

                  int block_layer = -2;   // they must be written without a layer record
                  IPC_WriteData(&(block->getDataList()), block->getXoffset() * scale, block->getYoffset() * scale,
                     block->getRotationRadians(), 0, scale, 1, block_layer, scale);
                  wipc_write_endcomplex();
               }
               else
               {
                  double sizeA = block->getSizeA() * scale;
                  double sizeB = block->getSizeB() * scale; 
                  double offx = block->getXoffset() * scale;
                  double offy = block->getYoffset() * scale; 
                  wipc_aperturedef(block->getShape(), block->getDcode(), sizeA, sizeB, offx, offy, block->getRotationRadians());
               }
            }
         }
         wipc_write2file("");

         // need here list layerlist with names

         wipc_write2file("START DESIGN");

         POSITION pos = doc->getFileList().GetHeadPosition();
         while (pos)
         {
            FileStruct *file = doc->getFileList().GetNext(pos);

            if (!file->isShown())
               continue;

            if (file->getBlockType() == BLOCKTYPE_PCB)  
               IPC_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * scale, file->getInsertY() * scale, file->getRotation(), file->isMirrored(), scale, 0, -1, scale);
            else if (file->getBlockType() == BLOCKTYPE_PANEL) 
            {
               padstackMap.empty();

               // do all relative pinloc calc. for all pcb's
               POSITION pos2 = doc->getFileList().GetHeadPosition();
               while (pos2)
               {
                  FileStruct *file1 = doc->getFileList().GetNext(pos2);
                  generate_PINLOC(doc,file1,0); // this function generates the PINLOC argument for all pins.
               }
   
               generate_PADSTACKACCESSFLAG(doc, 1);
               do_padstacks(scale);

               // prepare the netlist for a pcb file.
               // make generate_ncnet for every pcb file
               int   start_netcnt = 0;
               int pcb_found = FALSE;

               POSITION pos1 = file->getBlock()->getDataList().GetHeadPosition();
               while (pos1)
               {
                  DataStruct *data = file->getBlock()->getDataList().GetNext(pos1);
                  if (data->getDataType() != T_INSERT)         
                     continue;

                  if (data->getInsert()->getInsertType() == INSERTTYPE_PCB) 
                  {
                     // find file to switch on
                     POSITION pos2 = doc->getFileList().GetHeadPosition();
                     while (pos2)
                     {
                        FileStruct *file1 = doc->getFileList().GetNext(pos2);
                        if (file1->getBlock()->getBlockNumber() == data->getInsert()->getBlockNumber())
                        {
                           pcb_found++;
                           CString  tmp;
                           tmp.Format("START DESIGN %d", pcb_found);
                           wipc_write2file(tmp);

                           IPC_WriteData(&(file->getBlock()->getDataList()), data->getInsert()->getOriginX() * scale,
                                 data->getInsert()->getOriginY() * scale, data->getInsert()->getAngle(), data->getInsert()->getMirrorFlags(), scale, 0, -1, scale);

                           tmp.Format("END DESIGN %d", pcb_found);
                           wipc_write2file(tmp);
                        }
                     }
                  }
               }
            }
            else
            {
               IPC_WriteData(&(file->getBlock()->getDataList()), file->getInsertX() * scale, file->getInsertY() * scale,
                     file->getRotation(), file->isMirrored(), scale, 0, -1, scale);
            }
         }
         wipc_Graph_File_Close();
      }
      break;
   }

   padstackMap.empty();

   if (check_report(flog))
      display_error++;
   check_deinit();

   t = t.GetCurrentTime();
   fprintf(flog,"%s\n", t.Format("Done :%A, %B %d, %Y at %H:%M:%S"));

   fprintf(flog, "Logfile closed\n");

   fclose(flog);

   if (display_error)
      Logreader(logFile);
}

/******************************************************************************
* GetLayerStackNum
*/
static int GetLayerStackNum(int layerNum)
{
   for (int i=0; i<maxArr; i++)
   {
      if (mLayerArr[i]->layerindex == layerNum)
         return mLayerArr[i]->stackNum;
   }

   return -99;
}

/******************************************************************************
* do_layerlist
*/
static void do_layerlist()
{
   LayerStruct *layer;
   int         signr = 0;
   char        typ = 'D';

   //#define   LAY_TOP           -1
   //#define   LAY_BOT           -2
   //#define   LAY_INNER         -3
   //#define   LAY_ALL           -4

   for (int j=0; j<doc->getMaxLayerIndex(); j++) // getMaxLayerIndex() is the exact number of layers, not the next index
   {
      layer = doc->getLayerArray()[j];
      if (layer == NULL)
         continue; // could have been deleted.

      signr = 0;
      typ = 'D';

      if (layer->getElectricalStackNumber())
      {
         typ = 'S';
         signr = layer->getElectricalStackNumber();
         if (signr > max_signallayer)
            max_signallayer = signr;
      }
      else if (layer->isElectricalAll())
      {
         signr = LAY_ALL;
      }
      else if (layer->isElectricalTop())
      {
         signr = LAY_TOP;
      }
      else if (layer->isElectricalInner())
      {
         signr = LAY_INNER;
      }
      else if (layer->isElectricalBottom())
      {
         signr = LAY_BOT;
      }

      if (signr == 0)
      {
         // do not allow electrical layers to be switched off.
         if (!doc->get_layer_visible(j, FALSE))
            continue;
      }

      // here check if layer->getName() is not already done

      //mlArr.SetSizes
      IPCLayerStruct *ml = new IPCLayerStruct;
      ml->stackNum = signr;
      ml->layerindex = j;
      ml->on = TRUE;
      ml->type = typ;
      ml->Name = layer->getName();
      mLayerArr.SetAtGrow(maxArr++, ml);
   }

   return;
}

/******************************************************************************
* free_layerlist
*/
static void free_layerlist()
{
   for (int i=0; i<maxArr; i++)
      delete mLayerArr[i];
   mLayerArr.RemoveAll();
   maxArr = 0;
}

/******************************************************************************
* ipcPadArea
*/
 double ipcPadPseudoSize(ApertureShapeTag shape, double sizeA, double sizeB)
 {
	 // Does not really determine size, but rather determines a marker for
	 // size. This is intended for use in comparison of two pads of same
	 // shape. It is meaningless to use for comparison of pads of different 
	 // shape. Ultimately it is the area that is to be compared, but we are
	 // avoiding the overhead of actually computing area if we can.
	 // E.g. to compare two ROUND pads, we need only compare the diameter.
	 // If the shape is not round or square, it is assumed to be rectangular,
	 // in which case we do compute the area.

	 if (shape == T_ROUND || shape == T_SQUARE)
	 {
		 return sizeA;  // diameter or length of one side
	 }
	
	 return sizeA * sizeB;
 }

/******************************************************************************
* IPC_GetPADSTACKData
      return 0x1 top
             0x2 bottom
             0x4 drill
*/
static void IPC_GetPADSTACKData(CIPCPadstack& padstack, BlockStruct& padstackBlock, int insertLayer, double scale)
{
   for (CDataListIterator dataList(padstackBlock, dataTypeInsert); dataList.hasNext();)
   {
      DataStruct* data = dataList.getNext();

      if (data->getInsert()->getInsertType() == INSERTTYPE_RELIEFPAD)
         continue;
      if (data->getInsert()->getInsertType() == INSERTTYPE_CLEARPAD)
         continue;
      if (data->getInsert()->getInsertType() == INSERTTYPE_OBSTACLE)
         continue;
      if (data->getInsert()->getInsertType() == INSERTTYPE_DRILLSYMBOL)
         continue;

      if (data->getInsert()->getInsertType() == INSERTTYPE_PCBCOMPONENT)
			break;
		if (data->getInsert()->getInsertType() == INSERTTYPE_VIA)
			break;

      updateIPCPadstack(padstack, *data, insertLayer, scale);
   }
} 

//------------------------------------------------------------------------

static void GetPadDimensions(BlockStruct *block, DataStruct &data, int insertLayerIndex, int &layerIndex, 
                             double &sizeA, double &sizeB, double &xoffset, double &yoffset)
{
   sizeA = sizeB = xoffset = yoffset = 0.0;

   if (block != NULL)
   {
      if (block->isComplexAperture())
      {
         BlockStruct *subblock = doc->Find_Block_by_Num((int)(block->getSizeA()));
         if (insertLayerIndex == -1 && doc->IsFloatingLayer(data.getLayerIndex()))
            layerIndex = Get_PADSTACKLayer(doc, &(subblock->getDataList()));

         // here we have to find the layer and block->size
         doc->validateBlockExtents(block);

         if (!block || block->getXmin() > block->getXmax())
         {
            sizeA = 0;
            sizeB = 0;
         }
         else
         { 
            sizeA = block->getXmax() - block->getXmin();
            sizeB = block->getYmax() - block->getYmin();
            xoffset = block->getXmin() + (block->getXmax() - block->getXmin())/2;
            yoffset = block->getYmin() + (block->getYmax() - block->getYmin())/2;
         }
      }
      else
      {
         sizeA = block->getSizeA();
         sizeB = block->getSizeB();
         xoffset = block->getXoffset();
         yoffset = block->getYoffset();

         // round and square have no B size.
         // if the Y in feature size is 0 feature is considered to be round
         // else square
         if (block->getShape() == apertureSquare)
            sizeB = sizeA;
      }
   }
}

/******************************************************************************
* updateIPCPadstackWithBlock
*/
static void updateIPCPadstack(CIPCPadstack& padstack, DataStruct& data, int insertLayerIndex, double scale)
{
   if (data.getDataType() != dataTypeInsert)
      return;

   // insert if mirror is either global mirror or block_mirror, but not if both.
	BlockStruct *block = doc->Find_Block_by_Num(data.getInsert()->getBlockNumber());
   if (block == NULL)
      return;

	if (block->isTool())
	{
      padstack.setDrillSize(block->getToolSize() * scale);
      padstack.setDrillPlated(block->getToolHolePlated());
	}
	else if (block->isAperture())
	{
		double sizeA, sizeB, xoffset, yoffset;

		int layerIndex = data.getLayerIndex();
		if (insertLayerIndex == -1 && doc->IsFloatingLayer(layerIndex))
			layerIndex = Get_PADSTACKLayer(doc, &(block->getDataList()));
      else if(insertLayerIndex != -1 && doc->IsFloatingLayer(layerIndex))
         layerIndex = insertLayerIndex;

		GetPadDimensions(block, data, insertLayerIndex, layerIndex, sizeA, sizeB,xoffset, yoffset);
      
      if (layerIndex < 0) 
			return;

      LayerStruct *ll = doc->getLayerAt(layerIndex);

      

		// if multiple electrical layers
      int type = padstack.getType();
		int lay = GetLayerStackNum(layerIndex);
		if (lay == 1 || lay == LAY_TOP)
		{
			double oldSizeA = padstack.getXSizeTop();
			double oldSizeB = padstack.getYSizeTop();

			if (ipcPadPseudoSize(block->getShape(), sizeA, sizeB) >
				ipcPadPseudoSize(block->getShape(), oldSizeA, oldSizeB))
			{
				if (ll->getNeverMirror())   
					type |= 0x8;
				else
					type |= 0x1;

            padstack.setShapeTypeTop(block->getShape());
            padstack.setRotationTop(block->getRotationRadians() + data.getInsert()->getAngle());
            padstack.setXSizeTop(sizeA);
            padstack.setYSizeTop(sizeB);
            padstack.setXOffsetTop(xoffset);
            padstack.setYOffsetTop(yoffset);
            padstack.setTopMostLayerNum(1);

            // Set bottom if not already set, in case this is only pad
            if (padstack.getBottomMostLayerNum() < 0)
               padstack.setBottomMostLayerNum(1);
			}
		}
		else if (lay == max_signallayer || lay == LAY_BOT)
		{
			double oldSizeA = padstack.getXSizeBottom();
			double oldSizeB = padstack.getYSizeBottom();

			if (ipcPadPseudoSize(block->getShape(), sizeA, sizeB) >
				ipcPadPseudoSize(block->getShape(), oldSizeA, oldSizeB))
			{
				if (ll->getMirrorOnly()) 
					type |= 0x10;
				else
					type |= 0x2;

            padstack.setShapeTypeBottom(block->getShape());
            padstack.setRotationBottom(block->getRotation() + data.getInsert()->getAngle());
            padstack.setXSizeBottom(sizeA);
            padstack.setYSizeBottom(sizeB);
            padstack.setXOffsetBottom(xoffset);
            padstack.setYOffsetBottom(yoffset);
            padstack.setBottomMostLayerNum(max_signallayer);

            // Set top if not already set, in case this is only pad
            if (padstack.getTopMostLayerNum() < 0)
               padstack.setTopMostLayerNum(max_signallayer);
         }
		}
		else if (lay == LAY_ALL)
		{
			// Since top and bottom would be set to the same size in this
			// case, it is enough to just check one or the other, no need
			// to check both.
			double oldSizeA = padstack.getXSizeTop();
			double oldSizeB = padstack.getYSizeTop();

			if (ipcPadPseudoSize(block->getShape(), sizeA, sizeB) >
				ipcPadPseudoSize(block->getShape(), oldSizeA, oldSizeB))
			{
				type |= 0x3;

            padstack.setShapeTypeTop(block->getShape());
            padstack.setRotationTop(block->getRotationRadians() + data.getInsert()->getAngle());
            padstack.setXSizeTop(sizeA);
            padstack.setYSizeTop(sizeB);
            padstack.setXOffsetTop(xoffset);
            padstack.setYOffsetTop(yoffset);
            padstack.setTopMostLayerNum(1);

            padstack.setShapeTypeBottom(block->getShape());
            padstack.setRotationBottom(block->getRotationRadians() + data.getInsert()->getAngle());
            padstack.setXSizeBottom(sizeA);
            padstack.setYSizeBottom(sizeB);
            padstack.setXOffsetBottom(xoffset);
            padstack.setYOffsetBottom(yoffset);
            padstack.setBottomMostLayerNum(max_signallayer);
			}
		}
      else if (lay != -99)
      {
         // Inner layer pad
         // not top or bottom or all
         CString layername(ll->getName());
         LayerTypeTag layertype = ll->getLayerType();
         int electLayNum = ll->getElectricalStackNumber();

         // If we have a legit electLayNum...
         if (electLayNum > 0)
         {
            // Init if needed
            if (padstack.getTopMostLayerNum() < 0)
               padstack.setTopMostLayerNum(electLayNum);
            if (padstack.getBottomMostLayerNum() < 0)
               padstack.setBottomMostLayerNum(electLayNum);

            // Keep lowest electLayNum for top, highest for bottom
            if (electLayNum < padstack.getTopMostLayerNum())
               padstack.setTopMostLayerNum(electLayNum);
            if (electLayNum > padstack.getBottomMostLayerNum())
               padstack.setBottomMostLayerNum(electLayNum);
         }
      }
      else
      {
         // Not electrical layer so we don't care about this pad.
         CString layername(ll->getName());
         LayerTypeTag layertype = ll->getLayerType();
      }

      padstack.setType(type);
	}
}

/******************************************************************************
* do_padstacks
*/
static void do_padstacks(double scale)
{
   for (int i=0; i<doc->getMaxBlockIndex(); i++)
   {
      BlockStruct *block = doc->getBlockAt(i);

      if (!block)
         continue;

      if (block->getFlags() & BL_WIDTH)
         continue;
         
      if (block->getBlockType() != BLOCKTYPE_PADSTACK)
         continue;

      CIPCPadstack* padstack = padstackMap.addPadstack(block);
      if (padstack != NULL)
         IPC_GetPADSTACKData(*padstack, *block, -1, scale);
   }
}

/******************************************************************************
* load_ipcsettings
*/
static int load_ipcsettings(const CString fname)
{
   FILE  *fp;
   char  line[255];
   char  *lp;

   linefeed = 1;
   ipc356_usePinInsertLoc = false;  // use center of extent is default
   ipc356_unusedpins = false;
   ipc356_single_nc = false;
   ipc356_unusednetname = "";
   ipc356_panel_netlist = "";
   ipc356_vias = false;
   ipc356_viarefdes = false;
   ARC_ANGLE_DEGREE = 5;

   if ((fp = fopen(fname,"rt")) == NULL)
   {
      // no settings file found
      CString tmp;
      tmp.Format("File [%s] not found",fname);
      ErrorMessage( tmp,"IPC Settings", MB_OK | MB_ICONHAND);
      return 0;
   }
              
   while (fgets(line,255,fp))
   {
      if ((lp = strtok(line," \t\n")) == NULL)  continue;
      if (lp[0] == '.')
      {
         // here do it.
         if (!STRCMPI(lp,".NEWLINE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'n' || lp[0] == 'N')
               linefeed = 0;
         }
         else
         if (!STRCMPI(lp,".USE_PIN_INSERT_LOC"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ipc356_usePinInsertLoc = (lp[0] == 'y' || lp[0] == 'Y');
         }
         else
         if (!STRCMPI(lp,".IPC356_UNUSEDPINS"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               ipc356_unusedpins = true;
         }
         else
         if (!STRCMPI(lp,".IPC356_SINGLENET_UNUSEDPINS"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               ipc356_single_nc = true;
         }
         else
         if (!STRCMPI(lp,".IPC356_USEVIAS"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               ipc356_vias = true;
         }
         else
         if (!STRCMPI(lp,".IPC356_VIAREFDES"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            if (lp[0] == 'y' || lp[0] == 'Y')
               ipc356_viarefdes = true;
         }
         else
         if (!STRCMPI(lp,".IPC356_UNUSEDNETNAME"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ipc356_unusednetname = lp;
         }
         else
         if (!STRCMPI(lp,".IPC356_PANEL_NETLIST"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ipc356_panel_netlist = lp;
         }
         else
         if (!STRICMP(lp,".ARCSTEPANGLE"))
         {
            if ((lp = strtok(NULL," \t\n")) == NULL)  continue;
            ARC_ANGLE_DEGREE = atof(lp);
            if (ARC_ANGLE_DEGREE < 4)     // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too small, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 4;
            }
            else
            if (ARC_ANGLE_DEGREE > 45)    // 360 / 120 not allowed
            {
               fprintf(flog,".ARCSTEPANGLE too large, must be between 4..45\n");
               display_error++;
               ARC_ANGLE_DEGREE = 45;
            }
         }
      }
   }

   fclose(fp);
   return 1;
}

//--------------------------------------------------------------
// getIpcUnitsPerpageUnits
//  1 IPC = 0.1 Mils = 0.0001 Inches = 0.001 mm
static int getIpcUnitsPerpageUnits(PageUnitsTag units)
{
   int IPC_scaleFactor = 1000; //default 

   switch(units)
   {
   case pageUnitsInches:
      IPC_scaleFactor = 10000;
      break;
   case pageUnitsMils:
      IPC_scaleFactor = 10;
      break;
   case pageUnitsMilliMeters:
      IPC_scaleFactor = 1000;
      break;
   }

   return IPC_scaleFactor;
}

static int getIpcUnitsAngle(double radians)
{
   // Degrees if English, 1/100 radians if SI.
   // Three digit integer.

   int retval = 0;

   switch(ipc_units)
   {
   case pageUnitsInches:
   case pageUnitsMils:
      retval = round(normalizeDegrees(RadToDeg(radians)));
      break;

   case pageUnitsMilliMeters:
      retval = round(normalizeRadians(radians) * 100.0);
      break;
   }

   return retval;
}

//--------------------------------------------------------------
//  Convert Inches, Mils, Millimeters to IPC units (no decimal)
static int conv_2_ipc(double value)
{
   int d;
   d = (int)floor(value * IpcUnitsPerPageUnit + 0.5);
   return d;
}

/******************************************************************************
* do_testvias
*/
static void do_testvias(const char *netname, const char *netname14, CDataList *DataList,
                        double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   Mat2x2   m;
   RotMat2(&m, rotation);

   for (int i=0; i<ipcViaCnt; i++)
   {
      if (!strcmp(ipcViaArray[i]->m_netname, netname))
      {
         IPCVia *ipcVia = ipcViaArray[i];

         DataStruct *data = ipcVia->m_data;
         if (data == NULL)
            continue;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         CIPCPadstack* padstack = NULL;
         
         // Find Padstack block of the bondpad
         if(data->isInsertType(insertTypeBondPad))
            block = GetBondPadPadstackBlock(doc->getCamCadData(), block);

         CString padstackName = (block)?block->getName():"";
         padstack = padstackMap.findPadstack(padstackName);
         if (padstack == NULL)
            continue;
         
         //floating layer should assign data's layer for bnondpad
         if(data->isInsertType(insertTypeBondPad))
            IPC_GetPADSTACKData(*padstack, *block, data->getLayerIndex(), scale);

         Point2 point2;
         point2.y = data->getInsert()->getOriginY() * scale;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         wipc_putchar_ipc(1, "327");       // here land, gets updates to drill if found
         wipc_set_opcode(true);    //indicating this is opcode
         wipc_putchar_ipc(4, netname14); // netname 14 char max. if longer use NNAME
      
         if (ipc356_viarefdes && data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
         {
            CString compname6;
            compname6.Format("%-6.6s", check_name('c', data->getInsert()->getRefname()));
            wipc_putchar_ipc(21, compname6);
            wipc_putchar_ipc(27, "-");
            wipc_putchar_ipc(28, "1");
         }
         else
            wipc_putchar_ipc(21, "VIA");

         CString diam;
         CString accesscode = "A00";
         double drill  =  padstack->getDrillSize();
         if (drill)
            wipc_putchar_ipc(33, "D"); // hole diameter
         else
            wipc_putchar_ipc(33, " "); // no diameter

         int ptyp = padstack->getType();   // 1 top  2 bottom
         int padmirror = 0;
         double pinrot = data->getInsert()->getAngleDegrees();
         double dimx = 0.0, dimy = 0.0;
         double offx = 0.0, offy = 0.0;
         double aprot = 0.;

         if (ptyp == 1 || ptyp == 0x08)   // 08 is top only
         {
            if (!padmirror)
            {
               accesscode = "A01";
               dimx = padstack->getXSizeTop();
               dimy = padstack->getYSizeTop();
               aprot = padstack->getRotationTop();
            }
            else
            {
               accesscode.Format("A%02d", max_signallayer);
               dimx = padstack->getXSizeBottom();
               dimy = padstack->getYSizeBottom();
               aprot = padstack->getRotationBottom();
            }
         }
         else if (ptyp == 2 || ptyp == 0x10)
         {
            if (!padmirror)
            {
               accesscode.Format("A%02d", max_signallayer);
               dimx = padstack->getXSizeBottom();
               dimy = padstack->getYSizeBottom();
               aprot = padstack->getRotationBottom();
            }
            else
            {
               accesscode = "A01";
               dimx = padstack->getXSizeTop();
               dimy = padstack->getYSizeTop();
               aprot = padstack->getRotationTop();
            }
         }
         else if (ptyp == 3)
         {
            accesscode = "A00";
            dimx = padstack->getXSizeBottom();
            dimy = padstack->getYSizeBottom();
            aprot = padstack->getRotationBottom();
         }
         else
         {
            fprintf(flog, "Test via [%s] without test area \n", block->getName());
            display_error++;
            continue;
         }

         // here rotate dimx, dimy in 90 degree steps
         if (dimy > 0)  // if dimy  == 0 it is round or square.
         {
            aprot = fabs(RadToDeg(aprot));
            for (int i=0;i < round((pinrot+aprot)/90); i++)
            {
               double tmp;
               tmp = dimx;
               dimx = dimy;
               dimy = tmp;
            }
         }

         if (drill > 0)
         {
            wipc_putchar_ipc(1, "317");
            wipc_set_opcode(true);    //indicating this is opcode
            diam.Format("%4d", conv_2_ipc(drill)); 
            wipc_putchar_ipc(34, diam);   // diam format
            wipc_putchar_ipc(38, padstack->getDrillPlated() ? "P" : "U");
         }
         // access code A00 = all
         //             A01 = top
         //             A0n = bottom
         wipc_putchar_ipc(39, accesscode);
               
         CString koo; // +- 6 digit, 
         koo.Format("X%+07dY%+07d", conv_2_ipc(point2.x + (offx * scale)), conv_2_ipc(point2.y + (offy * scale)));
         wipc_putchar_ipc(42, koo);

         koo.Format("X%04dY%04d",conv_2_ipc(dimx * scale), conv_2_ipc(dimy * scale));
         wipc_putchar_ipc(58,koo);        // xdim ydim
         wipc_write_ipcline();
      }
   }  

   return;
}
/******************************************************************************
* do_testvias356A
*/
static void do_testvias356A(const char *netname, const char *netname14, CDataList *DataList,
                        double insert_x, double insert_y, double rotation, int mirror, double scale)
{
   Mat2x2   m;
   RotMat2(&m, rotation);

   for (int i=0; i<ipcViaCnt; i++)
   {
      if (!strcmp(ipcViaArray[i]->m_netname, netname))
      {
         IPCVia *ipcVia = ipcViaArray[i];

         DataStruct *data = ipcVia->m_data;
         if (data == NULL)
            continue;

         BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
         CIPCPadstack* padstack = NULL;
         
         // Find Padstack block of the bondpad
         if(data->isInsertType(insertTypeBondPad))
            block = GetBondPadPadstackBlock(doc->getCamCadData(), block);

         CString padstackName = (block)?block->getName():"";
         padstack = padstackMap.findPadstack(padstackName);
         if (padstack == NULL)
            continue;
         
         // Floating layer should assign data's layer for bondpad
         if(data->isInsertType(insertTypeBondPad))
            IPC_GetPADSTACKData(*padstack, *block, data->getLayerIndex(), scale);

         Point2 point2;
         point2.y = data->getInsert()->getOriginY() * scale;
         point2.x = data->getInsert()->getOriginX() * scale;
         if (mirror)
            point2.x = -point2.x;
         TransPoint2(&point2, 1, &m, insert_x, insert_y);

         CString accesscode = "A00";
         double drill  =  padstack->getDrillSize();

         int ptyp = padstack->getType();   // 1 top  2 bottom
         int viaMirror = data->getInsert()->getGraphicMirrored();
         double pinrot = data->getInsert()->getAngleDegrees();
         double dimx = 0.0, dimy = 0.0;
         double offx = 0.0, offy = 0.0;
         double aprot = 0.;

         bool blindViaVisibleFromTop = (padstack->getTopMostLayerNum() == 1 && padstack->getBottomMostLayerNum() <  max_signallayer);
         bool blindViaVisibleFromBot = (padstack->getTopMostLayerNum() >  1 && padstack->getBottomMostLayerNum() == max_signallayer);
         bool blindVia = (blindViaVisibleFromTop || blindViaVisibleFromBot);
         bool buriedVia = (padstack->getTopMostLayerNum() > 1 && padstack->getBottomMostLayerNum() < max_signallayer);

         if (ptyp == 1 || ptyp == 0x08)   // 08 is top only, blind via
         {
            if (!viaMirror)
            {
               accesscode = "A01";
            }
            else
            {
               accesscode.Format("A%02d", max_signallayer);
            }
            dimx = padstack->getXSizeTop();
            dimy = padstack->getYSizeTop();
            aprot = padstack->getRotationTop();
         }
         else if (ptyp == 2 || ptyp == 0x10) // bottom only, blind via
         {
            if (!viaMirror)
            {
               accesscode.Format("A%02d", max_signallayer);
            }
            else
            {
               accesscode = "A01";
            }
            dimx = padstack->getXSizeBottom();
            dimy = padstack->getYSizeBottom();
            aprot = padstack->getRotationBottom();
         }
         else if (ptyp == 3) // actual through via
         {
            accesscode = "A00";
            dimx = padstack->getXSizeBottom();
            dimy = padstack->getYSizeBottom();
            aprot = padstack->getRotationBottom();
         }
         else
         {
            // Buried via
            // No pad on any surface for buried via, set access to nothing, sizes/rotation to zero.
            accesscode = "   "; // Keep size at 3 chars to match size of A01 and A0n
            dimx = 0.0;
            dimy = 0.0;
            aprot = 0.0;
         }

         // here rotate dimx, dimy in 90 degree steps
         if (dimy > 0)  // if dimy  == 0 it is round or square.
         {
            aprot = fabs(RadToDeg(aprot));
            for (int i=0;i < round((pinrot+aprot)/90); i++)  // BUG This is mixing radians and degrees !
            {
               double tmp;
               tmp = dimx;
               dimx = dimy;
               dimy = tmp;
            }
         }

         // Blind and buried vias both get 307 to define via.
         // Genuine through via gets 317.
         // Blind via gets 027 in addition, to define surface access point.
         if (blindVia || buriedVia)
         {
            wipc_putchar_ipc(1, "307");   // 307 for blind and buried vias
            wipc_set_opcode(true);    // indicating this is opcode
         }
         else
         {
            // Regular thru vias
            wipc_putchar_ipc(1, "317");   // 307 for blind and buried vias
            wipc_set_opcode(true);    // indicating this is opcode
         }

         // IPC356A spec says 307 and 317 are the same in char positions 4 to 74, except access side
         // is inconsequential for 307 (it will be taken from layer info) and feature size should
         // be blank in 307. Feature size starts in col 58, so really 307 and 317 are the same
         // in 4 to 57.

         // Netname, 4-17 same in both
         wipc_putchar_ipc(4, netname14); // netname 14 char max. if longer use NNAME

         // 18-20 should be left blank

         // 21-32 is refdes and pinref
         CString ipcRefname("VIA"); // default
         if (ipc356_viarefdes && data->getInsert()->getRefname() && strlen(data->getInsert()->getRefname()))
         {
            // Refname limited to 6 chars, cols 21-26, just truncate it. The "-1" at end is the pinref.
            ipcRefname.Format("%-6.6s-1", check_name('c', data->getInsert()->getRefname()));
         }
         wipc_putchar_ipc(21, ipcRefname);

         // 33-38 hole definition
         if (drill > 0)
         {
            CString diam;
            wipc_putchar_ipc(33, "D"); // hole diameter code
            diam.Format("%4d", conv_2_ipc(drill)); 
            wipc_putchar_ipc(34, diam);   // diameter format
            wipc_putchar_ipc(38, padstack->getDrillPlated() ? "P" : "U");
         }

         // 39-41 surface access code
         // access code A00 = all
         //             A01 = top
         //             A0n = bottom
         //         <blank> = buried via
         wipc_putchar_ipc(39, accesscode);

         // 42-57 access point location aka feature location          
         CString featureLocation; // +- 6 digit, 
         featureLocation.Format("X%+07dY%+07d", conv_2_ipc(point2.x + (offx * scale)), conv_2_ipc(point2.y + (offy * scale)));
         wipc_putchar_ipc(42, featureLocation);

         // Now 307 (blind/buried) and 317 (thru) vias get different again.
         // 58-71 Feature size.
         // The size is written here for 317, but not for 307.
         // For 307 the size will be part of 027 continuation record, so go ahead and calc
         // it here.
         CString featureSize;
         featureSize.Format("X%04dY%04d",conv_2_ipc(dimx * scale), conv_2_ipc(dimy * scale));
         if (!blindVia && !buriedVia)
         {
            wipc_putchar_ipc(58, featureSize);        // xdim ydim
         }

         // 73-74 is defined in spec as soldermask info. This exporter was writing nothing
         // here before DRs 682292 and 647197, which are essentially about adding support for
         // 307 blind/buries vias. So nothing was changed about that, the DRs don't mention
         // any soldermask stuff.

         // Layer info, blind/buried vias only
         if (blindVia || buriedVia)
         {
            CString layerDesignator;

            // Consider the via insert mirror, might cause padstack layers to flip.
            // The calc here will flip (mirror) the layers, and also keep top as
            // the smaller number, which IPC seems to want.
            int topLayNum = padstack->getTopMostLayerNum();
            int botLayNum = padstack->getBottomMostLayerNum();
            if (viaMirror)
            {
               int tempTopLayNum = topLayNum;
               topLayNum = max_signallayer - botLayNum + 1;
               botLayNum = max_signallayer - tempTopLayNum + 1;
            }

            // 75-77 start layer, closest to top is considered start
            layerDesignator.Format("L%02d", topLayNum);
            wipc_putchar_ipc(75, layerDesignator);

            // 78-80 end layer, closest to bottom is considered end
            layerDesignator.Format("L%02d", botLayNum);
            wipc_putchar_ipc(78, layerDesignator);
         }

         // Write the 307/317 record
         wipc_write_ipcline();

         // Blind vias have one surface pad exposed, and must have this
         // this companion 027 record. This looks just like a 317 record,
         // except no drill is specified (because drill is in 307 and is
         // limited to the spanned layers), and it is considered a
         // continuation record rather than an opcode record. It also does
         // not contain the netname, but I bet it wouldn't matter if it did.
         if (blindVia)
         {
            wipc_putchar_ipc(1, "027");
            wipc_set_opcode(true);
            wipc_putchar_ipc(21, ipcRefname);
            wipc_putchar_ipc(39, accesscode);
            wipc_putchar_ipc(42, featureLocation);
            wipc_putchar_ipc(58, featureSize);
            wipc_write_ipcline();
         }
      }
   }  

   return;
}

/******************************************************************************
* load_testvias
*/
static void load_testvias(CDataList *DataList, double scale)
{
   CInsertTypeMask insertTypeMask(insertTypeVia, insertTypeFreePad, insertTypeBondPad);
   for (CDataListIterator dataListIterator(*DataList, insertTypeMask); dataListIterator.hasNext();)
   {
      DataStruct* data = dataListIterator.getNext();

      CString netname = "";
      Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
      if (attrib)
         netname = attrib->getStringValue();

      IPCVia *via = new IPCVia(netname, data);
      ipcViaArray.SetAtGrow(ipcViaCnt++, via);  
   }
}

/******************************************************************************
* write_pins
*/
static int write_pins(int ipcType, NetStruct *net, double file_x, double file_y, double file_angle, 
                      int file_mirror, double scale, const char *netname14)
{
   Mat2x2 m;
   RotMat2(&m, file_angle);

   // SaveAttribs(stream, &net->getAttributesRef());
   POSITION compPinPos = net->getHeadCompPinPosition();
   while (compPinPos != NULL)
   {
      CompPinStruct *compPin = net->getNextCompPin(compPinPos);

      if (compPin->getPinCoordinatesComplete())
      {
         Point2 compPinInsertOrigin;
         compPinInsertOrigin.y = compPin->getOriginY();
         compPinInsertOrigin.x = compPin->getOriginX();
         if (file_mirror)
            compPinInsertOrigin.x = -compPinInsertOrigin.x;
         
         TransPoint2(&compPinInsertOrigin, 1, &m, file_x, file_y);

         double pinx = compPinInsertOrigin.x;
         double piny = compPinInsertOrigin.y;
         double pinrotRadians = compPin->getRotationRadians()+file_angle;
         int padmirror = file_mirror ^ compPin->getMirror();

         CString padstackname = "";
         int smd = FALSE;
         if (compPin->getPadstackBlockNumber() > -1)
         {
            BlockStruct *block = doc->Find_Block_by_Num(compPin->getPadstackBlockNumber());
            if (block != NULL)
            {
               padstackname = block->getName();

               // here need to find block definition for padstackArray to find
               // if there is a SMD flag.
               if (is_attvalue(doc, block->getAttributesRef(), ATT_SMDSHAPE, 1)) // 
                  smd = TRUE;
            }
         }

         CIPCPadstack* padstack = padstackMap.findPadstack(padstackname);
         if (padstack == NULL)
         {
            //Die pins can't be found because the blockTypeDiePin not supported
            fprintf(flog, "Net: %s Skipped CompPin %s - Can't find padstack %s.\n", 
               net->getNetName(), compPin->getPinRef(), padstackname);
            continue;
         }

         int not_anytest = FALSE;
         int test_attribute = FALSE;

         CString compname6, pinname4;
         compname6.Format("%-6.6s", check_name('c', compPin->getRefDes()));
         pinname4.Format("%-4.4s", check_name('p', compPin->getPinName()));

         wipc_putchar_ipc(1, "327");       // here land, gets updates to drill if found
         wipc_set_opcode(true);    //indicating this is opcode
         wipc_putchar_ipc(4, netname14); // netname 14 char max. if longer use NNAME
         wipc_putchar_ipc(21, compname6);
         wipc_putchar_ipc(27, "-");
         wipc_putchar_ipc(28, pinname4);

         double drill  = padstack->getDrillSize();
         if (drill)
            wipc_putchar_ipc(33, "D"); // hole diameter
         else
            wipc_putchar_ipc(33, " "); // no hole

         int ptyp = padstack->getType();   // 1 top
         int play = 0;  // pin layer
         if (!padmirror)   
         {
            // pad not mirrored
            if (ptyp & 0x01)
               play |= 1;
            else if (ptyp & 0x08)
               play |= 1;
            if (ptyp & 0x02)
               play |= 2;
         }
         else
         {
            // pad is mirrored
            if (ptyp & 0x01)
               play |= 2;
            else if (ptyp & 0x10)
               play |= 2;

            if (ptyp & 0x02)
               play |= 1;
         }

         CString accesscode = "A00";
         double dimx = 0.0, dimy = 0.0;
         double offx = 0.0, offy = 0.0;
         double aprotRadians = pinrotRadians;
         if (play == 1)
         {
            Point2 off;
            dimx = padstack->getXSizeTop();
            dimy = padstack->getYSizeTop();
            aprotRadians += padstack->getRotationTop();
            off.x = padstack->getXOffsetTop();
            off.y = padstack->getYOffsetTop();

            Mat2x2 m1;
            RotMat2(&m1, aprotRadians);

            TransPoint2(&off, 1, &m1, 0.0, 0.0);
            offx = off.x;
            offy = off.y;

            accesscode = "A01";
         }
         else if (play == 2)
         {
            Point2 off;
            if (ptyp == 1) // pindef top
            {
               dimx = padstack->getXSizeTop();
               dimy = padstack->getYSizeTop();
               aprotRadians += padstack->getRotationTop();
               off.x = padstack->getXOffsetTop();
               off.y = padstack->getYOffsetTop();
            }
            else
            {
               dimx = padstack->getXSizeBottom();
               dimy = padstack->getYSizeBottom();
               aprotRadians += padstack->getRotationBottom();
               off.x = padstack->getXOffsetBottom();
               off.y = padstack->getYOffsetBottom();
            }

            Mat2x2 m1;
            RotMat2(&m1, aprotRadians);

            TransPoint2(&off, 1, &m1, 0.0, 0.0);
            offx = off.x;
            offy = off.y;

            accesscode.Format("A%02d",max_signallayer);
         }
         else if (ptyp == 3)
         {
            accesscode = "A00";
            Point2   off;
            dimx = padstack->getXSizeBottom();
            dimy = padstack->getYSizeBottom();
            aprotRadians += padstack->getRotationBottom();
            off.x = padstack->getXOffsetBottom();
            off.y = padstack->getYOffsetBottom();

            Mat2x2 m1;
            RotMat2(&m1, aprotRadians);

            TransPoint2(&off, 1, &m1, 0.0, 0.0);
            offx = off.x;
            offy = off.y;
         }
         else
         {
            fprintf(flog, "Test pin without test area Component [%s] Pin [%s]\n", compPin->getRefDes(), compPin->getPinName());
            display_error++;
            continue;
         }

         // here rotate dimx, dimy in 90 degree steps
         // DR 851845 changed the way we do this rotation offset thing.
         double rotOffsetDeg = 0; //*rcf
         if (dimy > 0)  // if dimy  == 0 it is round or square.
         {
            double aprotDeg = fabs(RadToDeg(aprotRadians));

            int aprotDegInt = (int)aprotDeg; // Yes, drop fractions of degree. 45.000003342 was messing us up with an extra rot cycle.
            // We want to loop once  for each increment of 90 degrees in the angle. Trouble is with
            // angles that are "close", e.g. 89.2 is close enough to 90.
            // So, we have the positive rot in degrees, we'll loop while it is greater than 80 degrees.
            // Drop off 90 each time, rotate the aperture while there is still > 80.
            while (aprotDegInt > 80)
            {
               aprotDegInt -= 90;

               double tmp;
               tmp = dimx;
               dimx = dimy;
               dimy = tmp;

               rotOffsetDeg += 90;
            }
         }

         if (drill > 0)
         {
            wipc_putchar_ipc(1, "317");      // here land, gets updates to drill if found
            wipc_set_opcode(true);       //indicating this is opcode

            CString diam;
            diam.Format("%4d", conv_2_ipc(drill)); 
            wipc_putchar_ipc(34, diam);      // diam format
            wipc_putchar_ipc(38, padstack->getDrillPlated() ? "P" : "U");
         }

         // access code A00 = all
         //             A01 = top
         //             A0n = bottom
         wipc_putchar_ipc(39, accesscode);
            
         // DR 682296 User can select to apply offset of use original pin insert loc.
         // ipc356_usePinInsertLoc == true means use the pin insert as-is.
         // == false means offset the pin to center of pad, which is original standard
         CString koo; // +- 6 digit, 
         double ipcPinX = pinx;
         double ipcPinY = piny;
         if (!ipc356_usePinInsertLoc)
         {
            ipcPinX += offx;
            ipcPinY += offy;
         }
         koo.Format("X%+07dY%+07d", conv_2_ipc(ipcPinX * scale), conv_2_ipc(ipcPinY * scale));

         wipc_putchar_ipc(42, koo);

         koo.Format("X%04dY%04d", conv_2_ipc(dimx * scale), conv_2_ipc(dimy * scale));
         wipc_putchar_ipc(58, koo);          // xdim ydim

         // DR 851845 says we need to supply rotation. IPC spec says send rotation only for non-zero.
         // Spec we have that says Rotation is supported is IPC356A, so do this for 356A only.

         double exportRotRadians = aprotRadians - DegToRad(rotOffsetDeg);

         if (ipcType == IPC356A && exportRotRadians != 0)
         {
            // Degrees for inch-based, hundredths of radian if SI.
            int ipcAngle = getIpcUnitsAngle(exportRotRadians);
            // Double check to make sure it didn't turn to zero.
            if (ipcAngle != 0)
            {
               koo.Format("R%03d", ipcAngle);
               wipc_putchar_ipc(68, koo);
            }
         }
            
         // do not write if no dimension or drill.
         if (dimx == 0 && dimy == 0 && drill == 0)
            wipc_clean_ipcline();
         else
            wipc_write_ipcline();
      }
   }

   return 1;
}

/******************************************************************************
* do_pinloc
*/
static int do_pinloc(CNetList *NetList, double scale, CDataList *DataList, int ipcvias, const char *netlist_prefix, int *start_netcnt,
      double file_x, double file_y, double file_angle, int file_mirror)
{
   NetStruct *net;
   POSITION netPos;
   int      netcnt = *start_netcnt;
   CString  netname;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      netcnt++;

      // if nonconnect used
      if (!ipc356_unusedpins && (net->getFlags() & NETFLAG_UNUSEDNET))
         continue;

      CString  netname14; 
      netname = netlist_prefix;
      netname += net->getNetName();

      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
         netname14 = ipc356_unusednetname;
      }
      else
         netname14.Format("%-14s",check_name('n', netname));
   
      CString  tmp;
      tmp.Format("CAD Netname : %s --- IPC Netname : %s", netname, netname14);
      wipc_write2file(tmp);

      write_pins(IPC356, net, file_x, file_y, file_angle, file_mirror, scale, netname14);

      if (ipcvias)
         do_testvias(net->getNetName(), netname14, DataList, file_x, file_y, file_angle, file_mirror, scale);
   }

   if (ipcvias)
      do_testvias("", ipc356_unusednetname, DataList, file_x, file_y, file_angle, file_mirror, scale);

   *start_netcnt = netcnt;
   return 1;
} // do_pinloc

/******************************************************************************
* do_netnamemap356a
*/
static int do_netnamemap356a(CNetList *NetList, const char *netlist_prefix)
{
   NetStruct *net;
   POSITION netPos;
   CString  netname;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      // if nonconnect used
      if (!ipc356_unusedpins && (net->getFlags() & NETFLAG_UNUSEDNET))   continue;

      CString  netname14; 
      netname = netlist_prefix;
      netname += net->getNetName();

      if (net->getFlags() & NETFLAG_UNUSEDNET)
      {
         netname14 = ipc356_unusednetname;
         continue;   // to not mention this.
      }
      else
      {
         netname14.Format("%-14s",check_name('n', netname));
         netname14.TrimLeft();
         netname14.TrimRight();
      }

      if (netname14.CompareNoCase(netname))  // only write out if it is different.
      {
         wipc_putchar_ipc(1,"P");   
         wipc_putchar_ipc(4, netname14);
         wipc_putchar_ipc(15, netname);
         wipc_write_ipcline();
      }
   }

   return 1;
}

/******************************************************************************
* do_pinloc356A
*/
static int do_pinloc356A(CNetList *NetList, double scale, CDataList *DataList, int ipcvias, const char *netlist_prefix, 
      int *start_netcnt, double file_x, double file_y, double file_angle, int file_mirror)
{
   NetStruct *net;
   POSITION netPos;
   int      netcnt = *start_netcnt;
   CString  netname;

   netPos = NetList->GetHeadPosition();
   while (netPos != NULL)
   {
      net = NetList->GetNext(netPos);
      netcnt++;

      // if nonconnect used
      if (!ipc356_unusedpins && (net->getFlags() & NETFLAG_UNUSEDNET))   continue;

      CString  netname14; 
      netname = netlist_prefix;
      netname += net->getNetName();

      if (net->getFlags() & NETFLAG_UNUSEDNET)
         netname14 = ipc356_unusednetname;
      else
         netname14.Format("%-14s",check_name('n', netname));
   
      write_pins(IPC356A, net, file_x, file_y, file_angle, file_mirror, scale, netname14);

      if (ipcvias)
         do_testvias356A(net->getNetName(), netname14, DataList, file_x, file_y, file_angle, file_mirror, scale);
   }

   if (ipcvias)
      do_testvias356A("", "N/C", DataList, file_x, file_y, file_angle, file_mirror, scale);

   *start_netcnt = netcnt;
   return 1;
} // do_pinloc356A

//--------------------------------------------------------------
static void write_attributes(CAttributes* map, double ix, double iy, double rotation, int mirror, double scale)
{
   if (map == NULL)
      return;

   double   TEXTRATIO = 0.75;

   CString  dxf_line;
   WORD keyword;
   Attrib* attrib;
   
   for (POSITION pos = map->GetStartPosition();pos != NULL;)
   {
      map->GetNextAssoc(pos, keyword, attrib);

      // do not write out general Keywords.
      //if (keyword == doc->IsKeyWord(ATT_NETNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_REFNAME, 0))      continue;
      //if (keyword == doc->IsKeyWord(ATT_COMPPINNR, 0))    continue;
      //if (keyword == doc->IsKeyWord(ATT_TYPELISTLINK, 0)) continue;

      int      flg = 0;
      double   height, width, angle;

      if (!attrib->isVisible())
      {
         flg = 1;
         continue;      // do not show an invisble attribute.
      }

      height = attrib->getHeight() * scale;
      width  = attrib->getWidth() * scale;

      if (mirror)
         angle = rotation - attrib->getRotationRadians();
      else
         angle = rotation + attrib->getRotationRadians();

      // ix, iy
      Point2   point2;
      Mat2x2   m;

      RotMat2(&m, rotation);

      point2.x = attrib->getX()*scale;
      if (mirror)
      {
         point2.x = -attrib->getX()*scale;
      }
      point2.y = attrib->getY()*scale;

      TransPoint2(&point2, 1, &m, ix,iy);
      
      //if (!doc->get_layer_visible(np->getLayerIndex(), mirror, np->getDataType()))
      //    continue;
      wipc_Graph_Layer(doc->get_layer_mirror(attrib->getLayerIndex(), mirror));

      switch (attrib->getValueType())
      {
         case VT_INTEGER:
         {
            CString line;
            line.Format("%d", attrib->getIntValue());
            wipc_Graph_Text( line, point2.x, point2.y,
                      height,
                      width,
                      doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale,
                      angle, mirror, 
                      0 );
         }
         break;
         case VT_UNIT_DOUBLE:
         case VT_DOUBLE:
         {
            CString line;
            line.Format("%lg", attrib->getDoubleValue());
            wipc_Graph_Text( line, point2.x, point2.y,
                      height,
                      width,
                      doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale,
                      angle, mirror, 
                      0 );
         }
         break;
         case VT_STRING:
         {
            char *tok, *temp = STRDUP(get_attvalue_string(doc, attrib));
            
            tok = strtok(temp,"\n");
            while (tok)
            {
               CString  line;
               if (attrib->getStringValueIndex() != -1)
                  line.Format("%s", tok); // multiple values are delimited by \n
               else
               {                 
                  continue;
               }
               wipc_Graph_Text( line, point2.x, point2.y,
                      height,
                      width,
                      doc->getWidthTable()[attrib->getPenWidthIndex()]->getSizeA()*scale,
                      angle, mirror, 
                      0 );
               tok = strtok(NULL,"\n");
            }
            free(temp);
         }
         break;
      }
   }

   return;
}

/******************************************************************************
* write_356apoly
*/
static void write_356apoly(CString opCode, CString netname, Point2* points, int pointCount, double lineWidth, int layerNum)
{
   wipc_putchar_ipc(1, opCode);  
   wipc_set_opcode(true);    //indicating this is opcode
   opCode.SetAt(0, '0');

   wipc_putchar_ipc(4, netname); 

   // here layer
   if (layerNum > 0)
   {
      CString layerName;
      layerName.Format("L%02d", layerNum);
      wipc_putchar_ipc(19, layerName); 
   }

   // here write width
   CString width;
   width.Format("X%04d", conv_2_ipc(lineWidth)),
   wipc_putchar_ipc(23,width); 

   int start = 34;

   for (int i=0; i<pointCount; i++)
   {
      double da = atan(points[i].bulge) * 4;

      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;   

         x1 = points[i].x;
         y1 = points[i].y;
         x2 = points[i+1].x;
         y2 = points[i+1].y;
         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         // here arc to poly
         int ii, ppolycnt = 255; // only for an arc 
         // start center
         Point2 *ppoly = (Point2 *)calloc(255,sizeof(Point2));
         ArcPoly2(x1,y1,cx,cy,da,1.0,ppoly,&ppolycnt,DegToRad(ARC_ANGLE_DEGREE));

         for (ii=0;ii<ppolycnt;ii++)
         {
            CString coordinate;
            coordinate.Format("X%+07dY%+07d", conv_2_ipc(ppoly[ii].x), conv_2_ipc(ppoly[ii].y));

            if (start + coordinate.GetLength() + 1 > 72)
            {
               wipc_write_ipcline();
               wipc_putchar_ipc(1, opCode);
               wipc_set_opcode(true);    //indicating this is opcode
               start = 5;  
            }

            wipc_putchar_ipc(start, coordinate);
            start += strlen(coordinate) + 1;
         }
         free(ppoly);
      }
      else
      {
         CString coordinate;
         coordinate.Format("X%+07dY%+07d",conv_2_ipc(points[i].x), conv_2_ipc(points[i].y));
         if (start + coordinate.GetLength() + 1 > 72)
         {
            wipc_write_ipcline();
            wipc_putchar_ipc(1, opCode);  
            wipc_set_opcode(true);    //indicating this is opcode
            start = 5;  
         }

         wipc_putchar_ipc(start, coordinate);
         start += strlen(coordinate) + 1;
      }
   }

   wipc_write_ipcline();
}

//--------------------------------------------------------------
static int get_app_values(BlockStruct *block, double block_rot, double *dimx, double *dimy, 
                          double *offx, double *offy)
{
   double approt = fabs(RadToDeg(block_rot + block->getRotationRadians()));
   
   *dimx = block->getSizeA();
   *dimy = block->getSizeB();
   *offx = block->getXoffset();
   *offy = block->getYoffset();
   
   double x, y;
   Rotate(*offx, *offy, RadToDeg(approt),&x,&y);
   *offx = x;
   *offy = y;

   if (dimy > 0)  // if dimy  == 0 it is round or square.
   {
      int i;
      for (i=0;i < round((approt)/90);i++)
      {
         double tmp;
         tmp = *dimx;
         *dimx = *dimy;
         *dimy = tmp;
      }
   }

   return 1;
}

/******************************************************************************
* IPC356A_WriteData
*/
static void IPC356A_WriteData(CDataList *DataList, CString netPrefix, double insert_x, double insert_y, double rotation,
      int mirror, double scale, int embeddedLevel, int insertLayer, double unitScale) // unitScale only used for apertures because they are not scaled
{
   DTransform xform(insert_x, insert_y, scale, rotation, mirror);

   POSITION dataPos = DataList->GetHeadPosition();
   while (dataPos)
   {
      DataStruct *data = DataList->GetNext(dataPos);

      BOOL BoardOutlineLayer = FALSE;

      // only draw visible layer (Inserts do not have layers)
      if (data->getDataType() != T_INSERT)
      {
         int layerNum;

         if (data->getLayerIndex() == -1)
            layerNum = insertLayer;
         else
            layerNum = data->getLayerIndex();

         if (insertLayer == -2)   // this is the non layer for the complex aperture definition
         {
            // ipclib should not write a layer in its entities
            layerNum = -2;
         }
         else
         {
            if (!doc->get_layer_visible(layerNum, mirror))
              continue;
         }

         LayerStruct *layer = doc->FindLayer(layerNum);
         if (layer->getLayerType() == LAYTYPE_BOARD_OUTLINE)
            BoardOutlineLayer = TRUE;

         wipc_Graph_Layer(layerNum);
      }

      // inserts have no initial layers
      switch (data->getDataType())
      {
      case T_POLY:
         {
            // count pnts
            int pointCount = 0;
            POSITION polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               if (poly->isHidden())
                  continue;

               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);
                  pointCount++;
               }
            }

            Point2 *points = (Point2 *)calloc(pointCount, sizeof(Point2)); // entities+1 because num of points = num of segments +1

            polyPos = data->getPolyList()->GetHeadPosition();
            while (polyPos)
            {
               CPoly *poly = data->getPolyList()->GetNext(polyPos);

               if (poly->isHidden())
                  continue;

               int dcode = doc->getWidthTable()[poly->getWidthIndex()]->getDcode();
               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();

               double lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA() * scale;
                     
               int pointCount = 0;
               POSITION pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos)
               {
                  CPnt *pnt = poly->getPntList().GetNext(pntPos);

                  Point2 *point = &points[pointCount++];
                  point->x = pnt->x;
                  point->y = pnt->y;
                  point->bulge = pnt->bulge;
                  xform.TransformPoint(point);
               }

               if (BoardOutlineLayer || data->getGraphicClass() == GR_CLASS_BOARDOUTLINE)
                  write_356apoly("389", "BOARD_EDGE", points, pointCount, lineWidth, -1);
               else if (data->getGraphicClass() == GR_CLASS_ETCH)
               {
                  Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
                  if (attrib)
                  {
                     CString netname;
                     netname.Format("%s%s", netPrefix, attrib->getStringValue());
                     CString netname14;
                     netname14.Format("%-14s", check_name('n', netname));
                     int layerNum = GetLayerStackNum(data->getLayerIndex());
                     write_356apoly("378", netname14, points, pointCount, lineWidth, layerNum);
                  }
               }
            }

            free(points);
         }
         break;

      case T_TEXT:
         break;

      case T_INSERT:
         {
            Point2 point;
            point.x = data->getInsert()->getOriginX();
            point.y = data->getInsert()->getOriginY();
            xform.TransformPoint(&point);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ data->getInsert()->getMirrorFlags();
            double block_rot;

            if (mirror)
               block_rot = rotation - data->getInsert()->getAngle();
            else
               block_rot = rotation + data->getInsert()->getAngle();

            CString netname = ipc356_unusednetname;
            Attrib *attrib = is_attvalue(doc, data->getAttributesRef(), ATT_NETNAME, 1);
            if (attrib)
               netname.Format("%s%s", netPrefix, attrib->getStringValue());
            CString netname14;
            netname14.Format("%-14s", check_name('n', netname));

            BlockStruct *block = doc->Find_Block_by_Num(data->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               int layerNum = Get_ApertureLayer(doc, data, block, insertLayer);

               int stackNum = GetLayerStackNum(layerNum);
               CString accesscode;
               accesscode.Format("A%02d", max_signallayer);

               if (stackNum == 1 || stackNum == LAY_TOP)
                  accesscode = "A01";
               else if (stackNum > 1 || stackNum == LAY_BOT)
                  accesscode.Format("A%02d", max_signallayer);
               else if (stackNum == LAY_ALL )
                  accesscode = "A00";
               
               if (doc->get_layer_visible(layerNum, mirror)) 
               {
                  // 327 are features
                  // 327$Net15           VIA               A02X+023890Y-065240X+910Y+630     S1       
                  wipc_putchar_ipc(1,"327");     // here land, gets updates to drill if found
                  wipc_set_opcode(true);    //indicating this is opcode
                  wipc_putchar_ipc(4,netname14); // netname 14 char max. if longer use NNAME
                  wipc_putchar_ipc(21,"VIA");

                  double dimx = 0.0, dimy = 0.0;
                  double offx = 0.0, offy = 0.0;

                  get_app_values(block, block_rot, &dimx, &dimy, &offx, &offy);

                  // access code A00 = all
                  //             A01 = top
                  //             A0n = bottom
                  wipc_putchar_ipc(39,accesscode);
               
                  CString koo; // +- 6 digit, 
                  koo.Format("X%+07dY%+07d", conv_2_ipc(point.x+(offx*scale)), conv_2_ipc(point.y+(offy*scale)));
                  wipc_putchar_ipc(42,koo);

                  if (dimy > 0)
                     koo.Format("X%04dY%04d", conv_2_ipc(dimx*scale), conv_2_ipc(dimy*scale));
                  else
                     koo.Format("X%04dY", conv_2_ipc(dimx*scale));
                  wipc_putchar_ipc(58,koo); // xdim ydim

#ifdef DR_851845
                  // The DR data did not trigger any rotated stuff here, so not certain this should be here.
                  // It is being left for easy activation, but until proven it is desired it is disabled.

                  // DR 851845 says we need to supply rotation. IPC spec says send rotation only for non-zero.
                  if (block_rot != 0)
                  {
                     // Degrees for inch-based, hundredths of radian if SI.
                     int ipcAngle = getIpcUnitsAngle(block_rot);
                     // Double check to make sure it didn't turn to zero.
                     if (ipcAngle != 0)
                     {
                        koo.Format("R%03d", ipcAngle);
                        wipc_putchar_ipc(68, koo);
                     }
                  }
#endif
         
                  wipc_write_ipcline();

               }
            }
            else if ( (block->getFlags() & BL_TOOL) || (block->getFlags() & BL_BLOCK_TOOL) )
            {
               // 317 are holes
               //    317$Net16           VIA        M      A00X+027370Y-044370X+390Y         S0       

               wipc_putchar_ipc(1,"317");     // here land, gets updates to drill if found
               wipc_set_opcode(true);    //indicating this is opcode
               wipc_putchar_ipc(4,netname14); // netname 14 char max. if longer use NNAME
               wipc_putchar_ipc(21,"VIA");

               double drill = block->getToolSize() * scale;

               if (drill > 0)
               {
                  CString diam;
                  diam.Format("%4d",conv_2_ipc(drill));  
                  wipc_putchar_ipc(34,diam); // diam format
                  wipc_putchar_ipc(38, block->getToolHolePlated() ? "P" : "U");
               }

               wipc_putchar_ipc(39,"A00");
               
               CString koo; // +- 6 digit, 
               koo.Format("X%+07dY%+07d", conv_2_ipc(point.x), conv_2_ipc(point.y));

               wipc_putchar_ipc(42,koo);

               koo.Format("X%04dY",conv_2_ipc(drill));
               wipc_putchar_ipc(58,koo);
      
               wipc_write_ipcline();
            }
         }
         break;
      }
   }
}

/******************************************************************************
* IPC_WriteData
*/
void IPC_WriteData(CDataList *DataList,  double insert_x, double insert_y, double rotation, int mirror, double scale, int embeddedLevel,
      int insertLayer, double unitScale) // unitScale only used for apertures because they are not scaled
{
   Mat2x2      m;
   Point2      point2;
   int         layer;
   long        entcnt = 0;
   POSITION    pos;
   DataStruct  *np;
   int         neg = FALSE;

   RotMat2(&m, rotation);

   pos = DataList->GetHeadPosition();
   while (pos != NULL)
   {
      np = DataList->GetNext(pos);
        entcnt++;
      // only draw visible layer (Inserts do not have layers)
      if (np->getDataType() != T_INSERT)
      {
         if (np->getLayerIndex() == -1)
            layer = insertLayer;
         else
            layer = np->getLayerIndex();

         if (insertLayer == -2)   // this is the non layer for the
                                 // complex aperture definition
         {
            // ipclib should not write a layer in its entities
            layer = -2;
         }
         else
         {
            if (!doc->get_layer_visible(layer, mirror))
              continue;
         }
         wipc_Graph_Layer(layer);
         if (np->isNegative() && !neg)  // if entity is negative, but system
                // not in negative mode
         {
            wipc_negativelayer(0);
            neg = TRUE;
         }
         else
         if (!np->isNegative() && neg)
         {
            wipc_positivelayer(0);
            neg = FALSE;
         }
      }

      // inserts have no initial layers
      switch(np->getDataType())
      {
         case T_POLY:
         {
            CPoly *poly;
            CPnt  *pnt;
            POSITION polyPos, pntPos;
            double   lineWidth;
            Point2 *points;
            int    cnt = 0;

            // count thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               pntPos = poly->getPntList().GetHeadPosition();
               while (pntPos != NULL)
               {
                  pnt = poly->getPntList().GetNext(pntPos);
                  cnt++;
               }
            }  // while

            points = (Point2 *)calloc(cnt, sizeof(Point2)); // entities+1 because num of points = num of segments +1
            // loop thru polys
            polyPos = np->getPolyList()->GetHeadPosition();
            while (polyPos != NULL)
            {
               poly = np->getPolyList()->GetNext(polyPos);
               if (poly->isHidden()) continue;

               BOOL polyFilled = poly->isFilled();
               BOOL closed     = poly->isClosed();
               BOOL voided     = poly->isVoid();

               int dcode = doc->getWidthTable()[poly->getWidthIndex()]->getDcode();
               int apshape = doc->getWidthTable()[poly->getWidthIndex()]->getShape();

               lineWidth = doc->getWidthTable()[poly->getWidthIndex()]->getSizeA()*scale;
                     
               double cx, cy, radius;
               if (PolyIsCircle(poly, &cx, &cy, &radius)) // circle
               {
                  Point2   center;
                  center.x = cx * scale;
                  center.y = cy * scale;
                  TransPoint2(&center, 1, &m, insert_x, insert_y);
                  wipc_Circle(center, radius*scale, lineWidth, polyFilled);
               }
               else
               {
                  Point2 p2;
                  cnt = 0;
                  pntPos = poly->getPntList().GetHeadPosition();
                  while (pntPos != NULL)
                  {
                     pnt = poly->getPntList().GetNext(pntPos);
                     p2.x = pnt->x * scale;
                     if (mirror) p2.x = -p2.x;
                     p2.y = pnt->y * scale;
                     p2.bulge = pnt->bulge;
                     if (mirror) p2.bulge = -p2.bulge;
                     TransPoint2(&p2, 1, &m, insert_x, insert_y);
                     // here deal with bulge
                     points[cnt] = p2;
                     cnt++;
                  }

                  wipc_Graph_Polyline(points,cnt, dcode, apshape, lineWidth, polyFilled);
               }
            }  // while
            free(points);
         }
         break;  // POLYSTRUCT

         case T_TEXT:
         {
            double text_rot;
            point2.x = (np->getText()->getPnt().x)*scale;
            point2.y = (np->getText()->getPnt().y)*scale;

            if (mirror)
            {
               text_rot = rotation - np->getText()->getRotation();
               point2.x = -point2.x;
            }
            else
               text_rot = rotation + np->getText()->getRotation();

            TransPoint2(&point2, 1, &m,insert_x,insert_y);

            Mat2x2   m1;
            RotMat2(&m1, text_rot);

            int text_mirror = np->getText()->getResultantMirror(mirror);

            wipc_Graph_Text( np->getText()->getText(), point2.x, point2.y,
                      np->getText()->getHeight()*scale,
                      np->getText()->getWidth()*scale,
                      doc->getWidthTable()[np->getText()->getPenWidthIndex()]->getSizeA()*scale,
                      np->getText()->getRotation(), text_mirror, 
                      np->getText()->getOblique() );
         }  // for case text
         break;
         case T_INSERT:
            // EXPLODEALL - call DoWriteData() recursively to write embedded entities
            point2.x = np->getInsert()->getOriginX() * scale;
            if (mirror) point2.x = -point2.x;
            point2.y = np->getInsert()->getOriginY() * scale;
            TransPoint2(&point2, 1, &m, insert_x, insert_y);

            // insert if mirror is either global mirror or block_mirror, but not if both.
            int block_mirror = mirror ^ np->getInsert()->getMirrorFlags();
            double block_rot;

            if (mirror)
               block_rot = rotation - np->getInsert()->getAngle();
            else
               block_rot = rotation + np->getInsert()->getAngle();

            BlockStruct *block = doc->Find_Block_by_Num(np->getInsert()->getBlockNumber());
            if ( (block->getFlags() & BL_APERTURE) || (block->getFlags() & BL_BLOCK_APERTURE) )
            {
               layer = Get_ApertureLayer(doc, np, block, insertLayer);

               if ( doc->get_layer_visible(layer, mirror)) 
                  wipc_Flash_App(point2, block->getDcode(), block->getShape(),layer,block->getSizeA()*scale);
            }
            else
            {
               int block_layer = -1;
               if (insertLayer != -1)
                  block_layer = insertLayer;
               else if (np->getLayerIndex() != -1)
                  block_layer = np->getLayerIndex();

               IPC_WriteData(&(block->getDataList()),
                     point2.x,
                     point2.y,
                     block_rot,
                     block_mirror,
                     scale * np->getInsert()->getScale(),
                     embeddedLevel+1, block_layer, unitScale);
               // here write out attribute
               write_attributes(np->getAttributesRef(),
                        point2.x, point2.y,
                        block_rot, block_mirror, scale * np->getInsert()->getScale());

            }
         break;
      } // end switch
   } // end for

   if (neg)
   {
      wipc_positivelayer(0);
   }
}

//-----------------------------------------------------------------------------
// CIPCPadstack
//-----------------------------------------------------------------------------
CIPCPadstack::CIPCPadstack(const CString name)
{
   m_name = name;
   m_type = 0;
   m_shapeTypeTop = 0;
   m_shapeTypeBottom = 0;
   m_drillSize = 0;
   m_drillPlated = false;
   m_xSizeTop = m_xSizeBottom = 0;
   m_ySizeTop = m_ySizeBottom = 0;
   m_xOffsetTop = m_xOffsetBottom = 0;
   m_yOffsetTop = m_yOffsetBottom = 0;
   m_rotationTop = m_rotationBottom = 0;
   m_topMostLayerNum = m_bottomMostLayerNum = -98;
}

CIPCPadstack::~CIPCPadstack()
{
}

//-----------------------------------------------------------------------------
// CIPCPadstackMap
//-----------------------------------------------------------------------------
CIPCPadstackMap::CIPCPadstackMap()
{
   empty();
}

CIPCPadstackMap::~CIPCPadstackMap()
{
   empty();
}

void CIPCPadstackMap::empty()
{
   m_padstackMap.empty();
}

CIPCPadstack* CIPCPadstackMap::addPadstack(BlockStruct* block)
{
   if (block == NULL)
      return NULL;

   CIPCPadstack* padstack = findPadstack(block->getName());
   if (padstack ==  NULL)
   {
      padstack = new CIPCPadstack(block->getName());
      m_padstackMap.SetAt(block->getName(), padstack);
   }

   return padstack;
}
 
CIPCPadstack* CIPCPadstackMap::findPadstack(const CString name)
{
   CIPCPadstack* padstack = NULL;
   m_padstackMap.Lookup(name, padstack);
   return padstack;
}
