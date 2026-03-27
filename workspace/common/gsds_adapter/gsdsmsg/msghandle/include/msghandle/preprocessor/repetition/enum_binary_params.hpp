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
# ifndef MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_BINARY_PARAMS_HPP
# define MSGHANDLE_PREPROCESSOR_REPETITION_ENUM_BINARY_PARAMS_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/punctuation/comma_if.hpp>
# include <msghandle/preprocessor/repetition/repeat.hpp>
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_ENUM_BINARY_PARAMS */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS(count, p1, p2) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# else
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS(count, p1, p2) MSGHANDLE_PP_ENUM_BINARY_PARAMS_I(count, p1, p2)
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_I(count, p1, p2) MSGHANDLE_PP_REPEAT(count, MSGHANDLE_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# endif
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_STRICT()
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M(z, n, pp) MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_IM(z, n, MSGHANDLE_PP_TUPLE_REM_2 pp)
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_IM(z, n, im) MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_I(z, n, im)
# else
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M(z, n, pp) MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_I(z, n, MSGHANDLE_PP_TUPLE_ELEM(2, 0, pp), MSGHANDLE_PP_TUPLE_ELEM(2, 1, pp))
# endif
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_I(z, n, p1, p2) MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_II(z, n, p1, p2)
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_II(z, n, p1, p2) MSGHANDLE_PP_COMMA_IF(n) p1 ## n p2 ## n
# else
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_M_I(z, n, p1, p2) MSGHANDLE_PP_COMMA_IF(n) MSGHANDLE_PP_CAT(p1, n) MSGHANDLE_PP_CAT(p2, n)
# endif
#
# /* MSGHANDLE_PP_ENUM_BINARY_PARAMS_Z */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_Z(z, count, p1, p2) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# else
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_Z(z, count, p1, p2) MSGHANDLE_PP_ENUM_BINARY_PARAMS_Z_I(z, count, p1, p2)
#    define MSGHANDLE_PP_ENUM_BINARY_PARAMS_Z_I(z, count, p1, p2) MSGHANDLE_PP_REPEAT_ ## z(count, MSGHANDLE_PP_ENUM_BINARY_PARAMS_M, (p1, p2))
# endif
#
# endif
