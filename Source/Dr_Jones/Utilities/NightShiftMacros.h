#pragma once

#ifndef NS_EARLY
	#define NS_EARLY(exp) \
	{\
		if (exp)\
		{\
			return;\
		}\
	}
#endif

#ifndef NS_EARLY_R
	#define NS_EARLY_R(exp, retval) \
	{\
		if (exp)\
		{\
			return retval;\
		}\
	}
#endif
