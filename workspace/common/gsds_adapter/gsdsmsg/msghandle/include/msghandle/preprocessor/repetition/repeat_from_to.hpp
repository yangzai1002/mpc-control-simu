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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_REPEAT_FROM_TO_HPP
#
# include <msghandle/preprocessor/arithmetic/add.hpp>
# include <msghandle/preprocessor/arithmetic/sub.hpp>
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/debug/error.hpp>
# include <msghandle/preprocessor/detail/auto_rec.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_REPEAT_FROM_TO */
#
# if 0
#    define MSGHANDLE_PP_REPEAT_FROM_TO(first, last, macro, data)
# endif
#
# define MSGHANDLE_PP_REPEAT_FROM_TO MSGHANDLE_PP_CAT(MSGHANDLE_PP_REPEAT_FROM_TO_, MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_REPEAT_P, 4))
#
# define MSGHANDLE_PP_REPEAT_FROM_TO_1(f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_1(MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_WHILE_P, 256), f, l, m, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_2(f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_2(MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_WHILE_P, 256), f, l, m, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_3(f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_3(MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_WHILE_P, 256), f, l, m, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_4(f, l, m, dt) MSGHANDLE_PP_ERROR(0x0003)
#
# define MSGHANDLE_PP_REPEAT_FROM_TO_1ST MSGHANDLE_PP_REPEAT_FROM_TO_1
# define MSGHANDLE_PP_REPEAT_FROM_TO_2ND MSGHANDLE_PP_REPEAT_FROM_TO_2
# define MSGHANDLE_PP_REPEAT_FROM_TO_3RD MSGHANDLE_PP_REPEAT_FROM_TO_3
#
# /* MSGHANDLE_PP_REPEAT_FROM_TO_D */
#
# if 0
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D(d, first, last, macro, data)
# endif
#
# define MSGHANDLE_PP_REPEAT_FROM_TO_D MSGHANDLE_PP_CAT(MSGHANDLE_PP_REPEAT_FROM_TO_D_, MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_REPEAT_P, 4))
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_1(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_2(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_3(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# else
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_1(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_2(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_3(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_1_I(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_1(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_1, (d, f, m, dt))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_2_I(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_2(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_2, (d, f, m, dt))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_D_3_I(d, f, l, m, dt) MSGHANDLE_PP_REPEAT_3(MSGHANDLE_PP_SUB_D(d, l, f), MSGHANDLE_PP_REPEAT_FROM_TO_M_3, (d, f, m, dt))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_1_IM(z, n, MSGHANDLE_PP_TUPLE_REM_4 dfmd)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_2_IM(z, n, MSGHANDLE_PP_TUPLE_REM_4 dfmd)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_3_IM(z, n, MSGHANDLE_PP_TUPLE_REM_4 dfmd)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_1_IM(z, n, im) MSGHANDLE_PP_REPEAT_FROM_TO_M_1_I(z, n, im)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_2_IM(z, n, im) MSGHANDLE_PP_REPEAT_FROM_TO_M_2_I(z, n, im)
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_3_IM(z, n, im) MSGHANDLE_PP_REPEAT_FROM_TO_M_3_I(z, n, im)
# else
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_1(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_1_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(4, 0, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 1, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 2, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 3, dfmd))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_2(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_2_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(4, 0, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 1, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 2, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 3, dfmd))
#    define MSGHANDLE_PP_REPEAT_FROM_TO_M_3(z, n, dfmd) MSGHANDLE_PP_REPEAT_FROM_TO_M_3_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(4, 0, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 1, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 2, dfmd), MSGHANDLE_PP_TUPLE_ELEM(4, 3, dfmd))
# endif
#
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_1_I(z, n, d, f, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_M_1_II(z, MSGHANDLE_PP_ADD_D(d, n, f), m, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_2_I(z, n, d, f, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_M_2_II(z, MSGHANDLE_PP_ADD_D(d, n, f), m, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_3_I(z, n, d, f, m, dt) MSGHANDLE_PP_REPEAT_FROM_TO_M_3_II(z, MSGHANDLE_PP_ADD_D(d, n, f), m, dt)
#
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_1_II(z, n, m, dt) m(z, n, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_2_II(z, n, m, dt) m(z, n, dt)
# define MSGHANDLE_PP_REPEAT_FROM_TO_M_3_II(z, n, m, dt) m(z, n, dt)
#
# endif
