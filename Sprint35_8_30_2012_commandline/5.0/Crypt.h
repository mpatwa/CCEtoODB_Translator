// $Header: /CAMCAD/4.6/Crypt.h 14    1/15/07 5:08p Lynn Phung $

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.
*/           

#pragma once

#include "license.h"

//extern unsigned long apiSerialNum; // API.CPP
//extern unsigned long tempSerialNum; // LICENSE.CPP
//extern License *licenses; // LIC.CPP

//#if defined ( LicenseBypass )
//   #define get_license(i) (1)
//#elif defined ( SHAREWARE ) 
//   #define get_license(i) (licenses[i].type & ACCESS_DEFAULT)
//#else
//   #define get_license(i) \
//         ( \
//            (!(int)licenses[i].api && licenses[i].allowed == i+1) || \
//            ((int)licenses[i].api && \
//               (licenses[i].allowed == i+1 && \
//                  (licenses[i].type & ACCESS_DEFAULT || \
//                     (licenses[i].accesscode && \
//                     licenses[i].accesscode == (0xffffffff ^ licenses[i].originalaccesscode) && \
//                     (licenses[i].accesscode == crypt(apiSerialNum, i) || \
//                     licenses[i].accesscode == crypt(tempSerialNum, i)) ) \
//                  ) \
//               ) \
//            )  \
//         )
//#endif

// end CRYPT.H

/*  Project CAMCAD
    Router Solutions Inc.
    Copyright © 1994-6. All Rights Reserved.


// CRYPT.H

#pragma once

#include "license.h"

extern unsigned long serialNum; // LICENSE.CPP
extern unsigned long tempSerialNum; // LICENSE.CPP
extern License *licenses; // LIC.CPP

   #ifdef SHAREWARE // get_license(i) define
      #define get_license(i) (licenses[i].type & ACCESS_DEFAULT)
   #else
      #define get_license(i) \
         (licenses[i].allowed == i+1 && \
            (licenses[i].type & ACCESS_DEFAULT || \
               (licenses[i].accesscode && \
               licenses[i].accesscode == (0xffffffff ^ licenses[i].originalaccesscode) && \
               (licenses[i].accesscode == crypt(serialNum, i) || \
               licenses[i].accesscode == crypt(tempSerialNum, i)) ) ) )
   #endif
*/
// end CRYPT.H
