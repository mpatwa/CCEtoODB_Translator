/*
** HEADER
**    mgls_licensing.h --  This file contains the MGLS's core status values
**                         and some data structure definitions.
**
** COPYRIGHT
** 
** Copyright 1992-2011 Mentor Graphics Corporation
** All Rights Reserved
**
** THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY
** INFORMATION WHICH IS THE PROPERTY OF MENTOR
** GRAPHICS CORPORATION OR ITS LICENSORS AND IS
** SUBJECT TO LICENSE TERMS. 
**
**
** WRITTEN BY:
**   Mahmood TahanPesar
**
** DESCRIPTION
**    This file contains the #defines for the lower 4 bytes of the MGLS error 
**    codes and contains the MGLS's error structure definition.  This file is
**    included by the core_licensing.h.
**
** USER GUIDE
**
*/
#ifndef INCLUDED_MGLS_LICENSING
#define INCLUDED_MGLS_LICENSING

#define     MAX_UFARG_LEN  82
#define     MAX_ERRARG_LEN 35


struct MGLS_ERRNO_STRUCT {
          long  mgls_errno;               /*  MGLS error code               */
          char  uferrs[MAX_UFARG_LEN+6];  /*  FLEXlm/UNIX error string      */
                                 /* +6 characters for "\n//  " prefix       */
          char  arg1[MAX_ERRARG_LEN];     /*  First substitution argument   */
          char  arg2[MAX_ERRARG_LEN];     /*  Second substitution argument  */
          };


/*
 *  The value of Core_status::BITSHIFT_SUBMOD would probably not change at all,
 *  But in case it does, the following #define should follow the change.
 */
#define Core_status__BITSHIFT_SUBMOD 8



#define    MGLS_INIT       (0x01 << Core_status__BITSHIFT_SUBMOD )
#define    MGLS_RUN        (0x02 << Core_status__BITSHIFT_SUBMOD )
#define    MGLS_NOTE       (0x03 << Core_status__BITSHIFT_SUBMOD )
#define    MGLS_WARN       (0x04 << Core_status__BITSHIFT_SUBMOD )




#define       MGLS_problem_starting_child       (0x01 | MGLS_INIT)
#define       MGLS_no_mgls_home                 (0x02 | MGLS_INIT)
#define       MGLS_no_falconfw_license          (0x03 | MGLS_INIT)
#define       MGLS_pkginfofile_corrupt          (0x04 | MGLS_INIT)
#define       MGLS_svr_comm_failure             (0x05 | MGLS_INIT)
#define       MGLS_license_file_problem         (0x06 | MGLS_INIT)
#define       MGLS_unknown_attribute            (0x81 | MGLS_INIT)
#define       MGLS_error_in_set_lou             (0x82 | MGLS_INIT)
#define       MGLS_pkginfofile_not_found        (0x83 | MGLS_INIT)

#define       MGLS_malloc_failure               (0x01 | MGLS_RUN)
#define       MGLS_persistent_lic_problem       (0x02 | MGLS_RUN)
#define       MGLS_license_request_failed       (0x03 | MGLS_RUN)
#define       MGLS_invalid_license_version      (0x04 | MGLS_RUN)
#define       MGLS_subversion_detected          (0x05 | MGLS_RUN)
#define       MGLS_somesort_of_failure          (0x06 | MGLS_RUN)
#define       MGLS_comm_failure                 (0x07 | MGLS_RUN)
#define       MGLS_flexlm_failure               (0x08 | MGLS_RUN)
#define       MGLS_no_license_source            (0x09 | MGLS_RUN)
#define       MGLS_license_info_error           (0x10 | MGLS_RUN)
#define       MGLS_flexlm_server_too_old        (0x20 | MGLS_RUN)
#define       MGLS_maxusers                     (0x21 | MGLS_RUN)
#define       MGLS_some_ok                      (0x22 | MGLS_RUN)
#define       MGLS_license_excluded             (0X23 | MGLS_RUN)

#define       MGLS_waiting_in_queue             (0x81 | MGLS_NOTE)
#define       MGLS_checking_queue_status        (0x82 | MGLS_NOTE)
#define       MGLS_reconnect_in_progress        (0x83 | MGLS_NOTE)
#define       MGLS_reconnect_done               (0x84 | MGLS_NOTE)
#define       MGLS_waiting_for_response         (0x85 | MGLS_NOTE)

#define       MGLS_trans_id_not_found           (0x81 | MGLS_WARN)
#define       MGLS_exp_warning_msg              (0x82 | MGLS_WARN)
#define       MGLS_attr_out_of_range            (0x83 | MGLS_WARN)
#define       MGLS_queue_not_requested          (0x84 | MGLS_WARN)
#define       MGLS_lost_license                 (0x85 | MGLS_WARN)
#define       MGLS_lost_connection              (0x86 | MGLS_WARN)
#define       MGLS_somesort_of_warning          (0x87 | MGLS_WARN)
#define       MGLS_wont_override_env            (0x88 | MGLS_WARN)
#define       MGLS_checkin_warning              (0x89 | MGLS_WARN)
#define       MGLS_flexlm_warning               (0x8a | MGLS_WARN)
#define       MGLS_unknown_license_name         (0x8b | MGLS_WARN)
#define       MGLS_unknown_license_id           (0x8c | MGLS_WARN)
#define       MGLS_invalid_version_num          (0x8d | MGLS_WARN)
#define       MGLS_value_out_of_range           (0x90 | MGLS_WARN)
#define       MGLS_values_out_of_range          (0x91 | MGLS_WARN)
#define       MGLS_quantity_exceeds_checkedout  (0x92 | MGLS_WARN)
#define       MGLS_lost_features_at_freeq       (0x93 | MGLS_WARN)
#define       MGLS_not_atomic_id                (0x94 | MGLS_WARN)

#define       MGLS_end_of_status_codes          0

#endif
