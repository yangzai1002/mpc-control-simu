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
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_TO_ARRAY_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_TO_ARRAY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/seq/enum.hpp>
# include <msghandle/preprocessor/seq/size.hpp>
#
# /* MSGHANDLE_PP_SEQ_TO_ARRAY */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_SEQ_TO_ARRAY(seq) (MSGHANDLE_PP_SEQ_SIZE(seq), (MSGHANDLE_PP_SEQ_ENUM(seq)))
# else
#    define MSGHANDLE_PP_SEQ_TO_ARRAY(seq) MSGHANDLE_PP_SEQ_TO_ARRAY_I(seq)
#    define MSGHANDLE_PP_SEQ_TO_ARRAY_I(seq) (MSGHANDLE_PP_SEQ_SIZE(seq), (MSGHANDLE_PP_SEQ_ENUM(seq)))
# endif
#
# endif
