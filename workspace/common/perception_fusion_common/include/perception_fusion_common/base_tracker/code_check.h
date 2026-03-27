#pragma once

namespace avos {
namespace perception {


// #define ENABLE_CODE_CHECK


#ifdef ENABLE_CODE_CHECK
	#define CHECK_RANGE(variable, min_value, max_value) \
		do { \
			if ((variable) < (min_value) || (variable) > (max_value)) { \
				AINFO << "CHECK_RANGE " << #variable << ": " << (variable); \
			} \
		} while (false)
#else
	#define CHECK_RANGE(variable, min_value, max_value)
#endif



#ifdef ENABLE_CODE_CHECK
	#define CHECK_EXPR0(expression) \
		do { \
			if (!(expression)) { \
				AINFO << "CHECK_EXPR " << #expression; \
			} \
		} while (false)
#else
	#define CHECK_EXPR0(expression)
#endif



#ifdef ENABLE_CODE_CHECK
	#define CHECK_EXPR1(expression, var1) \
		do { \
			if (!(expression)) { \
				AINFO << "CHECK_EXPR " << #expression << " " << (var1); \
			} \
		} while (false)
#else
	#define CHECK_EXPR1(expression, var1)
#endif


#ifdef ENABLE_CODE_CHECK
	#define CHECK_EXPR2(expression, var1, var2) \
		do { \
			if (!(expression)) { \
				AINFO << "CHECK_EXPR " << #expression << " " << (var1) << " " << (var2); \
			} \
		} while (false)
#else
	#define CHECK_EXPR2(expression, var1, var2)
#endif



// inline bool check_angle_normalize() {

// }




}
}

