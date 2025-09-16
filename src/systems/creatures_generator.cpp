#include "systems/creatures_generator.hpp"
#include "ecs.hpp"
#include "config.hpp"
#include "engine/mesh.hpp"


namespace creatures_generator {

    using namespace ecs;

    static float generateTrait(const ubyte *dna, uint32_t seed);
    static void generateMesh(CID cid, int brain);
    static void generateCreature(CID cid);
    static void initializeEyeMesh();

    static bool mesh_brain_continuous = false;
    
    void initialize(){
        initializeEyeMesh();
    }

    void cleanup(){
        
    }

    void update(){
        for(size_t i = 0; i < creature_data.vector.size(); i++){
            if(creature_data.vector[i].state == CreatureData::FETUS){
                generateCreature(i);
                creature_data.vector[i].state = CreatureData::READY;
            }

            if(creature_data.vector[i].to_mesh){
                if(creature_data.vector[i].highlighted){
                    if(mesh_brain_continuous){
                        generateMesh(i, 2);
                    }else{
                        generateMesh(i, 1);
                    }
                }else{
                    generateMesh(i, 0);
                    creature_data.vector[i].to_mesh = false;
                }
            }
        }
    }

    void setBrainMeshing(bool continuous){
        mesh_brain_continuous = continuous;
    }

    static inline int allele_at(const ubyte* dna, uint32 bit_index) {
        const uint32 byte_index = bit_index / 8;
        const uint32 bit = bit_index % 8;
        return (dna[byte_index] & (1 << bit));
    }

    inline static float tanh_approx(float x) {
        // tanh approximation from https://stackoverflow.com/questions/3446170/fast-tanh-approximation-algorithm-for-c-c
        if (x < -3) return -1;
        else if (x > 3) return 1;
        else return x * (27 + x * x) / (27 + 9 * x * x);
	}

    inline static float generateTrait(const ubyte *dna, uint32_t seed){

        // returns int from 0 to 31
        
        float trait = 0.0f;
        const int k = 10;
        float trait_power = 1.5f;

        for(int i = 0; i < k; i++){
            const int l = 12;
            int bit_values[l];
            for (int j = 0; j < l; j++) {
                seed = hash(seed);
				bit_values[j] = allele_at(dna, seed % (config::CREATURE_DNA_SIZE * 8));
            }

            float gene = (trait_power / k); // trait direction
			gene *= (bit_values[0] & ~bit_values[1]) ? -1.0f : 1.0f; // gene influence flip
            gene *= (bit_values[2] & ~bit_values[3]) ? 3.0f : 1.0f; // gene influence 3x
            gene *= (bit_values[4] & ~bit_values[5]) ? 0.33f : 1.0f; // gene influence 0.33x 
            gene *= (bit_values[6] ^ bit_values[7]) ? 2.0f : 1.0f; // gene influence 2x
            gene *= (bit_values[8] ^ bit_values[9]) ? 0.5f : 1.0f; // gene influence 0.5x 
            gene *= (bit_values[2] & ~bit_values[9] & bit_values[10] & ~bit_values[11]) ? 16.0f : 1.0f; // rare strongy :D
            trait += gene;
        }
        return tanh_approx(trait) * 0.5f + 0.5f;
    }

    static void generateCreature(CID cid){

        uint32_t count = config::CREATURE_GENERATOR_SEED + 5;

        CreatureData &creature = creature_data.vector[cid];

        creature.brain_input_rate = config::BRAIN_INPUTRATE_MIN; 
        creature.brain_input_rate += (config::BRAIN_INPUTRATE_MAX - config::BRAIN_LEAKRATE_MIN) * generateTrait(creature.dna, count++);
        creature.brain_leak_rate = config::BRAIN_LEAKRATE_MIN; 
        creature.brain_leak_rate += (config::BRAIN_LEAKRATE_MAX - config::BRAIN_LEAKRATE_MIN) * generateTrait(creature.dna, count++);
        creature.carnivore = generateTrait(creature.dna, count++);
        creature.size = 0.5f + 0.5f * generateTrait(creature.dna, count++);
        creature.sex = generateTrait(creature.dna, count++);
        creature.metabolic_rate = generateTrait(creature.dna, count++);
        creature.mutation_rate = generateTrait(creature.dna, count++);
        creature.color.r = generateTrait(creature.dna, count++);
        creature.color.g = generateTrait(creature.dna, count++);
        creature.color.b = generateTrait(creature.dna, count++);
        

        int n = 0;
        for(int i = 0; i < config::CREATURE_MAX_APPENDAGES; i++){
            float trait1 = generateTrait(creature.dna, count++);
            float trait2 = generateTrait(creature.dna, count++);
            float trait3 = generateTrait(creature.dna, count++);
            float trait4 = generateTrait(creature.dna, count++);

            creature.appendages[i].strength = trait4 * trait4;
            n++;
            if(trait1 < 0.5f){
                n--;
                creature.appendages[i].type = Appendage::NONE;
            }else if(trait1 < 0.7f){
                creature.appendages[i].type = Appendage::SKIN;
            }else {
                // organelle
                if(trait2 < 0.3f){
                    creature.appendages[i].type = Appendage::EYE;
                }else if(trait2 < 0.7f){
                    if(trait3 < 0.25f){
                        creature.appendages[i].type = Appendage::TURNER_RIGHT;
                    }else if(trait3 < 0.75f){    
                        creature.appendages[i].type = Appendage::JET;
                    }else{
                        creature.appendages[i].type = Appendage::TURNER_LEFT;
                    }
                }else{
                    creature.appendages[i].type = Appendage::SPIKE;
                }
            }
        }
        int k = 0;
        
        for(int i = 0; i < config::CREATURE_MAX_APPENDAGES; i++){
            Appendage &app = creature.appendages[i];
            if(app.type != Appendage::NONE){
                app.angle = PI * 2.0f * (k + 0.5f) / (float)n;
                int x = 1 + k * 3;
                int y = 1;
                if(x >= config::BRAIN_SIZE - 1){
                    x -= (config::BRAIN_SIZE / 3) * 3;
                    y = config::BRAIN_SIZE - 2;
                }
                app.neuron_x = x + config::BRAIN_SYNAPSE_RADIUS;
                app.neuron_y = y + config::BRAIN_SYNAPSE_RADIUS;
                if(app.type == Appendage::EYE){
                    creature.neurons[creature.appendages[i].neuron_y][creature.appendages[i].neuron_x].type = Neuron::INPUT;
					creature.neurons[creature.appendages[i].neuron_y][creature.appendages[i].neuron_x+1].type = Neuron::INPUT;
					creature.neurons[creature.appendages[i].neuron_y+1][creature.appendages[i].neuron_x - 1].type = Neuron::INPUT;
                }else if(app.type == Appendage::JET || app.type == Appendage::TURNER_LEFT || app.type == Appendage::TURNER_RIGHT){
                    creature.neurons[creature.appendages[i].neuron_y][creature.appendages[i].neuron_x].type = Neuron::OUTPUT;
                }
                k++;
            }
        }

		creature.neurons[15][10].type = Neuron::INPUT;
		creature.neurons[15][12].type = Neuron::INPUT;
		creature.neurons[15][14].type = Neuron::INPUT;
		creature.neurons[15][16].type = Neuron::INPUT;
		creature.neurons[15][18].type = Neuron::INPUT;
		creature.neurons[17][10].type = Neuron::INPUT;
		creature.neurons[17][12].type = Neuron::INPUT;
		creature.neurons[17][14].type = Neuron::INPUT;

        if(n < 3){
            // fix edge case
            creature.appendages[0].type = Appendage::NONE;
            creature.appendages[0].angle = 0 * PI * 2.0f / 3;
            creature.appendages[1].type = Appendage::NONE;
            creature.appendages[1].angle = 1 * PI * 2.0f / 3;
            creature.appendages[2].type = Appendage::NONE;
            creature.appendages[2].angle = 2 * PI * 2.0f / 3;
            n = 3;
        }
        creature.appendage_count = n;

        constexpr float SYN_MIN = config::BRAIN_SYNAPSE_MIN;
        constexpr float SYN_MAX = config::BRAIN_SYNAPSE_MAX;
        constexpr float INPUT_MIN = config::BRAIN_NEURON_INPUTRATE_MIN;
        constexpr float INPUT_MAX = config::BRAIN_NEURON_INPUTRATE_MAX;
        constexpr float LEAK_MIN = config::BRAIN_NEURON_LEAKRATE_MIN;
        constexpr float LEAK_MAX = config::BRAIN_NEURON_LEAKRATE_MAX;

        for(int ny = config::BRAIN_SYNAPSE_RADIUS; ny < config::BRAIN_SYNAPSE_RADIUS + config::BRAIN_SIZE; ny++){
            for(int nx = config::BRAIN_SYNAPSE_RADIUS; nx < config::BRAIN_SYNAPSE_RADIUS + config::BRAIN_SIZE; nx++){
                creature.neurons[ny][nx].input_rate = (INPUT_MAX - INPUT_MIN) * generateTrait(creature.dna, count++) + INPUT_MIN;
                creature.neurons[ny][nx].leak_rate = (LEAK_MAX - LEAK_MIN) * generateTrait(creature.dna, count++) + LEAK_MIN;
                for(int sy = 0; sy < config::BRAIN_SYNAPSE_WIDTH; sy++){
                    for (int sx = 0; sx < config::BRAIN_SYNAPSE_WIDTH; sx++){
                        float trait = generateTrait(creature.dna, count++);
                        creature.neurons[ny][nx].synapses[sy][sx] = (SYN_MAX - SYN_MIN) * (trait * trait * trait) + SYN_MIN;
                    }
                }
            }
        }
    }

    static constexpr Mesh::Vertex spike[] = {
        {vec3(-1.0f, 0.0f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(1.0f, 0.0f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.0f, 0.9f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.85f, 0.0f, 1.0f), COLOR_WHITE, vec2(0.0f)},
        {vec3(0.85f, 0.0f, 1.0f), COLOR_WHITE, vec2(0.0f)},
        {vec3(0.0f, 0.75f, 1.0f), COLOR_WHITE, vec2(0.0f)}
    };

    static constexpr Mesh::Vertex jet[] = {
        {vec3(-1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(-0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)},

        {vec3(-0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.7f, 1.2f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.7f, 1.2f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.7f, 1.2f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.6f, 0.8f, 1.0f), COLOR_BLACK, vec2(0.0f)}
    };

    static Mesh::Vertex eye[config::RENDER_EYE_SEGMENTS * 3 * 2];
    
    static void initializeEyeMesh(){
        // initialize eye mesh
        int segments = config::RENDER_EYE_SEGMENTS;
        vec2 scale = {0.7f, 0.7f};
        vec3 offset = {0.0f, -0.95f, 1.0f};
        vec3 color = COLOR_WHITE;

        float increment = 2.0f * PI / segments;
        float angle = 0.0f;

        for(int i = 0; i < segments; i++){
            angle += increment;
            eye[i*3+0] = {vec3(scale.x * cos(angle) + offset.x, scale.y * sin(angle) + offset.y, offset.z), color, vec2(0.0f)};
            eye[i*3+1] = {vec3(scale.x * cos(angle+increment) + offset.x, scale.y * sin(angle+increment) + offset.y, offset.z), color, vec2(0.0f)};
            eye[i*3+2] = {vec3(offset.x, offset.y, offset.z), color, vec2(0.0f)};
        }
        int k = segments * 3;

        color = COLOR_BLACK;
        offset = {0.0f, -0.7, 1.0f};
        scale = {0.35f, 0.35f};

        for(int i = 0; i < segments; i++){
            angle += increment;
            eye[k+i*3+0] = {vec3(scale.x * cos(angle) + offset.x, scale.y * sin(angle) + offset.y, offset.z), color, vec2(0.0f)};
            eye[k+i*3+1] = {vec3(scale.x * cos(angle+increment) + offset.x, scale.y * sin(angle+increment) + offset.y, offset.z), color, vec2(0.0f)};
            eye[k+i*3+2] = {vec3(offset.x, offset.y, offset.z), color, vec2(0.0f)};
        }
    }

    static Mesh::Vertex turner_left[] = {
        {vec3(-1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(0.75f, 0.6f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.75f, 0.6f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(0.8f, 1.2f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
    };

    static Mesh::Vertex turner_right[] = {
        {vec3(1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(-1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
        {vec3(-0.75f, 0.6f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.75f, 0.6f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(-0.8f, 1.2f, 1.0f), COLOR_BLACK, vec2(0.0f)},
        {vec3(1.0f, 0.0f, 1.0f), COLOR_GRAY, vec2(0.0f)},
    };

    static void generateAppendageMesh(Mesh::VertexBuffer &vb, Mesh::IndexBuffer &ib, Appendage::Type t, int i, int n, float size, vec3 color){
        int vertex_count;
        const Mesh::Vertex *part_vertices = nullptr;
        switch(t){
            case Appendage::SKIN:
                part_vertices = nullptr;
                vertex_count = 0;
                break;
            case Appendage::JET:
                part_vertices = jet;
                vertex_count = ARRAY_LEN(jet);
                break;
            case Appendage::TURNER_LEFT:
                part_vertices = turner_left;
                vertex_count = ARRAY_LEN(turner_left);
                break;
            case Appendage::TURNER_RIGHT:
                part_vertices = turner_right;
                vertex_count = ARRAY_LEN(turner_right);
                break;
            case Appendage::SPIKE:
                part_vertices = spike;
                vertex_count = ARRAY_LEN(spike);
                break;
            case Appendage::EYE:
                part_vertices = eye;
                vertex_count = ARRAY_LEN(eye);
                break;
            default:
                assert(false);
        }
            
        // regular appendage
        float sin_val = sin(PI / n);
        float cos_val = cos(PI / n);
        size = sqrt(MAX(size, 0.05f)); // bias upwards
        float y_scale = size;
        float x_scale = size;
        float angle_offset = 2.0f * PI * (-0.25f + (i + 0.5f) / n);

        mat3 scale = glm::scale(vec3(x_scale * sin_val, y_scale * sin_val, 1.0f));
        mat3 rotate = glm::rotate(mat3(1.0f), angle_offset);
        mat3 translate = glm::translate(mat3(1.0f), vec2(0.0f, cos_val));
        mat3 trans = rotate * translate * scale;

        for(int i = 0; i < vertex_count; i++){
            Mesh::Vertex current = part_vertices[i];
            current.position = trans * current.position;
            // reduce jittering
            //current.position.x = std::round(current.position.x * 1000.0) / 1000.0;
            //current.position.y = std::round(current.position.y * 1000.0) / 1000.0;
            current.position.z = 0.0f;
            if(t != Appendage::EYE){
                current.color.r = 0.5f * (current.color.r + color.r);
                current.color.g = 0.5f * (current.color.g + color.g);
                current.color.b = 0.5f * (current.color.b + color.b);
            }
            vb.push_back(current);
            ib.push_back(vb.size()-1);
        }
    }

    void generateBrainMesh(Mesh::VertexBuffer &vb, Mesh::IndexBuffer &ib, CID cid, bool continuous){


        float scale = 1.0f;
        float offset = -0.5f;
        int lastSize = (int)vb.size();

        float delta = 1.0f / (float) config::BRAIN_SIZE;
        int counter = 0;

        for(int y = 0; y < config::BRAIN_SIZE; y++){
            for(int x = 0; x < config::BRAIN_SIZE; x++){
                int nx = x + config::BRAIN_SYNAPSE_RADIUS;
                int ny = y + config::BRAIN_SYNAPSE_RADIUS;
                Neuron &neuron = creature_data.vector[cid].neurons[ny][nx];

                float npot = 0.2f + 0.8f * neuron.potential / config::BRAIN_ACTION_THRESHOLD;
                npot = std::min(npot, 1.0f);
                npot = std::max(npot, 0.0f);

                if(!continuous && npot < 1.0f){
                    npot = 0.0f;
                }

                vec3 color = vec3(npot, npot, npot);
                switch(neuron.type){
                    case Neuron::NORMAL:
                        color = vec3(npot, npot, npot);
                        break;
                    case Neuron::OUTPUT:
                        color = vec3(1.0f, npot, npot);
                        break;
                    case Neuron::INPUT:
                        color = vec3(npot, npot, 1.0f);
                        break;
                }
                
                float x1 = offset + ((float)x * delta) * scale;
                float x2 = offset + (float)(x+1) * delta * scale;
                float y1 = offset + 1.0f - (float) y * delta * scale;
                float y2 = offset + 1.0f - (float) (y - 1) * delta * scale;

                Mesh::Vertex v1 = {vec3(x1, y2, 0.0f), color, vec2()};
                Mesh::Vertex v2 = {vec3(x1, y1, 0.0f), color, vec2()};
                Mesh::Vertex v3 = {vec3(x2, y1, 0.0f), color, vec2()};
                Mesh::Vertex v4 = {vec3(x2, y2, 0.0f), color, vec2()};

                vb.push_back(v1);
                vb.push_back(v2);
                vb.push_back(v3);
                vb.push_back(v4);

                ib.push_back(lastSize + 4 * counter + 0);
                ib.push_back(lastSize + 4 * counter + 1);
                ib.push_back(lastSize + 4 * counter + 2);

                ib.push_back(lastSize + 4 * counter + 0);
                ib.push_back(lastSize + 4 * counter + 2);
                ib.push_back(lastSize + 4 * counter + 3);

                counter++;
            }
        }
    }

    static void generateMesh(CID cid, int mesh_brain){
        CreatureData &creature = creature_data.vector[cid];
        assert(creature.appendage_count > 0);

        Mesh::VertexBuffer &vertices = ecs::meshes[creature.mesh_id].vertex_buffer;
        Mesh::IndexBuffer &indices = ecs::meshes[creature.mesh_id].index_buffer;
        if(vertices.size() > 0){
            // some fetus was killed during generation!
            vertices.clear();
            indices.clear();
        }

        // generate body
        vertices.reserve(creature.appendage_count + 1);
        indices.reserve(creature.appendage_count * 3);

        vec3 brighten_color = vec3(std::min(1.0f, creature.color.r * 1.3f), std::min(1.0f, creature.color.g * 1.3f), std::min(1.0f, creature.color.b * 1.3f));
        Mesh::Vertex origin = {vec3(0.0f), brighten_color, vec2(0.5f, 0.5f)};
        vertices.push_back(origin);

        float increment = 2.0f * PI / creature.appendage_count;
        float angle = 0.0f;

        for(int i = 0; i < creature.appendage_count; i++){
            angle += increment;
            Mesh::Vertex v = {vec3(cos(angle), sin(angle), 0.0f), creature.color, vec2()};
            vertices.push_back(v);
            indices.push_back(0);
            indices.push_back(i+1);
            indices.push_back(i+2);
        }
        indices[indices.size()-1] = 1;

        // generate appendages
        int count = 0;
        for(int i = 0; i < config::CREATURE_MAX_APPENDAGES; i++){
            if(creature.appendages[i].type == Appendage::NONE){
                continue;
            }
            Appendage &app = creature.appendages[i];
            generateAppendageMesh(vertices, indices, app.type, count, creature.appendage_count, app.strength, creature.color);
            count++;
        }

        // generate brain
        if(mesh_brain > 0){
            generateBrainMesh(vertices, indices, cid, mesh_brain == 2);
        }
    }
}