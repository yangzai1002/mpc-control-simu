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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_REST_N_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_REST_N_HPP
#
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/comparison/not_equal.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/expr_iif.hpp>
# include <msghandle/preprocessor/facilities/identity.hpp>
# include <msghandle/preprocessor/logical/bitand.hpp>
# include <msghandle/preprocessor/seq/detail/is_empty.hpp>
# include <msghandle/preprocessor/seq/detail/split.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_SEQ_REST_N */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_REST_N(n, seq) MSGHANDLE_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, MSGHANDLE_PP_SEQ_DETAIL_EMPTY_SIZE(seq))
# else
#    define MSGHANDLE_PP_SEQ_REST_N(n, seq) MSGHANDLE_PP_SEQ_REST_N_I(n, seq)
#    define MSGHANDLE_PP_SEQ_REST_N_I(n, seq) MSGHANDLE_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, MSGHANDLE_PP_SEQ_DETAIL_EMPTY_SIZE(seq))
# endif
#
#    define MSGHANDLE_PP_SEQ_REST_N_DETAIL_EXEC(n, seq, size) \
		MSGHANDLE_PP_EXPR_IIF \
			( \
			MSGHANDLE_PP_BITAND \
				( \
				MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size), \
				MSGHANDLE_PP_NOT_EQUAL(n,size) \
				), \
			MSGHANDLE_PP_TUPLE_ELEM(2, 1, MSGHANDLE_PP_SEQ_SPLIT(MSGHANDLE_PP_INC(n), MSGHANDLE_PP_IDENTITY( (nil) seq )))() \
			) \
/**/
#
# endif
