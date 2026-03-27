# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2011.                                    *
#  *     (C) Copyright Paul Mensonides 2011.                                  *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_ENUM_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_ENUM_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
#
# /* MSGHANDLE_PP_ARRAY_ENUM */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_ARRAY_ENUM(array) MSGHANDLE_PP_ARRAY_ENUM_I(MSGHANDLE_PP_TUPLE_REM_CTOR, array)
#    define MSGHANDLE_PP_ARRAY_ENUM_I(m, args) MSGHANDLE_PP_ARRAY_ENUM_II(m, args)
#    define MSGHANDLE_PP_ARRAY_ENUM_II(m, args) MSGHANDLE_PP_CAT(m ## args,)
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_ARRAY_ENUM(array) MSGHANDLE_PP_ARRAY_ENUM_I(array)
#    define MSGHANDLE_PP_ARRAY_ENUM_I(array) MSGHANDLE_PP_TUPLE_REM_CTOR ## array
# else
#    define MSGHANDLE_PP_ARRAY_ENUM(array) MSGHANDLE_PP_TUPLE_REM_CTOR array
# endif
#
# endif
