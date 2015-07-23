#ifndef _SAFE_MEM_H
#define _SAFE_MEM_H

#include "libimage.h"

namespace mem
{
	LIBIMAGE_API inline void safe_delete(void *object)
	{
		if (object != nullptr)
			delete object;

		object = nullptr;
	}

	LIBIMAGE_API inline void safe_delete_arr(void *object)
	{
		if (object != nullptr)
			delete [] object;

		object = nullptr;
	}
}

#endif // !_SAFE_MEM_H
