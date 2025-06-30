#include "generate_key.hpp"
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>

// Простейший хэш-функция на основе XOR и сдвигов (НЕ криптостойкая)
uint32_t simple_hash(const std::string& input, uint32_t seed = 0xABCDEF01) {
    uint32_t hash = seed;
    for (char c : input) {
        hash ^= static_cast<uint8_t>(c);
        hash = (hash << 5) | (hash >> 27); // ROTL 5
        hash += 0x9E3779B9; // псевдорандомная прибавка (золотое число)
    }
    return hash;
}

std::vector<uint8_t> generate_key(const std::string& password, size_t key_size) {
    if (key_size == 0) throw std::invalid_argument("key_size must be > 0");

    std::vector<uint8_t> key;
    std::string current = password;
    uint32_t counter = 0;

    while (key.size() < key_size) {
        uint32_t h = simple_hash(current + std::to_string(counter));
        // Преобразуем 32-битный хэш в 4 байта
        for (int i = 0; i < 4 && key.size() < key_size; ++i) {
            key.push_back((h >> (i * 8)) & 0xFF);
        }
        ++counter;
    }

    return key;
}