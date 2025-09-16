#include "util/mesher_primitive.hpp"

#include "engine/mesh.hpp"
#include "engine/common.hpp"


namespace mesher_primitive{

    void quad(Mesh::VertexBuffer &vertices, Mesh::IndexBuffer &indices, vec3 position, vec2 scale, vec2 uv_pos, vec2 uv_scale, vec3 color){
        uint16_t last = vertices.size();

        float x1 = position.x;
        float x2 = position.x + scale.x;
        float y1 = position.y;
        float y2 = position.y + scale.y;

        Mesh::Vertex v[4];
        v[0] = {vec3(x1, y2, position.z), color, vec2(uv_pos.x, uv_pos.y)};
        v[1] = {vec3(x1, y1, position.z), color, vec2(uv_pos.x, uv_pos.y+ uv_scale.y)};
        v[2] = {vec3(x2, y1, position.z), color, vec2(uv_pos.x + uv_scale.x, uv_pos.y+ uv_scale.y)};
        v[3] = {vec3(x2, y2, position.z), color, vec2(uv_pos.x + uv_scale.x, uv_pos.y)};

        vertices.push_back(v[0]);
        vertices.push_back(v[1]);
        vertices.push_back(v[2]);
        vertices.push_back(v[3]);

        indices.push_back(last + 0);
        indices.push_back(last + 1);
        indices.push_back(last + 2);
        indices.push_back(last + 0);
        indices.push_back(last + 2);
        indices.push_back(last + 3);
    }

    void circle(Mesh::VertexBuffer &vertices, Mesh::IndexBuffer &indices, vec3 position, vec2 scale, vec3 color, int segments){
        uint16_t last = vertices.size();
        vertices.reserve(vertices.size() + segments + 1);
        indices.reserve(indices.size() + segments * 3);

        Mesh::Vertex origin = {position, color, vec2(0.5f, 0.5f)};
        vertices.push_back(origin);
        
        float increment = 2.0f * PI / segments;
        float angle = 0.0f;

        for(int i = 0; i < segments; i++){
            angle += increment;
            float cosine = cos(angle);
            float sine = sin(angle);
            vec3 pos;
            pos.x = position.x + scale.x * cosine;
            pos.y = position.y + scale.y * sine;
            pos.z = position.z;
            vec2 uv = vec2(0.5f * cosine + 0.5f, 0.5f * sine + 0.5f);
            Mesh::Vertex v = {pos, color, uv};
            vertices.push_back(v);
            indices.push_back(last + 0);
            indices.push_back(last + i+1);
            indices.push_back(last + i+2);
        }
        indices[indices.size()-1] = last + 1;
    }
}