# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* Revised by Paul Mensonides (2011) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_LIST_TO_SEQ_HPP
# define MSGHANDLE_PREPROCESSOR_LIST_TO_SEQ_HPP
#
# include <msghandle/preprocessor/list/for_each.hpp>
#
# /* MSGHANDLE_PP_LIST_TO_SEQ */
#
# define MSGHANDLE_PP_LIST_TO_SEQ(list) \
    MSGHANDLE_PP_LIST_FOR_EACH(MSGHANDLE_PP_LIST_TO_SEQ_MACRO, ~, list) \
    /**/
# define MSGHANDLE_PP_LIST_TO_SEQ_MACRO(r, data, elem) (elem)
#
# /* MSGHANDLE_PP_LIST_TO_SEQ_R */
#
# define MSGHANDLE_PP_LIST_TO_SEQ_R(r, list) \
    MSGHANDLE_PP_LIST_FOR_EACH_R(r, MSGHANDLE_PP_LIST_TO_SEQ_MACRO, ~, list) \
    /**/
#
# endif /* MSGHANDLE_PREPROCESSOR_LIST_TO_SEQ_HPP */
