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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_FOLD_RIGHT_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/debug/error.hpp>
# include <msghandle/preprocessor/detail/auto_rec.hpp>
#
# if 0
#    define MSGHANDLE_PP_LIST_FOLD_RIGHT(op, state, list)
# endif
#
# define MSGHANDLE_PP_LIST_FOLD_RIGHT MSGHANDLE_PP_CAT(MSGHANDLE_PP_LIST_FOLD_RIGHT_, MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_WHILE_P, 256))
#
# define MSGHANDLE_PP_LIST_FOLD_RIGHT_257(o, s, l) MSGHANDLE_PP_ERROR(0x0004)
#
# define MSGHANDLE_PP_LIST_FOLD_RIGHT_D(d, o, s, l) MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(o, s, l)
# define MSGHANDLE_PP_LIST_FOLD_RIGHT_2ND MSGHANDLE_PP_LIST_FOLD_RIGHT
# define MSGHANDLE_PP_LIST_FOLD_RIGHT_2ND_D MSGHANDLE_PP_LIST_FOLD_RIGHT_D
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    include <msghandle/preprocessor/list/detail/edg/fold_right.hpp>
# else
#    include <msghandle/preprocessor/list/detail/fold_right.hpp>
# endif
#
# endif
