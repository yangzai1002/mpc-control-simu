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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_XOR_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_XOR_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/logical/bool.hpp>
# include <msghandle/preprocessor/logical/bitxor.hpp>
#
# /* MSGHANDLE_PP_XOR */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_XOR(p, q) MSGHANDLE_PP_BITXOR(MSGHANDLE_PP_BOOL(p), MSGHANDLE_PP_BOOL(q))
# else
#    define MSGHANDLE_PP_XOR(p, q) MSGHANDLE_PP_XOR_I(p, q)
#    define MSGHANDLE_PP_XOR_I(p, q) MSGHANDLE_PP_BITXOR(MSGHANDLE_PP_BOOL(p), MSGHANDLE_PP_BOOL(q))
# endif
#
# endif
