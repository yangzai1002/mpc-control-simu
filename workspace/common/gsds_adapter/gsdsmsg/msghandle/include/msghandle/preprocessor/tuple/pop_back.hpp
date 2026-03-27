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
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_POP_BACK_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_POP_BACK_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/array/pop_back.hpp>
# include <msghandle/preprocessor/array/to_tuple.hpp>
# include <msghandle/preprocessor/comparison/greater.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/tuple/size.hpp>
# include <msghandle/preprocessor/tuple/to_array.hpp>
#
# /* MSGHANDLE_PP_TUPLE_POP_BACK */
#
# define MSGHANDLE_PP_TUPLE_POP_BACK(tuple) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER(MSGHANDLE_PP_TUPLE_SIZE(tuple),1), \
		MSGHANDLE_PP_TUPLE_POP_BACK_EXEC, \
		MSGHANDLE_PP_TUPLE_POP_BACK_RETURN \
		) \
	(tuple) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_BACK_EXEC(tuple) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_POP_BACK(MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_BACK_RETURN(tuple) tuple
#
# /* MSGHANDLE_PP_TUPLE_POP_BACK_Z */
#
# define MSGHANDLE_PP_TUPLE_POP_BACK_Z(z, tuple) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER(MSGHANDLE_PP_TUPLE_SIZE(tuple),1), \
		MSGHANDLE_PP_TUPLE_POP_BACK_Z_EXEC, \
		MSGHANDLE_PP_TUPLE_POP_BACK_Z_RETURN \
		) \
	(z, tuple) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_BACK_Z_EXEC(z, tuple) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_POP_BACK_Z(z, MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple))) \
/**/
#
# define MSGHANDLE_PP_TUPLE_POP_BACK_Z_RETURN(z, tuple) tuple
#
# endif // MSGHANDLE_PP_VARIADICS
#
# endif // MSGHANDLE_PREPROCESSOR_TUPLE_POP_BACK_HPP
