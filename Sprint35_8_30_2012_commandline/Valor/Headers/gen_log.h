/***********************************************************
 *
 *  Module: gen_log.h
 *
 *  Description:
 *
 *    This module includes declarations used by the gen_log.c module.
 *
 *  Creation Date:	19 Oct 93
 *  Author:		Ofer Shofman
 ***********************************************************/

#ifndef GEN_LOG_DEF
#define GEN_LOG_DEF

/* Error codes */

#define E_GEN_LOG_INTERNAL	4001
#define E_GEN_LOG_MEM		4002
#define E_GEN_LOG_SETUID	4003
#define E_GEN_LOG_OPEN		4004

/* Library general arguments */

#define Gen_logNlogPrintOff "Gen_logNlogPrintOff"

/*
 * Types of messages in the global log file
 */

typedef enum {LOG_SEV_DEBUG,		/* debug messages */
	      LOG_SEV_INFO,		/* low level, frequent information messages */
	      LOG_SEV_NOTICE,		/* high level, less-frequent information messages */
	      LOG_SEV_NOTICE_TITLED,	/* same as above, add header line before the message */
	      LOG_SEV_WARN,		/* warnings */
	      LOG_SEV_ERROR,		/* error messages */
	      LOG_SEV_FATAL		/* VERY critical error messages,
					   usually one of last actions before system crash */
} log_severity_enum;

/* Old-style messages for compatibility - try not to use */
#define LOG_MSG_NONE	"NONE"		/* converted to LOG_SEV_NOTICE */
#define LOG_MSG_ACTION	"ACTION"	/* converted to LOG_SEV_NOTICE_TITLED */
#define LOG_MSG_WARN    "WARNING"	/* converted to LOG_SEV_WARN */
#define LOG_MSG_ERR	"ERROR"		/* converted to LOG_SEV_ERROR */
#define LOG_MSG_INTERR  "INTERNAL"	/* converted to LOG_SEV_ERROR with context "INTERNAL" */
#define LOG_MSG_SQL	"SQL"		/* converted to LOG_SEV_NOTICE with context "SQL". DON'T USE!!! */
#define LOG_MSG_MEM     "MEMORY"	/* converted to LOG_SEV_NOTICE with context "MEMORY". DON'T USE!!! */

typedef struct {
  int  uni_count, tot_count;
  int  msg_td;
  int  status;
  int  line;
  char *file;
} msg_filter_struc;

#endif
