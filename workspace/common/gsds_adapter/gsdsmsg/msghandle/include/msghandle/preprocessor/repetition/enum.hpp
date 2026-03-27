# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/debug/error.hpp>
# include <msghandle/preprocessor/detail/auto_rec.hpp>
# include <msghandle/preprocessor/punctuation/comma_if.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_ENUM */
#
# if 0
#    define MSGHANDLE_PP_ENUM(count, macro, data)
# endif
#
# define MSGHANDLE_PP_ENUM MSGHANDLE_PP_CAT(MSGHANDLE_PP_ENUM_, MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_REPEAT_P, 4))
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_1(c, m, d) MSGHANDLE_PP_REPEAT_1(c, MSGHANDLE_PP_ENUM_M_1, (m, d))
#    define MSGHANDLE_PP_ENUM_2(c, m, d) MSGHANDLE_PP_REPEAT_2(c, MSGHANDLE_PP_ENUM_M_2, (m, d))
#    define MSGHANDLE_PP_ENUM_3(c, m, d) MSGHANDLE_PP_REPEAT_3(c, MSGHANDLE_PP_ENUM_M_3, (m, d))
# else
#    define MSGHANDLE_PP_ENUM_1(c, m, d) MSGHANDLE_PP_ENUM_1_I(c, m, d)
#    define MSGHANDLE_PP_ENUM_2(c, m, d) MSGHANDLE_PP_ENUM_2_I(c, m, d)
#    define MSGHANDLE_PP_ENUM_3(c, m, d) MSGHANDLE_PP_ENUM_3_I(c, m, d)
#    define MSGHANDLE_PP_ENUM_1_I(c, m, d) MSGHANDLE_PP_REPEAT_1(c, MSGHANDLE_PP_ENUM_M_1, (m, d))
#    define MSGHANDLE_PP_ENUM_2_I(c, m, d) MSGHANDLE_PP_REPEAT_2(c, MSGHANDLE_PP_ENUM_M_2, (m, d))
#    define MSGHANDLE_PP_ENUM_3_I(c, m, d) MSGHANDLE_PP_REPEAT_3(c, MSGHANDLE_PP_ENUM_M_3, (m, d))
# endif
#
# define MSGHANDLE_PP_ENUM_4(c, m, d) MSGHANDLE_PP_ERROR(0x0003)
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ENUM_M_1(z, n, md) MSGHANDLE_PP_ENUM_M_1_IM(z, n, MSGHANDLE_PP_TUPLE_REM_2 md)
#    define MSGHANDLE_PP_ENUM_M_2(z, n, md) MSGHANDLE_PP_ENUM_M_2_IM(z, n, MSGHANDLE_PP_TUPLE_REM_2 md)
#    define MSGHANDLE_PP_ENUM_M_3(z, n, md) MSGHANDLE_PP_ENUM_M_3_IM(z, n, MSGHANDLE_PP_TUPLE_REM_2 md)
#    define MSGHANDLE_PP_ENUM_M_1_IM(z, n, im) MSGHANDLE_PP_ENUM_M_1_I(z, n, im)
#    define MSGHANDLE_PP_ENUM_M_2_IM(z, n, im) MSGHANDLE_PP_ENUM_M_2_I(z, n, im)
#    define MSGHANDLE_PP_ENUM_M_3_IM(z, n, im) MSGHANDLE_PP_ENUM_M_3_I(z, n, im)
# else
#    define MSGHANDLE_PP_ENUM_M_1(z, n, md) MSGHANDLE_PP_ENUM_M_1_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(2, 0, md), MSGHANDLE_PP_TUPLE_ELEM(2, 1, md))
#    define MSGHANDLE_PP_ENUM_M_2(z, n, md) MSGHANDLE_PP_ENUM_M_2_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(2, 0, md), MSGHANDLE_PP_TUPLE_ELEM(2, 1, md))
#    define MSGHANDLE_PP_ENUM_M_3(z, n, md) MSGHANDLE_PP_ENUM_M_3_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(2, 0, md), MSGHANDLE_PP_TUPLE_ELEM(2, 1, md))
# endif
#
# define MSGHANDLE_PP_ENUM_M_1_I(z, n, m, d) MSGHANDLE_PP_COMMA_IF(n) m(z, n, d)
# define MSGHANDLE_PP_ENUM_M_2_I(z, n, m, d) MSGHANDLE_PP_COMMA_IF(n) m(z, n, d)
# define MSGHANDLE_PP_ENUM_M_3_I(z, n, m, d) MSGHANDLE_PP_COMMA_IF(n) m(z, n, d)
#
# endif
