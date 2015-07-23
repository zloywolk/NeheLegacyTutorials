#ifndef _MESH_PROTO_H
#define _MESH_PROTO_H

#include <cassert>
#include <memory.h>
#include <stdio.h>

#include "safe_mem.h"

#define TRINGLE_VERT_COUNT	3u
#define MESH_FILE			"level/World.txt"

struct vertex_t
{
	float x, y, z;	// vertex position
	float u, v;		// UV texture coordinates
};

struct triangle_t
{
	vertex_t vertex[TRINGLE_VERT_COUNT];
};

struct sector_t
{
	int n_triangles;
	triangle_t *triangle;
};

static vertex_t vertex;
static sector_t sector;

extern int load_world();
extern void read_line(FILE *pf, char *dst_buffer);
extern void destroy_world();
extern sector_t *get_world();

#endif // !_MESH_PROTO_H
