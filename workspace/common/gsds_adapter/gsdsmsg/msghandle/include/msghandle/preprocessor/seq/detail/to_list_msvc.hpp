# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2016.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/control/while.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
#
# define MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT(state) \
    MSGHANDLE_PP_TUPLE_ELEM(2, 0, state) \
/**/
# define MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state) \
    MSGHANDLE_PP_TUPLE_ELEM(2, 1, state) \
/**/
# define MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_PRED(d,state) \
    MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state) \
/**/
# define MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_OP(d,state) \
    ( \
    MSGHANDLE_PP_CAT(MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT(state),), \
    MSGHANDLE_PP_DEC(MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_SIZE(state)) \
    ) \
/**/
#
# /* MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC */
#
# define MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC(result,seqsize) \
    MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_STATE_RESULT \
        ( \
        MSGHANDLE_PP_WHILE \
            ( \
            MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_PRED, \
            MSGHANDLE_PP_SEQ_DETAIL_TO_LIST_MSVC_OP, \
            (result,seqsize) \
            ) \
        ) \
/**/
# endif // MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#
# endif // MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_TO_LIST_MSVC_HPP
