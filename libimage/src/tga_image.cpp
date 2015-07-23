#include "tga_image.h"

err_t image::tga_image::load()
{
	return get_raw_data();
}

image::tga_image::tga_image()
	:m_pContent(nullptr), m_width(0u), m_height(0u), m_size(0u), m_bpp(0u)
{
	memset(static_cast<void *>(&this->m_tga_header), 0, sizeof(tga_header_t));
}

err_t image::tga_image::get_raw_data()
{
	FILE *pf;
	if (m_file_name.empty())
		return ERR_FILE_NAME_EMPTY; 

	if (!(pf = fopen(m_file_name.c_str(), "rb")))
		return ERR_FILE_OPEN;

	/* Read header */
	size_t read_bytes = fread(&m_tga_header, 1, sizeof(tga_header_t), pf);
	if (read_bytes != sizeof(tga_header_t))
	{
		fclose(pf);
		return ERR_INVALID_TGA_HEADER;
	}


	if (m_tga_header.data_type_code != TGA_DATA_TYPE_UNCOMPRESSED_RGB)
	{
		fclose(pf);
		return ERR_UNSUPPORTED_TGA_DATA_TYPE;
	}

	if (m_tga_header.bits_per_pixel != IMAGE_16_BIT &&
		m_tga_header.bits_per_pixel != IMAGE_24_BIT &&
		m_tga_header.bits_per_pixel != IMAGE_32_BIT)
	{
		fclose(pf);
		return ERR_INVALID_IMAGE_FORMAT;
	}

	m_bpp = m_tga_header.bits_per_pixel;

	if (static_cast<uint_t>(m_tga_header.id_length > 0u))
	{
		char *buff = new char[m_tga_header.id_length + 1];
		memset(buff, 0, sizeof(char) *m_tga_header.id_length + 1);

		read_bytes = fread(buff, 1, m_tga_header.id_length, pf);
		mem::safe_delete_arr(buff);
	}

	m_width = m_tga_header.width;
	m_height = m_tga_header.height;

	/* Read raw content of rgb */
	if (m_tga_header.data_type_code == TGA_DATA_TYPE_UNCOMPRESSED_RGB)
	{
		m_size = m_width * m_height * m_tga_header.bits_per_pixel / 8;
		m_pContent = new ubyte_t[m_size];
		memset(m_pContent, 0, m_size * sizeof(ubyte_t));

		read_bytes = fread(m_pContent, 1, m_size, pf);

		ubyte_t *m_RgbContent = new ubyte_t[m_size];
		memset(m_RgbContent, 0, m_size * sizeof(ubyte_t));

		for (size_t i = 0; i < m_size; i += m_bpp / 8)
		{
			int j = m_bpp / 8 - 1;
			while (j >= 0)
				m_RgbContent[i + (m_bpp / 8 - 1) - j] = m_pContent[i + j--];
		}

		mem::safe_delete_arr(m_pContent);
		m_pContent = m_RgbContent;
	}

	fclose(pf);
	return SUCCESS;
}

image::tga_image::~tga_image()
{
	mem::safe_delete_arr(m_pContent);
}

image::tga_image::tga_image(const std::string& str_path)
	:tga_image()
{
	m_file_name = str_path;
}
