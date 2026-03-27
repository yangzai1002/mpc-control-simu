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
# ifndef MSGHANDLE_PREPROCESSOR_SLOT_DETAIL_DEF_HPP
# define MSGHANDLE_PREPROCESSOR_SLOT_DETAIL_DEF_HPP
#
# /* MSGHANDLE_PP_SLOT_OFFSET_x */
#
# define MSGHANDLE_PP_SLOT_OFFSET_10(x) (x) % 1000000000UL
# define MSGHANDLE_PP_SLOT_OFFSET_9(x) MSGHANDLE_PP_SLOT_OFFSET_10(x) % 100000000UL
# define MSGHANDLE_PP_SLOT_OFFSET_8(x) MSGHANDLE_PP_SLOT_OFFSET_9(x) % 10000000UL
# define MSGHANDLE_PP_SLOT_OFFSET_7(x) MSGHANDLE_PP_SLOT_OFFSET_8(x) % 1000000UL
# define MSGHANDLE_PP_SLOT_OFFSET_6(x) MSGHANDLE_PP_SLOT_OFFSET_7(x) % 100000UL
# define MSGHANDLE_PP_SLOT_OFFSET_5(x) MSGHANDLE_PP_SLOT_OFFSET_6(x) % 10000UL
# define MSGHANDLE_PP_SLOT_OFFSET_4(x) MSGHANDLE_PP_SLOT_OFFSET_5(x) % 1000UL
# define MSGHANDLE_PP_SLOT_OFFSET_3(x) MSGHANDLE_PP_SLOT_OFFSET_4(x) % 100UL
# define MSGHANDLE_PP_SLOT_OFFSET_2(x) MSGHANDLE_PP_SLOT_OFFSET_3(x) % 10UL
#
# /* MSGHANDLE_PP_SLOT_CC_x */
#
# define MSGHANDLE_PP_SLOT_CC_2(a, b) MSGHANDLE_PP_SLOT_CC_2_D(a, b)
# define MSGHANDLE_PP_SLOT_CC_3(a, b, c) MSGHANDLE_PP_SLOT_CC_3_D(a, b, c)
# define MSGHANDLE_PP_SLOT_CC_4(a, b, c, d) MSGHANDLE_PP_SLOT_CC_4_D(a, b, c, d)
# define MSGHANDLE_PP_SLOT_CC_5(a, b, c, d, e) MSGHANDLE_PP_SLOT_CC_5_D(a, b, c, d, e)
# define MSGHANDLE_PP_SLOT_CC_6(a, b, c, d, e, f) MSGHANDLE_PP_SLOT_CC_6_D(a, b, c, d, e, f)
# define MSGHANDLE_PP_SLOT_CC_7(a, b, c, d, e, f, g) MSGHANDLE_PP_SLOT_CC_7_D(a, b, c, d, e, f, g)
# define MSGHANDLE_PP_SLOT_CC_8(a, b, c, d, e, f, g, h) MSGHANDLE_PP_SLOT_CC_8_D(a, b, c, d, e, f, g, h)
# define MSGHANDLE_PP_SLOT_CC_9(a, b, c, d, e, f, g, h, i) MSGHANDLE_PP_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i)
# define MSGHANDLE_PP_SLOT_CC_10(a, b, c, d, e, f, g, h, i, j) MSGHANDLE_PP_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j)
#
# define MSGHANDLE_PP_SLOT_CC_2_D(a, b) a ## b
# define MSGHANDLE_PP_SLOT_CC_3_D(a, b, c) a ## b ## c
# define MSGHANDLE_PP_SLOT_CC_4_D(a, b, c, d) a ## b ## c ## d
# define MSGHANDLE_PP_SLOT_CC_5_D(a, b, c, d, e) a ## b ## c ## d ## e
# define MSGHANDLE_PP_SLOT_CC_6_D(a, b, c, d, e, f) a ## b ## c ## d ## e ## f
# define MSGHANDLE_PP_SLOT_CC_7_D(a, b, c, d, e, f, g) a ## b ## c ## d ## e ## f ## g
# define MSGHANDLE_PP_SLOT_CC_8_D(a, b, c, d, e, f, g, h) a ## b ## c ## d ## e ## f ## g ## h
# define MSGHANDLE_PP_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i) a ## b ## c ## d ## e ## f ## g ## h ## i
# define MSGHANDLE_PP_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j) a ## b ## c ## d ## e ## f ## g ## h ## i ## j
#
# endif
