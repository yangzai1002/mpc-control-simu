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
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_SIZE_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_SIZE_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/variadic/size.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#    if MSGHANDLE_PP_VARIADICS_MSVC
#        define MSGHANDLE_PP_TUPLE_SIZE(tuple) MSGHANDLE_PP_CAT(MSGHANDLE_PP_VARIADIC_SIZE tuple,)
#    else
#        define MSGHANDLE_PP_TUPLE_SIZE(tuple) MSGHANDLE_PP_VARIADIC_SIZE tuple
#    endif
# endif
#
# endif
