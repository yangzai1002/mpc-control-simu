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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_POP_BACK_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_POP_BACK_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/first_n.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
#
# /* MSGHANDLE_PP_SEQ_POP_BACK */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_POP_BACK(seq) MSGHANDLE_PP_SEQ_FIRST_N(MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq)), seq)
# else
#    define MSGHANDLE_PP_SEQ_POP_BACK(seq) MSGHANDLE_PP_SEQ_POP_BACK_I(seq)
#    define MSGHANDLE_PP_SEQ_POP_BACK_I(seq) MSGHANDLE_PP_SEQ_FIRST_N(MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_SIZE(seq)), seq)
# endif
#
# endif
