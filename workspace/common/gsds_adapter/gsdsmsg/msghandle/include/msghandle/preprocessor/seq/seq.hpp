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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_SEQ_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_SEQ_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/elem.hpp>
#
# /* MSGHANDLE_PP_SEQ_HEAD */
#
# define MSGHANDLE_PP_SEQ_HEAD(seq) MSGHANDLE_PP_SEQ_ELEM(0, seq)
#
# /* MSGHANDLE_PP_SEQ_TAIL */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_SEQ_TAIL(seq) MSGHANDLE_PP_SEQ_TAIL_1((seq))
#    define MSGHANDLE_PP_SEQ_TAIL_1(par) MSGHANDLE_PP_SEQ_TAIL_2 ## par
#    define MSGHANDLE_PP_SEQ_TAIL_2(seq) MSGHANDLE_PP_SEQ_TAIL_I ## seq
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_SEQ_TAIL(seq) MSGHANDLE_PP_SEQ_TAIL_ID(MSGHANDLE_PP_SEQ_TAIL_I seq)
#    define MSGHANDLE_PP_SEQ_TAIL_ID(id) id
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_TAIL(seq) MSGHANDLE_PP_SEQ_TAIL_D(seq)
#    define MSGHANDLE_PP_SEQ_TAIL_D(seq) MSGHANDLE_PP_SEQ_TAIL_I seq
# else
#    define MSGHANDLE_PP_SEQ_TAIL(seq) MSGHANDLE_PP_SEQ_TAIL_I seq
# endif
#
# define MSGHANDLE_PP_SEQ_TAIL_I(x)
#
# /* MSGHANDLE_PP_SEQ_NIL */
#
# define MSGHANDLE_PP_SEQ_NIL(x) (x)
#
# endif
