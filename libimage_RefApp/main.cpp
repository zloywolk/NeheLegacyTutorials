#include <iostream>

#include "libimage.h"
#include "tga_format.h"

int main(int argc, char *argv[])
{
	image::tga_image tga_image("textures/tex01.tga");
	err_t err = tga_image.load();

	return (0);
}