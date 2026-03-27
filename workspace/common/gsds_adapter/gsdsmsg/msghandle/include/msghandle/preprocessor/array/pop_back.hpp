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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_POP_BACK_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_POP_BACK_HPP
#
# include <msghandle/preprocessor/arithmetic/dec.hpp>
# include <msghandle/preprocessor/array/elem.hpp>
# include <msghandle/preprocessor/array/size.hpp>
# include <msghandle/preprocessor/repetition/enum.hpp>
# include <msghandle/preprocessor/repetition/deduce_z.hpp>
#
# /* MSGHANDLE_PP_ARRAY_POP_BACK */
#
# define MSGHANDLE_PP_ARRAY_POP_BACK(array) MSGHANDLE_PP_ARRAY_POP_BACK_Z(MSGHANDLE_PP_DEDUCE_Z(), array)
#
# /* MSGHANDLE_PP_ARRAY_POP_BACK_Z */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_EDG()
#    define MSGHANDLE_PP_ARRAY_POP_BACK_Z(z, array) MSGHANDLE_PP_ARRAY_POP_BACK_I(z, MSGHANDLE_PP_ARRAY_SIZE(array), array)
# else
#    define MSGHANDLE_PP_ARRAY_POP_BACK_Z(z, array) MSGHANDLE_PP_ARRAY_POP_BACK_Z_D(z, array)
#    define MSGHANDLE_PP_ARRAY_POP_BACK_Z_D(z, array) MSGHANDLE_PP_ARRAY_POP_BACK_I(z, MSGHANDLE_PP_ARRAY_SIZE(array), array)
# endif
#
# define MSGHANDLE_PP_ARRAY_POP_BACK_I(z, size, array) (MSGHANDLE_PP_DEC(size), (MSGHANDLE_PP_ENUM_ ## z(MSGHANDLE_PP_DEC(size), MSGHANDLE_PP_ARRAY_POP_BACK_M, array)))
# define MSGHANDLE_PP_ARRAY_POP_BACK_M(z, n, data) MSGHANDLE_PP_ARRAY_ELEM(n, data)
#
# endif
