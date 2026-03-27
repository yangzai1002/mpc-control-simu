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
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_TO_ARRAY_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_TO_ARRAY_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/facilities/overload.hpp>
# include <msghandle/preprocessor/tuple/size.hpp>
# include <msghandle/preprocessor/variadic/size.hpp>
#
# /* MSGHANDLE_PP_TUPLE_TO_ARRAY */
#
# if MSGHANDLE_PP_VARIADICS
#    if MSGHANDLE_PP_VARIADICS_MSVC
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY(...) MSGHANDLE_PP_TUPLE_TO_ARRAY_I(MSGHANDLE_PP_OVERLOAD(MSGHANDLE_PP_TUPLE_TO_ARRAY_, __VA_ARGS__), (__VA_ARGS__))
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY_I(m, args) MSGHANDLE_PP_TUPLE_TO_ARRAY_II(m, args)
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY_II(m, args) MSGHANDLE_PP_CAT(m ## args,)
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY_1(tuple) (MSGHANDLE_PP_TUPLE_SIZE(tuple), tuple)
#    else
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY(...) MSGHANDLE_PP_OVERLOAD(MSGHANDLE_PP_TUPLE_TO_ARRAY_, __VA_ARGS__)(__VA_ARGS__)
#        define MSGHANDLE_PP_TUPLE_TO_ARRAY_1(tuple) (MSGHANDLE_PP_VARIADIC_SIZE tuple, tuple)
#    endif
#    define MSGHANDLE_PP_TUPLE_TO_ARRAY_2(size, tuple) (size, tuple)
# else
#    define MSGHANDLE_PP_TUPLE_TO_ARRAY(size, tuple) (size, tuple)
# endif
#
# endif
