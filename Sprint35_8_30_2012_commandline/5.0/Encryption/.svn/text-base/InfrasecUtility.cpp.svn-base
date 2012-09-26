/**********************************************
Copyright Mentor Graphics Corporation 2011

All Rights Reserved.

THIS WORK CONTAINS TRADE SECRET
AND PROPRIETARY INFORMATION WHICH IS THE
PROPERTY OF MENTOR GRAPHICS
CORPORATION OR ITS LICENSORS AND IS
SUBJECT TO LICENSE TERMS. 
**********************************************/


#include "StdAfx.h"

#include "InfrasecUtility.h"
#include "EncryptedKeyData.h"
#include "xmlsec/crypto.h"
#include "xmlsec/xmlenc.h"
#include "xmlsec/templates.h"
#include "xmlsec/xmltree.h"

// This header is used to load the encrypted key data, and to register
// it with the XML Sec library. Then keys can be just looked up by identifier
// (name) when needed.
#include "infra_sec/SDDinfrasec_xmlSecKeysMngr.h"


// static
bool CInfrasecUtility::InitializeXMLSec()
{
    // Run once
    static bool bInitialized = false;
    if( bInitialized )  return true;
    bInitialized = true;

    /* Init libxml and libxslt libraries */
    xmlInitParser();

    LIBXML_TEST_VERSION
    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);

    /* Init xmlsec library */
    if(xmlSecInit() < 0) {
        //"Error: xmlsec initialization failed.
        return false;
    }

    /* Check loaded library version */
    if(xmlSecCheckVersion() != 1) {
        //"Error: loaded xmlsec library version is not compatible.
        return false;
    }

    /* Load default crypto engine if we are supporting dynamic
    * loading for xmlsec-crypto libraries. Use the crypto library
    * name ("openssl", "nss", etc.) to load corresponding 
    * xmlsec-crypto library.
    */
#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
    if(xmlSecCryptoDLLoadLibrary(BAD_CAST XMLSEC_CRYPTO) < 0) {
        //Error: unable to load default xmlsec-crypto library. Make sure
        //that you have it installed and check shared libraries path
        //(LD_LIBRARY_PATH) envornment variable.
        return false;     
    }
#endif /* XMLSEC_CRYPTO_DYNAMIC_LOADING */

    /* Init crypto library */
    if(xmlSecCryptoAppInit(NULL) < 0) {
        //Error: crypto initialization failed
        return false;
    }

    /* Init xmlsec-crypto library */      
    if(xmlSecCryptoInit() < 0) {
        //Error: xmlsec-crypto initialization failed.
        return false;
    }

    return true;
}

bool CInfrasecUtility::EncryptXMLNode(xmlDocPtr _doc, xmlNodePtr _node)
{
    bool            retVal = true;
    bool            localRet = false;
    xmlNodePtr      encDataNode = NULL;
    xmlSecEncCtxPtr encCtx = NULL;

    localRet = CreateEncryptedDataNode(_doc, xmlSecTypeEncContent, &encDataNode);
    if (false == localRet)
        retVal = false;

    if (true == retVal) {
        // Create encryption context using the Keys Manager that was passed in.
        encCtx = xmlSecEncCtxCreate(keysMngr);
        if (NULL == encCtx) {
            //Error: failed to create encryption context;
            xmlFreeNode(encDataNode);   encDataNode = NULL;
            retVal = false;
        } else {
            // The following assignment defines the encryption algorithm, 
            // that will be used if there isn't an EncryptionMethod element
            // defined for the EncryptedData node.
            encCtx->defEncMethodId = xmlSecTransformAes128CbcId;
        }
    }

    if ((NULL != encDataNode) && (NULL != encCtx)) {
        // Encrypt the data
        // The data will be encrypted using:
        //  . The algorithm determined either by:
        //      . The EncryptionMethod child element of the EncryptedData node
        //      . The defEncMethodId value of the Encryption Context.
        //  . The key determined either by:
        //      . A key explicitly assigned to the Encryption Context (encKey).
        //      . The name in the KeyInfo field, looked up in the Keys Manager
        //          assigned to the Encryption Context.
        //      . The first key in the keys manager that meets the key
        //          requirements (determined by the algorithm above).
        // Since this encryption is done using: "xmlSecEncCtxXmlEncrypt()"
        // on the "ccNode" node, that node will be replaced with the 
        // EncryptedData node "encDataNode" after it is filled in.
        if (xmlSecEncCtxXmlEncrypt(encCtx, encDataNode, _node) < 0) {
            //Error: encryption failed.
            retVal = false;
        } else {
            // Set the Encrypted Data Node to NULL, since it is now owned by
            // the document, and will be cleaned up with the call to xmlFreeDoc().
            encDataNode = NULL;
        }
    }

    //done:
    // cleanup
    if (NULL != encCtx)         xmlSecEncCtxDestroy(encCtx);
    if (NULL != encDataNode)    xmlFreeNode(encDataNode);

    return retVal;
}

bool CInfrasecUtility::DecryptXMLNode(xmlDocPtr doc)
{
    xmlSecEncCtxPtr encCtx = NULL;

    // Now, that the encryption has passed successfully, decrypt the document.
    bool            nodeFound = false;
    xmlNodePtr      node      = NULL;

    // Loop through the document, looking for EncryptedData nodes.
    for (node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeEncryptedData, xmlSecEncNs);
        NULL != node;
        node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeEncryptedData, xmlSecEncNs)) {
            if (node != NULL) {
                nodeFound = true;
                break;
            }
    }

    //Node to be decrepted not found
    if(false == nodeFound)
        return false;

    // Sanity check the arguments.
    assert(NULL != doc);
    assert(NULL != node);
    assert(NULL != keysMngr);

    // Create encryption context. If this fails, just return, since
    // there is nothing that can be done, and no memory has been
    // allocated.
    encCtx = xmlSecEncCtxCreate(keysMngr);
    if (NULL == encCtx) {
        //Error: failed to create encryption context
        return false;
    }
    // The following assignment defines the encryption algorithm, 
    // that will be used if there isn't an EncryptionMethod element
    // defined for the EncryptedData node.
    encCtx->defEncMethodId = xmlSecTransformAes128CbcId;

    /* decrypt the data */
    int retcode = xmlSecEncCtxDecrypt(encCtx, node);
    if ((/*xmlSecEncCtxDecrypt(encCtx, node)*/retcode < 0) || (encCtx->result == NULL)) {
        //Error: decryption failed.
        xmlSecEncCtxDestroy(encCtx);
        return false;
    }
        
    /* print decrypted data to stdout */
    if (encCtx->resultReplaced == 0) {
        // It is not quite clear how one would get to this clause.
        // TODO: See how to get here.... try encrypting a binary file into
        // contents?
        if(xmlSecBufferGetData(encCtx->result) != NULL) {
            fwrite(xmlSecBufferGetData(encCtx->result), 
                  1, 
                  xmlSecBufferGetSize(encCtx->result),
                  stdout);
        }
    }
        
    /* cleanup */
    if (NULL != encCtx)     xmlSecEncCtxDestroy(encCtx);
    
    return true;
}

bool CInfrasecUtility::CreateEncryptedDataNode(xmlDocPtr _doc,
                                         const xmlChar* _type,
                                         xmlNodePtr*    _encDataNode)
{
    bool            retVal = true;
    xmlNodePtr      encDataNode = NULL;

    // Initialize the return value to NULL, if not already.
    if (NULL == _encDataNode) return false;
    *_encDataNode = NULL;

    // Create the <enc:EncryptedData> node. In XML Sec terminology, this is a
    // "template".
    // The encMethodId argument (2nd arg) signifies the encryption algorithm.
    // Defining the TransformId here also causes the function to generate an
    // EncryptionMethod child node under the EncryptedData Node.
    // That is not required by the standard, or by the code, as the "default
    // encryption method can be specified on the encryption context. This is
    // the technique used in this sample.
    // The xmlSecTypeEncElement signifies the type of contents of the
    // EncryptedData. This will be written as an attribute of the
    // EncryptedData node. It is used during the actual encryption when
    // calling "xmlSecEncCtxXmlEncrypt()" - this value triggers the function
    // to encrypt and replace the selected node (recursively, meaning all of
    // its children as well).
    // XMLSEC_EXPORT xmlNodePtr
    // xmlSecTmplEncDataCreate(xmlDocPtr doc,
    //                         xmlSecTransformId encMethodId,
    //                         const xmlChar *id,
    //                         const xmlChar *type,
    //                         const xmlChar *mimeType,
    //                         const xmlChar *encoding);
    //encDataNode = xmlSecTmplEncDataCreate(doc, xmlSecTransformAes128CbcId,
    //                                      NULL, xmlSecTypeEncElement, NULL, NULL);
    encDataNode = xmlSecTmplEncDataCreate(_doc, NULL, NULL, _type, NULL, NULL);
    if (NULL == encDataNode) {
        //Error: failed to create encryption template.
        retVal = false;
    }

    // Ensure that the <enc:CipherValue/> node is created and added to the
    // <enc:EncryptedData> node.
    if (NULL != encDataNode) {
        if(xmlSecTmplEncDataEnsureCipherValue(encDataNode) == NULL) {
            //Error: failed to add CipherValue node.
            retVal = false;
        }

        // If the caller specified a Key Name, then create and add the 
        // <dsig:KeyInfo/> and <dsig:KeyName/> nodes to the
        // <enc:EncryptedData> node.
        if (NULL != GetKeyName()) {
            // Create the Key Info node.
            xmlNodePtr keyInfoNode = xmlSecTmplEncDataEnsureKeyInfo(encDataNode, NULL);
            if (NULL == keyInfoNode) {
                //Error: failed to add key info.
                retVal = false;
            } else {
                // Add the Key Name to the Key Info Node.
                if (NULL == xmlSecTmplKeyInfoAddKeyName(keyInfoNode, GetKeyName())) {
                    //Error: failed to add key name.
                    retVal = false;
                }
            }
        }
    }

    // If there was an error along the way, free up the encDataNode.
    if (false == retVal) {
        xmlFreeNode(encDataNode);
        encDataNode = NULL;
    }

    // The following call will delete the TEXT_NODEs that are created by
    // XML Sec. These text nodes just have carriage returns in them, and 
    // end up defeating the formatting of the Dump Routine below.
    PurgeCRTextNodes(encDataNode);

    if (NULL != encDataNode)
        *_encDataNode = encDataNode;

    return retVal;
}

void CInfrasecUtility::PurgeCRTextNodes(xmlNodePtr _node)
{
    xmlNodePtr  curr = NULL;
    xmlNodePtr  thisNode = NULL;
    xmlChar     *textContent = NULL;

    // Don't process if the node is NULL.
    if (NULL == _node) return;

    // Process the node, and its siblings.
    // The loop increment is handled specially at the beginning of the block
    // to handle the fact the the current node might get deleted, and to 
    // ensure that each node is visited once and only once.
    for (curr = _node; NULL != curr; ) {
        // First, save off the current node, and increment for the next iteration.
        thisNode = curr;
        curr = curr->next;
        if (thisNode->type == XML_TEXT_NODE) {
            textContent = xmlNodeGetContent(thisNode);
            if (xmlStrEqual(textContent, xmlSecStringCR))
            {
                xmlNodeSetContent(thisNode, NULL);
                xmlUnlinkNode(thisNode);
                xmlFreeNode(thisNode);
                thisNode = NULL;
            }
            xmlFree(textContent);
        } else {
            PurgeCRTextNodes(thisNode->children);
        }
    }

    return;
}

bool CInfrasecUtility::InitializeKeyManager()
{
    if(keysMngr != NULL)
        return true;

    keysMngr = SDDInfrasec::sddisXmlSecKeysMngrCreate(key_data, sizeof(key_data));
    if (NULL == keysMngr) {
        return false;
    }
    return true;
}

void CInfrasecUtility::CleanupKeyManager()
{
    //Release the Keys Manager that was created before.
    if (NULL != keysMngr)
        xmlSecKeysMngrDestroy(keysMngr);
}

// static
void CInfrasecUtility::CleanupSec()
{
    // Shutdown the various libraries, in reverse order to their initialization.

    /* Shutdown xmlsec-crypto library */
    xmlSecCryptoShutdown();

    /* Shutdown crypto library */
    xmlSecCryptoAppShutdown();

    /* Shutdown xmlsec library */
    xmlSecShutdown();

    // This function is expected to be called at the end of the application.  Otherwise there can be crashes.
    xmlCleanupParser();  // Only an app should call this
}

