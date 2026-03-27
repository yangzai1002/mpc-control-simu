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
# ifndef MSGHANDLE_PREPROCESSOR_SELECTION_MAX_HPP
# define MSGHANDLE_PREPROCESSOR_SELECTION_MAX_HPP
#
# include <msghandle/preprocessor/comparison/less_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
#
# /* MSGHANDLE_PP_MAX */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_MAX(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL(x, y), y, x)
# else
#    define MSGHANDLE_PP_MAX(x, y) MSGHANDLE_PP_MAX_I(x, y)
#    define MSGHANDLE_PP_MAX_I(x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL(x, y), y, x)
# endif
#
# /* MSGHANDLE_PP_MAX_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_MAX_D(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL_D(d, x, y), y, x)
# else
#    define MSGHANDLE_PP_MAX_D(d, x, y) MSGHANDLE_PP_MAX_D_I(d, x, y)
#    define MSGHANDLE_PP_MAX_D_I(d, x, y) MSGHANDLE_PP_IIF(MSGHANDLE_PP_LESS_EQUAL_D(d, x, y), y, x)
# endif
#
# endif
