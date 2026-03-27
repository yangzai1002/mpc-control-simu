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
# ifndef MSGHANDLE_PREPROCESSOR_CAT_HPP
# define MSGHANDLE_PREPROCESSOR_CAT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_CAT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_CAT(a, b) MSGHANDLE_PP_CAT_I(a, b)
# else
#    define MSGHANDLE_PP_CAT(a, b) MSGHANDLE_PP_CAT_OO((a, b))
#    define MSGHANDLE_PP_CAT_OO(par) MSGHANDLE_PP_CAT_I ## par
# endif
#
# if (~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()) || (defined(__INTEL_COMPILER) && __INTEL_COMPILER >= 1700)
#    define MSGHANDLE_PP_CAT_I(a, b) a ## b
# else
#    define MSGHANDLE_PP_CAT_I(a, b) MSGHANDLE_PP_CAT_II(~, a ## b)
#    define MSGHANDLE_PP_CAT_II(p, res) res
# endif
#
# endif
