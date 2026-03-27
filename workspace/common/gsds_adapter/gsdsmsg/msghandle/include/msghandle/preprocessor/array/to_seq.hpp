# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_TO_SEQ_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_TO_SEQ_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/tuple/to_seq.hpp>
#
# /* MSGHANDLE_PP_ARRAY_TO_SEQ */
#
#    define MSGHANDLE_PP_ARRAY_TO_SEQ(array) \
		MSGHANDLE_PP_IF \
			( \
			MSGHANDLE_PP_ARRAY_SIZE(array), \
			MSGHANDLE_PP_ARRAY_TO_SEQ_DO, \
			MSGHANDLE_PP_ARRAY_TO_SEQ_EMPTY \
			) \
		(array) \
/**/
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_EMPTY(array)
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_DO(array) MSGHANDLE_PP_ARRAY_TO_SEQ_I(MSGHANDLE_PP_TUPLE_TO_SEQ, array)
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_I(m, args) MSGHANDLE_PP_ARRAY_TO_SEQ_II(m, args)
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_II(m, args) MSGHANDLE_PP_CAT(m ## args,)
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_DO(array) MSGHANDLE_PP_ARRAY_TO_SEQ_I(array)
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_I(array) MSGHANDLE_PP_TUPLE_TO_SEQ ## array
# else
#    define MSGHANDLE_PP_ARRAY_TO_SEQ_DO(array) MSGHANDLE_PP_TUPLE_TO_SEQ array
# endif
#
# endif
