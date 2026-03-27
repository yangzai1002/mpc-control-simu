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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_BITNOR_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_BITNOR_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_BITNOR */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_BITNOR(x, y) MSGHANDLE_PP_BITNOR_I(x, y)
# else
#    define MSGHANDLE_PP_BITNOR(x, y) MSGHANDLE_PP_BITNOR_OO((x, y))
#    define MSGHANDLE_PP_BITNOR_OO(par) MSGHANDLE_PP_BITNOR_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_BITNOR_I(x, y) MSGHANDLE_PP_BITNOR_ ## x ## y
# else
#    define MSGHANDLE_PP_BITNOR_I(x, y) MSGHANDLE_PP_BITNOR_ID(MSGHANDLE_PP_BITNOR_ ## x ## y)
#    define MSGHANDLE_PP_BITNOR_ID(id) id
# endif
#
# define MSGHANDLE_PP_BITNOR_00 1
# define MSGHANDLE_PP_BITNOR_01 0
# define MSGHANDLE_PP_BITNOR_10 0
# define MSGHANDLE_PP_BITNOR_11 0
#
# endif
