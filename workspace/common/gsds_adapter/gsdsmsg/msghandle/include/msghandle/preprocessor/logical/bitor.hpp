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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_BITOR_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_BITOR_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_BITOR */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_BITOR(x, y) MSGHANDLE_PP_BITOR_I(x, y)
# else
#    define MSGHANDLE_PP_BITOR(x, y) MSGHANDLE_PP_BITOR_OO((x, y))
#    define MSGHANDLE_PP_BITOR_OO(par) MSGHANDLE_PP_BITOR_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_BITOR_I(x, y) MSGHANDLE_PP_BITOR_ ## x ## y
# else
#    define MSGHANDLE_PP_BITOR_I(x, y) MSGHANDLE_PP_BITOR_ID(MSGHANDLE_PP_BITOR_ ## x ## y)
#    define MSGHANDLE_PP_BITOR_ID(id) id
# endif
#
# define MSGHANDLE_PP_BITOR_00 0
# define MSGHANDLE_PP_BITOR_01 1
# define MSGHANDLE_PP_BITOR_10 1
# define MSGHANDLE_PP_BITOR_11 1
#
# endif
