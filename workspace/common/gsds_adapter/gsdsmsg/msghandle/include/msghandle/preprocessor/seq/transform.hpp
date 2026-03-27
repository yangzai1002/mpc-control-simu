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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_TRANSFORM_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_TRANSFORM_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/fold_left.hpp>
# include <msghandle/preprocessor/seq/seq.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_SEQ_TRANSFORM */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_TRANSFORM(op, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define MSGHANDLE_PP_SEQ_TRANSFORM(op, data, seq) MSGHANDLE_PP_SEQ_TRANSFORM_I(op, data, seq)
#    define MSGHANDLE_PP_SEQ_TRANSFORM_I(op, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_SEQ_TRANSFORM_O(s, state, elem) MSGHANDLE_PP_SEQ_TRANSFORM_O_IM(s, MSGHANDLE_PP_TUPLE_REM_3 state, elem)
#    define MSGHANDLE_PP_SEQ_TRANSFORM_O_IM(s, im, elem) MSGHANDLE_PP_SEQ_TRANSFORM_O_I(s, im, elem)
# else
#    define MSGHANDLE_PP_SEQ_TRANSFORM_O(s, state, elem) MSGHANDLE_PP_SEQ_TRANSFORM_O_I(s, MSGHANDLE_PP_TUPLE_ELEM(3, 0, state), MSGHANDLE_PP_TUPLE_ELEM(3, 1, state), MSGHANDLE_PP_TUPLE_ELEM(3, 2, state), elem)
# endif
#
# define MSGHANDLE_PP_SEQ_TRANSFORM_O_I(s, op, data, res, elem) (op, data, res (op(s, data, elem)))
#
# /* MSGHANDLE_PP_SEQ_TRANSFORM_S */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_TRANSFORM_S(s, op, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# else
#    define MSGHANDLE_PP_SEQ_TRANSFORM_S(s, op, data, seq) MSGHANDLE_PP_SEQ_TRANSFORM_S_I(s, op, data, seq)
#    define MSGHANDLE_PP_SEQ_TRANSFORM_S_I(s, op, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_TRANSFORM_O, (op, data, (nil)), seq)))
# endif
#
# endif
