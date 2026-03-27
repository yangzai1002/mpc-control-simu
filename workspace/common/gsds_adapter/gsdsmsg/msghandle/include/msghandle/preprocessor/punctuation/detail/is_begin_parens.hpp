# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#ifndef MSGHANDLE_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP
#define MSGHANDLE_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP

#if MSGHANDLE_PP_VARIADICS_MSVC

#include <msghandle/preprocessor/facilities/empty.hpp>

#define MSGHANDLE_PP_DETAIL_VD_IBP_CAT(a, b) MSGHANDLE_PP_DETAIL_VD_IBP_CAT_I(a, b)
#define MSGHANDLE_PP_DETAIL_VD_IBP_CAT_I(a, b) MSGHANDLE_PP_DETAIL_VD_IBP_CAT_II(a ## b)
#define MSGHANDLE_PP_DETAIL_VD_IBP_CAT_II(res) res

#define MSGHANDLE_PP_DETAIL_IBP_SPLIT(i, ...) \
    MSGHANDLE_PP_DETAIL_VD_IBP_CAT(MSGHANDLE_PP_DETAIL_IBP_PRIMITIVE_CAT(MSGHANDLE_PP_DETAIL_IBP_SPLIT_,i)(__VA_ARGS__),MSGHANDLE_PP_EMPTY()) \
/**/

#define MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_C(...) 1 1

#else

#define MSGHANDLE_PP_DETAIL_IBP_SPLIT(i, ...) \
    MSGHANDLE_PP_DETAIL_IBP_PRIMITIVE_CAT(MSGHANDLE_PP_DETAIL_IBP_SPLIT_,i)(__VA_ARGS__) \
/**/

#define MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_C(...) 1

#endif /* MSGHANDLE_PP_VARIADICS_MSVC */

#define MSGHANDLE_PP_DETAIL_IBP_SPLIT_0(a, ...) a
#define MSGHANDLE_PP_DETAIL_IBP_SPLIT_1(a, ...) __VA_ARGS__

#define MSGHANDLE_PP_DETAIL_IBP_CAT(a, ...) MSGHANDLE_PP_DETAIL_IBP_PRIMITIVE_CAT(a,__VA_ARGS__)
#define MSGHANDLE_PP_DETAIL_IBP_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

#define MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_R_1 1,
#define MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_R_MSGHANDLE_PP_DETAIL_IBP_IS_VARIADIC_C 0,

#endif /* MSGHANDLE_PREPROCESSOR_DETAIL_IS_BEGIN_PARENS_HPP */
