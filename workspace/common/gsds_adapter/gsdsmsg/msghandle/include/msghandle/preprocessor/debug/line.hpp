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
# ifndef MSGHANDLE_PREPROCESSOR_DEBUG_LINE_HPP
# define MSGHANDLE_PREPROCESSOR_DEBUG_LINE_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/iteration/iterate.hpp>
# include <msghandle/preprocessor/stringize.hpp>
#
# /* MSGHANDLE_PP_LINE */
#
# if MSGHANDLE_PP_CONFIG_EXTENDED_LINE_INFO
#    define MSGHANDLE_PP_LINE(line, file) line MSGHANDLE_PP_CAT(MSGHANDLE_PP_LINE_, MSGHANDLE_PP_IS_ITERATING)(file)
#    define MSGHANDLE_PP_LINE_MSGHANDLE_PP_IS_ITERATING(file) #file
#    define MSGHANDLE_PP_LINE_1(file) MSGHANDLE_PP_STRINGIZE(file MSGHANDLE_PP_CAT(MSGHANDLE_PP_LINE_I_, MSGHANDLE_PP_ITERATION_DEPTH())())
#    define MSGHANDLE_PP_LINE_I_1() [MSGHANDLE_PP_FRAME_ITERATION(1)]
#    define MSGHANDLE_PP_LINE_I_2() MSGHANDLE_PP_LINE_I_1()[MSGHANDLE_PP_FRAME_ITERATION(2)]
#    define MSGHANDLE_PP_LINE_I_3() MSGHANDLE_PP_LINE_I_2()[MSGHANDLE_PP_FRAME_ITERATION(3)]
#    define MSGHANDLE_PP_LINE_I_4() MSGHANDLE_PP_LINE_I_3()[MSGHANDLE_PP_FRAME_ITERATION(4)]
#    define MSGHANDLE_PP_LINE_I_5() MSGHANDLE_PP_LINE_I_4()[MSGHANDLE_PP_FRAME_ITERATION(5)]
# else
#    define MSGHANDLE_PP_LINE(line, file) line __FILE__
# endif
#
# endif
