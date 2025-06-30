#pragma once
#include <cstdint>

struct RabbitState {
    uint32_t x[8], c[8], carry;
};

void key_setup(RabbitState& s, const uint8_t key[16]);
void generate_block(RabbitState& s, uint8_t out[16]);