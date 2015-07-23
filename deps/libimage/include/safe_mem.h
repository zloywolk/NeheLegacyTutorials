#ifndef _SAFE_MEM_H
#define _SAFE_MEM_H

#include "libimage.h"

namespace mem
{
	template <typename T>
	inline LIBIMAGE_API void safe_delete(T *object)
	{
		if (object != nullptr)
			delete object;

		object = nullptr;
	}

	template <typename T>
	inline LIBIMAGE_API void safe_arr_delete(T object[])
	{
		if (object != nullptr)
			delete [] object;

		object = nullptr;
	}
}

#endif // !_SAFE_MEM_H
