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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_LIST_FOR_EACH_I_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/repetition/for.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_FOR_EACH_I */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG() && ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_LIST_FOR_EACH_I(macro, data, list) MSGHANDLE_PP_FOR((macro, data, list, 0), MSGHANDLE_PP_LIST_FOR_EACH_I_P, MSGHANDLE_PP_LIST_FOR_EACH_I_O, MSGHANDLE_PP_LIST_FOR_EACH_I_M)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_I(macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I_I(macro, data, list)
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_I(macro, data, list) MSGHANDLE_PP_FOR((macro, data, list, 0), MSGHANDLE_PP_LIST_FOR_EACH_I_P, MSGHANDLE_PP_LIST_FOR_EACH_I_O, MSGHANDLE_PP_LIST_FOR_EACH_I_M)
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_P(r, x) MSGHANDLE_PP_LIST_FOR_EACH_I_P_D x
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_P_D(m, d, l, i) MSGHANDLE_PP_LIST_IS_CONS(l)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_P(r, x) MSGHANDLE_PP_LIST_IS_CONS(MSGHANDLE_PP_TUPLE_ELEM(4, 2, x))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_O(r, x) MSGHANDLE_PP_LIST_FOR_EACH_I_O_D x
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_O_D(m, d, l, i) (m, d, MSGHANDLE_PP_LIST_REST(l), MSGHANDLE_PP_INC(i))
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_O(r, x) (MSGHANDLE_PP_TUPLE_ELEM(4, 0, x), MSGHANDLE_PP_TUPLE_ELEM(4, 1, x), MSGHANDLE_PP_LIST_REST(MSGHANDLE_PP_TUPLE_ELEM(4, 2, x)), MSGHANDLE_PP_INC(MSGHANDLE_PP_TUPLE_ELEM(4, 3, x)))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_M(r, x) MSGHANDLE_PP_LIST_FOR_EACH_I_M_D(r, MSGHANDLE_PP_TUPLE_ELEM(4, 0, x), MSGHANDLE_PP_TUPLE_ELEM(4, 1, x), MSGHANDLE_PP_TUPLE_ELEM(4, 2, x), MSGHANDLE_PP_TUPLE_ELEM(4, 3, x))
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_M(r, x) MSGHANDLE_PP_LIST_FOR_EACH_I_M_I(r, MSGHANDLE_PP_TUPLE_REM_4 x)
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_M_I(r, x_e) MSGHANDLE_PP_LIST_FOR_EACH_I_M_D(r, x_e)
# endif
#
# define MSGHANDLE_PP_LIST_FOR_EACH_I_M_D(r, m, d, l, i) m(r, d, i, MSGHANDLE_PP_LIST_FIRST(l))
#
# /* MSGHANDLE_PP_LIST_FOR_EACH_I_R */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, macro, data, list) MSGHANDLE_PP_FOR_ ## r((macro, data, list, 0), MSGHANDLE_PP_LIST_FOR_EACH_I_P, MSGHANDLE_PP_LIST_FOR_EACH_I_O, MSGHANDLE_PP_LIST_FOR_EACH_I_M)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list)
#    define MSGHANDLE_PP_LIST_FOR_EACH_I_R_I(r, macro, data, list) MSGHANDLE_PP_FOR_ ## r((macro, data, list, 0), MSGHANDLE_PP_LIST_FOR_EACH_I_P, MSGHANDLE_PP_LIST_FOR_EACH_I_O, MSGHANDLE_PP_LIST_FOR_EACH_I_M)
# endif
#
# endif
