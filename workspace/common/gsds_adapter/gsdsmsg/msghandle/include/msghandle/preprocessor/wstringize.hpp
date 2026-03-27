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
# ifndef MSGHANDLE_PREPROCESSOR_WSTRINGIZE_HPP
# define MSGHANDLE_PREPROCESSOR_WSTRINGIZE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_WSTRINGIZE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_WSTRINGIZE(text) MSGHANDLE_PP_WSTRINGIZE_I(text)
# else
#    define MSGHANDLE_PP_WSTRINGIZE(text) MSGHANDLE_PP_WSTRINGIZE_OO((text))
#    define MSGHANDLE_PP_WSTRINGIZE_OO(par) MSGHANDLE_PP_WSTRINGIZE_I ## par
# endif
#
# define MSGHANDLE_PP_WSTRINGIZE_I(text) MSGHANDLE_PP_WSTRINGIZE_II(#text)
# define MSGHANDLE_PP_WSTRINGIZE_II(str) L ## str
#
# endif
