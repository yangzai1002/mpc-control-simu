/*
Copyright Ruslan Baratov 2017
Copyright Rene Rivera 2017
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_PLAT_IOS_H
#define MSGHANDLE_PREDEF_PLAT_IOS_H

#include <msghandle/predef/os/ios.h> // MSGHANDLE_OS_IOS
#include <msghandle/predef/version_number.h> // MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

/*`
[heading `MSGHANDLE_PLAT_IOS_DEVICE`]
[heading `MSGHANDLE_PLAT_IOS_SIMULATOR`]

[table
    [[__predef_symbol__] [__predef_version__]]

    [[`TARGET_IPHONE_SIMULATOR`] [__predef_detection__]]
    ]
 */

#define MSGHANDLE_PLAT_IOS_DEVICE MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE
#define MSGHANDLE_PLAT_IOS_SIMULATOR MSGHANDLE_VERSION_NUMBER_NOT_AVAILABLE

// https://opensource.apple.com/source/CarbonHeaders/CarbonHeaders-18.1/TargetConditionals.h
#if MSGHANDLE_OS_IOS
#    include <TargetConditionals.h>
#    if TARGET_IPHONE_SIMULATOR == 1
#        undef MSGHANDLE_PLAT_IOS_SIMULATOR
#        define MSGHANDLE_PLAT_IOS_SIMULATOR MSGHANDLE_VERSION_NUMBER_AVAILABLE
#    else
#        undef MSGHANDLE_PLAT_IOS_DEVICE
#        define MSGHANDLE_PLAT_IOS_DEVICE MSGHANDLE_VERSION_NUMBER_AVAILABLE
#    endif
#endif

#if MSGHANDLE_PLAT_IOS_SIMULATOR
#    define MSGHANDLE_PLAT_IOS_SIMULATOR_AVAILABLE
#    include <msghandle/predef/detail/platform_detected.h>
#endif

#if MSGHANDLE_PLAT_IOS_DEVICE
#    define MSGHANDLE_PLAT_IOS_DEVICE_AVAILABLE
#    include <msghandle/predef/detail/platform_detected.h>
#endif

#define MSGHANDLE_PLAT_IOS_SIMULATOR_NAME "iOS Simulator"
#define MSGHANDLE_PLAT_IOS_DEVICE_NAME "iOS Device"

#endif // MSGHANDLE_PREDEF_PLAT_IOS_H

#include <msghandle/predef/detail/test.h>
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_IOS_SIMULATOR,MSGHANDLE_PLAT_IOS_SIMULATOR_NAME)
MSGHANDLE_PREDEF_DECLARE_TEST(MSGHANDLE_PLAT_IOS_DEVICE,MSGHANDLE_PLAT_IOS_DEVICE_NAME)
