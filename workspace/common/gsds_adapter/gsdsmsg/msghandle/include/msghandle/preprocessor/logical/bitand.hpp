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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_BITAND_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_BITAND_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_BITAND */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_BITAND(x, y) MSGHANDLE_PP_BITAND_I(x, y)
# else
#    define MSGHANDLE_PP_BITAND(x, y) MSGHANDLE_PP_BITAND_OO((x, y))
#    define MSGHANDLE_PP_BITAND_OO(par) MSGHANDLE_PP_BITAND_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_BITAND_I(x, y) MSGHANDLE_PP_BITAND_ ## x ## y
# else
#    define MSGHANDLE_PP_BITAND_I(x, y) MSGHANDLE_PP_BITAND_ID(MSGHANDLE_PP_BITAND_ ## x ## y)
#    define MSGHANDLE_PP_BITAND_ID(res) res
# endif
#
# define MSGHANDLE_PP_BITAND_00 0
# define MSGHANDLE_PP_BITAND_01 0
# define MSGHANDLE_PP_BITAND_10 0
# define MSGHANDLE_PP_BITAND_11 1
#
# endif
