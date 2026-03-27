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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_PARAMS_WITH_A_DEFAULT_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_PARAMS_WITH_A_DEFAULT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/facilities/intercept.hpp>
# include <msghandle/preprocessor/repetition/enum_binary_params.hpp>
#
# /* MSGHANDLE_PP_ENUM_PARAMS_WITH_A_DEFAULT */
#
# define MSGHANDLE_PP_ENUM_PARAMS_WITH_A_DEFAULT(count, param, def) MSGHANDLE_PP_ENUM_BINARY_PARAMS(count, param, = def MSGHANDLE_PP_INTERCEPT)
#
# endif
