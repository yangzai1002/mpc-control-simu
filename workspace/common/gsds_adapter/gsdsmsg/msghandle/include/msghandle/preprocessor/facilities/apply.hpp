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
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_APPLY_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_APPLY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/expr_iif.hpp>
# include <msghandle/preprocessor/detail/is_unary.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_APPLY */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_APPLY(x) MSGHANDLE_PP_APPLY_I(x)
#    define MSGHANDLE_PP_APPLY_I(x) MSGHANDLE_PP_EXPR_IIF(MSGHANDLE_PP_IS_UNARY(x), MSGHANDLE_PP_TUPLE_REM_1 x)
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_BCC()
#    define MSGHANDLE_PP_APPLY(x) MSGHANDLE_PP_APPLY_I(x)
#    define MSGHANDLE_PP_APPLY_I(x) MSGHANDLE_PP_APPLY_ ## x
#    define MSGHANDLE_PP_APPLY_(x) x
#    define MSGHANDLE_PP_APPLY_MSGHANDLE_PP_NIL
# else
#    define MSGHANDLE_PP_APPLY(x) MSGHANDLE_PP_EXPR_IIF(MSGHANDLE_PP_IS_UNARY(x), MSGHANDLE_PP_TUPLE_REM_1 x)
# endif
#
# endif
