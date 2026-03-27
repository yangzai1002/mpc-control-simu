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
# ifndef MSGHANDLE_PREPROCESSOR_CONTROL_IF_HPP
# define MSGHANDLE_PREPROCESSOR_CONTROL_IF_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/logical/bool.hpp>
#
# /* MSGHANDLE_PP_IF */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_IF(cond, t, f) MSGHANDLE_PP_IIF(MSGHANDLE_PP_BOOL(cond), t, f)
# else
#    define MSGHANDLE_PP_IF(cond, t, f) MSGHANDLE_PP_IF_I(cond, t, f)
#    define MSGHANDLE_PP_IF_I(cond, t, f) MSGHANDLE_PP_IIF(MSGHANDLE_PP_BOOL(cond), t, f)
# endif
#
# endif
