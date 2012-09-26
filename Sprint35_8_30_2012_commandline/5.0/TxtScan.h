// $Header: /CAMCAD/4.5/TxtScan.h 11    5/24/06 8:19p Kurt Van Ness $

/*********************************************************************
**********************************************************************
*  Text scanner library header file and function prototypes
**********************************************************************
*********************************************************************/

#include <stdio.h>

/*********************************************************************
*  Text scanner module defined values
*********************************************************************/
#define DEF_WRD_DELIMS   " ,.;:[](){}<>/" /* default word delimeters */
#define DEF_WHT_CHAR    " \t\n\r"         /* default white-characters */
#define DEF_NON_CHAR     "\n\r\t\b"    /* default non-characters */
#define DEF_TXT_DELIMS   "\""          /* default text delimeters */
#define DEF_START_CMNT   "/*"          /* default start-comment */
#define DEF_STOP_CMNT    "*/"          /* default stop-comment */

#define NORMAL_TEXT  0     /* char normal ASCII text character */
#define WORD_DELIMETER  1     /* char is word delimeter */
#define START_TEXT      2     /* char is start-text character */
#define STOP_TEXT    3     /* char is stop-text character */
#define START_COMMENT   4     /* char is start-comment character */
#define STOP_COMMENT 5     /* char is stop-comment character */
#define EOL_CHARACTER   6     /* char is end-of-line character */
#define EOF_CHARACTER   7     /* char is end-of-file character */
#define NON_CHARACTER   8     /* char is non-character */


/*********************************************************************
*  Global Variables and Variable Types
*********************************************************************/
/* none */


/*********************************************************************
*  Function Prototypes
*********************************************************************/
void Set_Word_Delimeters(const char* delimeters );
void Set_Non_Characters(const char* non_chars );
void Set_White_Characters( char *white_chars );
void Set_Text_Delimeter(const char* delimeter );
void Set_Comment( char *start_string, char *stop_string );
void Set_Linecnt( long i );
void Skip_Line( FILE *text_file );
int Reset_File( FILE *text_file );
int Delimeter( void );
int Delimeter_Type( void );
long Get_Linecnt( void );
int Read_Char( FILE *text_file , char *ch );
int Read_Token( FILE *text_file, char *string, int max_length );
int Read_Word( FILE *text_file, char *string, int max_length );
int Read_Line( FILE *text_file, char *string, int max_length );
char  *Get_ReadCharString();
/********************************************************************/
/*
    End of Data.....
*/
/********************************************************************/

//_____________________________________________________________________________
class CTextScanner
{
private:
   static long m_lineCount;

public:
   static long getLineCount();
   static void setLineCount(long lineCount);
   static long incrementLineCount();

};
