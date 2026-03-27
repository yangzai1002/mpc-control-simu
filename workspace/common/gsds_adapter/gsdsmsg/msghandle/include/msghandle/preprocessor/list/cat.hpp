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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_CAT_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_CAT_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/list/fold_left.hpp>
#
# /* MSGHANDLE_PP_LIST_CAT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_CAT(list) MSGHANDLE_PP_LIST_FOLD_LEFT(MSGHANDLE_PP_LIST_CAT_O, MSGHANDLE_PP_LIST_FIRST(list), MSGHANDLE_PP_LIST_REST(list))
# else
#    define MSGHANDLE_PP_LIST_CAT(list) MSGHANDLE_PP_LIST_CAT_I(list)
#    define MSGHANDLE_PP_LIST_CAT_I(list) MSGHANDLE_PP_LIST_FOLD_LEFT(MSGHANDLE_PP_LIST_CAT_O, MSGHANDLE_PP_LIST_FIRST(list), MSGHANDLE_PP_LIST_REST(list))
# endif
#
# define MSGHANDLE_PP_LIST_CAT_O(d, s, x) MSGHANDLE_PP_CAT(s, x)
#
# /* MSGHANDLE_PP_LIST_CAT_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_CAT_D(d, list) MSGHANDLE_PP_LIST_FOLD_LEFT_ ## d(MSGHANDLE_PP_LIST_CAT_O, MSGHANDLE_PP_LIST_FIRST(list), MSGHANDLE_PP_LIST_REST(list))
# else
#    define MSGHANDLE_PP_LIST_CAT_D(d, list) MSGHANDLE_PP_LIST_CAT_D_I(d, list)
#    define MSGHANDLE_PP_LIST_CAT_D_I(d, list) MSGHANDLE_PP_LIST_FOLD_LEFT_ ## d(MSGHANDLE_PP_LIST_CAT_O, MSGHANDLE_PP_LIST_FIRST(list), MSGHANDLE_PP_LIST_REST(list))
# endif
#
# endif
