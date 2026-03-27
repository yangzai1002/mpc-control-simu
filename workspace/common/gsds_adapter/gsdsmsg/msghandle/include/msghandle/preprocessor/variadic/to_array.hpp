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
# ifndef MSGHANDLE_PREPROCESSOR_VARIADIC_TO_ARRAY_HPP
# define MSGHANDLE_PREPROCESSOR_VARIADIC_TO_ARRAY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/to_array.hpp>
# if MSGHANDLE_PP_VARIADICS_MSVC
#    include <msghandle/preprocessor/variadic/size.hpp>
# endif
#
# /* MSGHANDLE_PP_VARIADIC_TO_ARRAY */
#
# if MSGHANDLE_PP_VARIADICS
#    if MSGHANDLE_PP_VARIADICS_MSVC
#        define MSGHANDLE_PP_VARIADIC_TO_ARRAY(...) MSGHANDLE_PP_TUPLE_TO_ARRAY_2(MSGHANDLE_PP_VARIADIC_SIZE(__VA_ARGS__),(__VA_ARGS__))
#    else
#        define MSGHANDLE_PP_VARIADIC_TO_ARRAY(...) MSGHANDLE_PP_TUPLE_TO_ARRAY((__VA_ARGS__))
#    endif
# endif
#
# endif
