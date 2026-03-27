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
# ifndef MSGHANDLE_PREPROCESSOR_COMPARISON_EQUAL_HPP
# define MSGHANDLE_PREPROCESSOR_COMPARISON_EQUAL_HPP
#
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/logical/compl.hpp>
#
# /* MSGHANDLE_PP_EQUAL */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_EQUAL(x, y) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_NOT_EQUAL(x, y))
# else
#    define MSGHANDLE_PP_EQUAL(x, y) MSGHANDLE_PP_EQUAL_I(x, y)
#    define MSGHANDLE_PP_EQUAL_I(x, y) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_NOT_EQUAL(x, y))
# endif
#
# /* MSGHANDLE_PP_EQUAL_D */
#
# define MSGHANDLE_PP_EQUAL_D(d, x, y) MSGHANDLE_PP_EQUAL(x, y)
#
# endif
