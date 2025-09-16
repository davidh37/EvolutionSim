#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/ext.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <vector>
#include <queue>


#define COLOR_RED vec3(1.0f, 0.0f, 0.0f)
#define COLOR_BLUE vec3(0.0f, 0.0f, 1.0f)
#define COLOR_GREEN vec3(0.0f, 1.0f, 0.0f)
#define COLOR_WHITE vec3(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK vec3(0.0f, 0.0f, 0.0f)
#define COLOR_LIGHTGRAY vec3(0.85f, 0.85f, 0.85f)
#define COLOR_GRAY vec3(0.5f, 0.5f, 0.5f)
#define COLOR_DARKGRAY vec3(0.2f, 0.2f, 0.2f)

typedef uint8_t ubyte;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

using std::cout;
using std::unique_ptr;
using std::endl;
using std::string;
using std::to_string;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

#define PI 3.14159265358979f

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define CLAMP(X, L, H) (MAX(MIN(X, H), L))

static string TERMINAL_CLEAR = "\033[0m";


static inline string f_to_str(float f){
    int len = snprintf(NULL, 0, "%f", f);
    static char buffer[128];
    snprintf(buffer, len+1, "%.2f", f);
    string s = string(buffer);
    return s;
}

static inline string hex_to_str(ubyte *data, size_t size){
    char buffer[3];
    string output = "";
    for(size_t i = 0; i < size; i++){
        snprintf(buffer, 3, "%02x", data[i]);
        output += buffer;
    }
    return output;
}

static inline uint32_t hash(uint32_t index) {
    uint64_t x = index;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    x = x * 0x2545F4914F6CDD1DULL;
    return (uint32_t) (x >> 32);
}

static inline float randf(float min, float max){
    return min + (max - min ) * (rand() / (float) RAND_MAX);
}

static inline vec2 randv(){
    float phi = randf(0.0f, PI * 2.0f);
    return vec2(cos(phi), sin(phi));
}


