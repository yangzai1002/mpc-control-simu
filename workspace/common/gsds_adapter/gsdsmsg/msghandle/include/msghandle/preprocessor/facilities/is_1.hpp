# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2003.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_IS_1_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_IS_1_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/facilities/is_empty.hpp>
#
# /* MSGHANDLE_PP_IS_1 */
#
# define MSGHANDLE_PP_IS_1(x) MSGHANDLE_PP_IS_EMPTY(MSGHANDLE_PP_CAT(MSGHANDLE_PP_IS_1_HELPER_, x))
# define MSGHANDLE_PP_IS_1_HELPER_1
#
# endif
