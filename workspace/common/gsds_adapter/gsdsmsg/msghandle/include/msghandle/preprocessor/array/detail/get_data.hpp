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
# ifndef MSGHANDLE_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP
# define MSGHANDLE_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP
#
# include <msghandle/preprocessor/config/config.hpp>
# include <msghandle/preprocessor/tuple/rem.hpp>
# include <msghandle/preprocessor/control/if.hpp>
# include <msghandle/preprocessor/control/iif.hpp>
# include <msghandle/preprocessor/facilities/is_1.hpp>
#
# /* MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA */
#
# define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_NONE(size, data)

# if MSGHANDLE_PP_VARIADICS && !(MSGHANDLE_PP_VARIADICS_MSVC && _MSC_VER <= 1400)
# 	 if MSGHANDLE_PP_VARIADICS_MSVC
# 		define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_DEFAULT(size, data) MSGHANDLE_PP_TUPLE_REM(size) data
# 		define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_CAT(size, data) MSGHANDLE_PP_TUPLE_REM_CAT(size) data
# 		define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) \
			MSGHANDLE_PP_IIF \
				( \
				MSGHANDLE_PP_IS_1(size), \
				MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_CAT, \
				MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY_VC_DEFAULT \
				) \
			(size,data) \
/**/
#    else
# 		define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) MSGHANDLE_PP_TUPLE_REM(size) data
#    endif
# else
# 	 define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY(size, data) MSGHANDLE_PP_TUPLE_REM(size) data
# endif

# define MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA(size, data) \
	MSGHANDLE_PP_IF \
		( \
		size, \
		MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_ANY, \
		MSGHANDLE_PP_ARRAY_DETAIL_GET_DATA_NONE \
		) \
	(size,data) \
/**/
#
# endif /* MSGHANDLE_PREPROCESSOR_ARRAY_DETAIL_GET_DATA_HPP */
