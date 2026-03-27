# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_BINARY_TRANSFORM_HPP
# define MSGHANDLE_PREPROCESSOR_SEQ_DETAIL_BINARY_TRANSFORM_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/eat.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
# include <msghandle/preprocessor/variadic/detail/is_single_return.hpp>
#
# /* MSGHANDLE_PP_SEQ_BINARY_TRANSFORM */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM(seq) MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_I(, seq)
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_I(p, seq) MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_II(p ## seq)
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_II(seq) MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_III(seq)
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_III(seq) MSGHANDLE_PP_CAT(MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A seq, 0)
# else
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM(seq) MSGHANDLE_PP_CAT(MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A seq, 0)
# endif
# if MSGHANDLE_PP_VARIADICS
#    if MSGHANDLE_PP_VARIADICS_MSVC
#		define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_REM(data) data
#       define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A(...) (MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_REM, __VA_ARGS__)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B
#       define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B(...) (MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_REM, __VA_ARGS__)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A
#	 else
#       define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A(...) (MSGHANDLE_PP_REM, __VA_ARGS__)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B
#       define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B(...) (MSGHANDLE_PP_REM, __VA_ARGS__)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A
#	 endif
# else
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A(e) (MSGHANDLE_PP_REM, e)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B
#    define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B(e) (MSGHANDLE_PP_REM, e)() MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A
# endif
# define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_A0 (MSGHANDLE_PP_EAT, ?)
# define MSGHANDLE_PP_SEQ_BINARY_TRANSFORM_B0 (MSGHANDLE_PP_EAT, ?)
#
# endif
