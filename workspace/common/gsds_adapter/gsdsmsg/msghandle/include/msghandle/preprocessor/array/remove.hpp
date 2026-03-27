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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_REMOVE_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_REMOVE_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/array/elem.hpp>
# include <msghandle/preprocessor/array/push_back.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/control/deduce_d.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ARRAY_REMOVE */
#
# define MSGHANDLE_PP_ARRAY_REMOVE(array, i) MSGHANDLE_PP_ARRAY_REMOVE_I(MSGHANDLE_PP_DEDUCE_D(), array, i)
# define MSGHANDLE_PP_ARRAY_REMOVE_I(d, array, i) MSGHANDLE_PP_ARRAY_REMOVE_D(d, array, i)
#
# /* MSGHANDLE_PP_ARRAY_REMOVE_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_REMOVE_D(d, array, i) MSGHANDLE_PP_TUPLE_ELEM(4, 2, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_REMOVE_P, MSGHANDLE_PP_ARRAY_REMOVE_O, (0, i, (0, ()), array)))
# else
#    define MSGHANDLE_PP_ARRAY_REMOVE_D(d, array, i) MSGHANDLE_PP_ARRAY_REMOVE_D_I(d, array, i)
#    define MSGHANDLE_PP_ARRAY_REMOVE_D_I(d, array, i) MSGHANDLE_PP_TUPLE_ELEM(4, 2, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_REMOVE_P, MSGHANDLE_PP_ARRAY_REMOVE_O, (0, i, (0, ()), array)))
# endif
#
# define MSGHANDLE_PP_ARRAY_REMOVE_P(d, st) MSGHANDLE_PP_NOT_EQUAL(MSGHANDLE_PP_TUPLE_ELEM(4, 0, st), MSGHANDLE_PP_ARRAY_SIZE(MSGHANDLE_PP_TUPLE_ELEM(4, 3, st)))
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ARRAY_REMOVE_O(d, st) MSGHANDLE_PP_ARRAY_REMOVE_O_I st
# else
#    define MSGHANDLE_PP_ARRAY_REMOVE_O(d, st) MSGHANDLE_PP_ARRAY_REMOVE_O_I(MSGHANDLE_PP_TUPLE_ELEM(4, 0, st), MSGHANDLE_PP_TUPLE_ELEM(4, 1, st), MSGHANDLE_PP_TUPLE_ELEM(4, 2, st), MSGHANDLE_PP_TUPLE_ELEM(4, 3, st))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_DMC()
#    define MSGHANDLE_PP_ARRAY_REMOVE_O_I(n, i, res, arr) (MSGHANDLE_PP_INC(n), i, MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(n, i), MSGHANDLE_PP_ARRAY_PUSH_BACK, res MSGHANDLE_PP_TUPLE_EAT_2)(res, MSGHANDLE_PP_ARRAY_ELEM(n, arr)), arr)
# else
#    define MSGHANDLE_PP_ARRAY_REMOVE_O_I(n, i, res, arr) (MSGHANDLE_PP_INC(n), i, MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(n, i), MSGHANDLE_PP_ARRAY_PUSH_BACK, MSGHANDLE_PP_TUPLE_ELEM_2_0)(res, MSGHANDLE_PP_ARRAY_ELEM(n, arr)), arr)
# endif
#
# endif
