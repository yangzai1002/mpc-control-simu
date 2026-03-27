# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2012.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_VARIADIC_SEQ_TO_SEQ_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_VARIADIC_SEQ_TO_SEQ_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ */
#
# if MSGHANDLE_PP_VARIADICS
#    define MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ(vseq) MSGHANDLE_PP_CAT(MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_A vseq, 0)
#    define MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_A(...) ((__VA_ARGS__)) MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_B
#    define MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_B(...) ((__VA_ARGS__)) MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_A
#    define MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_A0
#    define MSGHANDLE_PP_VARIADIC_SEQ_TO_SEQ_B0
# endif
#
# endif
