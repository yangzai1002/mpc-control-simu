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
# ifndef MSGHANDLE_PREPROCESSOR_COMPARISON_GREATER_HPP
# define MSGHANDLE_PREPROCESSOR_COMPARISON_GREATER_HPP
#
# include <msghandle/preprocessor/comparison/less.hpp>
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_GREATER */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_GREATER(x, y) MSGHANDLE_PP_LESS(y, x)
# else
#    define MSGHANDLE_PP_GREATER(x, y) MSGHANDLE_PP_GREATER_I(x, y)
#    define MSGHANDLE_PP_GREATER_I(x, y) MSGHANDLE_PP_LESS(y, x)
# endif
#
# /* MSGHANDLE_PP_GREATER_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_GREATER_D(d, x, y) MSGHANDLE_PP_LESS_D(d, y, x)
# else
#    define MSGHANDLE_PP_GREATER_D(d, x, y) MSGHANDLE_PP_GREATER_D_I(d, x, y)
#    define MSGHANDLE_PP_GREATER_D_I(d, x, y) MSGHANDLE_PP_LESS_D(d, y, x)
# endif
#
# endif
