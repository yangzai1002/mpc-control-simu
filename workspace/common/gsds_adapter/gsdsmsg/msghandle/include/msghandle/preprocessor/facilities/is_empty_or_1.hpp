# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2003.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_OR_1_HPP
# define MSGHANDLE_PREPROCESSOR_FACILITIES_IS_EMPTY_OR_1_HPP
#
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/facilities/empty.hpp>
# include <msghandle/preprocessor/facilities/identity.hpp>
# include <msghandle/preprocessor/facilities/is_1.hpp>
# include <msghandle/preprocessor/facilities/is_empty.hpp>
#
# /* MSGHANDLE_PP_IS_EMPTY_OR_1 */
#
# define MSGHANDLE_PP_IS_EMPTY_OR_1(x) \
    MSGHANDLE_PP_IIF( \
        MSGHANDLE_PP_IS_EMPTY(x MSGHANDLE_PP_EMPTY()), \
        MSGHANDLE_PP_IDENTITY(1), \
        MSGHANDLE_PP_IS_1 \
    )(x) \
    /**/
#
# endif
