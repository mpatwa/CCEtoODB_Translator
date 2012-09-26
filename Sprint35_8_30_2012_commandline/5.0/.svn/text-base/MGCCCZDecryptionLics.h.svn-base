/**********************************************
  Copyright Mentor Graphics Corporation 2010

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

// MGCCCZDecryptionLics.h
// Author Rick Faltersack

// Based on MGCPCBAutomationLics.h
// Author Toby Rimes

/////////////////////////////
// DO NOT CHANGE THIS FILE //
/////////////////////////////

// This file is used to allow CAMCAD API clients to access CCZ (CCE) Decryption.

// Example validation handshake code
//
// CCZData cczDataObj;
// long seed;
// cczDataObj.Validate(0, &seed);
// cczDataObj.Validate(GetMGCCCZDecryptionKey(seed), &seed);


int GetMGCCCZDecryptionKey(int num)
{
    int key;

    key = (int)( ( (num * 1.63) + 532 ) / 24 );

    // swap words
    int high = key << 16;
    int low = key >> 16;
    key = high | low;

    return key;
}
