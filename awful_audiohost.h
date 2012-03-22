#ifndef _AWFUL_AUDIOHOST_
#define _AWFUL_AUDIOHOST_

#include "awful.h"

typedef enum
{
	AW_HOST_NO_ERROR = 0,
	AW_HOST_INVALID_PARAM,
	AW_HOST_INTERNAL_ERROR,
	AW_HOST_DEVICE_GET_ERROR
} AW_HOST_ERROR_CODE_T;


extern AW_HOST_ERROR_CODE_T Host_GetDeviceCount(int *count);
extern AW_HOST_ERROR_CODE_T Host_GetDeviceInfo(const PaDeviceInfo **ppDevInfo, int index);

#endif /* _AWFUL_AUDIOHOST_ */