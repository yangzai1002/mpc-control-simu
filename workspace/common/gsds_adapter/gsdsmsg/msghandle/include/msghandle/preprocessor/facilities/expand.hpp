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
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_EXPAND_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_EXPAND_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC() && ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_DMC()
#    define MSGHANDLE_PP_EXPAND(x) MSGHANDLE_PP_EXPAND_I(x)
# else
#    define MSGHANDLE_PP_EXPAND(x) MSGHANDLE_PP_EXPAND_OO((x))
#    define MSGHANDLE_PP_EXPAND_OO(par) MSGHANDLE_PP_EXPAND_I ## par
# endif
#
# define MSGHANDLE_PP_EXPAND_I(x) x
#
# endif
