# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_OVERLOAD_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/variadic/size.hpp>
#
# /* MSGHANDLE_PP_OVERLOAD */
#
# if MSGHANDLE_PP_VARIADICS
#    define MSGHANDLE_PP_OVERLOAD(prefix, ...) MSGHANDLE_PP_CAT(prefix, MSGHANDLE_PP_VARIADIC_SIZE(__VA_ARGS__))
# endif
#
# endif
