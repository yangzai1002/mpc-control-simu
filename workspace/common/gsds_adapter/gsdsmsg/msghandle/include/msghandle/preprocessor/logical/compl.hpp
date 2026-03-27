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
# ifndef MSGHANDLE_PREPROCESSOR_LOGICAL_COMPL_HPP
# define MSGHANDLE_PREPROCESSOR_LOGICAL_COMPL_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_COMPL */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_COMPL(x) MSGHANDLE_PP_COMPL_I(x)
# else
#    define MSGHANDLE_PP_COMPL(x) MSGHANDLE_PP_COMPL_OO((x))
#    define MSGHANDLE_PP_COMPL_OO(par) MSGHANDLE_PP_COMPL_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_COMPL_I(x) MSGHANDLE_PP_COMPL_ ## x
# else
#    define MSGHANDLE_PP_COMPL_I(x) MSGHANDLE_PP_COMPL_ID(MSGHANDLE_PP_COMPL_ ## x)
#    define MSGHANDLE_PP_COMPL_ID(id) id
# endif
#
# define MSGHANDLE_PP_COMPL_0 1
# define MSGHANDLE_PP_COMPL_1 0
#
# endif
