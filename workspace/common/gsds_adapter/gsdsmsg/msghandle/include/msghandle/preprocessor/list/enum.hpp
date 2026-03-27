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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_ENUM_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_ENUM_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/for_each_i.hpp>
# include <msghandle/preprocessor/punctuation/comma_if.hpp>
#
# /* MSGHANDLE_PP_LIST_ENUM */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_ENUM(list) MSGHANDLE_PP_LIST_FOR_EACH_I(MSGHANDLE_PP_LIST_ENUM_O, MSGHANDLE_PP_NIL, list)
# else
#    define MSGHANDLE_PP_LIST_ENUM(list) MSGHANDLE_PP_LIST_ENUM_I(list)
#    define MSGHANDLE_PP_LIST_ENUM_I(list) MSGHANDLE_PP_LIST_FOR_EACH_I(MSGHANDLE_PP_LIST_ENUM_O, MSGHANDLE_PP_NIL, list)
# endif
#
# define MSGHANDLE_PP_LIST_ENUM_O(r, _, i, elem) MSGHANDLE_PP_COMMA_IF(i) elem
#
# /* MSGHANDLE_PP_LIST_ENUM_R */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_ENUM_R(r, list) MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, MSGHANDLE_PP_LIST_ENUM_O, MSGHANDLE_PP_NIL, list)
# else
#    define MSGHANDLE_PP_LIST_ENUM_R(r, list) MSGHANDLE_PP_LIST_ENUM_R_I(r, list)
#    define MSGHANDLE_PP_LIST_ENUM_R_I(r, list) MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, MSGHANDLE_PP_LIST_ENUM_O, MSGHANDLE_PP_NIL, list)
# endif
#
# endif
