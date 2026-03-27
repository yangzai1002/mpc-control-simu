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
# ifndef MSGHANDLE_PREPROCESSOR_ITERATION_LOCAL_HPP
# define MSGHANDLE_PREPROCESSOR_ITERATION_LOCAL_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/slot/slot.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_LOCAL_ITERATE */
#
# define MSGHANDLE_PP_LOCAL_ITERATE() <msghandle/preprocessor/iteration/detail/local.hpp>
#
# define MSGHANDLE_PP_LOCAL_C(n) (MSGHANDLE_PP_LOCAL_S) <= n && (MSGHANDLE_PP_LOCAL_F) >= n
# define MSGHANDLE_PP_LOCAL_R(n) (MSGHANDLE_PP_LOCAL_F) <= n && (MSGHANDLE_PP_LOCAL_S) >= n
#
# endif
