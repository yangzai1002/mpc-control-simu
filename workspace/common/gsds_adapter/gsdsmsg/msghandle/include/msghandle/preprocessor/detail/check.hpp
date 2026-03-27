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
# ifndef MSGHANDLE_PREPROCESSOR_DETAIL_CHECK_HPP
# define MSGHANDLE_PREPROCESSOR_DETAIL_CHECK_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_CHECK */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_CHECK(x, type) MSGHANDLE_PP_CHECK_D(x, type)
# else
#    define MSGHANDLE_PP_CHECK(x, type) MSGHANDLE_PP_CHECK_OO((x, type))
#    define MSGHANDLE_PP_CHECK_OO(par) MSGHANDLE_PP_CHECK_D ## par
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC() && ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_DMC()
#    define MSGHANDLE_PP_CHECK_D(x, type) MSGHANDLE_PP_CHECK_1(MSGHANDLE_PP_CAT(MSGHANDLE_PP_CHECK_RESULT_, type x))
#    define MSGHANDLE_PP_CHECK_1(chk) MSGHANDLE_PP_CHECK_2(chk)
#    define MSGHANDLE_PP_CHECK_2(res, _) res
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_CHECK_D(x, type) MSGHANDLE_PP_CHECK_1(type x)
#    define MSGHANDLE_PP_CHECK_1(chk) MSGHANDLE_PP_CHECK_2(chk)
#    define MSGHANDLE_PP_CHECK_2(chk) MSGHANDLE_PP_CHECK_3((MSGHANDLE_PP_CHECK_RESULT_ ## chk))
#    define MSGHANDLE_PP_CHECK_3(im) MSGHANDLE_PP_CHECK_5(MSGHANDLE_PP_CHECK_4 im)
#    define MSGHANDLE_PP_CHECK_4(res, _) res
#    define MSGHANDLE_PP_CHECK_5(res) res
# else /* DMC */
#    define MSGHANDLE_PP_CHECK_D(x, type) MSGHANDLE_PP_CHECK_OO((type x))
#    define MSGHANDLE_PP_CHECK_OO(par) MSGHANDLE_PP_CHECK_0 ## par
#    define MSGHANDLE_PP_CHECK_0(chk) MSGHANDLE_PP_CHECK_1(MSGHANDLE_PP_CAT(MSGHANDLE_PP_CHECK_RESULT_, chk))
#    define MSGHANDLE_PP_CHECK_1(chk) MSGHANDLE_PP_CHECK_2(chk)
#    define MSGHANDLE_PP_CHECK_2(res, _) res
# endif
#
# define MSGHANDLE_PP_CHECK_RESULT_1 1, MSGHANDLE_PP_NIL
#
# endif
