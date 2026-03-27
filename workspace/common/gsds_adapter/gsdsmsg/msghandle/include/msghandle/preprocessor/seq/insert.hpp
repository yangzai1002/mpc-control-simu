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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_INSERT_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_INSERT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/first_n.hpp>
# include <msghandle/preprocessor/seq/rest_n.hpp>
#
# /* MSGHANDLE_PP_SEQ_INSERT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_INSERT(seq, i, elem) MSGHANDLE_PP_SEQ_FIRST_N(i, seq) (elem) MSGHANDLE_PP_SEQ_REST_N(i, seq)
# else
#    define MSGHANDLE_PP_SEQ_INSERT(seq, i, elem) MSGHANDLE_PP_SEQ_INSERT_I(seq, i, elem)
#    define MSGHANDLE_PP_SEQ_INSERT_I(seq, i, elem) MSGHANDLE_PP_SEQ_FIRST_N(i, seq) (elem) MSGHANDLE_PP_SEQ_REST_N(i, seq)
# endif
#
# endif
