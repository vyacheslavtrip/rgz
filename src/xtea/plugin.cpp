#include "xtea.hpp"

extern "C" ICipher* create_cipher() {
    return new XTEA();
}

extern "C" void destroy_cipher(ICipher* cipher) {
    delete cipher;
}