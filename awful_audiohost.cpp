#include "awful_audiohost.h"

AW_HOST_ERROR_CODE_T Host_GetDeviceCount(int *count)
{
	AW_HOST_ERROR_CODE_T err_code = AW_HOST_NO_ERROR;

	if (count != NULL)
	{
		*count = 0;

		int numDevices = Pa_GetDeviceCount();

		if (numDevices < 0)
		{
			err_code = AW_HOST_DEVICE_GET_ERROR;
		}
		else
		{
			*count = numDevices;
		}
	}
	else
	{
		err_code = AW_HOST_INVALID_PARAM;
	}

	return err_code;
}

AW_HOST_ERROR_CODE_T Host_GetDeviceInfo(const PaDeviceInfo **ppDevInfo, int index)
{
	AW_HOST_ERROR_CODE_T err_code = AW_HOST_NO_ERROR;

	if ((index < 0) || (NULL == ppDevInfo))
	{
		err_code = AW_HOST_INVALID_PARAM;
	}
	else
	{
		*ppDevInfo = Pa_GetDeviceInfo(index);
	}
	return err_code;
}