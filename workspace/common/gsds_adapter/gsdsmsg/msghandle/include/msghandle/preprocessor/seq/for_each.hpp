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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_FOR_EACH_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_FOR_EACH_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/repetition/for.hpp>
# include <msghandle/preprocessor/seq/seq.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
# include <msghandle/preprocessor/seq/detail/is_empty.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_SEQ_FOR_EACH */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_FOR_EACH(macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq)
# else
#    define MSGHANDLE_PP_SEQ_FOR_EACH(macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_D(macro, data, seq)
#    define MSGHANDLE_PP_SEQ_FOR_EACH_D(macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq)
# endif
#
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC(macro, data, seq) MSGHANDLE_PP_FOR((macro, data, seq, MSGHANDLE_PP_SEQ_SIZE(seq)), MSGHANDLE_PP_SEQ_FOR_EACH_P, MSGHANDLE_PP_SEQ_FOR_EACH_O, MSGHANDLE_PP_SEQ_FOR_EACH_M)
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY(macro, data, seq)
#
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK(macro, data, seq) \
		MSGHANDLE_PP_IIF \
			( \
			MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq), \
			MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC, \
			MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY \
			) \
		(macro, data, seq) \
/**/
#
# define MSGHANDLE_PP_SEQ_FOR_EACH_P(r, x) MSGHANDLE_PP_TUPLE_ELEM(4, 3, x)
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_SEQ_FOR_EACH_O(r, x) MSGHANDLE_PP_SEQ_FOR_EACH_O_I x
# else
#    define MSGHANDLE_PP_SEQ_FOR_EACH_O(r, x) MSGHANDLE_PP_SEQ_FOR_EACH_O_I(MSGHANDLE_PP_TUPLE_ELEM(4, 0, x), MSGHANDLE_PP_TUPLE_ELEM(4, 1, x), MSGHANDLE_PP_TUPLE_ELEM(4, 2, x), MSGHANDLE_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define MSGHANDLE_PP_SEQ_FOR_EACH_O_I(macro, data, seq, sz) \
	MSGHANDLE_PP_SEQ_FOR_EACH_O_I_DEC(macro, data, seq, MSGHANDLE_PP_DEC(sz)) \
/**/
# define MSGHANDLE_PP_SEQ_FOR_EACH_O_I_DEC(macro, data, seq, sz) \
	( \
	macro, \
	data, \
	MSGHANDLE_PP_IF \
		( \
		sz, \
		MSGHANDLE_PP_SEQ_FOR_EACH_O_I_TAIL, \
		MSGHANDLE_PP_SEQ_FOR_EACH_O_I_NIL \
		) \
	(seq), \
	sz \
	) \
/**/
# define MSGHANDLE_PP_SEQ_FOR_EACH_O_I_TAIL(seq) MSGHANDLE_PP_SEQ_TAIL(seq)
# define MSGHANDLE_PP_SEQ_FOR_EACH_O_I_NIL(seq) MSGHANDLE_PP_NIL
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_SEQ_FOR_EACH_M(r, x) MSGHANDLE_PP_SEQ_FOR_EACH_M_IM(r, MSGHANDLE_PP_TUPLE_REM_4 x)
#    define MSGHANDLE_PP_SEQ_FOR_EACH_M_IM(r, im) MSGHANDLE_PP_SEQ_FOR_EACH_M_I(r, im)
# else
#    define MSGHANDLE_PP_SEQ_FOR_EACH_M(r, x) MSGHANDLE_PP_SEQ_FOR_EACH_M_I(r, MSGHANDLE_PP_TUPLE_ELEM(4, 0, x), MSGHANDLE_PP_TUPLE_ELEM(4, 1, x), MSGHANDLE_PP_TUPLE_ELEM(4, 2, x), MSGHANDLE_PP_TUPLE_ELEM(4, 3, x))
# endif
#
# define MSGHANDLE_PP_SEQ_FOR_EACH_M_I(r, macro, data, seq, sz) macro(r, data, MSGHANDLE_PP_SEQ_HEAD(seq))
#
# /* MSGHANDLE_PP_SEQ_FOR_EACH_R */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_FOR_EACH_R(r, macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq)
# else
#    define MSGHANDLE_PP_SEQ_FOR_EACH_R(r, macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq)
#    define MSGHANDLE_PP_SEQ_FOR_EACH_R_I(r, macro, data, seq) MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq)
# endif
#
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC_R(r, macro, data, seq) MSGHANDLE_PP_FOR_ ## r((macro, data, seq, MSGHANDLE_PP_SEQ_SIZE(seq)), MSGHANDLE_PP_SEQ_FOR_EACH_P, MSGHANDLE_PP_SEQ_FOR_EACH_O, MSGHANDLE_PP_SEQ_FOR_EACH_M)
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY_R(r, macro, data, seq)
#
#    define MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_R(r, macro, data, seq) \
		MSGHANDLE_PP_IIF \
			( \
			MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq), \
			MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EXEC_R, \
			MSGHANDLE_PP_SEQ_FOR_EACH_DETAIL_CHECK_EMPTY_R \
			) \
		(r, macro, data, seq) \
/**/
#
# endif
