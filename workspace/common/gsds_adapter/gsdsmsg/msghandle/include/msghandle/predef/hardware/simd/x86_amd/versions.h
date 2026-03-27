/*
Copyright Charly Chevalier 2015
Copyright Joel Falcou 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef MSGHANDLE_PREDEF_HARDWARE_SIMD_X86_AMD_VERSIONS_H
#define MSGHANDLE_PREDEF_HARDWARE_SIMD_X86_AMD_VERSIONS_H

#include <msghandle/predef/version_number.h>

/*`
 Those defines represent x86 (AMD specific) SIMD extensions versions.

 [note You *MUST* compare them with the predef `MSGHANDLE_HW_SIMD_X86_AMD`.]
 */


// ---------------------------------

/*`
 [heading `MSGHANDLE_HW_SIMD_X86_AMD_SSE4A_VERSION`]

 [@https://en.wikipedia.org/wiki/SSE4##SSE4A SSE4A] x86 extension (AMD specific).

 Version number is: *4.0.0*.
 */
#define MSGHANDLE_HW_SIMD_X86_AMD_SSE4A_VERSION MSGHANDLE_VERSION_NUMBER(4, 0, 0)

/*`
 [heading `MSGHANDLE_HW_SIMD_X86_AMD_FMA4_VERSION`]

 [@https://en.wikipedia.org/wiki/FMA_instruction_set#FMA4_instruction_set FMA4] x86 extension (AMD specific).

 Version number is: *5.1.0*.
 */
#define MSGHANDLE_HW_SIMD_X86_AMD_FMA4_VERSION MSGHANDLE_VERSION_NUMBER(5, 1, 0)

/*`
 [heading `MSGHANDLE_HW_SIMD_X86_AMD_XOP_VERSION`]

 [@https://en.wikipedia.org/wiki/XOP_instruction_set XOP] x86 extension (AMD specific).

 Version number is: *5.1.1*.
 */
#define MSGHANDLE_HW_SIMD_X86_AMD_XOP_VERSION MSGHANDLE_VERSION_NUMBER(5, 1, 1)


#endif
