#pragma once
#include <cstdint>
#include <vector>
#include <string>

std::vector<uint8_t> generate_key(const std::string& password, size_t key_size);