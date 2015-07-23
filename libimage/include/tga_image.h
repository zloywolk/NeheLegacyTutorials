#ifndef _TGA_FORMAT_H
#define _TGA_FORMAT_H

#include "libimage.h"

#include "safe_mem.h"

#include <string>
#include <cstdio>

/************************************************************************
	All Targa formats are identified by a Data Type field, which is 
	a one byte binary integer located in byte three of the file. 
	These data types are:

	0	-  No image data included.
	1	-  Uncompressed, color-mapped images.
	2	-  Uncompressed, RGB images.
	3	-  Uncompressed, black and white images.
	9	-  Runlength encoded color-mapped images.
	10  -  Runlength encoded RGB images.
	11  -  Compressed, black and white images.
	32  -  Compressed color-mapped data, using Huffman, Delta, and
		   runlength encoding.
	33  -  Compressed color-mapped data, using Huffman, Delta, and
		   runlength encoding.  4-pass quadtree-type process.
/************************************************************************/

/* No image */
#define TGA_DATA_TYPE_NO_IMAGE						0
/* Uncompressed images */
#define TGA_DATA_TYPE_UNCOMPRESSED_COLOR_MAPPED		1
#define TGA_DATA_TYPE_UNCOMPRESSED_RGB				2
#define TGA_DATA_TYPE_UNCOMPRESSED_BLACK_WHITE		3
/* Runlength encoded */
#define TGA_DATA_TYPE_RUNLENGTH_ENC_COLOR_MAPPED	9
#define TGA_DATA_TYPE_RUNLENGTH_ENC_RGB				10
/* Compressed encoded */
#define TGA_DATA_TYPE_COMPRESSED_ENC_BLACK_WHITE	11
#define TGA_DATA_TYPE_COMPRESSED_ENC_COLOR_MAPPED	32
#define TGA_DATA_TYPE_COMPRESSED_ENC_RGB			33


namespace image
{
	class LIBIMAGE_API tga_image
	{
	private:
#pragma pack(push, 1)
		struct tga_header_t
		{
			byte_t  id_length;
			byte_t  color_map_type;
			byte_t  data_type_code;
			short_t color_map_origin;
			short_t color_map_length;
			byte_t  color_map_depth;
			short_t x_origin;
			short_t y_origin;
			short_t width;
			short_t height;
			byte_t  bits_per_pixel;
			byte_t  image_descriptor;
		} m_tga_header;
#pragma pack(pop)

		uint_t m_width;
		uint_t m_height;
		uint_t m_size;
		uint_t m_bpp;
		ubyte_t	*m_pContent;

		std::string m_file_name;
		tga_image();
		err_t get_raw_data();
	public:
		uint_t width() const { return m_width; }
		uint_t height() const { return m_height; }
		uint_t size() const { return m_size; }
		uint_t bpp() const { return m_bpp; }
		ubyte_t *content() const { return m_pContent; }
		std::string file_name() const { return m_file_name; }
		void file_name(std::string val) { m_file_name = val; }
	public:
		err_t load();
		tga_image(const std::string& str_path);
		~tga_image();
	};
}

#endif // !_TGA_FORMAT_H
