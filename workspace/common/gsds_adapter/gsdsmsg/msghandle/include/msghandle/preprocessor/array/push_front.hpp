# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_PUSH_FRONT_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_PUSH_FRONT_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/array/data.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/punctuation/comma_if.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
# include <msghandle/preprocessor/array/detail/get_data.hpp>
#
# /* MSGHANDLE_PP_ARRAY_PUSH_FRONT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_PUSH_FRONT(array, elem) MSGHANDLE_PP_ARRAY_PUSH_FRONT_I(MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_ARRAY_DATA(array), elem)
# else
#    define MSGHANDLE_PP_ARRAY_PUSH_FRONT(array, elem) MSGHANDLE_PP_ARRAY_PUSH_FRONT_D(array, elem)
#    define MSGHANDLE_PP_ARRAY_PUSH_FRONT_D(array, elem) MSGHANDLE_PP_ARRAY_PUSH_FRONT_I(MSGHANDLE_PP_ARRAY_SIZE(array), MSGHANDLE_PP_ARRAY_DATA(array), elem)
# endif
#
# define MSGHANDLE_PP_ARRAY_PUSH_FRONT_I(size, data, elem) (MSGHANDLE_PP_INC(size), (elem MSGHANDLE_PP_COMMA_IF(size) MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA(size,data)))
#
# endif
