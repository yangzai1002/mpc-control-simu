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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_INSERT_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_INSERT_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/array/elem.hpp>
# include <msghandle/preprocessor/array/push_back.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/control/deduce_d.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ARRAY_INSERT */
#
# define MSGHANDLE_PP_ARRAY_INSERT(array, i, elem) MSGHANDLE_PP_ARRAY_INSERT_I(MSGHANDLE_PP_DEDUCE_D(), array, i, elem)
# define MSGHANDLE_PP_ARRAY_INSERT_I(d, array, i, elem) MSGHANDLE_PP_ARRAY_INSERT_D(d, array, i, elem)
#
# /* MSGHANDLE_PP_ARRAY_INSERT_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_INSERT_D(d, array, i, elem) MSGHANDLE_PP_TUPLE_ELEM(5, 3, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_INSERT_P, MSGHANDLE_PP_ARRAY_INSERT_O, (0, i, elem, (0, ()), array)))
# else
#    define MSGHANDLE_PP_ARRAY_INSERT_D(d, array, i, elem) MSGHANDLE_PP_ARRAY_INSERT_D_I(d, array, i, elem)
#    define MSGHANDLE_PP_ARRAY_INSERT_D_I(d, array, i, elem) MSGHANDLE_PP_TUPLE_ELEM(5, 3, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_INSERT_P, MSGHANDLE_PP_ARRAY_INSERT_O, (0, i, elem, (0, ()), array)))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ARRAY_INSERT_P(d, state) MSGHANDLE_PP_ARRAY_INSERT_P_I state
# else
#    define MSGHANDLE_PP_ARRAY_INSERT_P(d, state) MSGHANDLE_PP_ARRAY_INSERT_P_I(nil, nil, nil, MSGHANDLE_PP_TUPLE_ELEM(5, 3, state), MSGHANDLE_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define MSGHANDLE_PP_ARRAY_INSERT_P_I(_i, _ii, _iii, res, arr) MSGHANDLE_PP_NOT_EQUAL(MSGHANDLE_PP_ARRAY_SIZE(res), MSGHANDLE_PP_INC(MSGHANDLE_PP_ARRAY_SIZE(arr)))
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ARRAY_INSERT_O(d, state) MSGHANDLE_PP_ARRAY_INSERT_O_I state
# else
#    define MSGHANDLE_PP_ARRAY_INSERT_O(d, state) MSGHANDLE_PP_ARRAY_INSERT_O_I(MSGHANDLE_PP_TUPLE_ELEM(5, 0, state), MSGHANDLE_PP_TUPLE_ELEM(5, 1, state), MSGHANDLE_PP_TUPLE_ELEM(5, 2, state), MSGHANDLE_PP_TUPLE_ELEM(5, 3, state), MSGHANDLE_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define MSGHANDLE_PP_ARRAY_INSERT_O_I(n, i, elem, res, arr) (MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(MSGHANDLE_PP_ARRAY_SIZE(res), i), MSGHANDLE_PP_INC(n), n), i, elem, MSGHANDLE_PP_ARRAY_PUSH_BACK(res, MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(MSGHANDLE_PP_ARRAY_SIZE(res), i), MSGHANDLE_PP_ARRAY_ELEM(n, arr), elem)), arr)
#
# endif
