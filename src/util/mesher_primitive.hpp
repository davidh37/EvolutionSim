#pragma once
#include "engine/common.hpp"
#include "engine/mesh.hpp"

namespace mesher_primitive{
    void quad(Mesh::VertexBuffer &vertices, Mesh::IndexBuffer &indices, vec3 position, vec2 scale, vec2 uv_pos, vec2 uv_scale, vec3 color);
    void circle(Mesh::VertexBuffer &vertices, Mesh::IndexBuffer &indices, vec3 position, vec2 scale, vec3 color, int segments);
}
