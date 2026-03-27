# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_STRINGIZE_HPP
# define MSGHANDLE_PREPROCESSOR_STRINGIZE_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_STRINGIZE */
#
# if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#    define MSGHANDLE_PP_STRINGIZE(text) MSGHANDLE_PP_STRINGIZE_A((text))
#    define MSGHANDLE_PP_STRINGIZE_A(arg) MSGHANDLE_PP_STRINGIZE_I arg
# elif MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_STRINGIZE(text) MSGHANDLE_PP_STRINGIZE_OO((text))
#    define MSGHANDLE_PP_STRINGIZE_OO(par) MSGHANDLE_PP_STRINGIZE_I ## par
# else
#    define MSGHANDLE_PP_STRINGIZE(text) MSGHANDLE_PP_STRINGIZE_I(text)
# endif
#
# define MSGHANDLE_PP_STRINGIZE_I(text) #text
#
# endif
