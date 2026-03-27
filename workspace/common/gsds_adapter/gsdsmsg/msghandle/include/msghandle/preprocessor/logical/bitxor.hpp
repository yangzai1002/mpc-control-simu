# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_BITXOR_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_BITXOR_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_BITXOR */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_BITXOR(x, y) MSGHANDLE_PP_BITXOR_I(x, y)
# else
#    define MSGHANDLE_PP_BITXOR(x, y) MSGHANDLE_PP_BITXOR_OO((x, y))
#    define MSGHANDLE_PP_BITXOR_OO(par) MSGHANDLE_PP_BITXOR_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_BITXOR_I(x, y) MSGHANDLE_PP_BITXOR_ ## x ## y
# else
#    define MSGHANDLE_PP_BITXOR_I(x, y) MSGHANDLE_PP_BITXOR_ID(MSGHANDLE_PP_BITXOR_ ## x ## y)
#    define MSGHANDLE_PP_BITXOR_ID(id) id
# endif
#
# define MSGHANDLE_PP_BITXOR_00 0
# define MSGHANDLE_PP_BITXOR_01 1
# define MSGHANDLE_PP_BITXOR_10 1
# define MSGHANDLE_PP_BITXOR_11 0
#
# endif
