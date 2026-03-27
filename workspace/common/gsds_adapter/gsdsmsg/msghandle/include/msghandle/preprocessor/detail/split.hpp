# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# ifndef MSGHANDLE_PREPROCESSOR_DETAIL_SPLIT_HPP
# define MSGHANDLE_PREPROCESSOR_DETAIL_SPLIT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_SPLIT */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_SPLIT(n, im) MSGHANDLE_PP_SPLIT_I((n, im))
#    define MSGHANDLE_PP_SPLIT_I(par) MSGHANDLE_PP_SPLIT_II ## par
#    define MSGHANDLE_PP_SPLIT_II(n, a, b) MSGHANDLE_PP_SPLIT_ ## n(a, b)
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_SPLIT(n, im) MSGHANDLE_PP_SPLIT_I(n((im)))
#    define MSGHANDLE_PP_SPLIT_I(n) MSGHANDLE_PP_SPLIT_ID(MSGHANDLE_PP_SPLIT_II_ ## n)
#    define MSGHANDLE_PP_SPLIT_II_0(s) MSGHANDLE_PP_SPLIT_ID(MSGHANDLE_PP_SPLIT_0 s)
#    define MSGHANDLE_PP_SPLIT_II_1(s) MSGHANDLE_PP_SPLIT_ID(MSGHANDLE_PP_SPLIT_1 s)
#    define MSGHANDLE_PP_SPLIT_ID(id) id
# else
#    define MSGHANDLE_PP_SPLIT(n, im) MSGHANDLE_PP_SPLIT_I(n)(im)
#    define MSGHANDLE_PP_SPLIT_I(n) MSGHANDLE_PP_SPLIT_ ## n
# endif
#
# define MSGHANDLE_PP_SPLIT_0(a, b) a
# define MSGHANDLE_PP_SPLIT_1(a, b) b
#
# endif
