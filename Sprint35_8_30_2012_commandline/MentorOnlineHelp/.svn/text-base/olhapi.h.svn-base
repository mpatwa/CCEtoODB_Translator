/* olhapi.h
 *
 * Copyright 1997-2005 Mentor Graphics Corporation
 *
 *    All Rights Reserved.
 *
 * THIS WORK CONTAINS TRADE SECRET
 * AND PROPRIETARY INFORMATION WHICH IS THE
 * PROPERTY OF MENTOR GRAPHICS
 * CORPORATION OR ITS LICENSORS AND IS
 * SUBJECT TO LICENSE TERMS. 
 */
#ifndef _INCLUDED_OLHAPI
#define _INCLUDED_OLHAPI

/* online help function return values */
typedef enum {
   OLH_OK               = 0,	/* everything's fine */
   OLH_FAILED           = 2,	/* operation could not be performed */
   OLH_INVALID_PARAM    = 3,	/* invalid parameter */
   OLH_DOC_NOT_FOUND    = 4,	/* document not found */
   OLH_OPEN_FAIL        = 5,	/* unable to open document */
   OLH_TAG_NOT_FOUND    = 6,	/* tag not in document */
   OLH_VIEWER_NOT_FOUND = 7,	/* viewer not found */
   OLH_TEXT_NOT_FOUND   = 8,	/* text not found in document */
   OLH_INVALID_PAGE     = 9,	/* page does not exist in document */

   /* DEPRECATED, only supported with the old, illegal Acrobat plug-in */
   OLH_COMM_FAILED      = 1,	/* communication failure */
   OLH_PLUGIN_NOT_FOUND = 10	/* communication plugin not found */
} OlhStatus;

typedef enum {
   OLH_FALSE  = 0,    /* false */
   OLH_TRUE   = 1     /* true */
} OlhBool;

#define MAXURLLEN 2048

#if defined(_WIN32)
#if !defined(OLH_DLL_ATTRIBUTE)
/* clients import from the olh dll */
#define OLH_DLL_ATTRIBUTE __declspec(dllimport)
#endif 
#else
#define OLH_DLL_ATTRIBUTE /* Unused in UNIX */
#endif

#ifdef __cplusplus
extern  "C" {
#endif

/* supported on line help viewers */
#define VIEWER_COUNT 2
typedef enum {
   OLH_VIEWER_PDF      =  0,	/* PDF documentation */
   OLH_VIEWER_HTML     =  1,	/* HTML documentation */

   /* special viewer types (NOT included in VIEWER_COUNT) */
   OLH_VIEWER_DEFAULT  = -1,	/* use default viewer */
   OLH_VIEWER_INFOHUB  = -2,    /* OlhInfoHub automatically sets viewer to this */

   /* DEPRECATED (duplicates, not included in VIEWER_COUNT) */
   OLH_VIEWER_ACROBAT  =  0,	/* replaced by OLH_VIEWER_PDF */
   OLH_VIEWER_NETSCAPE =  1		/* replaced by OLH_VIEWER_HTML */

   /* possible future development
   OLH_VIEWER_JAVAHELP =  2,
   OLH_VIEWER_CHM      =  3,
   */
} OlhViewer;

/* supported display types */
typedef enum {
   OLH_DISPLAY_NORMAL  = 0,     /* display normal window with navigation and TOC */
   OLH_DISPLAY_TOPIC   = 1,		/* display single topic with navigation only */
   OLH_DISPLAY_POPUP   = 2      /* display popup of context-sensitive information only */
} OlhDisplayType;

/*************************************************************************
 * OlhHelpCall
 *
 * Description:
 *   Open document specified by 'handle' to indicated 'topic' and display
 *   in manner indicated by 'type'. Return OLH_OK on success.
 *
 * Arguments:
 *   handle   - Name of help document to open. Should be located in
 *              documentation location, otherwise the full path needs
 *              to be specified.
 *   topic    - Topic within document to be displayed. May be NULL or
 *              empty string, indicating to open to first page of document.
 *   type     - OlhDisplayType to use. Currently only affects HTML
 *              documentation. If NULL, OLH_DISPLAY_NORMAL is used.
 *
 * Return Value:
 *   OLH_OK on success, or more specific OlhStatus value if failed.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhHelpCall( const char*    handle,
             const char*    topic,
             OlhDisplayType type );

/*************************************************************************
 * OlhHelpTest
 *
 * Description:
 *   Test for existence of document indicated by 'handle' that contains
 *   the specified 'topic'.
 *
 * Arguments:
 *   handle   - Name of help document to open. Should be located in
 *              documentation location, otherwise the full path needs
 *              to be specified.
 *   topic    - Topic within document to be displayed. May be NULL or
 *              empty string, indicating to open to first page of document.
 *
 * Return Value:
 *   OLH_OK on success, or more specific OlhStatus value if failed.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhHelpTest( const char*    handle,
             const char*    topic );

/*************************************************************************
 * OlhInfoHub
 *
 * Description:
 *   Open InfoHub specified by 'handle' with the specified 'tab' showing
 *   by default.
 *
 * Arguments:
 *   handle   - Name of help document to open. Should be located in
 *              documentation location, otherwise the full path needs
 *              to be specified.
 *   tab      - Tab within InfoHub to be displayed. May be NULL or
 *              empty string, indicating to open default tab.
 *
 * Return Value:
 *   OLH_OK on success, or more specific OlhStatus value if failed.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhInfoHub( const char*    handle,
            const char*    tab );

/*************************************************************************
 * OlhSetViewer
 *
 * Description:
 *   Makes specified viewer the primary viewer by making it the
 *   first viewer to try when performing a help call.  Remaining
 *   viewers are shifted down in the order to accomodate.
 *   If OLH_VIEWER_DEFAULT is specified, the viewer order will
 *   revert back to the default.
 *
 * Arguments:
 *   viewer   - The OlhViewer that shall become the primary viewer.
 *   startit  - If non-zero, the viewer will also be invoked.
 *
 * Return Value:
 *   OLH_OK on success, other OlhStatus value otherwise.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhSetViewer( OlhViewer viewer,
			  int       start_viewer );

/*************************************************************************
 * OlhSetViewerPath
 *
 * Description:
 *   Set the full path or filename (if located in $PATH) of
 *   the specified viewer's executable.
 *
 * Arguments:
 *   viewer    - OlhViewer for which to set the executable path.
 *   path      - Full path or filename of viewer executable.
 *
 * Return Value:
 *   OLH_OK on success, other OlhStatus value otherwise.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhSetViewerPath( OlhViewer   viewer,
                  const char* path );

/*************************************************************************
 * OlhIsViewerRunning
 *
 * Description:
 *   Determines if the specified viewer is running.  This allows the user
 *   interface to tell the user that a slight delay may occur while the
 *   viewer is starting up.
 *
 * Arguments:
 *   viewer     - The viewer to check.
 *
 * Return Value:
 *   0 if selected viewer is NOT running, non-zero otherwise.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE int
OlhIsViewerRunning( OlhViewer viewer );

/*************************************************************************
 * OlhGetDocPath
 *
 * Description:
 *   Stores the full path to the documentation directory in the
 *   buffer pointed to by 'rtn_docpath'.  This buffer should be of
 *   size MAXPATHLEN, or greater.
 *
 * Arguments:
 *   rtn_docpath  - Buffer to copy full doc path into.
 *
 * Return Value:
 *   0 if successful, non-zero otherwise.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE int
OlhGetDocPath( char* rtn_docpath );

/*************************************************************************
 * OlhSetViewerOrder
 *
 * Description:
 *   Set the order that a help call uses for displaying
 *   documentation.  The order is specified with a string
 *   of colon-separated values.  Currently, the valid
 *   values are "pdf", "html", and "htm" (case-insensitive).
 *   If more OlhViewers become supported in the future,
 *   this list will grow.
 *
 *   Not all doc types need to be specified in the order string.
 *   Those specified will simply be added to the beginning of
 *   the list.  Specifying a single type will produce the same
 *   result as calling OlhSetViewer directly.
 *
 * Arguments:
 *   order   - String specifying the order to use.
 *
 * Return Value:
 *   OLH_OK on success, other OlhStatus value otherwise.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhSetViewerOrder( const char* order );

/*************************************************************************
 * OlhDocGotoTag
 *
 * Description:
 *   Provided for legacy support, wraps OlhHelpCall().
 *   Passes handle and tag args directly, and sets
 *   display type to OLH_DISPLAY_NORMAL.
 *
 * Arguments:
 *   doc      - Name of help document to open. Should be located in
 *              documentation location, otherwise the full path needs
 *              to be specified.
 *   tag      - Tag to position document at. May be NULL or empty string,
 *              indicating to open to first page of document.
 *
 * Return Value:
 *   OLH_OK on success, or more specific OlhStatus value if failed.
 *************************************************************************/
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhDocGotoTag( const char* doc,
               const char* tag );


/********************************************************************
 * THE FOLLOWING API FUNCTIONS ARE SUPPORTED BY THE ACROBAT READER
 * ONLY. IF YOU USE THESE YOU MAY HAVE TROUBLE MIGRATING TO HTML HELP
 * AT A LATER DATE.
 ********************************************************************/

/* close all documents and shut down document viewer */
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhAppExit( void );

/* This function allows client to adjust the timeout value used by
 * the interface when attempting to determine if Acroread is up & running.
 * The default value is 1/2 second, but this may be too short for some
 * applications on some machines. If you see OlhIsViewerRunning returning
 * false when the viewer is really up, try increasing the timeout value.
 * Parameters to this function are the number of seconds and microseconds
 * to wait for a communication reply before assuming the reader is not running.
 */
extern OLH_DLL_ATTRIBUTE void
OlhAcroSetTimeout( unsigned long sec,
				   long          usec);

/* Open/Position document at indicated page. If document is already open this
 * call does not open a second view of the document but simply uses the
 * already open view.
 */
extern OLH_DLL_ATTRIBUTE OlhStatus
OlhDocGotoPage( const char* doc,
                long        page );

#ifdef __cplusplus
/* Close for extern "C" */
}
#endif

#endif
