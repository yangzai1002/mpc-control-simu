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
# ifndef MSGHANDLE_PREPROCESSOR_DEBUG_ASSERT_HPP
# define MSGHANDLE_PREPROCESSOR_DEBUG_ASSERT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/expr_iif.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/logical/not.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
#
# /* MSGHANDLE_PP_ASSERT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ASSERT MSGHANDLE_PP_ASSERT_D
# else
#    define MSGHANDLE_PP_ASSERT(cond) MSGHANDLE_PP_ASSERT_D(cond)
# endif
#
# define MSGHANDLE_PP_ASSERT_D(cond) MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT(cond), MSGHANDLE_PP_ASSERT_ERROR, MSGHANDLE_PP_TUPLE_EAT_1)(...)
# define MSGHANDLE_PP_ASSERT_ERROR(x, y, z)
#
# /* MSGHANDLE_PP_ASSERT_MSG */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ASSERT_MSG MSGHANDLE_PP_ASSERT_MSG_D
# else
#    define MSGHANDLE_PP_ASSERT_MSG(cond, msg) MSGHANDLE_PP_ASSERT_MSG_D(cond, msg)
# endif
#
# define MSGHANDLE_PP_ASSERT_MSG_D(cond, msg) MSGHANDLE_PP_EXPR_IIF(MSGHANDLE_PP_NOT(cond), msg)
#
# endif
