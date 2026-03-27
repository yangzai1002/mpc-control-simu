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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_FILTER_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_FILTER_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/expr_if.hpp>
# include <msghandle/preprocessor/facilities/empty.hpp>
# include <msghandle/preprocessor/seq/fold_left.hpp>
# include <msghandle/preprocessor/seq/seq.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_SEQ_FILTER */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_FILTER(pred, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# else
#    define MSGHANDLE_PP_SEQ_FILTER(pred, data, seq) MSGHANDLE_PP_SEQ_FILTER_I(pred, data, seq)
#    define MSGHANDLE_PP_SEQ_FILTER_I(pred, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_SEQ_FILTER_O(s, st, elem) MSGHANDLE_PP_SEQ_FILTER_O_IM(s, MSGHANDLE_PP_TUPLE_REM_3 st, elem)
#    define MSGHANDLE_PP_SEQ_FILTER_O_IM(s, im, elem) MSGHANDLE_PP_SEQ_FILTER_O_I(s, im, elem)
# else
#    define MSGHANDLE_PP_SEQ_FILTER_O(s, st, elem) MSGHANDLE_PP_SEQ_FILTER_O_I(s, MSGHANDLE_PP_TUPLE_ELEM(3, 0, st), MSGHANDLE_PP_TUPLE_ELEM(3, 1, st), MSGHANDLE_PP_TUPLE_ELEM(3, 2, st), elem)
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_DMC()
#   define MSGHANDLE_PP_SEQ_FILTER_O_I(s, pred, data, res, elem) (pred, data, res MSGHANDLE_PP_EXPR_IF(pred(s, data, elem), (elem)))
# else
#   define MSGHANDLE_PP_SEQ_FILTER_O_I(s, pred, data, res, elem) (pred, data, res MSGHANDLE_PP_EXPR_IF(pred##(s, data, elem), (elem)))
# endif
#
# /* MSGHANDLE_PP_SEQ_FILTER_S */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_FILTER_S(s, pred, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# else
#    define MSGHANDLE_PP_SEQ_FILTER_S(s, pred, data, seq) MSGHANDLE_PP_SEQ_FILTER_S_I(s, pred, data, seq)
#    define MSGHANDLE_PP_SEQ_FILTER_S_I(s, pred, data, seq) MSGHANDLE_PP_SEQ_TAIL(MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_FILTER_O, (pred, data, (nil)), seq)))
# endif
#
# endif
