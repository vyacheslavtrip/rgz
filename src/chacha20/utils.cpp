#include "chacha20/utils.hpp"
#include <cstring>

namespace chacha20_utils {

// Quarter round operation
inline void quarter_round(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
    a += b; d ^= a; d = (d << 16) | (d >> 16);
    c += d; b ^= c; b = (b << 12) | (b >> 20);
    a += b; d ^= a; d = (d << 8) | (d >> 24);
    c += d; b ^= c; b = (b << 7) | (b >> 25);
}

// The ChaCha20 block function, input = 16 words (512 bits)
void chacha20_block(uint32_t output[16], const uint32_t input[16]) {
    memcpy(output, input, 64);
    for (int i = 0; i < 10; ++i) {  // 20 rounds (10 double rounds)
        // Odd round
        quarter_round(output[0], output[4], output[8], output[12]);
        quarter_round(output[1], output[5], output[9], output[13]);
        quarter_round(output[2], output[6], output[10], output[14]);
        quarter_round(output[3], output[7], output[11], output[15]);
        // Even round
        quarter_round(output[0], output[5], output[10], output[15]);
        quarter_round(output[1], output[6], output[11], output[12]);
        quarter_round(output[2], output[7], output[8], output[13]);
        quarter_round(output[3], output[4], output[9], output[14]);
    }
    for (int i = 0; i < 16; ++i) {
        output[i] += input[i];
    }
}

// XOR data buffer with key stream buffer (of same length)
void xor_data(std::vector<uint8_t>& data, const std::vector<uint8_t>& key_stream) {
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] ^= key_stream[i];
    }
}

}  // namespace chacha20_utils