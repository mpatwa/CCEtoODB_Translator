/*
** HEADER
**    core_licensing.h -- This file contains the MGLS's core status code values
**
** COPYRIGHT
** 
** Copyright 1992-2008 Mentor Graphics Corporation
** All Rights Reserved
**
** THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY
** INFORMATION WHICH IS THE PROPERTY OF MENTOR
** GRAPHICS CORPORATION OR ITS LICENSORS AND IS
** SUBJECT TO LICENSE TERMS. 
**
** WRITTEN BY:
**   Mahmood TahanPesar
**
** DESCRIPTION
**    This file contains the #define status codes of the MGLS for Falcon 
**    applications.  It may be included by Falcon applications only.
**
** USER GUIDE
**
*/
#ifndef INCLUDED_CORE_LICENSING
#define INCLUDED_CORE_LICENSING

#ifndef INCLUDED_CORE_BASE
#include <core_base.h>
#endif

#ifndef INCLUDED_MGLS_LICENSING
#include <mgls_licensing.h>
#endif



//
// The licensing Status Codes are defined here:
// Refer to mgls_licensing.h for MGLS_* values.
// 
// The value of Core_status::BITSHIFT_SUBMOD would probably not change at all,
// But in case it does, the Core_status__BITSHIFT_SUBMOD in mgla_licensing.h 
// should also change to the same value.
//
// CORE_MODC_PFX_LICENSING  has the correct bit pattern for
// subsystem and module.
//

         /*---  CORE_MGLS_INIT  ---*/

//------------------------------------------------------------------------//
#define      Core_MGLS_problem_starting_child   (MGLS_problem_starting_child \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  When trying to start the child process that handles license
//  transcations with the FLEXlm, child process has encountered difficulties.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_no_mgls_home             (MGLS_no_mgls_home \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS needs to have the MGLS_HOME shell environment variable
//  set to the location where the MGLS tree has been installed.  This message
//  indicates that it has not been set.

//------------------------------------------------------------------------//
#define      Core_MGLS_no_falconfw_license      (MGLS_no_falconfw_license \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  Some applications may require consumption of a 'Falcon
//  FrameWork' license prior to invocation.  This message indicates that
//  this requirement can not be satisfied, because there are no valid
//  falconfw license(s) in the license files.

//------------------------------------------------------------------------//
#define      Core_MGLS_pkginfofile_corrupt      (MGLS_pkginfofile_corrupt \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS uses an encrypted binary file to place and get the
//  packaging composition information.  This message specifies that the
//  packaging information file either has been tampered with or it is not
//  supported.

//------------------------------------------------------------------------//
#define      Core_MGLS_pkginfofile_not_found    (MGLS_pkginfofile_not_found \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS uses an encrypted binary file to place and get the
//  packaging composition information.  This message specifies that the
//  packaging information file cannot be found.

//------------------------------------------------------------------------//
#define      Core_MGLS_svr_comm_failure         (MGLS_svr_comm_failure    \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This error indicates a problem that FLEXlm has encountered
//  either with a vendor daemon or the licensing daemon (lmgrd).

//------------------------------------------------------------------------//
#define      Core_MGLS_license_file_problem     (MGLS_license_file_problem\
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This error indicates that FLEXlm has encountered a syntax
//  error, a code mismatch or something of that nature.

//------------------------------------------------------------------------//
#define      Core_MGLS_unknown_attribute        (MGLS_unknown_attribute \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message specifies an attemp to set or get value for
//  an attribute that is not known.

//------------------------------------------------------------------------// 
#define      Core_MGLS_error_in_set_lou         (MGLS_error_in_set_lou \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS sets Location of Use for each invocation of any
//  application to a specific value based on the tty and/or X Display value.
//  This error indicates that the location of use could not be set easilly.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//


         /*---  CORE_MGLS_RUN   ---*/

//------------------------------------------------------------------------//
#define      Core_MGLS_malloc_failure           (Core_MGLS_malloc_failure \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS uses malloc to allocate memory for its needs.  This
//  message indicates a memory problem.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_persistent_lic_problem   (MGLS_persistent_lic_problem \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  While MGLS tries to handle licensing problems transparently
//  and without involving the application, there are times that MGLS has done
//  all possible and yetr the situation has persisted.  IN this event, the
//  application is informed and MGLS requests furthur required actions to take.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_license_request_failed   (MGLS_license_request_failed \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This error message indicates a license request failure. Name
//  of the feature (license) that the license request has failed on, is also
//  printed.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_invalid_license_version  (MGLS_invalid_license_version \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS allows newer licenses to satisfy license requests for
//  older applications.  But it does not allow a newer application to use an
//  older version of a license.  The message includes the feature name and
//  the highest version allowed.
//  The first line of message would be the English text of the FLEXlm reported
//  error.

//------------------------------------------------------------------------//
#define      Core_MGLS_subversion_detected      (MGLS_subversion_detected \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS uses a private encryption scheme included in each
//  FEATURE line (license).  This error condition indicates that such a
//  value for an specific license has been tampered with or it is incorrect.
//  The feature name from the FEATURE line that the subversion has been 
//  detected is also included in the message.

//------------------------------------------------------------------------//
#define      Core_MGLS_somesort_of_failure      (MGLS_somesort_of_failure \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  Some sort of failure that has not been identified before
//  has occured.  This is provided here to cover between release problems
//  that need to be included and reported prior to being able to change the
//  header files to acommodate this new error condition.

//------------------------------------------------------------------------//
#define      Core_MGLS_comm_failure             (MGLS_comm_failure \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This error indicates a parent/child communication problem
//  via the established pipes.

//------------------------------------------------------------------------//
#define      Core_MGLS_flexlm_failure           (MGLS_flexlm_failure      \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This could be any FLEXlm failure which has not been covered
//  by the known error conditions at this time.

//------------------------------------------------------------------------//
#define      Core_MGLS_no_license_source        (MGLS_no_license_source    \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS could not check out the requested license, because the
//  MGLS_LICENSE_SOURCE environment variable contained neither a source for
//  the license request nor the key word 'ANY'

//------------------------------------------------------------------------//
#define      Core_MGLS_license_info_error        (MGLS_license_info_error \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS could not get the information regarding the last
//  license that was checked out.

//------------------------------------------------------------------------//
#define      Core_MGLS_flexlm_server_too_old     (MGLS_flexlm_server_too_old \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  MGLS could not check out the requested license, because the
//  FLEXlm version of the server was older than the FLEXlm version of the 
//  client.


         /*---  CORE_MGLS_NOTE  ---*/

//------------------------------------------------------------------------//
#define      Core_MGLS_waiting_in_queue         (MGLS_waiting_in_queue \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message indicates that all the licenses which can be
//  used to invoke the application are in use.
//  The first line of message would be the English text of the FLEXlm reported
//  status.

//------------------------------------------------------------------------//
#define      Core_MGLS_checking_queue_status    (MGLS_checking_queue_status \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message indicates that a license request which has been
//  entered into product queues is still waiting for a license to become 
//  available.  The message includes the feature name and the time interval
//  which retries are made.
//  The first line of message would be the English text of the FLEXlm reported
//  status.

//------------------------------------------------------------------------//
#define      Core_MGLS_reconnect_in_progress    (MGLS_reconnect_in_progress \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message indicates a communication problem exists
//  between the license server and the client application.  The feature
//  name and the attempt number can also be rovided in the text.
//  The first line of message would be the English text of the FLEXlm reported
//  status.

//------------------------------------------------------------------------//
#define      Core_MGLS_reconnect_done           (MGLS_reconnect_done \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message indicates that a reconnect attemp has been
//  successful in establishing the connection between the client and server.
//  Server name and number of retries took to establish the connection once
//  again is also included in the message.
//  The first line of message would be the English text of the FLEXlm reported
//  status.

//------------------------------------------------------------------------//
#define      Core_MGLS_waiting_for_response     (MGLS_waiting_for_response \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This message indicates that the child process is busy and
//  is not responding.  Application needs to wait a short while and re-check
//  again.


//------------------------------------------------------------------------//


         /*---  CORE_MGLS_WARN  ---*/

//------------------------------------------------------------------------//
#define      Core_MGLS_trans_id_not_found       (MGLS_trans_id_not_found \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message specifies that an invalid transaction
//  is has been passed to the MGLS system.  The transcation id received by
//  MGLS is also printed.

//------------------------------------------------------------------------//
#define      Core_MGLS_exp_warning_msg          (MGLS_exp_warning_msg \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message indicates that the license is within
//  MGLS_EXP_WARN_DAYS (a shell environment variable with default of 15).
//  Name of the feature (license) and the number of days to expiration date
//  are also included.
//

//------------------------------------------------------------------------//
#define      Core_MGLS_attr_out_of_range        (MGLS_attr_out_of_range \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message indicates an out of range value for
//  a MGLS attribute value has been entered.  This message prints the out
//  of range value and the default value that was used instead.

//------------------------------------------------------------------------//
#define      Core_MGLS_queue_not_requested      (MGLS_queue_not_requested \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message indicates that while queuing was 
//  disabled by the application (via core_Mheap_set_attr call), no licenses
//  were available to satisfy the license requirement of the application.
//  Name of the feature is also printed.

//------------------------------------------------------------------------//
#define      Core_MGLS_lost_license             (MGLS_lost_license \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message indicates that a previouslly held
//  license is no longer held by the application.  Name of the license is
//  also included in the message.
//  The first line of message would be the English text of the specific
//  problem translated from FLEXlm errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_lost_connection          (MGLS_lost_connection \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning message indicates a lost connection between 
//  the license server and client.
//  The first line of message would be the English text of the specific
//  problem translated from either FLEXlm or UNIX errno.

//------------------------------------------------------------------------//
#define      Core_MGLS_somesort_of_warning      (MGLS_somesort_of_warning \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  Some sort of warning that has not been identified before
//  has occured.  This is provided here to cover between release warnings
//  that need to be included and reported prior to being able to change the
//  header files to acommodate this new warning condition.

//------------------------------------------------------------------------//
#define      Core_MGLS_wont_override_env        (MGLS_wont_override_env \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  Application is requesting to change value of an environment
//  variable that has already been set up by the end user.  MGLS would not
//  override the user's environment.

//------------------------------------------------------------------------//
#define      Core_MGLS_checkin_warning          (MGLS_checkin_warning   \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  While checking in a license or doing the cleanup, FLEXlm
//  or MGLS have encountered some problems.  The problems should be looked
//  at.  There is no failure associated with this message, it should be 
//  considered as a simple warning.

//------------------------------------------------------------------------//
#define      Core_MGLS_flexlm_warning           (MGLS_flexlm_warning    \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning covers any other warning messages that
//  FLEXlm would issues that are not known at this time.  It is a generic
//  warning message holder.

//------------------------------------------------------------------------//
#define      Core_MGLS_unknown_license_name     (MGLS_unknown_license_name    \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning lets you know that the specified license name
//  was not found in the MGLS package info file.

//------------------------------------------------------------------------//
#define      Core_MGLS_unknown_license_id       (MGLS_unknown_license_id    \
                                                 | CORE_MODC_PFX_LICENSING)
//  Description:  This warning lets you know that the specified license id
//  was not found in the MGLS package info file.


#endif

