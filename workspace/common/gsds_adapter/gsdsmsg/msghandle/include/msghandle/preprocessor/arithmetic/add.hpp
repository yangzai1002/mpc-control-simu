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
# ifndef MSGHANDLE_PREPROCESSOR_ARITHMETIC_ADD_HPP
# define MSGHANDLE_PREPROCESSOR_ARITHMETIC_ADD_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ADD */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ADD(x, y) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_ADD_P, MSGHANDLE_PP_ADD_O, (x, y)))
# else
#    define MSGHANDLE_PP_ADD(x, y) MSGHANDLE_PP_ADD_I(x, y)
#    define MSGHANDLE_PP_ADD_I(x, y) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE(MSGHANDLE_PP_ADD_P, MSGHANDLE_PP_ADD_O, (x, y)))
# endif
#
# define MSGHANDLE_PP_ADD_P(d, xy) MSGHANDLE_PP_TUPLE_ELEM(2, 1, xy)
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_ADD_O(d, xy) MSGHANDLE_PP_ADD_O_I xy
# else
#    define MSGHANDLE_PP_ADD_O(d, xy) MSGHANDLE_PP_ADD_O_I(MSGHANDLE_PP_TUPLE_ELEM(2, 0, xy), MSGHANDLE_PP_TUPLE_ELEM(2, 1, xy))
# endif
#
# define MSGHANDLE_PP_ADD_O_I(x, y) (MSGHANDLE_PP_INC(x), MSGHANDLE_PP_DEC(y))
#
# /* MSGHANDLE_PP_ADD_D */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ADD_D(d, x, y) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ADD_P, MSGHANDLE_PP_ADD_O, (x, y)))
# else
#    define MSGHANDLE_PP_ADD_D(d, x, y) MSGHANDLE_PP_ADD_D_I(d, x, y)
#    define MSGHANDLE_PP_ADD_D_I(d, x, y) MSGHANDLE_PP_TUPLE_ELEM(2, 0, MSGHANDLE_PP_WHILE_ ## d(MSGHANDLE_PP_ADD_P, MSGHANDLE_PP_ADD_O, (x, y)))
# endif
#
# endif
