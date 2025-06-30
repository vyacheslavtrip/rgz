// utils.hpp
#pragma once
#include <vector>
#include <cstdint>

namespace xtea_utils {
    void pad(std::vector<uint8_t>& data, size_t blockSize);
    void unpad(std::vector<uint8_t>& data);
}