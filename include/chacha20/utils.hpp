#pragma once

#include <vector>
#include <cstdint>

namespace chacha20_utils {

void chacha20_block(uint32_t output[16], const uint32_t input[16]);
void xor_data(std::vector<uint8_t>& data, const std::vector<uint8_t>& key_stream);

}