# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2013.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/array/push_front.hpp>
# include <msghandle/preprocessor/array/to_tuple.hpp>
# include <msghandle/preprocessor/tuple/to_array.hpp>
#
#
# /* MSGHANDLE_PP_TUPLE_PUSH_FRONT */
#
# define MSGHANDLE_PP_TUPLE_PUSH_FRONT(tuple, elem) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_PUSH_FRONT(MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple), elem)) \
/**/
#
# endif // MSGHANDLE_PP_VARIADICS
#
# endif // MSGHANDLE_PREPROCESSOR_TUPLE_PUSH_FRONT_HPP
