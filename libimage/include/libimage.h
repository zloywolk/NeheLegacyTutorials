#ifndef _LIBIMAGE_H_
#define _LIBIMAGE_H_

/************************************************************************/
/* Exports/imports defines                                              */
/************************************************************************/

#ifndef LIBIMAGE_STATIC
	#ifdef _LIBIMAGE_DLL
		#define LIBIMAGE_API __declspec(dllexport)
	#else
		#define LIBIMAGE_API __declspec(dllimport)
	#endif // _LIBIMAGE_DLL  
#else
	#define LIBIMAGE_API
#endif // !LIBIMAGE_STATIC

#ifdef _MSC_VER
	#pragma warning(disable: 4251)
#endif // _MSC_VER

#include <windows.h>

typedef unsigned int uint_t;
typedef short int short_t;
typedef unsigned short int ushort_t;
typedef char byte_t;
typedef unsigned char ubyte_t;

#define IMAGE_16_BIT					16
#define IMAGE_24_BIT					24
#define IMAGE_32_BIT					32

typedef int err_t;

#ifndef SUCCESS
#define SUCCESS							0x0000
#endif
#define ERR_FILE_NAME_EMPTY				0x1000
#define ERR_FILE_OPEN					0x1001
#define ERR_INVALID_IMAGE_FORMAT		0x1002
	
#define ERR_INVALID_TGA_HEADER			0x2000
#define ERR_UNSUPPORTED_TGA_DATA_TYPE	0x2001
#define ERR_INVALID_TGA_DATA_TYPE		0x2002

namespace image
{
	namespace err 
	{
		extern LIBIMAGE_API const char *image_error(err_t err_code);
	}
}

#endif // !_LIBIMAGE_H_
