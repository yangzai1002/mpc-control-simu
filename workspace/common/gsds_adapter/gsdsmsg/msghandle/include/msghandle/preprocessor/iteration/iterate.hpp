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
# ifndef MSGHANDLE_PREPROCESSOR_ITERATION_ITERATE_HPP
# define MSGHANDLE_PREPROCESSOR_ITERATION_ITERATE_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/arithmetic/inc.hpp>
# include <msghandle/preprocessor/array/elem.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/slot/slot.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# /* MSGHANDLE_PP_ITERATION_DEPTH */
#
# define MSGHANDLE_PP_ITERATION_DEPTH() 0
#
# /* MSGHANDLE_PP_ITERATION */
#
# define MSGHANDLE_PP_ITERATION() MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_, MSGHANDLE_PP_ITERATION_DEPTH())
#
# /* MSGHANDLE_PP_ITERATION_START && MSGHANDLE_PP_ITERATION_FINISH */
#
# define MSGHANDLE_PP_ITERATION_START() MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_START_, MSGHANDLE_PP_ITERATION_DEPTH())
# define MSGHANDLE_PP_ITERATION_FINISH() MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_FINISH_, MSGHANDLE_PP_ITERATION_DEPTH())
#
# /* MSGHANDLE_PP_ITERATION_FLAGS */
#
# define MSGHANDLE_PP_ITERATION_FLAGS() (MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_FLAGS_, MSGHANDLE_PP_ITERATION_DEPTH())())
#
# /* MSGHANDLE_PP_FRAME_ITERATION */
#
# define MSGHANDLE_PP_FRAME_ITERATION(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_, i)
#
# /* MSGHANDLE_PP_FRAME_START && MSGHANDLE_PP_FRAME_FINISH */
#
# define MSGHANDLE_PP_FRAME_START(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_START_, i)
# define MSGHANDLE_PP_FRAME_FINISH(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_FINISH_, i)
#
# /* MSGHANDLE_PP_FRAME_FLAGS */
#
# define MSGHANDLE_PP_FRAME_FLAGS(i) (MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATION_FLAGS_, i)())
#
# /* MSGHANDLE_PP_RELATIVE_ITERATION */
#
# define MSGHANDLE_PP_RELATIVE_ITERATION(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_RELATIVE_, i)(MSGHANDLE_PP_ITERATION_)
#
# define MSGHANDLE_PP_RELATIVE_0(m) MSGHANDLE_PP_CAT(m, MSGHANDLE_PP_ITERATION_DEPTH())
# define MSGHANDLE_PP_RELATIVE_1(m) MSGHANDLE_PP_CAT(m, MSGHANDLE_PP_DEC(MSGHANDLE_PP_ITERATION_DEPTH()))
# define MSGHANDLE_PP_RELATIVE_2(m) MSGHANDLE_PP_CAT(m, MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_ITERATION_DEPTH())))
# define MSGHANDLE_PP_RELATIVE_3(m) MSGHANDLE_PP_CAT(m, MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_ITERATION_DEPTH()))))
# define MSGHANDLE_PP_RELATIVE_4(m) MSGHANDLE_PP_CAT(m, MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_DEC(MSGHANDLE_PP_ITERATION_DEPTH())))))
#
# /* MSGHANDLE_PP_RELATIVE_START && MSGHANDLE_PP_RELATIVE_FINISH */
#
# define MSGHANDLE_PP_RELATIVE_START(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_RELATIVE_, i)(MSGHANDLE_PP_ITERATION_START_)
# define MSGHANDLE_PP_RELATIVE_FINISH(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_RELATIVE_, i)(MSGHANDLE_PP_ITERATION_FINISH_)
#
# /* MSGHANDLE_PP_RELATIVE_FLAGS */
#
# define MSGHANDLE_PP_RELATIVE_FLAGS(i) (MSGHANDLE_PP_CAT(MSGHANDLE_PP_RELATIVE_, i)(MSGHANDLE_PP_ITERATION_FLAGS_)())
#
# /* MSGHANDLE_PP_ITERATE */
#
# define MSGHANDLE_PP_ITERATE() MSGHANDLE_PP_CAT(MSGHANDLE_PP_ITERATE_, MSGHANDLE_PP_INC(MSGHANDLE_PP_ITERATION_DEPTH()))
#
# define MSGHANDLE_PP_ITERATE_1 <msghandle/preprocessor/iteration/detail/iter/forward1.hpp>
# define MSGHANDLE_PP_ITERATE_2 <msghandle/preprocessor/iteration/detail/iter/forward2.hpp>
# define MSGHANDLE_PP_ITERATE_3 <msghandle/preprocessor/iteration/detail/iter/forward3.hpp>
# define MSGHANDLE_PP_ITERATE_4 <msghandle/preprocessor/iteration/detail/iter/forward4.hpp>
# define MSGHANDLE_PP_ITERATE_5 <msghandle/preprocessor/iteration/detail/iter/forward5.hpp>
#
# endif
