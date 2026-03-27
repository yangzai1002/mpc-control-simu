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
# if !defined(MSGHANDLE_PP_INDIRECT_SELF)
#    error MSGHANDLE_PP_ERROR:  no indirect file to include
# endif
#
# define MSGHANDLE_PP_IS_SELFISH 1
#
# include MSGHANDLE_PP_INDIRECT_SELF
#
# undef MSGHANDLE_PP_IS_SELFISH
# undef MSGHANDLE_PP_INDIRECT_SELF
