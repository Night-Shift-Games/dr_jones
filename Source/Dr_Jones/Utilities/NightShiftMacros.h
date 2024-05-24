#pragma once

#ifndef NS_EARLY
	#define B_EARLY(exp) \
	{\
		if (exp)\
		{\
			return;\
		}\
	}
#endif

#ifndef NS_EARLY_R
	#define B_EARLY_R(exp, retval) \
	{\
		if (exp)\
		{\
			return retval;\
		}\
	}
#endif
