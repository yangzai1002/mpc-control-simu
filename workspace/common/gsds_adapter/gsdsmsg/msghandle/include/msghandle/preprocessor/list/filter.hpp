# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_LIST_FILTER_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_FILTER_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/list/fold_right.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_FILTER */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FILTER(pred, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_FILTER_O, (pred, data, MSGHANDLE_PP_NIL), list))
# else
#    define MSGHANDLE_PP_LIST_FILTER(pred, data, list) MSGHANDLE_PP_LIST_FILTER_I(pred, data, list)
#    define MSGHANDLE_PP_LIST_FILTER_I(pred, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_FILTER_O, (pred, data, MSGHANDLE_PP_NIL), list))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FILTER_O(d, pdr, elem) MSGHANDLE_PP_LIST_FILTER_O_D(d, MSGHANDLE_PP_TUPLE_ELEM(3, 0, pdr), MSGHANDLE_PP_TUPLE_ELEM(3, 1, pdr), MSGHANDLE_PP_TUPLE_ELEM(3, 2, pdr), elem)
# else
#    define MSGHANDLE_PP_LIST_FILTER_O(d, pdr, elem) MSGHANDLE_PP_LIST_FILTER_O_I(d, MSGHANDLE_PP_TUPLE_REM_3 pdr, elem)
#    define MSGHANDLE_PP_LIST_FILTER_O_I(d, im, elem) MSGHANDLE_PP_LIST_FILTER_O_D(d, im, elem)
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_DMC()
#    define MSGHANDLE_PP_LIST_FILTER_O_D(d, pred, data, res, elem) (pred, data, MSGHANDLE_PP_IF(pred(d, data, elem), (elem, res), res))
# else
#    define MSGHANDLE_PP_LIST_FILTER_O_D(d, pred, data, res, elem) (pred, data, MSGHANDLE_PP_IF(pred##(d, data, elem), (elem, res), res))
# endif
#
# /* MSGHANDLE_PP_LIST_FILTER_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FILTER_D(d, pred, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_FILTER_O, (pred, data, MSGHANDLE_PP_NIL), list))
# else
#    define MSGHANDLE_PP_LIST_FILTER_D(d, pred, data, list) MSGHANDLE_PP_LIST_FILTER_D_I(d, pred, data, list)
#    define MSGHANDLE_PP_LIST_FILTER_D_I(d, pred, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_FILTER_O, (pred, data, MSGHANDLE_PP_NIL), list))
# endif
#
# endif
