# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_TO_LIST_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_TO_LIST_HPP
#
# include <msghandle/preprocessor/punctuation/comma.hpp>
# include <msghandle/preprocessor/punctuation/paren.hpp>
# include <msghandle/preprocessor/seq/detail/binary_transform.hpp>
#
# /* MSGHANDLE_PP_SEQ_TO_LIST */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
# include <msghandle/preprocessor/seq/size.hpp>
# include <msghandle/preprocessor/seq/detail/to_list_msvc.hpp>
# define MSGHANDLE_PP_SEQ_TO_LIST(seq) \
    MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC \
        ( \
        MSGHANDLE_PP_SEQ_TO_LIST_I(MSGHANDLE_PP_SEQ_BINARY_TRANSFORM(seq)), \
        MSGHANDLE_PP_SEQ_SIZE(seq) \
        ) \
/**/
# else
# define MSGHANDLE_PP_SEQ_TO_LIST(seq) MSGHANDLE_PP_SEQ_TO_LIST_I(MSGHANDLE_PP_SEQ_BINARY_TRANSFORM(seq))
# endif
# define MSGHANDLE_PP_SEQ_TO_LIST_I(bseq) MSGHANDLE_PP_SEQ_TO_LIST_A bseq MSGHANDLE_PP_NIL MSGHANDLE_PP_SEQ_TO_LIST_B bseq
# define MSGHANDLE_PP_SEQ_TO_LIST_A(m, e) m(MSGHANDLE_PP_LPAREN() e MSGHANDLE_PP_COMMA() MSGHANDLE_PP_SEQ_TO_LIST_A_ID)
# define MSGHANDLE_PP_SEQ_TO_LIST_A_ID() MSGHANDLE_PP_SEQ_TO_LIST_A
# define MSGHANDLE_PP_SEQ_TO_LIST_B(m, e) m(MSGHANDLE_PP_RPAREN() MSGHANDLE_PP_SEQ_TO_LIST_B_ID)
# define MSGHANDLE_PP_SEQ_TO_LIST_B_ID() MSGHANDLE_PP_SEQ_TO_LIST_B
#
# endif
