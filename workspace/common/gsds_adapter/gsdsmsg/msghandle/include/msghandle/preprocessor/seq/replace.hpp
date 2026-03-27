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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_REPLACE_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_REPLACE_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/comparison/equal.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/seq/first_n.hpp>
# include <msghandle/preprocessor/seq/rest_n.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
#
# /* MSGHANDLE_PP_SEQ_REPLACE */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_REPLACE(seq, i, elem) MSGHANDLE_PP_SEQ_FIRST_N(i, seq) (elem) MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST(seq, i)
# else
#    define MSGHANDLE_PP_SEQ_REPLACE(seq, i, elem) MSGHANDLE_PP_SEQ_REPLACE_I(seq, i, elem)
#    define MSGHANDLE_PP_SEQ_REPLACE_I(seq, i, elem) MSGHANDLE_PP_SEQ_FIRST_N(i, seq) (elem) MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST(seq, i)
# endif
#
#    define MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST_EMPTY(seq, i)
#    define MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST_VALID(seq, i) MSGHANDLE_PP_SEQ_REST_N(MSGHANDLE_PP_INC(i), seq)
#    define MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST(seq, i) \
		MSGHANDLE_PP_IIF \
			( \
			MSGHANDLE_PP_EQUAL(i,MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq))), \
			MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST_EMPTY, \
			MSGHANDLE_PP_SEQ_REPLACE_DETAIL_REST_VALID \
			) \
		(seq, i) \
/**/
#
# endif
