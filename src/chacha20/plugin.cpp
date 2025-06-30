#include "chacha20/chacha20.hpp"
#include "ICipher.hpp"

extern "C" ICipher* create_cipher() {
    return new ChaCha20();
}

extern "C" void destroy_cipher(ICipher* cipher) {
    delete cipher;
}