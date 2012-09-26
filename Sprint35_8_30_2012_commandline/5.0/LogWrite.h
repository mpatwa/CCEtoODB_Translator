// $Header: /CAMCAD/4.3/LogWrite.h 7     8/12/03 9:06p Kurt Van Ness $

/*****************************************************************************/
/*
   Project CAMCAD
   Router Solutions Inc.
   Copyright ©  1994-99. All Rights Reserved.
*/

#ifndef __LOGWRITE_H
#define __LOGWRITE_H

// 
int log_set_filename(const char *f);

// original used in Barcoin.cpp - an aperture is used, but not defined.
int log_undefinded_aperturenumber(FILE *fp, int appnr, long linecnt);

// original used in Padsin.cpp - a devicetype could not be mapped to an
// internal CAMCAD device type 
int log_unknown_devicetype(FILE *fp, const char *ltype, long linecnt);

// just a info line
int log_information_none(FILE *fp, const char *l, long linecnt);
int log_information_refname(FILE *fp, const char *refname, const char *l, long linecnt);

#endif

/* end logwrite.h */
