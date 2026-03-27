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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_TRANSFORM_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_TRANSFORM_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/fold_right.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_TRANSFORM */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_TRANSFORM(op, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_TRANSFORM_O, (op, data, MSGHANDLE_PP_NIL), list))
# else
#    define MSGHANDLE_PP_LIST_TRANSFORM(op, data, list) MSGHANDLE_PP_LIST_TRANSFORM_I(op, data, list)
#    define MSGHANDLE_PP_LIST_TRANSFORM_I(op, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT(MSGHANDLE_PP_LIST_TRANSFORM_O, (op, data, MSGHANDLE_PP_NIL), list))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_TRANSFORM_O(d, odr, elem) MSGHANDLE_PP_LIST_TRANSFORM_O_D(d, MSGHANDLE_PP_TUPLE_ELEM(3, 0, odr), MSGHANDLE_PP_TUPLE_ELEM(3, 1, odr), MSGHANDLE_PP_TUPLE_ELEM(3, 2, odr), elem)
# else
#    define MSGHANDLE_PP_LIST_TRANSFORM_O(d, odr, elem) MSGHANDLE_PP_LIST_TRANSFORM_O_I(d, MSGHANDLE_PP_TUPLE_REM_3 odr, elem)
#    define MSGHANDLE_PP_LIST_TRANSFORM_O_I(d, im, elem) MSGHANDLE_PP_LIST_TRANSFORM_O_D(d, im, elem)
# endif
#
# define MSGHANDLE_PP_LIST_TRANSFORM_O_D(d, op, data, res, elem) (op, data, (op(d, data, elem), res))
#
# /* MSGHANDLE_PP_LIST_TRANSFORM_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_TRANSFORM_D(d, op, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_TRANSFORM_O, (op, data, MSGHANDLE_PP_NIL), list))
# else
#    define MSGHANDLE_PP_LIST_TRANSFORM_D(d, op, data, list) MSGHANDLE_PP_LIST_TRANSFORM_D_I(d, op, data, list)
#    define MSGHANDLE_PP_LIST_TRANSFORM_D_I(d, op, data, list) MSGHANDLE_PP_TUPLE_ELEM(3, 2, MSGHANDLE_PP_LIST_FOLD_RIGHT_ ## d(MSGHANDLE_PP_LIST_TRANSFORM_O, (op, data, MSGHANDLE_PP_NIL), list))
# endif
#
# endif
