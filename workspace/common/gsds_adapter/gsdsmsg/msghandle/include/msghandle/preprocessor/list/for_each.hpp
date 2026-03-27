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
# ifndef MSGHANDLE_PREPROCESSOR_LIST_FOR_EACH_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_FOR_EACH_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/list/for_each_i.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_LIST_FOR_EACH */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FOR_EACH(macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I(MSGHANDLE_PP_LIST_FOR_EACH_O, (macro, data), list)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH(macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_X(macro, data, list)
#    define MSGHANDLE_PP_LIST_FOR_EACH_X(macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I(MSGHANDLE_PP_LIST_FOR_EACH_O, (macro, data), list)
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FOR_EACH_O(r, md, i, elem) MSGHANDLE_PP_LIST_FOR_EACH_O_D(r, MSGHANDLE_PP_TUPLE_ELEM(2, 0, md), MSGHANDLE_PP_TUPLE_ELEM(2, 1, md), elem)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_O(r, md, i, elem) MSGHANDLE_PP_LIST_FOR_EACH_O_I(r, MSGHANDLE_PP_TUPLE_REM_2 md, elem)
#    define MSGHANDLE_PP_LIST_FOR_EACH_O_I(r, im, elem) MSGHANDLE_PP_LIST_FOR_EACH_O_D(r, im, elem)
# endif
#
# define MSGHANDLE_PP_LIST_FOR_EACH_O_D(r, m, d, elem) m(r, d, elem)
#
# /* MSGHANDLE_PP_LIST_FOR_EACH_R */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_LIST_FOR_EACH_R(r, macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, MSGHANDLE_PP_LIST_FOR_EACH_O, (macro, data), list)
# else
#    define MSGHANDLE_PP_LIST_FOR_EACH_R(r, macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_R_X(r, macro, data, list)
#    define MSGHANDLE_PP_LIST_FOR_EACH_R_X(r, macro, data, list) MSGHANDLE_PP_LIST_FOR_EACH_I_R(r, MSGHANDLE_PP_LIST_FOR_EACH_O, (macro, data), list)
# endif
#
# endif
