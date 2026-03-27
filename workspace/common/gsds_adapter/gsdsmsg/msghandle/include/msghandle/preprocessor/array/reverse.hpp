# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_REVERSE_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_REVERSE_HPP
#
# include <msghandle/preprocessor/array/data.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/reverse.hpp>
#
# /* MSGHANDLE_PP_ARRAY_REVERSE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_REVERSE(array) (MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_TUPLE_REVERSE(MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_ARRAY_DATA(array)))
# else
#    define MSGHANDLE_PP_ARRAY_REVERSE(array) MSGHANDLE_PP_ARRAY_REVERSE_I(array)
#    define MSGHANDLE_PP_ARRAY_REVERSE_I(array) (MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_TUPLE_REVERSE(MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_ARRAY_DATA(array)))
# endif
#
# endif
