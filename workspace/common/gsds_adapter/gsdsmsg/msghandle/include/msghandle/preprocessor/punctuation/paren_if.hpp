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
# ifndef MSGHANDLE_PREPROCESSOR_PUNCTUATION_PAREN_IF_HPP
# define MSGHANDLE_PREPROCESSOR_PUNCTUATION_PAREN_IF_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/facilities/empty.hpp>
# include <msghandle/preprocessor/punctuation/paren.hpp>
#
# /* MSGHANDLE_PP_LPAREN_IF */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LPAREN_IF(cond) MSGHANDLE_PP_IF(cond, MSGHANDLE_PP_LPAREN, MSGHANDLE_PP_EMPTY)()
# else
#    define MSGHANDLE_PP_LPAREN_IF(cond) MSGHANDLE_PP_LPAREN_IF_I(cond)
#    define MSGHANDLE_PP_LPAREN_IF_I(cond) MSGHANDLE_PP_IF(cond, MSGHANDLE_PP_LPAREN, MSGHANDLE_PP_EMPTY)()
# endif
#
# /* MSGHANDLE_PP_RPAREN_IF */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_RPAREN_IF(cond) MSGHANDLE_PP_IF(cond, MSGHANDLE_PP_RPAREN, MSGHANDLE_PP_EMPTY)()
# else
#    define MSGHANDLE_PP_RPAREN_IF(cond) MSGHANDLE_PP_RPAREN_IF_I(cond)
#    define MSGHANDLE_PP_RPAREN_IF_I(cond) MSGHANDLE_PP_IF(cond, MSGHANDLE_PP_RPAREN, MSGHANDLE_PP_EMPTY)()
# endif
#
# endif
