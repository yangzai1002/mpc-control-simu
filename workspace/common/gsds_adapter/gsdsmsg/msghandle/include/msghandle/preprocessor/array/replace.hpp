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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_REPLACE_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_REPLACE_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/array/elem.hpp>
# include <msghandle/preprocessor/array/push_back.hpp>
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/deduce_d.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ARRAY_REPLACE */
#
# define MSGHANDLE_PP_ARRAY_REPLACE(array, i, elem) MSGHANDLE_PP_ARRAY_REPLACE_I(MSGHANDLE_PP_DEDUCE_D(), array, i, elem)
# define MSGHANDLE_PP_ARRAY_REPLACE_I(d, array, i, elem) MSGHANDLE_PP_ARRAY_REPLACE_D(d, array, i, elem)
#
# /* MSGHANDLE_PP_ARRAY_REPLACE_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_REPLACE_D(d, array, i, elem) MSGHANDLE_PP_TUPLE_ELEM(5, 3, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_REPLACE_P, MSGHANDLE_PP_ARRAY_REPLACE_O, (0, i, elem, (0, ()), array)))
# else
#    define MSGHANDLE_PP_ARRAY_REPLACE_D(d, array, i, elem) MSGHANDLE_PP_ARRAY_REPLACE_D_I(d, array, i, elem)
#    define MSGHANDLE_PP_ARRAY_REPLACE_D_I(d, array, i, elem) MSGHANDLE_PP_TUPLE_ELEM(5, 3, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ARRAY_REPLACE_P, MSGHANDLE_PP_ARRAY_REPLACE_O, (0, i, elem, (0, ()), array)))
# endif
#
# define MSGHANDLE_PP_ARRAY_REPLACE_P(d, state) MSGHANDLE_PP_NOT_EQUAL(MSGHANDLE_PP_TUPLE_ELEM(5, 0, state), MSGHANDLE_PP_ARRAY_SIZE(MSGHANDLE_PP_TUPLE_ELEM(5, 4, state)))
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ARRAY_REPLACE_O(d, state) MSGHANDLE_PP_ARRAY_REPLACE_O_I state
# else
#    define MSGHANDLE_PP_ARRAY_REPLACE_O(d, state) MSGHANDLE_PP_ARRAY_REPLACE_O_I(MSGHANDLE_PP_TUPLE_ELEM(5, 0, state), MSGHANDLE_PP_TUPLE_ELEM(5, 1, state), MSGHANDLE_PP_TUPLE_ELEM(5, 2, state), MSGHANDLE_PP_TUPLE_ELEM(5, 3, state), MSGHANDLE_PP_TUPLE_ELEM(5, 4, state))
# endif
#
# define MSGHANDLE_PP_ARRAY_REPLACE_O_I(n, i, elem, res, arr) (MSGHANDLE_PP_INC(n), i, elem, MSGHANDLE_PP_ARRAY_PUSH_BACK(res, MSGHANDLE_PP_IIF(MSGHANDLE_PP_NOT_EQUAL(n, i), MSGHANDLE_PP_ARRAY_ELEM(n, arr), elem)), arr)
#
# endif
