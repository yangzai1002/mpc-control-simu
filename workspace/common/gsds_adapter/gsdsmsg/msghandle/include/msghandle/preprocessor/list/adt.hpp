# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  *
#  * See http://www.boost.org for most recent version.
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# ifndef MSGHANDLE_PREPROCESSOR_LIST_ADT_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_ADT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/detail/is_binary.hpp>
# include <msghandle/preprocessor/logical/compl.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
#
# /* MSGHANDLE_PP_LIST_CONS */
#
# define MSGHANDLE_PP_LIST_CONS(head, tail) (head, tail)
#
# /* MSGHANDLE_PP_LIST_NIL */
#
# define MSGHANDLE_PP_LIST_NIL MSGHANDLE_PP_NIL
#
# /* MSGHANDLE_PP_LIST_FIRST */
#
# define MSGHANDLE_PP_LIST_FIRST(list) MSGHANDLE_PP_LIST_FIRST_D(list)
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_LIST_FIRST_D(list) MSGHANDLE_PP_LIST_FIRST_I list
# else
#    define MSGHANDLE_PP_LIST_FIRST_D(list) MSGHANDLE_PP_LIST_FIRST_I ## list
# endif
#
# define MSGHANDLE_PP_LIST_FIRST_I(head, tail) head
#
# /* MSGHANDLE_PP_LIST_REST */
#
# define MSGHANDLE_PP_LIST_REST(list) MSGHANDLE_PP_LIST_REST_D(list)
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_LIST_REST_D(list) MSGHANDLE_PP_LIST_REST_I list
# else
#    define MSGHANDLE_PP_LIST_REST_D(list) MSGHANDLE_PP_LIST_REST_I ## list
# endif
#
# define MSGHANDLE_PP_LIST_REST_I(head, tail) tail
#
# /* MSGHANDLE_PP_LIST_IS_CONS */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_BCC()
#    define MSGHANDLE_PP_LIST_IS_CONS(list) MSGHANDLE_PP_LIST_IS_CONS_D(list)
#    define MSGHANDLE_PP_LIST_IS_CONS_D(list) MSGHANDLE_PP_LIST_IS_CONS_ ## list
#    define MSGHANDLE_PP_LIST_IS_CONS_(head, tail) 1
#    define MSGHANDLE_PP_LIST_IS_CONS_MSGHANDLE_PP_NIL 0
# else
#    define MSGHANDLE_PP_LIST_IS_CONS(list) MSGHANDLE_PP_IS_BINARY(list)
# endif
#
# /* MSGHANDLE_PP_LIST_IS_NIL */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_BCC()
#    define MSGHANDLE_PP_LIST_IS_NIL(list) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_IS_BINARY(list))
# else
#    define MSGHANDLE_PP_LIST_IS_NIL(list) MSGHANDLE_PP_COMPL(MSGHANDLE_PP_LIST_IS_CONS(list))
# endif
#
# endif
