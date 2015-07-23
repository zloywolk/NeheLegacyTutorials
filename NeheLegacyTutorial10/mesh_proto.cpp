#include "mesh_proto.h"

int load_world()
{
	FILE *pf;
	if (!(pf = fopen(MESH_FILE, "rt")))
		return -1;

	memset(static_cast<void *>(&sector), 0, sizeof(sector_t));
	memset(static_cast<void *>(&vertex), 0, sizeof(vertex_t));

	char buff[BUFSIZ];
	read_line(pf, buff);
	sscanf(buff, "NUMPOLLIES %d\n", &sector.n_triangles);

	sector.triangle = new triangle_t[sector.n_triangles];
	memset(static_cast<void *>(sector.triangle), 0, sector.n_triangles * sizeof(triangle_t));

	for (size_t i = 0; i < (size_t)sector.n_triangles; i++)
	{
		for (size_t j = 0; j < TRINGLE_VERT_COUNT; j++)
		{
			read_line(pf, buff);
			sscanf(buff, "%f %f %f %f %f", &vertex.x, &vertex.y, &vertex.z, &vertex.u, &vertex.v);
			memcpy(&sector.triangle[i].vertex[j], &vertex, sizeof(vertex_t));
		}
	}

	fclose(pf);
	return 0;
}

void destroy_world()
{
	mem::safe_delete_arr(static_cast<void *>(sector.triangle));
}

void read_line(FILE *pf, char *dst_buffer)
{
	assert(pf);
	assert(dst_buffer);

	do
	{
		fgets(dst_buffer, BUFSIZ, pf);
	} while (*dst_buffer == '/' || *dst_buffer == '\n');
}

sector_t *get_world()
{
	return &sector;
}
