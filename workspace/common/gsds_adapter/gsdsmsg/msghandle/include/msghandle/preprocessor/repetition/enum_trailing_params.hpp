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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_TRAILING_PARAMS_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
#
# /* MSGHANDLE_PP_ENUM_TRAILING_PARAMS */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS(count, param) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS(count, param) MSGHANDLE_PP_ENUM_TRAILING_PARAMS_I(count, param)
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS_I(count, param) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# define MSGHANDLE_PP_ENUM_TRAILING_PARAMS_M(z, n, param) , param ## n
#
# /* MSGHANDLE_PP_ENUM_TRAILING_PARAMS_Z */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_TRAILING_PARAMS_M, param)
# else
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS_Z(z, count, param) MSGHANDLE_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param)
#    define MSGHANDLE_PP_ENUM_TRAILING_PARAMS_Z_I(z, count, param) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_TRAILING_PARAMS_M, param)
# endif
#
# endif
