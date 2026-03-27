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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_NOR_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_NOR_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/logical/bool.hpp>
# include <msghandle/preprocessor/logical/bitnor.hpp>
#
# /* MSGHANDLE_PP_NOR */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_NOR(p, q) MSGHANDLE_PP_BITNOR(MSGHANDLE_PP_BOOL(p), MSGHANDLE_PP_BOOL(q))
# else
#    define MSGHANDLE_PP_NOR(p, q) MSGHANDLE_PP_NOR_I(p, q)
#    define MSGHANDLE_PP_NOR_I(p, q) MSGHANDLE_PP_BITNOR(MSGHANDLE_PP_BOOL(p), MSGHANDLE_PP_BOOL(q))
# endif
#
# endif
