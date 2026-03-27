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
# ifndef MSGHANDLE_PREPROCESSOR_SELECTION_MIN_HPP
# define MSGHANDLE_PREPROCESSOR_SELECTION_MIN_HPP
#
# include <msghandle/preprocessor/comparison/less_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
#
# /* MSGHANDLE_PP_MIN */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_MIN(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL(y, x), y, x)
# else
#    define MSGHANDLE_PP_MIN(x, y) MSGHANDLE_PP_MIN_I(x, y)
#    define MSGHANDLE_PP_MIN_I(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL(y, x), y, x)
# endif
#
# /* MSGHANDLE_PP_MIN_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_MIN_D(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL_D(d, y, x), y, x)
# else
#    define MSGHANDLE_PP_MIN_D(d, x, y) MSGHANDLE_PP_MIN_D_I(d, x, y)
#    define MSGHANDLE_PP_MIN_D_I(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL_D(d, y, x), y, x)
# endif
#
# endif
