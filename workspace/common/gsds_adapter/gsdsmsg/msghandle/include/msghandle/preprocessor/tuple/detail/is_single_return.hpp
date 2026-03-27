# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Edward Diener 2014.                                    *
#  *     Distributed under the Boost Software License, Version 1.0. (See      *
#  *     accompanying file LICENSE_1_0.txt or copy at                         *
#  *     http://www.boost.org/LICENSE_1_0.txt)                                *
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef MSGHANDLE_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP
# define MSGHANDLE_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
#
# /* MSGHANDLE_PP_TUPLE_IS_SINGLE_RETURN */
#
# if MSGHANDLE_PP_VARIADICS && MSGHANDLE_PP_VARIADICS_MSVC
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/facilities/is_1.hpp>
# include <msghandle/preprocessor/tuple/size.hpp>
# define MSGHANDLE_PP_TUPLE_IS_SINGLE_RETURN(sr,nsr,tuple)	\
	MSGHANDLE_PP_IIF(MSGHANDLE_PP_IS_1(MSGHANDLE_PP_TUPLE_SIZE(tuple)),sr,nsr) \
	/**/
# endif /* MSGHANDLE_PP_VARIADICS && MSGHANDLE_PP_VARIADICS_MSVC */
#
# endif /* MSGHANDLE_PREPROCESSOR_TUPLE_DETAIL_IS_SINGLE_RETURN_HPP */
