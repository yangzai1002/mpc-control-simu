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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_AT_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_AT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/adt.hpp>
# include <msghandle/preprocessor/list/rest_n.hpp>
#
# /* MSGHANDLE_PP_LIST_AT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_AT(list, index) MSGHANDLE_PP_LIST_FIRST(MSGHANDLE_PP_LIST_REST_N(index, list))
# else
#    define MSGHANDLE_PP_LIST_AT(list, index) MSGHANDLE_PP_LIST_AT_I(list, index)
#    define MSGHANDLE_PP_LIST_AT_I(list, index) MSGHANDLE_PP_LIST_FIRST(MSGHANDLE_PP_LIST_REST_N(index, list))
# endif
#
# /* MSGHANDLE_PP_LIST_AT_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_AT_D(d, list, index) MSGHANDLE_PP_LIST_FIRST(MSGHANDLE_PP_LIST_REST_N_D(d, index, list))
# else
#    define MSGHANDLE_PP_LIST_AT_D(d, list, index) MSGHANDLE_PP_LIST_AT_D_I(d, list, index)
#    define MSGHANDLE_PP_LIST_AT_D_I(d, list, index) MSGHANDLE_PP_LIST_FIRST(MSGHANDLE_PP_LIST_REST_N_D(d, index, list))
# endif
#
# endif
