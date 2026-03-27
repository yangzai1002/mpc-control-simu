# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_PARAMS_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_PARAMS_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/punctuation/comma_if.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
#
# /* MSGHANDLE_PP_ENUM_PARAMS */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_PARAMS(count, param) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_PARAMS_M, param)
# else
#    define MSGHANDLE_PP_ENUM_PARAMS(count, param) MSGHANDLE_PP_ENUM_PARAMS_I(count, param)
#    define MSGHANDLE_PP_ENUM_PARAMS_I(count, param) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_PARAMS_M, param)
# endif
#
# define MSGHANDLE_PP_ENUM_PARAMS_M(z, n, param) MSGHANDLE_PP_COMMA_IF(n) param ## n
#
# /* MSGHANDLE_PP_ENUM_PARAMS_Z */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_PARAMS_Z(z, count, param) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_PARAMS_M, param)
# else
#    define MSGHANDLE_PP_ENUM_PARAMS_Z(z, count, param) MSGHANDLE_PP_ENUM_PARAMS_Z_I(z, count, param)
#    define MSGHANDLE_PP_ENUM_PARAMS_Z_I(z, count, param) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_PARAMS_M, param)
# endif
#
# endif
