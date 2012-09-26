/**********************************************
Copyright Mentor Graphics Corporation 2011

All Rights Reserved.

THIS WORK CONTAINS TRADE SECRET
AND PROPRIETARY INFORMATION WHICH IS THE
PROPERTY OF MENTOR GRAPHICS
CORPORATION OR ITS LICENSORS AND IS
SUBJECT TO LICENSE TERMS. 
**********************************************/

#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

#include "libxml/tree.h"
#include "InfrasecUtility.h"
#include <fstream>
#include <sstream>

/******************************************************************************
* CEncryption
This class is responsible to Encrypt and Decrypt the CC data 
*/
class CEncryption
{
public:
    CEncryption(){};
    ~CEncryption(){};

    //Encrypt the given stringstream and write encrypted cc to the given filepath
    bool EncryptData(const char* filePath, std::stringstream &ssToEncrypt);

    //Decrypt the given file and return the decrypted ccdata stringstream
    bool DecryptData(const char* filePath, std::stringstream &decryptedSS);

protected:
    //Get the node to be encrypted from using the Doc and buffer
    bool CreateDoc(xmlDocPtr* _pDoc ,xmlNodePtr* _pNode, std::stringstream &ssToEncrypt);

    //Parse the root node for the CDATA section and return the decrypted CC stream
    bool ParseCDATASection(xmlNodePtr pNode, const xmlChar** content);

private:
    CInfrasecUtility utilPtr;
};

#endif //_ENCRYPTION_H_
