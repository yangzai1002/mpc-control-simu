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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_APPEND_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_APPEND_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/fold_right.hpp>
#
# /* MSGHANDLE_PP_LIST_APPEND */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_APPEND(a, b) MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_APPEND_O, b, a)
# else
#    define MSGHANDLE_PP_LIST_APPEND(a, b) MSGHANDLE_PP_LIST_APPEND_I(a, b)
#    define MSGHANDLE_PP_LIST_APPEND_I(a, b) MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_APPEND_O, b, a)
# endif
#
# define MSGHANDLE_PP_LIST_APPEND_O(d, s, x) (x, s)
#
# /* MSGHANDLE_PP_LIST_APPEND_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_APPEND_D(d, a, b) MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_APPEND_O, b, a)
# else
#    define MSGHANDLE_PP_LIST_APPEND_D(d, a, b) MSGHANDLE_PP_LIST_APPEND_D_I(d, a, b)
#    define MSGHANDLE_PP_LIST_APPEND_D_I(d, a, b) MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_APPEND_O, b, a)
# endif
#
# endif
