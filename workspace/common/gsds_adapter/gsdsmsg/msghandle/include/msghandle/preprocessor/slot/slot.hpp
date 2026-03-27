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
# ifndef MSGHANDLE_PREPROCESSOR_SLOT_SLOT_HPP
# define MSGHANDLE_PREPROCESSOR_SLOT_SLOT_HPP
#
# include <msghandle/preprocessor/cat.hpp>
# include <msghandle/preprocessor/slot/detail/def.hpp>
#
# /* MSGHANDLE_PP_ASSIGN_SLOT */
#
# define MSGHANDLE_PP_ASSIGN_SLOT(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_ASSIGN_SLOT_, i)
#
# define MSGHANDLE_PP_ASSIGN_SLOT_1 <msghandle/preprocessor/slot/detail/slot1.hpp>
# define MSGHANDLE_PP_ASSIGN_SLOT_2 <msghandle/preprocessor/slot/detail/slot2.hpp>
# define MSGHANDLE_PP_ASSIGN_SLOT_3 <msghandle/preprocessor/slot/detail/slot3.hpp>
# define MSGHANDLE_PP_ASSIGN_SLOT_4 <msghandle/preprocessor/slot/detail/slot4.hpp>
# define MSGHANDLE_PP_ASSIGN_SLOT_5 <msghandle/preprocessor/slot/detail/slot5.hpp>
#
# /* MSGHANDLE_PP_SLOT */
#
# define MSGHANDLE_PP_SLOT(i) MSGHANDLE_PP_CAT(MSGHANDLE_PP_SLOT_, i)()
#
# endif
