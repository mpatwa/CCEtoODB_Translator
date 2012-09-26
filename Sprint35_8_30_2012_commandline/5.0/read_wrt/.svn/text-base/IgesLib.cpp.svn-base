// $Header: /CAMCAD/4.6/read_wrt/IgesLib.cpp 11    4/03/07 5:52p Lynn Phung $

/****************************************************************************/
/*
   All function return 1 for SUCCESS and 0 for ERROR

   On Polylines a block ROUNDEND is inserted to simulate
   On Polylines a block SQUAREEND is inserted to simulate
   Pen roundings
  
*/
/****************************************************************************/

#include "StdAfx.h"
#include "General.h"
#include "igeslib.h"
#include "string.h"
#include "math.h"
#include "Units.h"

typedef struct Poly_Point
{
   double x;
   double y;
   int    arc_flag;
} Poly_Point;

typedef long LONG;

#define OK  0

#define MAX_STRING_LEN 126
#define BUFFER_FILL_CHAR ' '
#define BUFFER_TERMINATOR '\0'
#define DIR_BUFFER_LEN 72
#define PARM_BUFFER_LEN 64
#define TEXT_END_CHARS "\n"
#define MAX_LAYER_NAMES 100

static char **LayerBuffer;
static int LayerCount = 0;

static char Text_Buffer[ MAX_STRING_LEN+1 ];
static int  Text_Buffer_Pos;
static long Start_Lines = 0;
static long Global_Lines = 0;
static long Directory_Lines = 0;
static long Parameter_Lines = 0;

static char Subfigure_Name[MAX_STRING_LEN];
static char Group_Name[ MAX_STRING_LEN ];
static int Hide_Active = 0;
static int Physical_Active = 0;
static int Subfigure_Active = 0;
static int Group_Active = 0;
static int Group_Count = 0;
static int Entity_Annotation = 0;
static int Entity_Definition = 0;
static int Entity_Other_Use = 0;
static int Strong_Subordinates = 0;
static int Subfigure_Count = 0;
static int Subfigure_Nesting_Count = 0;
static int Text_Orientation = 0;
static int Text_Mirroring = 0;

static char Header_Information[] = "HEADER INFORMATION";

static FILE *Iges_Main_File;
static FILE *Iges_Directory_File;
static FILE *Iges_Parameter_File;

typedef struct
{
  char   Parameter_Delimeter;
  char   Record_Delimeter;
  char   Senders_Prod_ID[ MAX_STRING_LEN ];
  char   File_Name[ MAX_STRING_LEN ];
  char   System_ID[ MAX_STRING_LEN ];
  char   Preprocessor_Vers[ MAX_STRING_LEN ];
  int    Bits_Per_Int;
  int    Max_Pwr_Single;
  int    Sig_Figs_Single;
  int    Max_Pwr_Double;
  int    Sig_Figs_Double;
  char   Receivers_Prod_ID[ MAX_STRING_LEN ];
  double Model_Space_Scale;
  int    Unit_Flag;
  char   Unit_String[ 7 ];
  int    Max_Weight_Gradiations;
  double Max_Width;
  char   Date_Time[ 14 ];
  double Min_Resolution;
  double Max_Coordinates;
  char   Authors_Name[ MAX_STRING_LEN ];
  char   Organization_Name[ MAX_STRING_LEN ];
  int    Version_Code;
  int    Drafting_Code;
} Global_Record;

static Global_Record *Global_Data;

static int AddLayerName(const char *layer )
{
   int i = 0;

   if( LayerCount < MAX_LAYER_NAMES )
   {
      LayerBuffer[LayerCount] = STRDUP(layer);
      i = LayerCount;
      LayerCount++;
   }

   return i;
}

static int FindLayerName(const char *layer )
{
   int i;

   for( i=0; i<LayerCount; i++ ) 
   {
      if( !strcmp( LayerBuffer[i], layer ) ) 
      {
         return i;
      }
   }

   return -1;
}

static void FreeMem( )
{
   int i;

   free(Global_Data);

   for( i=0; i<LayerCount; i++ ) 
   {
      if( LayerBuffer[i] != NULL )
      {
         free( LayerBuffer[i] );
      }
   }
   free( LayerBuffer );
}

static int AllocMem( )
{
   if ((Global_Data = (Global_Record *)calloc(1,sizeof(Global_Record))) == NULL)
   {
      return 1;
   }
   if( (LayerBuffer = (char **)calloc(MAX_LAYER_NAMES,sizeof(char *))) == NULL )
   {
      return 1;
   }
   LayerCount = 0;
   Start_Lines = 0;
   Global_Lines = 0;
   Directory_Lines = 0;
   Parameter_Lines = 0;

   Hide_Active = 0;
   Physical_Active = 0;
   Subfigure_Active = 0;
   Group_Active = 0;
   Group_Count = 0;
   Entity_Annotation = 0;
   Entity_Definition = 0;
   Entity_Other_Use = 0;
   Strong_Subordinates = 0;
   Subfigure_Count = 0;
   Subfigure_Nesting_Count = 0;
   Text_Orientation = 0;
   Text_Mirroring = 0;

   return 0;
}

static void Clear_Buffer() 
{
   memset( Text_Buffer, BUFFER_FILL_CHAR, MAX_STRING_LEN );
   Text_Buffer[ MAX_STRING_LEN ] = BUFFER_TERMINATOR;
   Text_Buffer_Pos = 0;
}

static void Write_Buffer( FILE *dest, int characters )
{
  fwrite( Text_Buffer, characters, 1, dest );
  if( ferror( dest ) )
  {
    printf( "ERROR: out of disk space or disk failure\n" );
    fclose( Iges_Main_File );
    fclose( Iges_Parameter_File );
    fclose( Iges_Directory_File );
    UNLINK( "$igespar.tmp" );
    UNLINK( "$igesdir.tmp" );
    exit( 0 );
  }
}

static void Buffer_String( char *string, int *string_pos, int length,
        int fragment )
{
  int string_fits = 0;

  if( (int)strlen( string ) <= ( length-1 )-Text_Buffer_Pos )
    string_fits = 1;
  while( *string_pos <= (int)strlen( string )-1 )
  {
    switch( string[ *string_pos ] )
    {
      case '\n': ;
      case '\r':
        while( Text_Buffer_Pos <= length-1 )
        {
          Text_Buffer[ Text_Buffer_Pos ] =
            BUFFER_FILL_CHAR;
          Text_Buffer_Pos++;
        }
        *string_pos = *string_pos+1;
      return;
      case '\t':
        *string_pos = *string_pos+1;
      break;
      default  :
        if( string_fits )
        {
          Text_Buffer[ Text_Buffer_Pos ] =
            string[ *string_pos ];
          Text_Buffer_Pos++;
          *string_pos = *string_pos+1;
        }
        else
        {
          if( fragment )
          {
            if( Text_Buffer_Pos <= length-1 ) 
            {
              Text_Buffer[ Text_Buffer_Pos ] =
                string[ *string_pos ];
              Text_Buffer_Pos++;
              *string_pos = *string_pos+1;
            } else
              return;
          }
          else
          {
            while( Text_Buffer_Pos <= length-1 )
            {
              Text_Buffer[ Text_Buffer_Pos ] =
                BUFFER_FILL_CHAR;
              Text_Buffer_Pos++;
            }
            return;
          }
        break;
      }
    }
  }
}

int Iges_Write_Sequence( FILE *iges_file, char section_char, long line_count )
{
  char count_string[ 9 ];

  sprintf( count_string, "%c%7ld", section_char, line_count );
  fputs(  count_string, iges_file );
  fputs( TEXT_END_CHARS, iges_file );
  if( ferror( iges_file ) ) 
  {
    printf( "ERROR: out of disk space or disk failure\n" );
    fclose( Iges_Main_File );
    fclose( Iges_Parameter_File );
    fclose( Iges_Directory_File );
    UNLINK( "$igespar.tmp" );
    UNLINK( "$igesdir.tmp" );
    exit( 0 );
  }
  return OK;
}

int Iges_Write_Header( char *header ) 
{
  int header_pos;

  Clear_Buffer();

   if (header == NULL)  return OK;

  header_pos = 0;
  while( header_pos < (int)strlen( header ) )
  {
    Buffer_String( header, &header_pos, DIR_BUFFER_LEN, 1 );
    Start_Lines++;
    Write_Buffer( Iges_Main_File, DIR_BUFFER_LEN );
    Iges_Write_Sequence( Iges_Main_File, 'S', Start_Lines );
    Clear_Buffer();
  }
  return OK;
}

void Set_Iges_Pram_Delimeter( char delimeter ) 
{
  Global_Data->Parameter_Delimeter = delimeter;
}

void Set_Iges_Rec_Delimeter( char delimeter ) 
{
  Global_Data->Record_Delimeter = delimeter;
}

void Set_Iges_Senders_Prod_ID( char *id ) 
{
  strcpy( Global_Data->Senders_Prod_ID, id );
}

void Set_Iges_File_Name( char *name ) {
  strcpy( Global_Data->File_Name, name );
}

void Set_Iges_System_ID( char *id ) 
{
  strcpy( Global_Data->System_ID, id );
}

void Set_Iges_Preprocessor_Vers( char *version ) 
{
  strcpy( Global_Data->Preprocessor_Vers, version );
}

void Set_Iges_Bits_Per_Int( int bits ) 
{
  Global_Data->Bits_Per_Int = bits;
}

void Set_Iges_Max_Pwr_Single( int max_power ) 
{
  Global_Data->Max_Pwr_Single = max_power;
}

void Set_Iges_Sig_Figs_Single( int sig_figs ) 
{
  Global_Data->Sig_Figs_Single = sig_figs;
}

void Set_Iges_Max_Pwr_Double( int max_power ) 
{
  Global_Data->Max_Pwr_Double = max_power;
}

void Set_Iges_Sig_Figs_Double( int sig_figs ) 
{
  Global_Data->Sig_Figs_Double = sig_figs;
}

void Set_Iges_Receivers_Prod_ID( char *id ) 
{
  strcpy( Global_Data->Receivers_Prod_ID, id );
}

void Set_Iges_Model_Space_Scale( double scale ) 
{
  Global_Data->Model_Space_Scale = scale;
}

int Convert_Camcad_Units(int units)
{
	// Convert values  as defined in Units.cpp to
	// units as  used in Set_Iges_Units().

	switch (units)
	{
	case pageUnitsInches:      return (1);
	case pageUnitsMils:        return (8);
	case pageUnitsMilliMeters: return (2);
	}

	return (1);  // inches
}

void Set_Iges_Units( int unit_flag ) 
{
  /* unit_flag = 3 flags a special units specification. a default
   *   conversion of inches is used for now.
   */
  if( unit_flag>=1 && unit_flag<=11 )
    Global_Data->Unit_Flag = unit_flag;
  else
    Global_Data->Unit_Flag = 1;
  switch( Global_Data->Unit_Flag ) 
  {
    case 1:  { strcpy( Global_Data->Unit_String, "INCH" ); break; };
    case 2:  { strcpy( Global_Data->Unit_String, "MM" ); break; };
    case 3:  { strcpy( Global_Data->Unit_String, "INCH" ); break; };
    case 4:  { strcpy( Global_Data->Unit_String, "FT" ); break; };
    case 5:  { strcpy( Global_Data->Unit_String, "MI" ); break; };
    case 6:  { strcpy( Global_Data->Unit_String, "M" ); break; };
    case 7:  { strcpy( Global_Data->Unit_String, "KM" ); break; };
    case 8:  { strcpy( Global_Data->Unit_String, "MIL" ); break; };
    case 9:  { strcpy( Global_Data->Unit_String, "UM" ); break; };
    case 10: { strcpy( Global_Data->Unit_String, "CM" ); break; };
    case 11: { strcpy( Global_Data->Unit_String, "UIN" ); break; };
  }
}

void Set_Iges_Max_Weight_Gradiations( int max_gradiations ) {
  Global_Data->Max_Weight_Gradiations = max_gradiations;
}

void Set_Iges_Max_Width( double max_width ) 
{
  /* max width should be expressed in terms of the units specified
   *  in the Units Flag Global Record Parameter.
   */
  Global_Data->Max_Width = max_width;
}

void Set_Iges_Date_Time( char *date_time ) {
  if( strlen( date_time ) == 13 )
    strcpy( Global_Data->Date_Time, date_time );
  else
    strcpy( Global_Data->Date_Time, "000000.000000" );
}

void Set_Iges_Min_Resolution( double min_resolution ) 
{
  Global_Data->Min_Resolution = min_resolution;
}

void Set_Iges_Max_Coordinates( double max_coordinates ) 
{
  Global_Data->Max_Coordinates = max_coordinates;
}

void wiges_Graph_Authors_Name( char *author ) 
{
  strcpy( Global_Data->Authors_Name, author );
}

void wiges_Graph_Organization_Name( char *organization ) 
{
  strcpy( Global_Data->Organization_Name, organization );
}

void Set_Iges_Version_Code( int version ) 
{
  Global_Data->Version_Code = version;
}

void Set_Iges_Drafting_Code( int drafting_code ) 
{
  Global_Data->Drafting_Code = drafting_code;
}

void Iges_Initialize_Globals()
{
  Set_Iges_Pram_Delimeter( ',' );
  Set_Iges_Rec_Delimeter( ';' );
  Set_Iges_Senders_Prod_ID( "MENTOR GRAPHICS" );
  Set_Iges_File_Name( "IGESFILE.DEF" );
  Set_Iges_System_ID( "NO SYSTEM ID" );
  Set_Iges_Preprocessor_Vers( "IGESLIB V.1" );
  Set_Iges_Bits_Per_Int( 16 );
  Set_Iges_Max_Pwr_Single( 8 );
  Set_Iges_Sig_Figs_Single( 24 );
  Set_Iges_Max_Pwr_Double( 12 );
  Set_Iges_Sig_Figs_Double( 52 );
  Set_Iges_Receivers_Prod_ID( "NO RECEIVERS PRODUCT ID" );
  Set_Iges_Model_Space_Scale( 1.0 );
  Set_Iges_Units( 1 );
  Set_Iges_Max_Weight_Gradiations( 32767 );
  Set_Iges_Max_Width( 32.767 );
  Set_Iges_Date_Time( "900218.120000" );
  Set_Iges_Min_Resolution( 1.0e-8 );
  Set_Iges_Max_Coordinates( 1000 );
  wiges_Graph_Authors_Name( "NO AUTHOR NAME" );
  wiges_Graph_Organization_Name( "NO ORGANIZATION NAME" );
  Set_Iges_Version_Code( 4 );
  Set_Iges_Drafting_Code( 0 );
}

void Iges_Build_String( char *dest, char *source ) 
{
   if ( strlen( source ) > 0 && strlen( source )<=MAX_STRING_LEN-5 ) 
   {
       sprintf(dest,"%dH%s", strlen(source), source );
   } 
   else
      strcpy( dest, "," );
}

int Iges_Buffer_Globals( FILE *iges_file, char *data,  int fragment ) 
{
  int data_pos;

  data_pos = 0;
  while( data_pos <= (int)strlen( data )-1 ) 
  {
    Buffer_String( data, &data_pos, DIR_BUFFER_LEN, fragment );
    if( Text_Buffer_Pos > DIR_BUFFER_LEN-1 ) 
    {
      Global_Lines++;
      Write_Buffer( iges_file, DIR_BUFFER_LEN );
      Iges_Write_Sequence( iges_file, 'G', Global_Lines );
      Clear_Buffer();
    }
  }
  return OK;
}

void Iges_Write_Globals( ) 
{
  char next_global[ MAX_STRING_LEN ];

  Clear_Buffer();
  sprintf( next_global, "1H%c,", Global_Data->Parameter_Delimeter );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "1H%c,", Global_Data->Record_Delimeter );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  Iges_Build_String( next_global, Global_Data->Senders_Prod_ID );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  Iges_Build_String( next_global, Global_Data->File_Name );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  Iges_Build_String( next_global, Global_Data->System_ID );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  Iges_Build_String( next_global, Global_Data->Preprocessor_Vers );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "%d,", Global_Data->Bits_Per_Int );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d,", Global_Data->Max_Pwr_Single );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d,", Global_Data->Sig_Figs_Single );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d,", Global_Data->Max_Pwr_Double );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d,", Global_Data->Sig_Figs_Double );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  Iges_Build_String( next_global, Global_Data->Receivers_Prod_ID );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "%10.4f,", Global_Data->Model_Space_Scale );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d,", Global_Data->Unit_Flag );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  Iges_Build_String( next_global, Global_Data->Unit_String );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "%d,", Global_Data->Max_Weight_Gradiations );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%10.4f,", Global_Data->Max_Width );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  Iges_Build_String( next_global, Global_Data->Date_Time );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "%10.4f,", Global_Data->Min_Resolution );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%10.4f,", Global_Data->Max_Coordinates );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  Iges_Build_String( next_global, Global_Data->Authors_Name );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  Iges_Build_String( next_global, Global_Data->Organization_Name );
  strcat( next_global, "," );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 1 );
  sprintf( next_global, "%d,", Global_Data->Version_Code );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
  sprintf( next_global, "%d;\n", Global_Data->Drafting_Code );
  Iges_Buffer_Globals( Iges_Main_File, next_global, 0 );
}

void Iges_Write_Terminator( )
{
  int i;

  fprintf( Iges_Main_File, "S%7ld", Start_Lines );
  fprintf( Iges_Main_File, "G%7ld", Global_Lines );
  fprintf( Iges_Main_File, "D%7ld", Directory_Lines );
  fprintf( Iges_Main_File, "P%7ld", Parameter_Lines );
  for( i=1; i<=5; i++ ) fprintf( Iges_Main_File, "%8s", " " );
  fprintf( Iges_Main_File, "T%7d%s", 1, TEXT_END_CHARS );
}

static struct Directory_Record 
{
  int  Nesting_Level;
  int  Entity_Number;
  long  Parameter_Pointer;
  int  Structure_Pointer;
  int  Line_Font_Pattern;
  int  Level_Pointer;
  int  View_Pointer;
  long  Trans_Matrix_Pointer;
  int  Associativity_Pointer;
  char  Status_Flags[ 9 ];
  long   First_Sequence_Number;
  /* position 11 holds the entity type number, same as position 1 */
  int  Line_Weight;
  int  Color;
  long Parameter_Lines;
  int  Form_Number;
  /* positions 16 and 17 reserved for future use */
  char  Entity_Label[ 9 ];
  int  Entity_Subscript;
  long  Second_Sequence_Number;
} Directory_Defaults, Directory_Data;

void wiges_Graph_Level(const char *level ) 
{
   int i;

   // do not store an empty layer
   if (level == NULL) 
      return;
   if (strlen(level) == 0)
      return;

   if( (i = FindLayerName(level)) == -1 ) 
   {
      i = AddLayerName( level );
   }

  Directory_Defaults.Level_Pointer = i;
}

void wiges_Graph_Color( int color )
{
  Directory_Defaults.Color = color;
}

void Set_Entity_Parm_Ptr( long parameter_pointer ) 
{
  Directory_Defaults.Parameter_Pointer = parameter_pointer;
}

void Set_Entity_Structure_Ptr( int structure ) 
{
  Directory_Defaults.Structure_Pointer= structure;
}

void Set_Entity_Line_Font( int line_font )
{
  Directory_Defaults.Line_Font_Pattern = line_font;
}

void Set_Entity_View_Pointer( int view_pointer ) 
{
  Directory_Defaults.View_Pointer = view_pointer;
}

void Set_Entity_Trans_Matrix( long matrix_pointer ) 
{
  Directory_Defaults.Trans_Matrix_Pointer = matrix_pointer;
}

void Set_Entity_Associativity( int associativity_pointer )
{
  Directory_Defaults.Associativity_Pointer = associativity_pointer;
}

void Set_Entity_Status_Flags( char *flags ) 
{
  strcpy( flags, "00000000" );
  if( Hide_Active       ) flags[ 1 ] = '1';
  if( Physical_Active   ) flags[ 3 ] = '1';
  if( Subfigure_Active  ) flags[ 3 ] = '3';
  if( Entity_Definition ) flags[ 5 ] = '2';
  if( Entity_Annotation ) flags[ 5 ] = '1';
  if( Entity_Other_Use  ) flags[ 5 ] = '3';
  if( Strong_Subordinates ) flags[ 7 ] = '1';
}

void wiges_Graph_Line_Weight( int line_weight ) 
{
  Directory_Defaults.Line_Weight = line_weight;
}

void wiges_Graph_Line_Width( double line_width ) 
{
  wiges_Graph_Line_Weight( (int) (line_width/Global_Data->Max_Width*
            Global_Data->Max_Weight_Gradiations) );
}

void Set_Entity_Parameter_Lines( long parameter_lines ) 
{
  Directory_Defaults.Parameter_Lines = parameter_lines;
}

void Set_Entity_Form_Number( int form_number ) 
{
  Directory_Defaults.Form_Number = form_number;
}

void Set_Entity_Label( char *label ) 
{
  strncpy( Directory_Defaults.Entity_Label, label, 8 );
  Directory_Defaults.Entity_Label[ 8 ] = '\0';
}

void Set_Entity_Subscript( int subscript ) {
  Directory_Defaults.Entity_Subscript = subscript;
}

void Iges_Initialize_Directory() {
  Directory_Defaults.Nesting_Level = 0;
  Set_Entity_Line_Font( 1 );
  wiges_Graph_Level( "0" );
  Set_Entity_View_Pointer( 0 );
  Set_Entity_Trans_Matrix( (LONG) 0 );
  Set_Entity_Associativity( 0 );
  Set_Entity_Status_Flags( Directory_Defaults.Status_Flags );
  Directory_Defaults.First_Sequence_Number = 0;
  wiges_Graph_Line_Weight( 0 );
  wiges_Graph_Color( 0 );
  Set_Entity_Form_Number( 0 );
  Set_Entity_Label( " " );
  Set_Entity_Subscript( 0 );
  Directory_Defaults.Second_Sequence_Number = 0;
}

void Iges_Set_Directory() {
  memcpy( &Directory_Data, &Directory_Defaults,  sizeof(Directory_Data) );
  Directory_Data.First_Sequence_Number = ++Directory_Lines;
  Directory_Data.Second_Sequence_Number = ++Directory_Lines;
}

void Iges_Write_Directory( ) {
  fprintf( Iges_Main_File, "%8d", Directory_Data.Entity_Number );
  fprintf( Iges_Main_File, "%8ld", Directory_Data.Parameter_Pointer );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Structure_Pointer );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Line_Font_Pattern );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Level_Pointer );
  fprintf( Iges_Main_File, "%8d", Directory_Data.View_Pointer );
  fprintf( Iges_Main_File, "%8ld", Directory_Data.Trans_Matrix_Pointer );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Associativity_Pointer );
  fprintf( Iges_Main_File, "%s", Directory_Data.Status_Flags );
  fprintf( Iges_Main_File, "D%7ld", Directory_Data.First_Sequence_Number );
  fprintf( Iges_Main_File, "%s", TEXT_END_CHARS );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Entity_Number );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Line_Weight );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Color );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Parameter_Lines );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Form_Number );
  fprintf( Iges_Main_File, "%16s", " " );
  fprintf( Iges_Main_File, "%8s", Directory_Data.Entity_Label );
  fprintf( Iges_Main_File, "%8d", Directory_Data.Entity_Subscript );
  fprintf( Iges_Main_File, "D%7ld", Directory_Data.Second_Sequence_Number );
  fprintf( Iges_Main_File, "%s", TEXT_END_CHARS );
  fwrite( &Directory_Data, sizeof( Directory_Data ), 1,
      Iges_Directory_File );
  if( ferror( Iges_Main_File ) ) 
  {
    printf( "ERROR: out of disk space or disk failure\n" );
    fclose( Iges_Main_File );
    fclose( Iges_Parameter_File );
    fclose( Iges_Directory_File );
    UNLINK( "$igespar.tmp" );
    UNLINK( "$igesdir.tmp" );
    exit( 0 );
  }
}

int Iges_Buffer_Parameter( FILE *iges_file, char *data, int fragment )
{
  int data_pos;

  data_pos = 0;
  while( data_pos <= (int)strlen( data )-1 )
  {
    Buffer_String( data, &data_pos, PARM_BUFFER_LEN, fragment );
    if( Text_Buffer_Pos > PARM_BUFFER_LEN-1 )
    {
      Parameter_Lines++;
      Write_Buffer( iges_file, PARM_BUFFER_LEN );
      fprintf( iges_file, "%8ld",
          Directory_Data.First_Sequence_Number );
      Iges_Write_Sequence( iges_file, 'P', Parameter_Lines );
      Clear_Buffer();
    }
  }
  return OK;
}

void Iges_Write_100( double sx, double sy, double tx, double ty,
         double cx,  double cy, double cz ) 
{
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 100;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 100 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", cz );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", cx );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", cy );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", sx );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", sy );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", tx );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f;\n", ty );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Set_Plane_XY()
{
  //Current_Plane = 0;
}

void Set_Plane_XZ() 
{
  //Current_Plane = 1;
}

void Set_Plane_YZ() 
{
  // Current_Plane = 2;
}


void Iges_Write_110( double x1, double y1, double z1,
          double x2, double y2, double z2 ) 
{
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 110;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 110 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", x1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", y1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", z1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", x2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", y2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f;\n", z2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Iges_Write_116( double x1, double y1, double z1,
          int point_symbol ) {
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 116;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 116 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", x1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", y1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", z1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%i;\n", point_symbol );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Iges_Write_120( double x1, double y1, double z1, double z2,
         double radius  ) {
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Write_110( x1, y1, z1, x1, y1, z2 );
  Iges_Write_110( x1+radius, y1, z1, x1+radius, y1, z2 );

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 120;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 120 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%i,", Directory_Lines-5 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%i,", Directory_Lines-3 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", 0.0 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f;\n", 6.283185 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Iges_Write_124( double r11, double r12, double r13, double t1,
         double r21, double r22, double r23, double t2,
         double r31, double r32, double r33, double t3 ) 
{
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 124;
  Directory_Data.Line_Weight = 0;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 124 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r11 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r12 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r13 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", t1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r21 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r22 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r23 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", t2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r31 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r32 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", r33 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f;\n", t3 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Rotation_Factors( double ax, double ay, double az, double m[][ 3 ] ) {
  m[ 0 ][ 0 ] = cos( az )*cos( ay );
  m[ 0 ][ 1 ] = -1*sin( az )*cos( ax )-cos( az )*sin( ay )*sin( ax );
  m[ 0 ][ 2 ] = sin( az )*sin( ax )-cos( az )*sin( ay )*cos( ax );
  m[ 1 ][ 0 ] = sin( az )*cos( ay );
  m[ 1 ][ 1 ] = cos( az )*cos( ax )-sin( az )*sin( ay )*sin( ax );
  m[ 1 ][ 2 ] = -1*cos( az )*sin( ax )-sin( az )*sin( ay )*cos( ax );
  m[ 2 ][ 0 ] = sin( ay );
  m[ 2 ][ 1 ] = cos( ay )*sin( ax );
  m[ 2 ][ 2 ] = cos( ay )*cos( ax );
}

int Iges_Write_106( Point2 *coordinates, int pairs, double z ) {
  char next_parameter[ MAX_STRING_LEN ];
  double x, y;
  int i;

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 106;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Form_Number = 12;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 106 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", 2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", pairs );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  for( i=1; i<=pairs; i++ ) 
  {
      x = coordinates[ i-1 ].x;
      y = coordinates[ i-1 ].y;
      sprintf( next_parameter, "%f,", x );
      Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
      sprintf( next_parameter, "%f,", y );
      Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
      if( i < pairs )
        sprintf( next_parameter, "%f,", z );
      else
        sprintf( next_parameter, "%f;\n", z );
      Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  }
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
  return OK;
}

int Iges_Write_118( Point2 *poly1, int vert1, double z1,
       Point2 *poly2, int vert2, double z2,
       double ax, double ay, double az,
       double ox, double oy, double oz ) 
{
  char next_parameter[ MAX_STRING_LEN ];
  double m[ 3 ][ 3 ];
  
  Rotation_Factors( ax, ay, az, m );
  Iges_Write_124( m[ 0 ][ 0 ], m[ 0 ][ 1 ], m[ 0 ][ 2 ], ox,
         m[ 1 ][ 0 ], m[ 1 ][ 1 ], m[ 1 ][ 2 ], oy,
         m[ 2 ][ 0 ], m[ 2 ][ 1 ], m[ 2 ][ 2 ], oz );
  /* do not display rule lines */
  Hide_Active = 1;
  Iges_Write_106( poly1, vert1, z1 );
  Iges_Write_106( poly2, vert2, z2 );
  Hide_Active = 0;
  Iges_Set_Directory();
  Directory_Data.Entity_Number = 118;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Form_Number = 1;
  Directory_Data.Trans_Matrix_Pointer = Directory_Lines-7;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 118 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%ld,", Directory_Lines-5 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%ld,", Directory_Lines-3 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", 0 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d;\n", 0 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
  return OK;
}

void Set_Text_Horizontal() 
{
  Text_Orientation = 0;
}

void Set_Text_Vertical() {
  Text_Orientation = 1;
}

void Set_Text_Mirror_Horizontal() {
  Text_Mirroring = 2;
}

void Set_Text_Mirror_Vertical() 
{
  Text_Mirroring = 1;
}

void Set_Text_Mirror_Off() 
{
  Text_Mirroring = 0;
}

void Set_Text_Style(char *style)
{
}

void Set_Mirroring_On() 
{
  // Mirroring = 1;
}

void Set_Mirroring_Off() 
{
  // Mirroring = 0;
}

void Iges_Write_212( char *general_note, double x1, double y1, double z1,
          double height, double width, double angle ) 
{
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Entity_Annotation = 1;
  Directory_Data.Entity_Number = 212;
  Directory_Data.Line_Weight = 0;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Line_Font_Pattern = 1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  if( Subfigure_Active ) Subfigure_Count++;
  if( Group_Active ) Group_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 212 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", 1 ); /* number of notes */
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", strlen( general_note ) );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", width );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", height );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", 1 ); /* font characterstic */
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", (3.1415/2) );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", angle );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", Text_Mirroring );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", Text_Orientation );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", x1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", y1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%f,", z1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Iges_Build_String( next_parameter, general_note );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 1 );
  sprintf( next_parameter, ";\n" );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 1 );
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
  Entity_Annotation = 0;
}

void Iges_Write_406_6000( char *text )
{
  char next_parameter[ MAX_STRING_LEN ];

  Iges_Set_Directory();
  Directory_Data.Entity_Number = 406;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Line_Font_Pattern = 1;
  Directory_Data.Form_Number = 6000;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  strcpy( Directory_Data.Entity_Label, Subfigure_Name );
  if( Subfigure_Active ) Subfigure_Count++;

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 406 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", 1 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Iges_Build_String( next_parameter, text );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 1 );
  sprintf( next_parameter, ";\n" );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );

  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

void Iges_Open_308(const char *name ) 
{
  if( strlen( name ) < MAX_STRING_LEN )
    strcpy( Subfigure_Name, name );
  else
    strncpy( Subfigure_Name, name, MAX_STRING_LEN-1 );
  Subfigure_Name[MAX_STRING_LEN-1] = '\0';
  Entity_Definition = 1;
  Subfigure_Active = 1;
  Subfigure_Count = 0;
  Subfigure_Nesting_Count = 0;
}

void Iges_Close_308( )
{
  char next_parameter[ MAX_STRING_LEN ];
  int i;

  Iges_Set_Directory();
  Subfigure_Active = 0;
  Directory_Data.Entity_Number = 308;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Line_Font_Pattern = 1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  CpyStr( Directory_Data.Entity_Label, Subfigure_Name, 8);

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 308 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", Subfigure_Nesting_Count );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  Iges_Build_String( next_parameter, Subfigure_Name );
  strcat( next_parameter, "," );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 1 );
  sprintf( next_parameter, "%d,", Subfigure_Count );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  for( i=1; i<Subfigure_Count; i++ ) 
  {
    sprintf( next_parameter, "%ld,",
        Directory_Data.First_Sequence_Number-i*2 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  }
  sprintf( next_parameter, "%ld;\n",
        Directory_Data.First_Sequence_Number-i*2 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );

  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
  Entity_Definition = 0;
  Subfigure_Count = 0;
  Subfigure_Nesting_Count = 0;
}

void Iges_Write_402_1( ) 
{
  char next_parameter[ MAX_STRING_LEN ];
  int i;

  Iges_Set_Directory();
  Subfigure_Active = 0;
  Directory_Data.Entity_Number = 402;
  Directory_Data.Form_Number   = 1;
  Directory_Data.Parameter_Pointer = Parameter_Lines+1;
  Directory_Data.Line_Font_Pattern = 1;
  Set_Entity_Status_Flags( Directory_Data.Status_Flags );
  strcpy( Directory_Data.Entity_Label, Subfigure_Name );

  Clear_Buffer();
  sprintf( next_parameter, "%d,", 402 );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  sprintf( next_parameter, "%d,", Group_Count );
  Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  for( i=1; i<Group_Count; i++ ) {
    sprintf( next_parameter, "%ld,",
        Directory_Data.First_Sequence_Number-(i+1)*2 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  }
  if( Group_Active ) {
    sprintf( next_parameter, "%ld,",
        Directory_Data.First_Sequence_Number-(i+1)*2 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%d,%d,%ld;\n", 0, 1,
        Directory_Data.First_Sequence_Number-2 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  }
  else {
    sprintf( next_parameter, "%ld;\n",
        Directory_Data.First_Sequence_Number-(i+1)*2 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
  }
  Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
  Iges_Write_Directory();
}

int Iges_Find_Subfigure(const char *name, long *dir_entry, int *nesting ) 
{

  fseek( Iges_Directory_File, (LONG) 0, SEEK_SET );
  do {
    if( fread( &Directory_Data, sizeof( Directory_Data ), 1,
        Iges_Directory_File ) ) 
    {
      if( strncmp( Directory_Data.Entity_Label, name, 8 )==0 ) 
      {
        fseek( Iges_Directory_File, (LONG) 0, SEEK_END );
        *dir_entry = Directory_Data.First_Sequence_Number;
        if( Directory_Data.Nesting_Level >
          Subfigure_Nesting_Count )
          *nesting = Directory_Data.Nesting_Level;
        return OK;
      }
    }
    else 
    {
      fseek( Iges_Directory_File, (LONG) 0, SEEK_END );
      *dir_entry = 0;
      return OK;
    }
  } while( !feof( Iges_Directory_File ) );
  fseek( Iges_Directory_File, (LONG) 0, SEEK_END );
  *dir_entry = 0;
  return OK;
}

void Iges_Write_408(const char *name,
              double x1, double y1, double z1,
              double angle, int mirror, double scale ) 
{
  char next_parameter[ MAX_STRING_LEN ];
  long dir_entry;
  int nesting = 0;
  int sign;
  int rotate = 0;

  Iges_Find_Subfigure( name, &dir_entry, &nesting );
  if( dir_entry > 0 ) {
    if( ((angle > 0.01) || (angle < -0.01)) || mirror ) 
    {
      rotate = 1;
      if( mirror )
        sign = -1;
      else
        sign = 1;
      Physical_Active = 1;
      Iges_Write_124( sign*cos( angle ), -1*sin( angle ), 0.0, x1,
             sin( angle ), cos( angle ), 0.0, y1,
             0.0, 0.0, 1, z1 );
      x1 = y1 = z1 = 0;
      Physical_Active = 0;
    }

    Iges_Set_Directory();
    Directory_Data.Entity_Number = 408;
    Directory_Data.Parameter_Pointer = Parameter_Lines+1;
    Directory_Data.Line_Font_Pattern = 1;
    if( rotate ) Directory_Data.Trans_Matrix_Pointer =
        Directory_Data.First_Sequence_Number-2;
    Set_Entity_Status_Flags( Directory_Data.Status_Flags );
    if( Subfigure_Active ) {
      Subfigure_Count++;
      Subfigure_Nesting_Count = nesting+1;
    }
    if( Group_Active ) Group_Count++;

    Clear_Buffer();
    sprintf( next_parameter, "%d,", 408 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%ld,", dir_entry ); /* 308 dir position */
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%f,", x1 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%f,", y1 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%f,", z1 );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    sprintf( next_parameter, "%f;\n", scale );
    Iges_Buffer_Parameter( Iges_Parameter_File, next_parameter, 0 );
    Directory_Data.Parameter_Lines = Parameter_Lines-
                Directory_Data.Parameter_Pointer+1;
    Iges_Write_Directory();
  }
}

void Iges_Open_Files( FILE *iges_out_file ) {
  Iges_Main_File = iges_out_file;
  if ((Iges_Directory_File = fopen( "$igesdir.tmp", "w+b" )) == NULL) {
    fprintf(stderr, "ERROR: Unable to open [$igesdir.tmp]\n");
    exit(-1);
  }

  if ((Iges_Parameter_File = fopen( "$igespar.tmp", "w+t" )) == NULL) {
    fprintf(stderr, "ERROR: Unable to open [$igesdir.tmp]\n");
    exit(-1);
  }
}

void Iges_Merge_Files() 
{
  int c;

  rewind( Iges_Parameter_File );
  while( (c = fgetc( Iges_Parameter_File )) != EOF )
  {
    fputc( c, Iges_Main_File );
    if( ferror( Iges_Main_File ) || ferror( Iges_Parameter_File ) ) 
    {
      printf( "ERROR: out of disk space or disk failure\n" );
      fclose( Iges_Main_File );
      fclose( Iges_Parameter_File );
      fclose( Iges_Directory_File );
      UNLINK( "$igespar.tmp" );
      UNLINK( "$igesdir.tmp" );
      exit( 0 );
    }
  }
}

void Iges_Close_Files()
{
  Iges_Merge_Files();
  Iges_Write_Terminator();
  fclose( Iges_Main_File );
  fclose( Iges_Parameter_File );
  fclose( Iges_Directory_File );
  UNLINK( "$igespar.tmp" );
  UNLINK( "$igesdir.tmp" );
}


/*******************************/

int wiges_Graph_Init(int unitsFlag)
{
  AllocMem();
  Iges_Initialize_Globals();
  Iges_Initialize_Directory();

  // Case 1671, Units setting in format dialog was being ignored
  Set_Iges_Units(Convert_Camcad_Units(unitsFlag));

  // Case 1671, Max Width is probably a useless value that no one uses.
  // But Kevin is concerned that it looks like an extent that is not getting
  // Units conversion. So convert it.
  double maxWidth = 32.767;
  if (unitsFlag == pageUnitsMils)
	  maxWidth *= 1000.0;
  else if (unitsFlag == pageUnitsMilliMeters)
	  maxWidth *= 25.4;

  Set_Iges_Max_Width( maxWidth );

  return OK;
}

int wiges_Graph_File_Open( FILE *iges_out_file, char *extra_header_info )
{
  Iges_Open_Files( iges_out_file );
  Iges_Write_Header( Header_Information );
  Iges_Write_Header( extra_header_info );
  Iges_Write_Globals();
  return OK;
}

int wiges_Graph_File_Close()
{
  FreeMem( );
  Iges_Close_Files();
  return OK;
}

int wiges_Graph_Donut( double center_x, double center_y, double center_z,
         double radius, double innerradius )
{
  if( radius == 0.0 )
    return OK;
  Iges_Write_100( center_x+radius, center_y, center_x+radius,
         center_y, center_x, center_y, center_z );

  return OK;
}

int wiges_Graph_Circle( double center_x, double center_y, double center_z,
      double radius )
{
  if( radius == 0.0 )
    return OK;
  Iges_Write_100( center_x+radius, center_y, center_x+radius,
         center_y, center_x, center_y, center_z );
  return OK;
}

/****************************************************************************/
/*
   Arc is always counter clock
*/
int wiges_Graph_Arc( double center_x, double center_y, double center_z, double radius,
               double startangle, double deltaangle )
{
   double stop_x, stop_y;
   double start_x, start_y;

   if (RadToDeg(deltaangle) < 0)
   {
      start_x = radius*cos(startangle+deltaangle) + center_x;
      start_y = radius*sin(startangle+deltaangle) + center_y;
      stop_x = radius*cos(startangle) + center_x;
      stop_y = radius*sin(startangle) + center_y;
   }
   else
   {
      stop_x = radius*cos(startangle+deltaangle) + center_x;
      stop_y = radius*sin(startangle+deltaangle) + center_y;
      start_x = radius*cos(startangle) + center_x;
      start_y = radius*sin(startangle) + center_y;
   }

  Iges_Write_100( start_x, start_y, stop_x, stop_y,
         center_x, center_y, center_z );
  return OK;
}

int wiges_Graph_Line( double x1, double y1, double z1, double x2, double y2,
      double z2 )
{

  if( (x1 == x2) && (y1 == y2) && (z1 == z2) )
    return OK;
  Iges_Write_110( x1, y1, z1, x2, y2, z2 );
  return OK;
}

int wiges_Graph_Point( double x1, double y1, double z1 )
{
  Iges_Write_116( x1, y1, z1, 0 );
  return OK;
}

int wiges_Graph_Text( char *text, double x1, double y1, double z1,
      double height, double width, double angle, int mirror )
{
   /* width = character width. */
   if( strlen( text ) == 0 )
      return OK;

   int oldmirror = Text_Mirroring;

   if (mirror)
      Set_Text_Mirror_Vertical();

   Iges_Write_212( text, x1, y1, z1, height, strlen(text)*width, angle );

   Text_Mirroring = oldmirror;

   return OK;
}

int wiges_Graph_Block_On(const char *block_name )
{
  Iges_Open_308( block_name );
  return OK;
}

int wiges_Graph_Block_Off( )
{
  Iges_Close_308( );
  return OK;
}

int wiges_Graph_Group_On( char *group )
{
  Group_Active = 1;
  Group_Count = 0;
  strncpy( Group_Name, group, MAX_STRING_LEN-1 );
  Group_Name[ MAX_STRING_LEN-1 ] = '\0';
  return OK;
}

int wiges_Graph_Group_Off( )
{
  Iges_Write_406_6000( Group_Name );
  Iges_Write_402_1( );
  Group_Active = 0;
  Group_Count = 0;
  Group_Name[ 0 ] = '\0';
  return OK;
}

int wiges_Graph_Block_Reference(const char *block_name,
                double x1, double y1, double z1,
                double angle, int mirror, double scale)
{
  Iges_Write_408( block_name, x1, y1, z1, angle, mirror, scale );
  return OK;
}

int wiges_Graph_Block_Reference_Layer(const char *layer_name,
                const char *block_name,
                double x1, double y1, 
                double angle, int mirror, double scale )
{
  Iges_Write_408( block_name, x1, y1, 0.0, angle, mirror, scale );
  return OK;
}

int wiges_Graph_Wire_3D( double z1, double z2, Point2 *p,
             int pairs ) 
{
  int i;

  for( i=1; i<pairs; i++ ) 
  {
    Iges_Write_110( p[i-1].x, p[i-1].y, z1, p[i].x, p[i].y, z1 );
    Iges_Write_110( p[i-1].x, p[i-1].y, z2, p[i].x, p[i].y, z2 );
    Iges_Write_110( p[i-1].x, p[i-1].y, z1, p[i-1].x,p[i-1].y,z2 );
  }
  return OK;
}

int wiges_Graph_Polyline( Point2 *coordinates, int pairs, double z, int filled)
{
   int i;

   for (i=1;i<pairs;i++)
   {
      double da = atan(coordinates[i-1].bulge) * 4;
      if (fabs(da) > SMALLANGLE)
      {
         double cx,cy,r,sa;
         double x1,y1,x2,y2;

         x1 = coordinates[i-1].x;
         y1 = coordinates[i-1].y;
         x2 = coordinates[i].x;
         y2 = coordinates[i].y;

         ArcPoint2Angle(x1,y1,x2,y2,da,&cx,&cy,&r,&sa);
         // make positive start angle.
         if (sa < 0) sa += PI2;
         wiges_Graph_Arc( cx, cy, 0.0, r, sa, da);
      }
      else
      {
         Iges_Write_110( coordinates[i-1].x, coordinates[i-1].y, 0.0,
             coordinates[i].x, coordinates[i].y, 0.0) ;
      }
   }
  // Iges_Write_106( coordinates, pairs, z );
  return OK;
}

int wiges_Graph_Seqend()
{
   return OK;
}

void wiges_Graph_Line_End( int onoff )
{
}

int wiges_Graph_Aperture( int type, double x, double y, double sizeA, double sizeB )
{
   Point2   points[10];

   switch (type)
   {
      case T_DONUT:
         wiges_Graph_Donut( x, y, 0.0, sizeA/2, sizeB/2);
      break;
      case T_ROUND:
      {
         wiges_Graph_Circle( x, y, 0.0, sizeA/2);
      }
      break;
      case T_SQUARE:
         sizeB = sizeA; // rectangle where sizeB = sizeA
         points[0].x = x-sizeA/2;
         points[0].y = y+sizeB/2;
         points[0].bulge = 0;
         points[1].x = x+sizeA/2;
         points[1].y = y+sizeB/2;
         points[1].bulge = 0;
         points[2].x = x+sizeA/2;
         points[2].y = y-sizeB/2;
         points[2].bulge = 0;
         points[3].x = x-sizeA/2;
         points[3].y = y-sizeB/2;
         points[3].bulge = 0;
         points[4].x = x-sizeA/2;
         points[4].y = y+sizeB/2;
         points[4].bulge = 0;

         wiges_Graph_Polyline( points, 5, 0.0, 1);
      break;
      case T_RECTANGLE:
      {
         double   x0, y0, x1,y1,x2,y2;
         double   rotation = 0;
         x1 = -sizeA/2;
         x2 =  sizeA/2;
         y1 = -sizeB/2;
         y2 =  sizeB/2;

         Rotate(x1,y1,RadToDeg(rotation),&x0, &y0);
         x1 = x+x0;
         y1 = y+y0;
         Rotate(x2,y2,RadToDeg(rotation),&x0, &y0);
         x2 = x+x0;
         y2 = y+y0;

         points[0].x = x1;
         points[0].y = y1;
         points[0].bulge = 0;
         points[1].x = x2;
         points[1].y = y1;
         points[1].bulge = 0;
         points[2].x = x2;
         points[2].y = y2;
         points[2].bulge = 0;
         points[3].x = x1;
         points[3].y = y2;
         points[3].bulge = 0;
         points[4].x = x1;
         points[4].y = y1;
         points[4].bulge = 0;

         wiges_Graph_Polyline( points, 5, 0.0, 1);
      }
      break;
      case T_OBLONG:
      {
/*
         double   x0, y0, x1,y1,x2,y2;
         double   width;
         Dxf_Poly(fp,Dxf_Level,0);
         Dxf_Handle(fp);

         // rotation
         width = sizeB;

         x1 = -(sizeA - sizeB)/2 + xoffset;
         x2 =  (sizeA - sizeB)/2 + xoffset;

         y1 = yoffset;
         y2 = yoffset;

         Rotate(x1,y1,RadToDeg(rotation),&x0, &y0);
         x1 = x+x0;
         y1 = y+y0;
         Rotate(x2,y2,RadToDeg(rotation),&x0, &y0);
         x2 = x+x0;
         y2 = y+y0;

         Dxf_Width(fp,sizeB);
         Dxf_Vertex(fp,Dxf_Level,x1,y1, 0.0); // left
         Dxf_Vertex(fp,Dxf_Level,x2,y2, 0.0); // right

         Dxf_Seqend(fp,Dxf_Level);
*/
      }
      break;
      case T_OCTAGON:
      {
/*
         Dxf_Poly(fp,Dxf_Level,1);
         Dxf_Width(fp,Dxf_Linewidth);
         Dxf_Handle(fp);
         
         double radius = sizeA/2;
         double halfSide = sizeA/4.83; // size of half of one side of octagon
         Dxf_Vertex(fp, Dxf_Level, x - radius, y + halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - halfSide, y + radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + halfSide, y + radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + radius, y + halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + radius, y - halfSide, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x + halfSide, y - radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - halfSide, y - radius, 0.0);
         Dxf_Vertex(fp, Dxf_Level, x - radius, y - halfSide, 0.0);
         Dxf_Seqend(fp,Dxf_Level);
*/
      }
      break;
      case T_TARGET:
      case T_THERMAL:
      case T_UNDEFINED:
      case T_COMPLEX:
      default:
      break;
   }
   return OK;
}

// end IGESLIB.CPP

