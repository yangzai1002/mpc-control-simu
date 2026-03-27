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
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_POP_FRONT_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_POP_FRONT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/array/pop_front.hpp>
# include <msghandle/preprocessor/array/to_tuple.hpp>
# include <msghandle/preprocessor/comparison/greater.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/tuple/size.hpp>
# include <msghandle/preprocessor/tuple/to_array.hpp>
#
#
# /* MSGHANDLE_PP_TUPLE_POP_FRONT */
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT(tuple) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER(MSGHANDLE_PP_TUPLE_SIZE(tuple),1), \
		MSGHANDLE_PP_TUPLE_POP_FRONT_EXEC, \
		MSGHANDLE_PP_TUPLE_POP_FRONT_RETURN \
		) \
	(tuple) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT_EXEC(tuple) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_POP_FRONT(MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT_RETURN(tuple) tuple
#
# /* MSGHANDLE_PP_TUPLE_POP_FRONT_Z */
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT_Z(z, tuple) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER(MSGHANDLE_PP_TUPLE_SIZE(tuple),1), \
		MSGHANDLE_PP_TUPLE_POP_FRONT_Z_EXEC, \
		MSGHANDLE_PP_TUPLE_POP_FRONT_Z_RETURN \
		) \
	(z, tuple) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT_Z_EXEC(z, tuple) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_POP_FRONT_Z(z, MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_FRONT_Z_RETURN(z, tuple) tuple
#
# endif // MSGHANDLE_PP_VARIADICS
#
# endif // MSGHANDLE_PREPROCESSOR_TUPLE_POP_FRONT_HPP
