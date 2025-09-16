#include "creatures_thinking.hpp"
#include "ecs.hpp"

namespace creatures_thinking {

    using namespace ecs;

    void initialize(){

    }

    void cleanup(){

    }

    void update(){
		const int brain_size = config::BRAIN_SIZE;
		const int synapse_radius = config::BRAIN_SYNAPSE_RADIUS;
		const int synapse_width = config::BRAIN_SYNAPSE_WIDTH;
		const float action_potential = config::BRAIN_ACTION_THRESHOLD;
		const float delta = config::SIM_DELTA;

        for(CID cid = 0; cid < creature_data.vector.size(); cid++){
            CreatureData &creature = creature_data.vector[cid];
			const float creature_leak_rate = creature.brain_leak_rate;
			const float creature_input_rate = creature.brain_input_rate;

            creature.number_neurons_firing = 0;
            if(creature.state & CreatureData::ALIVE){
                // fire synapses
                for(int ny = 0; ny < brain_size; ny++){
                    for(int nx = 0; nx < brain_size; nx++){
                        Neuron &neuron = creature.neurons[ny + synapse_radius][nx + synapse_radius];
                        if(neuron.potential >= action_potential){
                            creature.number_neurons_firing++;
                            for(int sy = 0; sy < synapse_width; sy++){
                                for(int sx = 0; sx < synapse_width; sx++){
                                    // synapse radius cancels out
                                    creature.neurons[ny + sy][nx + sx].input += neuron.synapses[sy][sx];
                                }
                            }
                            // refractory period
                            neuron.potential = -action_potential;
                        }
                    }
                }

                // recalculate neuron potential
                for(int ny = 0; ny < brain_size; ny++){
                    for(int nx = 0; nx < brain_size; nx++){
                        Neuron &neuron = creature.neurons[ny + synapse_radius][nx + synapse_radius];
                        neuron.potential += delta * (-neuron.potential * neuron.leak_rate * creature_leak_rate);
                        neuron.potential += neuron.input * neuron.input_rate * creature_input_rate;
                        neuron.input = 0.0f;
                    }
                }
            }
        }
    }
    
}