# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2015.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_IS_EMPTY_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_IS_EMPTY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/logical/bool.hpp>
# include <msghandle/preprocessor/logical/compl.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
#
/* An empty seq is one that is just MSGHANDLE_PP_SEQ_NIL */
#
# define MSGHANDLE_PP_SEQ_DETAIL_IS_EMPTY(seq) \
	MSGHANDLE_PP_COMPL \
		( \
		MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq) \
		) \
/**/
#
# define MSGHANDLE_PP_SEQ_DETAIL_IS_EMPTY_SIZE(size) \
	MSGHANDLE_PP_COMPL \
		( \
		MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size) \
		) \
/**/
#
# define MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY(seq) \
	MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(MSGHANDLE_PP_SEQ_DETAIL_EMPTY_SIZE(seq)) \
/**/
#
# define MSGHANDLE_PP_SEQ_DETAIL_IS_NOT_EMPTY_SIZE(size) \
	MSGHANDLE_PP_BOOL(size) \
/**/
#
# define MSGHANDLE_PP_SEQ_DETAIL_EMPTY_SIZE(seq) \
	MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq (nil))) \
/**/
#
# endif
