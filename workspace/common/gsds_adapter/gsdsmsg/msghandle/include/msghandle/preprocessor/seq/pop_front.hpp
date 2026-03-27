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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_POP_FRONT_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_POP_FRONT_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/seq.hpp>
#
# /* MSGHANDLE_PP_SEQ_POP_FRONT */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_POP_FRONT(seq) MSGHANDLE_PP_SEQ_TAIL(seq)
# else
#    define MSGHANDLE_PP_SEQ_POP_FRONT(seq) MSGHANDLE_PP_SEQ_POP_FRONT_I(seq)
#    define MSGHANDLE_PP_SEQ_POP_FRONT_I(seq) MSGHANDLE_PP_SEQ_TAIL(seq)
# endif
#
# endif
