# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2003.
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# if MSGHANDLE_PP_VARIADICS
#
# include <msghandle/preprocessor/facilities/is_empty_variadic.hpp>
#
# else
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC() && ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
# include <msghandle/preprocessor/tuple/elem.hpp>
# include <msghandle/preprocessor/facilities/identity.hpp>
# else
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/detail/split.hpp>
# endif
#
# /* MSGHANDLE_PP_IS_EMPTY */
#
# if ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC() && ~MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MWCC()
#    define MSGHANDLE_PP_IS_EMPTY(x) MSGHANDLE_PP_IS_EMPTY_I(x MSGHANDLE_PP_IS_EMPTY_HELPER)
#    define MSGHANDLE_PP_IS_EMPTY_I(contents) MSGHANDLE_PP_TUPLE_ELEM(2, 1, (MSGHANDLE_PP_IS_EMPTY_DEF_ ## contents()))
#    define MSGHANDLE_PP_IS_EMPTY_DEF_MSGHANDLE_PP_IS_EMPTY_HELPER 1, MSGHANDLE_PP_IDENTITY(1)
#    define MSGHANDLE_PP_IS_EMPTY_HELPER() , 0
# else
#    if MSGHANDLE_PP_CONFIG_FLAGS() & MSGHANDLE_PP_CONFIG_MSVC()
#        define MSGHANDLE_PP_IS_EMPTY(x) MSGHANDLE_PP_IS_EMPTY_I(MSGHANDLE_PP_IS_EMPTY_HELPER x ())
#        define MSGHANDLE_PP_IS_EMPTY_I(test) MSGHANDLE_PP_IS_EMPTY_II(MSGHANDLE_PP_SPLIT(0, MSGHANDLE_PP_CAT(MSGHANDLE_PP_IS_EMPTY_DEF_, test)))
#        define MSGHANDLE_PP_IS_EMPTY_II(id) id
#    else
#        define MSGHANDLE_PP_IS_EMPTY(x) MSGHANDLE_PP_IS_EMPTY_I((MSGHANDLE_PP_IS_EMPTY_HELPER x ()))
#        define MSGHANDLE_PP_IS_EMPTY_I(par) MSGHANDLE_PP_IS_EMPTY_II ## par
#        define MSGHANDLE_PP_IS_EMPTY_II(test) MSGHANDLE_PP_SPLIT(0, MSGHANDLE_PP_CAT(MSGHANDLE_PP_IS_EMPTY_DEF_, test))
#    endif
#    define MSGHANDLE_PP_IS_EMPTY_HELPER() 1
#    define MSGHANDLE_PP_IS_EMPTY_DEF_1 1, MSGHANDLE_PP_NIL
#    define MSGHANDLE_PP_IS_EMPTY_DEF_MSGHANDLE_PP_IS_EMPTY_HELPER 0, MSGHANDLE_PP_NIL
# endif
#
# endif /* MSGHANDLE_PP_VARIADICS */
#
# endif /* MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_HPP */
