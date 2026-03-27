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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_FIRST_N_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_FIRST_N_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/seq/detail/split.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_SEQ_FIRST_N */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_FIRST_N(n, seq) MSGHANDLE_PP_IF(n, MSGHANDLE_PP_TUPLE_ELEM, MSGHANDLE_PP_TUPLE_EAT_3)(2, 0, MSGHANDLE_PP_SEQ_SPLIT(n, seq (nil)))
# else
#    define MSGHANDLE_PP_SEQ_FIRST_N(n, seq) MSGHANDLE_PP_SEQ_FIRST_N_I(n, seq)
#    define MSGHANDLE_PP_SEQ_FIRST_N_I(n, seq) MSGHANDLE_PP_IF(n, MSGHANDLE_PP_TUPLE_ELEM, MSGHANDLE_PP_TUPLE_EAT_3)(2, 0, MSGHANDLE_PP_SEQ_SPLIT(n, seq (nil)))
# endif
#
# endif
