// $Header: /CAMCAD/4.6/TxtScan.cpp 13    4/03/07 5:47p Lynn Phung $

/*********************************************************************
**********************************************************************
*  text scanner function library
*
*  Version:
*     2.01
*
*  Header File:
*     "txtscan.h"
*
*  Local Variables:
*     Word_Delimeters      :Pointer to word delimeter string
*     White_Characters     :Pointer to white-char delimeter string
*     Non_Characters       :Pointer to non-char delimeter string
*     Text_Delimeter       :Quoted text start and stop character
*     Start_Comment        :Pointer to start-comment char sequence str
*     Stop_Comment         :Pointer to stop-comment char sequence str
*     Last_Char            :Last char delimeter read
*     Last_Delimeter       :Defined delimeter type for last char
*     Comment_String       :Flags next char within comment str
*     Text_String       :Flags next char within quoted text str
*     New_Line          :Flags end-of-line
*     Char_Count        :Holds current char number in text file
*     Line_Count        :Holds current line number in text file
*
*  Local C-Functions:
*     Check_NewLine        :Checks next character for new line
*     Check_Quote          :Checks for imbedded quotes
*     Check_Start_Comment  :Checks next character for start comment
*     Check_Stop_Comment   :Checks next character for stop comment
*     Character_Type       :Checks delimeter type for character
*
*  Callable C-Functions:
*     Set_Word_Delimeters  :Assigns word delimeter string to module
*     Set_Non_Characters   :Assigns non-char string to module
*     Set_White_Characters :Assigns white-char string to module
*     Set_Text_Delimeter   :Assigns quoted text delimeter to module
*     Set_Comment          :Assigns start-comment string to module
*     Set_Linecnt          :Assigns value to module's line counter
*     Skip_Line            :Moves file pos past next new-line char
*     Reset_File           :Sets file and module counters to begining
*     Delimeter            :Returns last delimeter character
*     Delimeter_Type       :Returns last delimeter defined constant
*     Get_Linecnt          :Returns current text-line position
*     Read_Char            :Returns next not non-char in text file
*     Read_Token           :Finds next delimeted token in text file
*     Read_Word            :Finds next not empty token in text file
*     Read_Line            :Finds next text-line in file
*
*  Comments:
*  2/12/91: Version 2.00 is an update to the previous ASCII
*     text scanner developed for Mentor Graphic Corporation (C)
*     (no version number).  While the previous function
*     calls and return values have been kept as close as
*     posible to the original module, significant changes
*     have been made to improve performance.  The module
*     no longer buffers its own character input, but uses the
*     standard C buffered io-stream procedures.  As a result,
*     all the previous character buffer and file positioning
*     functions have been removed.  In addition, a few additional
*     functions have been added to acommidate the new buffering
*     scheme.  Furthermore, the assignment of delimeter strings
*     has been modified to improve performance and flexibility,
*     and the defined values governing the maximum lengths of
*     delimeter lists has been removed.  SAVE THE ORIGINAL
*     TEXTSCAN MODULE FOR LINKING WITH EXISTING SOFTWARE, AND
*     COMPARE THE NEW FUNCTION LIST FOR VERSION 2.00 WITH THE
*     ORIGINAL MODULE TO NOTE THE DIFFERENCES.  For a short
*     description on using the module, refer to the documentation
*     file "txtscan.doc".
*
*    12/5/91: Updated to Version 2.01.  Gereral cleaning.  Major
*     change to line and character counting.  Error fixed in
*     '\r''\n' new line pairs.  Error fixed in quotes within
*     quotes '\"\"'.  Fixed extra character read in Read_Line.
*********************************************************************
*********************************************************************/

/*#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>*/
#include "StdAfx.h"
#include "General.h"
#include "txtscan.h"

static int  C_MODE = 1;             /* CMODE = 1 is that \" is a quote
                                       in Text, and not the end of text.
                                       CMODE = 0 is that "" is a quote
                                       in Text, and not the end of Text.
                                    */
static const char* Word_Delimeters  = NULL;  /* points to word delimeter string */
static char *White_Characters = NULL;  /* points to white-char string */
static const char* Non_Characters   = NULL;  /* points to non-char string */
static const char* Text_Delimeters  = NULL;  /* quoted text start/stop character */
static char *Start_Comment = NULL;  /* points to start-comment string */
static char *Stop_Comment  = NULL;  /* points to stop-comment string */

static char Last_Char      = '\0';        /* last delimeter read */
static char Last_Delimeter = NON_CHARACTER;  /* last delimeter type */
static char Last_Quote     = '\0';        /* last quote char found */

static int Comment_String = 0;   /* 1=commented text, 0=not commented */
static int Text_String    = 0;   /* 1=quoted text, 0=not quoted */
static int New_Line       = 0;   /* 1=end-of-line, 0=not end-of-line */

static long Char_Count = 0;   /* characters read from text file */
//static long Line_Count = 0;   /* lines read form text file */

static CString read_char_string; // this string is every char read from stream



/*********************************************************************
*  Check that next character read completes ASCII new line sequence
*     '\r','\n'.
*
*  C-Prototype:
*     static int Check_NewLine( FILE *text_file, char *ch );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*
*  Returns:
*     1  = next character read completes sequence '\r','\n'
*     0  = next character read is not new line character
*
*  Comments:
*     This routine is called whenever a carriage return '\r'
*     character is encountered during the file read.  If the
*     next character read from the file is a new line '\n'
*     character, then the functin returns true and the file pos
*     is left pointing to the first character after the
*     new-line sequence.  Otherwise, the function returns false
*     and moves the file pos back to the character after the '\r'.
*********************************************************************/
static int Check_NewLine( FILE *text_file, char *ch )
{
   char nextch;

   /* check for end of line character */
   if( (*ch == '\n') || (*ch == '\r') )
   {
      /* check for '\r''\n' pair and move ch to '\n' if present */
      if( *ch == '\r' )
      {
         if( (nextch = fgetc( text_file )) == '\n' )
         {
            Char_Count++;
            *ch = nextch;
         }
         else
         {
            /* just '\r', push nextch back onto file stream */
            ungetc( nextch, text_file );
         }
      }

      CTextScanner::incrementLineCount();

      return 1;
   }

   return 0;
}


/*********************************************************************
*  Check if next character read is an imbedded quote.
*
*  C-Prototype:
*     static int Check_Quote( FILE *text_file, int quote );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     quote    = char in text delimeters found in string
*
*  Returns:
*     1  = next character read is an imbedded quote
*     0  = next character read terminates quoted text
*
*  Comments:
*     This routine is called whenever a character in the
*     text delimeter list is encountered.  If the quote
*     character is the same as the quote that started the
*     string, then the next character must be the same
*     to flag an imbedded quote or the quoted text will
*     be terminated.  If the quote character is different
*     from the quote that started the string, it is treated
*     as an imbedded quote.
*********************************************************************/
static int Check_Quote( FILE *text_file, char quote )
{
   char ch;

   if (C_MODE)
      return 1;

   /* check that quote char matches char which started quoted text. */
   if( quote == Last_Quote ) {
      /* check for same character immediately following.  if so, */
      /* its not a terminating quote.  read literally. */
      if( (ch = fgetc( text_file )) == quote ) {
         Char_Count++;
         return 0;
      }
      ungetc( ch, text_file );
   }

   return 1;
}


/*********************************************************************
*  Check that next characters read complete the user specified
*     start-comment character sequence.
*
*  C-Prototype:
*     static int Check_Start_Comment( FILE *text_file );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*
*  Returns:
*     1  = next characters read complete the sequence Start_Comment
*     0  = next characters read do not complete the sequence.
*
*  Comments:
*     This routine is called whenever the current character read
*     matches the first character in the user specified
*     start-comment character sequence. If the characters match
*     the sequence, the function returns true and the file pos
*     is left pointing to the character following the sequence.
*     Otherwise, the function returns false and the file pos
*     is moved back to the character following the current
*     character.
*********************************************************************/
static int Check_Start_Comment( FILE *text_file )
{
   char ch;
   int i, length;

   /* determine length of start-comment sequence */
   if( (length = STRLEN( Start_Comment )) == 0 ) {
      return 0;
   }

   /* compare remaining chars against start-comment sequence */
   /* the start-comment sequence can not extend across new lines */
   for( i=1; i<length; i++ ) {
      if( (ch = fgetc( text_file )) != Start_Comment[i] ) {
         ungetc( ch, text_file );
         return 0;
      }
      Char_Count++;
   }

   return 1;
}


/*********************************************************************
*  Check that next characters read complete the user specified
*     stop-comment character sequence.
*
*  C-Prototype:
*     static int Check_Stop_Comment( FILE *text_file );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*
*  Returns:
*     1  = next characters read complete the sequence Stop_Comment
*     0  = next characters read do not complete the sequence.
*
*  Comments:
*     This routine is called whenever the current character read
*     matches the first character in the user specified
*     stop-comment character sequence. If the characters match
*     the sequence, the function returns true and the file pos
*     is left pointing to the character following the sequence.
*     Otherwise, the function returns false and the file pos
*     is moved back to the character following the current
*     character.
*********************************************************************/
static int Check_Stop_Comment( FILE *text_file )
{
   char ch;
   int i, length;

   /* determine length of stop-comment sequence */
   if( (length = strlen( Stop_Comment )) == 0 ) {
      return 0;
   }

   /* compare remaining chars agains stop-comment sequence.  the */
   /* stop comment sequence can not cross lines. */
   for( i=1; i<length; i++ ) {
      if( (ch = fgetc( text_file )) != Stop_Comment[i] ) {
         ungetc( ch, text_file );
         return 0;
      }
      Char_Count++;
   }

   return 1;
}


/*********************************************************************
*  Determine the delimeter classification for a character.
*
*  C-Prototype:
*     static int Character_Type( FILE *text_file, int ch );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     ch    = potential character delimeter
*
*  Returns:
*     Delimeter type associated with the character as defined
*     in the header file "textscan.h".
*
*  Comments:
*     To determine the delimeter classification for the
*     character, the function prioritizes the comparisons
*     according to the following hierarchy:
*
*        1). check if character marks the end of the file or error
*        2). check if character flags the begining of commented
*           text.  if so, all characters to the end of the
*           comment are ignored.
*        3). check if character flags the end of commented text.
*           if so, resume normal reading of characters.
*        4). check if character flags the begining or end
*           of quoted text. if it flags the begining,
*           all word delimeters are ignored until the end
*           of the quoted text is found. if it flags the end
*           of quoted text, resume normal parsing with
*           word delimeters.
*        5). check if character is a word delimeter flagging
*           the end of a token string.
*        6). if none of the above, character is normal text.
*
*         Non-characters are stripped out by the "get char" portions
*     of other functions and are simply ignored.  The character
*     count and line count are increased as needed.
*********************************************************************/
static int Character_Type( FILE *text_file, char ch )
{
   /* check for end-of-file delimeter.  this takes priority over all */
   /* other delimeters. */
   if( ch == EOF ) {
      Last_Char = EOF;
      Last_Delimeter = EOF_CHARACTER;
      return EOF_CHARACTER;
   }

   /* comments come next in priority.  check char against list marking */
   /* the start of commented text.  all delimeters within commented */
   /* text other then one marking end-of-comment are ignored */
   if( (ch == Start_Comment[0]) && !Comment_String )  
   {
      Last_Char = ch;
      if( Check_Start_Comment( text_file ) ) 
      {
         Comment_String = 1;
         return START_COMMENT;
      }
   }

   /* check ch against list marking end-of-comment.  only valid if */
   /* currently within commented text. */
   if( (ch == Stop_Comment[0]) && Comment_String ) 
   {
      Last_Char = ch;
      if( Check_Stop_Comment( text_file ) ) 
      {
         Comment_String = 0;
         return STOP_COMMENT;
      }
   }

   /* next in priority are delimeted text strings.  check here for */
   /* begining of text string.  all delimeters within delimeted */
   /* text are ignored except the end-of-text mark. */
   if( strchr( Text_Delimeters, ch ) && !Comment_String )   
   {
      Last_Char = ch;
      /* flags begining of quoted text if not already in quoted string */
      if( !Text_String ) 
      {
         Last_Quote = ch;
         Last_Delimeter = START_TEXT;
         Text_String = 1;
         return START_TEXT;
      }
      /* flags end of quoted text if already in quoted string */
      else
      {
         if( !Check_Quote( text_file, ch ) )
         {
            return NORMAL_TEXT;
         }
         else
         {
            Last_Delimeter = STOP_TEXT;
            Text_String = 0;
            return STOP_TEXT;
         }
      }
   }

   /* last in priority, plain text and delimeters.  only valid if */
   /* not in commented text or quoted text.  first check for */
   /* word/token delimeters. */
   if( strchr( Word_Delimeters, ch ) && !Text_String && !Comment_String )
   {
      Last_Char = ch;
      Last_Delimeter = WORD_DELIMETER;
      return WORD_DELIMETER;
   }

   return NORMAL_TEXT;
}


/*********************************************************************
*  Set the module's word delimeters to a list of characters.
*
*  C-Prototype:
*     void Set_Word_Delimeters( char *delimeters );
*
*  Parameters:
*     delimeter   = string specifying word delimeter characters
*
*  Returns:
*
*  Comments:
*     Sets or changes the character list used by the
*     module for identifying word/token delimeters.
*********************************************************************/
void Set_Word_Delimeters(const char* delimeters )
{
   Word_Delimeters = delimeters;
}

/*********************************************************************
*  Set the CMODE true or false.
*
*  C-Prototype:
*     void Set_CMODE( int );
*
*  Parameters:
*     int   = TRUE 1 or FALSE 0
*
*  Returns:
*
*  Comments:
*     CMODE on  = \" is an endebbed text quote
*     CMODE off = "" is an endebbed text quote
*********************************************************************/
void Set_CMODE( int boolean )
{
   C_MODE = boolean;
}


/*********************************************************************
*  Set the module's non-characters to a list of characters.
*
*  C-Prototype:
*     void Set_Non_Characters( char *non_chars );
*
*  Parameters:
*     non_chars   = string specifying non-characters
*
*  Returns:
*
*  Comments:
*     Sets or changes the character list used by the module
*     for identifying characters to always ignore.
*********************************************************************/
void Set_Non_Characters(const char* non_chars )
{
   Non_Characters = non_chars;
}


/*********************************************************************
*  Set the module's white-characters to a list of characters.
*
*  C-Prototype:
*     void Set_White_Characters( char *white_chars );
*
*  Parameters:
*     white_chars = string specifying white-characters
*
*  Returns:
*
*  Comments:
*     Sets or changes the character list used by the module
*     for identifying which characters to ignore
*     until a valid text character is found.
*********************************************************************/
void Set_White_Characters( char *white_chars )
{
   White_Characters = white_chars;
}


/*********************************************************************
*  Set the module's quoted text delimeter to a character value
*
*  C-Prototype:
*     void Set_Text_Delimeter( char *delimeters );
*
*  Parameters:
*     delimeter   = char flagging start/stop of quoted text
*
*  Returns:
*
*  Comments:
*     Sets or changes the character used by the module for
*     identifying the beginin or end of a quoted text string
*********************************************************************/
void Set_Text_Delimeter(const char* delimeters )
{
   Text_Delimeters = delimeters;
}


/*********************************************************************
*  Set the module's start and stop sequences for commented text
*
*  C-Prototype:
*     void Set_Comment( char *start_string, char *stop_string );
*
*  Parameters:
*     start_string   = char sequence flagging start of commented text
*     stop_string = char sequence flagging stop of commented text
*
*  Returns:
*
*  Comments:
*     Sets or changes the character sequences used by the module
*     for identifying the beginin and end of commented text.
*     Commented text is ignored.
*********************************************************************/
void Set_Comment( char *start_string, char *stop_string )
{
   Start_Comment = start_string;
   Stop_Comment = stop_string;
}


/*********************************************************************
*  Set the module's line counter
*     USE ONLY FOR UPDATING LINE COUNT VARIABLE AFTER SWITCHING
*     BETWEEN TEXT FILES.  IT DOES NOTHING WITH THE FILE PTR; ITS
*     NOT A SEEK FUNCTION.
*
*  C-Prototype:
*     void Set_Linecnt( long i );
*
*  Parameters:
*     i  = new line position for text file
*
*  Returns:
*
*  Comments:
*     Sets the text file line counter to a new pos.  Note, this
*     value should only be changed to update the module's line
*     counter when file pointers are changed or when the caller
*     knows for certain which line pos accurately reflects the
*     current file's file position pointer (i.e. setting the
*     file pos pointer back to the begining, SEEK_SET).  Other
*     changes will likely render this value inaccurate.
*********************************************************************/
void Set_Linecnt( long i )
{
   CTextScanner::setLineCount(i);
}


/*********************************************************************
*  Get the module's line counter
*
*  C-Prototype:
*     long Get_Linecnt( void );
*
*  Parameters:
*
*  Returns:
*     The value of the module's line counter variable
*
*  Comments:
*     This function returns the current text line in the file
*     being read (unless a call to Set_Linecnt renders it useless).
*********************************************************************/
long Get_Linecnt( )
{
   return CTextScanner::getLineCount();
}


/*********************************************************************
*  Skip to the next text line in the file
*
*  C-Prototype:
*     void Skip_Line( FILE text_file );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*
*  Returns:
*
*  Comments:
*     Moves the file pos pointer past the next new line
*     sequence '\r','\n\.
*********************************************************************/
void Skip_Line( FILE *text_file )
{
   char  ch;

   /* This is already on end of the Line */
   if (Last_Char == '\n')
   {
      Last_Char = '\0';
   }
   else
   {
      do {
         ch = fgetc( text_file );
         Char_Count++;
         New_Line = Check_NewLine( text_file, &ch );
      } while( !New_Line && (ch != EOF) );
   }
   return;
}


/*********************************************************************
*  Set the file pointers back to the begining.
*
*  C-Prototype:
*     void Reset_File( FILE text_file );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*
*  Returns:
*     0  = if file pointers are successfuly moved
*     other on error
*
*  Comments:
*     Resets the file pointers, character counter, and line
*     counter.
*********************************************************************/
int Reset_File( FILE *text_file )
{
   Char_Count = 0;
   CTextScanner::setLineCount(1);
   Comment_String = 0;  /* 1=commented text, 0=not commented */
   Text_String    = 0;  /* 1=quoted text, 0=not quoted */
   New_Line       = 0;  /* 1=end-of-line, 0=not end-of-line */

   return fseek( text_file, 0, SEEK_SET );
}


/*********************************************************************
*  Get the last delimeter character read.
*
*  C-Prototype:
*     int Delimeter( void );
*
*  Parameters:
*
*  Returns:
*     Last delimeter character read.
*
*  Comments:
*********************************************************************/
int Delimeter( )
{
   return Last_Char;
}


/*********************************************************************
*  Get the defined type of the last delimeter read.
*
*  C-Prototype:
*     int Delimeter_Type( void );
*
*  Parameters:
*
*  Returns:
*     Last delimeter type read as defined in "textscan.h".
*
*  Comments:
*********************************************************************/
int Delimeter_Type( )
{
   return Last_Delimeter;
}


/*********************************************************************
*  Get the next character and delimeter classification from
*  the current text file.
*
*  C-Prototype:
*     int Read_Char( FILE *text_file, char *ch );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     ch       = holds next character read.
*
*  Returns:
*     Delimeter classification for next character as defined
*        in "textscan.h"
*     ch    = next character read.
*
*  Comments:
*         Characters specified as non-characters are ignored
*     durring the file read for the next character.  Both
*     the character count and line count are increased as
*     needed.
*********************************************************************/
int Read_Char( FILE *text_file , char *ch )
{
   char  c;

   read_char_string = "";
   /* get next valid character form text file */
   do {
      c = fgetc( text_file );
      read_char_string += c;
      Char_Count++;
      New_Line = Check_NewLine( text_file, &c );
   } while( (strchr(Non_Characters, c) || (c == '\0')) && (c != EOF) );

   /* check for new line */
   *ch = c;
   /* return char's delimeter type to caller */
   return Character_Type( text_file, *ch );
}

/*********************************************************************
 return a string of every character 
*********************************************************************/
char *Get_ReadCharString()
{
   return read_char_string.GetBuffer(0);
}

/*********************************************************************
*  Get the next character token from the current text file.
*
*  C-Prototype:
*     int Read_Token( FILE *text_file, char *token, int max_length );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     token = character string for holding token
*     max_length= maximum character length for token
*
*  Returns:
*     Last token delimeter type read as defined in "textscan.h".
*     token = next token read from file
*
*  Comments:
*         Non-characters and preceding white characters are ignored.
*     Characters beyond the maximum length are ignored.
*********************************************************************/
int Read_Token( FILE *text_file, char *token, int max_length )
{
   char ch;
   int  delimeter;
   int  pos = 0;
   int  done = 0;
   int  text_del = 0;

   Text_String = 0;  /* Always reset text flag */

   /* skip all white chars until non-white char is found */
   do {
      delimeter = Read_Char( text_file, &ch );
   } while( strchr( White_Characters, ch ) && (ch != EOF) );

   /* read token's chars until token delimeter is found */
   while( !done && (ch != EOF) && (text_del < 2))
   {
      switch( delimeter )
      {
         case NORMAL_TEXT:
            /* if not in commented text, add character to token.  */
            /* strip control characters that get through and fill */
            /* up to length of token string. */
            if( !Comment_String && (pos < max_length) && ((unsigned char)ch > 0) )
            {
               token[ pos++ ] = ch;
            }
            /* read next character in text file because this is a delimeter */
            if (ch == '\\' && text_del && C_MODE)
            {
               /* */
               if((delimeter = Read_Char( text_file, &ch )) == STOP_TEXT)
               {
                  delimeter = NORMAL_TEXT;
                  Text_String = 1;
               }
               if (pos < max_length)   token [pos-1] = ch;
            }
            /* read next character in text file */
            delimeter = Read_Char( text_file, &ch );
            break;
         case START_COMMENT:
         case STOP_COMMENT:
         case NON_CHARACTER:
            /* these are not delimeters.  skip white spaces until */
            /* normal text begins. */
            do
            {
               delimeter = Read_Char( text_file, &ch );
            } while( strchr( White_Characters, ch ) && (ch != EOF) );
            break;
         case START_TEXT:
         case STOP_TEXT:
            text_del++;
            /* read next character in text file */
            
            if (text_del < 2)      // this is end of text
               delimeter = Read_Char( text_file, &ch );
         break;
         case WORD_DELIMETER:
         case EOF_CHARACTER:
            /* all mark end-of-token */
            done = 1;
         break;
      };
   }

   /* terminate token with null */
   token[ pos ] = '\0';

   /* return terminating delimeter to caller */
   return delimeter;
}


/*********************************************************************
*  Get the next character word from the current text file.
*
*  C-Prototype:
*     int Read_Word( FILE *text_file, char *word, int max_length );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     word     = character string for holding word
*     max_length= maximum character length for word
*
*  Returns:
*     Last token delimeter type read as defined in "textscan.h".
*     word     = next word read from file
*
*  Comments:
*     The function behaves like Read_Token except that only
*     non-empty tokens are returned. Characters beyond the
*     maximum length are ignored.
*********************************************************************/
int Read_Word( FILE *text_file, char *word, int max_length )
{
   int delimeter;

   /* find first non-empty token */
   do {
      delimeter = Read_Token( text_file, word, max_length );
   } while( (strlen( word ) == 0) && (delimeter != EOF_CHARACTER));

   /* return terminating delimeter to caller */
   return delimeter;
}


/*********************************************************************
*  Get the next character line from the current text file.
*
*  C-Prototype:
*     int Read_Line( FILE *text_file, char *line, int max_length );
*
*  Parameters:
*     text_file   = ASCII text file streem pointer
*     line     = character string for holding the text line
*     max_length= maximum character length for text line
*
*  Returns:
*     Last token delimeter type read as defined in "textscan.h".
*     line     = next text line read from file.
*
*  Comments:
*         Non-characters and preceding white characters are ignored.
*     All other characters up to the next new line character are
*     returned.  Characters beyond the maximum length are ignored.
*********************************************************************/
int Read_Line( FILE *text_file, char *string, int max_length )
{
   char ch;
   int  delimeter;
   int  pos = 0;
   int  done = 0;

   /* if Last_Char is on End of Line return */
   if( (Last_Char == '\n') || (Last_Char == '\r') )
   {
      string[0] = '\0';
      return WORD_DELIMETER;     
   }
   
   /* skip all white characters until non-white char found */
   do {
      delimeter = Read_Char( text_file, &ch );
   } while( strchr( White_Characters,ch ) && (ch != EOF) );

   /* fill text line with chars */
   while( !done && (ch != EOF) ) 
   {
      switch( delimeter )  
      {
         case NORMAL_TEXT:
         case WORD_DELIMETER:
         case START_TEXT:
         case STOP_TEXT:
            if( !Comment_String && (pos < max_length) ) 
            {
               if( New_Line ) 
               {
                  done = 1;
               }
               else 
               {
                  string[ pos++ ] = ch;
                  delimeter = Read_Char( text_file, &ch );
               }
            }
            else 
            {
               delimeter = Read_Char( text_file, &ch );
            }
            break;
         case START_COMMENT:
         case STOP_COMMENT:
         case NON_CHARACTER:
            do
            {
               delimeter = Read_Char( text_file, &ch );
            } while( strchr( White_Characters, ch ) && (ch != EOF) );
            break;
         case EOF_CHARACTER:
            done = 1;
            break;
      };
   }

   /* terminate line string with a null */
   string[ pos ] = '\0';

   return delimeter;
}

//_____________________________________________________________________________
long CTextScanner::m_lineCount = 0;

long CTextScanner::getLineCount()
{
   return m_lineCount;
}

void CTextScanner::setLineCount(long lineCount)
{
   m_lineCount = lineCount;
}

long CTextScanner::incrementLineCount()
{
   m_lineCount++;

   //if (m_lineCount == 579)
   //{
   //   int iii = 3;
   //}

   return m_lineCount;
}

/*********************************************************************
*
*  End TXTSCAN.C
*
*********************************************************************/

