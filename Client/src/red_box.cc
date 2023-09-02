#include "red_box.h"

namespace PMG {
	RedBox::RedBox() {
		mesh = new Mesh();
        color_shader_vertex_t* vert = new color_shader_vertex_t[8]{
            color_shader_vertex_t{{-0.5f, 2.0f, -0.5f}, {1.0f, 0, 0, 1}},
            color_shader_vertex_t{{0.5f, 2.0f, 0.5f}, {1.0f, 0, 0, 1}},
            color_shader_vertex_t{{0.5f, 2.0f, -0.5f}, {1.0f, 0, 0, 1}},
            color_shader_vertex_t{{-0.5f, 2.0f, 0.5f}, {1.0f, 0, 0, 1}},
            color_shader_vertex_t{{-0.5f, 0.2f, -0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{0.5f, 0.2f, 0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{0.5f, 0.2f, -0.5f}, {0, 0, 0.0f, 1}},
            color_shader_vertex_t{{-0.5f, 0.2f, 0.5f}, {0, 0, 0.0f, 1}},
        };

        unsigned int* indices = new unsigned int[12] {0, 1, 2, 1, 0, 3, 4, 5, 6, 5, 4, 7};
        mesh->vertices = vert;
        mesh->vertexCount = 8;
        mesh->indices = indices;
        mesh->indexCount = 12;

        health = 50;
        max_health = 100;
        position = { 0, 0, 0 };
        rotation = { 0, 0, 0 };
	}
}