# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_TO_TUPLE_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_TO_TUPLE_HPP
#
# include <msghandle/preprocessor/array/data.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/control/if.hpp>
#
# /* MSGHANDLE_PP_ARRAY_TO_TUPLE */
#
#    define MSGHANDLE_PP_ARRAY_TO_TUPLE(array) \
		MSGHANDLE_PP_IF \
			( \
			MSGHANDLE_PP_ARRAY_SIZE(array), \
			MSGHANDLE_PP_ARRAY_DATA, \
			MSGHANDLE_PP_ARRAY_TO_TUPLE_EMPTY \
			) \
		(array) \
/**/
#    define MSGHANDLE_PP_ARRAY_TO_TUPLE_EMPTY(array)
#
# endif
