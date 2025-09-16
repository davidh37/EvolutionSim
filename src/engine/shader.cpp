#include "glad/glad/glad.h"
#include <fstream>
#include "common.hpp"
#include "shader.hpp"

static bool readShaderFile(string& buffer, string const& location){
    std::ifstream myfile(location, std::ios::in);
    if(!myfile.is_open()) {
        return false;
    }

    std::string line = "";
    while(!myfile.eof()){
        std::getline(myfile, line);
        buffer.append(line + "\n");
    }
    myfile.close();
    return true;
}

void Shader::compile(string const& vert_location, string const& frag_location){
    string vert_shader;
    string frag_shader;
    if(!readShaderFile(vert_shader, vert_location)){
        cout << "Failed to read shader file " + vert_location + "\n";
        abort();
    }
    if(!readShaderFile(frag_shader, frag_location)){
        cout << "Failed to read shader file " + frag_location + "\n";
        abort();
    }
    const char* vert_shader_cstr = vert_shader.c_str();
    const char* frag_shader_cstr = frag_shader.c_str();

    GLint status;
    char err_buf[512];

    //compile vertex shader
    vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vert_shader_cstr, NULL);
    glCompileShader(vert_id);
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(vert_id, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        cout << "Vertex shader compilation failed: " + string(err_buf) + "\n";
        abort();
    }

    // Compile fragment shader
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &frag_shader_cstr, NULL);
    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        glGetShaderInfoLog(frag_id, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        cout << "Fragment shader compilation failed: " + string(err_buf) + "\n";
        abort();
    }

    // Link vertex and fragment shaders
    id = glCreateProgram();
    glAttachShader(id, vert_id);
    glAttachShader(id, frag_id);
    glBindFragDataLocation(id, 0, "out_Color");
    glLinkProgram(id);

    GLint isLinked = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &isLinked);
    if(isLinked == GL_FALSE){
        glGetProgramInfoLog(id, sizeof(err_buf), NULL, err_buf);
        err_buf[sizeof(err_buf)-1] = '\0';
        cout << "Shader program linking failed: " + string(err_buf) + "\n";
        abort();
    }

    glUseProgram(id);
}

void Shader::load(){
    glUseProgram(id);
}

void Shader::setUniformInteger(int location, int val){
    glUniform1i(location, val);
}

void Shader::setUniformMat4(int location, mat4 val){
    glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
}

void Shader::destroy(){
    glUseProgram(0);
    glDetachShader(id, vert_id);
    glDetachShader(id, frag_id);
    glDeleteProgram(id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    id = 0;
    vert_id = 0;
    frag_id = 0;
}
