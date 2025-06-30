#include "rabbit/utils.hpp"
#include <cstdint>

static inline uint32_t rotl(uint32_t v, int s) {
    return (v << s) | (v >> (32 - s));
}

static inline uint32_t g_func(uint32_t y) {
    uint64_t z = uint64_t(y) * y;
    return uint32_t(z ^ (z >> 32));
}

static void next_state(RabbitState& s) {
    static const uint32_t A[8] = {
        0x4D34D34D,0xD34D34D3,0x34D34D34,0x4D34D34D,
        0xD34D34D3,0x34D34D34,0x4D34D34D,0xD34D34D3
    };
    for (int i = 0; i < 8; ++i) {
        uint64_t sum = uint64_t(s.c[i]) + A[i] + s.carry;
        s.carry = (sum >> 32) & 1;
        s.c[i] = uint32_t(sum);
    }
    uint32_t g[8];
    for (int i = 0; i < 8; ++i) {
        g[i] = g_func(s.x[i] + s.c[i]);
    }
    s.x[0] = g[0] + rotl(g[7],16) + rotl(g[6],16);
    s.x[1] = g[1] + rotl(g[0],8)  +        g[7];
    s.x[2] = g[2] + rotl(g[1],16) + rotl(g[0],16);
    s.x[3] = g[3] + rotl(g[2],8)  +        g[1];
    s.x[4] = g[4] + rotl(g[3],16) + rotl(g[2],16);
    s.x[5] = g[5] + rotl(g[4],8)  +        g[3];
    s.x[6] = g[6] + rotl(g[5],16) + rotl(g[4],16);
    s.x[7] = g[7] + rotl(g[6],8)  +        g[5];
}

void key_setup(RabbitState& s, const uint8_t key[16]) {
    uint16_t k[8];
    for (int i = 0; i < 8; ++i) {
        k[i] = uint16_t(key[2*i]) | (uint16_t(key[2*i+1]) << 8);
    }
    for (int i = 0; i < 8; ++i) {
        if ((i & 1) == 0) {
            s.x[i] = (uint32_t(k[(i+1)&7]) << 16) | k[i];
            s.c[i] = (uint32_t(k[(i+4)&7]) << 16) | k[(i+5)&7];
        } else {
            s.x[i] = (uint32_t(k[(i+5)&7]) << 16) | k[(i+4)&7];
            s.c[i] = (uint32_t(k[i]) << 16)       | k[(i+1)&7];
        }
    }
    s.carry = 0;
    for (int i = 0; i < 4; ++i) next_state(s);
    for (int i = 0; i < 8; ++i) s.c[i] ^= s.x[(i+4)&7];
}

void generate_block(RabbitState& s, uint8_t out[16]) {
    next_state(s);
    uint32_t s0 = s.x[0] ^ (s.x[5] >> 16) ^ (s.x[3] << 16);
    uint32_t s1 = s.x[2] ^ (s.x[7] >> 16) ^ (s.x[5] << 16);
    uint32_t s2 = s.x[4] ^ (s.x[1] >> 16) ^ (s.x[7] << 16);
    uint32_t s3 = s.x[6] ^ (s.x[3] >> 16) ^ (s.x[1] << 16);
    uint32_t S[4] = {s0, s1, s2, s3};
    for (int i = 0; i < 4; ++i) {
        out[4*i  ] =  S[i]        & 0xFF;
        out[4*i+1] = (S[i] >>  8) & 0xFF;
        out[4*i+2] = (S[i] >> 16) & 0xFF;
        out[4*i+3] = (S[i] >> 24) & 0xFF;
    }
}