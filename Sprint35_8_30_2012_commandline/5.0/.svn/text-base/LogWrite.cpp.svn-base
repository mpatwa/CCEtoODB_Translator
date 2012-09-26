// $Header: /CAMCAD/4.3/LogWrite.cpp 7     8/12/03 4:20p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-98. All Rights Reserved.

;   A semi-colon depicts a comment to the end of the line
;   Only lines which begin with a period "." are evaluated as a classified log file entry.
;   All other lines are considered a non-classified basic log file entry. 

;   Sample Log File Line
   

;   .nn:nn [TypeName] message to end of line
 
;   .nnn = Error Level
;   .0000 = none
;   .1000 = Information
;   .2000 = Warning
;   .3000 = Critical
;   .4000 = Fatal
;   .5000 = Debug Information

;   :nn = Error Code
;   :00 = none
;   :10 = ReferenceDesignator
;   :11 = ReferenceDesignator.PinNumber
;   :20 = GeometryName
;   :21 = GeometryName.PinNumber
;   :30 = NetName

.1001:10 [E200] Geometry [TERMINAL] Component [E200] has movable pins -> changed to [TERMINAL_E200]
.1001:10 [E300] Geometry [TERMINAL] Component [E300] has movable pins -> changed to [TERMINAL_E300]
.1001:10 [E301] Geometry [TERMINAL] Component [E301] has movable pins -> changed to [TERMINAL_E301]
.1001:10 [E54] Geometry [TERMINAL] Component [E54] has movable pins -> changed to [TERMINAL_E54]
.1001:10 [E55] Geometry [TERMINAL] Component [E55] has movable pins -> changed to [TERMINAL_E55]
.5000:00 This is a debug line
.5000:10 [U700] U700 has a big problem!

This is a non-classified line.

*/
                       
// LOGWRITE.CPP

#include "stdafx.h"

#include "logwrite.h"

static   CString  filename;

/****************************************************************************/
/* 
*/
int log_set_filename(const char *f)
{
   filename = f;
   return 1;
}

/****************************************************************************/
/* 
*/
int log_undefinded_aperturenumber(FILE *ferr, int appnr, long linecnt)
{
   fprintf(ferr,".3000:00 [%ld] [%s] Aperture [%d] selected but not defined at %ld\n", 
      linecnt, filename, appnr);
   return 1;
}

/****************************************************************************/
/* 
*/
int log_unknown_devicetype(FILE *ferr, const char *ltype, long linecnt)
{
   fprintf(ferr,".3000:00 [%ld] [%s] Device Type [%s] is not known to CAMCAD at %ld\n", 
      linecnt, filename, ltype);
   return 1;
}

/****************************************************************************/
/* 
*/
int log_information_none(FILE *ferr, const char *ltype, long linecnt)
{
   fprintf(ferr,".1000:00 [%ld] [%s] %s\n", linecnt, filename, ltype);
   return 1;
}

/****************************************************************************/
/* 
*/
int log_information_refname(FILE *ferr, const char *refname, const char *ltype, long linecnt)
{
   fprintf(ferr,".1000:10 [%ld] [%s] [%s] %s\n", linecnt, filename, refname, ltype);
   return 1;
}

/*****************************************************************************/
/*
   end logwrite.cpp
*/
/*****************************************************************************/