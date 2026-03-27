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
# ifndef MSGHANDLE_PREPROCESSOR_CONTROL_IIF_HPP
# define MSGHANDLE_PREPROCESSOR_CONTROL_IIF_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_IIF(bit, t, f) MSGHANDLE_PP_IIF_I(bit, t, f)
# else
#    define MSGHANDLE_PP_IIF(bit, t, f) MSGHANDLE_PP_IIF_OO((bit, t, f))
#    define MSGHANDLE_PP_IIF_OO(par) MSGHANDLE_PP_IIF_I ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_IIF_I(bit, t, f) MSGHANDLE_PP_IIF_ ## bit(t, f)
# else
#    define MSGHANDLE_PP_IIF_I(bit, t, f) MSGHANDLE_PP_IIF_II(MSGHANDLE_PP_IIF_ ## bit(t, f))
#    define MSGHANDLE_PP_IIF_II(id) id
# endif
#
# define MSGHANDLE_PP_IIF_0(t, f) f
# define MSGHANDLE_PP_IIF_1(t, f) t
#
# endif
