// $Header: /CAMCAD/4.4/Dongle.cpp 9     2/26/04 5:23p Alvin $

//#include "stdafx.h"
//#include "spromeps.h"
//#include "mainfrm.h"
//
//#define DEVELOPER_ID         0x6075 
//#define WRITE_PASSWORD       0x3A3E
//#define OVERWRITE_PASSWORD_1 0xC8A8
//#define OVERWRITE_PASSWORD_2 0x2B2B
//
//BOOL dongle();
//
//static UINT timer = 0;
//
//void StartDongleTimer() 
//{
//   if (!timer)
//      timer = ((CMainFrame*)AfxGetMainWnd())->SetTimer(3, 10000, 0);
//}
//
//void KillDongleTimer() 
//{
//   if (timer)
//      ((CMainFrame*)AfxGetMainWnd())->KillTimer(timer);
//   timer = 0;
//}
//
//void CMainFrame::OnTimer(UINT nIDEvent) 
//{
//   if (nIDEvent == 3)
//   {
//      while (!dongle())
//      {
//         if (timer)
//            KillDongleTimer();
//         ErrorMessage("You need a security key to continue.");
//      }
//
//      if (!timer)
//         StartDongleTimer();
//   }
//   
//   CMDIFrameWnd::OnTimer(nIDEvent);
//}
//
//BOOL dongle()
//{
//   /* SuperPro API variables */
//   RB_SPRO_APIPACKET ApiPacket;
//   SP_STATUS         spStatus;
//   RB_WORD           developerID;
//   RB_WORD           cellAddress;
//   RB_WORD           queryLength;
//   RB_BYTE           queryData[SPRO_MAX_QUERY_SIZE];
//   RB_BYTE           queryResponse[SPRO_MAX_QUERY_SIZE];
//   RB_DWORD          query32Response;
//
//   /* output the function being executed */
//
////   RBIOoutputTextString( "Starting Demo." );
//
//   /* Start by initialzation the driver */
//
//   spStatus = RNBOsproFormatPacket( ApiPacket, sizeof(ApiPacket) );
//
//   if ( 0 != spStatus)
//   {
////      RBIOoutputTextString( "SproFormatPacket failed!" );
//      return FALSE;
//   }
//
//   spStatus = RNBOsproInitialize( ApiPacket );
//
//   if ( 0 != spStatus)
//   {
////      RBIOoutputTextString( "SproInitialize failed!" );
//      return FALSE;
//   }
//
////   RBIOoutputTextString( "SproInitialize successful." );
//
//   /* Next we call sproFindFirstUnit to locate a specific    */
//   /* Developer's SuperPro key use the hardcoded developer   */
//   /* ID for a demo. Refer to your password sheet (shipped   */
//   /* with the evaluation kit) for your particular           */
//   /* developerID number                                     */
//
//   developerID = DEVELOPER_ID;
//
//   spStatus = RNBOsproFindFirstUnit( ApiPacket, developerID );
//
//   if ( 0 != spStatus )
//   {
////      RBIOoutputTextString( "SproFindFirstUnit failed!" );
//      return FALSE;
//   }
////   RBIOoutputTextString( "SproFindFirstUnit successful." );
//
//   /* Next we will demo the sproQuery function. Calls to this function */
//   /* form the heart of the security of the SuperPro system, due to    */
//   /* the secret nature of the SuperPro bit scrambler in the hardware  */
//   /* key.                                                             */
//
//   /* We will use a hardcoded value for cell address, query input      */
//   /* data, and query length You will want to configure your           */
//   /* protection scheme to use different SuperPro cells and many       */
//   /* different query input/response pairs.                            */
//
//   cellAddress = 8; // CAMCAD
//
//   queryData[0] = 0x68;
//   queryData[1] = 0x4f;
//   queryData[2] = 0xb6;
//   queryData[3] = 0x52;
//   queryLength = 4;
//   
//   /* Query the algo at the hardcoded CellAddress  */
//
//   /* Get Buffer response value */
//   spStatus = RNBOsproQuery( ApiPacket,
//                             cellAddress,
//                             queryData,
//                             queryResponse,
//                             &query32Response,
//                             queryLength );
//
//   /* display the query input data and query output data via the */
//   /* I/O engine                                                 */
//
//   if ( 0 != spStatus )
//   {
//      // does not have a key
////      RBIOoutputTextString( "SproQuery failed!" );
//      return FALSE;
//   }
////   RBIOoutputTextString( "SproQuery successful." );
//
////   RBIOoutputTextString( "Ending Demo." );
//
//   unsigned short a, b, c, d;
//   a = queryResponse[0]; // f1
//   b = queryResponse[1]; // fa
//   c = queryResponse[2]; // 16
//   d = queryResponse[3]; // fd
//
//   if (a != 0xf1 && b != 0xfa && c != 0x16 && d != 0xfd)
//   {
//      // key not programmed
//      return FALSE;
//   }
//   
//   return TRUE;
//} 


