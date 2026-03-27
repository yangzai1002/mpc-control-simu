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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_REST_N_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_REST_N_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_LIST_REST_N */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_REST_N(count, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_REST_N_P, MSGHANDLE_PP_LIST_REST_N_O, (list, count)))
# else
#    define MSGHANDLE_PP_LIST_REST_N(count, list) MSGHANDLE_PP_LIST_REST_N_I(count, list)
#    define MSGHANDLE_PP_LIST_REST_N_I(count, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_LIST_REST_N_P, MSGHANDLE_PP_LIST_REST_N_O, (list, count)))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_REST_N_P(d, lc) MSGHANDLE_PP_TUPLE_ELEM(2, 1, lc)
# else
#    define MSGHANDLE_PP_LIST_REST_N_P(d, lc) MSGHANDLE_PP_LIST_REST_N_P_I lc
#    define MSGHANDLE_PP_LIST_REST_N_P_I(list, count) count
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_REST_N_O(d, lc) (MSGHANDLE_PP_LIST_REST(MSGHANDLE_PP_TUPLE_ELEM(2, 0, lc)), MSGHANDLE_PP_DEC(MSGHANDLE_PP_TUPLE_ELEM(2, 1, lc)))
# else
#    define MSGHANDLE_PP_LIST_REST_N_O(d, lc) MSGHANDLE_PP_LIST_REST_N_O_I lc
#    define MSGHANDLE_PP_LIST_REST_N_O_I(list, count) (MSGHANDLE_PP_LIST_REST(list), MSGHANDLE_PP_DEC(count))
# endif
#
# /* MSGHANDLE_PP_LIST_REST_N_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_REST_N_D(d, count, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_REST_N_P, MSGHANDLE_PP_LIST_REST_N_O, (list, count)))
# else
#    define MSGHANDLE_PP_LIST_REST_N_D(d, count, list) MSGHANDLE_PP_LIST_REST_N_D_I(d, count, list)
#    define MSGHANDLE_PP_LIST_REST_N_D_I(d, count, list) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_LIST_REST_N_P, MSGHANDLE_PP_LIST_REST_N_O, (list, count)))
# endif
#
# endif
