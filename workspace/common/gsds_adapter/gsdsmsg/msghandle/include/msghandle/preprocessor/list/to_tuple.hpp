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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_TO_TUPLE_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_TO_TUPLE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/enum.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
#
# /* MSGHANDLE_PP_LIST_TO_TUPLE */
#
# define MSGHANDLE_PP_LIST_TO_TUPLE(list) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_LIST_IS_NIL(list), \
		MSGHANDLE_PP_LIST_TO_TUPLE_EMPTY, \
		MSGHANDLE_PP_LIST_TO_TUPLE_DO \
		) \
	(list) \
/**/
# define MSGHANDLE_PP_LIST_TO_TUPLE_EMPTY(list)
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_TO_TUPLE_DO(list) (MSGHANDLE_PP_LIST_ENUM(list))
# else
#    define MSGHANDLE_PP_LIST_TO_TUPLE_DO(list) MSGHANDLE_PP_LIST_TO_TUPLE_I(list)
#    define MSGHANDLE_PP_LIST_TO_TUPLE_I(list) (MSGHANDLE_PP_LIST_ENUM(list))
# endif
#
# /* MSGHANDLE_PP_LIST_TO_TUPLE_R */
#
# define MSGHANDLE_PP_LIST_TO_TUPLE_R(r, list) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_LIST_IS_NIL(list), \
		MSGHANDLE_PP_LIST_TO_TUPLE_R_EMPTY, \
		MSGHANDLE_PP_LIST_TO_TUPLE_R_DO \
		) \
	(r, list) \
/**/
# define MSGHANDLE_PP_LIST_TO_TUPLE_R_EMPTY(r,list)
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_TO_TUPLE_R_DO(r, list) (MSGHANDLE_PP_LIST_ENUM_R(r, list))
# else
#    define MSGHANDLE_PP_LIST_TO_TUPLE_R_DO(r, list) MSGHANDLE_PP_LIST_TO_TUPLE_R_I(r, list)
#    define MSGHANDLE_PP_LIST_TO_TUPLE_R_I(r, list) (MSGHANDLE_PP_LIST_ENUM_R(r, list))
# endif
#
# endif
