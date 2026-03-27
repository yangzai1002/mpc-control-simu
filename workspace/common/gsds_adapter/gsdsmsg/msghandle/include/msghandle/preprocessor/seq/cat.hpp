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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_CAT_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_CAT_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/seq/fold_left.hpp>
# include <msghandle/preprocessor/seq/seq.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
#
# /* MSGHANDLE_PP_SEQ_CAT */
#
# define MSGHANDLE_PP_SEQ_CAT(seq) \
    MSGHANDLE_PP_IF( \
        MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq)), \
        MSGHANDLE_PP_SEQ_CAT_I, \
        MSGHANDLE_PP_SEQ_HEAD \
    )(seq) \
    /**/
# define MSGHANDLE_PP_SEQ_CAT_I(seq) MSGHANDLE_PP_SEQ_FOLD_LEFT(MSGHANDLE_PP_SEQ_CAT_O, MSGHANDLE_PP_SEQ_HEAD(seq), MSGHANDLE_PP_SEQ_TAIL(seq))
#
# define MSGHANDLE_PP_SEQ_CAT_O(s, st, elem) MSGHANDLE_PP_SEQ_CAT_O_I(st, elem)
# define MSGHANDLE_PP_SEQ_CAT_O_I(a, b) a ## b
#
# /* MSGHANDLE_PP_SEQ_CAT_S */
#
# define MSGHANDLE_PP_SEQ_CAT_S(s, seq) \
    MSGHANDLE_PP_IF( \
        MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq)), \
        MSGHANDLE_PP_SEQ_CAT_S_I_A, \
        MSGHANDLE_PP_SEQ_CAT_S_I_B \
    )(s, seq) \
    /**/
# define MSGHANDLE_PP_SEQ_CAT_S_I_A(s, seq) MSGHANDLE_PP_SEQ_FOLD_LEFT_ ## s(MSGHANDLE_PP_SEQ_CAT_O, MSGHANDLE_PP_SEQ_HEAD(seq), MSGHANDLE_PP_SEQ_TAIL(seq))
# define MSGHANDLE_PP_SEQ_CAT_S_I_B(s, seq) MSGHANDLE_PP_SEQ_HEAD(seq)
#
# endif
