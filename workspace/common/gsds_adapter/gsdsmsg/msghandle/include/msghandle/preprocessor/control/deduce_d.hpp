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
# ifndef MSGHANDLE_PREPROCESSOR_CONTROL_DEDUCE_D_HPP
# define MSGHANDLE_PREPROCESSOR_CONTROL_DEDUCE_D_HPP
#
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/detail/auto_rec.hpp>
#
# /* MSGHANDLE_PP_DEDUCE_D */
#
# define MSGHANDLE_PP_DEDUCE_D() MSGHANDLE_PP_AUTO_REC(MSGHANDLE_PP_WHILE_P, 256)
#
# endif
