#include "util/debuglines.hpp"

#include "engine/shader.hpp"
#include "engine/mesh.hpp"


namespace debuglines {

    static Shader shader;
    static Mesh lineMesh;

    void initialize(){
        shader.compile("resources/color.vert", "resources/color.frag");
        cout << TERMINAL_COLOR << "[debuglines] intitialized" << TERMINAL_CLEAR << endl;
    }

    void cleanup(){
        shader.destroy();
        lineMesh.destroy();
        cout << TERMINAL_COLOR << "[debuglines] cleanup" << TERMINAL_CLEAR << endl;
    }

    void addPoint(vec2 p, vec3 color){
        Mesh::Vertex v = {vec3(p.x, p.y, 0.0f), color, vec2()};
        lineMesh.vertex_buffer.push_back(v);
    }


    void render(mat4 transformation, float line_width){
        if(lineMesh.vertex_buffer.size() < 2){
            return;
        }
        glLineWidth(line_width);
        shader.load();
        shader.setUniformMat4(0, transformation);
        Mesh::IndexBuffer empty;
        lineMesh.upload(Mesh::STREAM, false);
        lineMesh.drawLines();
    }
}