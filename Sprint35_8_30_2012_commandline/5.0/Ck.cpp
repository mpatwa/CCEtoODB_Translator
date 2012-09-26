// $Header: /CAMCAD/4.5/Ck.cpp 22    11/07/06 4:44p Rick Faltersack $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright © 1994-2001. All Rights Reserved.

   In reading the setup file:
   \ chars must be defined as \\

*/

#include "StdAfx.h"
#include "General.h"
#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define FREE(P)        ( P ? ( free( (char *) P), P=NULL ) : 0 )

#define MIN(A,B)       ( ( (A) < (B) ) ? (A) : (B) )

// These macros allows clean error trapping without multiple
// return statments and fewer nested if/else's.
//
#define ERR_IF(EXPR,N)  if ( EXPR ) { strcpy(ErrFile,__FILE__), ErrLine=__LINE__, LastErr=N; goto Exit; }

/************************
** Linked-list Macros  **
************************/

#define DEL( ObjType, Node, Info, Count ) \
   ( (ObjType **) DelNode( (NODE_ **) Node, Info, Count ) )

#define NEXT(ppNODE_)       ( (*ppNODE_) ? &( (*ppNODE_)->Next ) : ppNODE_ )

/****************************************************
** Structure of each node in a generic NODE_ list. **
****************************************************/
struct NODE_
{
   void          *Info;
   struct NODE_  *Next;
   BYTE           Type;
   struct NODE_  *SubLst;
   int            SubCnt;
   WORD           Flags;
}; typedef struct NODE_ NODE_;

/***************************************
** Each entry in StrTab[] has this type.
***************************************/
struct StrLst_OBJ
{
   char  *InpNm;
	char  *OutNm;
   struct StrLst_OBJ *Next;

}; typedef struct StrLst_OBJ StrLst_OBJ;

#define TokInpBuf_MAX    1024    // Max length of input buffer.
#define TokLen_MAX       1024    // Max token length.

struct Tok_INFO
{
   FILE  *fpInp;            // Read this file to get Tok.
   char  *fnInp;            // Name of file.  MUST BE SET BY USER!
   char   InpBuf[ TokInpBuf_MAX+1 ];  // Buffer for fpInp.
   char *pInpBuf;           // Current location in InpBuf.
   long   InpLine_Cnt;      // Lines read so far in fpInp.
   char   Tok[ TokLen_MAX+1 ];   // The token read after each call to GetTok().
   int    RetainQuotes;     // Leave " quotes in Tok?
   char  *SpaceChars;       // Chars to treat as white space (default="\n \t")
   char  *SpecialToks;      // Treated as separate tokens.  Example: "()[];"
   int    LispLike;         // If LispLike is TRUE:
                            //    1. Treat '(' and ')' as separate tokens.
                            //    2. Ignore newlines on input.
   char  *CommentEOL[16];   // Comment char(s) (to end of line).
                            //    Up to 15 tokens allowed, such as  ";"  "--"  etc.
   char  *CommentBOL;       // Comment char(s) (beg of line allowed ONLY).
   char   C_Comments;       // Allow "//" and "/* */" ?
   int    CommentLevel;     // Current "/* */" nest level.
   char  *Quotes;           // Chars used for quoting. [0] = left, [1] = rigth
   char  *IgnoreChars;      // Chars to ignore on input.
   BYTE   BackSlashNormal;  // Treat backslash in strings as normal char?
   BYTE   GetByLine;        // Read input with fgets()?
   long   Offset;           // Current fpInp offset.   SK1109f
   char   PushTok;          // if pushtok is set, do not read a new token, but
                            // just return the same one.
}; typedef struct Tok_INFO Tok_INFO;

static char *PopStr( char *InpStr, char *OutStr, char *Quotes, char *SpaceChars, char *SpecialToks, int RetainQuotes );
static int   GetTok( Tok_INFO *t );
static int   GetTok_NextBuf( Tok_INFO *t );
static int   GetTok_fopen( Tok_INFO *t, char *FileNm, char *Mode );

static NODE_ **FindNode( NODE_ **Node, void *Info );

#include "ck.h"

#define ER_MAX_SBTAB     4045
#define ER_MAX_PKTAB     4046

struct StpCmd_NODE
{
   int                   ArgC;
   char                **ArgV;
   struct StpCmd_NODE   *Next;
   WORD                  Flags;
};

typedef struct StpCmd_NODE StpCmd_NODE;

// Max args on a setup or catalog line.
//
#define ArgV_MAX        64

#define L_BUF           255

// Name substitution table
//
#define SbTab_MAX       50000 // 

// Possible values for the Type field in an SbTab_OBJ object.
//
#define SB_EXACT    's'
#define SB_SUBSTR   'x'

#define FindNm_MAX      4     // See SbTab_OBJ definition
#define ErrNm_MAX       128   // Max length of an error name. See Va_CheckNm().

typedef struct
{
   char  NmType;     // NM_*
   char *Nm;         // Index into StrTab[]

} Sb_ARG;

typedef struct
{
   char   Type;                   // SB_EXACT, SB_SUBSTR, ...
   Sb_ARG Find[ FindNm_MAX+1 ];   // Specifies the names to match and their types NM_*
   Sb_ARG Repl;                   // Specifies the name to replace with and the name type NM_*
   char   Logged;

} SbTab_OBJ;

typedef struct
{
   int   MaxLen;      // Max length of name.
   char *ValidCh;     // Points to string of valid chars for this type of name.
   char *ErrPrefix;   // On error, name is converted using this prefix.
   int   ErrSuffix;   // On error, this is incremented and appended to ErrPrefix.

} Va_INFO;


static  SbTab_OBJ **SbTab;
static  int         SbTab_Cnt = 0;

// Validation table.
// Each element Va[i] holds a validation structure
// for a particular type i = 0..255.  VaTab['a'] == VaTab[97]
// 
static Va_INFO VaTab[256];

static int StpCmd_Add( int ArgC, char **ArgV );

typedef struct
{
   CString  reportstring;
   CString  InpNm;
   CString  OutNm;
	bool     collision; // report record type
}CKreport;
typedef CTypedPtrArray<CPtrArray, CKreport*> CKreportArray;

static   CKreportArray  reportarray;   
static   int            reportcnt;

/**************************
** BEGIN rsutil.c MODULE **
**************************/
static StrLst_OBJ **InpNmStrTab  = NULL;  // Table hashed on InpNm
static StrLst_OBJ **OutNmStrTab  = NULL;  // Table hashed on OutNm


static int          StrTab_Max       = 512;
static char       *pEmptyStr         = NULL;
static int          StrTab_RaiseCase = TRUE;
static bool         StrTab_IgnoreCollisions = false; // by default detect and remap collisions 

static int          ADD_BACKSLASH    = FALSE;      // should a backslash be added infront of a backslash

// These vars are set by the ERR_IF() macro.
//
static int   LastErr = 0;  // The values OK, FAIL, or an ER_* constant.
static char  ErrFile[64];
static int   ErrLine;

// File handle for "con" (DOS) or "/dev/tty" (Unix)  SK0501
// Should be set correctly in application init routine.
//
static FILE *fpScr = NULL;

static char TOUPPER(char C)
{
   if (!StrTab_RaiseCase)  return C;

   return ( islower(C) ? toupper(C) : C );
}

static NODE_ **DelNode( NODE_ **Node, void *Info, int *Count )
{
   if ( Info )
      Node = FindNode( Node, Info );

   if ( Node && *Node )
   {
      NODE_ *OldNode = *Node;

      *Node = (*Node)->Next;
   
      FREE( OldNode );
   
      if ( Count ) (*Count)--;
   }
   return Node;
}

static NODE_ **FindNode( NODE_ **Node, void *Info )
{
   while ( Node && *Node && (*Node)->Info != Info )
   {
      Node = NEXT( Node );
   }
   return Node;
}


static int StrTab_Hash( char *s )
{
   static int i;

   for ( i=0; *s; ) i = i << 1 ^ *s++;

   if ( i < 0 ) i = -i;

   return i;
}

static StrLst_OBJ *InpNmStrTab_Find( char *Str )
{
   int i;
   StrLst_OBJ **Node;

   LastErr = 0;

   if (InpNmStrTab == NULL)
   {
      ErrorMessage("Error ck.cpp", "StrTab is NULL");
      return NULL;
   }

   if ( !Str )
      return NULL;

   if ( StrTab_RaiseCase )
      STRUPR( Str );

   i    = StrTab_Hash( Str ) % StrTab_Max;
   Node = &InpNmStrTab[i];

   while ( Node && *Node && (*Node)->InpNm )
   {
      if ( !STRCMPI( (*Node)->InpNm, Str ) )
         return (*Node);

      Node = NEXT( Node );
   }

	return NULL;
}

static StrLst_OBJ *OutNmStrTab_Find( char *Str )
{
   int i;
   StrLst_OBJ **Node;

   LastErr = 0;

   if (OutNmStrTab == NULL)
   {
      ErrorMessage("Error ck.cpp", "OutNmStrTab is NULL");
      return NULL;
   }

   if ( !Str )
      return NULL;

   //if ( StrTab_RaiseCase )
   //   STRUPR( Str );

   i    = StrTab_Hash( Str ) % StrTab_Max;
   Node = &OutNmStrTab[i];

   while ( Node && *Node && (*Node)->OutNm )
   {
      if ( !STRCMPI( (*Node)->OutNm, Str ) )
         return (*Node);

      Node = NEXT( Node );
   }

	return NULL;
}

static StrLst_OBJ *OutNmStrTab_Upd( char *InpNm, char *OutNm )  // operates on InpNmStrTab
{
	// Hashed on OutNm

   int i;
   StrLst_OBJ **Node, *pNewNode;
   char *pNewInpNm;
	char *pNewOutNm;

   LastErr = 0;

   if (OutNmStrTab == NULL)
   {
      ErrorMessage("Error ck.cpp", "OutNmStrTab is NULL");
      return NULL;
   }

   if ( !InpNm || !OutNm )
      return NULL;

   //if ( StrTab_RaiseCase )
   //   STRUPR( Str );

   i    = StrTab_Hash( OutNm ) % StrTab_Max;
   Node = &OutNmStrTab[i];

   while ( Node && *Node && (*Node)->OutNm )
   {
      if ( !STRCMPI( (*Node)->OutNm, OutNm ) )
         return (*Node);

      Node = NEXT( Node );
   }

   pNewInpNm  = STRDUP( InpNm );
	pNewOutNm  = STRDUP( OutNm );
   pNewNode = (StrLst_OBJ *)calloc(1, sizeof(StrLst_OBJ));

   if ( !( pNewNode && pNewInpNm && pNewOutNm ) )
   {
      FREE( pNewNode );
      FREE( pNewInpNm );
		FREE( pNewOutNm );
      MemErrorMessage(__FILE__, __LINE__);
      return NULL;      
   }

	pNewNode->InpNm = pNewInpNm;
	pNewNode->OutNm = pNewOutNm;

   *Node = pNewNode;

   return (*Node);
}

static char *StrTab_Upd( char *Str )  // operates on InpNmStrTab
{
   int i;
   StrLst_OBJ **Node, *pNewNode;
   char *pNewStr;

   LastErr = 0;

   if (InpNmStrTab == NULL)
   {
      ErrorMessage("Error ck.cpp", "StrTab is NULL");
      return Str;
   }

   if ( !Str )
      return NULL;

   if ( StrTab_RaiseCase )
      STRUPR( Str );

   i    = StrTab_Hash( Str ) % StrTab_Max;
   Node = &InpNmStrTab[i];

   while ( Node && *Node && (*Node)->InpNm )
   {
      if ( !STRCMPI( (*Node)->InpNm, Str ) )
         return (*Node)->InpNm;

      Node = NEXT( Node );
   }

   pNewStr  = STRDUP( Str );
   pNewNode = (StrLst_OBJ *)calloc(1, sizeof(StrLst_OBJ));

   if ( !( pNewNode && pNewStr ) )
   {
      FREE( pNewNode );
      FREE( pNewStr );
      MemErrorMessage(__FILE__, __LINE__);
      return NULL;      
   }

	pNewNode->InpNm = pNewStr;
	pNewNode->OutNm = NULL;

   *Node = pNewNode;

   return (*Node)->InpNm;
}

static int StrTab_Create( StrLst_OBJ ***StrTab )
{
   if ( !(*StrTab) )  // Disallow init if already set.
   {
      if (((*StrTab) = (StrLst_OBJ **) calloc(StrTab_Max, sizeof(StrLst_OBJ *))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

      pEmptyStr = StrTab_Upd( "" );
   }
 
   return 1;
}   

static void StrTab_Destroy( StrLst_OBJ **StrTab )
{
   int i;
   StrLst_OBJ **Node;

   for ( i=0; StrTab && i < StrTab_Max; i++ )
   {
      for ( Node = &StrTab[i]; Node && *Node; )
      {
         if ( (*Node)->InpNm != NULL)
				FREE( (*Node)->InpNm );

			if ( (*Node)->OutNm != NULL)
				FREE( (*Node)->OutNm );

         DEL( StrLst_OBJ, Node, NULL, NULL );
      }
   }
   FREE( StrTab );
}   

// Temporarily disable StrTab_RaiseCase if enabled.
//
static char *StrTab_UpdNoRaise( char *Str )
{
   char *pStrTab    = NULL;
   int SaveStatus   = StrTab_RaiseCase;
   StrTab_RaiseCase = FALSE;
   pStrTab          = StrTab_Upd( Str );
   StrTab_RaiseCase = SaveStatus;
   return pStrTab;
}


// Set new StrTab_RaiseCase value with RaiseCase.  Save old value
// in *RaiseCaseSave if passed.  Return old value.
//
static int StrTab_SetRaiseCase( int RaiseCase, int *RaiseCaseSave )  // SK1116
{
   int Save = StrTab_RaiseCase;
   if ( RaiseCaseSave ) *RaiseCaseSave = Save;
   StrTab_RaiseCase = RaiseCase;
   return Save;
}


// strip quotes only on begin and end, but not if a '\\' is there <-- this is eliminated in Get_Esc...
// Strip Quotes should only strip quotes at begin and end, not in the middle.
// Copy s back to s without quotes.
//
static char *StripQuotes( char *s, char LQuote, char RQuote )
{
   char *p = s;

   if ( *s == LQuote ) s++;

   while ( *s)   // WS0111
   {
      *p++ = *s++;
   }
   *p = '\0';  // finish string
   if (p[strlen(p)-1] == RQuote)
      p[strlen(p)-1] = '\0';

   return p;
}


// Search InpBuf for every occurrence of Find and replace with Repl.
// The resulting string is placed in OutBuf (InpBuf is unchanged).
// OutBuf will not exceed MaxOutLen characters.
//
static int StrReplace( char *InpBuf, char *Find, char *Repl, char *OutBuf, int MaxOutLen )
{
   int  lFind      = strlen( Find );
   int  lRepl      = strlen( Repl );
   char *ip        = InpBuf;
   char *op        = OutBuf;
   char *opMax     = op + MaxOutLen - 1;
   int  ReplCount  = 0;

   memset( OutBuf, 0, MaxOutLen );

   while ( *ip && op <= opMax )
   {
      if ( strncmp( ip, Find, lFind ) )
      {
         *op++ = *ip++;
         continue;
      }
      strncat( op, Repl, opMax - op );

      ip += lFind,  op += lRepl,  ReplCount++;
   }
   op  = MIN( op, opMax ),  *op = '\0';

   return ReplCount;
}

/***************************
** END rsutil.c FUNCTIONS **
***************************/

/**************************
** BEGIN gettok.c MODULE **
**************************/
// Handle C-style escape sequences.
//
static int GetEscSequence( char **Inp, char **Out )
{
   int  i;
   char c = *++*Inp;  // Skip past \ escape char.

   switch ( ++*Inp, c )
   {
      // In order of likelihood:
      //
      case 'x': if ( sscanf( *Inp, "%2X" , &i ) > 0 ) *Inp += 2;  c = (char) i;  break;   // Hex "\xHH"
      case '0': if ( sscanf( *Inp, "x%3X", &i ) > 0 ) *Inp += 4;  c = (char) i;  break;   // Hex "\0xHHH"
      case 'n': c = 0x0A;  break;  // ^J
      case 't': c = 0x09;  break;  // ^I
      case 'r': c = 0x0D;  break;  // ^M
      case 'f': c = 0x0C;  break;  // ^L
      case 'a': c = 0x07;  break;  // ^G
      case 'b': c = 0x08;  break;  // ^H
   }
   *(*Out)++ = c;

   return 1;
}

#define STRCHR(S,C)      ( (S && C) ? strchr(S,C) : NULL )
#define IsSpecialTok(C)  ( SpecialToks && strchr( SpecialToks, C ) )
#define IN_COMMENT()     ( t->CommentLevel > 0 )

static int IsSpace( char *SpaceChars, char c )
{
   return SpaceChars ? ( strchr( SpaceChars, c ) != NULL ) : isspace(c);
}

static void SkipSpace( Tok_INFO *t )
{
   while ( *t->pInpBuf && IsSpace( t->SpaceChars, *t->pInpBuf ) )
      t->pInpBuf++;
}

// Pop a string out of InpStr and copy it to OutStr.
// Return pointer to stopping position in InpStr.
//
static char *PopStr( char *InpStr, char *OutStr, char *Quotes, char *SpaceChars, char *SpecialToks, int RetainQuotes )
{
   char LQuote = Quotes ? Quotes[0] : '\0';
   char RQuote = Quotes ? Quotes[1] : '\0';

   while ( *InpStr && IsSpace( SpaceChars, *InpStr ) )
      InpStr++;

   // String inside quote chars?
   //
   if ( *InpStr == LQuote )
   {
      if ( RetainQuotes )
         *OutStr++ = *InpStr++;
      else
         InpStr++;

      while ( *InpStr && *InpStr != RQuote )
      {
         if ( *InpStr == '\\' )
            GetEscSequence( &InpStr, &OutStr );
         else
            *OutStr++ = *InpStr++;
      }

      if ( *InpStr == RQuote )
      {
         if ( RetainQuotes )
            *OutStr++ = *InpStr;

         InpStr++;
      }

      *OutStr++ = '\0';
   }
   else
   {
      if ( IsSpecialTok( *InpStr ) )
      {
         *OutStr++ = *InpStr++;
      }
      else do
      {
         *OutStr++ = *InpStr++;
      }
      while ( *InpStr && !( IsSpace( SpaceChars, *InpStr ) || IsSpecialTok( *InpStr ) ) );
   }

   *OutStr = '\0';

   return InpStr;
}

static int IsCommentEOL( Tok_INFO *t )
{
   char **s;

   if ( t->C_Comments && !strncmp( t->pInpBuf, "//", 2 ) )
      return TRUE;
   
   for ( s = t->CommentEOL; *s; s++ )
      if ( !strncmp( t->pInpBuf, *s, strlen(*s) ) )
   {
      // fprintf( stderr, ">> Comment '%s' found!\n", *s );
      return TRUE;
   }
   return FALSE;
}

static int CheckComments( Tok_INFO *t )
{
   int FoundComments = FALSE;
   int n;

   // If  /* .. comments .. */  are allowed, handle them here.
   // 
   if ( t->C_Comments )
   {
      do
      {
         n = 0;

         if ( !strncmp( t->pInpBuf, "/*", 2 ) )
         {
            t->CommentLevel++,  n = 2;
         }
         else if ( IN_COMMENT() )
         {
            if ( !strncmp( t->pInpBuf, "*/", 2 ) )
            {
               t->CommentLevel--,  n = 2;
            }
            else
               n = 1;
         }

         if ( FoundComments = ( n > 0 ) )
         {
            t->pInpBuf += n;
            SkipSpace(t);  // Skip space.
         }

      } while ( *t->pInpBuf && IN_COMMENT() );
   }

   // If /*...*/ comments are NOT active...
   //
   if ( *t->pInpBuf && !IN_COMMENT() )
   {
      // Skip single-line comments.
      //
//    if ( STRCHR( t->CommentEOL, *t->pInpBuf ) || ( t->C_Comments && !strncmp( t->pInpBuf, "//", 2 ) ) )
      if ( IsCommentEOL(t) )
      {
         FoundComments = TRUE;
         *t->pInpBuf = '\0';
      }
   }

   return FoundComments;
}


// Use in place of fgets() for LispLike inputs files where
// newlines must be ignored.
//
static char *GetBuf( Tok_INFO *t, int BufMax )
{
   static int InStr  = FALSE;
   static int EscOn  = FALSE;
   char c, *p = t->InpBuf;
   
   BufMax -= 2;  // To be safe.
   
   while ( !feof( t->fpInp ) && p - t->InpBuf < BufMax )
   {
      c = getc( t->fpInp );

      if ( c == '\n' )
         t->InpLine_Cnt++;

      // Ignore c if it is found in t->IgnoreChars AND satisfies one of the
      // following: c is a newline OR c is outside of a quoted string.
      //
      if ( STRCHR( t->IgnoreChars, c ) && ( c == '\n' || !InStr ) )
         continue;

      *p++ = c;

      if ( InStr )
      {
         if ( EscOn )
            EscOn = FALSE;
         else
         {
            if ( t->Quotes && c == t->Quotes[1] )   // Right quote
               InStr = FALSE;
            else if ( c == '\\' )      // Escape char coming
            {
               EscOn = TRUE;

               if ( t->BackSlashNormal )  // Add extra slash so GetEscSequence()
                  *p++ = '\\';            //  will not treat as special later.
            }
         }
      }
      else
      {
         if ( t->Quotes && c == t->Quotes[0] )      // Left quote
            InStr = TRUE;
         else if ( c == ')' )          // Stop filling Buf now
            break;
      }
   }
   *p = '\0';

   return feof( t->fpInp ) ? NULL : t->InpBuf;
}

// Chars contains characters that should be deleted from Buf.
//
static char *DeleteChars( char *Buf, char *Chars )
{
   char *p, *q;
   
   if ( !( Buf && Chars ) )
      return NULL;

   for ( p=q=Buf; *p; p++ )
      if ( !strchr( Chars, *p ) )
         *q++ = *p;

   return *q = '\0', Buf;
}

// Read a single token Tok from the input file.  Return TRUE if a token
// was recieved, otherwise FALSE (end-of-file reached).
//
static int GetTok( Tok_INFO *t )
{
   int GotTok = FALSE;

   if (t->PushTok) // WS0118
   {
      t->PushTok = FALSE;  // retain the original one.
      return TRUE;
   }

   t->Tok[0] = '\0';

   if ( t->LispLike && !t->SpecialToks )
      t->SpecialToks = "()";

   do
   {
      // Read in a new line if we need one.
      //
      if ( !( t->pInpBuf && *t->pInpBuf ) )
      {
         memset( t->InpBuf, 0, TokInpBuf_MAX+1 );

         // break if EOF reached.
         //
//       if ( t->fpInp && fgets( t->InpBuf, TokInpBuf_MAX, t->fpInp ) == NULL )
//          break;

         if ( !t->fpInp )
            break;

         if ( t->LispLike && !t->GetByLine )
         {
            if ( !GetBuf( t, TokInpBuf_MAX ) )
               break;
         }
         else
         {
            if ( !fgets( t->InpBuf, TokInpBuf_MAX, t->fpInp ) )
               break;

            DeleteChars( t->InpBuf, t->IgnoreChars );

            t->InpLine_Cnt++;
         }

         t->Offset  = ftell( t->fpInp );  // SK1109
         t->pInpBuf = t->InpBuf;

         SkipSpace(t);  // Skip leading space.

         // Check for beg-of-line comment char.
         //
         if ( STRCHR( t->CommentBOL, *t->pInpBuf ) )
         {
            *t->pInpBuf = '\0';
            continue;
         }
      }
      else
         SkipSpace(t);  // Skip leading space.
   
      // If no more char in input line, just loop.
      //
      if ( !*t->pInpBuf )
         continue;

      // If comment(s) were found at t->pInpBuf and skipped, loop around again.
      //
      if ( CheckComments( t ) )
         continue;

      t->pInpBuf = PopStr( t->pInpBuf, t->Tok, t->Quotes, t->SpaceChars, t->SpecialToks, t->RetainQuotes );

      GotTok = t->Tok[0] != '\0';

      SkipSpace(t);  // Skip trailing space.

      CheckComments( t );

   } while ( !GotTok );

// printf( ">> Tok={%s}\n", t->Tok );

   return GotTok;
}

// Discard current t->InpBuf and read a new buffer (read next line).
//
static int GetTok_NextBuf( Tok_INFO *t )
{
   if ( t->pInpBuf )
      *t->pInpBuf = '\0';

   return GetTok(t);
}

static int GetTok_fopen( Tok_INFO *t, char *FileNm, char *Mode )
{
   if ( t && ( t->fpInp = fopen( FileNm, Mode ) ) )
   {
      t->fnInp = StrTab_UpdNoRaise( FileNm );
      return 1;
   }
   return -1;
}      

/************************
** END gettok.c MODULE **
************************/


/****************************
** BEGIN setuplib.c MODULE **
****************************/
static StpCmd_NODE *UserCmdLst     = NULL;
static int          UserCmdLst_Cnt = 0;


// Called from StpFile_Read() to read next command line.
//
static int StpCmd_Read( Tok_INFO *t, int *ArgC, char **ArgV )
{
   int MoreFile;

   *ArgC = 0;

   while ( MoreFile = GetTok( t ) )
   {
      // Strip quotes from token before using.
      //
      if ( t->Tok[0] == '"' )
         StripQuotes( t->Tok, '"', '"' );

      ArgV[ (*ArgC)++ ] = STRDUP(t->Tok);
      
// fprintf( stderr, "\r** ArgV[%d]={%s}\n", (*ArgC)-1, ArgV[ (*ArgC)-1 ] );
      // If no more on this line, break.
      //
      if ( !*t->pInpBuf )
         break;
   }

   ArgV[ *ArgC ] = NULL;

// printf( "** StpCmd_Read(): ArgV: {%s}\n", ArgV_Str(ArgV) );

   return MoreFile;
}

static void ArgV_Free( int *ArgC, char **ArgV )
{
   if ( ArgV )
   {
      for ( ; *ArgV; ArgV++ )
         FREE( *ArgV );
   }
   *ArgC = 0;
}

/******************************************************************************
* StpFile_Read
// Read the setup file *FileNm into memory.  Call user function
// StpCmd_Add() after parsing each command line.
*/
static int StpFile_Read(const char *FileNm )
{
   Tok_INFO t;
   int   ArgC = 0;
   char *ArgV[ ArgV_MAX+1 ];

   LastErr = 1;

   // Initialize token info for GetTok() calls.
   //
   memset( &t, 0, sizeof( Tok_INFO ) );
   t.Quotes        = "\"\"";  // Left and right quote chars.
   t.RetainQuotes  = TRUE;    // Retain quotes, strip later.
   t.CommentEOL[0] = ";";     // ";" to end-of-line is comment.
   t.C_Comments    = TRUE;    // Allow "//" and "/* ... */" comments

   if ( !FileNm )   // OK if name argument is NULL.
      goto Exit;

   if ( ( t.fpInp = fopen( FileNm, "rt" ) ) == NULL )
   {
      ErrorMessage(FileNm, "Error opening NAMECHECK file.");
      goto Exit;
   }

   while ( StpCmd_Read( &t, &ArgC, ArgV ) )
   {
      // Send this line to StpCmd_Add() if it looks like a dot command.
      //
      if ( ArgC && *ArgV[0] == '.' )
      {
         // Call StpFile_Read() again?
         //
         if ( !STRCMPI( ArgV[0], ".SetupFile" ) )
         {
            StpFile_Read( ArgV[1] );
         }
         else
         {
            // StpCmd_Add() is defined by user.
            //
            if ( StpCmd_Add( ArgC, ArgV ) != 1)
               LastErr = TRUE;
         }
      }
      // here was the error !!!! always needs to get freed, even if no valid setup command.
      ArgV_Free( &ArgC, ArgV );
   }

   fclose( t.fpInp );

   ArgV_Free( &ArgC, ArgV );

   Exit:

   return LastErr;
}
   
// Called by user function StpCmd_Add() if the command is
// not recognized.
//
static StpCmd_NODE *UserCmdLst_Add( int ArgC, char **ArgV )
{
   static StpCmd_NODE **Tail = &UserCmdLst;
   static StpCmd_NODE  *Node;
   int i;

   LastErr = 1;

   Node = (StpCmd_NODE *)calloc(1, sizeof(StpCmd_NODE));
 
   Node->ArgC = ArgC;

   Node->ArgV = (char **)calloc(ArgC+1, sizeof(char *));
   
   for ( i=0; i < ArgC; i++ )
      Node->ArgV[i] = StrTab_Upd( ArgV[i] );

   Node->ArgV[i] = NULL;

   *Tail = Node;
   Tail  = &Node->Next;

   return Node;
}


// Usually called by user function such as DeInitSetup().
//
static void UserCmdLst_Destroy( void )
{
   static StpCmd_NODE **Node, *OldNode;

   for ( Node = &UserCmdLst; *Node; )
   {
      OldNode = *Node;

      *Node = (*Node)->Next;
   
      FREE( OldNode->ArgV );
      FREE( OldNode );
   }   
}


// Example usage:
//    StpCmd_NODE *Cmd;
//    int i;
//    for ( Cmd=NULL; UserCmdLst_GetNext( ".FooCommand", &Cmd ); )
//    {
//       printf( "*** Cmd: " );
//       for ( i=0; i < Cmd->ArgC; i++ ) printf( " %s", Cmd->ArgV[i] );
//       printf( "\n" );
//    }
//
// ** Note that when Cmd == NULL, all entries
// ** from UserCmdLst are returned.
//
static StpCmd_NODE *UserCmdLst_GetNext( char *CmdNm, StpCmd_NODE **Cmd )
{
   int CmdNmLen = CmdNm ? strlen( CmdNm ) : 0;
   
   *Cmd = *Cmd ? (*Cmd)->Next : UserCmdLst;

   for ( ; *Cmd; *Cmd = (*Cmd)->Next )
   {
      if ( !( CmdNm && STRNICMP( (*Cmd)->ArgV[0], CmdNm, CmdNmLen ) ) )
         return *Cmd;
   }
   return NULL;
}

/*****************************
** END setuplib.c FUNCTIONS **
*****************************/

/**************
** UTILITIES **
**************/

/********************
** 'ADD' FUNCTIONS **
********************/


/****************************************************
   SYNTAX             WHEN THESE        CHANGE
                      NAMES MATCH      THIS NAME
   ~~~~~~~~~~~~~     ~~~~~~~~~~~~~~   ~~~~~~~~~~~~
   .s:n:n             Net              NewNet

   .s:c:c             Comp             NewComp
   .s:ct:c            Comp   Type      NewComp
   .s:cs:c            Comp   Shape     NewComp
   .s:cv:c            Comp   Value     NewComp

   .s:t:t             Type             NewType
   .s:ts:t            Type   Shape     NewType
   .s:tv:t            Type   Value     NewType

   .s:s:s             Shape            NewShape
   .s:st:s            Shape  Type      NewShape
   .s:sv:s            Shape  Value     NewShape

   .s:v:v             Value            NewValue
   .s:vt:v            Value  Type      NewValue
   .s:vs:v            Value  Shape     NewValue

   .s:p:p             Pin              NewPin
   .s:pt:p            Pin    Type      NewPin
   .s:ps:p            Pin    Shape     NewPin
   .s:pv:p            Pin    Value     NewPin

****************************************************/
static int SbTab_Add( int ArgC, char **ArgV )
{
   SbTab_OBJ *Sb = NULL;
   int i, j;
   char  *CmdStr  = ArgV[0];
   char **NmArg   = ArgV + 1;
   char  *p;
   char   NmType;
   char  *Nm;

   LastErr = 1;

   if (SbTab_Cnt >= SbTab_MAX)
   {
      ErrorMessage("Too many SbTab_Add", "Error ck.cpp SbTab_Add");
   }

   if ((Sb = (SbTab_OBJ *)calloc(1,sizeof(SbTab_OBJ))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

   Sb->Type = tolower(CmdStr[1]);

   // Examine CmdStr to determine what type of substitution command this is.
   //
   for ( p = strtok( CmdStr, ":" ), i=0; p; p = strtok( NULL, ":" ), i++ )
   {
      if ( i == 0 ) continue;

      if (i > 2)
         ErrorMessage("Error in Setup File", "Setupstring [0] wrong");

      for ( j=0; *p && j < FindNm_MAX; p++, j++, NmArg++ )
      {
         // Check for valid NmType char in command string
         //
         NmType = *p;

         // Not enough args for command
         //
         if (!*NmArg)
            ErrorMessage("Error in Setup File", "");

         Nm = StrTab_Upd( *NmArg );
         ERR_IF( !Nm, LastErr );

         if ( i == 1 )
         {
            Sb->Find[j].NmType = tolower(NmType);
            Sb->Find[j].Nm     = Nm;
         }
         else
         {
            if (j > 0)
               ErrorMessage("Error in Setup File", "");

            Sb->Repl.NmType = tolower(NmType);
            Sb->Repl.Nm     = Nm;
         }
      }
   }

   Sb->Logged = FALSE;

   SbTab[ SbTab_Cnt++ ] = Sb;

   Exit:

   if ( LastErr )
   {
      FREE( Sb ); 
      CString  tmp;
      tmp.Format("Syntax Error in .CHK file ");
      ErrorMessage(tmp, "Error reading setup file file");
      //LOGERR( "SbTab_Add()" );
   }
   return LastErr;
}


/******************************************
   SYNTAX:
     .v:n  MaxLen  ValidChars  ErrPrefix
     .v:c    "         "          "
     .v:t    "         "          "
     .v:s    "         "          "
     .v:v    "         "          "
     .v:p    "         "          "
    
******************************************/
static int Va_Add( int ArgC, char **ArgV )
{
   char NmType = tolower(ArgV[0][3]);
   Va_INFO *Va = &VaTab[ NmType ];

   LastErr = 1;

   if (ArgC != 4)
      ErrorMessage("Error in Setup File", "");

   Va->MaxLen    = atoi( ArgV[1] );
   Va->ValidCh   = StrTab_Upd( ArgV[2] );  ERR_IF( !Va->ValidCh   , LastErr );
   Va->ErrPrefix = StrTab_Upd( ArgV[3] );  ERR_IF( !Va->ErrPrefix , LastErr );
   Va->ErrSuffix = 0;

   Exit:

   if ( LastErr )
   {
      ErrorMessage("Va_Add()");
   }
   
   return LastErr;
}

// Examine ArgV[0] and call the appropriate *_Add function.
//
static int StpCmd_Add( int ArgC, char **ArgV )
{
   //char *Cmd = strlwr( ArgV[0] ); // this will change names - no good
   char *Cmd = ArgV[0];

   LastErr = 1;
   
   // fprintf( stderr, "\rCmd={%s} ArgV[1]={%s}\n", Cmd, ArgV[1] );

   // global commands
   if (!STRCMPI(Cmd,".case_sensitive"))
      StrTab_RaiseCase = FALSE;
   else
   {
      if ( !_strnicmp( Cmd, ".s:", 3 ) || !_strnicmp( Cmd, ".x:", 3 ) )
         SbTab_Add( ArgC, ArgV );
      else 
      if ( !_strnicmp( Cmd, ".v:", 3 ) )
         Va_Add( ArgC, ArgV );
   }
   return LastErr;
}

static int Va_CheckNm( char NmType, char **OrigNm, char **Nm, Va_INFO *Va, int DoLog )
{
   int GoodNm = FALSE;
   char *p;

   LastErr = 1;

   if (Va->MaxLen == 0)    // no validate found
      return LastErr;

   // Check name length
   //
   if ( (int)strlen(*Nm) <= Va->MaxLen )
   {
      // Every char in *Nm should be valid.
      //
      for ( p = *Nm; *p && strchr( Va->ValidCh, TOUPPER(*p) ); p++ )
         ;

      // *Nm is OK if we got all the way to the end.
      //
      if ( !*p )
         GoodNm = TRUE;
   }

   // If *Nm is bad, replace with an error name and log it.
   //
   if ( !GoodNm )
   {  
      char tmp[ ErrNm_MAX+1 ], *NewNm;
      CString  pref;

      pref = Va->ErrPrefix;   // this can be a NULL
      if (Va->MaxLen < (int)strlen(pref) + 4)
      {
         sprintf( tmp, "%.*s%03d", ErrNm_MAX-8, pref, ++Va->ErrSuffix );
      }
      else
      {
         sprintf( tmp, "%.*s%04d", ErrNm_MAX-8, pref, ++Va->ErrSuffix );
      }

      // it does not remenber the old name
      if ( ( NewNm = StrTab_Upd( tmp ) ) == NULL) 
         LastErr = TRUE;

      if (DoLog )
      {
         char  t[8];
         char  *a[3];
         sprintf( t, ".s:%c:%c", NmType, NmType);
         a[0] = t;
         a[1] = *OrigNm;   // this unmodified name
         a[2] = tmp;

         SbTab_Add( 3, a );
      }
      *Nm = NewNm;
   }

   return LastErr;
}

/*******************
** MISC FUNCTIONS **
*******************/

static int InitSetup( const char *path )
{
   if ((SbTab = (SbTab_OBJ **) calloc( SbTab_MAX, sizeof(SbTab_OBJ))) == NULL)
         MemErrorMessage(__FILE__, __LINE__);

   memset( (char *) VaTab, 0, sizeof(VaTab) );

   reportarray.SetSize(100,100);
   reportcnt = 0;



   return 1;
}

/*****************************************************************************/
/*
*/
static int DeInitSetup( void )
{
   int i;

   for ( i=0; i < SbTab_Cnt; i++ )  FREE( SbTab[i] );
   FREE( SbTab );  SbTab_Cnt = 0;

   UserCmdLst_Destroy();

   for (i=0;i<reportcnt;i++)
   {
      delete reportarray[i];
   }
   reportarray.RemoveAll();
   reportcnt = 0;

   return 1;
}


/*****************************************************************************/
/*
*/
static SbTab_OBJ *Sb_GetNext( SbTab_OBJ **Sb )
{
   static int i;

   i = *Sb ? i+1 : 0;

   if (SbTab == NULL)
      return NULL;

   return *Sb = SbTab[i];
}


/******************************
** Public check_* functions  **
******************************/

/******************************************************************************
* check_init
*/
void check_init( const char *FileNm, bool RaiseCase, bool IgnoreCollisions)
{
   StrTab_Create(&InpNmStrTab);
	StrTab_Create(&OutNmStrTab);

   // make a absolute filepath.
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   
   // need to do this, because panel only knows the path.ext
   _splitpath( FileNm, drive, dir, fname, ext );

   char  tmppath[_MAX_PATH];
   _makepath(tmppath, drive, dir, "temp", "log");

   InitSetup(tmppath);
	StrTab_RaiseCase = RaiseCase;
   StrTab_IgnoreCollisions = IgnoreCollisions;
   ADD_BACKSLASH = FALSE;     // should a backslash be added infront of a backslash

   StpFile_Read(FileNm);
}

/*****************************************************************************/
/*
*/
void check_addbackslash(int add_backslash)
{
   ADD_BACKSLASH = add_backslash;
}

/*****************************************************************************/
/*
*/
void check_deinit( void )
{
   DeInitSetup();

   StrTab_Destroy(InpNmStrTab);
	InpNmStrTab = NULL;

	StrTab_Destroy(OutNmStrTab);
	OutNmStrTab = NULL;
}

/******************************************************************************
* load_report
*/
static int load_report(const char *t, const char *inp, char **outp, Va_INFO *Va)
{

   int curPos = 0;
   CString tmpReportString = t;
   CString commandType = tmpReportString.Tokenize(" ", curPos);

   for (int i=0;i<reportcnt;i++)
   {
      CKreport *report = reportarray[i];
      if (!report->reportstring.Compare(tmpReportString))
         return 1;

      if (!strcmp(report->InpNm, inp))
      {
         // If renamed already exist, check to see if command type is the same
         curPos = 0;
         CString type = report->reportstring.Tokenize(" ", curPos);
         if (type.CompareNoCase(commandType))
            continue;
            
         // Reuse the same rename if input string and command type is the same
         *outp = StrTab_Upd(report->OutNm.GetBuffer(0));
         Va->ErrSuffix--;  // This got incremented in a function Va_CheckNm to rename, so go back to the previous number
         return 1;
      }
   }

   CKreport *c = new CKreport;
   reportarray.SetAtGrow(reportcnt++, c);  

   c->reportstring = tmpReportString;
   c->InpNm = inp;
   c->OutNm = *outp;
	c->collision = false;

   return 1;
}
/******************************************************************************
* load_report_collision
*/
static int load_report_collision(const char *t)
{
	// All collisions are reported, no message filtering

   CKreport *c = new CKreport;
   reportarray.SetAtGrow(reportcnt++, c);  

   c->reportstring = t;//tmpReportString;
   c->InpNm = "";
   c->OutNm = "";
	c->collision = true;

   return 1;
}

/*****************************************************************************/
/*
*/
char  *check_fixbackslash(char *Nm)
{

   if (ADD_BACKSLASH)
   {
      static CString tmp;
      tmp = "";
      for (int i=0;i<(int)strlen(Nm);i++)
      {
         if (Nm[i] == '\\')
            tmp += "\\";
         tmp += Nm[i];
      }

      return tmp.GetBuffer(0);
   }

   return Nm;
}

/******************************************************************************
* check_name
*/
char *check_name( char ReplType, const char *INm, BOOL doLog)
{
   static char Buf[ L_BUF+1 ];
   char *FindNm, *ReplNm, *OutNm;
   char InpNm1 [L_BUF+1 ];    
   char *InpNm;
   SbTab_OBJ *Sb;

   strncpy(InpNm1, INm, L_BUF);
	InpNm1[L_BUF] = '\0';

	StrLst_OBJ *strTabObj = InpNmStrTab_Find( InpNm1 );

	if (strTabObj != NULL && strTabObj->OutNm != NULL)
	{
		return strTabObj->OutNm;
	}

   InpNm = StrTab_Upd( InpNm1 );
   OutNm = InpNm;

	if (strTabObj == NULL)
		strTabObj = InpNmStrTab_Find( InpNm1 );

	if (strTabObj == NULL)
	{
		// Should not happen
		return NULL; // or crash
	}

   char replTypeLower = tolower(ReplType);

   // Find all entries in SbTab[] matching ReplType.
   // first only exact
   for ( Sb=NULL; Sb_GetNext( &Sb ); )
   {
      if (Sb->Repl.NmType != replTypeLower)
         continue;

      FindNm = Sb->Find[0].Nm;
      ReplNm = Sb->Repl.Nm;

      if ( Sb->Type == SB_EXACT )
      {
         if ( FindNm == InpNm )
         {
            OutNm = ReplNm;
            break;
         }
      }
   }

   // Find all entries in SbTab[] matching ReplType.
   // not exact 
   for ( Sb=NULL; Sb_GetNext( &Sb ); )
   {
      if (Sb->Repl.NmType != replTypeLower)
         continue;

      FindNm = Sb->Find[0].Nm;
      ReplNm = Sb->Repl.Nm;

      if ( Sb->Type == SB_SUBSTR )
      {
         if ( StrReplace( OutNm, FindNm, ReplNm, Buf, sizeof(Buf)-1 ) )
         {
				if (strTabObj->OutNm != NULL)
					free (strTabObj->OutNm);
				OutNm = strTabObj->OutNm = STRDUP(Buf);
            //break; <=== do not break, because there can be multiple replaces
         }
      }
   }

   // Now process validations.
   Va_CheckNm( ReplType, &InpNm, &OutNm, &VaTab[ReplType], doLog );
  
   // Here fix \\ character
	char *slashFixedName = check_fixbackslash(OutNm);

	if (strTabObj->OutNm != NULL && strTabObj->OutNm != slashFixedName)
		free (strTabObj->OutNm);
	OutNm = strTabObj->OutNm = STRDUP(slashFixedName);

	// Fix mapped name collisions
	StrLst_OBJ *existing = NULL;
	CString originalOutNm(OutNm);
	int suffix = 2;
   if (!StrTab_IgnoreCollisions)
   {
      // Detect different unique inputs mapping to same output, change output
      // for this one by appending a number.
      while ((existing = OutNmStrTab_Find( OutNm )) != NULL)
      {
         CString msg;
         msg.Format("Collision: \"%s\" and \"%s\" both map to \"%s\"",
            existing->InpNm, InpNm, existing->OutNm);
         load_report_collision(msg);

         CString newName;
         newName.Format("%s%d", originalOutNm, suffix++);

         if (strTabObj->OutNm != NULL)
            free (strTabObj->OutNm);
         OutNm = strTabObj->OutNm = STRDUP(newName);
      }
   }

	OutNmStrTab_Upd(strTabObj->InpNm, strTabObj->OutNm);


   // WS 2-Oct-97, changed 8-Nov-97, need to fix that already renamed do not show up.
   if (strcmp(InpNm, OutNm))  // if different.
   {
      // Sb_Log( SB_EXACT, Sb->Find, InpNm, OutNm, ReplType );
      CString tmp;
      tmp.Format(".s:%c:%c \"%s\" \"%s\"", ReplType, ReplType, InpNm, OutNm );
      load_report(tmp, InpNm, &OutNm, &VaTab[ReplType]);
   }

   return strTabObj->OutNm;
}

/*****************************************************************************/
/*
   return if report were written into the report file.
*/
int check_report( FILE *fp )
{
   int   i;

   if ( !fp )
      return 0;
/*
      if ( fpLog )
      {
         fprintf( fpLog, "; Replace Original [%s] to New [%s]\n",InpNm, OutNm);
         fprintf( fpLog, ".s:%c:%c \"%s\" \"%s\"\n", ReplType, ReplType, InpNm, OutNm );
      }
*/
   // Write header only if there are some replacements to list.
   if (reportcnt > 0)
   {
      fprintf(fp, "Name replacements produced by NameCheck utility (.chk file).\n");
   }

   for (i=0;i<reportcnt;i++)
   {
		if (!reportarray[i]->collision)
			fprintf(fp,"; Replace Original [%s] to New [%s]\n",
				reportarray[i]->InpNm, reportarray[i]->OutNm);

      fprintf(fp,"%s\n",reportarray[i]->reportstring);
      
   }
   return reportcnt;
}

int check_report( CFormatStdioFile *fp )
{
   int   i;

   if ( !fp )
      return 0;

   for (i=0;i<reportcnt;i++)
   {
		fp->WriteString("; Replace Original [%s] to New [%s]\n",
         reportarray[i]->InpNm, reportarray[i]->OutNm);
		fp->WriteString("%s\n",reportarray[i]->reportstring);
      
   }
   return reportcnt;
}

// add an command from external
int   check_add_command(int ArgC, char **ArgV)
{

   if ( StpCmd_Add( ArgC, ArgV ) != 1)
   {
      return 1; // error
   }
   return 0;   // all ok
}

//
// EOF ck.cpp
//

