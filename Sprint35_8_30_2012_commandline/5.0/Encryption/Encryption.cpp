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

#include "Encryption.h"
#include "ZipUtility.h"

// These header is used to load the encrypted key data, and to register
// it with the XML Sec library. Then keys can be just looked up by identifier
// (name) when needed.
#include "infra_sec/SDDinfrasec_xmlSecKeysMngr.h"

// Definitions for the Values used in the XML
#define XML_VERSION     "1.0"
#define XML_ENCODING    "ISO-8859-1"
#define ROOT_NODE       "CCZEncrypt"
#define DUMMY_NODE      "ActualEncryptedNode"

bool CEncryption::CreateDoc(xmlDocPtr* _pDoc ,xmlNodePtr* _pNode, std::stringstream &ssToEncrypt)
{
    xmlDocPtr   doc      = NULL;
    xmlNodePtr  root     = NULL;
    xmlNodePtr  cdataPtr = NULL;
    xmlNodePtr  encNode  = NULL;

    // First ensure that the return value is initialized.
    *_pDoc = NULL;
    *_pNode = NULL;

    // Create the Document Node
    doc = xmlNewDoc(BAD_CAST XML_VERSION);
    if (NULL == doc)
        return false;

    // Now, create the root node for the doc.
    root = xmlNewDocNode(doc, NULL, BAD_CAST ROOT_NODE, NULL);
    xmlDocSetRootElement(doc, root);

    //Add stream to a CDATA Section
    std::string str(ssToEncrypt.str());
    const char* pChar = str.c_str();
    const xmlChar* data = BAD_CAST pChar;

    int nSize = xmlStrlen( data );

    //Add the CDATA Section to the root node
    cdataPtr = xmlNewCDataBlock( doc, data, nSize );
    xmlAddChild( root, cdataPtr );

    *_pDoc  = doc;
    *_pNode = root;

    return true;
}

//Encrypt the given stringstream and write encrypted cc to the given file
bool CEncryption::EncryptData(const char* filePath, std::stringstream &ssToEncrypt)
{
    bool                retVal      = false;
    xmlDocPtr           doc         = NULL;
    xmlNodePtr          toBeEncNode = NULL;

    //Initialize the key manager
    retVal= utilPtr.InitializeKeyManager();
    if (false == retVal) {
        return retVal;
    }

    std::stringstream ssCompressed;
    DeflateSStream(ssToEncrypt, ssCompressed);

    //Get the node to be encrypted from using the Doc and buffer
    retVal = CreateDoc(&doc, &toBeEncNode, ssCompressed);
    if (false == retVal) {
        //Free the doc
        xmlFreeDoc(doc);
        doc = NULL;
        return retVal;
    }

    //Encrypt the doc
    retVal = utilPtr.EncryptXMLNode(doc, toBeEncNode);
    if (true == retVal)
    {
        //  We can possibly set compression on the xmlDoc as well... but this requires
        //  changes in CamCadFileReader which reads the first few bytes
        // doc->compression = 6; // Z_DEFAULT_COMPRESSION

        // Encryption successful, write the encrypted file.
        xmlSaveFormatFile( filePath, doc, 1);
    }

    //Free the doc
    xmlFreeDoc(doc);
    doc = NULL;

    //Clean up the key manager
    utilPtr.CleanupKeyManager();

    return retVal;
}

//Decrypt the given file and return the decrypted cdata stringstream
bool CEncryption::DecryptData(const char* filePath, std::stringstream &decryptedSS)
{
    xmlDocPtr   doc      = NULL;
    bool        retVal   = false;

    //Initialize the key manager
    retVal= utilPtr.InitializeKeyManager();
    if (false == retVal) {
        return retVal;
    }

    //Read the file to be decrypted
    xmlInitParser();

    //Read the file into the doc
    doc = xmlReadFile( filePath, NULL, XML_PARSE_HUGE );
    if ( !doc )
         return false;

    // Find the encrypted node and decrypt it(In place decryption).
    if (false == utilPtr.DecryptXMLNode(doc))
        return false;

    // Get the CCEncryptedDoc node(The doc is decrypted at this point)
    xmlNodePtr pRootNode = xmlDocGetRootElement( doc );
    if ( !pRootNode )
    {
        xmlFreeDoc(doc);
        doc = NULL;
        xmlCleanupParser();
        return false;
    }
    
    //Search for the Root node for CDATA Section
    //and get the cc data out of it
    const xmlChar* ccData = NULL;
    ParseCDATASection(pRootNode->children, &ccData);

    //Get the stringstream from the ccData ptr
    std::stringstream ssCompressed;
    if(ccData != NULL)
    {
        char * ccCharData = (char *) ccData;
        ssCompressed << ccCharData;
    }

    // Uncompress the decrypted data
    InflateSStream(ssCompressed, decryptedSS);

	// Miten -- At this point we have the xml node in decrypted form 
	// Miten -- NOTE : It has many random characters at the end of the xml tag and that is in variable "decryptedSS" 

    //Free the doc
    xmlFreeDoc(doc);
    doc = NULL;

    //Clean up the key manager
    utilPtr.CleanupKeyManager();

    return true;
}

//Parse the root node for the CDATA section and return the decrypted CC stream
bool CEncryption::ParseCDATASection(xmlNodePtr pNode, const xmlChar** content)
{
    xmlNodePtr pCurNode;
    const xmlChar* tempcontent = NULL;
    bool cdataNodeFound = false;
    for (pCurNode = pNode; pCurNode; pCurNode = pCurNode->next)
    {
        if ( pCurNode->type == XML_CDATA_SECTION_NODE )
        {
            cdataNodeFound = true;
            tempcontent = pCurNode->content;
            break;
        }
    }
    *content = tempcontent;
    return cdataNodeFound;
}
