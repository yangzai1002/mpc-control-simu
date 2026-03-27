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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_NOT_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_NOT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/logical/bool.hpp>
# include <msghandle/preprocessor/logical/compl.hpp>
#
# /* MSGHANDLE_PP_NOT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_NOT(x) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_BOOL(x))
# else
#    define MSGHANDLE_PP_NOT(x) MSGHANDLE_PP_NOT_I(x)
#    define MSGHANDLE_PP_NOT_I(x) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_BOOL(x))
# endif
#
# endif
