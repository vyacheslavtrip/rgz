#include "rabbit.hpp"
#include "ICipher.hpp"

extern "C" ICipher* create_cipher() {
    return new Rabbit();
}
extern "C" void destroy_cipher(ICipher* c) {
    delete c;
}