#include <iostream>
#include <common.hpp>
#include <ringbuffer.hpp>

using namespace ssp;

int main(){
    RingBuffer<float> *buffer = new RingBuffer<float>(10);
    float in[3] = {0.0,1.0,-1.0};
    float out[3] = {};
    float ref[3] = {1.0, 2.0, 3.0};
    buffer->write(in, 3);
    setEpochMark(buffer->at(0));
    if (!isEpochMark((*buffer)[0])){
        std::cout << "error" << std::endl;
    }
    unsetEpochMark(buffer->at(0));
    if (isEpochMark((*buffer)[0])){
        std::cout << "error" << std::endl;
    }
    if(!buffer->filled(3)){
        std::cout << "error" << std::endl;
    }

    if ((*buffer)[0] != in[0]){
        std::cout << "error" << std::endl;
    }
    buffer->movePos(1);
    if ((*buffer)[0] != in[1]){
        std::cout << "error" << std::endl;
    }
    if(buffer->filledFromCurrentPos(3)){
        std::cout << "error" << std::endl;
    }
    buffer->reset();
    buffer->write(in, 3);
    buffer->read(out, 3);
    for (int i=0;i<3;i++){
        if (in[i] != out[i]){
            std::cout << "error" << std::endl;
        }
    }
    if (buffer->filled(1)){
        std::cout << "error" << std::endl;
    }
    buffer->reset();
    for (int i=0;i<3;i++){
        (*buffer)[i] = ref[i];
    }
    buffer->moveWritePos(3);
    if (!buffer->filled(3)){
        std::cout << "error" << std::endl;
    }
    buffer->read(out, 3);
    for (int i=0;i<3;i++){
        if (ref[i] != out[i]){
            std::cout << "error" << std::endl;
        }
    }
    buffer->reset();
    for (int i=0;i<3;i++){
        (*buffer)[i] = ref[i];
        if ((*buffer)[i] != ref[i]){
            std::cout << "error" << std::endl;
        }
    }
    buffer->reset();
    buffer->write(in, 3);
    buffer->write(ref, 3);
    buffer->movePos(3);
    for (int i=0;i<3;i++){
        if ((*buffer)[i] != ref[i]){
            std::cout << "error" << std::endl;
        }
    }
    delete buffer;
}