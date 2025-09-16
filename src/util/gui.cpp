#include "util/gui.hpp"

#include "util/mesher_primitive.hpp"
#include "engine/common.hpp"
#include "engine/mesh.hpp"
#include "engine/texture.hpp"
#include "engine/shader.hpp"


namespace gui{

    static Shader shader;
    static Texture fontTexture;
    static Mesh mesh;

    static constexpr int CHARS_TOTAL = 256;
    static const int CHARS_FIRST = 32;
    static const int CHARS_CELLS_ROW = 16;
    static const int CHAR_CELL_WIDTH = 32;
    static vec4 char_uvs[CHARS_TOTAL];

    static void generate_char_uvs(){

        int pitch;
        uint8_t *pixels = (uint8_t*) fontTexture.get_pixels(pitch);


        for(int c = 0; c < CHARS_TOTAL; c++){
            int cx = (c % CHARS_CELLS_ROW) * CHAR_CELL_WIDTH;
            int cy = (c / CHARS_CELLS_ROW) * CHAR_CELL_WIDTH;

            int offset_x1 = -1;
            int offset_x2 = -1;
          
            for(int px = 0; px < CHAR_CELL_WIDTH; px++){
                for(int py = 0; py < CHAR_CELL_WIDTH; py++){
                    int x = cx + px;
                    int y = cy + py;

                    if(pixels[(y * pitch + x * 4 + 0)] > 0){
                        if(offset_x1 == -1){
                            offset_x1 = px;
                        }
                        offset_x2 = px;
                        break;
                    }
                }
            }

            float pixel_to_uv = 1.0f / ((float) CHARS_CELLS_ROW * CHAR_CELL_WIDTH);

            if(offset_x1 == -1 || offset_x2 == -1){
                offset_x1 = 0;
                offset_x2 = 2;
            }

            char_uvs[c][0] = pixel_to_uv * (cx + offset_x1 - 1);
            char_uvs[c][1] = pixel_to_uv * (cy);
            char_uvs[c][2] = pixel_to_uv * (offset_x2 - offset_x1 + 3);
            char_uvs[c][3] = pixel_to_uv * CHAR_CELL_WIDTH;
        }
    }

    void initialize(){
        shader.compile("resources/texture.vert", "resources/texture.frag");
        fontTexture.create("resources/calibri32clean.bmp", false);
        fontTexture.convert_black_to_alpha();
        fontTexture.upload(true);
        generate_char_uvs();
        // texture unit 0
        shader.setUniformInteger(1, 0);
        cout << TERMINAL_COLOR << "[gui] intitialized" << TERMINAL_CLEAR << endl;
    }

    void cleanup(){
        shader.destroy();
        fontTexture.destroy();
        mesh.destroy();
        cout << TERMINAL_COLOR << "[gui] cleanup" << TERMINAL_CLEAR << endl;
    }

    void drawBox(float x, float y, float w, float h, vec3 color){
        mesher_primitive::quad(mesh.vertex_buffer, mesh.index_buffer, vec3(x, y, 0.0f), vec2(w, h), vec2(0.999f, 0.999f), vec2(0.0f, 0.0f), color);
    }

    void drawChar(int character, float x, float y, float w, float h, vec3 color){
        assert(character >= CHARS_FIRST && character < CHARS_FIRST + CHARS_TOTAL);
        character = character - CHARS_FIRST;
        vec4 uvs = char_uvs[character];
        mesher_primitive::quad(mesh.vertex_buffer, mesh.index_buffer, vec3(x, y, 0.0f), vec2(w, h), vec2(uvs[0], uvs[1]), vec2(uvs[2], uvs[3]), color);
    }

    void drawString(string text, float x, float y, float w, float h, vec3 color){
        int n = text.size();
        assert(n > 0);

        //calculate total width needed
        float text_width = 0.0f;
        for(int i = 0; i < n; i++){
            int c = text[i] - CHARS_FIRST;
            text_width += char_uvs[c][2];
        }

        float uv_scale = w / text_width;

        float accumulator = x;
        for(int i = 0; i < n; i++){
            int c = text[i] - CHARS_FIRST;
            vec4 uv = char_uvs[c];
            float scaled_w = uv[2] * uv_scale;
            drawChar(c + CHARS_FIRST, accumulator, y, scaled_w, h, color);
            accumulator += scaled_w;
        }
    }

    void drawStringUnscaled(string text, float x, float y, float scale, vec3 color){
        int n = text.size();

        float accumulator = x;
        for(int i = 0; i < n; i++){
            int c = text[i] - CHARS_FIRST;
            vec4 uv = char_uvs[c];
            float w = scale * uv[2] / uv[3];
            float h = scale;
            drawChar(c + CHARS_FIRST, accumulator, y, w, h, color);
            accumulator += w;
        }
    }

    void render(){
        if(mesh.vertex_buffer.size() > 0){
            mat4 to_ndc = {
                2.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 2.0f, 0.0f,
                -1.0f, -1.0f, -1.0f, 1.0f
            };
            shader.load();
            fontTexture.load();
            shader.setUniformMat4(0, to_ndc);
            mesh.upload(Mesh::STREAM, false);
            mesh.draw();
        }
    }
}