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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_REVERSE_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_REVERSE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/facilities/empty.hpp>
# include <msghandle/preprocessor/seq/fold_left.hpp>
#
# /* MSGHANDLE_PP_SEQ_REVERSE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_REVERSE(seq) MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_REVERSE_O, MSGHANDLE_PP_EMPTY, seq)()
# else
#    define MSGHANDLE_PP_SEQ_REVERSE(seq) MSGHANDLE_PP_SEQ_REVERSE_I(seq)
#    define MSGHANDLE_PP_SEQ_REVERSE_I(seq) MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_REVERSE_O, MSGHANDLE_PP_EMPTY, seq)()
# endif
#
# define MSGHANDLE_PP_SEQ_REVERSE_O(s, state, elem) (elem) state
#
# /* MSGHANDLE_PP_SEQ_REVERSE_S */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_REVERSE_S(s, seq) MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_REVERSE_O, MSGHANDLE_PP_EMPTY, seq)()
# else
#    define MSGHANDLE_PP_SEQ_REVERSE_S(s, seq) MSGHANDLE_PP_SEQ_REVERSE_S_I(s, seq)
#    define MSGHANDLE_PP_SEQ_REVERSE_S_I(s, seq) MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_REVERSE_O, MSGHANDLE_PP_EMPTY, seq)()
# endif
#
# endif
