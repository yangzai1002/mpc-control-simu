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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_SIZE_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_SIZE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ARRAY_SIZE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_SIZE(array) MSGHANDLE_PP_TUPLE_ELEM(2, 0, array)
# else
#    define MSGHANDLE_PP_ARRAY_SIZE(array) MSGHANDLE_PP_ARRAY_SIZE_I(array)
#    define MSGHANDLE_PP_ARRAY_SIZE_I(array) MSGHANDLE_PP_ARRAY_SIZE_II array
#    define MSGHANDLE_PP_ARRAY_SIZE_II(size, data) size
# endif
#
# endif
