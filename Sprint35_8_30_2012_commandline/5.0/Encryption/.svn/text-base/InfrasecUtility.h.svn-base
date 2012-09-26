/**********************************************
Copyright Mentor Graphics Corporation 2011

All Rights Reserved.

THIS WORK CONTAINS TRADE SECRET
AND PROPRIETARY INFORMATION WHICH IS THE
PROPERTY OF MENTOR GRAPHICS
CORPORATION OR ITS LICENSORS AND IS
SUBJECT TO LICENSE TERMS. 
**********************************************/

#ifndef _INFRASECUTILITY_H_
#define _INFRASECUTILITY_H_

// Don't use XSLT
#ifndef XMLSEC_NO_XSLT
#define XMLSEC_NO_XSLT
#endif

// Configure the XML Sec environment to use OpenSSL
#ifndef XMLSEC_CRYPTO_OPENSSL
#define XMLSEC_CRYPTO_OPENSSL
#endif
#ifndef XMLSEC_CRYPTO
#define XMLSEC_CRYPTO           openssl
#endif

#include <assert.h>
#include "libxml/tree.h"
#include "xmlsec/xmlsec.h"

class CInfrasecUtility
{
private:
    xmlSecKeysMngrPtr   keysMngr;
    const xmlChar*      keyName;

public:
    CInfrasecUtility()
    {
        keysMngr = NULL;
        keyName = BAD_CAST "CczKey1";
    }
    ~CInfrasecUtility(){}

    //Set and get for the keyname
    void SetKeyName(const xmlChar* _keyName){ keyName = _keyName;}
    const xmlChar* GetKeyName(){ return keyName;}

    /////////////////////////////////////////////////////////////////////////
    // InitializeKeyManager:
    // Initialize Key manager
    // Load the keys, and create the Keys Manager.
    // This is the helper function from the SDDInfrasec library that takes
    // a packed, encrypted key buffer, and uses it to initialize an XML Sec
    // Keys Manager.
    /////////////////////////////////////////////////////////////////////////
    bool InitializeKeyManager();

    /////////////////////////////////////////////////////////////////////////
    // CleanupKeyManager:
    // All the clean up code for the Keys Manager goes here.
    /////////////////////////////////////////////////////////////////////////
    void CleanupKeyManager();

    /////////////////////////////////////////////////////////////////////////
    // CleanupSec:
    // All the clean up code for Sec goes here.
    /////////////////////////////////////////////////////////////////////////
    static void CleanupSec();           // Call once, if at all

    /////////////////////////////////////////////////////////////////////////
    // InitializeXMLSec:
    // Initialize the XML environment, including XML Sec.
    static bool InitializeXMLSec();     // Call once

    //// This routine performs the actual encryption of the character data.
    //// Encryption is performed as a 2-step process. First, it looks up the node
    //// by name, and then it encrypts it.
    /////////////////////////////////////////////////////////////////////////
    bool EncryptXMLNode( xmlDocPtr doc, xmlNodePtr node);

    /////////////////////////////////////////////////////////////////////////
    // DecryptXMLNode:
    // @_doc:               The XML document to use as the container for
    //                      this node.
    // @_type:              The type of data that the encrypted data node
    //                      will hold.
    // @_keyName:           The Name of the Key to use for the encryption.
    // @_encDataNode:       The resulting EncryptedData XML Node
    //
    // This function is used to create an EncryptedData element according
    // to the standards for SDD XML Encryption.
    // Those standards specify that:
    //  . The node will specify the encryption method (?)
    //  . The node will reference a "Key Name" so that the reader code can
    //    find the key properly.
    //  . The node will have a CipherValue child node. Note: this also means
    //    that CipherReference is NOT supported. All encrypted data will be
    //    in the XML file directly.
    //
    // Note: This will create an XML Node, that must be freed by the caller.
    //
    /////////////////////////////////////////////////////////////////////////
    bool    DecryptXMLNode(xmlDocPtr           _doc);

    /////////////////////////////////////////////////////////////////////////
    // CreateEncryptedDataNode:
    // @_doc:               The XML document to use as the container for
    //                      this node.
    // @_type:              The type of data that the encrypted data node
    //                      will hold.
    // @_keyName:           The Name of the Key to use for the encryption.
    // @_encDataNode:       The resulting EncryptedData XML Node
    //
    // This function is used to create an EncryptedData element according
    // to the standards for SDD XML Encryption.
    // Those standards specify that:
    //  . The node will specify the encryption method (?)
    //  . The node will reference a "Key Name" so that the reader code can
    //    find the key properly.
    //  . The node will have a CipherValue child node. Note: this also means
    //    that CipherReference is NOT supported. All encrypted data will be
    //    in the XML file directly.
    //
    // Note: This will create an XML Node, that must be freed by the caller.
    //
    /////////////////////////////////////////////////////////////////////////
    bool    CreateEncryptedDataNode(xmlDocPtr       _doc,
                                    const xmlChar*  _type,
                                    xmlNodePtr*     _encDataNode);

    /////////////////////////////////////////////////////////////////////////
    // PurgeCRTextNodes:
    // @_node:              The node from which to remove CR Text Nodes.
    //
    // This function will recursively process the provided node and all of
    // its children to remove Text Nodes that are just CRs (carriage returns)
    // These nodes are added by the XML Sec library, and will turn off the
    // formatting support in the libXML library when writing out the XML data.
    // So, as a hack, these nodes are manually removed after creating the
    // EncryptedData and Signature nodes.
    //
    /////////////////////////////////////////////////////////////////////////
    void    PurgeCRTextNodes(xmlNodePtr _node);
};

#endif //_INFRASECUTILITY_H_
