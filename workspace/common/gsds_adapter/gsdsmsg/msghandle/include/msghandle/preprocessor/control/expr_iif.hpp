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
# ifndef MSGHANDLE_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
# define MSGHANDLE_PREPROCESSOR_CONTROL_EXPR_IIF_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_EXPR_IIF */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_EXPR_IIF(bit, expr) MSGHANDLE_PP_EXPR_IIF_I(bit, expr)
# else
#    define MSGHANDLE_PP_EXPR_IIF(bit, expr) MSGHANDLE_PP_EXPR_IIF_OO((bit, expr))
#    define MSGHANDLE_PP_EXPR_IIF_OO(par) MSGHANDLE_PP_EXPR_IIF_I ## par
# endif
#
# define MSGHANDLE_PP_EXPR_IIF_I(bit, expr) MSGHANDLE_PP_EXPR_IIF_ ## bit(expr)
#
# define MSGHANDLE_PP_EXPR_IIF_0(expr)
# define MSGHANDLE_PP_EXPR_IIF_1(expr) expr
#
# endif
