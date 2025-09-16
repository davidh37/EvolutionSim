#include "common.hpp"
#include "mesh.hpp"
#include "shader.hpp"


void Mesh::upload(Hint hint, bool instanced){
    assert(vertex_buffer.size() > 0);

    if(vao == 0){
        //no buffers set up
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);

        if(instanced){
            glGenBuffers(1, &ivbo);
            glBindBuffer(GL_ARRAY_BUFFER, ivbo);
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (2 * sizeof(float)));
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
            glEnableVertexAttribArray(3);
            glEnableVertexAttribArray(4);
            glEnableVertexAttribArray(5);
            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1); 
            glVertexAttribDivisor(5, 1); 
        }

        glBindVertexArray(0);
    }

    GLenum usage = GL_STATIC_DRAW;
    switch(hint){
        case STATIC:
            usage = GL_STATIC_DRAW;
            break;
        case DYNAMIC:
            usage = GL_DYNAMIC_DRAW;
            break;
        case STREAM:
            usage = GL_STREAM_DRAW;
            break;
    }

    ebo_size = index_buffer.size();
    vbo_size = vertex_buffer.size();
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if(vertex_buffer.size() > vbo_capacity){
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_buffer.size(), vertex_buffer.data(), usage);
        vbo_capacity = vertex_buffer.size();
    }else{
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vertex_buffer.size(), vertex_buffer.data());
    }
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    if(index_buffer.size() > ebo_capacity){
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * index_buffer.size(), index_buffer.data(), usage);
        ebo_capacity = index_buffer.size();
    }else{
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint16_t) * index_buffer.size(), index_buffer.data());
    }
    
    glBindVertexArray(0);
    
    vertex_buffer.clear();
    index_buffer.clear();
}

#include "engine.hpp"

void Mesh::draw(){
    assert(ivbo == 0);
    assert(vao != 0);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glDrawElements(GL_TRIANGLES, ebo_size, GL_UNSIGNED_SHORT, NULL);
    glBindVertexArray(0);
    
}

void Mesh::drawLines(){
    assert(vao != 0);
    assert(ivbo == 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glDrawArrays(GL_LINES, 0, vbo_size);
    glBindVertexArray(0);
}

void Mesh::drawInstances(std::vector<float> &instance_data, Hint hint){
    assert(ivbo != 0);

    GLenum usage = GL_STATIC_DRAW;
    switch(hint){
        case STATIC:
            usage = GL_STATIC_DRAW;
            break;
        case DYNAMIC:
            usage = GL_DYNAMIC_DRAW;
            break;
        case STREAM:
            usage = GL_STREAM_DRAW;
            break;
    }

    ivbo_size = instance_data.size();

    glBindBuffer(GL_ARRAY_BUFFER, ivbo);
    if(instance_data.size() > ivbo_capacity){
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instance_data.size(), instance_data.data(), usage);
        ivbo_capacity = instance_data.size();
    }else{
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * instance_data.size(), instance_data.data());
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    assert(vao != 0);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glDrawElementsInstanced(GL_TRIANGLES, ebo_size, GL_UNSIGNED_SHORT, NULL, ivbo_size / 6);
    glBindVertexArray(0);
}

void Mesh::destroy(){
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ivbo);
    glDeleteVertexArrays(1, &vao);
    vao = 0;
    vbo = 0;
    ivbo = 0;
    ebo = 0;
    ebo_capacity = 0;
    vbo_capacity = 0;
    ivbo_capacity = 0;
    vbo_size = 0;
    ivbo_size = 0;
    ebo_size = 0;
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}
