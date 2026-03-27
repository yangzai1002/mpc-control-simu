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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_DEDUCE_Z_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_DEDUCE_Z_HPP
#
# include <msghandle/preprocessor/detail/auto_rec.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
#
# /* MSGHANDLE_PP_DEDUCE_Z */
#
# define MSGHANDLE_PP_DEDUCE_Z() MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_REPEAT_P, 4)
#
# endif
