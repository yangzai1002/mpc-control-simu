# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#ifndef MSGHANDLE_PREPROCESSOR_REMOVE_PARENS_HPP
#define MSGHANDLE_PREPROCESSOR_REMOVE_PARENS_HPP

#include <msghandle/preprocessor/config/config.hpp>

#if MSGHANDLE_PP_VARIADICS

#include <msghandle/preprocessor/control/iif.hpp>
#include <msghandle/preprocessor/facilities/identity.hpp>
#include <msghandle/preprocessor/punctuation/is_begin_parens.hpp>
#include <msghandle/preprocessor/tuple/enum.hpp>

#define MSGHANDLE_PP_REMOVE_PARENS(param) \
    MSGHANDLE_PP_IIF \
      ( \
      MSGHANDLE_PP_IS_BEGIN_PARENS(param), \
      MSGHANDLE_PP_REMOVE_PARENS_DO, \
      MSGHANDLE_PP_IDENTITY \
      ) \
    (param)() \
/**/

#define MSGHANDLE_PP_REMOVE_PARENS_DO(param) \
  MSGHANDLE_PP_IDENTITY(MSGHANDLE_PP_TUPLE_ENUM(param)) \
/**/

#endif /* MSGHANDLE_PP_VARIADICS */
#endif /* MSGHANDLE_PREPROCESSOR_REMOVE_PARENS_HPP */
