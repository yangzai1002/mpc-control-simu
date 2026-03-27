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
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_REMOVE_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_REMOVE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/array/remove.hpp>
# include <msghandle/preprocessor/array/to_tuple.hpp>
# include <msghandle/preprocessor/comparison/greater.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/tuple/size.hpp>
# include <msghandle/preprocessor/tuple/to_array.hpp>
#
# /* MSGHANDLE_PP_TUPLE_REMOVE */
#
# define MSGHANDLE_PP_TUPLE_REMOVE(tuple, i) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER(MSGHANDLE_PP_TUPLE_SIZE(tuple),1), \
		MSGHANDLE_PP_TUPLE_REMOVE_EXEC, \
		MSGHANDLE_PP_TUPLE_REMOVE_RETURN \
		) \
	(tuple, i) \
/**/
#
# define MSGHANDLE_PP_TUPLE_REMOVE_EXEC(tuple, i) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_REMOVE(MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple), i)) \
/**/
#
# define MSGHANDLE_PP_TUPLE_REMOVE_RETURN(tuple, i) tuple
#
# /* MSGHANDLE_PP_TUPLE_REMOVE_D */
#
# define MSGHANDLE_PP_TUPLE_REMOVE_D(d, tuple, i) \
	MSGHANDLE_PP_IIF \
		( \
		MSGHANDLE_PP_GREATER_D(d, MSGHANDLE_PP_TUPLE_SIZE(tuple), 1), \
		MSGHANDLE_PP_TUPLE_REMOVE_D_EXEC, \
		MSGHANDLE_PP_TUPLE_REMOVE_D_RETURN \
		) \
	(d, tuple, i) \
/**/
#
# define MSGHANDLE_PP_TUPLE_REMOVE_D_EXEC(d, tuple, i) \
	MSGHANDLE_PP_ARRAY_TO_TUPLE(MSGHANDLE_PP_ARRAY_REMOVE_D(d, MSGHANDLE_PP_TUPLE_TO_ARRAY(tuple), i)) \
/**/
#
# define MSGHANDLE_PP_TUPLE_REMOVE_D_RETURN(d, tuple, i) tuple
#
# endif // MSGHANDLE_PP_VARIADICS
#
# endif // MSGHANDLE_PREPROCESSOR_TUPLE_REMOVE_HPP
