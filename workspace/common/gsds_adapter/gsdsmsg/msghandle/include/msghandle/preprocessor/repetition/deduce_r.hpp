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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_DEDUCE_R_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_DEDUCE_R_HPP
#
# include <msghandle/preprocessor/detail/auto_rec.hpp>
# include <msghandle/preprocessor/repetition/for.hpp>
#
# /* MSGHANDLE_PP_DEDUCE_R */
#
# define MSGHANDLE_PP_DEDUCE_R() MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_FOR_P, 256)
#
# endif
