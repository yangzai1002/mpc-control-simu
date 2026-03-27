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
# ifndef MSGHANDLE_PREPROCESSOR_VARIADIC_TO_SEQ_HPP
# define MSGHANDLE_PREPROCESSOR_VARIADIC_TO_SEQ_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/to_seq.hpp>
#
# /* MSGHANDLE_PP_VARIADIC_TO_SEQ */
#
# if MSGHANDLE_PP_VARIADICS
#    define MSGHANDLE_PP_VARIADIC_TO_SEQ(...) MSGHANDLE_PP_TUPLE_TO_SEQ((__VA_ARGS__))
# endif
#
# endif
