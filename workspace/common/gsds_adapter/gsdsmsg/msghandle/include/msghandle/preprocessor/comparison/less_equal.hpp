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
# ifndef MSGHANDLE_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
# define MSGHANDLE_PREPROCESSOR_COMPARISON_LESS_EQUAL_HPP
#
# include <msghandle/preprocessor/arithmetic/sub.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/logical/not.hpp>
#
# /* MSGHANDLE_PP_LESS_EQUAL */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LESS_EQUAL(x, y) MSGHANDLE_PP_NOT(MSGHANDLE_PP_SUB(x, y))
# else
#    define MSGHANDLE_PP_LESS_EQUAL(x, y) MSGHANDLE_PP_LESS_EQUAL_I(x, y)
#    define MSGHANDLE_PP_LESS_EQUAL_I(x, y) MSGHANDLE_PP_NOT(MSGHANDLE_PP_SUB(x, y))
# endif
#
# /* MSGHANDLE_PP_LESS_EQUAL_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LESS_EQUAL_D(d, x, y) MSGHANDLE_PP_NOT(MSGHANDLE_PP_SUB_D(d, x, y))
# else
#    define MSGHANDLE_PP_LESS_EQUAL_D(d, x, y) MSGHANDLE_PP_LESS_EQUAL_D_I(d, x, y)
#    define MSGHANDLE_PP_LESS_EQUAL_D_I(d, x, y) MSGHANDLE_PP_NOT(MSGHANDLE_PP_SUB_D(d, x, y))
# endif
#
# endif
